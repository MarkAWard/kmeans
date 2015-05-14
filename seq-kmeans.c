#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <float.h>
#include "helper.h"

#define SQR(x) (x)*(x)

void initialize(double **data, double **centroids, options opt);
double l2_distance(double *x1, double *x2, options opt);
void find_nearest_centroid(double *x, double **centroids, options opt, \
                            int *idx, double *distance);
void _kmeans(double **data, double **centroids, int *membership, \
            double *inertia, options opt); 
void kmeans(double **data, double **centroids, int *membership, \
            double *inertia, options opt); 


int main(int argc, char **argv) {

    srand(time(NULL));

    options opt;
    parse_command_line(argc, argv, &opt);
    if(opt.n_points == -1 || opt.dimensions == -1) get_dimensions(&opt);

    // allocate memory for data
    double **data = (double**) alloc2d(opt.n_points, opt.dimensions);
    // read in the data file
    read_data(data, opt);

    // allocate centroids
    double **centroids = (double**) alloc2d(opt.n_centroids, opt.dimensions);
    // allocate and initialize points' cluster memberships to 0
    int *membership = (int*) calloc(opt.n_points, sizeof(int));
    check(membership);

    double inertia = DBL_MAX;
    kmeans(data, centroids, membership, &inertia, opt);

    if(opt.verbose > 0) { 
        printf("\nINERTIA: %f\n", inertia);
        print_vecs(centroids, opt, "centroids");
    }

    free(*data);
    free(data); 
    free(*centroids);
    free(centroids);
    free(membership);

    return 0;
}

void initialize(double **data, double **centroids, options opt) {
    int i, idx;
    int *init = (int*) malloc(opt.n_centroids * sizeof(int));
    check(init);
    for(i = 0; i < opt.n_centroids; i++){
        while(In(idx = randint(opt.n_points), init, i));
        init[i] = idx;
        memcpy(centroids[i], data[idx], opt.dimensions * sizeof(double));
    }
    free(init);
}

double l2_distance(double *x1, double *x2, options opt) {
    int i;
    double dist = 0.0;
    for (i = 0; i < opt.dimensions; i++) {
        dist += SQR(x1[i] - x2[i]); 
    }
    return sqrt(dist);
}

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
            double *inertia, options opt) {
    
    double dist, delta = (double) opt.n_points;
    int i, center, iters = 0;

    // allocate for new centroids that will be computed
    double **new_centers = (double**) alloc2d(opt.n_centroids, opt.dimensions);
    memset(*new_centers, 0, opt.n_centroids * opt.dimensions * sizeof(double));
    // allocate array to count points in each cluster, initialize to 0
    int *count_centers = (int*) calloc(opt.n_centroids, sizeof(int));

    initialize(data, centroids, opt);
    

    while (delta / ((double) opt.n_points) > opt.tol && iters < opt.max_iter) {
        delta = 0.0;
        *inertia = 0.0;
        for(i = 0; i < opt.n_points; i++){
            find_nearest_centroid(data[i], centroids, opt, &center, &dist);
            *inertia += dist;
            if (membership[i] != center) {
                delta++;
                membership[i] = center;       
            }
            add(new_centers[center], data[i], opt);
            count_centers[center]++; 
        }
        for(i = 0; i < opt.n_centroids; i++) {
            if(count_centers[i] == 0) {
                // pick a random point, not sure if this is the best option
                add(new_centers[i], data[randint(opt.n_points)], opt);
                // add to delta to ensure we dont stop after this
                delta += opt.tol * opt.n_points + 1.0;
            }
            else {
                // calculate the new center
                div_by(new_centers[i], count_centers[i], opt);
            }
        }
        memcpy(*centroids, *new_centers, opt.n_centroids * opt.dimensions * sizeof(double));
        // zero out new_centers and count_centers
        memset(*new_centers, 0, opt.n_centroids * opt.dimensions * sizeof(double));
        memset(count_centers, 0, opt.n_centroids * sizeof(int));

        iters++;
        if(opt.verbose > 1) {
            printf("\n\titers: %d\n", iters);
            printf("\tdelta: %d\n", (int)   delta);
            printf("\teps: %f\n", delta / ((double) opt.n_points));
            printf("\tinertia: %f\n", *inertia);
        }
    }
    free(*new_centers);
    free(new_centers);
    free(count_centers);
}

void kmeans(double **data, double **centroids, int *membership, \
            double *inertia, options opt) {
    int i;
    double **temp_centroids = (double**) alloc2d(opt.n_centroids, opt.dimensions);
    int *temp_membership = (int*) calloc(opt.n_points, sizeof(int));
    double temp_inertia = DBL_MAX;
    for(i = 0; i < opt.trials; i++){
        if(opt.verbose > 1) printf("\nTRIAL %d\n", i+1);
        _kmeans(data, temp_centroids, temp_membership, &temp_inertia, opt);
        if(temp_inertia < *inertia) {
            *inertia = temp_inertia;
            memcpy(*centroids, *temp_centroids, opt.n_centroids * opt.dimensions * sizeof(double));
            memcpy(membership, temp_membership, opt.n_points * sizeof(int));
        }
    }
    free(*temp_centroids);
    free(temp_centroids);
    free(temp_membership);
}



