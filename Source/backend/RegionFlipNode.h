#ifndef REGIONFLIPGRAPH_H_
#define REGIONFLIPGRAPH_H_

#include "AtlasNode.h"

class RegionFlipNode{

public:

	RegionFlipNode(AtlasNode *region, int flip, int nodeNum);
	~RegionFlipNode();

	//Getters
	int getFlip();
	AtlasNode* getRegion();
	int getNodeNum();
	bool getVisited();
	RegionFlipNode* getParent();
	std::vector<RegionFlipNode *> getadjList();

	//Setters
	void setFlip(int flip);
	void setRegion(AtlasNode*);
	void setVisited(bool);
	void setParent(RegionFlipNode*);
	void setadjList(std::vector<RegionFlipNode *>);

	//Methods
	void addChild(RegionFlipNode *);



private:

	int flip;
	int nodeNum;
	AtlasNode *region;
	bool visited;
	RegionFlipNode *parent;
	std::vector<RegionFlipNode*> adjList;

};

#endif REGIONFLIPGRAPH_H_
