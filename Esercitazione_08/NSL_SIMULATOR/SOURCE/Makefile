CC = g++
CFLAGS = -O3 --std=c++11
AFLAGS = -larmadillo
all: simulator.exe
simulator.exe : variational_MC.o NSL_SIMULATOR.o position.o random.o
	$(CC) variational_MC.o position.o random.o NSL_SIMULATOR.o -o simulator.exe $(AFLAGS)
NSL_SIMULATOR.o : NSL_SIMULATOR.cpp variational_MC.h position.h
	$(CC) -c NSL_SIMULATOR.cpp -o NSL_SIMULATOR.o $(CFLAGS)
variational_MC.o : variational_MC.cpp variational_MC.h random.h position.h
	$(CC) -c variational_MC.cpp -o variational_MC.o $(CFLAGS)
position.o : position.cpp position.h
	$(CC) -c position.cpp -o position.o $(CFLAGS)
random.o : random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)
clean :
	rm *.o simulator.exe
remove:
	rm ../OUTPUT/*.*
	rm ../OUTPUT/CONFIG/*.*

