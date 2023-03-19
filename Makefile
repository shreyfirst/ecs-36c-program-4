CC=g++
DEV=-Wall -g -std=c++14 -Wno-reorder
OPT=-O3 -std=c++14

.PHONY: all
all: AVLSanityCheck CreateData AVLCommands

CreateData: CreateData.cxx json.hpp
	$(CC) $(OPT) CreateData.cxx -o CreateData.exe

AVLSanityCheck: AVLSanityCheck.cxx AVL.o
	$(CC) $(DEV) AVLSanityCheck.cxx AVL.o -o AVLSanityCheck.exe

AVLCommands: AVLCommands.cxx AVL.o json.hpp
	$(CC) $(DEV) AVLCommands.cxx AVL.o -o AVLCommands.exe -I/path/to/nlohmann/json/include

AVL.o: AVL.cpp AVL.h
	$(CC) $(DEV) -c AVL.cpp

# Build
.PHONY: clean
clean:
	rm -f *.o
	rm -f *.exe
	rm -rf *dSYM

.PHONY: update
update:
	make clean
	make all
