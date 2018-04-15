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
 * roadNode.cpp
 *
 *  Created on: Feb 2, 2009
 *      Author: James Pence
 */

#include "AtlasNode.h"
#include "ThreadShare.h"
#include "Utils.h"
#include "Atlas.h"

#include <algorithm>
#include <cmath>
#include <utility>
#include <tuple>

using namespace std;

//pthread_spinlock_t nodeInUse;
extern  tuple<int, int, int> entryTags;
AtlasNode::AtlasNode() {
//	pthread_spin_init(&nodeInUse,NULL);

	this->visited = false;

	for (int i = 0; i < 3; i++) {
		this->loc[i] = 0;
		this->vel[i] = 0;
		this->force[i] = 0;
	}
	this->velMag = 15.0;
	this->forceMag = 1.0;

	this->numID = -1;
	this->numPass = 1;
	this->dim = 0;

	this->complete = false;
	this->noGoodOrientation = true;

	this->constraintGraph = new ActiveConstraintGraph(); // NULL;
	this->region = new ActiveConstraintRegion();

	this->dimWritten = false;
}

AtlasNode::AtlasNode(int ID, bool complete, bool empty, int numDim,
		double* location, vector<int> connection) {
	// pthread_spin_init(&nodeInUse,NULL);

	this->visited = false;

	for (int i = 0; i < 3; i++) {
		this->loc[i] = location[i];
		this->vel[i] = 0;
		this->force[i] = 0;
	}
	this->velMag = 15.0;
	this->forceMag = 1.0;

	this->numID = ID;
	this->numPass = 1;
	this->dim = numDim;

	this->complete = complete;

	if (numDim == 0 && !Settings::Sampling::short_range_sampling) // if it is dimension 0, there is no parameter to sample except 6d. // and virus case. || Settings::General::virus_mer
		this->complete = true;

	this->noGoodOrientation = empty; //it would be better to initialize it to false, by this way the node will be displayed during sample and if there is no good orientation in it after search THEN it will be set to TRUE then it will disappear.

	this->connection.assign(connection.begin(), connection.end());
	this->constraintGraph = NULL;
	this->region = new ActiveConstraintRegion(); // ??????? NULL;

	this->dimWritten = false;
}

AtlasNode::~AtlasNode() {
//	pthread_spin_destroy(&nodeInUse);

	this->connection.clear();
	delete this->constraintGraph;
	delete this->region;
}

void AtlasNode::setLocation(double x, double y, double z) {
	this->loc[0] = x;
	this->loc[1] = y;
	this->loc[2] = z;
}

double* AtlasNode::getLocation() {
	return this->loc;
}

double* AtlasNode::getVelocity() {
	return this->vel;
}

double* AtlasNode::getForce() {
	return this->force;
}

double AtlasNode::getForceMag() {
	return this->forceMag;
}

vector<int> AtlasNode::getConnection() {
	return this->connection;
}

void AtlasNode::applyForce() {
	this->forceMag = Utils::mag(this->force);

//	if(this->forceMag > .01){
	for (int i = 0; i < 3; i++) {
		if (i == 2) {
			this->vel[i] += this->force[i] * .01;
		} else {
			this->vel[i] += this->force[i] * .04;
		}
	}
//	}
	//dampening
	for (int i = 0; i < 3; i++) {
//            this->vel[i] *= (this->vel[i] < .005)?.6:.95;
		this->vel[i] *= .4;
	}

}

void AtlasNode::applyVelocity() {
	for (int i = 0; i < 3; i++) {
		if (fabs(this->vel[i]) > 30) {
			if (this->vel[i] < 0) {
				this->vel[i] = -30;
			} else {
				this->vel[i] = 30;
			}
		}
		this->loc[i] += this->vel[i];
	}
	this->velMag = Utils::mag(this->vel);
}

void AtlasNode::addConnection(int other) {
	for (size_t i = 0; i < this->connection.size(); i++) {
		if (this->connection[i] == other) {
			return;
		}
	}

	this->connection.push_back(other);
}

bool AtlasNode::removeConnection(int other) {
	vector<int>::iterator iter;
	iter = find(this->connection.begin(), this->connection.end(), other);
	if (iter == this->connection.end()) {
		return false;
	} else {
		this->connection.erase(iter);
		return true;
	}
}

bool AtlasNode::isConnectedTo(int other) {
	for (size_t i = 0; i < this->connection.size(); i++) {
		if (this->connection[i] == other) {
			return true;
		}
	}
	return false;
}

void AtlasNode::setComplete(bool setting) {
	this->complete = setting;
}

bool AtlasNode::hasAnyGoodOrientation() {
	return !noGoodOrientation;
}

void AtlasNode::setFoundGoodOrientation(bool setting) {
	noGoodOrientation = !setting;
}

bool AtlasNode::isComplete() {
	return this->complete;
}

int AtlasNode::getID() const {
	return this->numID;
}

void AtlasNode::setID(int id) {
	this->numID = id;
}

int AtlasNode::getDim() {
	return this->dim;
}

int AtlasNode::getParamDim() {
	if (this->constraintGraph != NULL) {
		return this->constraintGraph->getParamDim();
	}
	return this->dim;
}

ActiveConstraintRegion* AtlasNode::getACR() {
	return this->region;
}

void AtlasNode::setACR(ActiveConstraintRegion* newRegion) {
	if (this->region != NULL) {
		delete this->region;
	}
	this->region = newRegion;
}

ActiveConstraintGraph* AtlasNode::getCG() {
	return this->constraintGraph;
}

void AtlasNode::getWitnessFlips(std::vector<int> &WitnessFlips){
	ActiveConstraintRegion *acr = new ActiveConstraintRegion();
  ThreadShare::save_loader->loadNode(this->getID(), acr);
	std::vector<CayleyPoint*> witnesses = acr->getWitness();
	for(int i=0; i<witnesses.size(); i++){
		std::vector<int> v = witnesses[i]->getFlips();
		for(int j=0; j<v.size();j++){
			WitnessFlips.push_back(v[j]);
		}
	}
	std::sort(WitnessFlips.begin(),WitnessFlips.end());
	auto last = std::unique(WitnessFlips.begin(),WitnessFlips.end());
	WitnessFlips.erase(last, WitnessFlips.end());
	//TODO: Delete the acr you've read.
}

void AtlasNode::setCG(ActiveConstraintGraph* acg) {
	if (this->constraintGraph != NULL) {
		delete this->constraintGraph;
	}
	this->constraintGraph = acg;
	this->dim = acg->getDim();
}

void AtlasNode::trimNode() {
	delete this->region;
	this->region = NULL;

	delete this->constraintGraph;
	this->constraintGraph = NULL;
}

std::vector<std::vector< int > > AtlasNode::getFlipScheme(){
	return this->flipScheme;
}

void AtlasNode::setFlipScheme(std::vector<std::vector< int > > flipScheme){
	this->flipScheme = flipScheme;
}


void AtlasNode::setFlipSpace(vector<vector<pair<CayleyPoint*, int>*>*> space) {
	this->flipSpace = space;
}

vector<vector<pair<CayleyPoint*, int>*>*> AtlasNode::getFlipSpace() {
	return this->flipSpace;
}

void AtlasNode::setAllEntryPoints(vector<pair<CayleyPoint*, int> *> ep) {
	this->allEntryPoints = ep;
}

vector<pair<CayleyPoint*, int> *> AtlasNode::getAllEntryPoints() {
	return this->allEntryPoints;
}

/**Finds path in tree between two nodes (given they are in the same tree) */
std::vector<std::pair<CayleyPoint*, int>*> AtlasNode::findTreePath(
		EventPointNode* src, EventPointNode* dst,
		vector<vector<pair<CayleyPoint*, int>*>*> flipSpace, int flip_1,
		int flip_2) {

	std::vector<std::pair<CayleyPoint*, int>*> path;
	vector<EventPointNode*> nodePath;
	vector<EventPointNode*> src_path;
	vector<EventPointNode*> dst_path;

	EventPointNode* current;
	EventPointNode* temp;

	EventPointNode* root = this->EventPointForest[src->getComponent()];

	if (src->getComponent() != dst->getComponent()) {
		//cout << "Src and dst not in same tree" << endl;
		return path;
	}

	if (dst == src) {
		double temp[6];
		double data[6];
		src->getPoint()->getPoint(temp);
		dst->getPoint()->getPoint(data);
		cout
				<< "Same event points, discovered the entry points are merged flips."
				<< endl << "Path: " << temp[0] << " on flip " << flip_1
				<< " to " << data[0] << " on flip " << flip_2 << endl;
		return path;
	}

	current = src;
	while (current != root && current != NULL) {
		src_path.push_back(current);
		current = current->getParent();
	}
	src_path.push_back(root);

	current = dst;
	while (current != root && current != NULL) {
		dst_path.push_back(current);
		current = current->getParent();
	}
	dst_path.push_back(root);

	while (src_path.back() == dst_path.back()) {
		temp = src_path.back();
		src_path.pop_back();
		dst_path.pop_back();
	}

	src_path.push_back(temp);
	reverse(src_path.begin(), src_path.end());
	src_path.insert(src_path.end(), dst_path.begin(), dst_path.end());
	nodePath = src_path;

	vector<pair<CayleyPoint*, int>*> edgePath;

	for (int i = 0; i < nodePath.size() - 1; i++) {
		edgePath = findEdgePath(nodePath[i], nodePath[i + 1], flipSpace, flip_1,
				flip_2);
		for (int j = 0; j < edgePath.size(); j++) {
			path.push_back(edgePath[j]);
		}
	}

	return path;
}

void AtlasNode::printEventForest() const {
    cout <<endl << "Forest for node: " << this->getID()<< endl;
	for (int i = 0; i < EventPointForest.size(); i++) {
		cout << endl << "Tree " << i << ": " << endl << endl;
		printTree(EventPointForest[i]);
	}
	cout << endl;

}

void AtlasNode::printTree(const EventPointNode* input) const{

	input->printData();
	cout << endl;

	vector<EventPointNode*> children = input->getChildren();

	for (int i = 0; i < children.size(); i++) {
		printTree(children[i]);
	}
}

bool comparator(CayleyPoint* p1, CayleyPoint* p2) {
	double data1[6], data2[6];
	double cp1, cp2;
	p1->getPoint(data1);
	cp1 = data1[0];
	p2->getPoint(data2);
	cp2 = data2[0];
	return cp1 < cp2;

}

/**merge sort getWitSpace and getJustSpace by parameter */
std::vector<CayleyPoint*> AtlasNode::sortSpace(ActiveConstraintRegion* region) {

	vector<CayleyPoint*> witSpace = region->getWitness();


	//TODO: marker: Witness excluded currently

	//vector<CayleyPoint*> space = region->getSpace();
	vector<CayleyPoint*> space = region->getJustSpace();

	std::sort(space.begin(), space.end(), comparator);

	vector<int> deleteList;

	/**** COUT to test sortSpace
	 cout << "Before merge: " << endl;
	 for (int k = 0; k < space.size(); k++) {

	 space[k]->printData();
	 cout << "Flips: ";
	 for (int i = 0; i < space[k]->getFlips().size(); i++) {
	 cout << space[k]->getFlips()[i] << " ";
	 }
	 cout << endl << endl;
	 }
	 ******/

	//for all points except the last point, consider the point one slot in front of it...
	int i = 0;
	while (i < space.size() - 1) {

		double temp_1[6];
		double temp_2[6];
		space[i]->getPoint(temp_1);
		space[i + 1]->getPoint(temp_2);

		bool isUniqueOri = true;

		if (abs(temp_1[0] - temp_2[0]) < .000001) {

			for (int j = 0; j < space[i]->getOrientations().size(); j++) {

				for (int k = 0; k < space[i + 1]->getOrientations().size();
						k++) {

					if (space[i]->getOrientations()[j]->getFlipNum()
							== space[i + 1]->getOrientations()[k]->getFlipNum()) {
						isUniqueOri = false;
					}
				}
				if (isUniqueOri == true) {
					Orientation *orient = new Orientation(
							space[i]->getOrientations()[j]);
					space[i + 1]->addOrientation(orient);
				}
				isUniqueOri = true;
			}

			space.erase(space.begin() + i);
		} else {
			i++;
		}

	}
	/*** COUT for testing sortSpace
	 cout << "After merge: " << endl;
	 for (int k = 0; k < space.size(); k++) {

	 space[k]->printData();
	 cout << "Flips: ";
	 for (int i = 0; i < space[k]->getFlips().size(); i++) {
	 cout << space[k]->getFlips()[i] << " ";
	 }
	 cout << endl << endl;
	 }
	 ****/

	return space;
}

/** Take sorted space and split into (# of flips) vectors */
void AtlasNode::splitSpace(std::vector<CayleyPoint*> space) {
	/*
	 for (int k = 0; k < space.size(); k++) {
	 space[k]->printData();
	 cout << endl;
	 }
	 /*
	 /**Splits space into vectors of different flips */
	std::vector<std::pair<CayleyPoint*, int>*> *flip_0Space, *flip_1Space,
			*flip_2Space, *flip_3Space, *flip_4Space, *flip_5Space,
			*flip_6Space, *flip_7Space;
	flip_0Space = new vector<std::pair<CayleyPoint*, int>*>;
	flip_1Space = new vector<std::pair<CayleyPoint*, int>*>;
	flip_2Space = new vector<std::pair<CayleyPoint*, int>*>;
	flip_3Space = new vector<std::pair<CayleyPoint*, int>*>;
	flip_4Space = new vector<std::pair<CayleyPoint*, int>*>;
	flip_5Space = new vector<std::pair<CayleyPoint*, int>*>;
	flip_6Space = new vector<std::pair<CayleyPoint*, int>*>;
	flip_7Space = new vector<std::pair<CayleyPoint*, int>*>;

	flipSpace.push_back(flip_0Space);
	flipSpace.push_back(flip_1Space);
	flipSpace.push_back(flip_2Space);
	flipSpace.push_back(flip_3Space);
	flipSpace.push_back(flip_4Space);
	flipSpace.push_back(flip_5Space);
	flipSpace.push_back(flip_6Space);
	flipSpace.push_back(flip_7Space);

	std::pair<CayleyPoint*, int>* point_Flip;
	vector<int> flipList;
	pair<CayleyPoint*, int>* temp;

//step through the space
	for (int i = 0; i < space.size(); i++) {
		CayleyPoint* currentPoint = space[i];
		if (currentPoint->hasOrientation() == false) {
			//pushback nullptr onto all flip vectors for this index
			for (int k = 0; k < 8; k++) {
				flipSpace[k]->push_back(nullptr);
			}
			continue;
		}

		vector<int> currentFlips = currentPoint->getFlips();
		flipList = {0,1,2,3,4,5,6,7};
		pair<CayleyPoint*, int> temp;
		int flip;

		for (int j = 0; j < currentFlips.size(); j++) {
			flip = currentFlips[j];

			point_Flip = new std::pair<CayleyPoint*, int>(currentPoint, flip);

			flipSpace[flip]->push_back(point_Flip);
			flipList.erase(remove(flipList.begin(), flipList.end(), flip));
		}

		for (int g = 0; g < flipList.size(); g++) {
			flipSpace[flipList[g]]->push_back(nullptr);
		}
	}

	/*************

	 for (int y = 0; y < flipSpace.size(); y++) {
	 cout << endl << "Flip " << y << ": " << endl;
	 for (int z = 0; z < flipSpace[y]->size(); z++) {
	 if ((*flipSpace[y])[z] != nullptr) {
	 if ((*flipSpace[y])[z]->first == nullptr) {
	 cou
	 << " This is bad: array slot is non null but cayleyPoint is null "
	 << endl;
	 //<< (*flipSpace[y])[z]->first << endl;
	 }

	 else {
	 double temp[6];
	 (*flipSpace[y])[z]->first->getPoint(temp);
	 cout << temp[0] << endl;
	 }

	 }

	 else if ((*flipSpace[y])[z] == nullptr) {
	 cout << "nullptr" << endl;
	 }
	 }
	 cout << endl;
	 }

	 ***********************************/

}

void AtlasNode::printFlip(int y){

	 for (int z = 0; z < flipSpace[y]->size(); z++) {
		if ((*flipSpace[y])[z] != nullptr) {
			if ((*flipSpace[y])[z]->first == nullptr) {
				cout
						<< " This is bad: array slot is non null but cayleyPoint is null "
						<< endl;

			}

			else {
				double temp[6];
				(*flipSpace[y])[z]->first->getPoint(temp);
				cout << temp[0] << endl;
			}

		}

		else if ((*flipSpace[y])[z] == nullptr) {
			cout << "nullptr" << endl;
		}
	}
}



double AtlasNode::findL2Distance(MolecularUnit *MuA, MolecularUnit *MuB, Orientation* o1,
		Orientation *o2) {

	double distance = 0;
	double fromO1[3][3], toO1[3][3];
	double fromO2[3][3], toO2[3][3];

	o1->getFromTo(fromO1, toO1);
	o2->getFromTo(fromO2, toO2);

	vector<Atom*> atom1A = MuA->getXFAtoms(Utils::getIdentityMatrix());
	vector<Atom*> atom1B = MuA->getXFAtoms(fromO1, toO1);

	vector<Atom*> atom2A = MuB->getXFAtoms(Utils::getIdentityMatrix());
	vector<Atom*> atom2B = MuB->getXFAtoms(fromO2, toO2);

	for (int i = 0; i < atom1A.size(); i++) {
		double *loc1 = atom1A[i]->getLocation();
		double *loc2 = atom2A[i]->getLocation();

		distance += pow(loc1[0] - loc2[0], 2) + pow(loc1[1] - loc2[1], 2)
				+ pow(loc1[2] - loc2[2], 2);
	}

	for (int i = 0; i < atom1B.size(); i++) {
		double *loc1 = atom1B[i]->getLocation();
		double *loc2 = atom2B[i]->getLocation();

		distance += pow(loc1[0] - loc2[0], 2) + pow(loc1[1] - loc2[1], 2)
				+ pow(loc1[2] - loc2[2], 2);
	}

	distance = pow(distance, 0.5);

	return distance;
}



vector<pair<int, int>*> AtlasNode::checkBifurcation(
		std::pair<CayleyPoint*, int>* pointFlip, int index,
		vector<vector<pair<CayleyPoint*, int>*>*> flipSpace) {

	vector<pair<int, int>*> indices;

	//cout << endl << "checkBifurcation called" << endl;

	if (pointFlip == nullptr) {

		cout << "checkBifurcation: passed in a null. " << endl;

		return indices;
	}

	std::pair<int, int>* knownIndex = new std::pair<int, int>(index,
			pointFlip->second);

	indices.push_back(knownIndex);

	vector<Orientation*> orient = pointFlip->first->getOrientations();
	if (orient.size() < 2) {
		return indices;
	}

//epsilon should be fixed, probably don't want to change for same region
	double eps = 0.1;
//	cout << "Input epsilon: " << endl;
//	cin >> eps;

	Orientation* ori_1;
	Orientation* ori_2;
	MolecularUnit* MuA;
	MolecularUnit* MuB;



	//get the 1D contraint graphs
	MuA = this->getCG()->getMolecularUnitA();
	MuB = this->getCG()->getMolecularUnitB();

	//get the orientation on the given flip
	ori_1 = pointFlip->first->getOrientation(pointFlip->second);

	//	cout << endl << endl << "Test L2: " << endl << findL2Distance(MuA, MuB, ori_1, ori_1) << endl << endl;

	vector<int> flips = pointFlip->first->getFlips();

	for (int i = 0; i < flips.size(); i++) {

		if (flips[i] != pointFlip->second
				&& (*flipSpace[flips[i]])[index] != nullptr) {

			//if loop flip var has a valid point, get its orientaion
			ori_2 = (*flipSpace[flips[i]])[index]->first->getOrientation(
					flips[i]);

			//see if the two orientaions are close enough
			double dist = findL2Distance(MuA, MuB, ori_1, ori_2);

			//cout << "L2 distance is: " << dist << endl;

			if (dist < eps ) {
				//cout << "L2 distance is: " << dist << endl;
				std::pair<int, int>* temp;
				temp = new std::pair<int, int>(index, flips[i]);
				indices.push_back(temp);
			}
		}
	}

	return indices;
}

void AtlasNode::testBifurcation(int flip){

	std::pair<CayleyPoint*, int>* pointFlip;
	int index;

	vector<pair<int, int>*> index_flip;

	for(int i = 0; i < flipSpace[flip]->size(); i++){
		if((*flipSpace[flip])[i] != nullptr){
			pointFlip = (*flipSpace[flip])[i];
			index_flip = checkBifurcation(pointFlip,i,flipSpace);
			if(index_flip.size() > 1){
				cout << endl << "Bifurcation found at index " << i << " with flips ";
				for(int j = 0; j < index_flip.size(); j++){
					cout << index_flip[j]->second << " ";
				}
				cout << endl;
			}
		}
	}
}

bool AtlasNode::checkEntryPoint(std::pair<CayleyPoint*, int>* point) {
	bool isEntry = false;

	//cout << endl << "checkEntryPoint called" << endl;

	if (point == nullptr) {
		//char dummy;
		cout << "checkEntryPoint: null passed in." << endl;
		//cin >> dummy;
		return isEntry;
	}

	for (int i = 0; i < this->allEntryPoints.size(); i++) {
		double temp[6];
		double data[6];
		allEntryPoints[i]->first->getPoint(temp);
		point->first->getPoint(data);
		int flip = allEntryPoints[i]->second;
		if (temp[0] == data[0] && point->second == flip) {
			//cout << "Found entry point on flip at: " << data[0] << endl;
			return true;
		}
	}

	return isEntry;
}

bool AtlasNode::checkEntryPoint(CayleyPoint* point, int flip) {
	bool isEntry = false;


	if (point == nullptr) {
		return isEntry;
	}

	for (int i = 0; i < this->allEntryPoints.size(); i++) {
		double temp[6];
		double data[6];
		allEntryPoints[i]->first->getPoint(temp);
		point->getPoint(data);
		int EPflip = allEntryPoints[i]->second;
		if ( (temp[0] - data[0] < .000001) && flip == EPflip) {
			return true;
		}
	}

	return isEntry;

}

/**takes two adjacent edges and returns the continuous path between them */
vector<pair<CayleyPoint*, int>*> AtlasNode::findEdgePath(EventPointNode * src,
		EventPointNode * dst,
		vector<vector<pair<CayleyPoint*, int>*>*> flipSpace, int flip_1,
		int flip_2) {
	vector<pair<CayleyPoint*, int>*> edgePath;
	vector<pair<CayleyPoint*, int>*> flipArr;
	int start;
	int end;

	vector<pair<int, int> *> srcIndices = src->getFlipIndices();
	vector<pair<int, int> *> dstIndices = dst->getFlipIndices();
	int srcIndex;
	int dstIndex;

	int flip;
	/*******8
	 if (src->getType() == 0) {
	 flip = src->getFlip();
	 flipArr = *flipSpace[flip];
	 srcIndex = src->getIndexOfFlip(flip);
	 dstIndex = dst->getIndexOfFlip(flip);
	 }

	 else if (dst->getType() == 0) {
	 flip = dst->getFlip();
	 flipArr = *flipSpace[flip];
	 srcIndex = src->getIndexOfFlip(flip);
	 dstIndex = dst->getIndexOfFlip(flip);
	 } else {

	 *********/
	int sharedFlip;
    bool haveSharedFlip = false;
   //cout << endl << "enter FEP loop:" << endl;
	for (int i = 0; i < srcIndices.size(); i++) {
		for (int j = 0; j < dstIndices.size(); j++) {
			if (srcIndices[i]->second == dstIndices[j]->second) {

				sharedFlip = srcIndices[i]->second;
				flipArr = *flipSpace[sharedFlip];

				srcIndex = srcIndices[i]->first;
				dstIndex = dstIndices[j]->first;
				haveSharedFlip = true;
				break;
			}

                //  cout << srcIndices[i]->second << " " << dstIndices[j]->second << endl;

		}
	}
	if (haveSharedFlip == false) {
        cout    << endl
                << "error find edge path: no shared flip?"
                << " scrIndices.empty() = " << srcIndices.empty()<< endl << "dstIndices.empty() = " << dstIndices.empty()
				<< endl;
		return edgePath;
	}

	//}

	if (srcIndex < dstIndex) {
		start = srcIndex;
		end = dstIndex;
		if (sharedFlip != flip_1) {
			edgePath.push_back((*flipSpace[flip_1])[srcIndex]);
		}

	} else {
		start = dstIndex;
		end = srcIndex;
		if (sharedFlip != flip_2) {
			edgePath.push_back((*flipSpace[flip_2])[srcIndex]);
		}
	}

	for (int k = 0; k <= end - start; k++) {
		edgePath.push_back(flipArr[k + start]);
	}

	//add final flip change if end is bifurcation
	if (srcIndex == end && sharedFlip != flip_1) {
		edgePath.push_back((*flipSpace[flip_1])[srcIndex]);
	}

	else if (dstIndex == end && sharedFlip != flip_2) {
		edgePath.push_back((*flipSpace[flip_2])[srcIndex]);
	}

	return edgePath;
}

/**Finds all children of given EPN by searching through flipSpace	*/

int AtlasNode::searchSpace(EventPointNode* current,
		std::vector<std::vector<std::pair<CayleyPoint*, int>*>*> flipSpace) {

	/**
	if (current == NULL) {
		return -1;
	}
	****/

//finds all children of EPN current

	int parentFlip;
	int parentIndex;
	bool hasParent = false;
	std::pair<CayleyPoint*, int>* currentConfig;
	int currentIndex;
	pair<int, int>* temp;
	int flip;
	vector<pair<int, int>*> flip_indices;
	bool isEntryPoint;
	double data[6];
	double param[6];

	if (current->getParent() != nullptr) {
		hasParent = true;
		EventPointNode* parent = current->getParent();
		//when current was discovered, its flip was set to the flip that it was discovered on
		parentFlip = current->getFlip();
		parentIndex = parent->getIndexOfFlip(parentFlip);
	}


	//if this point hasn't been visited,check if it is a bifurcation point (only happens for root)
	if (current->getVisited() == false) {
		flip = current->getFlip();
		for (int z = 0; z < flipSpace[flip]->size(); z++) {

			if ((*flipSpace[flip])[z] == nullptr) {
				continue;
			}
			//check if the orientations are the same if there is a valid point

			/*
			 else if (current->getPoint()->getOrientation(flip)->isEqual(
			 (*flipSpace[flip])[z]->first->getOrientation(flip)) == true) {

			 */

			current->getPoint()->getPoint(data);
			(*flipSpace[flip])[z]->first->getPoint(param);

			if (abs(param[0] - data[0]) < .0001) {

				std::pair<CayleyPoint*, int>* point_flip = new std::pair<
						CayleyPoint*, int>(current->getPoint(), flip);

				if (point_flip == nullptr) {
					cout << "Line 729: searchSpace. Created a null pointer? "
							<< endl;
				}

				//cout << endl << "First CB: ";
				vector<pair<int, int>*> indices = checkBifurcation(point_flip,
						z, flipSpace);

				if (indices.size() > 1) {
					//cout << endl << "found a bifurcation point on index"
							//<< indices[0]->first;
				}

				current->setFlipIndices(indices);

				if (current->getFlipIndices().size() > 1) {
					current->setType(1);
				}

				break;

				/*
				 cout << "Value pushed to flipIndices: "
				 << current->getIndexOfFlip(flip) << endl;
				 cout << "Flip " << flip << " array: ";

				 for (int p = 0; p < flipSpace[flip]->size(); p++) {
				 cout << (*flipSpace[flip])[p] << endl;
				 }

				 } else {
				 /*
				 cout << "Line 652: first is: " << current->getPoint()
				 << endl;
				 cout << "second is: " << (*flipSpace[flip])[z]->first
				 << endl;
				 **/

			}
		}
	}
/*******
	if (current->getFlipIndices().size() < 1) {
		cout << endl
				<< "ERROR: current node in searchSpace has no flipIndices set. Returning -1."
				<< endl;
		return -1;
	}
	********/

	for (int j = 0; j < current->getFlipIndices().size(); j++) {

		flip = current->getFlipIndices()[j]->second; //this should get the flip#'s successively

		currentIndex = current->getIndexOfFlip(flip);

		if (currentIndex < 0 || currentIndex >= flipSpace[flip]->size()) {
			cout << "Index is not in range of space????" << endl;
			continue;
		}

		currentIndex += 1;
		currentConfig = (*flipSpace[flip])[currentIndex];

		while (currentConfig != nullptr

		&& currentIndex < flipSpace[flip]->size()) {

			if (hasParent == true) {
				if (parentFlip == flip && parentIndex >= currentIndex) {
					break;
				}
			}

			//cout << endl << "Second CB: ";
			flip_indices = checkBifurcation(currentConfig, currentIndex,
					flipSpace);

			if (flip_indices.size() > 1) {
				//cout << endl << "found a bifurcation point on index"
						//<< flip_indices[0]->first;
			}

			isEntryPoint = checkEntryPoint(currentConfig);

			if (flip_indices.size() > 1 || isEntryPoint) {

				EventPointNode* child = new EventPointNode(
						(*flipSpace[flip])[currentIndex]->first, flip, false,
						false, 0, current->getComponent());

				current->addChild(child);
				child->setParent(current);
				child->setFlipIndices(flip_indices);

				stringstream ss;
				ss << child->getPoint()->getID() << "," << child->getFlip();
				string hkey = ss.str();
				if (allEventPointsHash.find(hkey)==allEventPointsHash.end()) {
					allEventPointsHash.insert(make_pair(hkey,child));
					allEventPoints.push_back(child);
				}
				if (!isEntryPoint) {
					child->setType(1);
				}
				break;
			}

			currentIndex += 1;		//move right down the array
			currentConfig = (*flipSpace[flip])[currentIndex];
		}

		currentIndex = current->getIndexOfFlip(flip) - 1;

		currentConfig = (*flipSpace[flip])[currentIndex];

		while (currentConfig != nullptr && currentIndex >= 0) {

			if (hasParent == true) {
				if (parentFlip == flip && parentIndex <= currentIndex) {
					break;
				}
			}

			//cout << endl << "Third CB: ";
			vector<pair<int, int>*> flip_indices = checkBifurcation(
					currentConfig, currentIndex, flipSpace);

			if (flip_indices.size() > 1) {
				cout << endl << "found a bifurcation point on index"
						<< flip_indices[0]->first;
			}

			isEntryPoint = checkEntryPoint(currentConfig);

			if (flip_indices.size() > 1 || isEntryPoint) {

				EventPointNode* child = new EventPointNode(
						(*flipSpace[flip])[currentIndex]->first, flip, false,
						false, 0, current->getComponent());

				current->addChild(child);
				child->setParent(current);
				child->setFlipIndices(flip_indices);
				stringstream ss;
				ss << child->getPoint()->getID() << "," << child->getFlip();
				string hkey = ss.str();
				if (allEventPointsHash.find(hkey)==allEventPointsHash.end()) {
					allEventPointsHash.insert(make_pair(hkey,child));
					allEventPoints.push_back(child);
				}
				if (!isEntryPoint) {
					child->setType(1);
				}
				break;
			}

			currentIndex -= 1;		//move 'left' down the array
			currentConfig = (*flipSpace[flip])[currentIndex];
		}
	}


	current->setVisited(true);

	if (current->getChildren().size() > 0) {
		return 0;
	}

	else
		return -1;
}

//assumes that src is in tree treeNum, looks for dst
bool AtlasNode::buildDFStree(int treeNum, EventPointNode** dst,
		EventPointNode* current,
		vector<vector<pair<CayleyPoint*, int>*>*> flipSpace) {

//build BFS tree starting on root treeNum until tree is complete or dst is found

	//current is passed in from find1DOF as the root of tree treeNum

	double temp[6];
	double data[6];
	int dstFlip;
	int currentFlip;
	bool pathBool = false;

	//set current to the root of the tree (why is it input then? current->getComponenet shoudl be treeNum)
	if(current->getComponent() != treeNum){
		cout << endl << "line 1100: Error..." << endl ;
	}


	if (current->getVisited() == false) {
		int check = searchSpace(current, flipSpace);
/*******
		if (check == -1) {
			cout << "SearchSpace found no children" << endl;
		}

		else
			cout << "searchSpace found " << current->getChildren().size()
					<< " children." << endl << endl;
					*************/
	}


	vector<EventPointNode*> children = current->getChildren();

	(*dst)->getPoint()->getPoint(temp);
	dstFlip = (*dst)->getFlip();
	for (int i = 0; i < children.size(); i++) {

		children[i]->getPoint()->getPoint(data);

		if (abs(temp[0] - data[0]) < .000001) {
			//cout << "current value: " << data[0] << endl << "dst value: "
					//<< temp[0] << endl;
			for (int j = 0; j < children[i]->getFlipIndices().size(); j++) {

				currentFlip = children[i]->getFlipIndices()[j]->second;
				//cout << "current flip: " << currentFlip << "  dstFlip: "
						//<< dstFlip << endl;
				if (dstFlip == currentFlip) {
					*dst = children[i];
					return true;
				}
			}
		}


		if (children[i]->getExplored() == false) {
			pathBool = buildDFStree(treeNum, dst, children[i], flipSpace);
			if(pathBool == true && i != children.size()-1){
				return pathBool;
			}
		}
	}

	current->setExplored(true);

	if (EventPointForest[treeNum]->getExplored() == true) {
		treeCompleted[treeNum] = true;
	}


	return pathBool;
}

EventPointNode* AtlasNode::findCorrespondingEP(CayleyPoint* point, int flip) {

	double temp[6];
	double data[6];
	point->getPoint(temp);
	CayleyPoint* current;

	for (int i = 0; i < this->allEventPoints.size(); i++) {

		current = allEventPoints[i]->getPoint();
		current->getPoint(data);
		if (abs(data[0] - temp[0]) < .0001) {
			for (int j = 0; j < allEventPoints[i]->getFlipIndices().size();
					j++) {
				if (allEventPoints[i]->getFlipIndices()[j]->second == flip) {
					return allEventPoints[i];
				}
			}
			//in case the point has been added as a root but hasn't been visited yet
			if(allEventPoints[i]->getFlipIndices().empty()){
				if(allEventPoints[i]->getFlip() == flip){
					return allEventPoints[i];
				}
			}
		}
	}
	return NULL;
}

bool AtlasNode::convertTagsToPointers(Orientation* ori, ActiveConstraintRegion* parentACR){

    bool ret = false;

    int entryNode;
    int pointID;
    int flip;
    CayleyPoint* point = nullptr;

    std::vector<std::tuple<int, int, int> > tags = ori->getEntryPointTags();

    bool noTags = tags.empty();

    if (noTags == true) {
        cout << endl << "Error!!!!!!! No tags available in ConvertTags....!" << endl;
    }

    vector<CayleyPoint*> space = parentACR->getSpace();

    for (int k = 0; k < tags.size(); k++) {
        //cout << endl << endl << "No. of tags: " << tags.size() << endl << endl;

        entryNode = std::get < 0 > (tags[k]);

        if(this->getID() != entryNode){
            continue;
        }

        pointID = std::get < 1 > (tags[k]);
        flip = std::get < 2 > (tags[k]);

        for (int z = 0; z < space.size(); z++) {

            if (space[z]->getID() == pointID) {
                point = space[z];
                break;
            }
        }

        if (point == nullptr) {
            ret = false;
        }
        else{
            ori->pushbackEntryPoint(entryNode, point, point->getOrientation(flip));
            ret = true;
        }
    }

return ret;
}



//For a 0D child region and a child flip, find an entryPoint from that region and flip into this AtlasNode


std::pair<CayleyPoint*, int>*
AtlasNode::findEntryPoint(AtlasNode *AtlasNode_0D, int flip) {

//functions for converting tags

    ActiveConstraintRegion * child_acr = new ActiveConstraintRegion();
     ActiveConstraintRegion * parentACR = new ActiveConstraintRegion();
    ThreadShare::save_loader->loadNode(AtlasNode_0D->getID(), child_acr);

    ThreadShare::save_loader->loadNode(this->getID(), parentACR);


    //vector<CayleyPoint*> witSpace = acr->getWitness();

    //cout << endl << "Looking for EP from child flip " << flip << " in node "
           // << AtlasNode_0D->getID() << endl;

    std::pair<CayleyPoint*, int> * ep = NULL;
    std::vector<CayleyPoint*> child_space = child_acr->getWitness();

    //cout << "child_space->getWitness().size() = " << child_space.size() << endl;

		//For every witness in the child do this
    for (int k = 0; k < child_space.size(); k++) {


        vector<Orientation*> child_oris = child_space[k]->getOrientations();

        bool foundPoints = true;
        for (int l = 0; l < child_oris.size(); l++) {

            if (child_oris[l]->getEntryPoints().empty() == true) {
                foundPoints = false;
                foundPoints = convertTagsToPointers(child_oris[l], parentACR);

               /* if(foundPoints == false){
                    cout <<endl<<  "convertTagsToPointers returned false" << endl;
                }*/
            }

            //if(child_oris[l]->getEntryPoints().empty() == false){

            std::vector<std::tuple<int, CayleyPoint*, Orientation*> > entryPoints =
                    child_oris[l]->getEntryPoints();

            for (int m = 0; m < entryPoints.size(); m++) {

               // cout << "Child flip: " << child_oris[l]->getFlipNum() << endl;

                if (std::get < 0 > (entryPoints[m])  == this->getID()
                        && child_oris[l]->getFlipNum() == flip) {

                    CayleyPoint* point = std::get < 1 > (entryPoints[m]);

                    int entryFlip = std::get < 2 > (entryPoints[m])->getFlipNum();

                    ep = new std::pair<CayleyPoint*, int>(point, entryFlip);

                    return ep;
                }
            }
            //}
        }
    }

    return ep;
}



/**	Takes in two <CayleyPoint*, flip>'s and an Atlas pointer and returns a continuous motion path between them if it exists*/
std::vector<std::pair<CayleyPoint*, int>*> AtlasNode::findContinuousPath(
        CayleyPoint* point_1, CayleyPoint* point_2, int flip_1, int flip_2,
        Atlas* atlas) {

    EventPointNode* src;
    EventPointNode* dst;
    std::vector<std::pair<CayleyPoint*, int>*> path;
    int treeNum;

    if (point_1->getOrientation(flip_1) == nullptr
            || point_2->getOrientation(flip_2) == nullptr) {
       // cout
               // << "Error: passed in a point/flip pair without a realization."
                //<< endl;
        return path;
    }

    double temp[6];
    double data[6];
    point_1->getPoint(temp);
    point_2->getPoint(data);
    if (flip_1 == flip_2 && abs(data[0] - temp[0]) < .00001) {
        cout << "These points have the same entry point. Most simple path."
                << endl;
        return path;
    }

    ActiveConstraintRegion *acr = new ActiveConstraintRegion();

    //* If allEntryPoints is unset, set it*/
    if (this->getAllEntryPoints().empty() || this->getFlipSpace().empty()) {
        ThreadShare::save_loader->loadNode(this->getID(), acr);
    }

    if (this->getAllEntryPoints().empty()) {
        //for each point in the space, pushback all <CayleyPoint, int> pairs that are entryPoints

//        vector<pair<CayleyPoint*, int>*> allEntryPoints;
//        vector<AtlasNode*> children = atlas->getChildren(this->getID(), 'a',
//                true);

//        MolecularUnit* MuA = this->getCG()->getMolecularUnitA();

//        MolecularUnit* MuB = this->getCG()->getMolecularUnitB();

//        vector<int> flips;

//        std::pair<CayleyPoint*, int> * ep;

//        for (int i = 0; i < children.size(); i++) {

//            ActiveConstraintRegion* child_acr = children[i]->getACR();
//            std::vector<CayleyPoint*> child_space = child_acr->getSpace();

//            for (int k = 0; k < child_space.size(); k++) {
//                vector<Orientation*> child_oris =
//                        child_space[k]->getOrientations();

//                for (int l = 0; l < child_oris.size(); l++) {

//                    if (child_oris[l]->getHasEntryPoint() == true) {

//                        std::vector<std::tuple<int, CayleyPoint*, Orientation*> > entryPoints =
//                                child_oris[l]->getEntryPoints();

//                        for (int m = 0; m < entryPoints.size(); m++) {

//                            if (std::get < 0 > (entryPoints[l])
//                                    == this->getID()) {

//                                CayleyPoint* point = std::get < 1
//                                        > (entryPoints[l]);
//                                int flip = std::get < 2
//                                        > (entryPoints[l])->getFlipNum();
//                                ep = new std::pair<CayleyPoint*, int>(point,
//                                        flip);
//                                allEntryPoints.push_back(ep);

//                            }
//                        }
//                    }
//                }
//            }
//        }

        vector<pair<CayleyPoint*, int>* > entryPoints = atlas->preProcessEntryPoints(this->getID());

        setAllEntryPoints(entryPoints);
    }

    bool check_1 = checkEntryPoint(point_1, flip_1);
    bool check_2 = checkEntryPoint(point_2, flip_2);

    if (check_1 == false || check_2 == false) {
        cout << "One of the inputs is not an entry point. " << endl;
        return path;
    }


    /** If flipSpace is not set, set it **/

    if (this->getFlipSpace().empty()) {

        vector<CayleyPoint*> sortedSpace = sortSpace(acr);

        splitSpace(sortedSpace);
    }

    /************

     // BEGIN bunch of out statements for testing
     cout << endl << endl;

     cout << "input to findContinuous path. In 1D node " << this->getID() << endl
     << "Entry point 1: ";
     point_1->printData();

     cout << "flip 1: " << flip_1 << endl << "Entry point 2: ";
     point_2->printData();
     cout << "flip 2: " << flip_2 << endl << endl;

     for (int z = 0; z < flipSpace[flip_1]->size(); z++) {
     if ((*flipSpace[flip_1])[z] != nullptr) {
     if ((*flipSpace[flip_1])[z]->first == nullptr) {
     cout
     << " This is bad: array slot is non null but cayleyPoint is null "
     << endl;
     } else {
     double temp[6];
     (*flipSpace[flip_1])[z]->first->getPoint(temp);
     cout << temp[0] << endl;
     }
     }

     else if ((*flipSpace[flip_1])[z] == nullptr) {
     cout << "nullptr" << endl;
     }
     }
     cout << endl;

     if (flip_1 != flip_2) {
     for (int z = 0; z < flipSpace[flip_2]->size(); z++) {
     if ((*flipSpace[flip_2])[z] != nullptr) {
     if ((*flipSpace[flip_2])[z]->first == nullptr) {
     cout
     << " This is bad: array slot is non null but cayleyPoint is null "
     << endl;
     //<< (*flipSpace[y])[z]->first << endl;
     } else {
     double temp[6];
     (*flipSpace[flip_2])[z]->first->getPoint(temp);
     cout << temp[0] << endl;
     }
     }

     else if ((*flipSpace[flip_2])[z] == nullptr) {
     cout << "nullptr" << endl;
     }
     }
     cout << endl;
     }


     //END OUT STATEMENTS FOR TESTING
     *
     *
     ***************/

//case 1: There are no trees yet; this is the first search
    if (EventPointForest.empty()) {

        src = new EventPointNode(point_1, flip_1, false, false, 0, 0);
        dst = new EventPointNode(point_2, flip_2, false, false, 0, -1);
        ////findInitialIndex(src, flip_1);
        EventPointForest.push_back(src);
        allEventPoints.push_back(src);
				stringstream ss;
				ss << src->getPoint()->getID() << "," << src->getFlip();
				string hkey = ss.str();
				allEventPointsHash.insert(make_pair(hkey,src));
        treeCompleted.push_back(false);
        //allEventPoints.push_back(dst);

//Explore from src, until either dst is found or BFS finishes
        EventPointNode **dstPtr = &dst;
        bool pathBool = buildDFStree(0, dstPtr, EventPointForest[0], flipSpace);

        if (pathBool == true) {
            path = findTreePath(src, dst, flipSpace, flip_1, flip_2);
        }

        /****
         *
         * Better solution to above comment: just don't pushback dst onto allEventPoints at the beginning
         if(pathBool == false){
         EventPointForest.push_back(dst);
         treeCompleted.push_back(false);
         dst->setComponent(EventPointForest.size() -1 );
         }
         *******/
    }

    else if (!EventPointForest.empty()) {
//find the points, if they exist, else return NULL
        src = findCorrespondingEP(point_1, flip_1);
        dst = findCorrespondingEP(point_2, flip_2);

        /**case 2: both points have not been discovered yet; they are either in an incomplete tree, or a new tree */
        if (src == NULL && dst == NULL) {

            src = new EventPointNode(point_1, flip_1, false, false, 0, -1);

            dst = new EventPointNode(point_2, flip_2, false, false, 0, -1);

            //if src and dst are ever found, set to true
            bool srcvar = false;
            bool dstvar = false;

            //loop through all incomplete trees
            for (int i = 0; i < treeCompleted.size(); i++) {
                if (treeCompleted[i] == false) {
                    EventPointNode **dstPtr = &dst;
                    bool dstInTree = buildDFStree(i, dstPtr,
                            EventPointForest[i], flipSpace);

                    EventPointNode **srcPtr = &src;

                    bool srcInTree = buildDFStree(i, srcPtr,
                            EventPointForest[i], flipSpace);

                    /*******
                     if(srcInTree == true){
                     src->setComponent(i);
                     }

                     if(dstInTree == true){
                     dst->setComponent(i);
                     }

                     if((srcInTree == false && dstInTree == true )|| (srcInTree == true && dstInTree == false) ){
                     //They are not in the same tree, there is no path
                     break;
                     }
                     ***********/

                    if (srcInTree == true && dstInTree == true) {
                        path = findTreePath(src, dst, flipSpace, flip_1,
                                flip_2);
                        break;
                    }

                    if (srcInTree == true) {
                        srcvar = true;
                    }
                    if (dstInTree == true) {
                        dstvar = true;
                    }
                }
            }

            //if dst or src is not in a tree, create new tree with src as the root
            if (srcvar == false && dstvar == false) {
                EventPointForest.push_back(src);
                src->setComponent(EventPointForest.size() - 1);
								stringstream ss;
								ss << src->getPoint()->getID() << "," << src->getFlip();
								allEventPoints.push_back(src);
                treeCompleted.push_back(false);

                EventPointNode **dstPtr = &dst;
                bool pathBool = buildDFStree(EventPointForest.size() - 1,
                        dstPtr, EventPointForest[EventPointForest.size() - 1],
                        flipSpace);

                if (pathBool == true) {
                    path = findTreePath(src, dst, flipSpace, flip_1, flip_2);
                }
            }
        }

        /**Case 3: 1 entry point has been found in an existing tree, the other has not */
        else if (src == NULL && dst != NULL) {

            treeNum = dst->getComponent();

            src = new EventPointNode(point_1, flip_1, false, false, 0, -1);

            EventPointNode **srcPtr = &src;

            bool pathBool = buildDFStree(treeNum, srcPtr,
                    EventPointForest[treeNum], flipSpace);

            if (pathBool == true) {
                path = findTreePath(src, dst, flipSpace, flip_1, flip_2);
            }

        }

        else if (src != NULL && dst == NULL) {

            treeNum = src->getComponent();

            dst = new EventPointNode(point_2, flip_2, false, false, 0, -1);

            bool pathBool = buildDFStree(treeNum, &dst,
                    EventPointForest[treeNum], flipSpace);

            if (pathBool == true) {
                path = findTreePath(src, dst, flipSpace, flip_1, flip_2);
            }

        }

//case 4: both points are in an existing tree, see if it is the same one, find path if it is
        else if (src != NULL && dst != NULL) {
            path = findTreePath(src, dst, flipSpace, flip_1, flip_2);

        }
    }

     if(!path.empty()){
        // printEventForest();
     }


    //cout << endl << "findContinuousPath has finished" << endl << endl;

    /******
     if (path.size() == 0) {
     cout << "Got an empty path." << endl;
     } else {
     cout << "path found: " << endl;
     double temp[6];
     for (int i = 0; i < path.size(); i++) {
     if (path[i] != nullptr) {
     if (path[i]->first != nullptr) {
     path[i]->first->getPoint(temp);
     cout << "Step " << i << ": " << temp[0] << " on flip "
     << path[i]->second << endl;
     } else
     cout << "The " << i
     << " pair in path has first element null" << endl;
     } else
     cout << "null cp in the path vector" << endl << endl;
     }
     }

     *************/

    /*****
     double dist;
     MuA = this->getCG()->getMolecularUnitA();
     MuB = this->getCG()->getMolecularUnitB();

     for(int i = 0; i < 8 ; i++){
     for()
     Orientation* ori_1 = (*flipSpace[5])[53]->first->getOrientation(i);
     Orientation* ori_2 =

     dist = findL2Distance(MuA, MuB, );

     cout << dist

     }
     *********/

    return path;
}
