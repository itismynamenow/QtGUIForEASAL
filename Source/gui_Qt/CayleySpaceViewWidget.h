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
#ifndef CAYLEYSPACEVIEWWIDGET_H
#define CAYLEYSPACEVIEWWIDGET_H
#include <QOpenGLWidget>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QOpenGLShaderProgram>
#include <QComboBox>
#include <QLabel>
#include <QOpenGLBuffer>
#include <QOpenGLContext>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFramebufferObject>
#include <QGridLayout>
#include <QTimer>

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

#include <Source/backend/AtlasNode.h>
#include <Source/backend/Atlas.h>
#include <Source/backend/Settings.h>
#include <Source/backend/CayleyPoint.h>
#include <Source/backend/Utils.h>
#include <Source/backend/SaveLoader.h>
#include <Source/backend/CayleyPoint.h>
#include <Source/gui_Qt/Mesh3D.h>
#include <Source/gui_Qt/BondViewWidget.h>
#include <Source/gui_Qt/SharedDataGUI.h>
#include <Source/gui_Qt/CustomSlider.h>
#include <Source/gui_Qt/CayleyPointRenderingWrapper.h>
#include <Source/gui_Qt/Camera.h>
#include <Source/gui_Qt/Renderer.h>

#define PI 3.14159265
//number of point types is 2 because we only have sampled and witness points
#define NUMBER_OF_POINT_TYPES 2

using namespace std;

class CayleySpaceViewWidget: public QOpenGLWidget
{
    Q_OBJECT
public:
    CayleySpaceViewWidget(SharedDataGUI *sharedData);
    void setAtlasNode(AtlasNode* atlasNode);
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    void setData(Atlas *atlas, SaveLoader *saveLoader);
private slots:
    void pointTypeSelectionComboBoxSlot();
    void boundarySelectionComboBoxSlot();

private:
    //[ COMPONENTS]
    Camera camera;
    Renderer renderer;    
    CustomSlider sliderFor5thParamter;
    CustomSlider sliderFor4thParamter;
    QLabel parameter4thLabel;
    QLabel parameter5thLabel;
    QComboBox pointTypeSelectionComboBox;
    QLabel boundaryLabel;
    QComboBox boundarySelectionComboBox;
    //[~COMPONENTS]
    //[ DEPENDENCIES]
    SaveLoader *saveLoader;
    Atlas *atlas;
    SharedDataGUI *sharedData;
    //[~DEPENDENCIES]
    //[ MOUSE]
    bool mouseWasPressed = false;
    //[~MOUSE]
    //[ TIMER]
    /** @brief Timer that triggers screen updates once each m_updateTimeInMS*/
    QTimer m_timer;
    double m_updateTimeInMS;
    //[ TIMER]
    //[ UNIFORMS]

    //For cubes
    vector<QMatrix4x4> translationMatrices[NUMBER_OF_POINT_TYPES];
    vector<QMatrix4x4> rotationMatrices[NUMBER_OF_POINT_TYPES];
    vector<QMatrix4x4> scaleMatrices[NUMBER_OF_POINT_TYPES];
    QMatrix4x4 cameraMatrix;
    vector<QVector4D> colorVectors[NUMBER_OF_POINT_TYPES];
    QVector3D lightPositionVector;

    //For axis
    vector<QMatrix4x4> translationMatricesAxis;
    vector<QMatrix4x4> rotationMatricesAxis;
    vector<QMatrix4x4> scaleMatricesAxis;
    vector<QVector4D> colorVectorsAxis;

    //[~UNIFORMS]
    //[ MESHES 3D]
    Mesh3D cube;
    Mesh3D sphere;
    Mesh3D cylinder;
    //[ MESHES 3D]
    //[ VARIABLES]
    AtlasNode* atlasNode;
    ActiveConstraintRegion *currentACR = 0;
    /** @brief these arrays of doubles hold min and max value for each cayley parameter(bond length) */
    double minBondLength[6];
    double maxBondLength[6];
    /** @brief these 3d vector folds pointers to cayley points according to 4's and 5's dimension (we need it to render point in 3d space)*/
    vector<vector<vector<CayleyPointRenderingWrapper>>>groupedCayleyPoints;
    vector<CayleyPointRenderingWrapper> witnessAndSampledPoints;
    vector<int> groupedCayleyPointsBoundaryIndices;///<holds IDs of cayleyPoints (in groupedCayleyPoints with current5thIndex and current4thIndex) that are on boundary with selected atlas node
    //[~VARIABLES]
    //[ FLAGS]
    bool atlasNodeIsSet = false;
    bool dataWasSet = false;
    bool showBadAngle = false;
    bool showCollisions = false;
    bool showValid = false;
    bool showAll = true;
    bool showNotRealizable = true;
    //[~FLAGS]
    int NONE = -1;
    //[ INDICIES]
    int currentNodeID = NONE;
    int currentSpaceID = NONE;
    int currentFlipID = NONE;
    int index5 = 4;
    int index4 = 3;
    int current5thIndex = NONE;
    int current4thIndex = NONE;
    int currentCayleyPointIndex = NONE;
    int currentBoundaryIndex = NONE;
    PointType currentCayleyPointType = PointType::SampledPoint;
    int currentCayleyPointGroupedIndex = NONE;
    //[~INDICIES]
    //[ CONSTANTS]
    double cubeScale = 0.4;
    double transperancyStrength = 0.2;
    //[~CONSTANTS]
    //[ MISC]
    QGridLayout gridLayout;
    //[~MISC]

    void keyPressEvent(QKeyEvent *keyEvent) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *wheelEvent) Q_DECL_OVERRIDE;
    void generateTransformMatricesForAxis();
    void renderAxis();
    void updateAtlasNode();
    void updateSliders();
    void updateTransformationMatrices();
    /**
     * @brief updateCurrentCayleyPointIndex: takes care of mousepicking but doesn't work on first click so after it is called check if currentCayleyPointGroupedIndex != -1
     */
    void updateCurrentCayleyPointIndex();
    void updateCameraPosition();
    void clearTarnsformationMatrices();
    void clearGroupedCayleyPoints();
    /**
     * @brief fillWitnessAndSampledPoints: fills witnessAndSampledPoints with data from ACR
     * @param ACR
     */
    void fillWitnessAndSampledPoints(ActiveConstraintRegion *ACR);
    /**
     * @brief transformCylinderAccordingToData: generates transformation matrices for cylinder that is between two points
     * @param positionStart
     * @param positionEnd
     * @param radious
     * @param translation: in out matrix
     * @param rotation: in out matrix
     * @param scale: in out matrix
     */
    void transformCylinderAccordingToData(QVector3D positionStart, QVector3D positionEnd, float radious, QMatrix4x4 *translation, QMatrix4x4 *rotation, QMatrix4x4 *scale);
    /**
     * @brief groupBy4thAnd5thCayleyParameter: fills groupedCayleyPoints with data according to 4th and 5th cayley parameter
     * @param cayleySpace
     */
    void groupBy4thAnd5thCayleyParameter(std::vector<CayleyPoint *> cayleySpace);
    /**
     * @brief updateIndecies: updates index when for example mouse was presed
     * @param meshID
     * @param pointType
     */
    void updateIndecies(int meshID, PointType pointType);
    void updateBoundary();
};

#endif // CAYLEYSPACEVIEWWIDGET_H
