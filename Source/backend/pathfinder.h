#ifndef PATHFINDER_H
#define PATHFINDER_H
#include<iostream>
#include<vector>
#include "Atlas.h"
#include "AtlasNode.h"
#include "CayleyPoint.h"
#include "ThreadShare.h"


enum pathTypes {AtlasPath, VoronoiPath, OneDOFPath};

class PathFinder
{
public:
    PathFinder();
    /*
     * Constructor to be used for Atlas Paths
     */
    PathFinder(Atlas*,int, int);
    /*
     * Constructor to be used for Voronoi Paths
     */
    PathFinder(CayleyPoint*, CayleyPoint*, vector<CayleyPoint*>, double);
    /*
     * Constructor to be used for 1DOF Paths
     */
    PathFinder(Atlas *, int, int, int, int);
   /*
     * findAtlasPath
     * This function finds a path between two nodes in the atlas, that have dimension equal to Settings::Paths::energyLowerBound.
     * The paths should only go through nodes that have an energy level between Settings::Paths::energyLowerBound and Settings::Paths::energyUpperBound.
     * In general, we use this to find paths, between 0D nodes, going through 0D and 1D nodes.
     *
     * Input:
     *  nothing.
     *  Though the function does not take any input per say, it uses the following members of the class as input.
     *          - atlas : A sampled atlas
     *          - sourceNode : The source node of the path.
     *          - destinationNode : The destination node of the path.
     *
     * Output:
     *  vector<int> path.
     *    It returns a vector of atlas node numbers.
     *    If it returns an empty atlas node, it means either of the following:
     *          - The source and destination nodes violate the energy bounds.
     *          - There exists no path between the source and destination.
     */
    std::vector<int> findAtlasPath();
    /*
     * findVoronoiPath
     * This function finds a straightline path between two Cayley configurations.
     */
     std::vector<CayleyPoint*> findVoronoiPath();
     /*
      * find1DoFPath
      */
     std::vector<std::pair<CayleyPoint*, int>*> find1DoFPath();
 private:
    int sourceNode;
    int destinationNode;
    int sourceFlip;
    int destinationFlip;
    CayleyPoint *sourcePoint, *destinationPoint;
    vector<CayleyPoint*> space;
    pathTypes pathType;
    Atlas *atlas;
    double stepSize;

};

#endif // PATHFINDER_H
