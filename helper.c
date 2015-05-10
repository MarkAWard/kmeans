#include <stdlib.h>
#include "helper.h"
#include "csvparser.h"

void exit_with_help(){
    printf("HELP HELP HELPPPP!!\n");
    exit(1);
}

#define DELIMITER ","
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

int read_data(double **vec, options opt) {
    CsvParser *csvparser = CsvParser_new(opt.filename, opt.sep, 0);
    CsvRow *row;
    int j, i = 0;
    while ((row = CsvParser_getRow(csvparser)) && i < opt.n_points ) {
        char **rowFields = CsvParser_getFields(row);
        if (CsvParser_getNumFields(row) < opt.dimensions) return -1;
        for (j = 0 ; j < CsvParser_getNumFields(row) && j < opt.dimensions; j++) {
            vec[i][j] = atof(rowFields[j]);
        }
        CsvParser_destroy_row(row);
        i++;
    }
    CsvParser_destroy(csvparser);
    return 0;
}