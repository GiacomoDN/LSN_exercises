#include <iostream>
#include "variational_MC.h"

#include <chrono>

using namespace std;

void progress_bar(int current_block, int total_blocks, int width = 80) {
    float progress = (float)current_block / total_blocks; //Frazione di completamento
    int pos = width * progress;

    std::cerr << "\rBlock " << std::setw(3) << current_block << "/" << total_blocks << " [";     // \r per sovrascrivere
    
    for (int i = 0; i < width; ++i) {
        if (i < pos) std::cerr << "=";      // Parte completata
        else if (i == pos) std::cerr << ">"; // Testa della barra
        else std::cerr << " ";               // Parte mancante
    }

    std::cerr << "] " << std::fixed << std::setprecision(1) << (progress * 100.0) << "%" << std::flush;     // Uso flush() per mostrare subito l'output

    if (current_block == total_blocks) std::cerr << std::endl;      // Vado a capo per pulire il terminale
}

int main (int argc, char *argv[]){

    auto start = std::chrono::high_resolution_clock::now();
    
    variational_MC Marco;

    Marco.initialize();
    Marco.initialize_properties();
    Marco.block_reset(0);

    int total_blocks = Marco.get_nbl(); // Memorizzo il totale dei blocchi

    for(int i=0; i < total_blocks; i++){ //loop over blocks
        for(int j=0; j < Marco.get_nsteps(); j++){ //loop over steps in a block
            Marco.step();
            //Marco.measure();
        }
        Marco.averages(i+1);
        
        Marco.block_reset(i+1);

        progress_bar(i + 1, total_blocks);
    }
    Marco.finalize();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Tempo trascorso: " << elapsed.count() * 0.001 << " s" << std::endl;

    return 0;
}
