#define ALLFLIPS -1
#define ALLDIMS 5


#include "Statistics.h"

#include "AtlasBuilder.h"
#include "Atlas.h"
#include "Settings.h"
//#include "RatioChecker.h" Uncomment after update




#include "Eigen/Dense"
#include "Eigen/SVD"
// #include "Eigen/Eigenvalues"
using namespace Eigen;

using Eigen::Vector3d;
using Eigen::Matrix3d;
using Eigen::Quaterniond;
using Eigen::MatrixXd;
using Eigen::VectorXd;

#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
    #include <GL/glu.h>
#endif





#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <set>
#include <cassert>

#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <sstream>
#include <string>

using namespace std;

#define PI 3.14159265


MolecularUnit * Statistics::helxA;
MolecularUnit * Statistics::helxB;
PredefinedInteractions * Statistics::df;
int Statistics::counter;
std::ofstream Statistics::outFile, Statistics::outFile2;
int Statistics::node;
Atlas* Statistics::mapView;
