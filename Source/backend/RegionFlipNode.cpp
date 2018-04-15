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
 * RegionFlipNode.cpp
 *
 *  Created on: Mar 4, 2017
 *  Author: Rahul Prabhu
 */

#include "RegionFlipNode.h"

#include <time.h>
#include <string>
#include <fstream>

using namespace std;


RegionFlipNode::RegionFlipNode(AtlasNode *region, int flip, int nodeNum){
	this->region = region;
	this->flip = flip;
	this->visited = false;
	this->parent = NULL;
	this->nodeNum = nodeNum;
}


//Getters
int RegionFlipNode::getFlip(){
	return flip;
}

AtlasNode* RegionFlipNode::getRegion(){
	return region;
}

int RegionFlipNode::getNodeNum() {
	return nodeNum;
}

bool RegionFlipNode::getVisited(){
	return visited;
}


RegionFlipNode* RegionFlipNode::getParent(){
	return parent;
}

vector<RegionFlipNode *> RegionFlipNode::getadjList(){
	return adjList;
}

//Setters
void RegionFlipNode::setFlip(int flip){
	this->flip = flip;
}

void RegionFlipNode::setRegion(AtlasNode* region ){
	this->region = region;
}

void RegionFlipNode::setVisited(bool visited){
	this->visited = visited;
}

void RegionFlipNode::setParent(RegionFlipNode *parent){
	this->parent = parent;
}

void RegionFlipNode::setadjList(vector<RegionFlipNode *> adjList){
	this->adjList = adjList;
}

void RegionFlipNode::addChild(RegionFlipNode *child) {
	adjList.push_back(child);
}
