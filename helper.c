#include <stdlib.h>
#include <string.h>
#include "helper.h"

void exit_with_help(char *argv0) {
    char *help =
    "Usage: %s [options] -f filename -k num_clusters\n"
    "       -f filename     : file containing data to be clustered\n"
    "       -k num_clusters : the number of clusters to find [5]\n"
    "       -s delimiter    : file delimiter [,]\n"
    "       -n num_points   : number of points in file to read (optional)\n"
    "       -d num_dimens   : number of dimensions of input data (optional)\n"
    "       -e tolerance    : minimum fraction of points that don't change clusters to end kmeans loop [1e-5]\n"
    "       -i max_iter     : maximum number of iterations within kmeans [100]\n"
    "       -t num_trials   : number of kmeans trials to perform to find best clustering [25]\n"
    "       -v verbosity    : control amount of printing 0, 1, 2 [0]\n"
    "       -b buf_overlap  : set to number of characters per line for mpi file reading [100]\n"
    "       -h              : show this help menu\n";
    fprintf(stderr, help, argv0);
    exit(-1);
}

// define defaults
#define DELIMITER ","
#define DIM -1
#define NCLUSTERS 5
#define EPSILON 1e-4
#define MAX_ITER 100
#define MAX_TRIALS 25
#define VERBOSE 0
#define LOCROWS 0
#define OVERLAP 100
#define HELP 0
void parse_command_line(int argc, char **argv, options *opt) {
    // default options
    opt->sep = DELIMITER;
    opt->n_points = DIM;
    opt->dimensions = DIM;
    opt->n_centroids = NCLUSTERS;
    opt->tol = EPSILON;
    opt->max_iter = MAX_ITER;
    opt->trials = MAX_TRIALS;
    opt->verbose = VERBOSE;
    opt->local_rows = LOCROWS;
    opt->overlap = OVERLAP;
    // parse options
    int i;
    for(i=1;i<argc;i++) {
        if(argv[i][0] != '-') break;
        if(++i>=argc) exit_with_help(argv[0]);
        switch(argv[i-1][1]) {
            case 'f':
                opt->filename = argv[i];
                break;
            case 'k':
                opt->n_centroids = atoi(argv[i]);
                break;
            case 's':
                opt->sep = argv[i];
                break;
            case 'n':
                opt->n_points = atoi(argv[i]);
                break;
            case 'd':
                opt->dimensions = atoi(argv[i]);
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
           case 'b':
                opt->overlap = atoi(argv[i]);
                break;
            case 'h':
                exit_with_help(argv[0]);
                break;
            default:
                fprintf(stderr,"unknown option: -%c\n", argv[i-1][1]);
                exit_with_help(argv[0]);
        }
    }
    if(i < argc) opt->filename = argv[i];
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

void get_dimensions(options *opt) {
    char *record, *buf = NULL;
    size_t line_cap = 0;
    int rows, cols;
    FILE *fp = fopen(opt->filename, "r");

    rows = cols = 0;    
    getline(&buf, &line_cap, fp); rows++;
    while((record = strsep(&buf, ","))) cols++;
    while (EOF != (fscanf(fp, "%*[^\n]"), fscanf(fp, "%*c"))) rows++;

    if(opt->n_points == -1) opt->n_points = rows;
    if(opt->dimensions == -1) opt->dimensions = cols;

    fclose(fp);
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

void print_int_vec(int *vec, int len) {
    int j;
    for(j = 0; j < len; j++) {
        printf("%d ", vec[j]);
    }
    printf("\n");
}




