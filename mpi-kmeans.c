#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>
#include <mpi.h>
#include "helper.h"
#include "mpifile.h"

int main( int argc, char **argv) {

  srand(time(NULL));

  int mpi_rank, mpi_size;
  int i, r, rows, cols, total_rows;
  const int overlap = 100;
  MPI_File filename;
  int err;
  
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

  options opt;
  parse_command_line(argc, argv, &opt);

  err = MPI_File_open(MPI_COMM_WORLD, opt.filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &filename);
  if (err) {
    if (mpi_rank == 0) fprintf(stderr, "Couldn't open file %s\n", argv[1]);
    MPI_Finalize();
    exit(1);
  }

  double **data = mpi_read_data(&filename, &rows, &cols, mpi_rank, mpi_size, overlap);
  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Allreduce(&rows, &total_rows, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
  opt.n_points = total_rows;
  opt.dimensions = cols;
  if(mpi_rank == 0) printf("Total rows: %d\n", opt.n_points);

  // allocate centroids, everyone gets their own copy
  double **centroids = (double**) alloc2d(opt.n_centroids, opt.dimensions);
  // allocate and initialize points' cluster memberships to 0
  // only track the ones this process is responsible for
  int *membership = (int*) calloc(rows, sizeof(int));
  check(membership);

  double inertia = DBL_MAX;

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
  free(*centroids);
  free(centroids);
  free(membership);

  MPI_Finalize();

  return 0;
}