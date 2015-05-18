EXECUTABLES = seq-kmeans mpi-kmeans
COMPILER = gcc
MPICOMPILER = mpicc
CFLAGS = -g -Wall -lm -O3

.PHONY: all
all: $(EXECUTABLES)

.PHONY: seq
seq: seq-kmeans

seq-kmeans: seq-kmeans.c helper.c
	$(COMPILER) $(CFLAGS) $^ -o $@

.PHONY: mpi
mpi: mpi-kmeans

mpi-kmeans: mpi-kmeans.c mpifile.c helper.c
	$(MPICOMPILER) $(CFLAGS) -D TIME_ALL $^ -o $@

.PHONY: seq-test
seq-test:
	./seq-kmeans -f test_data.csv -n 25 -d 3 -k 5 -v 2

.PHONY: seq-blobs
seq-blobs:
	./seq-kmeans -f blobs_10000_10_k20.csv -k 20 -t 200 

.PHONY: mpi-test
mpi-test:
	mpirun -n 4 ./mpi-kmeans -f test_data.csv -k 5 -v 2

.PHONY: mpi-blobs
mpi-blobs:
	mpirun -n 4 ./mpi-kmeans -f blobs_10000_10_k20.csv -k 20 -t 200 

.PHONY: seq-memcheck
seq-memcheck:
	valgrind --leak-check=full ./seq-kmeans -f test_data.csv -n 25 -d 3 -k 5

.PHONY: mpi-memcheck
mpi-memcheck:
	mpirun -n 4 valgrind --leak-check=full ./mpi-kmeans -f test_data.csv -k 5

.PHONY: clean
clean:
	rm -rf $(EXECUTABLES) *~ a.out *.dSYM
