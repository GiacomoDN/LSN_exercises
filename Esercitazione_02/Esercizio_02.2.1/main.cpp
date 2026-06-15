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
#include "random.h"

using namespace std;

// --- FUNZIONI DI SUPPORTO ---

// Calcolo dell'incertezza statistica (Data Blocking)
double calcola_errore(double ave, double ave2, int n) {
    if (n == 0) return 0;
    return sqrt((ave2 - pow(ave, 2)) / n); 
}

// Calcolo del modulo al quadrato della posizione r^2 = x^2 + y^2 + z^2
double calcola_modulo_quad_pos(int x, int y, int z){
    return double(x*x) + double(y*y) + double(z*z);
}

// --- MAIN ---
int main (int argc, char *argv[]){

    // 1. Inizializzazione del Generatore di Numeri Casuali
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

    // 2. Parametri della Simulazione
    int N = 100;          // Numero di passi di ogni Random Walk
    int M = 1e4;          // Numero totale di simulazioni (RW)
    int n_blocks = 100;   // Numero di blocchi
    int L = int(M/n_blocks); // Numero di simulazioni per blocco

    // 3. Costruzione del "Dado" per il campionamento discreto
    int n_faces = 6; 
    vector<double> dice(n_faces);       
    for (int j = 0; j < n_faces; j++){
      dice[j] = (1. / double(n_faces)) * (j + 1); // Crea intervalli [1/6, 2/6, ... 1]
    }

    // 4. Strutture Dati per il Data Blocking
    int x{0}, y{0}, z{0}; // Coordinate nello spazio 3D
    vector<double> sum_prog(N, 0.0); // Accumulatore medie progressive (sqrt(<r^2>))
    vector<double> su2_prog(N, 0.0); // Accumulatore medie progressive al quadrato (<r^2>)
    vector<double> r_davvero_fin(N, 0.0);
    vector<double> errore_finale(N, 0.0);

    // 5. Ciclo Principale: Data Blocking
    for (int t = 0; t < n_blocks; t++){

      // Vettore per accumulare r^2 in questo specifico blocco. Si resetta a zero ogni blocco!
      vector<double> r_block(N, 0.0); 
   
      for (int j = 0; j < L; j++){ // Ciclo sulle simulazioni nel blocco

         double provv; 
         int estrazione = 0; 
         x = 0; y = 0; z = 0; // Il Random Walk riparte dall'origine
      


         for (int i = 0; i < N; i++){ // Ciclo sui passi del singolo RW

            // Estrazione della direzione basata sul "dado"
            provv = rnd.Rannyu();
            for (int u = 0; u < n_faces; u++){     
               if (provv < dice[u]){
                  estrazione = (u+1);  
                  break;
               }
            }
            
            // Alternativa per l'estrazione del dado (Più rapida)
            // int estrazione = (int)rnd.Rannyu(1, 7);

            // Aggiornamento coordinate
            switch (estrazione){
               case 1:  x++;  break;
               case 2:  x--;  break;
               case 3:  y++;  break;
               case 4:  y--;  break;
               case 5:  z++;  break;
               case 6:  z--;  break;
               default: break;
            }
            
            // Accumulo la distanza al quadrato per il passo i-esimo
            r_block[i] += calcola_modulo_quad_pos(x, y, z);
         }
      }

      // Chiusura del blocco: calcolo le medie di blocco e accumulo
      for (int i = 0; i < N; i++){
         // Media di r^2 all'interno del blocco
         r_block[i] = r_block[i] / double(L);

         // Accumulo la radice (RMS) e il suo quadrato per l'errore finale
         sum_prog[i] += sqrt(r_block[i]);
         su2_prog[i] += r_block[i]; 
      }
   }
   
    // 6. Calcolo Valori Finali e Incertezze
    for (int i = 0; i < N; i++){
        r_davvero_fin[i] = sum_prog[i] / double(n_blocks);
        double r2_davvero_fin = su2_prog[i] / double(n_blocks);

        errore_finale[i] = calcola_errore(r_davvero_fin[i], r2_davvero_fin, n_blocks);
    }
   
    // 7. Salvataggio su File
    string filename = "esercizio_02.2.dat"; 
    ofstream out(filename);
    
    if (!out.is_open()) {
        std::cerr << "Errore: impossibile aprire il file!" << std::endl;
        return 1;
    }
    
    for (int i = 0; i < N; i++){
        out << setw(8) << (i + 1) // Asse x: Numero del passo
            << setw(15) << fixed << setprecision(6) << r_davvero_fin[i] 
            << setw(15) << errore_finale[i] << endl;
    }
    
    out.close();
    cout << "Dati salvati in: " << filename << endl;  
    
    
    // Creo un RW di esempio
    int N_es = 1000;
    vector<int> x_es(N_es + 1, 0), y_es(N_es + 1, 0), z_es(N_es + 1, 0);
    double provv2;
    int estrazione{0};
    for (int i = 0; i < N_es; i++){
        // Estrazione della direzione basata sul "dado"
            provv2 = rnd.Rannyu();
            for (int u = 0; u < n_faces; u++){     
               if (provv2 < dice[u]){
                  estrazione = (u+1);  
                  break;
               }
            }
            
            // Alternativa per l'estrazione del dado (Più rapida)
            // int estrazione = (int)rnd.Rannyu(1, 7);

            // 1. Prima dello switch: "trascino" la posizione precedente nel nuovo passo
            x_es[i+1] = x_es[i];
            y_es[i+1] = y_es[i];
            z_es[i+1] = z_es[i];

            // 2. Nello switch: modifico solo la componente che cambia
            switch (estrazione) {
                case 1: x_es[i+1]++; break; // +x
                case 2: x_es[i+1]--; break; // -x
                case 3: y_es[i+1]++; break; // +y
                case 4: y_es[i+1]--; break; // -y
                case 5: z_es[i+1]++; break; // +z
                case 6: z_es[i+1]--; break; // -z
            }
    }
    // Salva
    string filename2 = "RW_disc.dat"; 
    ofstream out2(filename2);
    if (!out2.is_open()) {
        std::cerr << "Errore: impossibile aprire il file!" << std::endl;
        return 1;
    }
    for (int i = 0; i < N_es; i++){
        out2 << setw(8) << x_es[i] << setw(15) << y_es[i] << setw(15) << z_es[i] << endl;
    }
    out2.close();
    cout << "Dati salvati in: " << filename2 << endl;  

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

/* ==========================================================
   ARCHIVIO VECCHIO CODICE (Non usato nella versione finale)
   ========================================================== */

// Librerie scartate
// #include <numeric>
// #include <algorithm> // Necessario per std::fill
/*
// Vecchia funzione template per calcolare la media di un vettore generico
template <typename T>
double calcolaMedia(const std::vector<T>& v) {
    if (v.empty()) return 0.0;
    
    // Sommiamo usando 0.0 per forzare il risultato in double
    double somma = std::accumulate(v.begin(), v.end(), 0.0);
    return somma / v.size();
}

*/

// Vecchie dichiarazioni per salvare l'intera traiettoria (Memoria intensiva)
// vector<double> x1(N), y1(N), z1(N);

// Vecchia logica con matrici (Scartata per ottimizzazione RAM e CPU)
// MATRICE POSIZIONE IN CUI METTERO LA MEDIA: N righe, ognuna con n_blocks colonne inizializzate a 0
// vector<vector<double>> r_fin(N, vector<double>(n_blocks, 0));
// vector<vector<double>> r2_fin(N, vector<double>(n_blocks, 0));
// vector<vector<double>> r_old(N, vector<double>(L, 0));

/* Vecchio ciclo di chiusura blocco
   for (int i = 0; i < N; i++){
       //r_fin[i][t] = sqrt(calcolaMedia(r[i]));
       //r2_fin[i][t] = r_fin[i][t] * r_fin[i][t];
   }
*/
