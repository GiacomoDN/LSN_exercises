/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/

#define _USE_MATH_DEFINES
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <iomanip>
#include <numeric>
//#include <algorithm> // Necessario per std::fill
#include "random.h"

using namespace std;

// Funzione errore
double calcola_errore(double ave, double ave2, int n) {
    if (n == 0) return 0;
    return sqrt((ave2 - pow(ave, 2)) / n); // Formula del blocking method
}

// Funzione per calcolare il modulo quadrato della posizione
double calcola_modulo_quad_pos(int x, int y, int z){
   return double(x*x) + double(y*y) + double(z*z);
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

   //////////////////////

   // Inizializzo un po di variabili
   int N = 100; // Numero di passi di ogni RW
   int M = 1e4; // Numero totale di simulazioni
   int n_blocks = 100;  //Numero di blocchi in cui divido le simulazioni
   int L = int (M/n_blocks);

   int n_faces = 6; // Facce del dado
   vector<double> dice(n_faces);       // Divido l'intervallo in 6
   for (int j = 0; j < n_faces; j++){
      dice[j] = (1./double(n_faces)) * (j + 1);
   }

   //vector<double> x1(N), y1(N), z1(N);    //Questo dovrei usarlo se volessi salvare la traiettoria completa
   int x{0}, y{0}, z{0};   // Coordinate

   // Posizione finale
   vector<double> r_davvero_fin(N);
   vector<double> errore_finale(N);

   // MATRICE POSIZIONE IN CUI METTERO LA MEDIA: N righe, ognuna con n_blocks colonne inizializzate a 0
   //vector<vector<double>> r_fin(N, vector<double>(n_blocks, 0));
   //vector<vector<double>> r2_fin(N, vector<double>(n_blocks, 0));

   vector<double> sum_prog(N);
   vector<double> su2_prog(N);

   for (int t = 0; t < n_blocks; t++){

      // MATRICE POSIZIONE: N righe, ognuna con L colonne inizializzate a 0
      //vector<vector<double>> r_old(N, vector<double>(L, 0));
      vector<double> r(N, 0.0);
      
      for (int j = 0; j < L; j++){

         double provv; // Mi appoggio
         int estrazione = 0; // Numero che estraggo dal dado
         x = 0, y = 0, z = 0; // Azzero la posizione del RW
         
         for (int i = 0; i < N; i++){

            provv = rnd.Rannyu();
            for (int u = 0; u < n_faces; u++){     // Volendo potrei non fargli fare il controllo sull'ultimo
               if (provv <  dice[u]){
                  estrazione = (u+1);  
                  break;
               }
            }

            // Invece di questo ciclo for potrei usare la libreria che è più breve
            // int estrazione = (int)rnd.Rannyu(1, 7);
            switch (estrazione){
               case 1:  x++;  break;
               case 2:  x--;  break;
               case 3:  y++;  break;
               case 4:  y--;  break;
               case 5:  z++;  break;
               case 6:  z--;  break;
               
               default: break;
            }
            r[i] += calcola_modulo_quad_pos(x, y, z);
         }
      }

      for (int i = 0; i < N; i++){
            //r_fin[i][t] = sqrt(calcolaMedia(r[i]));
            //r2_fin[i][t] = r_fin[i][t] * r_fin[i][t];

            r[i] = r[i]/double(L);

            sum_prog[i] += sqrt(r[i]);
            su2_prog[i] += r[i];
         }
   }
   
   for (int i = 0; i < N; i++){
      r_davvero_fin[i] = sum_prog[i] / double(n_blocks);
      // Media dei quadrati
      double r2_davvero_fin = su2_prog[i] / double(n_blocks);

      // Errore statistico standard
      errore_finale[i] = calcola_errore(r_davvero_fin[i], r2_davvero_fin, n_blocks);
   }
   
   string filename = "esercizio_02.2.dat"; 
   // Apri il file
   ofstream out(filename);
   // Verifica se il file è stato aperto correttamente
   if (!out.is_open()) {
      std::cerr << "Errore: impossibile aprire il file!" << std::endl;
      return 1;
   }
   // Carica
   for (int i = 0; i < N; i++){
         out << setw(8) << (i + 1) 
          << setw(15) << fixed << setprecision(6) << r_davvero_fin[i] 
          << setw(15) << errore_finale[i] << endl;
   }
   out.close();
   cout << "Dati salvati in: " << filename << endl;   



   rnd.SaveSeed();
   return 0;
}


/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/


/*

-------------------- GARBAGE


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



int blocking_method(Random& rnd, int M, int N, string filename){
   
   int L = M/N; // Numero di tiri per blocco

   vector<double> ave(N), av2(N);
   vector<double> sum_prog(N), su2_prog(N), err_prog(N);
   vector<int> x_axis(N);

   // Variabili di appoggio per fare solo 1 ciclo for
   double current_total{0};
   double current_total2{0};

   for (int i = 0; i < N; i++){        // Ciclo su N
      
      double sum_block{0};                  

      for (int j = 0; j < L; j++){     // Ciclo su L
         sum_block += rnd.Rannyu();         // Generazione numeri casuali
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
    cerr << "Dati salvati in: " << filename << endl;
    return 0;
}



//Funzione media di un vector
template <typename T>
double calcolaMedia(const std::vector<T>& v) {
    if (v.empty()) return 0.0;
    
    // Sommiamo usando 0.0 per forzare il risultato in double
    double somma = std::accumulate(v.begin(), v.end(), 0.0);
    return somma / v.size();
}



*/


/* -------- OTHER TRASH

int main2(){
   // Ver.1: provo a farlo con switch che dovrebbe essere piu semplice
   // Variabili che mi servono

   int n_faces = 6; // Facce del dado
   vector<double> dice(n_faces);       // Divido l'intervallo in 6
   for (int j = 0; j < n_faces; j++){
      dice[j] = (1./double(n_faces)) * (j + 1);
   }

   int N = 100;   // Numero di passi del RW
   int M = 10^4;  // Numero di simulazioni
   int n_blocks = 100;  // Numero di blocchi
   int L = int(M/n_blocks);

   // vector<double> x(N), y(N), Z(N);    //Questo dovrei usarlo se volessi salvare la traiettoria completa
   int x{0}, y{0}, z{0};
   double r{0};

   vector<double> ave(N), av2(N);
   vector<double> sum_prog(N), su2_prog(N), err_prog(N);
   vector<int> x_axis(N);
   
   // Variabili di appoggio per fare solo 1 ciclo for
   double current_total{0};
   double current_total2{0};

   //agigungo for?

   for (int i = 0; i < n_blocks; i++){        // Ciclo su N
      
      double sum_block{0};                  

      for (int j = 0; j < L; j++){     // Ciclo su L

         double provv; // Mi appoggio
         int estrazione; // Numero che estraggo dal dado

         for (int i = 0; i < N; i++){     // Forse devo farlo partire da i+1 se non voglio che al passo 0 abbia gia fatto un passo
            //provv = rnd.Rannyu();
            for (int j = 0; j < n_faces; j++){     // Volendo potrei non fargli fare il controllo sull'ultimo, ma mi sembra inutile
               if (provv <  dice[j]){
                  estrazione = (j+1);
                  break;
               }
            }
            switch (estrazione){
               case 1:  x++;  break;
               case 2:  x--;  break;
               case 3:  y++;  break;
               case 4:  y--;  break;
               case 5:  z++;  break;
               case 6:  z--;  break;
               
               default:
                  break;
            }

            sum_block += calcola_modulo_pos(x, y, z);         // Generazione numeri casuali
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


   vector<double> pos_media_al_passo_N_finale(N);
   vector<double> pos_media_al_passo_N_blocco_n();
   vector<double> errore(N);

   vector<vector<int>> pippo(L, vector<int>(N));

   double provv; // Mi appoggio
   int estrazione; // Numero che estraggo dal dado

   for (int l = 0; l < n_blocks; l++){

   
   
      for (int k = 0; k < L; k++){
      
         for (int i = 0; i < N; i++){     // Forse devo farlo partire da i+1 se non voglio che al passo 0 abbia gia fatto un passo
            //provv = //rnd.Rannyu();
            for (int j = 0; j < n_faces; j++){     // Volendo potrei non fargli fare il controllo sull'ultimo, ma mi sembra inutile
               if (provv <  dice[j]){
                  estrazione = (j+1);
                  break;
               }
            }
            switch (estrazione){
               case 1:  x++;  break;
               case 2:  x--;  break;
               case 3:  y++;  break;
               case 4:  y--;  break;
               case 5:  z++;  break;
               case 6:  z--;  break;
               
               default:
                  break;
            }
            pippo[k][i] += calcola_modulo_pos(x,y,z);
         }
         //pos_media_al_passo_N[k] = mean(pippo[i]);
      
      }

   }

}






*/