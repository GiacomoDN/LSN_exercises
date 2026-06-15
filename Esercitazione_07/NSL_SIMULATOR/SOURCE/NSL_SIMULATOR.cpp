/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/

#include <iostream>
#include "system.h"

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
void equilibration(int n_ignored, System* SYS){
    cerr << "Waiting for equilibration\n";
    for(int i=0; i<n_ignored; i++) {
        SYS->step();
        if(i%50 == 0) progress_bar(i + 1, n_ignored);
    }
}

int main (int argc, char *argv[]){

    auto start = std::chrono::high_resolution_clock::now();

    int nconf = 1;
    System SYS;
    SYS.initialize();
    SYS.initialize_properties();
    SYS.block_reset(0);

    int total_blocks = SYS.get_nbl(); // Memorizzo il totale dei blocchi

    //Equilibration_____
    equilibration(10000, &SYS);
    //Equilibration_____

    for(int i=0; i < total_blocks; i++){ //loop over blocks
        for(int j=0; j < SYS.get_nsteps(); j++){ //loop over steps in a block
            SYS.step();
            SYS.measure();
            //cerr << _measurement[_index_penergy] << endl;
            if(j%50 == 0){
                // SYS.write_XYZ(nconf); //Write actual configuration in XYZ format //Commented to avoid "filesystem full"! 
                nconf++;
            }
        }

        SYS.averages(i+1);
        if (i == (total_blocks - 1)){     //PRINT LAST GOFR DISTRIB
            string nomeIn = "../OUTPUT/gofr.dat";
            string nomeOut = "../OUTPUT/gofr_final.dat";
            
            ifstream fileIn(nomeIn);
            if (!fileIn) return 1;

            string riga;
            vector<string> tutteLeRighe;
            getline(fileIn, riga); // 1. Salta la prima riga (header)

            
            while (getline(fileIn, riga)) {  // 2. Leggi tutto il resto del file
                if (!riga.empty()) {
                    tutteLeRighe.push_back(riga);
                }
            }
            fileIn.close();

            // 3. Calcola da dove iniziare a scrivere
            // Se chiedi 5 righe ma ne abbiamo 3, partiamo da 0 per evitare errori
            int n_bins = 100; 
            int totale = tutteLeRighe.size();
            int inizio = totale - n_bins; 

            // Protezione di sicurezza: se il file ha meno righe dei bin, parto da 0
            if (inizio < 0) inizio = 0; 

            // Scrivi sul nuovo file
            ofstream fileOut(nomeOut);
            for (int i = inizio; i < totale; ++i) {
                fileOut << tutteLeRighe[i] << "\n";
            }
            fileOut.close();
        }
        
        SYS.block_reset(i+1);

        progress_bar(i + 1, total_blocks);
    }
    SYS.finalize();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Tempo trascorso: " << elapsed.count()/1000 << " s" << std::endl;

    return 0;
}

/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/
