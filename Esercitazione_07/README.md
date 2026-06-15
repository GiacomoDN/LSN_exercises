# How to use my code - LSN Exercise 07

This repository contains C++ source code and Python data analysis.

## Directory Structure

NSL_SIMULATOR/
├── INPUT/      # Initial config
├── OUTPUT/     # Data generated
├── SOURCE/     # C++, headers, makefile, main
└── OUTPUT_XXX/ # "Frozen" directories where I saved data to read on the notebook

first, go to SOURCE, then:
    to compile hit make
    to run hit ./simulator.exe
    to remove *.o and *.exe files, hit make clean
    to remove all data contained in the OUTPUT dir, hit make remove