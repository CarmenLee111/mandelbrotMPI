#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

/* gloabl variables */
int size, rank;
int i, j, k;
const double threshold = 4.0f;         /* threshold if converges compare w/ modulus**2 */
const int    npls = 2048;              /* resolution of the image */
int maxiter;                           /* max number of iteration */


double modulus_sq(double x, double y);
void complex_sq(double *x, double *y);
void mandelbrot_set (double *pixels, int npls, int *mset);
void write_output(int m, int n, int* array, char* filename);
void print_matrix(int* A, int m, int n);

int main(int argc, char *argv[]) {
    double xmin, ymin, xmax, ymax;        /* domain for the computation */
    double dx, dy;                        /* spatial step */
    int npls_y;                           /* row-major partition, amount of strips */
    double *pixels;                       /* for storing local complex numbers */
    int *m;                               /* for storing local mset */
    int *mset;                            /* for storing mandel set */

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

    // printf("xmin, xmax, ymin, ymax: (%f, %f, %f, %f) \n", xmin, xmax, ymin, ymax);

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	if (log2((float)size) != (int) log2((float)size)) {
		if (rank==0) {	
			printf("Number of processors need to be powers of 2\n");
			return -1;
		}
	}

    /* memloc in master for the final mandel set */
    if (rank==0) {
        mset = (int *) malloc(npls * npls * sizeof(int));
    }

    /* creating local row-major strips, stride=number of processors */
    dx = (xmax - xmin) / (double)(npls-1);
    dy = (ymax - ymin) / (double)(npls-1);
    npls_y = npls/size;
    pixels = (double *) malloc(npls*npls_y*2 * sizeof(double));
    for (j=0; j<npls_y; j++) {
        for (i=0; i<npls; i++) {
            pixels[j*npls*2+i*2]   = xmin + dx * i;
            pixels[j*npls*2+i*2+1] = ymin + dy * (j*size+rank); 
        }
    }

	/* local mset for storing results */
    m = (int *) malloc(npls*npls_y*sizeof(int));

    /* computing the mandelbrot set */
    mandelbrot_set(pixels, npls*npls_y, m);
	//print_matrix(m, npls_y, npls);

    /* preparing counts and displs for MPI_Gatherv */
    int counts[size];
    int displs[size];
    for (i=0; i<size; i++) {
        counts[i] = npls;
        displs[i] = npls*i;
    }

    /* gather results strip by strip */
    for (i=0; i<npls_y; i++) {
        MPI_Gatherv(&m[i*npls], npls, MPI_INT, &mset[i*npls*size], counts, displs, MPI_INT, 0, MPI_COMM_WORLD);
    }
	
	/* write output to file */
	if (rank==0){
    	write_output(npls_y, npls, mset, outputfile);
	//    print_matrix(mset, npls, npls); 
	}
	
	/* free memory allocation */
    free(pixels);
    free(m);
    if (rank==0) free(mset);

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

void mandelbrot_set (double *pixels, int npls, int *m) {
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
        m[i] = (iter==maxiter)? -1: iter;
    }

}

 

void write_output(int m, int n, int* array, char* filename) {
  FILE *fp = fopen(filename, "w");
  int i;
  for (i=0; i<m*n; i++) {
      fprintf(fp, "%d ", array[i]);
      if ((i+1)%n == 0) fprintf(fp, "\n");
  }
  fclose(fp);
}

void print_matrix(int* A, int m, int n){
    int i;
    for (i=0; i<n*m; i++) {
        printf("%d ", A[i]);
        if ((i+1)%n == 0) printf("\n");
    }
    printf("\n");
}





