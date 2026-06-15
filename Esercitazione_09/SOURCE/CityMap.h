#pragma once

#include "random.h"
#include "Parameters.h"

#include <armadillo>
// Non uso "using_namespace" per best practice

class CityMap{
    private:
    int _N;      // N città
    arma::mat _coordinates;     // Matrice di dimensione N x 2 che contiene le coordinate x e y di ogni città
    arma::mat _distance_matrix; // Matrice di dimensione N x N che contiene le distanze tra ogni coppia di città, da calcolare a partire da _coordinates
    //Random _rnd;                

    double _side = 1.;          // Lato del quadrato in cui vengono generate le città se _map_type == 1

    int _map_type;              // If ==0 it generates cities on a circle, if ==1 it generates cities in a square of side = _side

    public:

    // Constructor
    CityMap(const Parameters& params, const std::string& output_filepath, Random& _rnd); // Costruttore che prende in input i parametri, il percorso del file di output e il generatore di numeri casuali, e inizializza la mappa delle città (caricamento o generazione delle città e calcolo della matrice delle distanze)

    void load_cities(const std::string& output_filepath, Random& _rnd);         // Caricamento delle città da file, se il file non esiste lo crea e genera le città
    void length_eval();         // Calcolo della matrice delle distanze a partire dalle coordinate delle città, da salvare in _distance_matrix
    double get_distance(int i, int j) const { return _distance_matrix(i, j); }  // Get_distance
    double get_coordinates(int i, int j) const { return _coordinates(i, j); }  // Get_coordinate

};
