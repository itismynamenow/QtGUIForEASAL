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

#ifndef ATLASNODE_H_
#define ATLASNODE_H_

#include "ActiveConstraintRegion.h"
#include "ActiveConstraintGraph.h"
#include "EventPointNode.h"


#include <vector>
#include <utility>
#include <queue>
#include <cstddef>
#include <unordered_map>

/**
 * Node in the Atlas, each represents an active constraint region labeled by ActiveConstraintGraph
 */

class Atlas;
class AtlasNode {
public:

	/////////////////////////////////
	// Constructors/Destructors
	/////////////////////////////////

	/** @brief Default constructor, gives basic physical values. */
	AtlasNode();

	/** @brief Constructor that allows initialization of a broad range of values */
	AtlasNode(int ID, bool complete, bool empty, int numdim, double* location,
			std::vector<int> connection);

	/** @brief Destructor that deletes ACG and ACR of instance. */
	virtual ~AtlasNode();

	/////////////////////////////////
	// Location
	/////////////////////////////////

	/**
	 * @brief A setter for the node's 3D coordinates
	 *
	 * @param x The x coordinate to give the atom.
	 * @param y The y coordinate to give the atom.
	 * @param z The z coordinate to give the atom.
	 */
	void setLocation(double x, double y, double z);

	/**
	 * @brief A getter for the atom's 3D coordinates
	 *
	 * @return A pointer to a double array of length 3 (x,y,z coords)
	 */
	double* getLocation();

	/////////////////////////////////
	// Physical Attributes (for display/GUI purpose)
	/////////////////////////////////

	/**
	 * @return The velocity of the AtlasNode.
	 */
	double* getVelocity();
	double* getForce();
	double getForceMag();

	/**
	 * @brief Alters the AtlasNode's location according the current velocity.
	 */
	void applyVelocity(); //todo maybe move this function to atlasdisplay

	/**
	 * @brief Alters the AtlasNode's velocity according the current force.
	 */
	void applyForce(); //todo maybe move this function to atlasdisplay

	/////////////////////////////////
	// Connections
	/////////////////////////////////

	/**
	 * @brief Put a link between this node and the other.
	 */
	void addConnection(int other);

	bool removeConnection(int other);

	/**
	 * @return True if there is an edge/link between this node and the other, False otherwise.
	 */
	bool isConnectedTo(int other);

	/** @return The set of indices of the nodes that this node is connected to */
	std::vector<int> getConnection();

  /* This function will be used to get all flips in witness points. This will mainly be used for pathfinding*/
	void getWitnessFlips(std::vector<int> &v);

	/////////////////////////////////
	// Others
	/////////////////////////////////
	/** @return The Index of the node in the atlas */
	int getID() const;
	void setID(int id);

	/** @return Dimension which is 6 - number_of_contacts */
	int getDim();

	/** @return Sampling dimension. paramdim can be greater than dim in case of short_range_sampling */
	int getParamDim();

	/**
	 * @param complete The boolean whether sampling of the node's region is completed or not
	 */
	void setComplete(bool complete);

	/**
	 * @return True if the sampling is finished, False otherwise.
	 */
	bool isComplete();

	/** @return True if there is at least one accepted Orientation, False otherwise */
	bool hasAnyGoodOrientation();

	void setFoundGoodOrientation(bool s);

	/**
	 * @return The ActiveConstraintGraph of the atlas node
	 */
	ActiveConstraintGraph* getCG();

	void setCG(ActiveConstraintGraph* newID);

	/**
	 * @return The ActiveConstraintRegion of the atlas node
	 */
	ActiveConstraintRegion* getACR();

	void setACR(ActiveConstraintRegion* region);

	/** @brief Cleans the ActiveConstraintRegion and ActiveConstraintGraph of the node */
	void trimNode();

	void setFlipSpace(vector<vector<pair<CayleyPoint*, int>*>* >);

	vector<vector<pair<CayleyPoint*, int>*>* > getFlipSpace();

	void setAllEntryPoints(vector<pair<CayleyPoint*, int> *>);

	vector<pair<CayleyPoint*, int> *> getAllEntryPoints();

	/** searches through the CC of entry point 1, stops when component is complete or ep_2 is found */
	std::vector<std::pair<CayleyPoint*, int>*> findContinuousPath(
			CayleyPoint* point_1, CayleyPoint* point_2, int flip_1, int flip_2, Atlas* atlas);

	/** finds the EventPointNode in all_ep   							 			*/
	EventPointNode* findCorrespondingEP(CayleyPoint* point, int flip);

	/** returns true if ep_1 and ep_2 are in the same connected component			*/
	bool connectedCheck(EventPointNode* ep_1, EventPointNode* ep_2);

	bool buildDFStree(int treeNum,
			EventPointNode** dst, EventPointNode* src,
			vector<vector<pair<CayleyPoint*, int>*>* > flipSpace);


	void printEventForest() const;

	void printTree(const EventPointNode* current) const;

	double findL2Distance(MolecularUnit *MuA, MolecularUnit *MuB, Orientation* o1,
			Orientation *o2);

	std::vector<std::pair<CayleyPoint*, int>*> findTreePath(EventPointNode* src,
			EventPointNode* dst,
			vector<vector<pair<CayleyPoint*, int>*>* > flipSpace, int flip_1,
			int flip_2);


    std::pair<CayleyPoint*, int> *
        findEntryPoint(AtlasNode *AtlasNode_0D, int flip);


	std::vector<CayleyPoint*> sortSpace(ActiveConstraintRegion* region);

	void splitSpace(
			std::vector<CayleyPoint*>);

	int searchSpace(EventPointNode* point,
			std::vector<std::vector<std::pair<CayleyPoint*, int>*>* > flipSpace);

	/** Is given point in region a bifurcation*/
	vector<pair<int, int>*> checkBifurcation(std::pair<CayleyPoint*, int>*, int index,
			vector<vector<pair<CayleyPoint*, int>*>* > flipSpace);

	void testBifurcation(int flip);

	/** Is given point in region an entry point */
	bool checkEntryPoint(std::pair<CayleyPoint*, int>* point);

	bool checkEntryPoint(CayleyPoint* point, int flip);

	/** Given two EPN's that are connected, returns the path denoted by that edge	*/
	vector<pair<CayleyPoint*, int>*> findEdgePath(EventPointNode*,
			EventPointNode*,
			vector<vector<pair<CayleyPoint*, int>*>* > flipSpace, int flip_1, int flip_2);


	void printFlip(int y);

	void setFlipScheme(std::vector< std::vector < int > > flipScheme);

	std::vector<std::vector<int> > getFlipScheme();

    bool convertTagsToPointers(Orientation* ori, ActiveConstraintRegion* parentACR);

	/////////////////////////////////
	// Public variables
	/////////////////////////////////

	std::vector<vector< int > > flipScheme;

	/**All root EventPointNodes*/
	vector<EventPointNode*> EventPointForest;

	/**true if tree is completely explored, false if else  */
	vector<bool> treeCompleted;

	vector<EventPointNode*> allEventPoints;
	unordered_map<string, EventPointNode*> allEventPointsHash;

	vector< pair<CayleyPoint*, int> *>allEntryPoints;

	/** to keep track of this node visited or not through some search and analysis criteria used by Statistic class*/
	bool visited;

	// pthread_spinlock_t nodeInUse;
	// pthread_mutex_t nodeInUse;

	bool dimWritten; //to keep track of dimension is written to the node.txt file or not

private:

	vector<vector<pair<CayleyPoint*, int>*>* > flipSpace;
	/** The Index of the node in the atlas */
	int numID;

	/** Dimension which is 6 - number_of_contacts */
	int dim; // dim should not be paramDim since paramDim can be same for all nodes in 6d sampling (i.e. paramdim=6 for n=2 molecules) for all nodes, in case of short-distance sampling hence will prevent distinguishing the display of nodes.

	/** True if the sampling is finished, False otherwise. */
	bool complete;

	/**
	 * True if there is no accepted Orientation, False if there is one or more accepted Orientations
	 * if True, then do not display the node
	 */
	bool noGoodOrientation;

	/** The set of IDs of the nodes that this node is connected to */
	std::vector<int> connection;

	/** The active constraint graph that labels the node. */
	ActiveConstraintGraph *constraintGraph;

	/** The set of Cayley points in the active region. */
	ActiveConstraintRegion * region;

	/////////////////////////////////
	// Physical Attributes (for display/GUI purpose)
	/////////////////////////////////
	double loc[3];
	double vel[3], velMag;
	double force[3], forceMag;

public:
	int numPass;

};

#endif /* ROADNODE_H_ */
