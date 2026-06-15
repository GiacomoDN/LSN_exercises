#include <iostream>
#include <armadillo>
#include <iomanip>
#include <chrono>

#include <mpi.h> // Include MPI header for parallel processing

#include "Population.h"
#include "Individual.h"
#include "CityMap.h"
#include "random.h"
#include "Parameters.h"

using namespace std;
using namespace arma;

// ============================================================================
// Progress Bar
void progress_bar(int progression_index, int total_index, int bar_width = 50) {
    // 1. Safety guard to prevent division by zero
    if (total_index <= 0) return; 

    float progress = static_cast<float>(progression_index) / total_index;
    int pos = static_cast<int>(bar_width * progress);
    
    // 2. Perfect dynamic alignment: calculate the number of digits in the total (e.g., 1000 has 4 digits)
    int num_width = std::to_string(total_index).length(); 

    // 3. Print the header
    std::cerr << "\rProgress: " << std::setw(num_width) << progression_index << "/" << total_index << " ["; 
    
    // 4. Draw the bar
    for (int i = 0; i < bar_width; ++i) { 
        if (i < pos) std::cerr << "=";
        else if (i == pos) std::cerr << ">";
        else std::cerr << " ";
    }

    // Use flush() to force immediate output
    std::cerr << "] " << std::fixed << std::setprecision(1) << (progress * 100.0) << "%" << std::flush;    

    // 5. Add a newline only at the end
    if (progression_index == total_index) std::cerr << std::endl; 
}

// Function to initialize all MPI cores with different seeds
// Random Number Generator Initialization (MPI Compatible)
void InitializeRNG(Random& _rnd, int rank) {
    int p1, p2;
    std::ifstream Primes("../INPUT/Primes");
    
    if (Primes.is_open()) {
        // Skip the first 'rank' lines to get different primes for each process
        for (int i = 0; i <= rank; i++) {
            Primes >> p1 >> p2;                 
        }
    } else {
        std::cerr << "ERROR: Unable to open ../INPUT/Primes" << std::endl;
        exit(1);
    }
    Primes.close();

    int seed[4];
    std::ifstream Seed("../INPUT/seed.in");
    
    if (Seed.is_open()) {
        Seed >> seed[0] >> seed[1] >> seed[2] >> seed[3];
    } else {
        std::cerr << "ERROR: Unable to open ../INPUT/seed.in" << std::endl;
        exit(1);
    }
    Seed.close();
    
    _rnd.SetRandom(seed, p1, p2);
}


void initialize_files(const std::string& best_length_filepath, 
                        const std::string& best_route_history_filepath, 
                        const std::string& best_route_filepath, 
                        const std::string& half_length_filepath, 
                        const std::string& output_filepath, 
                        int n_cities){
    ofstream coutf;
    coutf.open(best_length_filepath);
    if (coutf.is_open()) {
    coutf << "#BEST_LENGTH_HISTORY" << endl;
        coutf.close();
    } else {
        cerr << "Errore: impossibile aprire il file " << best_length_filepath << endl;
    }

    coutf.open(best_route_history_filepath);
    if (coutf.is_open()) {
    coutf << "#BEST_ROUTE_HISTORY" << endl;
    coutf << "#X_COORDINATE    Y_COORDINATE" << endl;
    coutf << n_cities << endl << endl << endl; // Prima riga con il numero di città, per poter poi leggere il file in python e sapere quante righe leggere per ogni percorso
        coutf.close();
    } else {
        cerr << "Errore: impossibile aprire il file " << best_route_history_filepath << endl;
    }

    coutf.open(best_route_filepath);
    if (coutf.is_open()) {
    coutf << "#BEST_ROUTE" << endl;
    coutf << "#X_COORDINATE    Y_COORDINATE" << endl;
    coutf << "# n_cities: "  << n_cities << endl << endl << endl; // Prima riga con il numero di città, per poter poi leggere il file in python e sapere quante righe leggere per ogni percorso
        coutf.close();
    } else {
        cerr << "Errore: impossibile aprire il file " << best_route_filepath << endl;
    }

    coutf.open(half_length_filepath);
    if (coutf.is_open()) {
    coutf << "#HALF_LENGTH_HISTORY" << endl;
        coutf.close();
    } else {
        cerr << "Errore: impossibile aprire il file " << half_length_filepath << endl;
    }

    coutf.open(output_filepath);
    if (coutf.is_open()) {
    coutf << "SIMULATION OUTPUT" << endl;
        coutf.close();
    } else {
        cerr << "Errore: impossibile aprire il file " << output_filepath << endl;
    }
    return;
}

int main(int argc, char* argv[]) {

    // Start MPI
    MPI_Init(&argc, &argv); 
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size); // How many of us are there?
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Wich number am I?

    // Start the clock
    auto start_time = chrono::high_resolution_clock::now();

    // Initialize RNG
    Random _rnd;
    InitializeRNG(_rnd, rank); // I implemented a function to initialize all cores with different seeds
    
    // Creating a parameters object
    Parameters params;

    // Defining parameters file path and loading them
    const string param_filepath = "../INPUT/input.dat";
    params.load_parameters(param_filepath);

    // Here I decide the file path for the cities file based on the map type parameter
    string cities_filepath; 
    if (params.get_map_type() == 0){
        cities_filepath = "../INPUT/cities_on_circle.dat";
    } else if (params.get_map_type() == 1){
        cities_filepath = "../INPUT/cities_in_square.dat";
    } else if (params.get_map_type() == 2){
        cities_filepath = "../INPUT/cap_prov_ita.dat";
    } else {
        cerr << "PROBLEM: unknown map type!" << endl;
        exit(EXIT_FAILURE);
    }

    // Other output file paths
    const string output_filepath = "../OUTPUT/output.dat"; // Sumup of the simulation
    const string best_length_filepath = "../OUTPUT/best_length_history.dat"; // Here are saved the best lengths for EACH generation
    const string half_length_filepath = "../OUTPUT/half_length_history.dat"; // Here is saved the best half average length for EACH generation
    const string best_route_history_filepath = "../OUTPUT/best_route_history.dat"; // Here is saved the best route for EACH generation
    const string best_route_filepath = "../OUTPUT/best_route.dat"; // Here is saved the BEST FINAL ROUTE
    
    // Only node 0 initialize files
    if (rank == 0) {
        initialize_files(best_length_filepath, 
                        best_route_history_filepath, 
                        best_route_filepath, 
                        half_length_filepath, 
                        output_filepath, 
                        params.n_cities);

        params.save_parameters(output_filepath); // Write parameters in sumup
    }

    // Creating a CityMap object: it only sets the size of the matrices (the actual loading/generation of the cities and calculation of the distance matrix is done in the constructor of CityMap)
    CityMap map(params);

    // Only node 0 generates cities if they dont already exist
    if (rank == 0){
        if (params.get_map_type() != 2){    // Se la simulazione non è del tipo prov ita generro il file
            map.generate_cities_file(cities_filepath, output_filepath, _rnd);
        }
        map.load_cities(cities_filepath);     // Load the cities
    }
    map.bcast_cities();                    // Broadcast the cities to all processes (now all processes have the same city coordinates in RAM)
    map.length_eval();                    // Calculate the distance matrix

    // Creating a Population object: it creates the first generation of individuals
    Population pop(params, map, _rnd);

    for (int i = 0; i < params.n_generations; i++){
        pop.selection(_rnd);

        // Migration: every 50 generations, each process sends its best individual to the next process and receives the best individual from the previous one
        if (i % 50 == 0) {
            if (params.migrations){
                MPI_Request req;
                int itag=1;
                
                // Perform an instantaneous send, to node (rank + 1) mod size (cyclic condition)
                MPI_Isend(pop.get_individual(0).get_route_ptr(), params.n_cities, MPI_LONG_LONG_INT, (rank + 1) % size, itag, MPI_COMM_WORLD, &req);

                // I wait for the message from the neighbour (rank - 1 + size) % size
                MPI_Recv(pop.get_individual(params.n_individuals - 1).get_route_ptr(), params.n_cities, MPI_LONG_LONG_INT, (rank - 1 + size) % size, itag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // Wait that every message has been received
                MPI_Wait(&req, MPI_STATUS_IGNORE);
                
                double old_length = pop.get_individual(params.n_individuals - 1).get_length(); // Invalido la lunghezza, così da essere sicuro che venga ricalcolata
                pop.get_individual(params.n_individuals - 1).length_eval(map);
                double new_length = pop.get_individual(params.n_individuals - 1).get_length();

                pop.selection(_rnd);
            }
        }

        // I perfom crossover first
        pop.crossover(_rnd);
        pop.mutation_1(_rnd);
        pop.mutation_2(_rnd);
        pop.mutation_3(_rnd);

        // Only node 0 prints
        if (rank == 0) {
            if (i <10 || i % 100 == 0){ // Print the best route every 100 generations (and for the first 10 generations)
                pop.print_best_route(best_route_history_filepath, params.n_cities);
            }
            // Stampo la best lengh for each generation
            pop.print_length(best_length_filepath, half_length_filepath);
            progress_bar(i + 1, params.n_generations);
        }
        
        // Move on generations
        pop.move_on_generations();
    }     
    
    _rnd.SaveSeed(); // Write the final state of the random number generator to seed.out

    // ____ COMPARISON ____

    // I call selection because it puts routes in length order
    pop.selection(_rnd);
    
    // Save the best four candidates
    double candidates[size];    // I create it in all nodes but i use it only in node 0
    for(int i = 0; i < size; i++) candidates[i]=0;
    double lunghezza = pop.get_individual(0).get_length();

    // Rank 0 collects all the candidates
    MPI_Gather (&lunghezza, 1, MPI_DOUBLE, candidates, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Variable for the winner that every node has
    int best_rank = 0; 

    if (rank == 0) {
        auto ptr_to_min = std::min_element(candidates, candidates + size); // Generate the pointer to where the min element in candidates is
        best_rank = std::distance(candidates, ptr_to_min); // Calculate wich position of the candidates array it is -> it will be the same of the node (gather works this way, otherwise I wouldn'b be sure that the node 2 put his best length in candidates[2])
    }

    // Rank 0 says to all nodes who won
    MPI_Bcast(&best_rank, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // --- Final switch ---
    if (rank == 0) {
        // If I am NOT the winner, I request the winning route from the victorious node
        if (best_rank != 0) { // Without this condition there would be a deadlock
            // Receive data and write it DIRECTLY into the internal buffer of my individual 0
            MPI_Recv(pop.get_individual(0).get_route_ptr(), params.n_cities, MPI_LONG_LONG_INT, best_rank, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Node 0 waits until it has received the best path
            
            // Update the length of individual 0, otherwise it will retain its outdated value!
            pop.get_individual(0).length_eval(map);
        }
        // pop.get_individual(0).length_eval(map);
        cerr << "\n=== GLOBAL FINAL RESULTS ===" << endl;
        cerr << "Winner: Rank " << best_rank << " with length " << pop.get_individual(0).get_length() << endl;

        // Now the first individual is the best one globally
        pop.print_best_route(best_route_filepath, params.n_cities);
        pop.print_length(best_length_filepath, half_length_filepath);

        // Finalize system
        params.finalize(output_filepath); 

        // Time
        auto end_time = chrono::high_resolution_clock::now();
        chrono::duration<double> total_time = end_time - start_time;
        cout << "\nExecution completed in " << total_time.count() << " s" << endl; 

    } else if (rank == best_rank) {
        // If i'm not zero, but the speaker just said my name:
        // I send my individual to rank 0
        MPI_Send(pop.get_individual(0).get_route_ptr(), params.n_cities, MPI_LONG_LONG_INT, 0, 99, MPI_COMM_WORLD);
    }

    // Shut off MPI
    MPI_Finalize();

    return 0;
}