#include <stdlib.h>
#include <string.h>
#include "helper.h"

/***
* TODO: provide real help menu
***/
void exit_with_help(){
    printf("HELP HELP HELPPPP!!\n");
    exit(-1);
}

/***
* TODO: defaults for n_points/sep to -1
*		add the other options
***/
#define DELIMITER ","
#define NCLUSTERS 5
#define EPSILON 1e-4
#define MAX_ITER 300
#define MAX_TRIALS 25
#define VERBOSE 0
void parse_command_line(int argc, char **argv, options *opt) {
    // default options
    opt->sep = DELIMITER;
    opt->n_centroids = NCLUSTERS;
    opt->tol = EPSILON;
    opt->max_iter = MAX_ITER;
    opt->trials = MAX_TRIALS;
    opt->verbose = VERBOSE;
    // parse options
    int i;
    for(i=1;i<argc;i++) {
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
            case 'k':
                opt->n_centroids = atoi(argv[i]);
                break;
            case 'e':
                opt->tol = atoi(argv[i]);
                break;
            case 'i':
                opt->max_iter = atoi(argv[i]);
                break;
            case 't':
                opt->trials = atoi(argv[i]);
                break;
            case 'v':
                opt->verbose = atoi(argv[i]);
                break;
            default:
                fprintf(stderr,"unknown option: -%c\n", argv[i-1][1]);
                exit_with_help();
        }
    }
}

/***
* TODO: check for n_points/sep, determine from file if not given 
***/
void read_data(double **vec, options opt) {
    char buffer[1024] ;
    char *record, *line;
    int j, i = 0;
    FILE *fstream = fopen(opt.filename, "r");
    if(fstream == NULL) {
      printf("\n file opening failed ");
      exit(-1) ;
    }
    while( (line = fgets(buffer, sizeof(buffer), fstream)) != NULL && i < opt.n_points) {
        j = 0;
        record = strsep(&line, opt.sep);
        while(record != NULL && j < opt.dimensions) {
            vec[i][j++] = atof(record) ;
            record = strsep(&line, opt.sep);
        }
        i++;
    }
    fclose(fstream);
}

void *alloc2d(int rows, int cols) {
    double **v = (double**) malloc(rows * sizeof(double*));
    check(v);
    double *_v = (double*) malloc(rows * cols * sizeof(double));
    check(_v);
    int i;
    for(i = 0; i < rows; i++) {
        v[i] = _v + (i * cols);
    }    
    return v;
}



/***
 * Returns an integer in the range [0, n).
 * Uses rand(), and so is affected-by/affects the same seed.
 */
int randint(int n) {
    if ((n - 1) == RAND_MAX) {
        return rand();
    } 
    else {
        // Chop off all of the values that would cause skew...
        long end = RAND_MAX / n; // truncate skew
        end *= n;    
        // and ignore results from rand() that fall above that limit.
        int r;
        while ((r = rand()) >= end);
        return r % n;
    }
}

int In(int idx, int *arr, int end) {
    int i;
    for(i = 0; i < end; i++){
        if(arr[i] == idx) return 1;
    }
    return 0;
}

void check(void *v) {
    if(v == NULL){
        printf("malloc failed\n");
        exit(-1);
    }
}

double *add(double *to, double *from, options opt) {
    int i;
    for(i = 0; i < opt.dimensions; i++)
        to[i] += from[i];
    return to;
}

double *div_by(double *vec, int c, options opt) {
    int i;
    for(i = 0; i < opt.dimensions; i++)
        vec[i] /= (double) c;
    return vec;
}

void *memory_copy(double **dest, double **src, options opt) {
    int i;
    for(i = 0; i < opt.n_centroids; i++){
        memcpy(dest[i], src[i], opt.dimensions * sizeof(double));
    }
    return dest;
}

void *memory_set(double **dest, double c, options opt) {
    int i;
    for(i = 0; i < opt.n_centroids; i++){
        memset(dest[i], c, opt.dimensions * sizeof(double));
    }
    return dest;
}

void print_vecs(double **vec, options opt, char *type) {
    int i, end;
    if(strcmp(type, "centroids") == 0) end = opt.n_centroids;
    else end = opt.n_points;
    for(i = 0; i < end; i++) {
        print_vec(vec[i], opt.dimensions);
    }
}

void print_vec(double *vec, int len) {
    int j;
    for(j = 0; j < len; j++) {
        printf("%f ", vec[j]);
    }
    printf("\n");
}





