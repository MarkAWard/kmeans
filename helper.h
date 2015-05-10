#ifndef HELPER_H
#define HELPER_H

#include <stdio.h>

typedef struct options {
    char *filename;
    char *sep;
    int n_points;
    int dimensions;
    int n_centroids;
} options;

void exit_with_help();
void parse_command_line(int argc, char **argv, options *opt);
int read_data(double **vec, options opt);
int randint(int n);
int In(int idx, int *arr, int end);

#endif