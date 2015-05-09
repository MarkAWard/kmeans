#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "csvparser.h"

double l2_distance(double *x1, double *x2, int d);

int main(int argc, char **argv) {

    char *filename = argv[1];
    int N = atoi(argv[2]);
    int d = atoi(argv[3]);

    int i, j;
    double **data = (double**) malloc(N * sizeof(double*));
    double *_data = (double*) malloc(N * d * sizeof(double));
    for(j = 0; j < N; j++){
        data[j] = _data + (j * d);
    }

    read_data(filename, ",", data, N, d);
    for(i=0; i<N; i++){
        for(j=0; j<d; j++){
            printf("%d, %d, %f\n", i, j, data[i][j]);
        }
    }

    printf("\n");
    for (i = 0; i < N; ++i) {
    	for(j = i+1; j < N; j++){
    		printf("%d, %d, %f\n", i, j, l2_distance(data[i], data[j], d));
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