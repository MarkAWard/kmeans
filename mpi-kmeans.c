#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <float.h> // DBL_MAX
#include <string.h>
#include <mpi.h>
#include "helper.h"
#include "mpifile.h"

void initialize(double **data, double **centroids, int *ppp, int rank, int size, options opt);
int get_owner(int *point_id, int *ppp);

int main( int argc, char **argv) {

  srand(time(NULL));

  int mpi_rank, mpi_size;
  int i, r, rows, cols, total_rows, err;
  const int overlap = 100;
  int *points_per_proc, *buffer;
  MPI_File filename;
  
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
  points_per_proc = (int*) calloc(mpi_size, sizeof(int));
  check(points_per_proc); 
  buffer = (int*) calloc(mpi_size, sizeof(int));
  check(buffer);
  buffer[mpi_rank] = rows;
  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Allreduce(buffer, points_per_proc, mpi_size, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
  free(buffer);
  MPI_Allreduce(&rows, &total_rows, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

  opt.n_points = total_rows;
  opt.dimensions = cols;
  if(mpi_rank == 0 && opt.verbose > 1) {
    printf("Total rows: %d\n", opt.n_points);
    for(r = 0; r < mpi_size; r++)
      printf("proc %d has %d\n", r, points_per_proc[r]);
  }
  // allocate centroids, everyone gets their own copy
  double **centroids = (double**) alloc2d(opt.n_centroids, opt.dimensions);
  // allocate and initialize points' cluster memberships to 0
  // only track the ones this process is responsible for
  int *membership = (int*) calloc(rows, sizeof(int));
  check(membership);

//  double inertia = DBL_MAX;

  initialize(data, centroids, points_per_proc, mpi_rank, mpi_size, opt);


  for(r=0; r < mpi_size; r++) {
    MPI_Barrier(MPI_COMM_WORLD);
    if(mpi_rank == r && opt.verbose > 1) {
      for(i=0; i < rows; i++) {
        printf("proc %d: %d --- ", mpi_rank, i);
        print_vec(data[i], cols);
      }
    }
  }

  MPI_File_close(&filename);

  free(points_per_proc);
  free(*data);
  free(data);
  free(*centroids);
  free(centroids);
  free(membership);

  MPI_Finalize();

  return 0;
}


void initialize(double **data, double **centroids, int *ppp, int rank, int size, options opt) {
  MPI_Status status;
  if(rank == 0) {
    int i, idx, owner;
    int *init = (int*) malloc(opt.n_centroids * sizeof(int));
    check(init);
    double *point = (double*) malloc(opt.dimensions * sizeof(double));
    check(point);
    for(i = 0; i < opt.n_centroids; i++){
        while(In(idx = randint(opt.n_points), init, i));
        init[i] = idx;
        owner = get_owner(&idx, ppp);
        if(owner != 0) {
          MPI_Send(&idx, 1, MPI_INT, owner, 999, MPI_COMM_WORLD);
          MPI_Recv(point, opt.dimensions, MPI_DOUBLE, owner, 999, MPI_COMM_WORLD, &status);
        } 
        else{
          point = data[idx];
        }
        printf("%d owned by %d at %d ", init[i], owner, idx);
        print_vec(point, opt.dimensions);
        // memcpy(centroids[i], data[idx], opt.dimensions * sizeof(double));
    }
    idx = -1;
    for(i = 1; i < size; i++)
      MPI_Send(&idx, 1, MPI_INT, i, 999, MPI_COMM_WORLD);
    free(init);
    free(point);
  }
  else {
    int get_point;
    while(1) {
      MPI_Recv(&get_point, 1, MPI_INT, 0, 999, MPI_COMM_WORLD, &status);
      if(get_point != -1)
        MPI_Send(data[get_point], opt.dimensions, MPI_DOUBLE, 0, 999, MPI_COMM_WORLD);
      else break;
    }
  }
}

int get_owner(int *point_id, int *ppp) {
  int i;
  for(i = 0; (*point_id -= ppp[i]) >= 0; i++);
  *point_id += ppp[i];
  return i;
}







