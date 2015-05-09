#include <stdlib.h>
#include <stdio.h>
#include "csvparser.h"


int main() {

    int i, j;
    double **data = (double**) malloc(5 * sizeof(double*));
    double *_data = (double*) malloc(5 * 3 * sizeof(double));
    for(j = 0; j < 5; j++){
        data[j] = _data + (j * 3);
    }

    read_data("test_data.csv", ",", data, 5, 3);
    for(i=0; i<5; i++){
        for(j=0; j<3; j++){
            printf("%d, %d, %f\n", i, j, data[i][j]);
        }
    }

    free(_data);
    free(data); 
    return 0;
}