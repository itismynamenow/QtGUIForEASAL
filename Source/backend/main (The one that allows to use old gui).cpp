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
// source
#include "AtlasBuilder.h"
#include "LaunchWindow.h" // Fox toolkit opening dialog window
#include "Atlas.h"


#include "Settings.h"
#include "SaveLoader.h"
#include "readIn.h"

#include "Thread.h"
#include "Thread_Viewer.h"
#include "Thread_BackEnd.h"

// external
#include <fx.h>


// standard
#include <iostream>
#include <map>

using namespace std;

// Only use once settings are loaded
void init_DistanceFinder_from_settings(PredefinedInteractions *df) {
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


// Only use once settings are loaded and you got your df from init_global_DistanceFinder
MolecularUnit* init_MolecularUnit_A_from_settings(PredefinedInteractions *df, bool nogui) {
	MolecularUnit *muA = readMolecularUnitFromFile(
			Settings::MolecularUnitA::file,
			Settings::MolecularUnitA::ignored_rows,
			Settings::MolecularUnitA::x_col,
			Settings::MolecularUnitA::y_col,
			Settings::MolecularUnitA::z_col,
			Settings::MolecularUnitA::radius_col,
			Settings::MolecularUnitA::label_col,
			Settings::MolecularUnitA::atomNo_col
			);
	muA->buildStree();
	if (Settings::General::candidate_interactions) // to just use interface atoms for detecting boundaries, new contacts etc. from now on it will ignore the rest even for collision
		muA->simplify(*df, true); // unless Settings::Constraint::wholeCollision is set to true. Then stree which holds all initial atoms will be used for collison check

	return muA;
}


// Only use once settings are loaded and you got your df from init_global_DistanceFinder
MolecularUnit* init_MolecularUnit_B_from_settings(PredefinedInteractions *df, bool nogui) {
	MolecularUnit *muB = readMolecularUnitFromFile(
			Settings::MolecularUnitB::file,
			Settings::MolecularUnitB::ignored_rows,
			Settings::MolecularUnitB::x_col,
			Settings::MolecularUnitB::y_col,
			Settings::MolecularUnitB::z_col,
			Settings::MolecularUnitB::radius_col,
			Settings::MolecularUnitB::label_col,
			Settings::MolecularUnitB::atomNo_col
			);
	muB->buildStree();
	if (Settings::General::candidate_interactions)
		muB->simplify(*df, true);

	return muB;
}



/**
 * TODO:Fix the comments
 * Main entrance of the program.
 * It handles command-line arguments. If nogui is typed through command line, then LaunchWindow and Thread_Viewer will not be created.
 * And main thread will run AtlasBuilder. Thread_BackEnd will also not created. The jobs of Thread_BackEnd will be done by main thread.
 * And all the necessary information will be provided by Setting.ini file.
 *
 * If GUI is allowed, the information in the Settings.ini will be loaded to LaunchWindow and user is allowed to change the settings through this window.
 * If runSample is checked, the main thread will do the setup of AtlasBuilder. Thread_BackEnd will start building the atlas. Thread_Viewer will handle display.
 * If runSample is not checked, the main thread will load the existing atlas. Thread_BackEnd will do nothing. Thread_Viewer will handle display.
 *
 * Note that Thread_Viewer is capable of creating other threads such as Thread_Sampling if user choose to start a new sampling or continue an old one.
 */
int main (int argc, char** argv) {

	// important that these are declared on the HEAP!!!
	// they are used in other pthreads and can not be
	// (local) stack variables.
	MolecularUnit *muA;
	MolecularUnit *muB;


	SaveLoader *save_loader;
	AtlasBuilder *atlas_builder;

	Atlas *atlas = new Atlas();
	// end HEAP vars

	// command line arguments
	bool nogui = false;
	// string settings_file = "../settings.ini";
	string settings_file = "./settings.ini";
#ifdef WIN32
	settings_file = "..\\settings.ini";
#endif
	// end CL vars

	// handle command-line arguments
	for (int i = 1; i < argc; i++) {
		string inp = argv[i];
		string flag, val;

		int eqsign = inp.find("=");
		if (eqsign+1 == inp.length()) {
			cerr << "No value for flag " << inp.substr(0, eqsign) << "." << endl;
			return 0;
		} else if (eqsign == -1) {
			flag = inp;
			val = "";
		} else {
			flag = inp.substr(0, eqsign);
			val  = inp.substr(eqsign+1, inp.length());
		}

		// compare returns 0 if the value is the same, so not ! determines if they are the same
		if (!flag.compare("-settings")) {
			if (!val.compare("")) {
				i++;
				if (i == argc) {
					cerr << "No filename provided for flag " << flag << "." << endl;
					return 0;
				}
				settings_file = argv[i];
			} else {
				cerr << "Invalid value for flag " << flag << "." << endl;
				return 0;
			}
		}

		else if (!flag.compare("-nogui")) {
			if (!val.compare("")) {
				nogui = true;
			} else {
				cerr << "Invalid value for flag " << flag << "." << endl;
				return 0;
			}
		}

	}


	// load the settings
	cout << "Loading settings from \"" << settings_file << "\"." << endl;
	Settings::load(settings_file.c_str());


	// potentially change settings using the launch window
	if (!nogui) {
		FXApp application("Molecular Bonding", "UF CISE");
		application.init(argc, argv);
		LaunchWindow openDialog(&application);
		application.create();
		application.run();
		application.destroy();
	} else {
		Settings::Sampling::runSample = true;
	}


	// Initialize some global variables
	// distance data
	PredefinedInteractions df;
	init_DistanceFinder_from_settings(&df);

	// molecular unit A and B
	muA = init_MolecularUnit_A_from_settings(&df, nogui);
	muB = init_MolecularUnit_B_from_settings(&df, nogui);

	// SaveLoader
	save_loader = new SaveLoader(Settings::Output::dataDirectory, muA, muB);


	if (Settings::Constraint::wholeCollision) {
		// reading the neighbour matrix
		ConstraintCheck::nei_matrix = Utils::getMatrixFromFileFromTo("nei.txt");
	} else {
		ConstraintCheck::nei_matrix = Utils::getIdentityMatrix();
	} 

	atlas_builder = new AtlasBuilder(muA, muB, save_loader, &df, atlas);


	if (Settings::Sampling::runSample) {
		atlas_builder->setup();
		cout << "Thread_Main: AtlasBuilder Set up done." << endl;

	} else {
		save_loader->loadMapView( atlas );
		cout << "Thread_Main: Loads existing atlas." << endl;
	}



	if (nogui) {
		cout << "Thread_Main: Calling atlas_builder->startAtlasBuilding()." << endl;
		atlas_builder->startAtlasBuilding();

		cout << "Thread_Main: Calling this->save_loader->saveRoadMap(this->atlas)." << endl;
		save_loader->saveRoadMap(atlas);

		cout << "Thread_Main: Finishes and Exits.." << endl;
	} else {
		Thread_Viewer *a=new Thread_Viewer(
				argc, argv,
				muA, muB,
				save_loader,
				atlas_builder, atlas
				);
		if (a->start() != 0) {
			cerr << "Thread_Viewer: Failed to start.";
			return EXIT_FAILURE;
		}
		cout << "Thread_Viewer: Started." << endl;

		Thread_BackEnd *b=new Thread_BackEnd(0);
		if (b->start() != 0) {
			cerr << "Thread_BackEnd: Failed to start.";
			return EXIT_FAILURE;
		}
		cout << "Thread_BackEnd: Started." << endl;

		if (a->wait() != 0) {  // blocks the calling thread until the specified thread terminates.
			cerr << "Thread_Viewer: Failed on wait.";
			return EXIT_FAILURE;
		}

		if (b->wait() != 0) {
			cerr << "Thread_BackEnd: Failed on wait.";
			return EXIT_FAILURE;
		}
		delete b;
		delete a;
	}

	return 0;   /* ANSI C requires main to return int. */
}
