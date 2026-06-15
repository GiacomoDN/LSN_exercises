#include "CityMap.h"
#include "random.h"

#include <cmath>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib> //exit

#include <armadillo>


using namespace std;
using namespace arma;

// Constructor
CityMap :: CityMap(const Parameters& params, const std::string& output_filepath, Random& _rnd) : _N(params.n_cities), _map_type(params.map_type) {
    _coordinates.resize(_N, 2);             // Allocate memory for coordinate matrix of dimension N x 2 for x and y coordinates
    _distance_matrix.resize(_N, _N);        // Allocate memory for distance matrix of dimension N x N for distances
    this->load_cities(output_filepath, _rnd);     // Load or generate the cities
    this->length_eval();                    // Calculate the distance matrix
}

void CityMap:: load_cities(const string& output_filepath, Random& _rnd){
    
    string cities_filepath;
    if (_map_type == 0){
        cities_filepath = "../INPUT/cities_on_circle.dat";
    } else if (_map_type == 1){
        cities_filepath = "../INPUT/cities_in_square.dat";
    } else {
        cerr << "PROBLEM: unknown map type!" << endl;
        exit(EXIT_FAILURE);
    }
    
    // 1. Provo ad aprire il file in sola LETTURA
    ifstream file_in(cities_filepath);

    if (file_in.is_open()) { // SE IL FILE ESISTE LO LEGGO E CARICO
        ofstream coutf(output_filepath,ios::app);                       // Oss: sarebbe meglio creare una variabile globale per il file di output, così da non dover cambiare ogni volta il percorso
            coutf << "File delle città trovato! Procedo con la lettura" << endl;
        coutf.close();
        int ncities;
        file_in >> ncities;
        if (ncities != _N){
            cerr << "PROBLEM: conflicting number of cities in input.dat & cities.dat not match!" << endl;
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < _N; i++){
            file_in >> _coordinates(i, 0) >> _coordinates(i, 1);    // load x e y
        }
        file_in.close();
    } else {                // SE IL FILE NON ESISTE LO CREO E CARICO
        ofstream coutf;
            coutf.open(output_filepath,ios::app);
            coutf << "File delle città non trovato! Creazione città" << endl;
        coutf.close();
        ofstream coutc(cities_filepath);
            coutc << _N << endl;

        for (int i = 0; i < _N; i++) {
            if (_map_type == 0) {
                //double random = _rnd.Rannyu();
                //cerr << "Random number for city " << i << ": " << random << endl; // Debug: stampa il numero casuale generato     
                double theta = _rnd.Rannyu() * 2.0 * M_PI;
                //double theta = _rnd.Rannyu(0., 2.0 * M_PI);
                _coordinates(i, 0) = cos(theta);
                _coordinates(i, 1) = sin(theta);
            } else if (_map_type == 1) {
                _coordinates(i, 0) = _rnd.Rannyu(0, _side);
                _coordinates(i, 1) = _rnd.Rannyu(0, _side);
            }
            // Scrivo sul file
            coutc << _coordinates(i, 0) << "    " << _coordinates(i, 1) << endl;
        }
        coutc.close();
    }
}

void CityMap :: length_eval(){
    for (int i = 0; i < _N; i++) {       
        for (int j = i; j < _N; j++) { 
            // CORRETTO: Distanza come norma della differenza
            double dist = norm(_coordinates.row(i) - _coordinates.row(j));
            
            // Riempio entrambe le metà della matrice (è simmetrica!)
            _distance_matrix(i, j) = dist;
            _distance_matrix(j, i) = dist;
        }
    }
}


