/*
 This file is part of EASAL. 

 EASAL is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 EASAL is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 *  Created on: 2016-2017
 *      Author: Chkhaidze Giorgio
 */
#include "MainWindow.h"
#include <thread>

MainWindow::MainWindow()
{
    setWindowTitle("EASAL");
    setFocusPolicy(Qt::StrongFocus);
    this->resize(1200,800);
//[ INIT WIDGETS]
    bondViewWidget = new BondViewWidget();
    realizationViewWidgetCornerVersion = new RealizationViewWidgetCornerVersion(&sharedData, bondViewWidget);
    sweepViewWidget = new SweepViewWidget(&sharedData);
    cayleySpaceViewWidget = new CayleySpaceViewWidget(&sharedData);
    inputWindowInstance = new InputWindow;
    atlasViewWidget = new AtlasViewWidget(&sharedData);
    nodeParentChildernViewWidget = new NodeParentChildernViewWidget(&sharedData);
    stackedWidget = new QStackedWidget();
    tabWidget = new QTabWidget();
    realizationViewWidget = new RealisationViewWidget(&sharedData);
    pathfindingViewWidget = new PathfindingViewWidget3DPart(&sharedData);
    constrainSelectionWindow = new ConstraintSelectionWindow();
    setLayouts();
//[ INIT WIDGETS]
//[ HANDEL TIMER]
    updateTimeInMS = 100;
    timer = new QTimer(this);
    // start argument determins update rate
    timer->start(updateTimeInMS);
    connect(timer,SIGNAL(timeout()),this,SLOT(updateSlot()));
//[~HANDEL TIMER]

//[ INIT PATHFINDING DATA]
    for(int i=0;i<2;i++)
    {
        currentPathfindingSelectedAtlasNodeID[i] = -1;
        currentPathfindingSelectedCayleyPointType[i] = NoType;
        currentPathfindingSelectedCayleyPointID[i] = -1;
    }
//[~INIT PATHFINDING DATA]
//[ MISC]
    stopStartSampleButton.setText("Pause Sampling ");
    runModeSelectionComboBox.addItem("TreeSample");
    runModeSelectionComboBox.addItem("ForestSample");
    runModeSelectionComboBox.addItem("ForestSampleAS");
    runModeSelectionComboBox.addItem("BreadthFirst");
    runModeSelectionComboBox.addItem("RefineSampling");
    runModeSelectionComboBox.addItem("Constraint selection");
    runModeSelectionComboBox.setCurrentText("ForestSampleAS");

    findPathBetweenD0NodesButton.setText("Find path");

    pathfindingButtonText[START] = "Select start Cayley point";
    pathfindingButtonText[END] = "Select end Cayley point";

    pathfindingButton[START].setText(pathfindingButtonText[START]);
    pathfindingButton[END].setText(pathfindingButtonText[END]);

    pathfindingValueLabel[START].setText("Start D0 Node:");
    pathfindingValueLabel[END].setText("End D0 Node:");

    pathfindingFlipLabel[START].setText("Flip:");
    pathfindingFlipLabel[END].setText("Flip:");
//    runModeSelectionComboBox.setVisible(false);
    runModeSelectionComboBox.setDisabled(true);

    connect(&stopStartSampleButton,SIGNAL(clicked(bool)),this,SLOT(stopStartSamplingButtonSlot()));
    connect(&findPathBetweenD0NodesButton,SIGNAL(clicked(bool)),this,SLOT(findPathBetweenD0NodesButtonSlot()));
    connect(&pathfindingButton[START],SIGNAL(clicked(bool)),this,SLOT(pathfindingStartNodeButtonSlot()));
    connect(&pathfindingButton[END],SIGNAL(clicked(bool)),this,SLOT(pathfindingEndNodeButtonSlot()));
    activeWidgetID = 1;


//[~MISC]

}



void MainWindow::openInputWindow()
{
    inputWindowInstance->exec();
    processDataFromSettings();
    sharedData.isSamplingStarted = true;
}

void MainWindow::stopStartSamplingButtonSlot()
{
    if(sharedData.isSamplingStarted)
    {
        isSamplingStopped = !isSamplingStopped;

        if(isSamplingStopped)
        {
            stopStartSampleButton.setText("Resume Sampling");

            //Set current text with name of current sampling mode for dropdown that allow to select sampling mode
            if(Settings::Sampling::runMode == Modes::TreeSample)
            {
                runModeSelectionComboBox.currentText() = "TreeSample";
            }
            else if(Settings::Sampling::runMode == Modes::ForestSample)
            {
                runModeSelectionComboBox.currentText() = "ForestSample";
            }
            else if(Settings::Sampling::runMode == Modes::ForestSampleAS)
            {
                runModeSelectionComboBox.currentText() = "ForestSampleAS";
            }
            else if(Settings::Sampling::runMode == Modes::BreadthFirst)
            {
                runModeSelectionComboBox.currentText() = "BreadthFirst";
            }
            else if(Settings::Sampling::runMode == Modes::RefineSampling)
            {
                runModeSelectionComboBox.currentText() = "RefineSampling";
            }

            runModeSelectionComboBox.setDisabled(false);
            Settings::AtlasBuilding::stop = true;
            Settings::Sampling::runMode = Modes::Stopped;
        }
        else
        {
            ThreadShare::save_loader->saveRoadMap(ThreadShare::atlas_view);
            stopStartSampleButton.setText("Pause Sampling ");
            if(runModeSelectionComboBox.currentText() == "TreeSample")
            {
                Settings::AtlasBuilding::breadthFirst = false;
                Settings::Sampling::runMode = Modes::TreeSample;
            }
            else if(runModeSelectionComboBox.currentText() == "ForestSample")
            {
                Settings::AtlasBuilding::breadthFirst = false;
                Settings::Sampling::runMode = Modes::ForestSample;
            }
            else if(runModeSelectionComboBox.currentText() == "ForestSampleAS")
            {
                Settings::AtlasBuilding::breadthFirst = false;
                Settings::Sampling::runMode = Modes::ForestSampleAS;
            }
            else if(runModeSelectionComboBox.currentText() == "BreadthFirst")
            {
                setSamplingNodeNumber(atlasViewWidget->getCurrentNode()->getID());
                Settings::Sampling::runMode = Modes::BreadthFirst;
            }
            else if(runModeSelectionComboBox.currentText() == "RefineSampling")
            {
                Settings::Sampling::runMode = Modes::RefineSampling;
            }
            else if(runModeSelectionComboBox.currentText() == "Constraint selection")
            {
                constrainSelectionWindow->exec();
               /* ActiveConstraintGraph *grph = constrainSelectionWindow->getContactGraph();
                if(grph != NULL) {
                    grph->setStepSize(Settings::Sampling::stepSize);
                    int num = ThreadShare::atlas_view->findNodeNum(graph);
                    if(num > -1) {
                        delete grph;
                        atlasViewWidget->setCurrentNode(num);
                    } else {
                        ThreadShare::atlas_view->addNode(grph, num);
                        atlasViewWidget->setCurrentNode(num);

                        AtlasNode *rnode_ID - (*ThreadShare::atlas_view)[num];
                        ThreadShare::save_loader->appendDimension(ronde_ID);
                    }
                    setSamplingNodeNumber(atlasViewWidget->getCurrentNode());
                }*/
                Settings::Sampling::runMode = Modes::ForestSample;

            }


            Settings::AtlasBuilding::stop = false;
            runModeSelectionComboBox.setDisabled(true);
        }
    }
}

void MainWindow::findPathBetweenD0NodesButtonSlot()
{
    MessageWindow messageWindow("Error: Wrong Pathfinding Data");

    bool startNodeIDIsInt = true;
    bool endNodeIDIsInt = true;
    int startNodeID = sharedData.pathfindingSelectedAtlasNodeID[START];
    int startFlip = pathfindingFlipSelectionComboBox[0].currentText().toInt();
    int endNodeID = sharedData.pathfindingSelectedAtlasNodeID[END];
    int endFlip =  pathfindingFlipSelectionComboBox[1].currentText().toInt();
    if(!startNodeIDIsInt)
    {
        messageWindow.addLineToMessage(QString("Start node is not integer"));
    }
    else if(startNodeID < 0 || startNodeID >= atlas->number_of_nodes())
    {
        messageWindow.addLineToMessage(QString("Start node is outside allowed range(0 to ") + QString::number(atlas->number_of_nodes()));
    }
    else if((*atlas)[startNodeID]->getDim() != 0)
    {
        messageWindow.addLineToMessage(QString("Start node is not dimension 0 node"));
    }
    if(!endNodeIDIsInt)
    {
        messageWindow.addLineToMessage(QString("End node is not integer"));
    }
    else if(endNodeID < 0 || endNodeID >= atlas->number_of_nodes())
    {
        messageWindow.addLineToMessage(QString("End node is outside allowed range(0 to ") + QString::number(atlas->number_of_nodes()));
    }
    else if((*atlas)[endNodeID]->getDim() != 0)
    {
        messageWindow.addLineToMessage(QString("End node is not dimension 0 node"));
    }

    if(!messageWindow.isEmpty())
    {
        messageWindow.exec();
    }
    else
    {
        vector<PathfindingReturnTuple> path;

        //PathFinder *pf = new PathFinder(atlas,startNodeID, startFlip, endNodeID, endFlip);
        //std::vector<std::pair<CayleyPoint*, int> *>
        //Atlaspath =  pf->find1DoFPath();

        //std::vector<std::pair<CayleyPoint*, int> *>
        //Atlaspath =  atlas->find1DPath(startNodeID, endNodeID);

        std::vector< std::tuple<int, CayleyPoint*, int> >
        Atlaspath =  atlas->find1DoFPath(startNodeID, endNodeID, pathfindingFlipSelectionComboBox[0].currentText().toInt(), pathfindingFlipSelectionComboBox[1].currentText().toInt());

        if(Atlaspath.size() ==0) {
            cout<<"Got an empty path"<<endl;
        } else {
            cout<<"Got a path"<<endl;
            for(int i =0; i<Atlaspath.size(); i++) {
                PathfindingReturnTuple tmpPathReturnTouple;
                Orientation *tmpOrientation = (std::get<1>(Atlaspath[i]))->getOrientation(std::get<2>(Atlaspath[i]));
                tmpPathReturnTouple.atlasNodeID = std::get<0>(Atlaspath[i]);
                tmpPathReturnTouple.orientation = tmpOrientation;
                if(tmpOrientation != NULL){
                 path.push_back(tmpPathReturnTouple);
               }
                //(std::get<0>(*Atlaspath[i]))->printData();
                //cout<<(std::get<1>(*Atlaspath[i]));
                //cout<<endl;
            }
            pathfindingViewWidget->setPath(path);
        }


        //form path manually for testing purposes

        /*
        for(int i=startNodeID;i<=endNodeID;i++)
        {
            if((*atlas)[i]->getDim()==0 ||(*atlas)[i]->getDim()==1 )
            {
                //Get ACR to get orientations from hard drive
                ActiveConstraintRegion *tmpACR = new ActiveConstraintRegion();
                saveLoader->loadNode(i,tmpACR);
                for(int j=0;j<tmpACR->space.size();j++)
                {
                    vector<Orientation*> tmpOrientations= tmpACR->space[j]->getOrientations();
                    for(int k=0;k<tmpOrientations.size();k++)
                    {
                        PathfindingReturnTuple tmpPathReturnTouple;
                        Orientation *tmpOrientation = new Orientation(tmpOrientations[k]);
                        tmpPathReturnTouple.atlasNodeID = i;
                        tmpPathReturnTouple.orientation = tmpOrientation;
                        path.push_back(tmpPathReturnTouple);
                    }
                }
                delete tmpACR;
            }
        }*/

    }


}

void MainWindow::updateSlot()
{
    if(sharedData.isSamplingStarted)
    {
        //Update info about sampling
        if(sharedData.atlasNodeWasSet)
        {
            currentSelectedNodeLabel.setText("Selected Node = " + QString::number(sharedData.currentAtlasNode->getID()));
        }
        else
        {
            currentSelectedNodeLabel.setText("Selected Node = NONE" );
        }

        currentSampledNodeLabel.setText("Sampled Node = " + QString::number(atlas->number_of_nodes()));
        currentRunModeLabel.setText("Run Mode = " + samplingModesNames[Settings::Sampling::runMode]);
        //Update pathfinding related data

        for(int i=0;i<2;i++)
        {
            if(sharedData.pathfindingCayleyPointValueWasChanged[i])
            {
                //Update current values
                currentPathfindingSelectedAtlasNodeID[i] = sharedData.pathfindingSelectedAtlasNodeID[i];
                currentPathfindingSelectedCayleyPointType[i] = sharedData.pathfindingSelectedCayleyPointType[i];
                currentPathfindingSelectedCayleyPointID[i] = sharedData.pathfindingSelectedCayleyPointID[i];
                //Set text for button
                pathfindingButton[i].setText(pathfindingButtonText[i]);
                //Update label with data about selected cayley point
                QString cayleyPointTypeAbreviation = (sharedData.pathfindingSelectedCayleyPointType[i]==WitnessPoint) ? "W" : "S";
                pathfindingValueLabel[i].setText("N: " + QString::number(currentPathfindingSelectedAtlasNodeID[i]) +
                                                 ", " + cayleyPointTypeAbreviation +
                                                 ", C: " +  QString::number(currentPathfindingSelectedCayleyPointID[i]));
                sharedData.pathfindingCayleyPointValueWasChanged[i] = false;
                //Update flip combo box
                pathfindingFlipSelectionComboBox[i].clear();
                for(int j=0;j<sharedData.pathfindingFlipVector[i].size();j++)
                {
                    pathfindingFlipSelectionComboBox[i].addItem(QString::number(sharedData.pathfindingFlipVector[i][j]));
                }
            }
        }
    }
}

void MainWindow::pathfindingStartNodeButtonSlot()
{
    cayleySpaceViewWidget->setAtlasNode(sharedData.currentAtlasNode);
    if(sharedData.pathfindingSelectCayleyPointNow[START] == false)
    {
        sharedData.pathfindingSelectCayleyPointNow[START] = true;
        pathfindingButton[START].setText("Click on valid Cayley point");
    }
    else
    {
        sharedData.pathfindingSelectCayleyPointNow[START] = false;
        pathfindingButton[START].setText(pathfindingButtonText[START]);
    }
}

void MainWindow::pathfindingEndNodeButtonSlot()
{
    cayleySpaceViewWidget->setAtlasNode(sharedData.currentAtlasNode);
    if(sharedData.pathfindingSelectCayleyPointNow[END] == false)
    {
        sharedData.pathfindingSelectCayleyPointNow[END] = true;
        pathfindingButton[END].setText("Click on valid Cayley point");
    }
    else
    {
        sharedData.pathfindingSelectCayleyPointNow[END] = false;
        pathfindingButton[END].setText(pathfindingButtonText[END]);
    }
}

void MainWindow::processDataFromSettings()
{
    PredefinedInteractions *df = new PredefinedInteractions();
    init_DistanceFinder_from_settings(df);
    bool nogui=false;

    // molecular unit A and B
    muA->init_MolecularUnit_A_from_settings(df, nogui);
    muB->init_MolecularUnit_B_from_settings(df, nogui);

    // SaveLoader
    saveLoader->setData(Settings::Output::dataDirectory, muA, muB);

    if (Settings::Constraint::wholeCollision) {
        // reading the neighbour matrix
        ConstraintCheck::nei_matrix = Utils::getMatrixFromFileFromTo("nei.txt");
    } else {
        ConstraintCheck::nei_matrix = Utils::getIdentityMatrix();
    }

    atlas_builder->setData(muA, muB, saveLoader, df, atlas);
    sweepViewWidget->setMolecularUnits(muA,muB);
    sweepViewWidget->setSaveLoader(saveLoader);
    cayleySpaceViewWidget->setData(atlas,saveLoader);
    Settings::Sampling::runMode = ForestSampleAS;

    if (Settings::Sampling::runSample) {
        atlas_builder->setup();
        cout << "AtlasBuilder Set up done." << endl;

    } else {
        saveLoader->loadMapView( atlas );
        cout << "Loads existing atlas." << endl;
    }

    ThreadShare::save_loader = saveLoader;
    ThreadShare::atlas_builder = atlas_builder;
    ThreadShare::atlas_view = atlas;

    *backEndThread  = new std::thread(BackEnd_Thread, 0);
    cout << "BackEnd_Thread: Started." << endl;

    atlasViewWidget->setComponents(atlas,saveLoader);
    nodeParentChildernViewWidget->setComponents(atlas,saveLoader);

    realizationViewWidgetCornerVersion->setMolecularUnits(muA,muB);
    realizationViewWidgetCornerVersion->setSaveLoader(saveLoader);

    realizationViewWidget->setMolecularUnits(muA,muB);
    realizationViewWidget->setSaveLoader(saveLoader);

    pathfindingViewWidget->setData(atlas,saveLoader,muA,muB);

    constrainSelectionWindow->setData(atlas,muA,muB);

}

void MainWindow::setLayouts()
{
    // Layout looks as vertical box with group box inside
    // Vertical box also contains menu bar
    mainLayout = new QVBoxLayout;
    // Group box contains atlas view and molecule widget
    mainGroupBox = new QWidget();
    // main layout belongs to main widget
    mainWidget = new QWidget();

    //[ HANDLE MENU]
    menuBar = new QMenuBar(mainWidget);
    createAction = new QAction(tr("&Create"),this);
    connect(createAction, SIGNAL(triggered()), this, SLOT(openInputWindow()));
    closeAction = new QAction(tr("&Close"),this);
    connect(closeAction, SIGNAL(triggered()), this, SLOT(close()));
    fileMenu = new QMenu("File");
    fileMenu->addAction(createAction);
    fileMenu->addAction(closeAction);
    helpMenu = new QMenu("Help");
    helpMenu->addAction(new QAction(tr("&Help"), this));
    menuBar->setFixedHeight(22);
    menuBar->addMenu(fileMenu);
    menuBar->addMenu(helpMenu);
    //[~HANDLE MENU]
    //[ HANDLE STACKEDWIDGET]
    tabWidget->addTab(atlasViewWidget,"Atlas");
    tabWidget->addTab(nodeParentChildernViewWidget,"Node neighbors");
    tabWidget->addTab(sweepViewWidget,"Sweep");
    tabWidget->addTab(cayleySpaceViewWidget,"Cayley space");
    tabWidget->addTab(realizationViewWidget,"Realization");
    tabWidget->addTab(pathfindingViewWidget,"Pathfinding");
    tabWidget->setCurrentWidget(atlasViewWidget);
    //[~HANDLE STACKEDWIDGET]

    mainGroupBoxLayout = new QGridLayout;


    mainGroupBoxLayout->setContentsMargins(1,1,1,1);
    atlasViewWidget->setContentsMargins(1,1,1,1);
    nodeParentChildernViewWidget->setContentsMargins(1,1,1,1);
    sweepViewWidget->setContentsMargins(1,1,1,1);
    cayleySpaceViewWidget->setContentsMargins(1,1,1,1);
    realizationViewWidgetCornerVersion->setContentsMargins(1,1,1,1);
    tabWidget->setContentsMargins(0,0,0,0);
    mainWidget->setContentsMargins(1,1,1,1);
    menuBar->setContentsMargins(1,1,1,1);
    mainLayout->setContentsMargins(1,1,1,1);
    mainGroupBox->setContentsMargins(1,1,1,1);
    this->setContentsMargins(1,1,1,1);

    //Set interface layout (the one that contains all buttons and active constrain graph)
    int interfaceLayoutRow = 0;
    interfaceLayout.addWidget(bondViewWidget,interfaceLayoutRow,0,3,1);
    interfaceLayout.addWidget(&currentSelectedNodeLabel,interfaceLayoutRow,1,1,1);
    interfaceLayoutRow++;
    interfaceLayout.addWidget(&currentSampledNodeLabel,interfaceLayoutRow,1,1,1);
    interfaceLayoutRow++;
    interfaceLayout.addWidget(&currentRunModeLabel,interfaceLayoutRow,1,1,1);
    interfaceLayoutRow++;
    interfaceLayout.addWidget(&findPathBetweenD0NodesButton,interfaceLayoutRow,0,1,2);
    interfaceLayoutRow++;
    interfaceLayout.addWidget(&pathfindingButton[START],interfaceLayoutRow,0,1,1);
    interfaceLayout.addWidget(&pathfindingFlipLabel[START],interfaceLayoutRow,1,1,1);
    interfaceLayoutRow++;
    interfaceLayout.addWidget(&pathfindingValueLabel[START],interfaceLayoutRow,0,1,1);
    interfaceLayout.addWidget(&pathfindingFlipSelectionComboBox[START],interfaceLayoutRow,1,1,1);
    interfaceLayoutRow++;
    interfaceLayout.addWidget(&pathfindingButton[END],interfaceLayoutRow,0,1,1);
    interfaceLayout.addWidget(&pathfindingFlipLabel[END],interfaceLayoutRow,1,1,1);
    interfaceLayoutRow++;
    interfaceLayout.addWidget(&pathfindingValueLabel[END],interfaceLayoutRow,0,1,1);
    interfaceLayout.addWidget(&pathfindingFlipSelectionComboBox[END],interfaceLayoutRow,1,1,1);

    interfaceBox.setFixedWidth(370);
    interfaceBox.setLayout(&interfaceLayout);

    mainGroupBoxLayout->addWidget(tabWidget,0,0,10,2);
    mainGroupBoxLayout->addWidget(realizationViewWidgetCornerVersion,0,3,4,1);
    mainGroupBoxLayout->addWidget(&interfaceBox,6,3,3,1);
    mainGroupBoxLayout->addWidget(&stopStartSampleButton,4,3,1,1);
    mainGroupBoxLayout->addWidget(&runModeSelectionComboBox,5,3,1,1);
    mainGroupBoxLayout->setSpacing(1);
    mainLayout->setSpacing(1);
    mainGroupBox->setLayout(mainGroupBoxLayout);

    mainLayout->addWidget(menuBar);
    mainLayout->addWidget(mainGroupBox);


    mainWidget->setLayout(mainLayout);
    setCentralWidget(mainWidget);
}

void MainWindow::init_DistanceFinder_from_settings(PredefinedInteractions *df)
{
    vector<vector<string> > data = readData(Settings::DistanceData::file);

    if(data.size() == 0)
            return;
    bool matrixInsteadOfColumns = false;
    //Check if the data is in matrix format
    if(data.front().size() == data.size()){
        for(std::vector<string>::size_type iter=0; iter != data.size(); iter++) {
            if(data[iter][1] == data[1][iter])
                matrixInsteadOfColumns = true;
            else{
                matrixInsteadOfColumns = false;
                break;
            }
        }
    }

    PredefinedInteractions *output;
    if (matrixInsteadOfColumns) {
        output = new PredefinedInteractions( data );
    }
    else if (Settings::DistanceData::radiusMin_col >= 0
            && Settings::DistanceData::radiusMax_col >= 0)
    {
        output = new PredefinedInteractions(
                data,
                Settings::DistanceData::label1_col,
                Settings::DistanceData::label2_col,
                Settings::DistanceData::radiusMin_col,
                Settings::DistanceData::radiusMax_col);
    } else if (Settings::DistanceData::radius_col >= 0) {
        output = new PredefinedInteractions(
                data,
                Settings::DistanceData::label1_col,
                Settings::DistanceData::label2_col,
                Settings::DistanceData::radius_col);
    } else if (Settings::DistanceData::radiusMin_col >= 0) {
        output = new PredefinedInteractions(
                data,
                Settings::DistanceData::label1_col,
                Settings::DistanceData::label2_col,
                Settings::DistanceData::radiusMin_col);
    } else {
        output = new PredefinedInteractions();
    }

    df->assign(output);
    delete output;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_F1)
    {
        std::cout<<"F1"<<std::endl;
        stackedWidget->setCurrentWidget(atlasViewWidget);
    }
    if(event->key()==Qt::Key_F2)
    {
        std::cout<<"F2"<<std::endl;
        stackedWidget->setCurrentWidget(nodeParentChildernViewWidget);

        nodeParentChildernViewWidget->setAtlasNode(atlasViewWidget->getCurrentNode());
    }
    if(event->key()==Qt::Key_F3)
    {
        std::cout<<"F3"<<std::endl;
        stackedWidget->setCurrentWidget(sweepViewWidget);

        sweepViewWidget->setAtlasNode(atlasViewWidget->getCurrentNode());
    }
    if(event->key()==Qt::Key_F4)
    {
        std::cout<<"F4"<<std::endl;
        stackedWidget->setCurrentWidget(cayleySpaceViewWidget);
        cayleySpaceViewWidget->setAtlasNode(atlasViewWidget->getCurrentNode());
    }
    if(event->key()==Qt::Key_F5)
    {
        std::cout<<"F5"<<std::endl;
        stackedWidget->setCurrentWidget(realizationViewWidget);
    }
}

void MainWindow::setDependenices(int argc,
                               char **argv,
                               MolecularUnit *muA,
                               MolecularUnit *muB,
                               SaveLoader *save_loader,
                               AtlasBuilder *atlas_builder,
                               Atlas *atlas_view, std::thread *backEndThread)
{
    this->argc=argc;
    this->argv=argv;
    this->muA=muA;
    this->muB=muB;
    this->saveLoader=save_loader;
    this->atlas_builder=atlas_builder;
    this->backEndThread = &backEndThread;
    this->atlas=atlas_view;
}
