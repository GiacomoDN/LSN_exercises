#include <iostream>
#include "Hydrogen.h"
#include "random.h"

using namespace std;

int main (int argc, char *argv[]){
    Hydrogen SYS;
    SYS.initialize();
    SYS.block_reset(0);

    // Equilibration
    // for(int i=0; i<2000; i++){
    //     SYS.step();
    // }

    for(int i=0; i < SYS.get_nbl(); i++){ //loop over blocks
        for(int j=0; j < SYS.get_nsteps(); j++){ //loop over steps in a block
            SYS.step();
            SYS.measure();
        }
        SYS.averages(i+1);
        SYS.block_reset(i+1);
    }

    SYS.write_history();
    SYS.finalize();

    cerr << "Simulation completed!" << endl;

    return 0;
}
