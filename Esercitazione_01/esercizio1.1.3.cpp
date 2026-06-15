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

   int repeats = 100;// # times i want to calculate chi^2
   int M = 100;      // # intervals
   int n = 100000;    // # throws each time
   double p = 1.0/M;   // probability
   int freq_esp = n/M;  // frequence espected

   double random{0};
   vector <double> chi(repeats);


   for (int i = 0; i < repeats; i++){     // Ciclo su repeats {i}

      for (int j = 0; j < M; j++){        // Ciclo su M  {j}

         int freq_obs{0};                 
         
         for (int k = 0; k < n; k++){     // Ciclo su n  {k}
            random = rnd.Rannyu();        // Generazione numeri
            if (random > j * p && random < (j + 1) * p) {
               freq_obs++;
            }
         }
         chi[i] = chi[i] + (pow((freq_obs - freq_esp), 2) / freq_esp);

      }

   }
   
   // Apri il file
    ofstream out("OUTPUT/risultati_01.1.3.dat");

    // Verifica se il file è stato aperto correttamente
    if (!out.is_open()) {
        std::cerr << "Errore: impossibile aprire il file!" << std::endl;
        return 1;
    }
    // Carica
    for (int i = 0; i < repeats; i++){
      out << chi[i] << endl;
    }
    out.close();

    cout << "Dati salvati con successo in: OUTPUT/risultati_01.1.3.dat" << endl;

    return 0;
}