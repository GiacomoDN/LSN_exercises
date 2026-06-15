#include <cmath>
#include <cstdlib>
#include <string>
#include "variational_MC.h"

using namespace std;
using namespace arma;

void variational_MC :: initialize(){ // Initialize according to the content of the input files in the ../INPUT/ directory

    int p1, p2; // Read from ../INPUT/Primes a pair of numbers to be used to initialize the RNG
    ifstream Primes("../INPUT/Primes");
    Primes >> p1 >> p2 ;
    Primes.close();
    int seed[4]; // Read the seed of the RNG
    ifstream Seed("../INPUT/seed.in");
    Seed >> seed[0] >> seed[1] >> seed[2] >> seed[3];
    _rnd.SetRandom(seed,p1,p2);

    ofstream couta("../OUTPUT/acceptance.dat"); // Set the heading line in file ../OUTPUT/acceptance.dat
    couta << "#   N_BLOCK:  ACCEPTANCE:" << endl;
    couta.close();

    ifstream input("../INPUT/input.dat"); // Start reading ../INPUT/input.dat
    ofstream coutf;
    coutf.open("../OUTPUT/output.dat");
    string property;
    double delta;
    while ( !input.eof() ){
        input >> property;
        if( property == "SIMULATION_TYPE" ){
        input >> _sim_type;
        if(_sim_type > 0){
            cerr << "PROBLEM: unknown simulation type" << endl;
            exit(EXIT_FAILURE);
        }
        if(_sim_type == 0)      coutf << "VARIATIONAL MONTE CARLO"  << endl;
        } else if( property == "TEMP" ){
        input >> _temp;
        _beta = 1.0/_temp;
        coutf << "TEMPERATURE= " << _temp << endl;
        } else if( property == "DELTA" ){
        input >> delta;
        coutf << "DELTA= " << delta << endl;
        _delta = delta;
        } else if( property == "START_RANGE_X" ){
        input >> _start_range_x;
        coutf << "START_RANGE_X= " << _start_range_x << endl;
        } else if( property == "START_RANGE_SIGMA_&_MU" ){
        input >> _start_range_sigma;
        input >> _start_range_mu;
        coutf << "START_RANGE_SIGMA_AND_MU= " << _start_range_sigma <<"  "<< _start_range_mu << endl;
        } else if( property == "SIGMA" ){
        input >> _sigma;
        coutf << "SIGMA= " << _sigma << endl;
        } else if( property == "MU" ){
        input >> _mu;
        coutf << "MU= " << _mu << endl;
        } else if( property == "A_B" ){
        input >> _a;
        input >> _b;
        coutf << "A_B_quartic_potential= " << _a << "    " << _b << endl;
        } else if( property == "NBLOCKS" ){
        input >> _nblocks;
        coutf << "NBLOCKS= " << _nblocks << endl;
        } else if( property == "NSTEPS" ){
        input >> _nsteps;
        coutf << "NSTEPS= " << _nsteps << endl;
        } else if( property == "ENDINPUT" ){
        coutf << "Reading input completed!" << endl;
        break;
        } else {cerr << "PROBLEM: unknown input" << endl; 
         cerr << property << endl;}
    }
    input.close();
    // Initialize a random position in [-_start_range, +_start_range)
    _ndim = 1;  //simulazione 1D
    _position.initialize(_ndim);

    coutf << "System initialized!" << endl;
    coutf.close();
    return;
}

void variational_MC :: initialize_properties(){ // Initialize data members used for measurement of properties

    string property;
    int index_property = 0;
    _nprop = 0;

    _measure_integral  = true; //Defining which properties will be measured

    ifstream input("../INPUT/properties.dat");
    if (input.is_open()){
         while ( !input.eof() ){
           input >> property;
           if( property == "INTEGRAL" ){
                ofstream coutp("../OUTPUT/integral.dat");
                coutp << "#     BLOCK:  ACTUAL_I:     I_AVE:      ERROR:" << endl;
                coutp.close();
                // --- AGGIUNTA PER IL SAMPLING ---
                ofstream coutpos("../OUTPUT/positions.dat");
                coutpos.close(); // Lo apro e lo chiudo subito solo per azzerarne il contenuto
                // --------------------------------
                _nprop++;
                _index_integral = index_property;
                _measure_integral = true;
                index_property++;
           } else if( property == "ENDPROPERTIES" ){
                ofstream coutf;
                coutf.open("../OUTPUT/output.dat",ios::app);
                coutf << "Reading properties completed!" << endl;
                coutf.close();
                break;
           } else cerr << "PROBLEM: unknown property" << endl;
         }
         input.close();
    } else cerr << "PROBLEM: Unable to open properties.dat" << endl;

    // according to the number of properties, resize the vectors _measurement,_average,_block_av,_global_av,_global_av2
    _measurement.resize(_nprop);
    _average.resize(_nprop);
    _block_av.resize(_nprop);
    _global_av.resize(_nprop);
    _global_av2.resize(_nprop);
    _average.zeros();
    _global_av.zeros();
    _global_av2.zeros();
    _nattempts = 0;
    _naccepted = 0;
    return;
}

void variational_MC :: block_reset(int blk){ // Reset block accumulators to zero
    if(_print_output){
        ofstream coutf;
        if(blk>0){
                coutf.open("../OUTPUT/output.dat",ios::app);
                coutf << "Block completed: " << blk << endl;
                coutf.close();
        }
        //_measurement.zeros();
    }
    _block_av.zeros();
    if(blk == 0){       // reset completo solo all'inizio di un nuovo run
        _global_av.zeros();
        _global_av2.zeros();
        _average.zeros();
        _nattempts = 0;
        _naccepted = 0;
    }
    return;
}

int variational_MC :: get_nbl(){
    return _nblocks;
}

int variational_MC :: get_nsteps(){
    return _nsteps;
}

void variational_MC :: step(){ // Perform a simulation step
    if(_sim_type == 0) {
        this->move(); // Perform a MC step on the Position
        _nattempts++; // update number of attempts performed

        // --- AGGIUNTA PER IL SAMPLING ---
        // Stampa la posizione solo se siamo nell'ultimo run (quando _print_output è true)
        if(_print_output){
            ofstream coutp;
            coutp.open("../OUTPUT/positions.dat", ios::app); // ios::app per appendere i dati
            // Assumendo dim=0 e 'false' perché la mossa definitiva è ora in _xold
            coutp << _position.getposition(0, true) << endl; 
            coutp.close();
        }
    }
    return;
}

void variational_MC :: move(){ // Propose a MC move for particle i
    if(_sim_type == 0){           // M(RT)^2
        vec shift(_ndim);       // Will store the proposed translation
        for(int j=0; j<_ndim; j++){
            shift(j) = _rnd.Rannyu(-1.0,1.0) * _delta; // uniform distribution in [-_delta;_delta)
        }
        
        _position.translate(shift);  //Call the function Position::translate
        //if(this->metro()){ //Metropolis acceptance evaluation
        double weight = (this->mod_quad_psi(true) / this->mod_quad_psi(false)); // Splitto
        if(_rnd.Rannyu() < weight) {
            // Accettata
            _position.acceptmove();
            _naccepted++;
            
            // Solo ORA che ho accettato e aggiornato la posizione definitiva, 
            // calcolo la NUOVA energia e aggiorno la mia memoria.
            // (Uso 'false' perché dopo acceptmove() la nuova config è diventata quella attuale)
            _current_energy = psi_kinetic(false) + psi_potential(false);        // Splitto
            
        } else {
            // Rifiutata
            _position.moveback();
            
            // _current_energy mantiene il valore che aveva allo step precedente
        }
        // 4. Salvo la misurazione (sia essa nuova o vecchia) e aggiorno le medie
        _measurement(_index_integral) = _current_energy;
        _block_av += _measurement;
    }
    return;
}

double variational_MC :: mod_quad_psi(bool xnew){
    int dim = 0;    // Simulazione 1D
    double x = _position.getposition(dim, xnew); // Position 
    
    double sigma2 = _sigma * _sigma;
    double den = 2.0 * sigma2;

    double psi = exp(-((x - _mu) * (x - _mu)) / den) + 
                exp(-((x + _mu) * (x + _mu)) / den);


    return (psi * psi);
}

double variational_MC :: psi_kinetic(bool xnew){
    int dim = 0;    // Simulazione 1D
    double x = _position.getposition(dim, xnew);
    
    // Evaluate to not repeat
    double sigma2 = _sigma * _sigma;
    double sigma4 = sigma2 * sigma2;
    double den = 2.0 * sigma2;

    double dx_minus = x - _mu;
    double dx_plus = x + _mu;
    
    double exp_minus = std::exp(-(dx_minus * dx_minus) / den);
    double exp_plus = std::exp(-(dx_plus * dx_plus) / den);
    
    // La funzione d'onda Ψ_T(x)
    double psi = exp_minus + exp_plus;
    
    // Numeratore: la derivata seconda Ψ_T''(x) calcolata pezzo per pezzo
    double d2_minus = ((dx_minus * dx_minus) / sigma4 - 1.0 / sigma2) * exp_minus;
    double d2_plus  = ((dx_plus * dx_plus)  / sigma4 - 1.0 / sigma2) * exp_plus;
    double d2_psi = d2_minus + d2_plus;
    
    // Termine cinetico locale: -(\hbar^2 / 2m) * (Ψ'' / Ψ)
    // Dato che \hbar = 1 e m = 1, il coefficiente è semplicemente -0.5
    return -0.5 * (d2_psi / psi);
}

double variational_MC :: psi_potential(bool xnew){
    int dim = 0;    // Simulazione 1D
    double x = _position.getposition(dim, xnew);
    
    // Quadrati una volta sola per ottimizzare
    double x2 = x * x;
    double x4 = x2 * x2;
    
    // Restituisce V(x) = x^4 - 2.5*x^2
    return _a * x4 + _b * x2;
}

void variational_MC :: averages(int blk){

    ofstream coutf;
    double average, sum_average, sum_ave2;

    _average     = _block_av / double(_nsteps);
    _global_av  += _average;
    _global_av2 += _average % _average; // % -> element-wise multiplication

    if (_measure_integral){
        average  = _average(_index_integral);
        sum_average = _global_av(_index_integral);
        sum_ave2 = _global_av2(_index_integral);
        if(_print_output){
            coutf.open("../OUTPUT/integral.dat",ios::app); // !!! DEVO INIZIALIZZARLO IN INITIALIZE PROPERTIES O INIZTIALIZE E BASTA
            coutf << setw(18) << blk 
                    << setw(18) << average
                    << setw(18) << sum_average/double(blk)
                    << setw(18) << this->error(sum_average, sum_ave2, blk) << endl;
            coutf.close();
        }
    }
    
    // ACCEPTANCE ///////////////////////////////
    double fraction;
    if(_nattempts > 0) fraction = double(_naccepted)/double(_nattempts);
    else fraction = 0.0; 
    if(_print_output){
        coutf.open("../OUTPUT/acceptance.dat",ios::app);    // !!! DEVO INIZIALIZZARLO IN INITIALIZE PROPERTIES O INIZTIALIZE E BASTA
        coutf << setw(18) << blk << setw(18) << fraction << endl;
        coutf.close();
    }
    return;
}

void variational_MC :: finalize(){
    _rnd.SaveSeed();
    if(_print_output){
        ofstream coutf;
        coutf.open("../OUTPUT/output.dat",ios::app);    // !!! DEVO INIZIALIZZARLO IN INITIALIZE PROPERTIES O
        coutf << "Simulation completed!" << endl;
        coutf.close();
    }
    return;
}

double variational_MC :: error(double acc, double acc2, int blk){
    if(blk <= 1) return 0.0;
    else return sqrt( fabs(acc2/double(blk) - pow( acc/double(blk) ,2) )/double(blk) );
}

void variational_MC :: set_printer(bool print){
    _print_output = print;
}

double variational_MC :: get_energy(){
    return _global_av(_index_integral) / double(_nblocks);
}

void variational_MC :: random_position(){
    for(int j=0; j<_ndim; j++){
        _position.setposition(j, _rnd.Rannyu(-1.0,1.0) * _start_range_x); // uniform distribution in [-_start_range, +_start_range)
    }
    _position.acceptmove(); // _x_old = _x_new   // Lo metto per non lasciare vuoto _xold
}

void variational_MC :: set_current_energy(){
    _current_energy = psi_kinetic(true) + psi_potential(true);  // Importante! Inizializzo la current energy per il primo step!!
}

void variational_MC :: set_temperature(double new_temp){
    _temp = new_temp;
    _beta = 1.0/_temp;
}

double variational_MC :: get_sigma(){
    return _sigma;
}

double variational_MC :: get_mu(){
    return _mu;
}

void variational_MC :: set_parameters(double new_sigma, double new_mu){
    _sigma = new_sigma;
    _mu = new_mu;
}

void variational_MC :: set_blks(double new_nblocks, double new_nsteps){
    _nblocks = new_nblocks;
    _nsteps = new_nsteps;
}