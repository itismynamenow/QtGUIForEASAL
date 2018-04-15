#-------------------------------------------------
#
# Project created by QtCreator 2017-04-24T14:28:39
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = helix-packing
TEMPLATE = app

INCLUDEPATH +=$$PWD/include /usr/local/include/mongocxx/v_noabi /usr/local/include/bsoncxx/v_noabi 


DESTDIR = ./bin
OBJECTS_DIR = ./bin
MOC_DIR = ./bin

CONFIG += c++11

# To create a debug build, uncomment the following line
CONFIG += debug

# Uncomment the following line to suppress warnings
CONFIG += warn_off 

#Uncomment the following line to build the Server code
#CONFIG += Server

CONFIG(Server) {
DEFINES += "SERVER"
LIBS += -ljsoncpp -lmongocxx -lbsoncxx
}

SOURCES += \
    Source/backend/ActiveConstraintGraph.cpp \
    Source/backend/ActiveConstraintRegion.cpp \
    Source/backend/Atlas.cpp \
    Source/backend/AtlasBuilder.cpp \
    Source/backend/AtlasNode.cpp \
    Source/backend/Atom.cpp \
    Source/backend/CartesianRealizer.cpp \
    Source/backend/CayleyParameterization.cpp \
    Source/backend/CayleyPoint.cpp \
    Source/backend/CgMarker.cpp \
    Source/backend/ConstraintCheck.cpp \
    Source/backend/ConvexChart.cpp \
    Source/backend/EventPointNode.cpp \
    Source/main.cpp \
    Source/backend/MolecularUnit.cpp \
    Source/backend/Orientation.cpp \
    Source/backend/PredefinedInteractions.cpp \
    Source/backend/RegionFlipNode.cpp \
    Source/backend/SaveLoader.cpp \
    Source/backend/Settings.cpp \
    Source/backend/Statistics.cpp \
    Source/backend/Stree.cpp \
    Source/backend/Thread_BackEnd.cpp \
    Source/backend/Utils.cpp \
    Source/gui_Qt/AdvancedOptionWindow.cpp \
    Source/gui_Qt/AtlasViewWidget.cpp \
    Source/gui_Qt/BondViewWidget.cpp \
    Source/gui_Qt/InputWindow.cpp \
    Source/gui_Qt/Mesh3D.cpp \
    Source/gui_Qt/NodeParentChildernViewWidget.cpp \
    Source/gui_Qt/SharedDataGUI.cpp \
    Source/gui_Qt/SweepViewWidget.cpp \
    Source/gui_Qt/CayleySpaceViewWidget.cpp \
    Source/gui_Qt/CustomSlider.cpp \
    Source/gui_Qt/Camera.cpp \
    Source/gui_Qt/Renderer.cpp \
    Source/gui_Qt/CayleyPointRenderingWrapper.cpp \
    Source/gui_Qt/RealisationViewWidget.cpp \
    Source/gui_Qt/MainWindow.cpp \
    Source/gui_Qt/RealizationViewWidgetCornerVersion.cpp \
    Source/gui_Qt/ConstraintSelectionWindow.cpp \
    Source/gui_Qt/ConstraintSelectionMoleculeViewWidget.cpp \
    Source/gui_Qt/ConstraintSelectionBondViewWidget.cpp \
    Source/gui_Qt/MessageWindow.cpp \
    Source/gui_Qt/PathfindingBetweenD0NodesViewWindow.cpp \
    Source/backend/PathfindingReturnTuple.cpp \
    Source/gui_Qt/PathfindingViewWidget3DPart.cpp \
    Source/backend/pathfinder.cpp \

HEADERS  += \
    Source/backend/ActiveConstraintGraph.h \
    Source/backend/ActiveConstraintRegion.h \
    Source/backend/Atlas.h \
    Source/backend/AtlasBuilder.h \
    Source/backend/AtlasNode.h \
    Source/backend/Atom.h \
    Source/backend/CartesianRealizer.h \
    Source/backend/CayleyParameterization.h \
    Source/backend/CayleyPoint.h \
    Source/backend/CgMarker.h \
    Source/backend/ConstraintCheck.h \
    Source/backend/ConvexChart.h \
    Source/backend/EventPointNode.h \
    Source/backend/miniball.h \
    Source/backend/MolecularUnit.h \
    Source/backend/Orientation.h \
    Source/backend/PredefinedInteractions.h \
    Source/backend/readIn.h \
    Source/backend/RegionFlipNode.h \
    Source/backend/SaveLoader.h \
    Source/backend/Settings.h \
    Source/backend/Statistics.h \
    Source/backend/Stree.h \
    Source/backend/Thread.h \
    Source/backend/Thread_BackEnd.h \
    Source/backend/ThreadShare.h \
    Source/backend/Utils.h \
    Source/gui_Qt/AdvancedOptionWindow.h \
    Source/gui_Qt/AtlasViewWidget.h \
    Source/gui_Qt/BondViewWidget.h \
    Source/gui_Qt/InputWindow.h \
    Source/gui_Qt/Mesh3D.h \
    Source/gui_Qt/NodeParentChildernViewWidget.h \
    Source/gui_Qt/SharedDataGUI.h \
    Source/gui_Qt/SweepViewWidget.h \
    Source/gui_Qt/CayleySpaceViewWidget.h \
    Source/gui_Qt/CustomSlider.h \
    Source/gui_Qt/Camera.h \
    Source/gui_Qt/Renderer.h \
    Source/gui_Qt/CayleyPointRenderingWrapper.h \
    Source/gui_Qt/RealisationViewWidget.h \
    Source/gui_Qt/MainWindow.h \
    Source/gui_Qt/RealizationViewWidgetCornerVersion.h \
    Source/gui_Qt/ConstraintSelectionWindow.h \
    Source/gui_Qt/ConstraintSelectionMoleculeViewWidget.h \
    Source/gui_Qt/ConstraintSelectionBondViewWidget.h \
    Source/gui_Qt/MessageWindow.h \
    Source/gui_Qt/PathfindingBetweenD0NodesViewWindow.h \
    Source/backend/PathfindingReturnTuple.h \
    Source/gui_Qt/PathfindingViewWidget3DPart.h \
    Source/backend/pathfinder.h \




