#ifndef EVENTPOINTNODE_H_
#define EVENTPOINTNODE_H_

#include "CayleyPoint.h"
#include <unordered_map>

class EventPointNode {

public:

	EventPointNode(CayleyPoint* point, int flip, bool visited, bool explored,
			int type, int component);
	~EventPointNode();

	bool getVisited();
	bool getExplored();
	int getFlip();
	CayleyPoint* getPoint();
	int getType();
	int getComponent();
	EventPointNode* getParent();
	vector<EventPointNode*> getChildren() const;
	vector< pair<int, int> *> getFlipIndices();
	int getIndexOfFlip(int);
  CayleyPoint* getPoint() const;
	void printData() const;
	void setVisited(bool);
	void setFlip(int flip);
	void setPoint(CayleyPoint* point);
	void setType(int);
	void setComponent(int);
	void setParent(EventPointNode*);
	void setChildren(vector<EventPointNode*>);
	void addChild(EventPointNode*);
	void setExplored(bool);
	void setFlipIndices(vector<pair<int,int>* >);
	void pushbackFlipIndices(pair<int,int>*);

private:

	int flip;
	vector<pair<int, int> *> flipIndices; //gives the index or indices in the flip array(s) of the EPN
	CayleyPoint* point;
	bool visited;
	bool explored;			//true if all children of that node have been discovered
	int type;				// 0-entry, 1-bifurcation
	int component;			//gives the index of the root in AtlasNode::eventPointForest
	EventPointNode* parent;
	vector<EventPointNode*> children;
	unordered_map<string, EventPointNode*> childrenHash;

};
#endif EVENTPOINTNODE_H_
