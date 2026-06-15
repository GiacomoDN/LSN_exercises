# Use CXX for C++ compilers and CXXFLAGS for C++ flags
CXX = g++
CXXFLAGS = -Wall -O3 -std=c++11

# Define the target executable name
TARGET = main

$(TARGET): main.o random.o
	$(CXX) main.o random.o -o $(TARGET)

main.o: main.cpp
	$(CXX) -c main.cpp -o main.o $(CXXFLAGS)

random.o: random.cpp random.h
	$(CXX) -c random.cpp -o random.o $(CXXFLAGS)

.PHONY: clean
clean:
	rm -f *.o $(TARGET)