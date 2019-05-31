
#include <mpi.h>
#include "compute_mandel.h"
#include "utility.h"


int main(int argc, char *argv[]) {
    int npls_y;                           /* row-major partition, amount of strips */

    if (argc < 6) {
        printf("Usage: ./mandelequal <xcenter> <ycenter> <radius> <resolution> <maxiter>");
        return -1;
    }

    /* store the arguments */
    xmin = atof(argv[1]) - atof(argv[3]); 
    ymin = atof(argv[2]) - atof(argv[3]);
    radius = atof(argv[3]);
    npls = atof(argv[4]);
    maxiter = atoi(argv[5]);
    char *outputfile = argv[6];
    assert(radius>0);


    MPI_Init(&argc, &argv);                   /* setting up MPI */
	starttime = MPI_Wtime();

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	

    /* restriction on the number of processors */
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
    dx = 2.0f * radius / (double)(npls-1);
    dy = 2.0f * radius / (double)(npls-1);
    npls_y = npls/size;
    pixels = (double *) malloc(npls*npls_y*2 * sizeof(double));
    for (j=0; j<npls_y; j++) {
        for (i=0; i<npls; i++) {
            pixels[j*npls*2+i*2]   = xmin + dx * (double) i;
            pixels[j*npls*2+i*2+1] = ymin + dy * (double) (j*size+rank); 
        }
    }

	/* local mset for storing results */
    m = (int *) malloc(npls*npls_y*sizeof(int));

    /* computing the mandelbrot set */
    mandelbrot_set(pixels, npls*npls_y, m);
	
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
        t = MPI_Wtime() - starttime;
        printf("Wall time: %lf\n", t);
    	write_output(npls, npls, mset, outputfile);
	}
	
	/* free memory allocation */
    free(pixels);
    free(m);
    if (rank==0) free(mset);

    MPI_Finalize();

    return 0;
}






