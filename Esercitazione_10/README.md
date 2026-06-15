# How to use my code - LSN Exercise 10

This repository contains C++ source code and Python data analysis.

## Directory Structure

INPUT/      # Initial config
OUTPUT/     # Data generated
SOURCE/     # C++, headers, makefile, main
gif/        # gif created of the evolution
OUTPUT_XXX/ # "Frozen" directories where I saved data to read on the notebook

first, go to SOURCE, then:
    to compile hit make
    to run hit mpiexec -np 4 main
    to remove *.o and *.exe files, hit make clean
    to remove all data contained in the OUTPUT dir, hit make remove