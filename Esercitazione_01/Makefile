# Compilatore e flag di ottimizzazione
CXX = g++
CXXFLAGS = -Wall -O3 -std=c++11

# 1. Trova automaticamente tutti i file che iniziano con "esercizio" e finiscono con ".cpp"
SRCS = $(wildcard esercizio*.cpp)

# 2. Crea una lista degli eseguibili sostituendo ".cpp" con ".exe"
EXECS = $(SRCS:.cpp=.exe)

# Regola di default eseguita quando scrivi semplicemente "make" nel terminale
all: $(EXECS)

# Regola per compilare la libreria dei numeri casuali
random.o: random.cpp random.h
	$(CXX) $(CXXFLAGS) -c random.cpp -o random.o

# Regola "magica" per compilare ogni singolo esercizio
# Il simbolo % agisce da jolly. $< è il file .cpp, $@ è l'eseguibile finale
%.exe: %.cpp random.o
	$(CXX) $(CXXFLAGS) $< random.o -o $@

# Regola per fare pulizia (comando: make clean)
clean:
	rm -f *.o *.exe