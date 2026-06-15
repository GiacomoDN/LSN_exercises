#include "Parameters.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
using namespace arma;

void Parameters :: load_parameters(const std::string& filename){
    std::ifstream file(filename);
    if (!file.is_open()){
        cerr << "ERROR: Unable to open parameter file '" << filename << "'" << endl;
        exit(EXIT_FAILURE);
    }

    bool has_n_cities = false;
    bool has_map_type = false;
    bool has_n_individuals = false;
    bool has_n_generations = false;
    bool has_mutation_rate = false;
    bool has_crossover_rate = false;
    bool has_selection_exponent = false;
    bool has_n_best = false;
    

    std::string line;
    while (std::getline(file, line)){
        // Skip empty lines
        std::istringstream iss(line);
        std::string key;
        if (!(iss >> key)){
            continue;
        }

        // Allow optional '=' separators by replacing them with spaces
        for (char &c : key){
            if (c == '=') c = ' ';
        }

        if (key == "N_CITIES"){
            if (!(iss >> n_cities)){
                cerr << "ERROR: Invalid value for N_CITIES in '" << filename << "'" << endl;
                exit(EXIT_FAILURE);
            }
            has_n_cities = true;
        }
        else if (key == "MAP_TYPE"){
            if (!(iss >> map_type)){
                cerr << "ERROR: Invalid value for MAP_TYPE in '" << filename << "'" << endl;
                exit(EXIT_FAILURE);
            }
            has_map_type = true;
        }
        else if (key == "N_INDIVIDUALS"){
            if (!(iss >> n_individuals)){
                cerr << "ERROR: Invalid value for N_INDIVIDUALS in '" << filename << "'" << endl;
                exit(EXIT_FAILURE);
            }
            has_n_individuals = true;
        }
        else if (key == "N_GENERATIONS"){
            if (!(iss >> n_generations)){
                cerr << "ERROR: Invalid value for N_GENERATIONS in '" << filename << "'" << endl;
                exit(EXIT_FAILURE);
            }
            has_n_generations = true;
        }
        else if (key == "MUTATION_RATE"){
            if (!(iss >> mutation_rate)){
                cerr << "ERROR: Invalid value for MUTATION_RATE in '" << filename << "'" << endl;
                exit(EXIT_FAILURE);
            }
            has_mutation_rate = true;
        }
        else if (key == "CROSSOVER_RATE"){
            if (!(iss >> crossover_rate)){
                cerr << "ERROR: Invalid value for CROSSOVER_RATE in '" << filename << "'" << endl;
                exit(EXIT_FAILURE);
            }
            has_crossover_rate = true;
        }
        else if (key == "SELECTION_EXPONENT"){
            if (!(iss >> selection_exponent)){
                cerr << "ERROR: Invalid value for SELECTION_EXPONENT in '" << filename << "'" << endl;
                exit(EXIT_FAILURE);
            }
            has_selection_exponent = true;
        }
        else if (key == "N_BEST_TO_SAVE"){
            if (!(iss >> n_best)){
                cerr << "ERROR: Invalid value for N_BEST_TO_SAVE in '" << filename << "'" << endl;
                exit(EXIT_FAILURE);
            }
            has_n_best = true;
        }
        else {
            cerr << "WARNING: Unknown parameter '" << key << "' in '" << filename << "', ignoring." << endl;
        }
    }

    if (!has_n_cities || !has_map_type || !has_n_individuals || !has_n_generations || !has_mutation_rate || !has_crossover_rate || !has_selection_exponent || !has_n_best){
        cerr << "ERROR: Missing required parameters in '" << filename << "'" << endl;
        exit(EXIT_FAILURE);
    }

    if (n_best > n_individuals){
        cerr << "ERROR: N_BEST_TO_SAVE can't be bigger than N_INDIVIDUALS" << endl;
        exit(EXIT_FAILURE);
    }
    
}

void Parameters :: save_parameters(const std::string& filepath){
    std::ofstream file(filepath);
    if (!file.is_open()){
        cerr << "ERROR: Unable to open output file '" << filepath << "'" << endl;
        exit(EXIT_FAILURE);
    }
    file << "--- INITIALIZATION ---" << endl << endl;
    file << "N_CITIES        " << n_cities << endl;
    file << "MAP_TYPE        " << map_type << endl;
    file << "N_INDIVIDUALS   " << n_individuals << endl;
    file << "N_GENERATIONS   " << n_generations << endl;
    file << "MUTATION_RATE   " << mutation_rate << endl;
    file << "CROSSOVER_RATE  " << crossover_rate << endl;
    file << "SELECTION_EXPONENT  " << selection_exponent << endl;
    file << "N_BEST_TO_SAVE  " << n_best << endl;

    file.close();
}

void Parameters :: finalize(const std::string& output_filepath){

    ofstream coutf;
    coutf.open(output_filepath, ios::app);
    coutf << "Simulation completed!" << endl;
    coutf.close();
    
    return;
}

int Parameters :: get_map_type() const {
    return map_type;
}