#include <iostream>
#include <armadillo>
#include <iomanip>

#include "Population.h"
#include "Individual.h"
#include "CityMap.h"
#include "random.h"
#include "Parameters.h"



using namespace std;
using namespace arma;

void progress_bar(int progression_index, int total_index, int bar_width = 50) {
    if (total_index <= 0) return; // 1. Guardia di sicurezza per evitare divisioni per zero

    float progress = (float)progression_index / total_index;
    int pos = bar_width * progress;
    int num_width = to_string(total_index).length(); // 2. Allineamento dinamico perfetto: calcola quante cifre ha il totale (es. 1000 ha 4 cifre)

    cerr << "\rProgress: " << setw(num_width) << progression_index << "/" << total_index << " ["; // 3. Stampa l'intestazione
    
    for (int i = 0; i < bar_width; ++i) { // 4. Disegna la barra
        if (i < pos) cerr << "=";
        else if (i == pos) cerr << ">";
        else cerr << " ";
    }

    if (progression_index == total_index) cerr << endl; // 5. Vai a capo solo alla fine
}

void InitializeRNG(Random& _rnd) {
    int p1, p2;
    ifstream Primes("../INPUT/Primes");
    if (Primes.is_open()){
        Primes >> p1 >> p2;
    } else {
        cerr << "ERRORE: Impossibile aprire ../INPUT/Primes" << endl;
        exit(1);
    }
    Primes.close();

    int seed[4];
    ifstream Seed("../INPUT/seed.in");
    if (Seed.is_open()){
        Seed >> seed[0] >> seed[1] >> seed[2] >> seed[3];
    } else {
        cerr << "ERRORE: Impossibile aprire ../INPUT/seed.in" << endl;
        exit(1);
    }
    _rnd.SetRandom(seed, p1, p2);
    Seed.close();
}

int main() {

    auto start_time = chrono::high_resolution_clock::now();
    Random _rnd;
    InitializeRNG(_rnd);

    Parameters params;
    const string param_file = "../INPUT/input.dat";     // Percorso del file di input dei parametri
    const string output_filepath = "../OUTPUT/output.dat";
    const string best_length_filepath = "../OUTPUT/best_length_history.dat";
    const string half_length_filepath = "../OUTPUT/half_length_history.dat";
    const string best_route_filepath = "../OUTPUT/best_route_history.dat";

    params.load_parameters(param_file);                            // Carica i parametri da file
    params.save_parameters(output_filepath);                       // Salva i parametri in output.dat

    CityMap map(params, output_filepath, _rnd);                    // Crea la mappa delle città, che si occupa di 
                                                        // generare o caricare le città e calcolare la matrice delle distanze
    Population pop(params, map, _rnd);                        // Crea la popolazione, che si occupa di creare la prima generazione e di 
                                                        // gestire le operazioni di mutazione e crossover
    pop.initialize_files(best_length_filepath, best_route_filepath, half_length_filepath, params.n_cities);
    pop.print_best_route(best_route_filepath, params.n_cities);

    for (int i = 0; i < params.n_generations; i++){
        pop.selection(_rnd);
        // Prima cosa che faccio è il crossover
        pop.crossover(_rnd);
        pop.mutation_1(_rnd);
        pop.mutation_2(_rnd);
        pop.mutation_3(_rnd);

        pop.print_length(best_length_filepath, half_length_filepath);
        if (i <10 || i % 10 == 0){ // Stampo il percorso migliore ogni 10 generazioni (e per le prime 10)>)
            pop.print_best_route(best_route_filepath, params.n_cities);
        }
        pop.move_on_generations();

        progress_bar(i, params.n_generations);
    }

    pop.print_best_route(best_route_filepath, params.n_cities);
    //pop.print_best_route(best_route_filepath, params.n_cities);           
    
    params.finalize(output_filepath); // Finalize system (messaggio di completamento)
    
    _rnd.SaveSeed(); // Scrive lo stato finale del generatore su seed.out

    // Tempo totale:
    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> total_time = end_time - start_time;
    cout << "\nEsecuzione completata in " << total_time.count() << " secondi." << endl;

    return 0;
}