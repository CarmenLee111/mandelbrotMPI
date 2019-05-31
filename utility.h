
/*******************************************************************
 * This header file contains dependencies and utlity functions     *
 *******************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <assert.h>

/*******************************************************************
 * Global variables pertaining to utilities                        *
 *******************************************************************/
int size, rank;
int i, j;
char *outputfile=NULL;
double starttime, t;


void write_output(int m, int n, int* array, char* filename);
void print_matrix(int* A, int m, int n);
double get_wall_seconds();

/*******************************************************************
 * write the output in a text file                                 *
 * args:                                                           *
 *      m - number of rows                                         *
 *      n - number of columns                                      *
 *      array - the 1D array to be reshaped to 2D                  *
 *      filename - output file name                                *
 *******************************************************************/
void write_output(int m, int n, int* array, char* filename) {
  if (filename != NULL) {
    FILE *fp = fopen(filename, "w");
    int i;
    for (i=0; i<m*n; i++) {
        fprintf(fp, "%d ", array[i]);
        if ((i+1)%n == 0) fprintf(fp, "\n");
    }
    fclose(fp);
  }
}


/*******************************************************************
 * print the matrix                                                *
 * args:                                                           *
 *      A - the matrix                                             *
 *      m - number of rows                                         *
 *      n - number of columns                                      *
 *******************************************************************/
void print_matrix(int* A, int m, int n){
    int i;
    for (i=0; i<n*m; i++) {
        printf("%d ", A[i]);
        if ((i+1)%n == 0) printf("\n");
    }
    printf("\n");
}


/*******************************************************************
 * get wall time in seconds                                        *
 *******************************************************************/
double get_wall_seconds() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  double seconds = tv.tv_sec + (double)tv.tv_usec / 1000000;
  return seconds;
}
