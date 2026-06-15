#pragma once

#include <armadillo>
#include "Individual.h"
#include "Parameters.h"
#include "CityMap.h"

class Population{

    private:
    int _n_indiv;                                       // Numero di individui per generazione
    int _n_best;
    std::vector<Individual> _current_generation;
    std::vector<Individual> _previous_generation; 
    double _selection_exponent;
    double _mutation_rate;
    double _crossover_rate;

    const CityMap& _map;   // riferimento membro

    public:

    // Constructor
    Population(const Parameters& params, const CityMap& map, Random& _rnd); // Costruttore che prende in input i parametri e la mappa delle città, e inizializza la popolazione (creazione della prima generazione)

    void initialize_population(const Parameters& params, Random& _rnd);          // Inizializzazione della popolazione (creazione della prima generazione)
    void selection(Random& _rnd);
    void mutation_1(Random& _rnd);    
    void mutation_2(Random& _rnd);
    void mutation_3(Random& _rnd); 
    void crossover(Random& _rnd);
    void cut_and_paste(int index_1, int index_2, Random& _rnd);       
    void initialize_files(const std::string& best_length_filepath, const std::string& best_route_filepath, const std::string& half_length_filepath, int n_cities);
    void print_length(const std::string& best_length_filepath, const std::string& half_length_filepath);  
    void print_best_route(const std::string& best_route_filepath, int n_cities);
    void move_on_generations();
    // Restituisce un riferimento diretto all'individuo all'indice richiesto
    Individual& get_individual(int index) { return _current_generation[index]; }
};
