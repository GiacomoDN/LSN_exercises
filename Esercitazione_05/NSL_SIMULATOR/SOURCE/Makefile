CC = g++
CFLAGS = -O3 --std=c++11
AFLAGS = -larmadillo

all: main

main: main.o Hydrogen.o random.o
	$(CC) $(CFLAGS) main.o Hydrogen.o random.o -o main $(AFLAGS)

main.o : main.cpp Hydrogen.h random.h
	$(CC) -c main.cpp -o main.o $(CFLAGS)

Hydrogen.o : Hydrogen.cpp Hydrogen.h random.h
	$(CC) -c Hydrogen.cpp -o Hydrogen.o $(CFLAGS)

random.o : random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)

clean :
	rm *.o main

remove:
	rm ../OUTPUT/*.*
	rm ../OUTPUT/CONFIG/*.*