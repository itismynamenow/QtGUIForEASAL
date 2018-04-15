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
#include "CayleySpaceViewWidget.h"

CayleySpaceViewWidget::CayleySpaceViewWidget(SharedDataGUI *sharedData)
{


    this->sharedData = sharedData;
    camera.init(this->size(),QVector3D(0,0,0),QVector3D(0,0,-1));
    //[ TIMER SETTINGS]
    // start argument determins update rate
    m_updateTimeInMS=30;
    m_timer.start(m_updateTimeInMS);
    // repaint is function of QWidget that triggers paintEvent
    connect(&m_timer,SIGNAL(timeout()),this,SLOT(update()));
    //[ TIMER SETTINGS]
    //[ WIDGET SETTINGS]
    setFocusPolicy(Qt::StrongFocus);
    setUpdateBehavior(UpdateBehavior::PartialUpdate);
    //[~WIDGET SETTINGS]
    //[ MISC]
        currentACR = new ActiveConstraintRegion();

        parameter4thLabel.setText("Select value for 4th parameter: ");
        parameter5thLabel.setText("Select value for 5th parameter: ");

        pointTypeSelectionComboBox.addItem(QString("All"));
        pointTypeSelectionComboBox.addItem(QString("Valid"));
        pointTypeSelectionComboBox.addItem(QString("Bad angle"));
        pointTypeSelectionComboBox.addItem(QString("Collision"));
        pointTypeSelectionComboBox.addItem(QString("Not realizable"));
        pointTypeSelectionComboBox.setMaximumWidth(150);
        connect(&pointTypeSelectionComboBox,SIGNAL(currentIndexChanged(QString)),this, SLOT(pointTypeSelectionComboBoxSlot()));
        connect(&boundarySelectionComboBox,SIGNAL(currentIndexChanged(QString)),this, SLOT(boundarySelectionComboBoxSlot()));

        boundaryLabel.setText("Boundary");
        boundarySelectionComboBox.addItem("NONE");


        gridLayout.setAlignment(Qt::AlignBottom);
        gridLayout.addWidget(&pointTypeSelectionComboBox,0,0,1,1);
        gridLayout.addWidget(&boundaryLabel,1,0,1,1);
        gridLayout.addWidget(&boundarySelectionComboBox,1,1,1,1);
        gridLayout.addWidget(&parameter4thLabel,2,0,1,1);
        gridLayout.addWidget(&parameter5thLabel,3,0,1,1);
        gridLayout.addWidget(&sliderFor4thParamter,2,1,1,1);
        gridLayout.addWidget(&sliderFor5thParamter,3,1,1,1);

        setLayout(&gridLayout);
    //[~MISC]re
}

void CayleySpaceViewWidget::setData(Atlas *atlas, SaveLoader *saveLoader)
{
    this->atlas=atlas;
    this->saveLoader=saveLoader;
    dataWasSet=true;
}

void CayleySpaceViewWidget::pointTypeSelectionComboBoxSlot()
{
    showAll = false;
    showValid = false;
    showBadAngle = false;
    showCollisions = false;
    showNotRealizable = false;
    QString currentComboBoxText = pointTypeSelectionComboBox.currentText();
    if(currentComboBoxText == "All")
    {
        showAll = true;
    }
    else if(currentComboBoxText == "Valid")
    {
        showValid = true;
    }
    else if(currentComboBoxText == "Bad angle")
    {
        showBadAngle = true;
    }
    else if(currentComboBoxText == "Collision")
    {
        showCollisions = true;
    }
    else if(currentComboBoxText == "Not realizable")
    {
        showNotRealizable = true;
    }

    updateTransformationMatrices();
}

void CayleySpaceViewWidget::boundarySelectionComboBoxSlot()
{
    if(boundarySelectionComboBox.currentText() == "NONE")
    {
        currentBoundaryIndex = NONE;
        updateBoundary();
    }
    else
    {
        bool isInt = true;
        int boundaryIndexTmp = boundarySelectionComboBox.currentText().toInt(&isInt,10);
        if(isInt)
        {
            currentBoundaryIndex = boundaryIndexTmp;
            updateBoundary();
        }
        else
        {
            currentBoundaryIndex = NONE;
            cout<<"CayleySpaceViewWidget::updateBoundarySelectionComboBox() ERROR - wrong value in boundarySelectionComboBox"<<endl;
        }
    }
}

void CayleySpaceViewWidget::initializeGL()
{
    renderer.initOpenGL();
    //[ INIT MESHES]
    cube.createAndBind("Cube.obj");
    cylinder.createAndBind("12Cylinder.obj");
    sphere.createAndBind("12by12UVsphere.obj");
    //[~INIT MESHES]
    generateTransformMatricesForAxis();
}

void CayleySpaceViewWidget::paintGL()
{
    if(sharedData->atlasNodeWasSet)
    {
        updateAtlasNode();
        updateSliders();
        renderer.renderingProgram->setUniformValue("lightPositionVector",camera.getEyeLocation()+camera.getRight()*10+camera.getUp()*30);
        renderer.renderingProgram->setUniformValue("cameraMatrix",camera.getCameraMatrix(mapFromGlobal(cursor().pos())));

        if(mouseWasPressed)
        {
            updateCurrentCayleyPointIndex();
            mouseWasPressed = false;
            if(currentCayleyPointGroupedIndex != -1)
            {
                for(int i=0;i<2;i++)//Check if user pressed button responsible for selecting cayley point as start or end node of pathfinding
                {
                    if(sharedData->pathfindingSelectCayleyPointNow[i])//TODO not forget that mouseclick is only procesed next frame!!!!
                    {
                        int numberOfOrientations = groupedCayleyPoints[current5thIndex][current4thIndex][currentCayleyPointGroupedIndex].cayleyPoint->getOrientations().size();
                        if(numberOfOrientations > 0)//Check if valid cayley point was selected
                        {
                            sharedData->pathfindingSelectedAtlasNodeID[i] = currentNodeID;
                            sharedData->pathfindingSelectedCayleyPointType[i] = groupedCayleyPoints[current5thIndex][current4thIndex][currentCayleyPointGroupedIndex].pointType;
                            sharedData->pathfindingSelectedCayleyPointID[i] = currentCayleyPointIndex;
                            sharedData->pathfindingSelectCayleyPointNow[i] = false;
                            sharedData->pathfindingCayleyPointValueWasChanged[i] = true;
                            sharedData->pathfindingFlipVector[i] = groupedCayleyPoints[current5thIndex][current4thIndex][currentCayleyPointGroupedIndex].cayleyPoint->getFlips();
                            //atlasNode->getWitnessFlips(sharedData->pathfindingFlipVector[i]);
                        }
                    }
                     //TODO:Move this to the outermost if an iterate over the vector returned by getWitnessFlips

                     //groupedCayleyPoints[current5thIndex][current4thIndex][currentCayleyPointGroupedIndex].cayleyPoint->getFlips();
                }
            }

        }
        else
        {
            renderer.bindRenderProgramAndClear();
            //Render cubes that represent sampled cayley points
            renderer.renderWithTransperansy(&cube,translationMatrices[PointType::SampledPoint],rotationMatrices[PointType::SampledPoint],scaleMatrices[PointType::SampledPoint],colorVectors[PointType::SampledPoint]);
            //Render spheres that represent witness cayley points
            renderer.renderWithTransperansy(&sphere,translationMatrices[PointType::WitnessPoint],rotationMatrices[PointType::WitnessPoint],scaleMatrices[PointType::WitnessPoint],colorVectors[PointType::WitnessPoint]);

            //Render 3 cylinders thatrepresent axis
            renderer.render(&cylinder, translationMatricesAxis,rotationMatricesAxis,scaleMatricesAxis,colorVectorsAxis);
        }
    }
}

void CayleySpaceViewWidget::resizeGL(int w, int h)
{
    camera.resizeEvent(w,h);
}

void CayleySpaceViewWidget::updateSliders()
{
    double current5thPatameterValue = sliderFor5thParamter.getCurrentValue();
    int current5thParamenterIndex = 0;
    //Find index correspondent to current5thPatameterValue
    for(int i=0;i<groupedCayleyPoints.size();i++)
    {
        if(groupedCayleyPoints[i].size()>0)
        {
            if((*groupedCayleyPoints[i][0][0].cayleyPoint)[4] == current5thPatameterValue)
            {
                current5thParamenterIndex=i;
                break;
            }
        }
    }
    //If value of 5th parameter changed we need to update slider for 4th parameter
    if(current5thIndex !=current5thParamenterIndex)
    {
        std::vector<double> dim4Values;
        for(int i=0;i<groupedCayleyPoints[current5thParamenterIndex].size();i++)
        {
            dim4Values.push_back((*groupedCayleyPoints[current5thParamenterIndex][i][0].cayleyPoint)[3]);
        }
        sliderFor4thParamter.setCayleySpaceViewData((*groupedCayleyPoints[0][0][0].cayleyPoint)[3],(*groupedCayleyPoints.back().back().back().cayleyPoint)[3],sharedData->currentAtlasNode->getID(),dim4Values);
    }
    //Now we obtain value of 4th parameter
    double current4thPatameterValue = sliderFor4thParamter.getCurrentValue();
    int current4thParamenterIndex = 0;
    //Find index correspondent to current4thPatameterValue
    for(int i=0;i<groupedCayleyPoints[current5thParamenterIndex].size();i++)
    {
        if(groupedCayleyPoints[current5thParamenterIndex][i].size()>0)
        {
            if((*groupedCayleyPoints[current5thParamenterIndex][i][0].cayleyPoint)[3] == current4thPatameterValue)
            {
                current4thParamenterIndex=i;
                break;
            }
        }
    }
    //Here we change indices if slider data is diffrerent from last iteration
    if(current5thIndex != current5thParamenterIndex || current4thIndex != current4thParamenterIndex)
    {
        currentCayleyPointIndex = -1;
        currentCayleyPointGroupedIndex = -1;
        current5thIndex = current5thParamenterIndex;
        current4thIndex = current4thParamenterIndex;
        updateTransformationMatrices();
        updateBoundary();
    }
}

void CayleySpaceViewWidget::updateTransformationMatrices()
{
    clearTarnsformationMatrices();
    if(groupedCayleyPoints.size()>0 && groupedCayleyPoints[0].size()>0)
    {
        for(int i=0;i<groupedCayleyPoints[current5thIndex][current4thIndex].size();i++)
        {
            int numberOfOrientations = groupedCayleyPoints[current5thIndex][current4thIndex][i].cayleyPoint->getOrientations().size();

            bool renderCurrentBandAnglePoint = groupedCayleyPoints[current5thIndex][current4thIndex][i].cayleyPoint->getBadAngleN() > 0 && (showBadAngle || showAll);
            bool renderCurrentCollisionPoint = groupedCayleyPoints[current5thIndex][current4thIndex][i].cayleyPoint->getCollidN() > 0 && (showCollisions || showAll);
            bool renderNotRealizable = !groupedCayleyPoints[current5thIndex][current4thIndex][i].cayleyPoint->isRealizable() && (showNotRealizable || showAll);
            bool renderCurrentValidPoint =  numberOfOrientations > 0 && (showValid || showAll);
            if(renderCurrentBandAnglePoint || renderCurrentCollisionPoint || renderCurrentValidPoint || renderNotRealizable)
            {
                double cayleyParameters[6];
                for(int j=0;j<6;j++)
                {
                    cayleyParameters[j]=0;
                }
                groupedCayleyPoints[current5thIndex][current4thIndex][i].cayleyPoint->getPoint(cayleyParameters);
                QVector3D cubePosition (cayleyParameters[0],
                                        cayleyParameters[1],
                                        cayleyParameters[2]);
                QMatrix4x4 translationMatrix;
                QMatrix4x4 rotationMatrix;
                QMatrix4x4 scaleMatrix;
                QVector4D colorVector;

                translationMatrix.translate(cubePosition);
                if(groupedCayleyPoints[current5thIndex][current4thIndex][i].pointType == PointType::SampledPoint)
                {
                    rotationMatrix.setToIdentity();
                }
                else if(groupedCayleyPoints[current5thIndex][current4thIndex][i].pointType == PointType::WitnessPoint)
                {
                    scaleMatrix.scale(1.3);
                }

//                scaleMatrix.scale(cubeScale/2);
                scaleMatrix.scale(Settings::Sampling::stepSize*cubeScale/2);
                if(currentCayleyPointGroupedIndex==i)
                {
                    colorVector = QVector4D(1,1,0,1);
                    scaleMatrix.scale(1.1);
                }
                else
                {
                    if(renderNotRealizable)
                    {
                        colorVector = QVector4D(0,0,1,1);
                        scaleMatrix.scale(1.1);
                    }
                    else if(renderCurrentValidPoint)
                    {
                        colorVector = QVector4D(0,1,0,1);
                        scaleMatrix.scale(1.06);
                    }
                    else if(renderCurrentBandAnglePoint)
                    {
                        colorVector = QVector4D(1,0,0,1);
                        scaleMatrix.scale(1.02);
                    }
                    else if(renderCurrentCollisionPoint)
                    {
                        colorVector = QVector4D(1,0,1,1);
                    }
                }

                //Take care of transperansy that we need in case if boundary node was selected to make all cayley points that are not on that boundary transperant
                bool isOnBoundaryWithSelectedNode = false;
                for(int j=0;j<groupedCayleyPointsBoundaryIndices.size();j++)
                {
                    if(groupedCayleyPointsBoundaryIndices[j]==i)
                    {
                        isOnBoundaryWithSelectedNode = true;
                        break;
                    }
                }

                if(!isOnBoundaryWithSelectedNode && currentBoundaryIndex != NONE)
                {
                    colorVector.setW(transperancyStrength);
                }


                translationMatrices[groupedCayleyPoints[current5thIndex][current4thIndex][i].pointType].push_back(translationMatrix);
                rotationMatrices[groupedCayleyPoints[current5thIndex][current4thIndex][i].pointType].push_back(rotationMatrix);
                scaleMatrices[groupedCayleyPoints[current5thIndex][current4thIndex][i].pointType].push_back(scaleMatrix);
                colorVectors[groupedCayleyPoints[current5thIndex][current4thIndex][i].pointType].push_back(colorVector);
            }
        }
    }
}

void CayleySpaceViewWidget::updateCurrentCayleyPointIndex()
{
    QOpenGLFramebufferObject fbo(this->size());
    fbo.setAttachment(QOpenGLFramebufferObject::Depth);
    fbo.bind();

    renderer.bindPickingProgramAndClear();
    //Render cubes that represent cayley points
    renderer.render(&cube,translationMatrices[PointType::SampledPoint],rotationMatrices[PointType::SampledPoint],scaleMatrices[PointType::SampledPoint],colorVectors[PointType::SampledPoint]);
    renderer.render(&sphere,translationMatrices[PointType::WitnessPoint],rotationMatrices[PointType::WitnessPoint],scaleMatrices[PointType::WitnessPoint],colorVectors[PointType::WitnessPoint],translationMatrices[PointType::SampledPoint].size()/32+1);

    int meshID = renderer.getPixelValueAt(mapFromGlobal(cursor().pos()),this->size());
    if(meshID>=0)
    {
        PointType pointTypeTmp;
        //If meshID that we got from mouse picking is bigger then number of sampled poits the nit is witness point
        if(meshID >= translationMatrices[PointType::SampledPoint].size() )
        {
            meshID = meshID - 32*(translationMatrices[PointType::SampledPoint].size()/32+1);
            pointTypeTmp = PointType::WitnessPoint;
        }
        else
        {
            pointTypeTmp = PointType::SampledPoint;
        }
        updateIndecies(meshID,pointTypeTmp);
        updateTransformationMatrices();
    }
    fbo.release();
    sharedData->currentCayleyPointID = currentCayleyPointIndex;
    sharedData->currentCayleyPointType = currentCayleyPointType;
    updateCameraPosition();
}

void CayleySpaceViewWidget::updateCameraPosition()
{
    if(currentCayleyPointGroupedIndex>=0)
    {
        CayleyPoint* tmpCayleyPoint = groupedCayleyPoints[current5thIndex][current4thIndex][currentCayleyPointGroupedIndex].cayleyPoint;
        QVector3D cayleyPointPosition((*tmpCayleyPoint)[0],(*tmpCayleyPoint)[1],(*tmpCayleyPoint)[2]);
        camera.setPosition(cayleyPointPosition);
    }
}

void CayleySpaceViewWidget::updateAtlasNode()
{
    if(currentNodeID!=sharedData->currentAtlasNode->getID())
    {
        setAtlasNode(sharedData->currentAtlasNode);
    }
}

void CayleySpaceViewWidget::clearTarnsformationMatrices()
{
    for(int i=0;i<NUMBER_OF_POINT_TYPES;i++)
    {
        translationMatrices[i].clear();
        rotationMatrices[i].clear();
        scaleMatrices[i].clear();
        colorVectors[i].clear();
    }
}

void CayleySpaceViewWidget::clearGroupedCayleyPoints()
{
    for(int i=0;i<groupedCayleyPoints.size();i++)
    {
        for(int j=0;j<groupedCayleyPoints[i].size();j++)
        {
            groupedCayleyPoints[i][j].clear();
        }
        groupedCayleyPoints[i].clear();
    }
    groupedCayleyPoints.clear();
}

void CayleySpaceViewWidget::fillWitnessAndSampledPoints(ActiveConstraintRegion *ACR)
{
    //clear
    witnessAndSampledPoints.clear();
    //fill with data from space that contains sampled Cayley points
    for(int i=0;i<ACR->space.size();i++)
    {
        witnessAndSampledPoints.push_back(CayleyPointRenderingWrapper(ACR->space[i],PointType::SampledPoint,i));
    }
    //fill with data from witnessPoints
    for(int i=0;i<ACR->witspace.size();i++)
    {
        witnessAndSampledPoints.push_back(CayleyPointRenderingWrapper(ACR->witspace[i],PointType::WitnessPoint,i));
    }
}

void CayleySpaceViewWidget::setAtlasNode(AtlasNode *atlasNode)
{
    currentNodeID = atlasNode->getID();
    //[ CLEAR]
    //Delete old ACR and create new to feed it to saveloader that will allow to get orientation data about atlasNode
    if(currentACR != 0)
    {
        delete currentACR;
    }

    //[~CLEAR]
    //[ GET NEW ACR]
    currentACR = new ActiveConstraintRegion();
    saveLoader->loadNode(atlasNode->getID(),currentACR);
    //[~GET NEW ACR]
    clearGroupedCayleyPoints();
    fillWitnessAndSampledPoints(currentACR);
    groupBy4thAnd5thCayleyParameter(currentACR->space);

    //[ SET DATA FOR SLIDERS]
    std::vector<double> dim5Values;
    std::vector<double> dim4Values;
    for(int i=0;i<groupedCayleyPoints.size();i++)
    {
        dim5Values.push_back((*groupedCayleyPoints[i][0][0].cayleyPoint)[4]);
    }
    for(int i=0;i<groupedCayleyPoints[0].size();i++)
    {
        dim4Values.push_back((*groupedCayleyPoints[0][i][0].cayleyPoint)[3]);
    }

    int dim = atlasNode->getDim();
    if(dim == 5)
    {
        sliderFor5thParamter.setVisible(true);
        parameter5thLabel.setVisible(true);
    }
    else
    {
        sliderFor5thParamter.setVisible(false);
        parameter5thLabel.setVisible(false);
    }

    if(dim == 5 || dim ==4)
    {
        sliderFor4thParamter.setVisible(true);
        parameter4thLabel.setVisible(true);
    }
    else
    {
        sliderFor4thParamter.setVisible(false);
        parameter4thLabel.setVisible(false);
    }

    sliderFor5thParamter.setCayleySpaceViewData((*groupedCayleyPoints[0][0][0].cayleyPoint)[4],(*groupedCayleyPoints.back().back().back().cayleyPoint)[4],atlasNode->getID(),dim5Values);
    sliderFor4thParamter.setCayleySpaceViewData((*groupedCayleyPoints[0][0][0].cayleyPoint)[3],(*groupedCayleyPoints.back().back().back().cayleyPoint)[3],atlasNode->getID(),dim4Values);
    current5thIndex = 0;
    current4thIndex = 0;
    currentCayleyPointIndex = 0;
    currentCayleyPointGroupedIndex = 0;
    updateCameraPosition();
    //[~SET DATA FOR SLIDERS]
    //[ SET DATA FOR BOUNDARY DROPDOWN]
    //MUST BE CALLED AFTER SET DATA FOR SLIDERS
    boundarySelectionComboBox.clear();
    boundarySelectionComboBox.addItem("NONE");

    vector <int> nodeNeighbors = atlasNode->getConnection();
    for(int i=0;i<nodeNeighbors.size();i++)
    {
        if((*atlas)[nodeNeighbors[i]]->getDim() < atlasNode->getDim())
        {
            boundarySelectionComboBox.addItem(QString::number(nodeNeighbors[i]));
        }
    }
    //[!SET DATA FOR BOUNDARY DROPDOWN]
    updateTransformationMatrices();
    return;
}

void CayleySpaceViewWidget::keyPressEvent(QKeyEvent *keyEvent)
{
    camera.keyPressEvent(keyEvent);

    if(keyEvent->key()==Qt::Key_1)
    {
        showValid = !showValid;
        if(showValid)
        {
            showBadAngle = false;
            showCollisions = false;
        }
        updateTransformationMatrices();
    }
    if(keyEvent->key()==Qt::Key_2)
    {
        showBadAngle = !showBadAngle;
        if(showBadAngle)
        {
            showValid = false;
            showCollisions = false;
        }
        updateTransformationMatrices();
    }
    if(keyEvent->key()==Qt::Key_3)
    {
        showCollisions = !showCollisions;
        if(showCollisions)
        {
            showValid = false;
            showBadAngle = false;
        }
        updateTransformationMatrices();
    }
}

void CayleySpaceViewWidget::mousePressEvent(QMouseEvent *mouseEvent)
{
    camera.mousePressEvent(mouseEvent);
    mouseWasPressed = true;
}

void CayleySpaceViewWidget::mouseReleaseEvent(QMouseEvent *mouseEvent)
{
    camera.mouseReleaseEvent(mouseEvent);
}

void CayleySpaceViewWidget::wheelEvent(QWheelEvent *wheelEvent)
{
    camera.wheelEvent(wheelEvent);
}

void CayleySpaceViewWidget::generateTransformMatricesForAxis()
{
    for(int i=0;i<3;i++)
    {
        translationMatricesAxis.push_back(QMatrix4x4());
        rotationMatricesAxis.push_back(QMatrix4x4());
        scaleMatricesAxis.push_back(QMatrix4x4());
        colorVectorsAxis.push_back(QVector4D());
    }

    colorVectorsAxis[0] = QVector4D(1,0,0,1);
    colorVectorsAxis[1] = QVector4D(0,1,0,1);
    colorVectorsAxis[2] = QVector4D(0,0,1,1);

    QVector3D start(0,0,0);
    QVector3D endX(10,0.001,0);
    QVector3D endY(0.001,10,0);
    QVector3D endZ(0,0.001,10);

    double axisRadious = 0.02;

    transformCylinderAccordingToData(start,endX,axisRadious,&translationMatricesAxis[0],&rotationMatricesAxis[0],&scaleMatricesAxis[0]);
    transformCylinderAccordingToData(start,endY,axisRadious,&translationMatricesAxis[1],&rotationMatricesAxis[1],&scaleMatricesAxis[1]);
    transformCylinderAccordingToData(start,endZ,axisRadious,&translationMatricesAxis[2],&rotationMatricesAxis[2],&scaleMatricesAxis[2]);
}

void CayleySpaceViewWidget::transformCylinderAccordingToData(QVector3D positionStart, QVector3D positionEnd,
                                                             float radious, QMatrix4x4 *translation,  QMatrix4x4 *rotation,  QMatrix4x4 *scale)
{

    //[ FIND ROTATION]
    QVector3D directionNotNormalized = QVector3D(positionEnd-positionStart);
    QVector3D direction = QVector3D(positionEnd-positionStart);
    QVector3D directionProjection = QVector3D(direction.x(),0,direction.z());
    direction.normalize();
    QVector3D axisOfRotation = QVector3D::crossProduct( directionProjection,direction);
    float angle = acos(QVector3D::dotProduct(direction, directionProjection)/(direction.length()*directionProjection.length()));
    angle = angle*180/PI+90;
    //[~FIND ROTATION]
    QOpenGLExtraFunctions *f = QOpenGLContext::currentContext()->extraFunctions();

    translation->setToIdentity();
    rotation->setToIdentity();
    scale->setToIdentity();

    translation->translate(positionStart+directionNotNormalized/2);
    rotation->rotate(angle,axisOfRotation);
    scale->scale(QVector3D(radious,directionNotNormalized.length()/2,radious));
}

void CayleySpaceViewWidget::groupBy4thAnd5thCayleyParameter(std::vector<CayleyPoint*> cayleySpace)
{
    //[ SORT BY 5TH PARAMETER]
    std::sort(witnessAndSampledPoints.begin(),witnessAndSampledPoints.end(),[](CayleyPointRenderingWrapper a,CayleyPointRenderingWrapper b)
    {
        return (*(a.cayleyPoint))[4] > (*(b.cayleyPoint))[4];
    });
    //[~SORT BY 5TH PARAMETER]
    //[ SORT BY 4TH PARAMETER]
    int indexBegin=0;
    int indexEnd=0;
    bool sortWasUsed = false;
    for(int i=1;i<witnessAndSampledPoints.size();i++)
    {
        if((*(witnessAndSampledPoints[i].cayleyPoint))[index5]!= (*(witnessAndSampledPoints[indexBegin].cayleyPoint))[index5])
        {
            indexEnd=i-1;
            std::vector<CayleyPointRenderingWrapper>::iterator beginIterator = witnessAndSampledPoints.begin() + indexBegin;
            std::vector<CayleyPointRenderingWrapper>::iterator endIterator = witnessAndSampledPoints.begin() + indexEnd;
            sort(beginIterator, endIterator, [](CayleyPointRenderingWrapper a,CayleyPointRenderingWrapper b)
            {
                return (*(a.cayleyPoint))[3] > (*(b.cayleyPoint))[3];
            });
            indexBegin=i;
        }
        //If all points had same 5th parameter we still need to sort them once in the end of this loop
        if(i==witnessAndSampledPoints.size()-1 && indexBegin==0)
        {
            indexEnd=i;
            std::vector<CayleyPointRenderingWrapper>::iterator beginIterator = witnessAndSampledPoints.begin() + indexBegin;
            std::vector<CayleyPointRenderingWrapper>::iterator endIterator = witnessAndSampledPoints.begin() + indexEnd;
            sort(beginIterator, endIterator, [](CayleyPointRenderingWrapper a,CayleyPointRenderingWrapper b)
            {
                return (*(a.cayleyPoint))[3] > (*(b.cayleyPoint))[3];
            });
        }
    }
    //[~SORT BY 4TH PARAMETER]
//    //test sorting
//    for(int i=1;i<sortedSpace.size();i++)
//    {
//       if((*sortedSpace[i])[4]>(*sortedSpace[i-1])[4])
//       {
//           cout<<"sotred incorrectly by 5th parameter"<<endl;
//       }
//    }
    //[ GROUP POINTS]
    {
    vector<vector<CayleyPointRenderingWrapper>> tmp2dCayleyPointVector;
    vector<CayleyPointRenderingWrapper> tmp1dCayleyPointVector;
    tmp1dCayleyPointVector.push_back(witnessAndSampledPoints[0]);
    tmp2dCayleyPointVector.push_back(tmp1dCayleyPointVector);
    groupedCayleyPoints.push_back(tmp2dCayleyPointVector);
    }
    for(int i=1;i<witnessAndSampledPoints.size();i++)
    {
        if( (*(witnessAndSampledPoints[i].cayleyPoint))[4] == (*(witnessAndSampledPoints[i-1].cayleyPoint))[4] &&
            (*(witnessAndSampledPoints[i].cayleyPoint))[3] == (*(witnessAndSampledPoints[i-1].cayleyPoint))[3] )
        {
            groupedCayleyPoints.back().back().push_back(witnessAndSampledPoints[i]);
        }
        else if((*(witnessAndSampledPoints[i].cayleyPoint))[4] == (*(witnessAndSampledPoints[i-1].cayleyPoint))[4] &&
                (*(witnessAndSampledPoints[i].cayleyPoint))[3] != (*(witnessAndSampledPoints[i-1].cayleyPoint))[3] )
        {
            vector<CayleyPointRenderingWrapper> tmp1dCayleyPointVector;
            tmp1dCayleyPointVector.clear();
            tmp1dCayleyPointVector.push_back(witnessAndSampledPoints[i]);
            groupedCayleyPoints.back().push_back(tmp1dCayleyPointVector);
        }
        else
        {
            vector<vector<CayleyPointRenderingWrapper>> tmp2dCayleyPointVector;
            vector<CayleyPointRenderingWrapper> tmp1dCayleyPointVector;
            tmp1dCayleyPointVector.push_back(witnessAndSampledPoints[i]);
            tmp2dCayleyPointVector.push_back(tmp1dCayleyPointVector);
            groupedCayleyPoints.push_back(tmp2dCayleyPointVector);
        }
    }
    //[~GROUP POINTS]
}

void CayleySpaceViewWidget::updateIndecies(int meshID, PointType pointType)
{
    //We have index of mesh that user selected
    //This index is related to groupedCayleyPoints and allows us to find cayley point that corresponds to it
    //This helps us to find index of cayley point in currentACR
    int count = -1;
    int groupedCayleyPointIndex = -1;
    if(groupedCayleyPoints.size()>0 && groupedCayleyPoints[0].size()>0 && meshID<groupedCayleyPoints[current5thIndex][current4thIndex].size())
    {
        //[ FIND CORREPONDING GROUPED CAYLEY POINT INDEX]
        for(int i=0;i<groupedCayleyPoints[current5thIndex][current4thIndex].size();i++)
        {
            int numberOfOrientations = groupedCayleyPoints[current5thIndex][current4thIndex][i].cayleyPoint->getOrientations().size();

            bool pointTypesAreSame = pointType == groupedCayleyPoints[current5thIndex][current4thIndex][i].pointType;

            bool renderCurrentBandAnglePoint = groupedCayleyPoints[current5thIndex][current4thIndex][i].cayleyPoint->getBadAngleN() > 0 && (showBadAngle || showAll);
            bool renderCurrentCollisionPoint = groupedCayleyPoints[current5thIndex][current4thIndex][i].cayleyPoint->getCollidN() > 0 && (showCollisions || showAll);
            bool renderNotRealizable = !groupedCayleyPoints[current5thIndex][current4thIndex][i].cayleyPoint->isRealizable() && (showNotRealizable || showAll);
            bool renderCurrentValidPoint =  numberOfOrientations > 0 && (showValid || showAll);

            if((renderCurrentBandAnglePoint || renderCurrentCollisionPoint || renderCurrentValidPoint || renderNotRealizable) && pointTypesAreSame)
            {
                count++;
            }
            if(count == meshID)
            {
                groupedCayleyPointIndex = i;
                break;
            }

        }
        //[~FIND CORREPONDING GROUPED CAYLEY POINT INDEX]
    }

//    cout<<"CayleyPointIndex = "<<cayleyPointIndex<<endl;

    currentCayleyPointIndex = groupedCayleyPoints[current5thIndex][current4thIndex][groupedCayleyPointIndex].indexInACR;
    currentCayleyPointType = groupedCayleyPoints[current5thIndex][current4thIndex][groupedCayleyPointIndex].pointType;
    currentCayleyPointType = pointType;
    currentCayleyPointGroupedIndex = groupedCayleyPointIndex;
}

void CayleySpaceViewWidget::updateBoundary()
{
    if(sharedData->atlasNodeWasSet)
    {
        groupedCayleyPointsBoundaryIndices.clear();
        if(currentBoundaryIndex != NONE)
        {
            for(int i=0;i<groupedCayleyPoints[current5thIndex][current4thIndex].size();i++)
            {
                list<int> indiciesOfNodesOnBoundary = groupedCayleyPoints[current5thIndex][current4thIndex][i].cayleyPoint->getBoundaries();
                for(auto indexPtr = indiciesOfNodesOnBoundary.begin();indexPtr != indiciesOfNodesOnBoundary.end();indexPtr++)
                {
                    if(*indexPtr == currentBoundaryIndex)
                    {
                        groupedCayleyPointsBoundaryIndices.push_back(i);
                    }
                }
            }
        }
        if(current5thIndex != -1 && current4thIndex != -1)
        {
            updateTransformationMatrices();
        }
    }
}
