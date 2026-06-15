#pragma once

#include <armadillo>
#include "Parameters.h"
#include "CityMap.h"
#include "random.h"
// Non uso "using_namespace" per best practice

class Individual{
    private:
    int N;      // N città
    arma::ivec route;  // Vec di int che indica il path: ivec è vettore di int di arma, altrimenti uso std::vector<int>
    double length; // Lunghezza del percorso, da calcolare a partire da route e dalla mappa delle città

    public:
    // Costruttore vuoto
    Individual() {}
    // Costruttore N
    Individual(int number) : N(number), route(number), length(-1.0) {}
    // Costruttore
    Individual(const Parameters& params, Random& _rnd, const CityMap& map); // Generates a random route di lunghezza N

    // Metodi    
    void swap_cities(Random& _rnd, const CityMap& map);             // Funzione per swappare due città
    void shift_cities(Random& _rnd, const CityMap& map);
    void reverse_cities(Random& _rnd, const CityMap& map);
    //std::pair<Individual, Individual> execute_crossover(const Individual& partner, Random& rnd) const;

    void execute_crossover(Individual& partner, Random& rnd, const CityMap& map);   // Invece di farla const modifico direttamente l'individuo con cui lo chiamo e il partner che ci metto dentro

    void length_eval(const CityMap& map);       // Calcola la lunghezza del percorso
    double get_length() const;                        // Restituisce la lunghezza del percorso. Con "const" specifico che questo metodo non modifica l'oggetto
    int get_route(int i) const;
};
