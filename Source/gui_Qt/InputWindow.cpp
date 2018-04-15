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
#include "InputWindow.h"


InputWindow::InputWindow()
{
    this->setWindowTitle(tr("EASAL - Input Window"));
    mainLayout = new QVBoxLayout;

    lineEditDataForMoleculeA = new QLineEdit;
    lineEditDataForMoleculeA->setText(QString::fromStdString(Settings::MolecularUnitA::file));

    lineEditDataForMoleculeB = new QLineEdit;
    lineEditDataForMoleculeB->setText(QString::fromStdString(Settings::MolecularUnitB::file));

    lineEditPredefinedInteractions = new QLineEdit;
    lineEditPredefinedInteractions->setText(QString::fromStdString(Settings::DistanceData::file));
    lineEditDataDirectory = new QLineEdit;
    lineEditDataDirectory->setText(QString::fromStdString(Settings::Output::dataDirectory));


    //init and connect line edits with functions that send data from line edits to variables
    boundingTreshholdLowerBoundLambdaLineEdit2_1 = new QLineEdit;
    boundingTreshholdLowerBoundLambdaLineEdit2_1->setText(QString::number(Settings::Constraint::bondingLowerLambda));

    boundingTreshholdLowerBoundDeltaLineEdit2_2 = new QLineEdit;
    boundingTreshholdLowerBoundDeltaLineEdit2_2->setText(QString::number(Settings::Constraint::bondingLowerDelta));

    boundingTreshholdUpperBoundLambdaLineEdit3_1 = new QLineEdit;
    boundingTreshholdUpperBoundLambdaLineEdit3_1->setText(QString::number(Settings::Constraint::bondingUpperLambda));

    boundingTreshholdUpperBoundDeltaLineEdit3_2 = new QLineEdit;
    boundingTreshholdUpperBoundDeltaLineEdit3_2->setText(QString::number(Settings::Constraint::bondingUpperDelta));

    collisionTreshholdLowerBoundLambdaLineEdit5_1 = new QLineEdit;
    collisionTreshholdLowerBoundLambdaLineEdit5_1->setText(QString::number(Settings::Constraint::collisionLambda));

    collisionTreshholdLowerBoundDeltaLineEdit5_2 = new QLineEdit;
    collisionTreshholdLowerBoundDeltaLineEdit5_2->setText(QString::number(Settings::Constraint::collisionDelta));

    thetaLowLineEdit7_1 = new QLineEdit;
    thetaLowLineEdit7_1->setText(QString::number(Settings::Constraint::angleLow));

    thetaHighLineEdit7_2 = new QLineEdit;
    thetaHighLineEdit7_2->setText(QString::number(Settings::Constraint::angleHigh));

    lineEditStepSize = new QLineEdit;
    lineEditStepSize->setText(QString::number(Settings::Sampling::stepSize));

    buttonBrowse1 = new QPushButton(tr("Browse"));
    connect(buttonBrowse1, SIGNAL (pressed()), this, SLOT (browse1()));
    buttonSetData1 = new QPushButton(tr("Set Data"));
    buttonBrowse2 = new QPushButton(tr("Browse"));
    connect(buttonBrowse2, SIGNAL (pressed()), this, SLOT (browse2()));
    buttonSetData2 = new QPushButton(tr("Set Data"));
    buttonBrowse3 = new QPushButton(tr("Browse"));
    connect(buttonBrowse3, SIGNAL (pressed()), this, SLOT (browse3()));
    buttonSetData3 = new QPushButton(tr("Set Data"));
    buttonBrowse4 = new QPushButton(tr("Browse"));
    connect(buttonBrowse4, SIGNAL (pressed()), this, SLOT (browse4()));

    advancedOptionsButton = new QPushButton(tr("Adanced Options"));
    connect(advancedOptionsButton, SIGNAL (pressed()), this, SLOT (advancedOptions()));

    buttonAccept = new QPushButton(tr("Accept"));
    connect(buttonAccept, SIGNAL (pressed()), this, SLOT (accept()));
    buttonExit = new QPushButton(tr("Exit"));
    connect(buttonExit, SIGNAL (pressed()), this, SLOT (exit()));

    fileFormatString = tr("Images (*.txt *.pdb)");

    setDataGroupBox();
    setBoundsGroupBox();
    setStepSizeGroupBox();
    setAcceptExitGroupBox();

    mainLayout->addWidget(data);
    mainLayout->addWidget(bounds);
    mainLayout->addWidget(stepSize);
    mainLayout->addWidget(acceptExit);

    setLayout(mainLayout);

    advancedOptionsWindowInstance = new AdvancedOptionWindow;

}
void InputWindow::setDataGroupBox()
{
    data = new QGroupBox;
    QGridLayout *layout = new QGridLayout;
    QLabel *label1 = new QLabel(tr("Data for Molecule A:"));
    QLabel *label2 = new QLabel(tr("Data for Molecule B:"));
    QLabel *label3 = new QLabel(tr("Predefined Interactions:"));
    QLabel *label4 = new QLabel(tr("Data Directory:"));

    layout->addWidget(label1,0,0);
    layout->addWidget(lineEditDataForMoleculeA,0,1);
    layout->addWidget(buttonBrowse1,0,2);
    layout->addWidget(buttonSetData1,0,3);

    layout->addWidget(label2,1,0);
    layout->addWidget(lineEditDataForMoleculeB,1,1);
    layout->addWidget(buttonBrowse2,1,2);
    layout->addWidget(buttonSetData2,1,3);

    layout->addWidget(label3,2,0);
    layout->addWidget(lineEditPredefinedInteractions,2,1);
    layout->addWidget(buttonBrowse3,2,2);
    layout->addWidget(buttonSetData3,2,3);

    layout->addWidget(label4,3,0);
    layout->addWidget(lineEditDataDirectory,3,1);
    layout->addWidget(buttonBrowse4,3,2);

    data->setLayout(layout);
}
void InputWindow::setBoundsGroupBox()
{
    bounds = new QGroupBox;
    QGridLayout *layout = new QGridLayout;

    QLabel *label1_1 = new QLabel(tr("Bonding Treshhold"));
    QLabel *label1_2 = new QLabel(tr("lambda"));
    QLabel *label1_3 = new QLabel(tr("delta"));

    QLabel *label2_1 = new QLabel(tr("Lower Bound"));
    QLabel *label2_2 = new QLabel(tr("*(ri+rj)+"));

    QLabel *label3_1 = new QLabel(tr("Upper Bound"));
    QLabel *label3_2 = new QLabel(tr("*(ri+rj)+"));

    QLabel *label4 = new QLabel(tr("Collision Treshold"));

    QLabel *label5_1 = new QLabel(tr("Lower Bound"));
    QLabel *label5_2 = new QLabel(tr("*(ri+rj)+"));

    QLabel *label6 = new QLabel(tr("Inter-helical Angle Constrain"));

    QLabel *label7_1 = new QLabel(tr("theta_low"));
    QLabel *label7_2 = new QLabel(tr("theta_high"));

    layout->addWidget(label1_1,0,0,1,2);
    layout->addWidget(label1_2,0,2);
    layout->addWidget(label1_3,0,4);

    layout->addWidget(label2_1,1,1);
    layout->addWidget(boundingTreshholdLowerBoundLambdaLineEdit2_1,1,2);
    layout->addWidget(label2_2,1,3);
    layout->addWidget(boundingTreshholdLowerBoundDeltaLineEdit2_2,1,4);

    layout->addWidget(label3_1,2,1);
    layout->addWidget(boundingTreshholdUpperBoundLambdaLineEdit3_1,2,2);
    layout->addWidget(label3_2,2,3);
    layout->addWidget(boundingTreshholdUpperBoundDeltaLineEdit3_2,2,4);

    layout->addWidget(label4,3,0,1,2);

    layout->addWidget(label5_1,4,1);
    layout->addWidget(collisionTreshholdLowerBoundLambdaLineEdit5_1,4,2);
    layout->addWidget(label5_2,4,3);
    layout->addWidget(collisionTreshholdLowerBoundDeltaLineEdit5_2,4,4);

    layout->addWidget(label6,5,0,1,2);

    layout->addWidget(label7_1,6,1);
    layout->addWidget(thetaLowLineEdit7_1,6,2);
    layout->addWidget(label7_2,6,3);
    layout->addWidget(thetaHighLineEdit7_2,6,4);

    bounds->setLayout(layout);
}
void InputWindow::setStepSizeGroupBox()
{
    stepSize= new QGroupBox;
    QGridLayout *layout = new QGridLayout;

    QLabel *labelStepSize = new QLabel(tr("Step Size"));

    layout->addWidget(labelStepSize,0,0);
    layout->addWidget(lineEditStepSize,0,1);
    layout->addWidget(advancedOptionsButton,0,2);

    stepSize->setLayout(layout);
}
void InputWindow::setAcceptExitGroupBox()
{
    acceptExit= new QGroupBox;
    QGridLayout *layout = new QGridLayout;

    layout->addWidget(buttonAccept,0,0);
    layout->addWidget(buttonExit,0,1);

    acceptExit->setLayout(layout);
}

bool InputWindow::isInputCorrect()
{
    numberOfIncorrectInputs = 0;
    warningWindow.clearText();

    //Check files
    if(!doesFileExist(lineEditDataForMoleculeA->text().toStdString()))
    {
        numberOfIncorrectInputs++;
        warningWindow.addLineToMessage("File " + lineEditDataForMoleculeA->text().toStdString() + " doesn't exist");
    }
    if(!doesFileExist(lineEditDataForMoleculeB->text().toStdString()))
    {
        numberOfIncorrectInputs++;
        warningWindow.addLineToMessage("File " + lineEditDataForMoleculeB->text().toStdString() + " doesn't exist");
    }
//    if(!doesFileExist(lineEditPredefinedInteractions->text().toStdString()))
//    {
//        numberOfIncorrectInputs++;
//        warningWindow.addLineToMessage("File " + lineEditPredefinedInteractions->text().toStdString() + " doesn't exist");
//    }
    if(!doesDirectoryExist(lineEditDataDirectory->text().toStdString()))
    {
        QDir dir(lineEditDataDirectory->text());
        dir.mkdir(".");

        //numberOfIncorrectInputs++;
        //warningWindow.addLineToMessage("Directory " + lineEditDataDirectory->text().toStdString() + " doesn't exist");
    }

    //Check numbers
    if(boundingTreshholdLowerBoundLambdaLineEdit2_1->text().toDouble() == 0  && boundingTreshholdLowerBoundLambdaLineEdit2_1->text()!="0")
    {
        numberOfIncorrectInputs++;
        warningWindow.addLineToMessage(QString("Bonding lower lambda is not number"));
    }
    if(boundingTreshholdLowerBoundLambdaLineEdit2_1->text().toDouble()<0)
    {
        numberOfIncorrectInputs++;
        warningWindow.addLineToMessage(QString("Bonding lower lambda is negative"));
    }

    if(boundingTreshholdUpperBoundLambdaLineEdit3_1->text().toDouble() == 0  && boundingTreshholdUpperBoundLambdaLineEdit3_1->text()!="0")
    {
        numberOfIncorrectInputs++;
        warningWindow.addLineToMessage(QString("Bonding upper lambda is not number"));
    }
    if(boundingTreshholdUpperBoundLambdaLineEdit3_1->text().toDouble()<0)
    {
        numberOfIncorrectInputs++;
        warningWindow.addLineToMessage(QString("Bonding upper lambda is negative"));
    }

    if(boundingTreshholdLowerBoundDeltaLineEdit2_2->text().toDouble() == 0  && boundingTreshholdLowerBoundDeltaLineEdit2_2->text()!="0")
    {
        numberOfIncorrectInputs++;
        warningWindow.addLineToMessage(QString("Bonding lower delta is not number"));
    }
    if(boundingTreshholdLowerBoundDeltaLineEdit2_2->text().toDouble()<0)
    {
        numberOfIncorrectInputs++;
        warningWindow.addLineToMessage(QString("Bonding lower delta is negative"));
    }

    if(boundingTreshholdUpperBoundDeltaLineEdit3_2->text().toDouble() == 0  && boundingTreshholdUpperBoundDeltaLineEdit3_2->text()!="0")
    {
        numberOfIncorrectInputs++;
        warningWindow.addLineToMessage(QString("Bonding upper delta is not number"));
    }
    if(boundingTreshholdUpperBoundDeltaLineEdit3_2->text().toDouble()<0)
    {
        numberOfIncorrectInputs++;
        warningWindow.addLineToMessage(QString("Bonding upper delta is negative"));
    }

    if(collisionTreshholdLowerBoundLambdaLineEdit5_1->text().toDouble() == 0  && collisionTreshholdLowerBoundLambdaLineEdit5_1->text()!="0")
    {
        numberOfIncorrectInputs++;
        warningWindow.addLineToMessage(QString("Collision lambda is not number"));
    }
    if(collisionTreshholdLowerBoundLambdaLineEdit5_1->text().toDouble()<0)
    {
        numberOfIncorrectInputs++;
        warningWindow.addLineToMessage(QString("Collision lambda is negative"));
    }

    if(collisionTreshholdLowerBoundDeltaLineEdit5_2->text().toDouble() == 0  && collisionTreshholdLowerBoundDeltaLineEdit5_2->text()!="0")
    {
        numberOfIncorrectInputs++;
        warningWindow.addLineToMessage(QString("Collision delta is not number"));
    }
    if(collisionTreshholdLowerBoundDeltaLineEdit5_2->text().toDouble()<0)
    {
        numberOfIncorrectInputs++;
        warningWindow.addLineToMessage(QString("Collision delta is negative"));
    }

    if(thetaLowLineEdit7_1->text().toDouble() == 0  && thetaLowLineEdit7_1->text()!="0")
    {
        numberOfIncorrectInputs++;
        warningWindow.addLineToMessage(QString("Theta low is not number"));
    }
    if(thetaLowLineEdit7_1->text().toDouble()<0)
    {
        numberOfIncorrectInputs++;
        warningWindow.addLineToMessage(QString("Theta low is negative"));
    }

    if(thetaHighLineEdit7_2->text().toDouble() == 0  && thetaHighLineEdit7_2->text()!="0")
    {
        numberOfIncorrectInputs++;
        warningWindow.addLineToMessage(QString("Theta high is not number"));
    }
    if(thetaHighLineEdit7_2->text().toDouble()<0)
    {
        numberOfIncorrectInputs++;
        warningWindow.addLineToMessage(QString("Theta high is negative"));
    }

    if(lineEditStepSize->text().toDouble() == 0  && lineEditStepSize->text()!="0")
    {
        numberOfIncorrectInputs++;
        warningWindow.addLineToMessage(QString("Step size is not number"));
    }
    if(lineEditStepSize->text().toDouble()<=0)
    {
        numberOfIncorrectInputs++;
        warningWindow.addLineToMessage(QString("Step size is equal or less than zero"));
    }

    if(numberOfIncorrectInputs == 0)
    {
        return true;
    }
    else
    {
        return false;
    }

}

bool InputWindow::doesFileExist(string fileName)
{
    if(FILE *file = fopen(fileName.c_str(),"r"))
    {
        fclose(file);
        return true;
    }
    else
    {
        return false;
    }
}

bool InputWindow::doesDirectoryExist(string directoryName)
{
    return QDir(QString::fromStdString(directoryName)).exists();
}
void InputWindow::exit()
{
    this->close();
}
void InputWindow::accept()
{
    bool doesNode0Exists = doesFileExist(lineEditDataDirectory->text().toStdString()+"/"+"Node0.txt");
    bool doesSettingsExist = doesFileExist(lineEditDataDirectory->text().toStdString()+"/"+"settings.ini");
    bool useNewSettings = true;
    if(isInputCorrect())
    {
        if(doesNode0Exists && doesSettingsExist)
        {
            QMessageBox useOldSettingsMessageBox;
            useOldSettingsMessageBox.setWindowTitle("Warning");
            useOldSettingsMessageBox.setText("A data directory with this name already exists and\ncontains previous nodes and settings.\n\nWould you like to load the old settings and nodes (yes)?\n(OR overwrite and use the new settings (no)).");
            useOldSettingsMessageBox.addButton(QMessageBox::Yes);
            useOldSettingsMessageBox.addButton(QMessageBox::No);
            if(useOldSettingsMessageBox.exec() == QMessageBox::Yes)
            {
                Settings::load(string(lineEditDataDirectory->text().toStdString()+"/"+"settings.ini").c_str());
                Settings::Sampling::runSample = false;
                Settings::Sampling::runMode = Stopped;
                useNewSettings = false;
            }
            else
            {
                //Delete old files
                QDir directoryQt(lineEditDataDirectory->text());
                directoryQt.setNameFilters(QStringList()<<"*.*");
                directoryQt.setFilter(QDir::Files);
                foreach (QString dirFile, directoryQt.entryList()) {
                    directoryQt.remove(dirFile);
                }
                useNewSettings = true;
            }
        }
        if(useNewSettings)
        {
            Settings::MolecularUnitA::file=lineEditDataForMoleculeA->text().toStdString();
            Settings::MolecularUnitB::file=lineEditDataForMoleculeB->text().toStdString();
            Settings::DistanceData::file=lineEditPredefinedInteractions->text().toStdString();
            Settings::Output::dataDirectory=lineEditDataDirectory->text().toStdString()+"/";

            Settings::Constraint::bondingLowerLambda=boundingTreshholdLowerBoundLambdaLineEdit2_1->text().toDouble();
            Settings::Constraint::bondingUpperLambda=boundingTreshholdUpperBoundLambdaLineEdit3_1->text().toDouble();
            Settings::Constraint::bondingLowerDelta=boundingTreshholdLowerBoundDeltaLineEdit2_2->text().toDouble();
            Settings::Constraint::bondingUpperDelta=boundingTreshholdUpperBoundDeltaLineEdit3_2->text().toDouble();

            Settings::Constraint::collisionLambda=collisionTreshholdLowerBoundLambdaLineEdit5_1->text().toDouble();
            Settings::Constraint::collisionDelta=collisionTreshholdLowerBoundDeltaLineEdit5_2->text().toDouble();

            Settings::Constraint::angleLow=thetaLowLineEdit7_1->text().toDouble();
            Settings::Constraint::angleHigh=thetaHighLineEdit7_2->text().toDouble();

            Settings::Sampling::stepSize=lineEditStepSize->text().toDouble();
            Settings::save(string(lineEditDataDirectory->text().toStdString()+"/"+"settings.ini").c_str());

            Settings::Sampling::runMode = ForestSampleAS;
        }

        this->close();
    }
    else
    {
        warningWindow.exec();
    }

}
void InputWindow::browse1()
{
    QString fileName =QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "",
                                                    fileFormatString );
    lineEditDataForMoleculeA->setText(fileName);
}
void InputWindow::browse2()
{
    QString fileName =QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "",
                                                    fileFormatString );
    lineEditDataForMoleculeB->setText(fileName);
}
void InputWindow::browse3()
{
    QString fileName =QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "",
                                                    fileFormatString );
    lineEditPredefinedInteractions->setText(fileName);
}
void InputWindow::browse4()
{
    QString fileName =QFileDialog::getExistingDirectory(this, tr("Select directory"));
    lineEditDataDirectory->setText(fileName);
}
void InputWindow::advancedOptions()
{

    advancedOptionsWindowInstance->exec();
}
