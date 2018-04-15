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
#ifndef REALISATIONVIEWWIDGET_H
#define REALISATIONVIEWWIDGET_H

#include <QOpenGLWidget>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLContext>
#include <QComboBox>
#include <QLabel>
#include <QOpenGLVertexArrayObject>
#include <QCheckBox>
#include <QOpenGLExtraFunctions>
#include <QGridLayout>
#include <QTimer>

#include <iostream>
#include <cmath>

#include <Source/backend/AtlasNode.h>
#include <Source/backend/Settings.h>
#include <Source/backend/Utils.h>
#include <Source/backend/SaveLoader.h>
#include <Source/gui_Qt/Mesh3D.h>
#include <Source/gui_Qt/CayleyPointRenderingWrapper.h>
#include <Source/gui_Qt/BondViewWidget.h>
#include <Source/gui_Qt/SharedDataGUI.h>
#include <Source/gui_Qt/Camera.h>
#include <Source/gui_Qt/Renderer.h>

#define PI 3.14159265

using namespace std;


class RealisationViewWidget: public QOpenGLWidget
{
    Q_OBJECT
public:
    RealisationViewWidget(SharedDataGUI *sharedData);
    /** @brief to render some mlecules we we to load nodes using save loader
     * but save loader is not included in constructor o we need to set it in setSaveLoader */
    void setSaveLoader(SaveLoader *saveLoader);
    void setAtlasNode(AtlasNode* atlasNode);
    void setCayleyPoint(int cayleyPointID, PointType pointType = PointType::SampledPoint);
    void setMolecularUnits(MolecularUnit *molecularUnitA, MolecularUnit *molecularUnitB);
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
private slots:
    void showAllOrintationsCheckBoxSlot();
    void orientationSelectionComboBoxSlot();

private:
    //[ COMPONENTS]
    Camera camera;
    Renderer renderer;
    QCheckBox showAllOrientationsCheckBox;
    QGridLayout gridLayout;
    QComboBox orientationSelectionComboBox;
    QLabel orientationSelectionLabel;
    //[ COMPONENTS]
    //[ DEPENDENCIES]
    SaveLoader *saveLoader;
    MolecularUnit* molecularUnitA;
    MolecularUnit* molecularUnitB;
    SharedDataGUI *sharedData;
    //[~DEPENDENCIES]
    //[ UNIFORMS]

    //For cubes
    vector<QMatrix4x4> translationMatricesSphere;
    vector<QMatrix4x4> rotationMatricesSphere;
    vector<QMatrix4x4> scaleMatricesSphere;
    vector<QVector4D> colorVectorsSphere;

    //For axis
    vector<QMatrix4x4> translationMatricesCylinder;
    vector<QMatrix4x4> rotationMatricesCylinder;
    vector<QMatrix4x4> scaleMatricesCylinder;
    vector<QVector4D> colorVectorsCylinder;

    //[~UNIFORMS]
    //[ TIMER]
    /** @brief Timer that triggers screen updates once each m_updateTimeInMS*/
    QTimer m_timer;
    double m_updateTimeInMS;
    //[ TIMER]
    //[ MESHES3D]
    Mesh3D sphere;
    Mesh3D cylinder;
    //[~MESHES3D]
    //[ ATLAS NODE]
    bool atlasNodeIsSet;
    AtlasNode* atlasNode;
    QMatrix4x4 moleculeARotation;
    QMatrix4x4 moleculeBTransform;
    vector<Atom*> atomsA;
    vector<vector<Atom*>> atomsB;
    vector<std::pair<int,int>> bondSolid;
    vector<std::pair<int,int>> bondVariable;
    QVector4D colors[8];
    vector<int> flipNumberCorrespondentToID;
    ActiveConstraintRegion *currentACR = 0;
    int currentNodeID;
    int currentCayleyPointID;
    PointType currentCayleyPointType;
    int currentFlipID;
    //[~ATLAS NODE]
    void updateAtlasNode();
    void updateCayleyPoint();
    void updateTransformationMatrices();
    void updateBondData();
    void updateOrientationSelectionComboBoxValues(std::vector<Orientation*> orientations);
    void clearTarnsformationMatrices();
    void keyPressEvent(QKeyEvent *keyEvent) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *wheelEvent) Q_DECL_OVERRIDE;
    //    QOpenGLShaderProgram *m_program;
    QByteArray versionedShaderCode(const char *src);
    void transformCylinderAccordingToData(QVector3D positionStart, QVector3D positionEnd, float radious, QMatrix4x4 *translation, QMatrix4x4 *rotation, QMatrix4x4 *scale);
};

#endif // REALISATIONVIEWWIDGET_H
