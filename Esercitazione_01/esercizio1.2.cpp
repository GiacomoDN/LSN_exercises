#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <iomanip>
#include "random.h"

using namespace std;

// Generates a random variable following an exponential distribution
// using the inversion of the cumulative distribution function (CDF)
double exp_distrib(Random& rnd, double lambda) {
   return -(1.0 / lambda) * log(1.0 - rnd.Rannyu()); 
}

// Structure to automatically handle the 4 sets of sums/averages
struct four_graphs {
    vector<double> S_1, S_2, S_10, S_100;
};

// Helper function to sample from the chosen probability distribution
double sample_distribution(Random& rnd, const string& type) {
    double u = rnd.Rannyu();
    
    if (type == "standard") {
        return u; // Uniform distribution in [0, 1)
    } 
    else if (type == "exponential") {
        return exp_distrib(rnd, 1.0); // Exponential with lambda = 1
    } 
    else if (type == "lorentzian") {
        return tan(M_PI * (u - 0.5)); // Cauchy-Lorentz with mu = 0, gamma = 1
    }
    return 0.0;
}

// Generates the dataset for N=1, 2, 10, 100 realizations
four_graphs generate_data(Random& rnd, const string& type, int N_sim) {

    four_graphs res;
    res.S_1.resize(N_sim); 
    res.S_2.resize(N_sim); 
    res.S_10.resize(N_sim); 
    res.S_100.resize(N_sim);

    for (int i = 0; i < N_sim; i++) {
        // N = 1
        res.S_1[i] = sample_distribution(rnd, type);
        
        // N = 2 (Average of 2 variables)
        res.S_2[i] = (sample_distribution(rnd, type) + sample_distribution(rnd, type)) / 2.0;
        
        // N = 10 (Average of 10 variables)
        double sum10 = 0.0;
        for(int j = 0; j < 10; j++) {
            sum10 += sample_distribution(rnd, type);
        }
        res.S_10[i] = sum10 / 10.0;
        
        // N = 100 (Average of 100 variables)
        double sum100 = 0.0;
        for(int k = 0; k < 100; k++) {
            sum100 += sample_distribution(rnd, type);
        }
        res.S_100[i] = sum100 / 100.0;
    }
    return res;
}

// Saves the generated data to a text file
void save_to_file(const four_graphs& res, const string& filename) {
    ofstream out(filename);
    
    if (out.is_open()) {
        // Write data column by column: S_1 | S_2 | S_10 | S_100
        for (size_t i = 0; i < res.S_1.size(); ++i) {
            out << std::setw(15) << res.S_1[i] 
                << std::setw(15) << res.S_2[i] 
                << std::setw(15) << res.S_10[i] 
                << std::setw(15) << res.S_100[i] << "\n";
        }
        out.close();
        cout << "File " << filename << " successfully saved!" << endl;
    } else {
        cerr << "ERROR: Unable to open file " << filename << endl;
    }
}

int main(int argc, char *argv[]) {

    // --- RANDOM NUMBER GENERATOR INITIALIZATION ---
    Random rnd;
    int seed[4];
    int p1, p2;
    
    ifstream Primes("Primes");
    if (Primes.is_open()){
        Primes >> p1 >> p2 ;
    } else {
        cerr << "PROBLEM: Unable to open Primes" << endl;
        return 1;
    }
    Primes.close();

    ifstream input("seed.in");
    string property;
    if (input.is_open()){
        while (!input.eof()){
            input >> property;
            if (property == "RANDOMSEED"){
                input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
                rnd.SetRandom(seed, p1, p2);
            }
        }
        input.close();
    } else {
        cerr << "PROBLEM: Unable to open seed.in" << endl;
        return 1;
    }

    // --- DATA GENERATION & OUTPUT ---
    
    // Generate data according to different distributions
    int M = 1e4;
    four_graphs std_dist = generate_data(rnd, "standard", M);
    four_graphs exp_dist = generate_data(rnd, "exponential", M); 
    four_graphs lor_dist = generate_data(rnd, "lorentzian", M);

    // Save results
    save_to_file(std_dist, "OUTPUT/standard.dat");
    save_to_file(exp_dist, "OUTPUT/exponential.dat");
    save_to_file(lor_dist, "OUTPUT/lorentzian.dat");

    return 0;
}