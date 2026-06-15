#pragma once

#include <armadillo>


class Parameters {
    
    public:

    // Parameters for CityMap
    int n_cities;
    int map_type;
    
    // Parameters for Population
    int n_best;     // Number of best individuals to save at each generation
    int n_individuals;
    int n_generations;
    double mutation_rate;
    double crossover_rate;
    double selection_exponent;
    bool migrations;   // Flag indicating if I want migrations
    
    void load_parameters(const std::string& filename); // legge il file .txt
    void save_parameters(const std::string& filepath); // salva i parametri su output_filepath
    void finalize(const std::string& output_filepath);  // stampa messaggio finale su output_filepath
    int get_map_type() const { return map_type; }
    int get_n_cities() const { return n_cities; }
    int get_n_individuals() const { return n_individuals; }
};
