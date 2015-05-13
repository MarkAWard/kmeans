#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "helper.h"
#include "mpifile.h"

int main( int argc, char **argv) {

  int mpi_rank, mpi_size;
  const int overlap = 100;
  MPI_File filename;
  int err;
  
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

  if (argc != 2) {
    if (mpi_rank == 0) fprintf(stderr, "Usage: %s infilename\n", argv[0]);
    MPI_Finalize();
    exit(1);
  }

  err = MPI_File_open(MPI_COMM_WORLD, argv[1], MPI_MODE_RDONLY, MPI_INFO_NULL, &filename);
  if (err) {
    if (mpi_rank == 0) fprintf(stderr, "Couldn't open file %s\n", argv[1]);
    MPI_Finalize();
    exit(2);
  }

  int i, r;
  int rows, cols;
  double **data = mpi_read_data(&filename, &rows, &cols, mpi_rank, mpi_size, overlap);

  for(r=0; r < mpi_size; r++) {
    MPI_Barrier(MPI_COMM_WORLD);
    if(mpi_rank == r) {
      for(i=0; i < rows; i++) {
	printf("proc %d: %d --- ", mpi_rank, i);
      print_vec(data[i], cols);
      }
    }
  }

  MPI_File_close(&filename);

  free(*data);
  free(data);

  MPI_Finalize();

  return 0;
}