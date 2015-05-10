#include <stdlib.h>
#include <string.h>
#include "helper.h"

/***
* TODO: provide real help menu
***/
void exit_with_help(){
    printf("HELP HELP HELPPPP!!\n");
    exit(1);
}

/***
* TODO: defaults for n_points/sep to -1
*		add the other options
***/
#define DELIMITER ","
#define NCLUSTERS 5
void parse_command_line(int argc, char **argv, options *opt) {
    // default options
    opt->sep = DELIMITER;
    opt->n_centroids = NCLUSTERS;
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
            default:
                fprintf(stderr,"unknown option: -%c\n", argv[i-1][1]);
                exit_with_help();
        }
    }
}

/***
* TODO: check for n_points/sep, determine from file if not given 
***/
int read_data(double **vec, options opt) {
    char buffer[1024] ;
    char *record, *line;
    int j, i = 0;
    FILE *fstream = fopen(opt.filename, "r");
    if(fstream == NULL) {
      printf("\n file opening failed ");
      return -1 ;
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
    return 0;
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






