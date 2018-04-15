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
 * CayleyPoint.cpp
 *
 *  Created on: Feb 22, 2010
 *      Author: James Pence
 */

#include "CayleyPoint.h"

#include "Utils.h"
#include "Settings.h"

#include "Eigen/Core"
#include "Eigen/Geometry"
#include "Eigen/LU"
#include "Eigen/Dense"

using namespace std;

using namespace Eigen;

using Eigen::Vector3d;
using Eigen::Matrix3d;
using Eigen::Quaterniond;
using Eigen::MatrixXd;
using Eigen::VectorXd;


void CayleyPoint::init_() {
    this->realizable = true;

    this->axis = -1;

    this->zIndex = 0;
    this->badAngleN = 0;
    this->collidN = 0;
}

CayleyPoint::CayleyPoint() {
    this->init_();
}

CayleyPoint::CayleyPoint(vector<double> values) {
    this->init_();

    for (size_t i = 0; i < values.size(); i++) {
        if (!std::isnan(values[i])) {
            this->data.push_back(values[i]);
        } else {
            this->data.push_back(-1.0);
        }
    }
    this->showPath = false;
}



CayleyPoint::CayleyPoint(Orientation* orient, vector<pair<int,int> > paramlines, MolecularUnit* helA, MolecularUnit* helB ){
    this->init_();


    double fb[3][3], tb[3][3];
    orient->getFromTo(fb, tb);

    vector<Atom*> xfHelA = helA->getAtoms();
    vector<Atom*> xfHelB = helB->getXFAtoms(fb,tb);

    for(size_t i = 0;i < paramlines.size();i++){
        Atom* atomA = xfHelA[paramlines[i].first];
        Atom* atomB = xfHelB[paramlines[i].second];
        this->data.push_back(Utils::dist(atomA->getLocation(),atomB->getLocation()));
    }

    for(vector<Atom*>::iterator iter = xfHelB.begin();  iter != xfHelB.end();   iter++)
        delete *iter;
    xfHelB.clear();



    this->addOrientation(new Orientation(orient));// using the original pointer would complicate deleting the point.
    this->showPath = false;

}

//New constructor by Brian for labeling Cayleypoints
CayleyPoint::CayleyPoint(Orientation* orient,
        vector<pair<int, int> > paramlines, MolecularUnit* helA,
        MolecularUnit* helB, int pointsCreated) {
    this->init_();

    double fb[3][3], tb[3][3];
    orient->getFromTo(fb, tb);

    vector<Atom*> xfHelA = helA->getAtoms();
    vector<Atom*> xfHelB = helB->getXFAtoms(fb, tb);

    for (size_t i = 0; i < paramlines.size(); i++) {
        Atom* atomA = xfHelA[paramlines[i].first];
        Atom* atomB = xfHelB[paramlines[i].second];
        this->data.push_back(
                Utils::dist(atomA->getLocation(), atomB->getLocation()));
    }

    for (vector<Atom*>::iterator iter = xfHelB.begin(); iter != xfHelB.end();
            iter++)
        delete *iter;
    xfHelB.clear();

    this->addOrientation(new Orientation(orient)); // using the original pointer would complicate deleting the point.
    this->showPath = false;

    //the only difference between this cnstr and the above - Brian
    this->ID = pointsCreated;


    //cout << endl << endl << "CP made with ID: " << this->ID << endl << endl;
}


CayleyPoint::CayleyPoint(const CayleyPoint &p) {
	if(&p == NULL)
		return;
	this->axis = p.axis;
	this->zIndex = p.zIndex;
	this->data = p.data;
	this->realizable = p.realizable;
	//Deep Copy Orientation
	if(p.orient.size() != 0){
		vector<Orientation *> o = p.orient;
		for(vector<Orientation * >::iterator iter = o.begin();iter != o.end();iter++){
			Orientation *orient = new Orientation(**iter);
			this->orient.push_back(orient);
		}
	}
	this->orient = p.orient;
	this->badAngleN = p.badAngleN;
	this->collidN = p.collidN;
	this->showPath = false;

    this->ID = p.ID;


}

CayleyPoint::~CayleyPoint() {
	trim_PointMultiD();
}

void CayleyPoint::trim_PointMultiD() {
    for (size_t i = 0; i< this->orient.size(); i++) {
        delete this->orient[i];
    }
    this->orient.clear();
}


void CayleyPoint::printData(){
    for(size_t i = 0;i < this->data.size();i++){
       if(! std::isnan(data[i]) ){
           cout <<  this->data[i] << "  ";
       }
    }
    cout << this->realizable << " ";
    cout << this->badAngleN << " ";
    cout << this->ID;
    cout << endl;
}

void CayleyPoint::setRealizable(bool setting){
    this->realizable = setting; //false if volume is negative
}



void CayleyPoint::incrementBadAngleN(){
    this->badAngleN++;
}

void CayleyPoint::incrementCollidN(){
    this->collidN++;
}



void CayleyPoint::setBadAngleN(int n){
    this->badAngleN = n;
}

void CayleyPoint::setCollidN(int n){
    this->collidN = n;
}


int CayleyPoint::getBadAngleN(){
    return this->badAngleN;
}

int CayleyPoint::getCollidN(){
    return this->collidN;
}



bool CayleyPoint::isRealizable(){
    return this->realizable;
}


size_t CayleyPoint::dim(){
    return this->data.size();
}


list<int> CayleyPoint::getBoundaries() {
    list<int> ret;

    for (vector<Orientation*>::iterator iter = this->orient.begin();
        iter != this->orient.end();
        iter++)
    {
        vector<int> b = (*iter)->getBoundary();
        for (int i=0; i<b.size(); i++)
                ret.push_back(b[i]);
    }

    ret.sort();
    ret.unique();
    return ret;
}

void CayleyPoint::addOrientation(Orientation* ori) {
    if (ori == NULL) return;

    for (size_t i = 0; i < this->orient.size(); i++)
        if (this->orient[i]->isEqual(ori)) {
            delete ori;
            return;
        }

    this->orient.push_back(ori);
}

bool CayleyPoint::hasOrientation(){
    return !(this->orient.empty());
}

vector<Orientation*> CayleyPoint::getOrientations() {
    return this->orient;
}

bool CayleyPoint::hasOrientation(int flip) {
  for(int i=0; i<orient.size(); i++) {
			if(orient[i]->getFlipNum() == flip) {
				return true;
			}
		}
    return false;
}

Orientation* CayleyPoint::getOrientation(int flip) {
	for(int i=0; i<orient.size(); i++) {
			if(orient[i]->getFlipNum() == flip) {
				return orient[i];
			}
		}
    return NULL;
}

vector<int> CayleyPoint::getFlips() {
	vector<int> flips;
	for(int it=0;it<orient.size();it++) {
		flips.push_back(orient[it]->getFlipNum());
	}
	return flips;
}

void CayleyPoint::setOrientations(vector<Orientation*> oris){
	trim_PointMultiD();
	this->orient = oris;
}

bool CayleyPoint::removeOrientation(Orientation* ori){
    vector<Orientation*>::iterator loc = find(this->orient.begin(), this->orient.end(), ori);

    if (loc == this->orient.end()) return false;

    this->orient.erase(loc);
    delete *loc;
    return true;
}

void CayleyPoint::getPoint(double out[6]){
    for (size_t i = 0; i < 6; i++) {
        if (i < this->data.size()) {
            out[i] = this->data[i];
        } else {
            out[i] = 0;
        }
    }
}

int CayleyPoint::getID(){
    return this->ID;
}

void CayleyPoint::setID(int num){
    this->ID = num;
}


/*
 * strictly a getter operation
 * When used with a pointer you need to be careful to de-reference.
 *
 * " (*ptr)[i] " is correct
 *
 * " ptr[i] " is not correct (WARNING : It will get a value, treating the pointer incorrectly as an array.)
 *
 * and " " (*ptr)[i] = x " won't work... use " ptr->set(i,x) "
 */
double CayleyPoint::operator[](size_t index){
    if(index < this->data.size()){
        return this->data.at(index);
    }
    return 0; // OR -1 ?
}
