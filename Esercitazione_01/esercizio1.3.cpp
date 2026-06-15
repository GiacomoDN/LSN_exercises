#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include "random.h"
#include <iomanip>

using namespace std;

// Calculates the statistical uncertainty using the Data Blocking method
// n is the number of blocks minus one (N_blocks - 1)
double error(double ave, double ave2, int n) {
    if (n == 0) return 0;
    return sqrt((ave2 - pow(ave, 2)) / n);
}

// Funzione per l'angolo
double rand_alpha(Random& rnd) {
    double x, y, r2;
    do {
        x = rnd.Rannyu(-1, 1);
        y = rnd.Rannyu(-1, 1);
        r2 = x*x + y*y;
    } while (r2 > 1.0 || r2 == 0); // Rigettiamo se fuori dal cerchio o nell'origine
    
    return atan2(y, x); // Restituisce l'angolo in (-pi, pi]
}

int main (int argc, char *argv[]){

    Random rnd;
    int seed[4];
    int p1, p2;
    ifstream Primes("Primes");
    if (Primes.is_open()){
    Primes >> p1 >> p2 ;
    } else cerr << "PROBLEM: Unable to open Primes" << endl;
    Primes.close();

    ifstream input("seed.in");
    string property;
    if (input.is_open()){
        while ( !input.eof() ){
            input >> property;
            if( property == "RANDOMSEED" ){
                input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
                rnd.SetRandom(seed,p1,p2);
            }
        }
        input.close();
    } else cerr << "PROBLEM: Unable to open seed.in" << endl;

    int M = 1000000; // # simulazioni
    int N = 100; // N blocchi
    int L = M/N;

    const double d = 10;
    const double l = 8;

    double alpha;
    double pos_x;

    // Vectors for Data Blocking
    vector<double> sum_prog(N, 0.0);
    vector<double> su2_prog(N, 0.0);
    vector<double> err_prog(N, 0.0);
    vector<int> x(N, 0);

    double current_total = 0.0;
    double current_total2 = 0.0;

    // --- BUFFON'S EXPERIMENT ---
    for (int i = 0; i < N; i++) {
        
        int N_hit = 0; // Hits in the current block
        
        // Block loop
        for (int j = 0; j < L; j++) {
            // Generate random orientation and position
            alpha = rand_alpha(rnd);
            pos_x = rnd.Rannyu(0, d/2);
            if (pos_x <= 0.5*l*fabs(sin(alpha))){
                N_hit++;
            }
        }
        
        // Estimate Pi for the current block
        double pi_est = 0;
        if (N_hit > 0) { // Safety check to avoid division by zero
            pi_est = (2.0 * l * L) / (static_cast<double>(N_hit) * d);
        }

        double pi_est2 = pi_est * pi_est;

        // Accumulate for progressive statistics
        current_total += pi_est;
        current_total2 += pi_est2;

        sum_prog[i] = current_total / (i + 1);
        su2_prog[i] = current_total2 / (i + 1);
        err_prog[i] = error(sum_prog[i], su2_prog[i], i);
        x[i] = (i + 1) * L;
    }

    // --- DATA OUTPUT ---
    ofstream out("OUTPUT/risultati_01.3.dat");
    if (out.is_open()) {
        for (int i = 0; i < N; i++) {
            // Saves: [# throws] [progressive estimation of Pi] [statistical error]
            out << x[i] << " " << sum_prog[i] << " " << err_prog[i] << "\n";
        }
        out.close();

    } else {
        cerr << "ERROR: Unable to open file for writing!" << endl;
    }

    return 0;
}
