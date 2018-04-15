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
#ifndef CONSTRAINTSELECTIONWINDOW_H
#define CONSTRAINTSELECTIONWINDOW_H
#include <QtWidgets>

#include <iostream>

#include "Source/backend/MolecularUnit.h"
#include "Source/backend/Atlas.h"
#include "Source/backend/AtlasBuilder.h"
#include "Source/gui_Qt/ConstraintSelectionMoleculeViewWidget.h"
#include "Source/gui_Qt/ConstraintSelectionBondViewWidget.h"


class ConstraintSelectionWindow: public QDialog
{
    Q_OBJECT
public:
    ConstraintSelectionWindow();

    void setData(Atlas *atlas, MolecularUnit *molecularUnitA, MolecularUnit *molecularUnitB);///<setData must be called before class is used

private slots:
    void updateConnectionData();
private:
    void setLayouts();

    //[ LAYOUT]
    QGridLayout mainGridLayout;
    QGridLayout constraintSelectionPanelGridLayout;
    QComboBox comboBoxesA[6];
    QComboBox comboBoxesB[6];
    QCheckBox connectCheckBox[6];
    ConnectionData connectionData;
    QPushButton acceptButton;
    QPushButton loadButton;
    QPushButton cancelButton;
    QLabel preloadNodeNumberLable;
    QLineEdit preloadNodeNumberLineEdit;
    QLabel doubleDashLable;
    QLabel nodeValuesRangeLable;
    QWidget constraintSelectionPanelWidget;///<We use this widget only for layout purposes, this widgets holds all dropdowns and reladeted stuff

    ConstraintSelectionMoleculeViewWidget constraintSelectionMoleculeViewWidget;
    ConstraintSelectionBondViewWidget constraintSelectionBondViewWidget;
    //[~LAYOUT]
    //[ DEPENDENCIES]
    Atlas *atlas;
    MolecularUnit *molecularUnitA;
    MolecularUnit *molecularUnitB;
    //[~DEPENDENCIES]
};

#endif // CONSTRAINTSELECTIONWINDOW_H
