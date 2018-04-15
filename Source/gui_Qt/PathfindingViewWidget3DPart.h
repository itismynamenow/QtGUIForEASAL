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
#ifndef PATHFINDINGVIEWWIDGET3DPART_H
#define PATHFINDINGVIEWWIDGET3DPART_H

#include <QtWidgets>

#include <iostream>
#include <vector>
#include <cmath>
#include <unordered_map>
#include <algorithm>

#include <Source/backend/AtlasNode.h>
#include <Source/backend/Atlas.h>
#include <Source/backend/Settings.h>
#include <Source/backend/CayleyPoint.h>
#include <Source/backend/Utils.h>
#include <Source/backend/SaveLoader.h>
#include <Source/backend/CayleyPoint.h>
#include <Source/backend/PathfindingReturnTuple.h>
#include <Source/gui_Qt/Mesh3D.h>
#include <Source/gui_Qt/BondViewWidget.h>
#include <Source/gui_Qt/SharedDataGUI.h>
#include <Source/gui_Qt/CustomSlider.h>
#include <Source/gui_Qt/CayleyPointRenderingWrapper.h>
#include <Source/gui_Qt/Camera.h>
#include <Source/gui_Qt/Renderer.h>

using namespace std;

class PathfindingViewWidget3DPart: public QOpenGLWidget
{
    Q_OBJECT
public:
    PathfindingViewWidget3DPart(SharedDataGUI *sharedData);
    void setData(Atlas *atlas, SaveLoader *saveLoader,MolecularUnit *molecularUnitA, MolecularUnit *molecularUnitB);
    void setPath(vector<PathfindingReturnTuple> path);
private slots:
    void selectedMoleculeOnTopCheckBoxSlot();
private:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    void updateTransformationMatrices();
    void updateBondData();
    void clearTarnsformationMatrices();
    void keyPressEvent(QKeyEvent *keyEvent) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *wheelEvent) Q_DECL_OVERRIDE;

    //[ COMPONENTS]
    Camera camera;
    Renderer renderer;
    CustomSlider slider;
    QGridLayout mainGridLayout;
    QCheckBox transparencyCheckBox;
    QCheckBox selectedMoleculeOnTopCheckBox;
    BondViewWidget bondViewWidget;
//    BondViewWidget bondViewWidget;
    //[~COMPONENTS]
    //[ DEPENDENCIES]
    SaveLoader *saveLoader;
    Atlas *atlas;
    SharedDataGUI *sharedData;
    MolecularUnit *molecularUnitA;
    MolecularUnit *molecularUnitB;
    //[~DEPENDENCIES]
    //[ TIMER]
    /** @brief Timer that triggers screen updates once each m_updateTimeInMS*/
    QTimer timer;
    double updateTimeInMS;
    //[ TIMER]
    //[ UNIFORMS]
    vector<QMatrix4x4> translationMatrices;
    vector<QMatrix4x4> rotationMatrices;
    vector<QMatrix4x4> scaleMatrices;
    vector<QVector4D> colorVectors;

    QMatrix4x4 cameraMatrix;
    QVector3D lightPositionVector;
    //[~UNIFORMS]
    //[ MESHES3D]
    Mesh3D sphere;
    Mesh3D cylinder;
    //[~MESHES3D]
    //[ FLAGS]
    bool isPathSet = false;
    //[~FLAGS]
    //[ INDICIES]
    int currentAtlasNodeID = -1;
    int currentOrientationID = -1;
    int selectedNodeID = -1;
    //[~INDICIES]
    //[ VARS]
    vector<Atom*>atomsA;
    vector<vector<vector<Atom*>>> groupedAtomsB;///< 1st index: correspondent to atlas node ID, 2nd index: correspondent to molecule, 3rd index: ID of *atom
    unordered_map<int,int> atlasNodeIDToGroupedAtomsBFirstIndex;///< maps an atlas node ID into correspondent first index of groupedAtomsB (that entry will contain all "molecules" related the atlas node)
    vector<int> nodeIDs;
    int totalNumberOfMoleculesB = -1;
    //[ VARS]
    //[ CONSTANTS]
    double transperancyStrength;
    //[~CONSTANTS]

};

#endif // PATHFINDINGVIEWWIDGET3DPART_H
