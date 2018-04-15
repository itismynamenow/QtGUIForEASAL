#include "pathfinder.h"

#include<vector>
#include<queue>

PathFinder::PathFinder(Atlas *atlas, int sourceNode, int destinationNode) {
    this->atlas = atlas;
    this->sourceNode = sourceNode;
    this->destinationNode = destinationNode;
    this->pathType = AtlasPath;
}


PathFinder::PathFinder(Atlas *atlas, int srcNode, int sourceFlip, int dstNode, int destinationFlip) {
    this->atlas = atlas;
    this->sourceNode = srcNode;
    this->sourceFlip = sourceFlip;
    this->destinationNode = dstNode;
    this->destinationFlip = destinationFlip;
    this->pathType = OneDOFPath;
}

PathFinder::PathFinder(CayleyPoint *src, CayleyPoint *dst, vector<CayleyPoint *> space, double stepSize) {
    this->sourcePoint = src;
    this->destinationPoint = dst;
    this->space = space;
    this->stepSize = stepSize;
    this->pathType = VoronoiPath;
}

//Helper method to find the index of a node in the newly built graph
int findIndex(vector<AtlasVertex*> Graph, int num) {
    for (int i = 0; i < Graph.size(); i++)
        if (Graph[i]->number == num)
            return i;
    return -1;
}

std::vector<int> PathFinder::findAtlasPath() {

    std::vector<int> atlasPath;

    if(pathType != AtlasPath) {
        cout<<"Wrong arguments passed!"<<endl;
        return atlasPath;
    }

    std::queue<int> Q;


    //Temporary structure we build to search through the graph.
    vector<AtlasVertex*> Graph;

    Q.push(sourceNode);

    clock_t begin = clock();

    atlas->BuildTree(Graph);

    if (findIndex(Graph, sourceNode) == -1 || findIndex(Graph, destinationNode) == -1) {
        return atlasPath;
    }

    while (!Q.empty()) {
        int current = Q.front();
        Q.pop();
        AtlasVertex *now = Graph[findIndex(Graph, current)];
        Graph[findIndex(Graph, current)]->visited = true;
        for (int i = 0; i < now->adjList.size(); i++) {
            if (Graph[findIndex(Graph, now->adjList[i])]->visited != true) {
                Q.push(now->adjList[i]);
                Graph[findIndex(Graph, now->adjList[i])]->visited = true;
                Graph[findIndex(Graph, now->adjList[i])]->parent = current;
            }
        }
    }

    int cur = destinationNode;
    while (cur != sourceNode) {
        atlasPath.push_back(cur);
        if (Graph[findIndex(Graph, cur)]->parent == -1) {
            atlasPath.clear();
            clock_t end = clock();
            double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
            return atlasPath;
        }
        cur = Graph[findIndex(Graph, cur)]->parent;
    }
    clock_t end = clock();
    atlasPath.push_back(cur);
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

    for (int i = 0; i < Graph.size(); i++)
        delete Graph[i];

    return atlasPath;
}

vector<vector<vector<CayleyPoint*> > > buildArray(vector<CayleyPoint*> src,
        double stepSize) {

    //used to determine range
    vector<double> minParam;
    vector<double> maxParam;

    double min;
    double max;

    //Find
    double temp[6];

    src[0]->getPoint(temp);

    min = temp[0];
    max = min;

    for (int i = 0; i < src.size(); i++) { //Finds min and max first parameter
        src[i]->getPoint(temp);

        if (max < temp[0]) {
            max = temp[0];
        }

        if (min > temp[0]) {
            min = temp[0];
        }
    }

    minParam.push_back(min);
    maxParam.push_back(max);

    min = temp[1];
    max = min;

    for (int i = 0; i < src.size(); i++) {
        src[i]->getPoint(temp);
        if (max < temp[1]) {
            max = temp[1];
        }

        if (min > temp[1]) {
            min = temp[1];
        }
    }

    minParam.push_back(min);
    maxParam.push_back(max);

    min = temp[2];
    max = min;
    for (int i = 0; i < src.size(); i++) {
        src[i]->getPoint(temp);

        if (max < temp[2]) {
            max = temp[2];
        }

        if (min > temp[2]) {
            min = temp[2];
        }
    }

    minParam.push_back(min);
    maxParam.push_back(max);

    //added ceil function to avoid non int values (though this didn't seem to cause a problem)
    int height = ceil(abs(maxParam[0] - minParam[0]) / stepSize);
    int width = ceil(abs(maxParam[1] - minParam[1]) / stepSize);
    int depth = ceil(abs(maxParam[2] - minParam[2]) / stepSize);

    vector<vector<vector<CayleyPoint*> > > array3D;
    array3D.resize(height);

    for (int i = 0; i < height; i++) {
        array3D[i].resize(width);

        for (int j = 0; j < width; j++) {
            array3D[i][j].resize(depth);
        }
    }

    //added floor function to try to resolve seg fault

    for (int i = 0; i < src.size(); i++) {
        src[i]->getPoint(temp);
        array3D[floor((temp[0] - minParam[0]) / stepSize)][floor(
                (temp[1] - minParam[1]) / stepSize)][floor(
                (temp[2] - minParam[2]) / stepSize)] = src[i];
    }

    return array3D;
}

void findBounds(vector<CayleyPoint*> src, vector<double> &minParam, vector<double> &maxParam) {

    double min;
    double max;

    //Find
    double temp[6];

    src[0]->getPoint(temp);

    min = temp[0];
    max = min;
    for (int i = 0; i < src.size(); i++) { //Finds min and max first parameter
        src[i]->getPoint(temp);

        if (max < temp[0]) {
            max = temp[0];
        }

        if (min > temp[0]) {
            min = temp[0];
        }
    }
    minParam.push_back(min);
    maxParam.push_back(max);

    min = temp[1];
    max = min;
    for (int i = 0; i < src.size(); i++) {
        src[i]->getPoint(temp);
        if (max < temp[1]) {
            max = temp[1];
        }

        if (min > temp[1]) {
            min = temp[1];
        }
    }
    minParam.push_back(min);
    maxParam.push_back(max);

    min = temp[2];
    max = min;
    for (int i = 0; i < src.size(); i++) {
        src[i]->getPoint(temp);

        if (max < temp[2]) {
            max = temp[2];
        }

        if (min > temp[2]) {
            min = temp[2];
        }
    }

    minParam.push_back(min);
    maxParam.push_back(max);
}



vector<double> computeLine(int src[3], int end[3]) {
    vector<double> line;
    line.push_back(src[0]);
    line.push_back(src[1]);
    line.push_back(src[2]);

    line.push_back(end[0] - src[0]);
    line.push_back(end[1] - src[1]);
    line.push_back(end[2] - src[2]);

    return line;
}

vector<vector<int> > findNeighbors(vector<double> intr) {

// 1) check to see how many cells an intersect borders
// 2) return the indices of those cells

    vector<CayleyPoint*> neighborPoints;

    int x_1;
    int y_1;
    int z_1;
    int x_2;
    int y_2;
    int z_2;

    bool x_intr;
    bool y_intr;
    bool z_intr;

    vector<vector<int> > neighborIndices;

    char num[20];
    double intr_i;
    sprintf(num, "%.2f", intr[0]);
    intr_i = atof(num);

    if ((floor(intr_i - .5) - (intr_i - .5)) == 0) { //if point lies on an x-grid line
        x_intr = true;
        x_1 = intr_i + .5;
        x_2 = intr_i - .5;
    } else {
        x_intr = false;
        x_1 = floor(intr_i + .5);
    }

    sprintf(num, "%.2f", intr[1]);
    intr_i = atof(num);

    if ((floor(intr_i - .5) - (intr_i - .5)) == 0) { //if point lies on an y-grid line
        y_intr = true;
        y_1 = intr_i + .5;
        y_2 = intr_i - .5;
    } else {
        y_intr = false;
        y_1 = floor(intr[1] + .5);
    }

    sprintf(num, "%.2f", intr[2]);
    intr_i = atof(num);

    if (floor(intr_i - .5) - (intr_i - .5) == 0) { //if point lies on an z-grid line
        z_intr = true;
        z_1 = intr_i + .5;
        z_2 = intr_i - .5;
    } else {
        z_intr = false;
        z_1 = floor(intr_i + .5);
    }

//split up 7 different cases

//neighborIndices.clear();

    if (x_intr && !y_intr && !z_intr) {				//x_intr only
        neighborIndices.push_back( { x_1, y_1, z_1 });
        neighborIndices.push_back( { x_2, y_1, z_1 });

    }

    else if (!x_intr && y_intr && !z_intr) {				//y_intr only
        neighborIndices.push_back( { x_1, y_1, z_1 });
        neighborIndices.push_back( { x_1, y_2, z_1 });

    }

    else if (!x_intr && !y_intr && z_intr) {				//z_intr only
        neighborIndices.push_back( { x_1, y_1, z_1 });
        neighborIndices.push_back( { x_1, y_1, z_2 });
    }

    else if (x_intr && y_intr && !z_intr) {	//x_intr and y_intr only, 4 neighboring cells
        neighborIndices.push_back( { x_1, y_1, z_1 });
        neighborIndices.push_back( { x_2, y_1, z_1 });

        neighborIndices.push_back( { x_1, y_2, z_1 });
        neighborIndices.push_back( { x_2, y_2, z_1 });

    }

    else if (x_intr && !y_intr && z_intr) {	//x_intr and z_intr only, 4 neighboring cells
        neighborIndices.push_back( { x_1, y_1, z_1 });
        neighborIndices.push_back( { x_2, y_1, z_1 });

        neighborIndices.push_back( { x_1, y_1, z_2 });
        neighborIndices.push_back( { x_2, y_1, z_2 });
    }

    else if (!x_intr && y_intr && z_intr) {	//y_intr and z_intr only, 4 neighboring cells
        neighborIndices.push_back( { x_1, y_1, z_1 });
        neighborIndices.push_back( { x_1, y_2, z_1 });

        neighborIndices.push_back( { x_1, y_1, z_2 });
        neighborIndices.push_back( { x_1, y_2, z_2 });

    }

    else if (x_intr && y_intr && z_intr) {
        neighborIndices.push_back( { x_1, y_1, z_1 });
        neighborIndices.push_back( { x_2, y_1, z_1 });

        neighborIndices.push_back( { x_1, y_2, z_1 });
        neighborIndices.push_back( { x_2, y_2, z_1 });

        neighborIndices.push_back( { x_1, y_1, z_2 });
        neighborIndices.push_back( { x_2, y_1, z_2 });

        neighborIndices.push_back( { x_1, y_2, z_2 });
        neighborIndices.push_back( { x_2, y_2, z_2 });
    }

    return neighborIndices;
}

vector<double> computeIntersection(vector<double> line, vector<double> plane) {

    vector<double> intr;
    intr.clear();
    //line variables
    double x0 = line[0];
    double y0 = line[1];
    double z0 = line[2];
    double a = line[3];
    double b = line[4];
    double c = line[5];

    //plane variables
    double xp = plane[0];
    double yp = plane[1];
    double zp = plane[2];
    double d = plane[3];
    double e = plane[4];
    double f = plane[5];
    //solve for t (plug in x0 for x etc...

    if ((-a * d) - (b * e) - (c * f) != 0) {

        double t = ((d * (x0 - xp)) + (e * (y0 - yp)) + (f * (z0 - zp)))
                / ((-a * d) - (b * e) - (c * f));//denominator should only be zero if line lies in the plane

        char num[20];
        float x, y, z;
        sprintf(num, "%.2f", x0 + a * t);			//to solve precision problem
        x = atof(num);
        sprintf(num, "%.2f", y0 + b * t);
        y = atof(num);
        sprintf(num, "%.2f", z0 + c * t);
        z = atof(num);

        intr.push_back(x);
        intr.push_back(y);
        intr.push_back(z);

    }

    return intr;

}

vector<vector<double> > mergeLists(int src[3], int dst[3],
        vector<vector<double> > x_intr, vector<vector<double> > y_intr,
        vector<vector<double> > z_intr) {

    vector<vector<double> > xyList;
    vector<vector<double> > all_intr;
    vector<bool> intersectionTypes;	//to record which line(s) each intersection lies on
    vector<vector<bool> > AllIntersectionTypes;		//list of all intersections

    //Break into cases, if no change in x dim, sort by y, if no change in x or y, sort by z
    //First reverse if necessary according to each dimension

    if (src[0] - dst[0] != 0) {

        if (!x_intr.empty() && x_intr.back()[0] - x_intr.front()[0] < 0) {
            //reverse if x-values are decreasing from src to dst
            reverse(x_intr.begin(), x_intr.end());
        }

        if (!y_intr.empty() && y_intr.back()[0] - y_intr.front()[0] < 0) {
            //reverse if --x values-- are decreasing for the "y intersections"
            reverse(y_intr.begin(), y_intr.end());
        }

        if (!z_intr.empty() && z_intr.back()[0] - z_intr.front()[0] < 0) {
            //reverse if --x values and y values-- are decreasing for the "z intersections"
            reverse(z_intr.begin(), z_intr.end());
        }

        while (!x_intr.empty() && !y_intr.empty()) {
            if (x_intr[0][0] < y_intr[0][0]) {
                xyList.push_back(x_intr[0]);
                x_intr.erase(x_intr.begin());
            }

            else {
                xyList.push_back(y_intr[0]);
                y_intr.erase(y_intr.begin());
            }
        }

        if (!x_intr.empty()) {
            //add the remaining values from the non-empty vector
            for (int i = 0; i < x_intr.size(); i++) {
                xyList.push_back(x_intr[i]);
            }
        }

        if (!y_intr.empty()) {
            //add the remaining values from the non-empty vector
            for (int i = 0; i < y_intr.size(); i++) {
                xyList.push_back(y_intr[i]);
            }
        }

        while (!xyList.empty() && !z_intr.empty()) {
            //merge xyList and z_intr, same way
            if (xyList[0][0] < z_intr[0][0]) {
                all_intr.push_back(xyList[0]);
                xyList.erase(xyList.begin());
            } else {
                all_intr.push_back(z_intr[0]);
                z_intr.erase(z_intr.begin());
            }

        }

        if (!xyList.empty()) {
            //add the remaining values from the non-empty vector
            for (int i = 0; i < xyList.size(); i++) {
                all_intr.push_back(xyList[i]);
            }
        }

        if (!z_intr.empty()) {
            //add the remaining values from the non-empty vector
            for (int i = 0; i < z_intr.size(); i++) {
                all_intr.push_back(z_intr[i]);
            }
        }
    }

    else if (src[1] - dst[1] != 0) {

        if (!x_intr.empty() && x_intr.back()[1] - x_intr.front()[1] < 0) {
            //reverse if y-values of x_intr are decreasing src to dst
            reverse(x_intr.begin(), x_intr.end());
        }

        if (!y_intr.empty() && y_intr.back()[1] - y_intr.front()[1] < 0) {
            //reverse if --x values-- are decreasing for the "y intersections"
            reverse(y_intr.begin(), y_intr.end());
        }

        if (!z_intr.empty() && z_intr.back()[1] - z_intr.front()[1] < 0) {
            // "" for z intersections
            reverse(z_intr.begin(), z_intr.end());
        }

        while (!x_intr.empty() && !y_intr.empty()) {
            if (x_intr[0][1] < y_intr[0][1]) {
                xyList.push_back(x_intr[0]);
                x_intr.erase(x_intr.begin());
            }

            else {
                xyList.push_back(y_intr[0]);
                y_intr.erase(y_intr.begin());
            }
        }

        if (!x_intr.empty()) {
            //add the remaining values from the non-empty vector
            for (int i = 0; i < x_intr.size(); i++) {
                xyList.push_back(x_intr[i]);
            }
        }

        if (!y_intr.empty()) {
            //add the remaining values from the non-empty vector
            for (int i = 0; i < y_intr.size(); i++) {
                xyList.push_back(y_intr[i]);
            }
        }

        while (!xyList.empty() && !z_intr.empty()) {
            //merge xyList and z_intr, same way
            if (xyList[0][1] < z_intr[0][1]) {
                all_intr.push_back(xyList[0]);
                xyList.erase(xyList.begin());
            } else {
                all_intr.push_back(z_intr[0]);
                z_intr.erase(z_intr.begin());
            }

        }

        if (!xyList.empty()) {
            //add the remaining values from the non-empty vector
            for (int i = 0; i < xyList.size(); i++) {
                all_intr.push_back(xyList[i]);
            }
        }

        if (!z_intr.empty()) {
            //add the remaining values from the non-empty vector
            for (int i = 0; i < z_intr.size(); i++) {
                all_intr.push_back(z_intr[i]);
            }
        }

    }

    else if (src[2] - dst[2] != 0) {

        if (!x_intr.empty() && x_intr.back()[2] - x_intr.front()[2] < 0) {
            reverse(x_intr.begin(), x_intr.end());
            //reverse if z-values of x_intr are decreasing src to dst
        }

        if (!y_intr.empty() && y_intr.back()[2] - y_intr.front()[2] < 0) {
            //reverse if --z values-- are decreasing for the "y intersections"
            reverse(y_intr.begin(), y_intr.end());
        }

        if (!z_intr.empty() && z_intr.back()[2] - z_intr.front()[2] < 0) {
            // "" for z intersections
            reverse(z_intr.begin(), z_intr.end());
        }

        while (!x_intr.empty() && !y_intr.empty()) {
            if (x_intr[0][2] < y_intr[0][2]) {
                xyList.push_back(x_intr[0]);
                x_intr.erase(x_intr.begin());
            }

            else {
                xyList.push_back(y_intr[0]);
                y_intr.erase(y_intr.begin());
            }
        }

        if (!x_intr.empty()) {
            //add the remaining values from the non-empty vector
            for (int i = 0; i < x_intr.size(); i++) {
                xyList.push_back(x_intr[i]);
            }
        }

        if (!y_intr.empty()) {
            //add the remaining values from the non-empty vector
            for (int i = 0; i < y_intr.size(); i++) {
                xyList.push_back(y_intr[i]);
            }
        }

        while (!xyList.empty() && !z_intr.empty()) {
            //merge xyList and z_intr, same way
            if (xyList[0][2] < z_intr[0][2]) {
                all_intr.push_back(xyList[0]);
                xyList.erase(xyList.begin());
            } else {
                all_intr.push_back(z_intr[0]);
                z_intr.erase(z_intr.begin());
            }

        }

        if (!xyList.empty()) {
            //add the remaining values from the non-empty vector
            for (int i = 0; i < xyList.size(); i++) {
                all_intr.push_back(xyList[i]);
            }
        }

        if (!z_intr.empty()) {
            //add the remaining values from the non-empty vector
            for (int i = 0; i < z_intr.size(); i++) {
                all_intr.push_back(z_intr[i]);
            }
        }

    }

    return all_intr;
}



vector<vector<vector<int> > > findVoronoiGridPath(int src[3], int dst[3]) {

    //if src and dst are same point, re-read src and dst
    if (src[0] == dst[0] && src[1] == dst[1] && src[2] == dst[2]) {
        vector<vector<vector<int> > > nullVec;
        return nullVec;
    }
    //line from src to dst
    vector<double> line = computeLine(src, dst);

    //directions in each dimension
    int xdir = 1, ydir = 1, zdir = 1;

    if (dst[0] - src[0] < 0) {
        xdir = -1;
    }
    if (dst[1] - src[1] < 0) {
        ydir = -1;
    }
    if (dst[2] - src[2] < 0) {
        zdir = -1;
    }

    //Voronoi gridlines, shifted from the src by 0.5
    double xGridLine = src[0] + .5 * xdir;
    double yGridLine = src[1] + .5 * ydir;
    double zGridLine = src[2] + .5 * zdir;

    //x,y and z values of intersections with the planes perpendicular to the x, y and z planes
    vector<vector<double> > x_intr;
    vector<vector<double> > y_intr;
    vector<vector<double> > z_intr;

    vector<double> intr;

    //yz planes normal to x-axis

    //plane format: same, but equation is d(x-x1)+e(y-y1)+f(z-z1) = 0
    //where x1, y1, z1 are coord. of point on the plane and <d,e,f> is a vector normal to the plane
    vector<double> plane = { 0, 0, 0, 0, 0, 0 };

    plane[3] = 1;

    for (int i = 0; i < abs(src[0] - dst[0]); i++) {
        //iterate src[0]-dst[0] times

        plane[0] = xGridLine;
        //compute intersections
        intr = computeIntersection(line, plane);

        if (!intr.empty()) {
            //record intersections
            x_intr.push_back( { intr[0], intr[1], intr[2] });
        }
        //increment gridlines in direction of xdimension
        xGridLine += xdir;

    }

    //xz planes normal to the y-axis
    plane[3] = 0;
    plane[4] = 1;

    for (int i = 0; i < abs(src[1] - dst[1]); i++) {

        plane[1] = yGridLine;
        intr = computeIntersection(line, plane);

        if (!intr.empty()) {
            y_intr.push_back( { intr[0], intr[1], intr[2] });
        }
        yGridLine = yGridLine + ydir;
    }

    //xy planes normal to the z-axis
    plane[4] = 0;
    plane[5] = 1;

    for (int i = 0; i < abs(src[2] - dst[2]); i++) {

        plane[2] = zGridLine;
        intr = computeIntersection(line, plane);

        if (!intr.empty()) {
            z_intr.push_back( { intr[0], intr[1], intr[2] });
        }
        zGridLine += zdir;
    }

    //sort intersections by x-value, then compare adjacent
    //intersections and output the intersection of the cells
    //they are the borders for
    vector<vector<double> > all_intr;

    //doesn't remove doubles from list of intersection
    all_intr = mergeLists(src, dst, x_intr, y_intr, z_intr);

    vector<vector<int> > current;
    vector<vector<int> > next;

    vector<vector<int> > intermediary;
    vector<vector<vector<int> > > pathByIndex;

    vector<int> start;
    for (int i = 0; i < 3; i++) {
        start.push_back(src[i]);
    }
    intermediary.push_back(start);
    pathByIndex.push_back(intermediary);
    intermediary.clear();

    //put starting point in path
    for (int i = 0; i < all_intr.size(); i++) {
        // Find neighbornig cells and print them for all intersections
        current = findNeighbors(all_intr[i]);
    }

    for (int i = 0; i < all_intr.size() - 1; i++) {
        if (all_intr[i] == all_intr[i + 1]) {
            // to account for doubles in list of intersections
            continue;
        } else {
            current = findNeighbors(all_intr[i]);
            next = findNeighbors(all_intr[i + 1]);

            if (current.size() <= next.size()) {
                for (int j = 0; j < current.size(); j++) {
                    for (int k = 0; k < next.size(); k++) {
                        if (current[j] == next[k]) {
                            intermediary.push_back(current[j]);
                        }
                    }
                }
                pathByIndex.push_back(intermediary);

            } else if (next.size() < current.size()) {
                for (int j = 0; j < next.size(); j++) {
                    for (int k = 0; k < current.size(); k++) {
                        if (next[j] == current[k]) {
                            intermediary.push_back(next[j]);
                        }
                    }
                }
                pathByIndex.push_back(intermediary);

            }

            intermediary.clear();
        }
    }

    vector<int> end;
    for (int i = 0; i < 3; i++) {
        end.push_back(dst[i]);
    }
    intermediary.push_back(end);
    pathByIndex.push_back(intermediary);

    return pathByIndex;
}


std::vector<CayleyPoint*> PathFinder::findVoronoiPath() {

    vector<CayleyPoint*> voronoiPath;
    if(pathType != VoronoiPath) {
        cout<<"Wrong arguments passed!"<<endl;
        return voronoiPath;

    }
    vector<double> minParam;
    vector<double> maxParam;
    char num[10];
    double temp[6];
    int src[3], dst[3];

    vector<vector<vector<CayleyPoint*> > > array = buildArray(space, stepSize);
    findBounds(space, minParam, maxParam);

    //Doing the following to avoid precision issues.
       sprintf(num, "%.2f", minParam[0]);
    minParam[0] = atof(num);
    sprintf(num, "%.2f", minParam[1]);
    minParam[1] = atof(num);
    sprintf(num, "%.2f", minParam[2]);
    minParam[2] = atof(num);

    sourcePoint->getPoint(temp);

    sprintf(num, "%.2f", temp[0]);
    temp[0] = atof(num);
    sprintf(num, "%.2f", temp[1]);
    temp[1] = atof(num);
    sprintf(num, "%.2f", temp[2]);
    temp[2] = atof(num);
    sprintf(num, "%.2f", temp[3]);
    temp[3] = atof(num);
    sprintf(num, "%.2f", temp[4]);
    temp[4] = atof(num);
    sprintf(num, "%.2f", temp[5]);
    temp[5] = atof(num);

    src[0] = floor((temp[0] - minParam[0]) / stepSize);
    src[1] = floor((temp[1] - minParam[1]) / stepSize);
    src[2] = floor((temp[2] - minParam[2]) / stepSize);

    destinationPoint->getPoint(temp);

    sprintf(num, "%.2f", temp[0]);
    temp[0] = atof(num);
    sprintf(num, "%.2f", temp[1]);
    temp[1] = atof(num);
    sprintf(num, "%.2f", temp[2]);
    temp[2] = atof(num);
    sprintf(num, "%.2f", temp[3]);
    temp[3] = atof(num);
    sprintf(num, "%.2f", temp[4]);
    temp[4] = atof(num);
    sprintf(num, "%.2f", temp[5]);
    temp[5] = atof(num);

    dst[0] = floor((temp[0] - minParam[0]) / stepSize);
    dst[1] = floor((temp[1] - minParam[1]) / stepSize);
    dst[2] = floor((temp[2] - minParam[2]) / stepSize);

   vector<vector<vector<int> > > pathByIndex = findVoronoiGridPath(src, dst);

    for (int i = 0; i < pathByIndex.size(); i++) {
                        for (int j = 0; j < pathByIndex[i].size(); j++) {
                            if (array[pathByIndex[i][j][0]][pathByIndex[i][j][1]][pathByIndex[i][j][2]] != NULL) {
                                voronoiPath.push_back(array[pathByIndex[i][j][0]][pathByIndex[i][j][1]][pathByIndex[i][j][2]]);
                            }
                        }
                    }

    return voronoiPath;

}


/**	Takes in two <CayleyPoint*, flip>'s and an Atlas pointer and returns a continuous motion path between them if it exists*/
std::vector<std::pair<CayleyPoint*, int>*> PathFinder::find1DoFPath() {
    if(this->pathType == OneDOFPath)
        cout<<"The correct constructor is being called"<<endl;
    //atlas->find1DoFPath(sourceNode,destinationNode,sourceFlip, destinationFlip);

    //std::vector<std::pair<CayleyPoint*, int> *> pathVec = this->atlas->find1DoFPath(this->sourceNode,this->destinationNode,this->sourceFlip, this->destinationFlip);
    //Atlaspath =  atlas->find1DoFPath(startNodeID, endNodeID, startFlip, endFlip);

    //if(pathVec.size() ==0) {
    //    cout<<"Got an empty path"<<endl;


    /*} else {
        for(int i =0; i<pathVec.size(); i++) {
                (std::get<0>(*pathVec[i]))->printData();
                cout<<(std::get<1>(*pathVec[i]));
                cout<<endl;
            }
    }*/
}
