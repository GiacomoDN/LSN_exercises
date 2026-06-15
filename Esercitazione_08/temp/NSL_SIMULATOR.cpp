#include <iostream>
#include "random.h"
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

void run_VMC(variational_MC &vmc_system){
    vmc_system.random_position();     // Set initial random position for the system to start
    vmc_system.set_current_energy();  // Set the right _current_energy
    vmc_system.block_reset(0);

    int total_blocks = vmc_system.get_nbl(); // Memorizzo il totale dei blocchi

    for(int i=0; i < total_blocks; i++){ //loop over blocks
        for(int j=0; j < vmc_system.get_nsteps(); j++){ //loop over steps in a block
            vmc_system.step();
            //vmc_system.measure();
        }
        vmc_system.averages(i+1);
        
        vmc_system.block_reset(i+1);
        
    }
    vmc_system.finalize();
}


int main (int argc, char *argv[]){

    auto start = std::chrono::high_resolution_clock::now();
    
    // Set another random
    Random second_rnd;
    int p1, p2; // Read from ../INPUT/Primes a pair of numbers to be used to initialize the RNG
    ifstream Primes("../INPUT/Primes");
    Primes >> p1 >> p2 ;
    Primes.close();
    int seed[4]; // Read the seed of the RNG
    ifstream Seed("../INPUT/seed.in");
    Seed >> seed[0] >> seed[1] >> seed[2] >> seed[3];
    second_rnd.SetRandom(seed,p1,p2);

    variational_MC vmc_system;

    vmc_system.initialize();
    vmc_system.initialize_properties();
    vmc_system.set_printer(false);      // Do NOT print during the averaging of the internal MC loop
    
    // 1. Inizializzazione parametri di partenza
    double current_mu = 1.0;     // Guess iniziale
    double current_sigma = 0.5;  // Guess iniziale
    double delta_SA = 0.1;       // Di quanto vario mu e sigma ad ogni mossa

    // 2. Calcolo l'energia per la prima volta (Fuori dal ciclo!)
    vmc_system.set_parameters(current_mu, current_sigma);
    run_VMC(vmc_system);
    double current_energy = vmc_system.get_energy();

    double T_start = 4.;
    double T_end = 0.2;
    // int n_T_steps = 10;
    // double T_step = abs(T_start - T_end) / n_T_steps;
    double beta;

    double cooling_rate = 0.85;

    int n_steps_SA = 300;
    
    for (double t = T_start; t > T_end; t *= cooling_rate){
        beta = 1.0/t;

        for (int i = 0; i < n_steps_SA; i++){
            
            //for (int j= 0; j< M; j++){
                /*
                double last_energy = vmc_system.get_energy(); // Da calcolare per la prima volta!
                run_VMC(vmc_system);
                
                double delta_E = vmc_system.get_energy() - last_energy;
                if (delta_E<=0){
                    vmc_system.set_parameters(vmc_system.get_sigma(), vmc_system.get_mu());
                }else{
                    double weight = exp(- beta*delta_E);
                    if(second_rnd.Rannyu() < weight){
                        vmc_system.set_parameters(vmc_system.get_sigma(), vmc_system.get_mu());
                    }
                    // Else do nothing
                }
                */
            //}

            // A. PROPONGO NUOVI PARAMETRI
            double proposed_mu = current_mu + second_rnd.Rannyu(-delta_SA, delta_SA);
            double proposed_sigma = current_sigma + second_rnd.Rannyu(-delta_SA, delta_SA);
            
            // Evitiamo sigma negativi o nulli (la gaussiana esploderebbe)
            if(proposed_sigma <= 0.05) continue; 

            // B. VALUTO L'ENERGIA CON I NUOVI PARAMETRI
            vmc_system.set_parameters(proposed_mu, proposed_sigma);
            run_VMC(vmc_system);
            double proposed_energy = vmc_system.get_energy();

            // C. METROPOLIS ACCEPTANCE
            double delta_E = proposed_energy - current_energy;
            
            // delta_E <= 0 viene sempre accettato dall'esponenziale se scritto bene, 
            // ma possiamo compattare l'if in questo modo elegante:
            double weight = exp(-beta * delta_E);
            
            if (delta_E <= 0 || second_rnd.Rannyu() < weight) {
                // MOSSA ACCETTATA! Aggiorno la mia "memoria"
                current_mu = proposed_mu;
                current_sigma = proposed_sigma;
                current_energy = proposed_energy;
            } else {
                // MOSSA RIFIUTATA! Ripristino i parametri vecchi nel sistema
                vmc_system.set_parameters(current_mu, current_sigma);
            }

            // Se l'energia è più bassa accettalo, se è più alta accettalo con prob exp...
            // Per accettarlo devo prima creare dentro run_VMC un metodo che prenda sigma e mu a partire 
            // da un certo start point con un vec che è un raggio (qua forse ci sta riusare position?)
            // fatto questo posso prenderlo e accettarlo
            
            progress_bar(i, n_steps_SA);
        }
    }
    vmc_system.set_printer(true);      // DO print during the averaging of the external MC loop
    run_VMC(vmc_system);


    second_rnd.SaveSeed();

    ///////

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Tempo trascorso: " << elapsed.count() * 0.001 << " s" << std::endl;

    return 0;
}

// ___________TRASH_______________

/*

// ----- OLD MAIN: exercise 08.1 BACKUP -----

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

*/
