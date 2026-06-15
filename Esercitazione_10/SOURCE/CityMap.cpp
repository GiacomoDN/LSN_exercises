#include "CityMap.h"
#include "random.h"

#include <cmath>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib> //exit

#include <armadillo>

#include <mpi.h> // Include MPI header for parallel processing


using namespace std;
using namespace arma;

// Constructor
CityMap :: CityMap(const Parameters& params) : _N(params.n_cities), _map_type(params.map_type) {
    _coordinates.resize(_N, 2);             // Allocate memory for coordinate matrix of dimension N x 2 for x and y coordinates
    _distance_matrix.resize(_N, _N);        // Allocate memory for distance matrix of dimension N x N for distances
}

void CityMap:: generate_cities_file(const string& cities_filepath, const string& output_filepath, Random& _rnd){

    
    
    // 1. Provo ad aprire il file in sola LETTURA
    ifstream file_in(cities_filepath);

    if (file_in.is_open()) { // SE IL FILE ESISTE LO DICO ALL'OUTPUT E VADO AVANTI
        ofstream coutf(output_filepath,ios::app);
            coutf << "File delle città già esistente!" << endl;
        coutf.close();
        file_in.close();
    } else {                // SE IL FILE NON ESISTE LO DICO ALL'OUTPUT E LO CREO
        ofstream coutf;
            coutf.open(output_filepath,ios::app);
            coutf << "File delle città non trovato! Creazione città" << endl;
        coutf.close();
        ofstream coutc(cities_filepath);
            coutc << _N  << endl;        // Nella prima riga scrivo il numero di città
        for (int i = 0; i < _N; i++) {
            if (_map_type == 0) { 
                double theta = _rnd.Rannyu() * 2.0 * M_PI;
                _coordinates(i, 0) = cos(theta);
                _coordinates(i, 1) = sin(theta);
            } else if (_map_type == 1) {
                _coordinates(i, 0) = _rnd.Rannyu(0, _side);
                _coordinates(i, 1) = _rnd.Rannyu(0, _side);
            }
            // Scrivo sul file
            coutc << _coordinates(i, 0) << "   " << _coordinates(i, 1) << endl;
        }
        coutc.close();
    }
}

void CityMap:: load_cities(const string& cities_filepath){
    
    // Apro
    ifstream file_in(cities_filepath);

    if (file_in.is_open()) { // SE IL FILE ESISTE LO LEGGO E CARICO
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
    } else {                // Se il file non esiste do messaggio di errore
        cerr << "PROBLEM: conflicting number of cities in input.dat & cities.dat not match!" << endl;
        exit(EXIT_FAILURE);
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

void CityMap::bcast_cities() {
    // Tutti i rank chiamano questa funzione.
    // Il Rank 0 prende i dati della sua matrice in RAM e li "spara" agli altri.
    // Gli altri rank ascoltano e riempiono la loro matrice.
    // Spediamo _N * 2 elementi (perché ogni città ha una X e una Y).
    
    MPI_Bcast(_coordinates.memptr(), _N * 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);
}