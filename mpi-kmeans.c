#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <float.h> // DBL_MAX
#include <math.h> // sqrt
#include <string.h>
#include <mpi.h>
#include "helper.h"
#include "mpifile.h"

#define SQR(x) (x)*(x)

void initialize(double **data, double **centroids, int *ppp, int rank, int size, options opt);
int get_owner(int *point_id, int *ppp);
double l2_distance(double *x1, double *x2, options opt);
void find_nearest_centroid(double *x, double **centroids, options opt, \
                            int *idx, double *distance);
void _kmeans(double **data, double **centroids, int *membership, \
            double *inertia, int rank, int size, int *ppp, options opt);
void kmeans(double **data, double **centroids, int *membership, \
            double *inertia, int rank, int size, int *ppp, options opt);

int main( int argc, char **argv) {

  srand(time(NULL));

  int mpi_rank, mpi_size;
  int i, r, rows, cols, total_rows, err;
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

  double **data = mpi_read_data(&filename, &rows, &cols, mpi_rank, mpi_size, opt.overlap);
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
  opt.local_rows = rows;

  if(mpi_rank == 0 && opt.verbose > 1) {
    printf("Total rows: %d\n", opt.n_points);
    for(r = 0; r < mpi_size; r++)
      printf("proc %d has %d\n", r, points_per_proc[r]);
  }
  for(r=0; r < mpi_size; r++) {
    MPI_Barrier(MPI_COMM_WORLD);
    if(mpi_rank == r && opt.verbose > 1) {
      for(i=0; i < rows; i++) {
        printf("proc %d: %d --- ", mpi_rank, i);
        print_vec(data[i], cols);
      }
    }
  }

  // allocate centroids, everyone gets their own copy
  double **centroids = (double**) alloc2d(opt.n_centroids, opt.dimensions);
  // allocate  cluster memberships
  // only track the ones this process is responsible for
  int *membership = (int*) malloc(opt.local_rows * sizeof(int));
  check(membership);

  double inertia = DBL_MAX;

  kmeans(data, centroids, membership, &inertia, mpi_rank, mpi_size, points_per_proc, opt);

  if(mpi_rank == 0 && opt.verbose > 0) { 
    printf("\nINERTIA: %e\n", inertia);
    print_vecs(centroids, opt, "centroids");
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
    double *tofree = point;
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
        // printf("%d owned by %d at %d ", init[i], owner, idx);
        // print_vec(point, opt.dimensions);
        memcpy(centroids[i], point, opt.dimensions * sizeof(double));
        point = tofree;
    }
    idx = -1;
    for(i = 1; i < size; i++)
      MPI_Send(&idx, 1, MPI_INT, i, 999, MPI_COMM_WORLD);
    free(init);
    free(tofree);
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

// unchanged from sequential code
double l2_distance(double *x1, double *x2, options opt) {
    int i;
    double dist = 0.0;
    for (i = 0; i < opt.dimensions; i++) {
        dist += SQR(x1[i] - x2[i]); 
    }
    return sqrt(dist);
}

// unchanged from sequential code
void find_nearest_centroid(double *x, double **centroids, options opt, \
                            int *idx, double *distance) {
    int i;
    double curr_distance;
    *distance = l2_distance(x, centroids[0], opt);
    *idx = 0;
    for(i = 1; i < opt.n_centroids; i++){
        curr_distance = l2_distance(x, centroids[i], opt);
        if( curr_distance < *distance) {
            *distance = curr_distance;
            *idx = i;
        }
    }
}



void _kmeans(double **data, double **centroids, int *membership, \
            double *inertia, int rank, int size, int *ppp, options opt) {

  //MPI_Status status;

  double dist, total_inertia, total_delta, delta = (double) opt.n_points;
  int i, center, iters = 0;

  // allocate for new centroids that will be computed
  double **new_centers = (double**) alloc2d(opt.n_centroids, opt.dimensions);
  memset(*new_centers, 0, opt.n_centroids * opt.dimensions * sizeof(double));

  // allocate array to count points in each cluster, initialize to 0
  int *count_centers = (int*) calloc(opt.n_centroids, sizeof(int));
  check(count_centers);
  int *new_count_centers = (int*) calloc(opt.n_centroids, sizeof(int));
  check(new_count_centers);

  // if a cluster has 0 points assigned use this for random reinitialization
  double *point = (double*) malloc(opt.dimensions * sizeof(double));
  check(point);
  double *tofree = point;

  initialize(data, centroids, ppp, rank, size, opt);
  MPI_Bcast(*centroids, opt.n_centroids*opt.dimensions, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
  while (delta / ((double) opt.n_points) > opt.tol && iters < opt.max_iter) {
    // MPI_Barrier(MPI_COMM_WORLD);
    delta = 0.0;
    *inertia = 0.0;
    for(i = 0; i < opt.local_rows; i++){
        find_nearest_centroid(data[i], centroids, opt, &center, &dist);
        *inertia += dist;
        if (membership[i] != center) {
            delta++;
            membership[i] = center;       
        }
        add(new_centers[center], data[i], opt);
        new_count_centers[center]++; 
    }

    MPI_Allreduce(*new_centers, *centroids, opt.n_centroids * opt.dimensions, 
                  MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    MPI_Allreduce(new_count_centers, count_centers, opt.n_centroids, 
                  MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    for(i = 0; i < opt.n_centroids; i++) {
        if(count_centers[i] == 0) {

          if(rank == 0){
            add(centroids[i], data[randint(opt.local_rows)], opt);
          }
          // broadcast this new point to everyone
          MPI_Bcast(centroids[i], opt.dimensions, MPI_DOUBLE, 0, MPI_COMM_WORLD);
          // add to delta to ensure we dont stop after this
          delta += opt.tol * opt.local_rows + 1.0;
        }
        // all good to divide, count is not 0
        else {
          // calculate the new center
          div_by(centroids[i], count_centers[i], opt);
        }
    }

    // sum up the number of cluster assignments that changed
    MPI_Allreduce(&delta, &total_delta, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    delta = total_delta;
    // sum up the inertias
    MPI_Allreduce(inertia, &total_inertia, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    *inertia = total_inertia;
    // zero out new_centers and count_centers
    memset(*new_centers, 0, opt.n_centroids * opt.dimensions * sizeof(double));
    memset(new_count_centers, 0, opt.n_centroids * sizeof(int));
    memset(count_centers, 0, opt.n_centroids * sizeof(int));

    iters++;
    if(opt.verbose > 1 && rank == 0) {
        printf("\n\titers: %d\n", iters);
        printf("\tdelta: %d\n", (int)   delta);
        printf("\teps: %f\n", delta / ((double) opt.n_points));
        printf("\tinertia: %f\n", *inertia);
    }
  }
  free(*new_centers);
  free(new_centers);
  free(count_centers);
  free(new_count_centers);
  free(tofree);
} 


void kmeans(double **data, double **centroids, int *membership, \
            double *inertia, int rank, int size, int *ppp, options opt) {
  int i;
  double **temp_centroids = (double**) alloc2d(opt.n_centroids, opt.dimensions);
  int *temp_membership = (int*) calloc(opt.local_rows, sizeof(int));
  check(temp_membership);
  double temp_inertia = DBL_MAX;
  for(i = 0; i < opt.trials; i++){
    // MPI_Barrier(MPI_COMM_WORLD);
    if(opt.verbose > 1 && rank == 0) printf("\nTRIAL %d\n", i+1);
    _kmeans(data, temp_centroids, temp_membership, &temp_inertia, rank, size, ppp, opt);
    if(temp_inertia < *inertia) {
      *inertia = temp_inertia;
      memcpy(*centroids, *temp_centroids, opt.n_centroids * opt.dimensions * sizeof(double));
      memcpy(membership, temp_membership, opt.local_rows * sizeof(int));
    }
  }
  free(*temp_centroids);
  free(temp_centroids);
  free(temp_membership);
}



