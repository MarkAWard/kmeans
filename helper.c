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
void parse_command_line(int argc, char **argv, options *opt) {
    // default options
    opt->sep = DELIMITER;
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


