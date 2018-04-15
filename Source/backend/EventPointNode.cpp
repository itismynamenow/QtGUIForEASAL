#include "EventPointNode.h"

EventPointNode::EventPointNode(CayleyPoint* point, int flip, bool visited,
		bool explored, int type, int component) {

	this->point = point;
	this->flip = flip;
	this->visited = visited;
	this->explored = explored;
	this->type = type;
	this->component = component;
	this->parent = nullptr;

}

void EventPointNode::printData() const{
    cout << "Parent: "	<< parent << endl;
    cout << "Component: "	<< component<< endl;
    cout << "Visited: "	<< visited << endl;
    cout << "Explored "	<< explored << endl;
    cout << "Type: ";
    if(type == 1)
        cout << "bifurcation";

    else
        cout << "entry point";

    cout << endl << "Point: ";
    point->printData();
    cout << "EventPoint address: " << this << endl;
    cout <<  "Flips: " << flip << endl;
    for(int i = 0; i< flipIndices.size(); i++){
        cout <<"Flip " << flipIndices[i]->second << " is at index " << flipIndices[i]->first << endl;
    }

}

bool EventPointNode::getVisited() {
	return this->visited;
}

bool EventPointNode::getExplored() {
	return this->explored;
}

int EventPointNode::getFlip() {
	return this->flip;
}

CayleyPoint* EventPointNode::getPoint() {
	return this->point;
}

vector<pair<int, int> *> EventPointNode::getFlipIndices() {
	return this->flipIndices;
}

int EventPointNode::getIndexOfFlip(int flip) {

	vector<pair<int, int> *> temp = this->getFlipIndices();
	for (int i = 0; i < temp.size(); i++) {
		if (temp[i]->second == flip) {
			return temp[i]->first;
		}
	}
	return -1;
}

int EventPointNode::getComponent() {
	return this->component;

}

int EventPointNode::getType() {
	return this->type;
}

vector<EventPointNode*> EventPointNode::getChildren() const {
	return this->children;
}

EventPointNode* EventPointNode::getParent() {
	return this->parent;
}

void EventPointNode::setVisited(bool visisted) {
	this->visited = visited;
}

void EventPointNode::setFlip(int flip) {
	this->flip = flip;
}

void EventPointNode::setPoint(CayleyPoint* point) {
	this->point = point;
}

void EventPointNode::setComponent(int component) {
	this->component = component;

}
//first int is the index, second int is flip
void EventPointNode::setFlipIndices(vector<pair<int, int>*> flipIndices) {
	this->flipIndices = flipIndices;

}

void EventPointNode::pushbackFlipIndices(pair<int, int>* index) {
	this->flipIndices.push_back(index);
}

void EventPointNode::setExplored(bool explored) {
	this->explored = explored;
}

void EventPointNode::setType(int type) {
	this->type = type;
}

void EventPointNode::setParent(EventPointNode* parent) {
	this->parent = parent;
}

void EventPointNode::setChildren(vector<EventPointNode*> children) {
	this->children = children;
}

CayleyPoint* EventPointNode::getPoint() const {
	return point;
}

void EventPointNode::addChild(EventPointNode* child) {
	stringstream ss;
	ss << child->getPoint()->getID() << "," << child->getFlip();
	string hkey = ss.str();
	if (childrenHash.find(hkey)==childrenHash.end()) {
		childrenHash.insert(make_pair(hkey,child));
			this->children.push_back(child);
	}
}
