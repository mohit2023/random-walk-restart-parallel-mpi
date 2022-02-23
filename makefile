CC=g++
CFLAGS=-std=c++17

compile:
	mpic++ -std=c++17 -o wtf *.cpp librandomizer.a
run:
	mpirun -np 4 ./wtf data/8717_31525/edges.dat 8717 31525 0.1 5000 30 20 369

priorityQueue.o:
	mpic++ -std=c++17 -o priorityQueue.o priorityQueue.cpp

clean:
	rm wtf *.o