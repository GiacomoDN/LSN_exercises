#define _USE_MATH_DEFINES
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <iomanip>
#include "random.h"


using namespace std;

// Funzione errore
double calcola_errore(double ave, double ave2, int n) {
    if (n == 0) return 0;
    return sqrt((ave2 - pow(ave, 2)) / n); // Formula del blocking method
}

// Funzione per la distrib esponenziale
double exp_distrib(Random& rnd, double lambda) {
   return -(1./lambda) * log(lambda * (1 - rnd.Rannyu())); // Formula inversa
}

// Funzione dell'esercizio 2 con campionamento uniforme
double exercise1_distrib(Random& rnd) {
   return M_PI * 0.5 * cos(0.5 * M_PI * rnd.Rannyu()); // Funzione del primo esercizio
}

// Funzione per la distrib lineare y = 1 - x nell'intervallo [0,1]
double lin_distrib(Random& rnd) {
   return 1.0 - sqrt(1.0 - rnd.Rannyu()); // Formula inversa
}

// Funzione dell'esercizio 2 con importance sampling
double exercise1_sampling(Random& rnd) {
   double x = lin_distrib(rnd);
   double f_x = M_PI * 0.5 * cos(0.5 * M_PI * x);
   double p_x = 2.0 * (1.0 - x);
   return f_x / p_x; // Funzione del primo esercizio
}

int blocking_method(Random& rnd, int M, int N, string type, string filename){
   
   int L = M/N; // Numero di tiri per blocco

   vector<double> ave(N), av2(N);
   vector<double> sum_prog(N), su2_prog(N), err_prog(N);
   vector<int> x_axis(N);

   // Variabili di appoggio per fare solo 1 ciclo for
   double current_total{0};
   double current_total2{0};

   for (int i = 0; i < N; i++){        // Ciclo su N
      
      double sum_block{0};                  
      if (type == "normal"){
         for (int j = 0; j < L; j++){     // Ciclo su L
            sum_block += exercise1_distrib(rnd);         // Generazione numeri casuali
         }
      }
      else if (type == "importance"){
         for (int j = 0; j < L; j++){     // Ciclo su L
            sum_block += exercise1_sampling(rnd);         // Generazione numeri casuali
         }
      }

      // Salvo A_i e (A_i)^2
      ave[i] = sum_block / double(L);                 
      av2[i] = pow(ave[i], 2);

      // Accumulo le somme di A_i e (A_i)^2 per fare le medie progressive
      current_total += ave[i]; 
      current_total2 += av2[i];

      // Carico somme progressive delle medie e delle medie al quadrato
      sum_prog[i] = current_total / (double)(i + 1);
      su2_prog[i] = current_total2 / (double)(i + 1);
      err_prog[i] = calcola_errore(sum_prog[i], su2_prog[i], i);

      // Crea le ascisse
      x_axis[i] = (i + 1) * L;
   }

   
   // Apri il file
    ofstream out(filename);

    // Verifica se il file è stato aperto correttamente
    if (!out.is_open()) {
        std::cerr << "Errore: impossibile aprire il file!" << std::endl;
        return 1;
    }

    // Carica
    for (int i = 0; i < N; i++){
      // MIGLIORIA: Uso setw per incollonare bene i dati nel file .dat
      out << std::setw(12) << x_axis[i] 
          << std::setw(15) << std::fixed << std::setprecision(6) << sum_prog[i] 
          << std::setw(15) << err_prog[i] << endl;
    }
    out.close();
    cout << "Dati salvati in: " << filename << " (" << type << ")" << endl;
    return 0;
}

 
int main (int argc, char *argv[]){

   Random rnd;
   int seed[4];
   int p1, p2;
   ifstream Primes("RNG/Primes");
   if (Primes.is_open()){
      Primes >> p1 >> p2 ;
   } else cerr << "PROBLEM: Unable to open RNG/Primes" << endl;
   Primes.close();

   ifstream input("RNG/seed.in");
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
   } else cerr << "PROBLEM: Unable to open RNG/seed.in" << endl;

   ////////////////////////////

   int M = 1000000; // Numero di throws
   int N = 100;     // 100 blocchi

   blocking_method(rnd, M, N, "normal", "risultati_02.1.dat");
   blocking_method(rnd, M, N, "importance", "risultati_02.2.dat");


   rnd.SaveSeed();
   return 0;
}

