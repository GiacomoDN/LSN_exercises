#ifndef __variational_MC__
#define __variational_MC__

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <armadillo>
#include <stdlib.h> //exit
#include "position.h"
#include "random.h"

using namespace std;
using namespace arma;

class variational_MC {
    private:
    int _ndim = 1;                  // Dimensionality of the system
    int _sim_type;              // Type of simulation (e.g., Metropolis, ising...)
    int _nblocks;               // Number of blocks for block averaging
    int _nsteps;                // Number of simulation steps in each block
    int _nattempts;             // Number of attempted moves
    int _naccepted;             // Number of accepted moves 
    double _temp, _beta;        // Temperature and inverse temperature
    double _delta;              // Displacement step for particle moves
    Random _rnd;                // Random number generator

    int _nprop;                 // Number of properties being measured
    bool _measure_integral;     // Measure something
    int _index_integral;        // Index of something in _measurem vector
    vec _block_av;              // Block average
    vec _global_av;             // Global average
    vec _global_av2;            // Squared global average
    vec _average;               // Average value
    vec _measurement;           // Measured value

    // VARIABILI AGGIUNTE
    Position _position;     // variable to store the position and steps
    double _mu;           // mu for the gauss
    double _sigma;        // sigma for the gauss
    double _start_range_x;    // Starting range: I generate the starting position as a random 
                            // number in a simmetric interval of radius=_start_range
    double _start_range_sigma;
    double _start_range_mu;
    double _a;
    double _b;          // Values of the quadratic
    double _current_energy; // Memorizing current energy to optimize the code

    bool _print_output;     // Decides whether to print averages

    //Position _parameters_to_opt;        // Here I piu sigma and mu

    public:
    int get_nbl();              // Get the number of blocks
    int get_nsteps();           // Get the number of steps in each block
    void initialize();          // Initialize system properties
    void initialize_properties();   // Initialize some things
    void finalize();            // Finalize system and clean up
    void step();                // Perform a simulation step
    void move();                // Perform a simulation move
    void block_reset(int blk);  // Reset block averages
    void measure();             // Measure properties of the system
    void averages(int blk);     // Compute averages of properties
    double error(double acc, double acc2, int blk); // Compute error
    bool metro();       // Perform Metropolis acceptance-rejection step
    double Boltzmann(int i, bool xnew); // Calculate Boltzmann factor for Metropolis acceptance
    
    // Aggiunte

    double mod_quad_psi(bool xnew);       // Calculate Boltzmann factor for Metropolis acceptance without normalizing
    double mod_quad_psi_norm( bool xnew); // Calculate sqd module of psi, normalized
    double psi_kinetic( bool xnew); // Calculate (H_kinetic on psi) / psi
    double psi_potential( bool xnew); // Calculate (H_potential on psi) / psi
    void set_printer(bool print);   // Setting the flag to print during averages
    double get_energy();        // Get the energy of the state 
    void random_position();     // Set initial random position for the system to start
    void set_current_energy();          // Evaluate total energy and put it in the right variabile
    void set_parameters(double new_sigma, double new_mu);
    void set_temperature(double new_temp);  // Set the temperature and beta
    double get_sigma();                                     // Get sigma
    double get_mu();                                        // Get mu

};

#endif // __variational_MC__