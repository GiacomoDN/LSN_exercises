#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include "random.h"

using namespace std;

// Calculates the statistical uncertainty using the Data Blocking method
// n is the number of blocks minus one (N_blocks - 1)
double error(double ave, double ave2, int n) {
    if (n == 0) return 0;
    return sqrt((ave2 - pow(ave, 2)) / n);
}

int main(int argc, char *argv[]) {

    // --- RANDOM NUMBER GENERATOR INITIALIZATION ---
    Random rnd;
    int seed[4];
    int p1, p2;

    ifstream Primes("Primes");
    if (Primes.is_open()) {
        Primes >> p1 >> p2;
    } else {
        cerr << "PROBLEM: Unable to open Primes" << endl;
        return 1; // Exit with error code
    }
    Primes.close();

    ifstream input("seed.in");
    string property;
    if (input.is_open()) {
        while (!input.eof()) {
            input >> property;
            if (property == "RANDOMSEED") {
                input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
                rnd.SetRandom(seed, p1, p2);
            }
        }
        input.close();
    } else {
        cerr << "PROBLEM: Unable to open seed.in" << endl;
        return 1; 
    }

    // --- PART 1: ESTIMATION OF <r> = 0.5 ---

    // Simulation parameters
    int M = 1e6;         // Total number of throws
    int N = 100;            // Number of blocks
    int L = M / N;          // Throws per block

    // Vectors to store block averages and x-axis values
    vector<double> ave(N, 0.0);
    vector<double> av2(N, 0.0);
    vector<int> x(N, 0);

    // Vectors to store progressive averages, squared averages, and errors
    vector<double> sum_prog(N, 0.0);
    vector<double> su2_prog(N, 0.0);
    vector<double> err_prog(N, 0.0);

    // Accumulators to compute progressive sums efficiently
    double current_total = 0.0;
    double current_total2 = 0.0;

    // Data Blocking Loop
    for (int i = 0; i < N; i++) {
        
        double sum1 = 0.0;                  
        
        // Loop within a single block
        for (int j = 0; j < L; j++) {
            sum1 += pow(rnd.Rannyu() - 0.5, 2); // Generate 
        }
        
        // Compute average and squared average for the i-th block
        ave[i] = sum1 / L;                 
        av2[i] = pow(ave[i], 2);

        // Accumulate block results for progressive statistics
        current_total += ave[i]; 
        current_total2 += av2[i];

        // Compute progressive average and progressive squared average
        sum_prog[i] = current_total / (i + 1);
        su2_prog[i] = current_total2 / (i + 1);

        // Compute statistical uncertainty
        // i represents (number of blocks - 1), which is exactly what the formula requires
        err_prog[i] = error(sum_prog[i], su2_prog[i], i);

        // Calculate the number of throws evaluated up to this block
        x[i] = (i + 1) * L; 
    }

    // --- DATA OUTPUT ---
    
    ofstream out("OUTPUT/risultati_01.1.2.dat");

    if (!out.is_open()) {
        cerr << "ERROR: Unable to open output file!" << endl;
        return 1;
    }
    
    // Write results to file
    for (int i = 0; i < N; i++) {
        out << x[i] << " " << sum_prog[i] << " " << err_prog[i] << "\n";
    }
    
    out.close();
    cout << "Data successfully saved in: OUTPUT/risultati_01.1.2.dat" << endl;

    return 0;
}


