#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "helper.h"

#define SQR(x) (x)*(x)

double l2_distance(double *x1, double *x2, options opt);
void find_nearest_centroid(double *x, double **centroids, options opt, \
                            int *idx, double *distance);

int main(int argc, char **argv) {

    options opt;
    parse_command_line(argc, argv, &opt);

    int i, j;
    double **data = (double**) malloc(opt.n_points * sizeof(double*));
    double *_data = (double*) malloc(opt.n_points * opt.dimensions * sizeof(double));
    for(j = 0; j < opt.n_points; j++){
        data[j] = _data + (j * opt.dimensions);
    }

    read_data(data, opt);
    printf("\n");
    for (i = 0; i < opt.n_points; ++i) {
        for(j = i+1; j < opt.n_points; j++){
            printf("%d, %d, %f\n", i, j, l2_distance(data[i], data[j], opt));
        }
    }

    int idx;
    double dist;
    printf("\n");
    for (i = 0; i < opt.n_points; ++i) {
        find_nearest_centroid(data[i], data, opt, &idx, &dist);
        printf("%d, %d, %f\n", i, idx, dist); 
    }


    free(_data);
    free(data); 
    return 0;
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
    for(i = 1; i < opt.n_points; i++){
        curr_distance = l2_distance(x, centroids[i], opt);
        if( curr_distance < *distance) {
            *distance = curr_distance;
            *idx = i;
        }
    }
}



