#include <stdio.h>
#include <stdlib.h> 
#include <string.h> // strsep
#include <mpi.h>


double **mpi_read_data(MPI_File *in, int *n_rows, int *dimensions, const int rank, const int size, const int overlap) {

  MPI_Offset globalstart;
  int mysize;
  char *chunk, *tofree;

  /* read in relevant chunk of file into "chunk",
   * which starts at location in the file globalstart
   * and has size mysize 
   */
  {
    MPI_Offset globalend;
    MPI_Offset filesize;

    /* figure out who reads what */
    MPI_File_get_size(*in, &filesize);
    filesize--;  /* get rid of text file eof */
    mysize = filesize/size;
    globalstart = rank * mysize;
    globalend   = globalstart + mysize - 1;
    if (rank == size-1) globalend = filesize-1;

    /* add overlap to the end of everyone's chunk except last proc... */
    if (rank != size-1)
      globalend += overlap;

    mysize =  globalend - globalstart + 1;

    /* allocate memory */
    tofree = chunk = malloc( (mysize + 1)*sizeof(char));

    /* everyone reads in their part */
    MPI_File_read_at_all(*in, globalstart, chunk, mysize, MPI_CHAR, MPI_STATUS_IGNORE);
    chunk[mysize] = '\0';
  }

  /*
   * everyone calculate what their start and end *really* are by going 
   * from the first newline after start to the first newline after the
   * overlap region starts (eg, after end - overlap + 1)
   */

  int locstart=0, locend=mysize-1;
  if (rank != 0) {
    while(chunk[locstart] != '\n') locstart++;
    locstart++;
  }
  if (rank != size-1) {
    locend-=overlap;
    while(chunk[locend] != '\n') locend++;
  }
  chunk[locend] = '\0';
  mysize = locend-locstart+1;

  /* 
   * Calculate the number of data points that are distributed
   * to each process, and the vector's dimension 
   */

  int i;
  *n_rows = *dimensions = 0;
  for(i = locstart; chunk[i] != '\0'; i++) {
    if(chunk[i] == '\n') (*n_rows)++;
    if(*n_rows == 0 && chunk[i] == ',') (*dimensions)++;
  }
  (*n_rows)++; (*dimensions)++;
  //  printf("%d x %d\n", *n_rows, *dimensions);

  // allocate the appropriate size for this process
  double **vec = (double**) malloc(*n_rows * sizeof(double*));
  double *_v = (double*) malloc(*n_rows * *dimensions * sizeof(double));
  for(i = 0; i < *n_rows; i++) {
    vec[i] = _v + (i * *dimensions);
  } 

  int start = locstart;
  int k, j = 0;
  char *line, *record, *ptr;
  for(i = locstart; i <= locend; i++){
    if(chunk[i] == '\n' || chunk[i] == '\0') {
      chunk[i] = '\0';
      line = &chunk[start];
      k = 0;
      record = strsep(&line, ",");
      while(record != NULL){
	vec[j][k++] = strtod(record, &ptr);
	record = strsep(&line, ",");
      }
      j++;
      start = i+1;
    }
  }
  
  free(tofree);

  return vec;
}
