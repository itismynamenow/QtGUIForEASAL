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
#include "AdvancedOptionWindow.h"

AdvancedOptionWindow::AdvancedOptionWindow()
{
    this->setWindowTitle(tr("Advanced Options"));
    mainLayout = new QVBoxLayout;

    buttonAccept = new QPushButton(tr("Accept"));
    connect(buttonAccept, SIGNAL (pressed()), this, SLOT (accept()));
    buttonCancel = new QPushButton(tr("Cancel"));
    connect(buttonCancel , SIGNAL (pressed()), this, SLOT (cancel()));

    setCheckBoxGroup();
    setlineEditGroup();
    setAcceptExitGroup();

    mainLayout->addWidget(checkBoxGroup);
    mainLayout->addWidget(lineEditGroup);
    mainLayout->addWidget(acceptExitGroup);

    setLayout(mainLayout);

}
void AdvancedOptionWindow::setCheckBoxGroup()
{
    checkBoxGroup = new QGroupBox;
    checkBoxGroupLayout = new QGridLayout;

    reverseWitnessCheckBox = new QCheckBox;
    dynamicStepSizeAmongCheckBox = new QCheckBox;
    useParticipatingAtomsZDistanceCheckBox = new QCheckBox;
    ShortRangeSamplingCheckBox = new QCheckBox;
    wholeCollisionsCheckBox = new QCheckBox;
    dynamicStepSizeWithinCheckBox = new QCheckBox;
    fourDRootNodeCheckBox = new QCheckBox;
    reversePairDumbbellsCheckBox = new QCheckBox;

    reverseWitnessLable = new QLabel(tr("Reverse Witness"));
    dynamicStepSizeAmongLable = new QLabel(tr("Dynamic Step Size Among"));
    useParticipatingAtomsZDistanceLable = new QLabel(tr("Use Participating Atoms Z Distance"));
    shortRangeSamplingLable = new QLabel(tr("Short Range Smapling"));
    wholeCollisionsLable = new QLabel(tr("Whole Collisions"));
    dynamicStepSizeWithinLable = new QLabel(tr("Dynamic Step Size Within"));
    participatingAtomsZDistanceLable = new QLabel(tr("Participating Atoms Z Distance"));
    fourDRootNodeLable = new QLabel(tr("4D Root Node"));
    reversePairDumbbellsLable = new QLabel(tr("Reverse Pair Dumbbells"));

    participatingAtomsZDistanceLineEdit = new QLineEdit;

    checkBoxGroupLayout->addWidget(reverseWitnessCheckBox,0,0);
    checkBoxGroupLayout->addWidget(reverseWitnessLable,0,1);
    checkBoxGroupLayout->addWidget(wholeCollisionsCheckBox,0,2);
    checkBoxGroupLayout->addWidget(wholeCollisionsLable,0,3);
    checkBoxGroupLayout->addWidget(fourDRootNodeCheckBox,0,4);
    checkBoxGroupLayout->addWidget(fourDRootNodeLable,0,5);

    checkBoxGroupLayout->addWidget(dynamicStepSizeAmongCheckBox,1,0);
    checkBoxGroupLayout->addWidget(dynamicStepSizeAmongLable,1,1);
    checkBoxGroupLayout->addWidget(dynamicStepSizeWithinCheckBox,1,2);
    checkBoxGroupLayout->addWidget(dynamicStepSizeWithinLable,1,3);
    checkBoxGroupLayout->addWidget(reversePairDumbbellsCheckBox,1,4);
    checkBoxGroupLayout->addWidget(reversePairDumbbellsLable,1,5);

    checkBoxGroupLayout->addWidget(useParticipatingAtomsZDistanceCheckBox,2,0);
    checkBoxGroupLayout->addWidget(useParticipatingAtomsZDistanceLable,2,1);
    checkBoxGroupLayout->addWidget(participatingAtomsZDistanceLable,2,2,1,2);
    checkBoxGroupLayout->addWidget(participatingAtomsZDistanceLineEdit,2,4,1,2);

    checkBoxGroupLayout->addWidget(ShortRangeSamplingCheckBox,3,0);
    checkBoxGroupLayout->addWidget(shortRangeSamplingLable,3,1);

    checkBoxGroup->setLayout(checkBoxGroupLayout);
}
void AdvancedOptionWindow::setlineEditGroup()
{
    lineEditGroup = new QGroupBox;
    lineEditGroupLayout = new QGridLayout;

    participatingAtomIndexLowLable = new QLabel(tr("Participating Atom Index Low"));
    initial4DContactSeperationLowLable = new QLabel(tr("Initial 4D Contact Seperation Low"));
    savePointsFrequencyLable = new QLabel(tr("Save Points Frequency"));
    participatingAtomIndexHighLable = new QLabel(tr("Participating Atom Index High"));
    initial4DContactSeperationHighLable = new QLabel(tr("Initial 4D Contact Seperation High"));

    participatingAtomIndexLowLineEdit = new QLineEdit;
    initial4DContactSeperationLowLineEdit = new QLineEdit;
    savePointsFrequencyLineEdit = new QLineEdit;
    participatingAtomIndexHighLineEdit = new QLineEdit;
    initial4DContactSeperationHighLineEdit = new QLineEdit;

    participatingAtomIndexLowLineEdit->setText(QString::number(Settings::RootNodeCreation::participatingAtomIndex_low));
    participatingAtomIndexHighLineEdit->setText(QString::number(Settings::RootNodeCreation::participatingAtomIndex_high));

    initial4DContactSeperationLowLineEdit->setText(QString::number(Settings::RootNodeCreation::initial4DContactSeparation_low));
    initial4DContactSeperationHighLineEdit->setText(QString::number(Settings::RootNodeCreation::initial4DContactSeparation_high));

    savePointsFrequencyLineEdit->setText(QString::number(Settings::Saving::savePointsFrequency));

    lineEditGroupLayout->addWidget(participatingAtomIndexLowLable,0,0);
    lineEditGroupLayout->addWidget(participatingAtomIndexLowLineEdit,0,1);
    lineEditGroupLayout->addWidget(participatingAtomIndexHighLable,0,2);
    lineEditGroupLayout->addWidget(participatingAtomIndexHighLineEdit,0,3);

    lineEditGroupLayout->addWidget(initial4DContactSeperationLowLable,1,0);
    lineEditGroupLayout->addWidget(initial4DContactSeperationLowLineEdit,1,1);
    lineEditGroupLayout->addWidget(initial4DContactSeperationHighLable,1,2);
    lineEditGroupLayout->addWidget(initial4DContactSeperationHighLineEdit,1,3);

    lineEditGroupLayout->addWidget(savePointsFrequencyLable,2,0);
    lineEditGroupLayout->addWidget(savePointsFrequencyLineEdit,2,1);

    lineEditGroup->setLayout(lineEditGroupLayout);
}
void AdvancedOptionWindow::setAcceptExitGroup()
{
    acceptExitGroup = new QGroupBox;
    acceptExitGroupLayout = new QGridLayout;

    acceptExitGroupLayout->addWidget(buttonAccept,0,0);
    acceptExitGroupLayout->addWidget(buttonCancel ,0,1);

    acceptExitGroup->setLayout(acceptExitGroupLayout);
}
void AdvancedOptionWindow::accept()
{
    Settings::General::reverseWitness = reverseWitnessCheckBox->isChecked();
    Settings::Sampling::dynamicStepSizeAmong = dynamicStepSizeAmongCheckBox->isChecked();
    Settings::RootNodeCreation::useParticipatingAtomZDistance = useParticipatingAtomsZDistanceCheckBox->isChecked();
    Settings::Sampling::short_range_sampling = ShortRangeSamplingCheckBox->isChecked();
    Settings::Constraint::wholeCollision = wholeCollisionsCheckBox->isChecked();
    if(fourDRootNodeCheckBox->isChecked())
    {
        Settings::RootNodeCreation::dimension_of_rootNodes = 4;
    }
    else
    {
        Settings::RootNodeCreation::dimension_of_rootNodes = 5;
    }
    Settings::RootNodeCreation::reversePairDumbbells = reversePairDumbbellsCheckBox->isChecked();

    Settings::RootNodeCreation::participatingAtomIndex_low = participatingAtomIndexLowLineEdit->text().toInt();
    Settings::RootNodeCreation::participatingAtomIndex_high = participatingAtomIndexHighLineEdit->text().toInt();

    Settings::RootNodeCreation::initial4DContactSeparation_low = initial4DContactSeperationLowLineEdit->text().toDouble();
    Settings::RootNodeCreation::initial4DContactSeparation_high = initial4DContactSeperationHighLineEdit->text().toDouble();

    Settings::Saving::savePointsFrequency = savePointsFrequencyLineEdit->text().toInt();

    this->close();
}
void AdvancedOptionWindow::cancel()
{
    this->close();
}

