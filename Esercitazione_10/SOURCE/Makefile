# VARIABILI=================================================
# CC: Specifica quale compilatore utilizzare: in questo caso, mpic++ (il wrapper di g++ per MPI)
CC = mpic++

# CFLAGS: Flag di compilazione (usati per tradurre i .cpp in .o)
# -O3      : Massima ottimizzazione del codice per la velocità d'esecuzione
# -std=c++11: Usa lo standard C++11
# -Wall -Wextra -pedantic: I "cani da guardia". Attivano tutti gli avvisi del compilatore per scovare bug nascosti.
CFLAGS = -O3 -std=c++11 -Wall -Wextra -pedantic

# AFLAGS: Flag per il linking (usati per collegare le librerie all'eseguibile finale)
# -larmadillo: Indica al linker di includere la libreria matematica Armadillo
AFLAGS = -larmadillo


# REGOLA DI DEFAULT==========================================
# "make" cercherà di costruire "all", che a sua volta richiede "main".
all: main


# FASE DI LINKING (Creazione dell'eseguibile)================
# $@  --> Viene sostituito in automatico col nome del target (main)
# $^  --> Viene sostituito in automatico con TUTTE le dipendenze (i vari .o)
main : main.o Population.o Individual.o CityMap.o Parameters.o random.o
	$(CC) $^ -o $@ $(AFLAGS)


# FASE DI COMPILAZIONE (Creazione dei file .o)================
# Il flag "-c" dice a g++ di fermarsi alla creazione del file oggetto (.o) senza cercare di creare un eseguibile.
# $<  --> Viene sostituito in automatico con la PRIMA dipendenza (il file .cpp)
# $@  --> Viene sostituito in automatico col nome del target (il file .o)

main.o : main.cpp Population.h Individual.h random.h
	$(CC) -c $< -o $@ $(CFLAGS)
Population.o : Population.cpp Population.h Individual.h random.h
	$(CC) -c $< -o $@ $(CFLAGS)
Individual.o : Individual.cpp Individual.h random.h
	$(CC) -c $< -o $@ $(CFLAGS)
CityMap.o : CityMap.cpp CityMap.h random.h
	$(CC) -c $< -o $@ $(CFLAGS)
Parameters.o : Parameters.cpp Parameters.h random.h
	$(CC) -c $< -o $@ $(CFLAGS)
random.o : random.cpp random.h
	$(CC) -c $< -o $@ $(CFLAGS)


# REGOLE DI PULIZIA===========================================
# "-f" (force) dice al comando rm di non dare errore se i file da cancellare non esistono già.

clean:
	rm -f *.o main

remove:
	rm -f ../OUTPUT/*.*




# Come funziona un makefile?
# bersaglio (target): dipendenze (file necessari)
# <TAB>comando per creare il bersaglio