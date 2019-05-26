#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

/* gloabl variables */
int size, rank;
int i, j, k;
const double threshold = 4.0f;         /* threshold if converges compare w/ modulus**2 */
const int    npls = 1024;               /* resolution of the image */
int maxiter;                           /* max number of iteration */


double modulus_sq(double x, double y);
void complex_sq(double *x, double *y);
void mandelbrot_set (double *pixels, int npls, int *mset);
void write_output(int n, int* array, char* filename);

int main(int argc, char *argv[]) {
    double xmin, ymin, xmax, ymax;        /* domain for the computation */
    
    if (argc != 7) {
        printf("Usage: ./mandelbrot <xmin> <xmax> <ymin> <ymax> <maxiter> <outputfile>");
        return -1;
    }

    xmin = atof(argv[1]);
    xmax = atof(argv[2]);
    ymin = atof(argv[3]);
    ymax = atof(argv[4]);
    maxiter = atoi(argv[5]);
    char *outputfile = argv[6];

    printf("xmin, xmax, ymin, ymax: (%f, %f, %f, %f) \n", xmin, xmax, ymin, ymax);

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    double dx = (xmax - xmin) / (double)(npls-1);
    double dy = (ymax - ymin) / (double)(npls-1);
    // double pixels[npls*npls*2];
    double *pixels = (double *) malloc(npls*npls*2 * sizeof(double));
    
    for (i=0; i<npls; i++) {
        for (j=0; j<npls; j++) {
            pixels[i*npls*2+j*2]   = xmin + dx * j;
            pixels[i*npls*2+j*2+1] = ymin + dy * i; 
        }
    }

    // for (i=0; i<npls; i++) {
    //     for (j=0; j<npls; j++) {
    //         printf("(%f, %f) ", pixels[i*npls*2+j*2], pixels[i*npls*2+j*2+1]);
    //     }
    //     printf("\n");
    // }
    // printf("\n");

    FILE *fp = fopen("domain", "w");
    for (i=0; i<npls*npls; i++) {
      fprintf(fp, "(%f, %f) ", pixels[i*2], pixels[i*2+1]);
      if ((i+1)%npls == 0) fprintf(fp, "\n");
    }
    fclose(fp);




    // int mset[npls*npls];
    int* mset = (int *) malloc(npls*npls*sizeof(int));
    mandelbrot_set(pixels, npls*npls, mset);

    // for (i=0; i<npls; i++) {
    //     for (j=0; j<npls; j++) {
    //         printf("%d ", mset[i*npls+j]);
    //     }
    //     printf("\n");
    // } 

    write_output(npls, mset, outputfile);
    // write_output(npls, mset, "outputall");



    free(pixels);
    free(mset);

    MPI_Finalize();

    return 0;
}


double modulus_sq(double x, double y){
    return x*x + y*y;
}

void complex_sq(double *x, double *y){
    double tmpx = (*x)*(*x) - (*y)*(*y);
    double tmpy = 2*(*x)*(*y);
    *x = tmpx;
    *y = tmpy;
}

void mandelbrot_set (double *pixels, int npls, int *mset) {
    int i;
    for (i=0; i<npls; i++) {
        int iter = 0;
        double cx = pixels[i*2];
        double cy = pixels[i*2+ 1];
        double x = 0.0f;   /* starting from zero */
        double y = 0.0f;
        // printf("The point is: (%.2f, %.2f)\n", cx, cy);
        
        /* compute Z_{n+1} = Z_n^2 + c */
        while (iter < maxiter) {
            complex_sq(&x, &y);
            // printf("x,y before adding cx cy: (%.2f, %.2f)\n", x, y);
            x += cx;
            y += cy;
            // printf("At iteration: %d, (x,y) = (%f, %f)\n", iter, x, y);
            
            double m = modulus_sq(x, y);
            // printf("At iteration: %d, modulus squared: %f\n", iter, m);
            if (m > threshold) break;
            iter++;
        }
        // printf("iter: %d\n", iter);

        /* set value of this pixel in the mandel_set */
        mset[i] = (iter==maxiter)? -1: iter;
    }

}

 

void write_output(int n, int* array, char* filename) {
  FILE *fp = fopen(filename, "w");
  int i;
  for (i=0; i<n*n; i++) {
      fprintf(fp, "%d ", array[i]);
      if ((i+1)%n == 0) fprintf(fp, "\n");
  }
  fclose(fp);
}




