#ifndef __System__
#define __System__

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <armadillo>
#include <stdlib.h> //exit
#include "random.h"

using namespace std;
using namespace arma;

class Hydrogen {

    private:
        const int _ndim = 3;            // Dimensionality of the system
        std::vector <double> _x, _y, _z;// Coordinates
        double _xprop, _yprop, _zprop;  // Proposed coordinates
        double _norm1, _norm2;          // Normalization factors
        int _nblocks;                   // Number of blocks for block averaging
        int _nsteps;                    // Number of simulation steps in each block
        int _nattempts;                 // Number of attempted moves
        int _naccepted;                 // Number of accepted moves
        Random _rnd;                    // Random number generator

        // Properties
        const double _a0 = 1.0;
        double _delta;                  // Delta for uniform proba
        int _state;
        int _prob_type;                 // 0 = uniform, 1 = Gaussian
        double _start_mu;
        double _start_sigma;
        int _nprop;                     // Number of properties being measured
        bool _measure_r;                // Flags for measuring
        int _index_r;                   // Indices for accessing
        vec _block_av;                  // Block averages of properties
        vec _global_av;                 // Global averages of properties
        vec _global_av2;                // Squared global averages of properties
        vec _average;                   // Average values of properties
        vec _measurement;               // Measured values of properties

    public: // Function declarations

        int get_nbl();                  // Get the number of blocks
        int get_nsteps();               // Get the number of steps in each block
        double get_delta();               // Get the number of steps in each block
        int get_prob_type();               // Get the number of steps in each block
        void initialize();              // Initialize system properties
        void finalize();                // Finalize system and clean up
        void step();                    // Perform a simulation step
        void block_reset(int blk);      // Reset block averages
        void measure();                 // Measure properties of the system
        void averages(int blk);         // Compute averages of properties
        double error(double acc, double acc2, int blk);     // Compute error
        bool metro(double fraction);     // Perform Metropolis acceptance-rejection step
        double pdf(double x, double y, double z);    // Return pdf
        void write_history();

};

#endif // __System__