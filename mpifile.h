#ifndef MPIFILE_H
#define MPIFILE_H

double **mpi_read_data(MPI_File *in, int *n_rows, int *dimensions, \
					const int rank, const int size, const int overlap);

#endif