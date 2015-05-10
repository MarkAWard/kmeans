#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "helper.h"

#define DELIMITER ","

double l2_distance(double *x1, double *x2, int d);

int main(int argc, char **argv) {

    options opt;
    parse_command_line(argc, argv, &opt);

    int i, j;
    double **data = (double**) malloc(opt.n_points * sizeof(double*));
    double *_data = (double*) malloc(opt.n_points * opt.dimensions * sizeof(double));
    for(j = 0; j < opt.n_points; j++){
        data[j] = _data + (j * opt.dimensions);
    }

    read_data(opt.filename, ",", data, opt.n_points, opt.dimensions);
    for(i=0; i<opt.n_points; i++){
        for(j=0; j<opt.dimensions; j++){
            printf("%d, %d, %f\n", i, j, data[i][j]);
        }
    }

    printf("\n");
    for (i = 0; i < opt.n_points; ++i) {
        for(j = i+1; j < opt.n_points; j++){
            printf("%d, %d, %f\n", i, j, l2_distance(data[i], data[j], opt.dimensions));
        }
    }

    free(_data);
    free(data); 
    return 0;
}


double l2_distance(double *x1, double *x2, int d) {
    int i;
    double dist = 0.0;
    for (i = 0; i < d; ++i) {
        dist += (x1[i] - x2[i]) * (x1[i] - x2[i]); 
    }
    return sqrt(dist);
}





