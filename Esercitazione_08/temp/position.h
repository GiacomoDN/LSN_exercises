#ifndef __Position__
#define __Position__

#include <armadillo>
#include "random.h"

using namespace std;
using namespace arma;

class Position {

private:
    int _ndim; // Dimensionality of the system
    vec _x;               // Current position vector
    vec _xold;            // Previous position vector (used in moveback())
    vec _start_range;

public:
    void initialize(int dim);                      // Initialize properties
    void translate(vec delta);   // Translate the particle within the simulation box
    void moveback();                       // Move particle back to previous position
    void acceptmove();                     // Accept the proposed move and update particle properties
    double getposition(int dim, bool xnew);// Get the position of the particle along a specific dimension
    void   setposition(int dim, double position); // Set the position of the particle along a specific dimension
    void   setpositold(int dim, double position); // Set the previous position of the particle along a specific dimension
};

#endif // __Position__