#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "helper.h"

#define SQR(x) (x)*(x)

void initialize(double **data, double **centroids, options opt);
double l2_distance(double *x1, double *x2, options opt);
void find_nearest_centroid(double *x, double **centroids, options opt, \
                            int *idx, double *distance);

int main(int argc, char **argv) {

    srand(time(NULL));
    int i, j;
    options opt;
    parse_command_line(argc, argv, &opt);

    double **data = (double**) malloc(opt.n_points * sizeof(double*));
    double *_data = (double*) malloc(opt.n_points * opt.dimensions * sizeof(double));
    for(i = 0; i < opt.n_points; i++) {
        data[i] = _data + (i * opt.dimensions);
    }
    read_data(data, opt);

    double **centroids = (double**) malloc(opt.n_centroids * sizeof(double*));
    double *_centroids = (double*) malloc(opt.n_centroids * opt.dimensions * sizeof(double));
    for(i = 0; i < opt.n_centroids; i++) {
        centroids[i] = _centroids + (i * opt.dimensions);
    }
    initialize(data, centroids, opt);

    // for (i = 0; i < opt.n_points; ++i) {
    //     for(j = i+1; j < opt.n_points; j++){

    //     }
    // }

    // printf("\n");
    // for (i = 0; i < opt.n_points; ++i) {
    //     for(j = i+1; j < opt.n_points; j++){
    //         printf("%d, %d, %f\n", i, j, l2_distance(data[i], data[j], opt));
    //     }
    // }

    int idx;
    double dist;
    printf("\n");
    for (i = 0; i < opt.n_points; ++i) {
        find_nearest_centroid(data[i], centroids, opt, &idx, &dist);
        printf("%d, %d, %f\n", i, idx, dist); 
    }


    free(_data);
    free(data); 
    free(centroids);
    free(_centroids);
    return 0;
}

void initialize(double **data, double **centroids, options opt) {
    int i, idx;
    int *init = (int*) malloc(opt.n_centroids * sizeof(int));
    for(i = 0; i < opt.n_centroids; i++){
        while(In(idx = randint(opt.n_points), init, i));
        init[i] = idx;
        printf("%d\n", idx);
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



