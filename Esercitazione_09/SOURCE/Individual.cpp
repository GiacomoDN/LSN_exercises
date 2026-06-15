#include "Individual.h"

#include <cmath>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib> 
#include <algorithm> // Required for std::swap and std::reverse
#include <armadillo>

using namespace std;
using namespace arma;

// Constructor
Individual::Individual(const Parameters& params, Random& rnd, const CityMap& map)
    : N(params.n_cities), route(params.n_cities), length(-1.0) // Initialize members. Length is -1.0 until evaluated.
{
    arma::ivec provv(N);
    for (int i = 0; i < N; i++) provv(i) = i;

    route(0) = 0; // Fix the first city to 0 to avoid cyclic symmetries

    // Randomly generate the path by picking from the remaining available indices
    for (int i = 1; i < N; i++) {       
        int j = int(rnd.Rannyu(i, N));
        route(i) = provv(j);
        provv(j) = provv(i);
    }
    
    this->length_eval(map); // Evaluate initial route length
}

void Individual::swap_cities(Random& _rnd, const CityMap& map) {
    
    // Random indices between 1 and N-1 (excluding the fixed 0-th city)
    int index_1 = int(_rnd.Rannyu(1, N));   
    int index_2;
    
    // Ensure the two selected indices are different
    do { 
        index_2 = int(_rnd.Rannyu(1, N)); 
    } while (index_2 == index_1);    

    std::swap(route(index_1), route(index_2));
    
    this->length_eval(map); 
}

void Individual::shift_cities(Random& _rnd, const CityMap& map) {
    // 1. Determine random parameters for the shift (excluding the fixed city)
    int n_to_shift = int(_rnd.Rannyu(1, N - 1));
    int starting_point = int(_rnd.Rannyu(1, N - n_to_shift + 1));
    int n_positions = int(_rnd.Rannyu(1, N - 1));

    // 2. Copy only the mobile cities (from index 1 to N-1) into a temporary vector
    std::vector<int> mobile_cities;
    for (int i = 1; i < N; i++) {
        mobile_cities.push_back(this->route(i));
    }
    
    // 3. Define iterators for the block boundaries
    auto first = mobile_cities.begin() + starting_point - 1;
    auto last = first + n_to_shift;
    
    // 4. Copy the block and remove it from the temporary vector
    std::vector<int> block(first, last);
    mobile_cities.erase(first, last);

    // 5. Calculate the new insertion position using modulo wrapping
    int new_insert_pos = (starting_point - 1 + n_positions) % mobile_cities.size();
    
    // 6. Paste the block back into the vector at the new calculated position
    mobile_cities.insert(mobile_cities.begin() + new_insert_pos, block.begin(), block.end());
    
    // 7. Reconnect the shifted cities back to the original route
    for (int i = 1; i < N; i++) {
        this->route(i) = mobile_cities[i - 1];
    }
    
    this->length_eval(map);
}

void Individual :: reverse_cities(Random& _rnd, const CityMap& map){
    
    int index_start = int(_rnd.Rannyu(1, N)); 
    int index_end;
    
    // Ensure distinct points for the inversion
    do { 
        index_end = int(_rnd.Rannyu(1, N)); 
    } while (index_end == index_start);    

    // Ensure the indices are properly ordered [start, end]
    if (index_start > index_end){
        std::swap(index_start, index_end);
    }

    int half = (index_end - index_start) / 2 + ((index_end - index_start) % 2);

    // In-place reversal of the segment between index_start and index_end
    for (int i = 0; i < half; i++){
        std::swap(route(index_start + i), route(index_end - i));      // Swap the two cities in the route
    }

    // OPTIMIZATION: std::reverse acts directly on iterators and is highly optimized. I din't use it because I wanted to keep the algorithm explicit and clear, but in a production codebase I would prefer std::reverse for its efficiency and readability.
    // std::reverse(route.begin() + index_start, route.begin() + index_end + 1);
    
    this->length_eval(map); // Update
}

void Individual::length_eval(const CityMap& map) {
    length = 0.;
    for(int i = 0; i < N - 1; i++){
        length += map.get_distance(route(i), route(i+1)); // Fetch from distance matrix
    }
    length += map.get_distance(route(N-1), route(0));     // Close the TSP loop
}

double Individual::get_length() const {
    return length;
}

int Individual::get_route(int i) const {
    return route(i);
}

void Individual::execute_crossover(Individual& partner, Random& rnd, const CityMap& map) {
    // 1. Create two empty children routes
    Individual child_1(N);
    Individual child_2(N);

    // 2. Extract a random cut point between 1 and N-1
    int cut = int(rnd.Rannyu(1, N));

    // Boolean registries to track which cities have already been placed
    std::vector<bool> in_child_1(N, false);
    std::vector<bool> in_child_2(N, false);

    // PHASE 1: Copy exact sequence from parents up to the cut point
    for (int i = 0; i < cut; i++){
        child_1.route(i) = this->route(i);
        in_child_1[route(i)] = true;

        child_2.route(i) = partner.route(i);
        in_child_2[partner.route(i)] = true;
    }

    // PHASE 2: Fill the remaining slots maintaining the order of the other parent
    int index_c1 = cut; 
    int index_c2 = cut; 

    // Loop over all cities starting from 1
    for (int i = 1; i < this->N; i++) {

        // For Child 1: read from Parent 2 (partner)
        int city_from_p2 = partner.route(i);
        if (in_child_1[city_from_p2] == false) { // Se non c'è già...
            child_1.route(index_c1) = city_from_p2; // ...la inserisco
            index_c1++;                           // e vado avanti di un posto
        }

        // For Child 2: read from Parent 1 (this)
        int city_from_p1 = this->route(i);
        if (in_child_2[city_from_p1] == false) {
            child_2.route(index_c2) = city_from_p1;
            index_c2++;
        }
    }

    // Overwrite parents with the newly generated offspring
    this->route = child_1.route;
    partner.route = child_2.route;

    // Update lengths for both individuals
    this->length_eval(map);
    partner.length_eval(map);
}

