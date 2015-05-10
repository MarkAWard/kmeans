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
void kmeans(double **data, double **centroids, int *membership, \
            double *inertia, options opt) ; 


int main(int argc, char **argv) {

    srand(time(NULL));
    int i;
    options opt;

    parse_command_line(argc, argv, &opt);

    // allocate memory for data
    double **data = (double**) malloc(opt.n_points * sizeof(double*));
    check(data);
    double *_data = (double*) malloc(opt.n_points * opt.dimensions * sizeof(double));
    check(_data);
    for(i = 0; i < opt.n_points; i++) {
        data[i] = _data + (i * opt.dimensions);
    }
    // read in the data file
    read_data(data, opt);

    // allocate centroids
    double **centroids = (double**) malloc(opt.n_centroids * sizeof(double*));
    check(centroids);
    // allocate continuous memory chunks for faster sequential access
    double *_centroids = (double*) malloc(opt.n_centroids * opt.dimensions * sizeof(double));
    check(_centroids);
    for(i = 0; i < opt.n_centroids; i++) {
        centroids[i] = _centroids + (i * opt.dimensions);
    }
    // initialize the centriods with random data points
    initialize(data, centroids, opt);

    // allocate and initialize points' cluster memberships to 0
    int *membership = (int*) calloc(opt.n_points, sizeof(int));
    check(membership);

    // int idx;
    // double dist;
    // printf("\n");
    // for (i = 0; i < opt.n_points; ++i) {
    //     find_nearest_centroid(data[i], centroids, opt, &idx, &dist);
    //     printf("%d, %d, %f\n", i, idx, dist); 
    // }

    // print_vecs(centroids, opt, "centroids");

    double inertia = DBL_MAX;
    kmeans(data, centroids, membership, &inertia, opt);

    printf("\nINERTIA: %f\n", inertia);
    print_vecs(centroids, opt, "centroids");

    free(_data);
    free(data); 
    free(centroids);
    free(_centroids);
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
    for (i = 0; i < opt.dimensions; ++i) {
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


void kmeans(double **data, double **centroids, int *membership, \
            double *inertia, options opt) {
    
    double dist, delta = (double) opt.n_points;
    int i, center, iters = 0;

    // allocate for new centroids that will be computed
    double **new_centers = (double**) malloc(opt.n_centroids * sizeof(double*));
    check(new_centers);
    // and initialize to 0
    double *_new_centers = (double*) calloc(opt.n_centroids * opt.dimensions, sizeof(double));
    check(_new_centers);
    for(i = 0; i < opt.n_centroids; i++) {
        new_centers[i] = _new_centers + (i * opt.dimensions);
    }
    // allocate array to count points in each cluster, initialize to 0
    int *count_centers = (int*) calloc(opt.n_centroids, sizeof(int));

    printf("\n");
    printf("max_iter: %d\n", opt.max_iter);
    printf("tol: %f\n", opt.tol);
    
    printf("\n");
    printf("iters: %d\n", iters);
    printf("delta: %f\n", delta);
    printf("eps: %f\n", delta / ((double) opt.n_points));
    printf("inertia: %f\n", *inertia);
    print_vecs(centroids, opt, "centroids");

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
        printf("\n");
        for(i = 0; i < opt.n_centroids; i++) {
            if(count_centers[i] == 0) {
                printf("HERE 0\n");
                // pick a random point, not sure if this is the best option
                add(new_centers[i], data[randint(opt.n_points)], opt);
                // add to delta to ensure we dont stop after this
                delta += opt.tol * opt.n_points + 1.0;
            }
            else {
                printf("HERE 1: %d\n", count_centers[i]);
                print_vec(new_centers[i], opt.dimensions);
                // calculate the new center
                div_by(new_centers[i], count_centers[i], opt);
                print_vec(new_centers[i], opt.dimensions);
            }
        }
        printf("\nBEFORE\n");
        print_vecs(centroids, opt, "centroids");
        memory_copy(centroids, new_centers, opt);
        printf("AFTER\n");
        print_vecs(centroids, opt, "centroids");
        // zero out new_centers and count_centers
        memory_set(new_centers,   0, opt);
        memset(count_centers, 0, opt.n_centroids * sizeof(int));
        printf("AFTER MEMSET\n");
        print_vecs(centroids, opt, "centroids");

        iters++;        
        printf("\n");
        printf("iters: %d\n", iters);
        printf("delta: %f\n", delta);
        printf("eps: %f\n", delta / ((double) opt.n_points));
        printf("inertia: %f\n", *inertia);
        print_vecs(centroids, opt, "centroids");
    }
    free(new_centers);
    free(_new_centers);
    free(count_centers);
}



