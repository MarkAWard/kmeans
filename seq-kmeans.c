#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "csvparser.h"

#define DELIMITER ","

void exit_with_help(){
    printf("HELP HELP HELPPPP!!\n");
    exit(1);
}

struct options {
    char *filename;
    char *sep;
    int n_points;
    int dimensions;
};
typedef struct options options;


void parse_command_line(int argc, char **argv, options *opt);
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





void parse_command_line(int argc, char **argv, options *opt) {
    // default options
    opt->sep = DELIMITER;
    // parse options
    int i;
    for(i=1;i<argc;i++) {
        printf("%d\n", i);
        if(argv[i][0] != '-') break;
        if(++i>=argc) exit_with_help();
        switch(argv[i-1][1]) {
            case 'f':
                opt->filename = argv[i];
                break;
            case 'n':
                opt->n_points = atoi(argv[i]);
                break;
            case 'd':
                opt->dimensions = atoi(argv[i]);
                break;
            case 's':
                opt->sep = argv[i];
            default:
                fprintf(stderr,"unknown option: -%c\n", argv[i-1][1]);
                exit_with_help();
        }
    }
}

