#include <cmath>
#include <cstdlib>
#include <string>

#include "Hydrogen.h"

using namespace std;
using namespace arma;

void Hydrogen :: initialize(){ // Initialize the Hydrogen object according to the content of the input files in the ../INPUT/ directory

    // Initialize RNG
    int p1, p2; // Read from ../INPUT/Primes a pair of numbers to be used to initialize the RNG
    ifstream Primes("../INPUT/Primes");
    Primes >> p1 >> p2 ;
    Primes.close();
    int seed[4]; // Read the seed of the RNG
    ifstream Seed("../INPUT/seed.in");
    Seed >> seed[0] >> seed[1] >> seed[2] >> seed[3];
    _rnd.SetRandom(seed,p1,p2);

    // Set the heading line in file ../OUTPUT/position.dat
    ofstream coutp("../OUTPUT/position.dat"); 
    coutp << "#     BLOCK:  ACTUAL_POSITION:     POSITION_AVE:      ERROR:" << endl;
    coutp.close();

    // Set the heading line in file ../OUTPUT/acceptance.dat
    ofstream couta("../OUTPUT/acceptance.dat"); 
    couta << "#   N_BLOCK:  ACCEPTANCE:" << endl;
    couta.close();

    // Start reading ../INPUT/input.dat
    ifstream input("../INPUT/input.dat");
    ofstream coutf;
    coutf.open("../OUTPUT/output.dat");
    string property;
    double delta;
    // Only three properties to read
    while ( !input.eof() ){
        input >> property;
        if( property == "STATE" ){
            input >> _state;
        } else if( property == "DELTA" ){
            input >> _delta;
            coutf << "DELTA= " << _delta << endl;
        } else if( property == "NBLOCKS" ){
            input >> _nblocks;
            coutf << "NBLOCKS= " << _nblocks << endl;
        } else if( property == "PROB_TYPE" ){
            input >> _prob_type;
            coutf << "PROB_TYPE= " << _prob_type << endl;
        } else if( property == "START_MU" ){
            input >> _start_mu;
            coutf << "START_MU= " << _start_mu << endl;
        } else if( property == "START_SIGMA" ){
            input >> _start_sigma;
            coutf << "START_SIGMA= " << _start_sigma << endl;
        } else if( property == "NSTEPS" ){
            input >> _nsteps;
            coutf << "NSTEPS= " << _nsteps << endl;
        } else if( property == "ENDINPUT" ){
            coutf << "Reading input completed!" << endl;
            break;
        } else cerr << "PROBLEM: unknown input" << endl;
    }

    input.close();

    // Initialize vectors and starting position
    int M = _nblocks*_nsteps; 
    _x.reserve(M);
    _y.reserve(M);
    _z.reserve(M);
    _x.push_back(_rnd.Gauss(_start_mu, _start_sigma));
    _y.push_back(_rnd.Gauss(_start_mu, _start_sigma));
    _z.push_back(_rnd.Gauss(_start_mu, _start_sigma));

    // Evaluation of norm factors to optimize
    _norm1 = 1.0 / (M_PI * std::pow(_a0, 3));
    _norm2 = 1.0 / (32.0 * M_PI * std::pow(_a0, 5));

    // Impotant: initialization of index for r
    _index_r = 0;
    int nprop = 1;
    
    // according to the number of properties, resize the vectors _measurement,_average,_block_av,_global_av,_global_av2
    _measurement.resize(nprop);
    _average.resize(nprop);
    _block_av.resize(nprop);
    _global_av.resize(nprop);
    _global_av2.resize(nprop);
    _average.zeros();
    _global_av.zeros();
    _global_av2.zeros();
    _nattempts = 0;
    _naccepted = 0;

    coutf << "Hydrogen initialized!" << endl;

    coutf.close();
    return;
}

void Hydrogen :: block_reset(int blk){ // Reset block accumulators to zero
    ofstream coutf;
    if(blk>0){
        coutf.open("../OUTPUT/output.dat",ios::app);
        coutf << "Block completed: " << blk << endl;
        coutf.close();
    }
    _block_av.zeros();
    return;
}

void Hydrogen :: step(){ // Perform a simulation step

    if(_prob_type == 0){ // Extract a new position uniform
        _xprop = _rnd.Rannyu(_x.back() - _delta, _x.back() + _delta);
        _yprop = _rnd.Rannyu(_y.back() - _delta, _y.back() + _delta);
        _zprop = _rnd.Rannyu(_z.back() - _delta, _z.back() + _delta);
    } else if(_prob_type == 1){ // Extract a gaussian new position
        _xprop = _rnd.Gauss(_x.back(), _delta);
        _yprop = _rnd.Gauss(_y.back(), _delta);
        _zprop = _rnd.Gauss(_z.back(), _delta);
    } else {
        cerr << "Invalid PROB_TYPE! Available: 0 - Uniform, 1 - Gaussian" << endl;
    }
    
    double old_p = pdf(_x.back(), _y.back(), _z.back());
    double new_p = pdf(_xprop, _yprop, _zprop);

    if (metro(new_p / old_p)){
        _x.push_back(_xprop);
        _y.push_back(_yprop);
        _z.push_back(_zprop);
        _naccepted++; // Increment accepted counter
    }
    else {
        _x.push_back(_x.back());
        _y.push_back(_y.back());
        _z.push_back(_z.back());
    }
    _nattempts++;
    return;
}

double Hydrogen:: pdf(double x, double y, double z){
    double probability = 0.0;
    // From polar to cartes
    double r = std::sqrt(x*x + y*y + z*z); 

    if (_state == 0) {  // State1,0,0 (1s)
        probability = _norm1 * std::exp(-2.0 * r / _a0);
    }
    else if (_state == 1) { // State 2,1,0 (2pz)
        probability = _norm2 * (z * z) * std::exp(-r / _a0);
    }
    return probability;
}

bool Hydrogen :: metro(double fraction){ // Metropolis algorithm
    bool decision = false;
    if(fraction > 1) return true;
    if(_rnd.Rannyu() < fraction ) decision = true; //Metropolis acceptance step
    return decision;
}

void Hydrogen :: measure(){
  _measurement.zeros();

  if (_measure_r){
    double last_distance = sqrt(_x.back() * _x.back() + _y.back() * _y.back() + _z.back() * _z.back());
    _measurement(_index_r) = last_distance;
  }

  _block_av += _measurement; //Update block accumulators

  return;
}

void Hydrogen:: write_history(){ // Write all the positions visited from the algorithm on a file
    
    ofstream coutf;
    coutf.open("../OUTPUT/history.dat");
    int M = _nblocks*_nsteps;
    coutf << "# total attempts: " << M << endl;
    for(int i = 0; i < M; i++){
        coutf << _x[i] << setw(20)
                << _y[i] << setw(20)
                << _z[i] << endl;
    }
    coutf.close();
    return;
}

void Hydrogen :: finalize(){
    _rnd.SaveSeed();
    ofstream coutf;
    coutf.open("../OUTPUT/output.dat",ios::app);
    coutf << "Simulation completed!" << endl;
    coutf.close();
    return;
}

void Hydrogen :: averages(int blk){ // Averages for data block

    ofstream coutf;
    double average, sum_average, sum_ave2;

    _average     = _block_av / double(_nsteps);
    _global_av  += _average;
    _global_av2 += _average % _average; // % -> element-wise multiplication

    // Position
    if (_measure_r){
    coutf.open("../OUTPUT/position.dat", ios::app);
    average  = _average(_index_r);
    sum_average = _global_av(_index_r);
    sum_ave2 = _global_av2(_index_r);
    coutf << setw(12) << blk 
            << setw(12) << average
            << setw(12) << sum_average/double(blk)
            << setw(12) << this->error(sum_average, sum_ave2, blk) << endl;
    coutf.close();
    }

    // Acceptance
    double fraction;
    coutf.open("../OUTPUT/acceptance.dat", ios::app);
    if(_nattempts > 0) fraction = double(_naccepted)/double(_nattempts);
    else fraction = 0.0; 
    coutf << setw(12) << blk << setw(12) << fraction << endl;
    coutf.close();

    return;
}

double Hydrogen :: error(double acc, double acc2, int blk){
  if(blk <= 1) return 0.0;
  else return sqrt( fabs(acc2/double(blk) - pow( acc/double(blk) ,2) )/double(blk) );
}

int Hydrogen :: get_nbl(){
  return _nblocks;
}

int Hydrogen :: get_nsteps(){
  return _nsteps;
}

double Hydrogen :: get_delta(){
  return _delta;
}

int Hydrogen :: get_prob_type(){
  return _prob_type;
}