#include "Population.h"
#include "random.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#include <algorithm>

#include <armadillo>

using namespace std;
using namespace arma;

// Constructor
Population :: Population(const Parameters& params, const CityMap& map, Random& _rnd) :      // Inizializzazione veloce
    _n_indiv(params.n_individuals), _n_best(params.n_best), _selection_exponent(params.selection_exponent),  
    _mutation_rate(params.mutation_rate), _crossover_rate(params.crossover_rate), _map(map)
    
    { 
        _current_generation.resize(_n_indiv);
        _previous_generation.resize(_n_indiv);
        this->initialize_population(params, _rnd); // Inizializzazione della popolazione (creazione della prima generazione)
}

void Population :: initialize_population(const Parameters& params, Random& _rnd){
    for(int i=0; i < _n_indiv; i++){
        _previous_generation[i] = Individual(params, _rnd, _map);
        _current_generation[i] = _previous_generation[i]; // Copio la prima generazione in quella corrente
    }
}

// Funzione di confronto per ordinare gli individui in base alla lunghezza del percorso
bool confronta(const Individual& a, const Individual& b) {          //Mi ha fatto tigliere il const
    return a.get_length() < b.get_length(); 
}

void Population :: selection(Random& _rnd){
    // Ordina la generazione corrente in base alla lunghezza del percorso
    std::sort(_previous_generation.begin(), _previous_generation.end(), confronta); 

    // Tengo i primi n_best individui più fit
    for (int i = 0; i < _n_best; i++){
        _current_generation[i] = _previous_generation[i];
    }

    // Gli altri li pesco a caso, con una funzione di potenza
    for (int i = _n_best; i < _n_indiv; i++){
        int rigged_wheel = int(_n_indiv * pow(_rnd.Rannyu(), _selection_exponent));
        _current_generation[i] = _previous_generation[rigged_wheel];
    } 
}

void Population :: mutation_1(Random& _rnd){
    // Parto da n_best così da salvarli
    for(int i=_n_best; i < _n_indiv; i++){
        // Probabilità di fare mutazione!
        if (_rnd.Rannyu() < _mutation_rate){
            _current_generation[i].swap_cities(_rnd, _map);
        }
    }
}

void Population :: mutation_2(Random& _rnd){
    // Parto da n_best così da salvarli
    for(int i=_n_best; i < _n_indiv; i++){
        // Probabilità di fare mutazione!
        if (_rnd.Rannyu() < _mutation_rate){
            _current_generation[i].shift_cities(_rnd, _map);
        }
    }
}

void Population :: mutation_3(Random& _rnd){
    // Parto da n_best così da salvarli
    for(int i=_n_best; i < _n_indiv; i++){
        // Probabilità di fare mutazione!
        if (_rnd.Rannyu() < _mutation_rate){
            _current_generation[i].reverse_cities(_rnd, _map);
        }
    }
}

void Population :: crossover(Random& _rnd){
    // Parto da n_best così da salvarli
    // Prima controllo su quante coppie faccio crossover per poter 
    for(int i=_n_best; i < _n_indiv - 1; i += 2){      // !!Se faccio il ciclo da n_best a n_indiv - 1 sono sicuro di fare solo le coppie, e se ho un numero pari faccio crossover per tutti, se è dispoari non lo faccio sull'ultimo
        // Probabilità di fare crossover
        if (_rnd.Rannyu() < _crossover_rate){
            _current_generation[i].execute_crossover(_current_generation[i + 1], _rnd, _map);
        }
    }
}

void Population :: print_length(const std::string& best_length_filepath, const std::string& half_length_filepath){
    ofstream coutf;
    coutf.open(best_length_filepath, ios::app);
    if (coutf.is_open()) {
    // Vado a prendere l'individuo 0 (il migliore) e gli chiedo la sua lunghezza
    coutf <<  _current_generation[0].get_length() << endl;
        coutf.close();
    } else {
        cerr << "Errore: impossibile aprire il file " << best_length_filepath << endl;
    }


    double best_half_length = 0.;
    for (int i = 0; i < int(_n_indiv / 2); i++){
        best_half_length += _current_generation[i].get_length();
    }
    best_half_length /= int(_n_indiv / 2);
    coutf.open(half_length_filepath, ios::app);
    if (coutf.is_open()) {
        coutf <<  best_half_length << endl;
        coutf.close();
    } else {
        cerr << "Errore: impossibile aprire il file " << half_length_filepath << endl;
    }
    return;
}

void Population :: print_best_route(const std::string& best_route_filepath, int n_cities){
    ofstream coutf;
    coutf.open(best_route_filepath, ios::app);
    if (coutf.is_open()) {
    // Stampo primo individuo (migliore)
    for (int i = 0; i < n_cities; i++){
        int ordered_index = _current_generation[0].get_route(i);
        coutf << _map.get_coordinates(ordered_index, 0) << "   " << _map.get_coordinates(ordered_index, 1) << endl;
        
    }
    coutf << endl; // Riga vuota per separare i percorsi
    coutf.close();
    } else {
        cerr << "Errore: impossibile aprire il file " << best_route_filepath << endl;
    }
    return;
}


void Population::move_on_generations(){
    for(int i=0; i < _n_indiv; i++){
        _previous_generation[i] = _current_generation[i];
    }
}

