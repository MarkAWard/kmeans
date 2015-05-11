EXECUTABLES = seq-kmeans
COMPILER = gcc
CFLAGS = -g -Wall -lm -O3

all: $(EXECUTABLES)
.PHONY: all

seq-kmeans: seq-kmeans.c helper.c
	$(COMPILER) $(CFLAGS) $^ -o $@

.PHONY: test
test:
	./seq-kmeans -f test_data.csv -n 25 -d 3 -k 5 -v 2

.PHONY: memcheck
memcheck:
	valgrind --leak-check=full ./seq-kmeans -f test_data.csv -n 25 -d 3 -k 5

.PHONY: clean
clean:
	rm -rf $(EXECUTABLES) *~ a.out *.dSYM
