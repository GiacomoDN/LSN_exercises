#define _USE_MATH_DEFINES
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <iomanip>
#include "random.h"

using namespace std;

// Calculates the statistical uncertainty (Data Blocking)
double calculate_error(double ave, double ave2, int n) {
    if (n == 0) return 0;
    double variance = ave2 - pow(ave, 2);
    // Safety check to avoid numerical issues related to floating-point precision
    if (variance < 0) variance = 0; 
    return sqrt(variance / n); 
}

// Saves the progressive data to an output file
void print_to_file(const string& filename, 
                   const vector<int>& x_axis, 
                   const vector<double>& sum_prog, 
                   const vector<double>& err_prog) {
    // Open the output file
    ofstream out(filename);
    
    // Check if the file was opened successfully
    if (!out.is_open()) {
        cerr << "ERROR: Unable to open file " << filename << "!" << endl;
        return;
    }
    
    // Write data column by column
    int N = x_axis.size();
    for (int i = 0; i < N; i++) {
        out << std::setw(12) << x_axis[i] 
            << std::setw(15) << sum_prog[i] 
            << std::setw(15) << err_prog[i] << "\n";
    }
    out.close();
    cout << "Data successfully saved to: " << filename << endl;
}

// Core function to simulate option pricing using Data Blocking
int blocking_method_option(Random& rnd, 
                           int M, int N,            // Total throws and number of blocks
                           const string& filename,  // Output file name (passed by reference)
                           const string& type,      // Option type ("call" or "put")
                           int N_steps,             // Number of discrete steps
                           double T,                // Delivery time
                           double S_0,              // Initial asset price
                           double r,                // Risk-free interest rate
                           double sigma,            // Volatility
                           double gauss_m,          // Gaussian mean
                           double gauss_s,          // Gaussian standard deviation
                           double K) {              // Strike price
   
    if (N_steps < 1) {
        cerr << "ERROR: N_steps must be >= 1" << endl;
        return 1;
    }
   
    int L = M / N; // Throws per block
    
    // Optimization: Pre-compute constants outside the loops
    double dt = T / static_cast<double>(N_steps); 
    double exp_term = exp((r - pow(sigma, 2) / 2.0) * dt);
    double sigma_sqrt_dt = sigma * sqrt(dt); 
    double discount_factor = exp(-r * T);

    vector<double> sum_prog(N), su2_prog(N), err_prog(N);
    vector<int> x_axis(N);

    // Support variables for progressive sums (avoids nested loops for averages)
    double current_total = 0.0;
    double current_total2 = 0.0;

    for (int i = 0; i < N; i++) { // Loop over blocks

        double sum_block = 0.0;
   
        for (int j = 0; j < L; j++) { // Loop over throws within the block
         
            double S_t = S_0;
            
            // Asset price evolution (handles both direct 1-step and discrete N-steps)
            for (int k = 0; k < N_steps; k++) {
                // Optimized GBM calculation
                S_t *= exp_term * exp(sigma_sqrt_dt * rnd.Gauss(gauss_m, gauss_s));
            }

            // Payoff calculation evaluated at time T
            double payoff = 0.0;
            if (type == "call") {
                payoff = discount_factor * fmax(0.0, S_t - K);
            }
            else if (type == "put") {
                payoff = discount_factor * fmax(0.0, K - S_t);
            }
            else {
                cerr << "ERROR: Invalid option type. Use 'call' or 'put'." << endl;
                return 1;
            }
            
            sum_block += payoff;
        }

        // Calculate average and squared average for the current block
        double ave = sum_block / static_cast<double>(L);                
        double av2 = ave * ave;

        // Accumulate values for progressive statistics
        current_total += ave; 
        current_total2 += av2;

        // Compute progressive averages and statistical errors
        sum_prog[i] = current_total / (double)(i + 1);
        su2_prog[i] = current_total2 / (double)(i + 1);
        err_prog[i] = calculate_error(sum_prog[i], su2_prog[i], i);

        // Store the progressive number of throws
        x_axis[i] = (i + 1) * L;
    }

    // Output results
    print_to_file(filename, x_axis, sum_prog, err_prog);
    return 0;
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
        return 1;
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

    // --- SIMULATION PARAMETERS ---
    int M = 100000;         // Total number of simulations
    int N = 100;            // Number of blocks
    int steps = 100;        // Number of time steps for discrete path

    double S_0 = 100.0;     // Initial asset price at t=0
    double T = 1.0;         // Delivery time
    double K = 100.0;       // Strike price
    double r = 0.1;         // Risk-free interest rate
    double sigma = 0.25;    // Volatility

    double gauss_m = 0.0;   // Gaussian mean
    double gauss_s = 1.0;   // Gaussian standard deviation

    string call = "call";
    string put = "put";
   
    // --- RUN SIMULATIONS ---
    
    cout << "--- Starting Options Pricing Simulation ---" << endl;
    
    cout << "\nRunning Direct Sampling (1 step)..." << endl;
    blocking_method_option(rnd, M, N, "call_direct.dat", call, 1, T, S_0, r, sigma, gauss_m, gauss_s, K);
    blocking_method_option(rnd, M, N, "put_direct.dat", put, 1, T, S_0, r, sigma, gauss_m, gauss_s, K);

    cout << "\nRunning Step-by-Step Sampling (" << steps << " steps)..." << endl;
    blocking_method_option(rnd, M, N, "call_sbs.dat", call, steps, T, S_0, r, sigma, gauss_m, gauss_s, K);
    blocking_method_option(rnd, M, N, "put_sbs.dat", put, steps, T, S_0, r, sigma, gauss_m, gauss_s, K);

    cout << "\n--- Simulation completed successfully! ---" << endl;

    rnd.SaveSeed();
    return 0;
}