#ifndef HELPER_H
#define HELPER_H

#include <stdio.h>

typedef struct options {
    char *filename;
    char *sep;
    int n_points;
    int dimensions;
    int n_centroids;
    int max_iter;
    double tol;
    int trials;
    int verbose;
} options;

void exit_with_help();
void parse_command_line(int argc, char **argv, options *opt);

void read_data(double **vec, options opt);
void get_dimensions(options *opt);
void *alloc2d(int rows, int cols);
void *memory_copy(double **dest, double **src, options opt);
void *memory_set(double **dest, double c, options opt);

int randint(int n);
int In(int idx, int *arr, int end);
void check(void *v);
double *add(double *to, double *from, options opt);
double *div_by(double *vec, int c, options opt);

void print_vecs(double **vec, options opt, char *type);
void print_vec(double *vec, int len);

#endif