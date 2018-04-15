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
#ifndef ATLASWINDOW_H
#define ATLASWINDOW_H

#include <QtWidgets>

#include "iostream"
#include <thread>
#include "Source/backend/AtlasBuilder.h"
#include "Source/backend/Atlas.h"
#include "Source/backend/Settings.h"
#include "Source/backend/SaveLoader.h"
#include "Source/backend/readIn.h"
#include "Source/backend/Thread.h"
#include "Source/backend/Thread_BackEnd.h"
#include "Source/backend/ThreadShare.h"
#include "Source/backend/pathfinder.h"


#include "Source/gui_Qt/RealizationViewWidgetCornerVersion.h"
#include "Source/gui_Qt/SweepViewWidget.h"
#include "Source/gui_Qt/NodeParentChildernViewWidget.h"
#include "Source/gui_Qt/SharedDataGUI.h"
#include "Source/gui_Qt/CayleySpaceViewWidget.h"
#include "Source/gui_Qt/RealisationViewWidget.h"
#include "Source/gui_Qt/ConstraintSelectionWindow.h"
#include "Source/gui_Qt/MessageWindow.h"
#include "Source/gui_Qt/PathfindingViewWidget3DPart.h"
#include "InputWindow.h"
#include "AtlasViewWidget.h"
#include "BondViewWidget.h"

class InputWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();

    //[ COMPONENTS]
    InputWindow *inputWindowInstance;

    RealizationViewWidgetCornerVersion *realizationViewWidgetCornerVersion;
    AtlasViewWidget *atlasViewWidget;
    SweepViewWidget *sweepViewWidget;
    QWidget *mainWidget;
    BondViewWidget *bondViewWidget;
    NodeParentChildernViewWidget *nodeParentChildernViewWidget;
    CayleySpaceViewWidget *cayleySpaceViewWidget;
    QStackedWidget *stackedWidget;
    QTabWidget *tabWidget;
    RealisationViewWidget *realizationViewWidget;
    ConstraintSelectionWindow *constrainSelectionWindow;
    PathfindingViewWidget3DPart *pathfindingViewWidget;

    QLayout *mainLayout;
    QGridLayout *mainGroupBoxLayout;    
    QGridLayout interfaceLayout;
    QWidget *mainGroupBox;
    QWidget interfaceBox;

    QLineEdit pathfindingLineEdit[2];
    QPushButton pathfindingButton[2];
    QLabel pathfindingValueLabel[2];
    QLabel pathfindingFlipLabel[2];
    QComboBox pathfindingFlipSelectionComboBox[2];
    QPushButton findPathBetweenD0NodesButton;

    SharedDataGUI sharedData;
    //[~COMPONENTS]
    //[ DEPENDENCIES]
    int argc;
    char **argv;
    MolecularUnit *muA;
    MolecularUnit *muB;
    SaveLoader *saveLoader;
    AtlasBuilder *atlas_builder;
    Atlas *atlas;
    std::thread **backEndThread;
    //[~DEPENDENCIES]
    //[ MENU]
    QMenuBar *menuBar;
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *helpMenu;
    QAction *createAction;
    QAction *closeAction;
    QAction *help;
    QPushButton stopStartSampleButton;
    QComboBox runModeSelectionComboBox;
    QLabel currentSampledNodeLabel;
    QLabel currentSelectedNodeLabel;
    QLabel currentRunModeLabel;
    //[~MENU]
    //[ TIMER]
    QTimer *timer;
    double updateTimeInMS;
    //[~TIMER]
    QString samplingModesNames[6] = {"Stopped", "TreeSample", "ForestSample", "ForestSampleAS", "BreadthFirst", "RefineSampling"};
    void setDependenices(int argc,
                      char **argv,
                      MolecularUnit *muA,
                      MolecularUnit *muB,
                      SaveLoader *saveLoader,
                      AtlasBuilder *atlas_builder,
                      Atlas *atlas, std::thread *backEndThread);

private slots:
    void openInputWindow();
    void stopStartSamplingButtonSlot();
    void findPathBetweenD0NodesButtonSlot();
    /**
     * @brief updateSamplingStatus updates currentSampledNodeLabel, currentSelectedNodeLabel, currentRunModeLabel each updateTimeInMS
     */
    void updateSlot();
    void pathfindingStartNodeButtonSlot();
    void pathfindingEndNodeButtonSlot();

private:
    bool isSamplingStopped = false;
    /**
     * @brief activeWidgetID: int that hold ID of current widget in stackedWidget
     */
    int activeWidgetID;
    /**
     * @brief processDataFromSettings: is called after dependencies are set to start back end with correct data and settings
     */
    void processDataFromSettings();
    /**
     * @brief setLayouts: wrapper for code that determines how all widgets are arranged
     */
    void setLayouts();
    /**
     * @brief init_DistanceFinder_from_settings: this function used to be in main.cpp and when I was making QT GUI I was forced to move it here
     * @param df
     */
    void init_DistanceFinder_from_settings(PredefinedInteractions *df);
    /**
     * @brief keyPressEvent: Qt function that we override to handle keyborad input
     * @param event
     */
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

    //[ PATHFINDING VARS]
    int currentPathfindingSelectedAtlasNodeID [2];
    PointType currentPathfindingSelectedCayleyPointType [2];
    int currentPathfindingSelectedCayleyPointID [2];
    QString pathfindingButtonText[2];
    //[~PATHFINDING VARS]
};




#endif // ATLASWINDOW_H
