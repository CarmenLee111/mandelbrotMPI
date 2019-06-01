
#include <mpi.h>
#include "compute_mandel.h"
#include "utility.h"


int main(int argc, char *argv[]) {
    int npls_y;                           /* row-major partition, amount of strips */
    int collect;                          /* number of times collecting equally from each PE */
    int pad;                              /* residual */

    if (argc < 6) {
        printf("Usage: ./mandelequal <xcenter> <ycenter> <radius> <resolution> <maxiter>\n");
        return -1;
    }

    /* store the arguments */
    xmin = atof(argv[1]) - atof(argv[3]); 
    ymin = atof(argv[2]) - atof(argv[3]);
    radius = atof(argv[3]);
    npls = atoi(argv[4]);
    maxiter = atoi(argv[5]);
    char *outputfile = argv[6];
    assert(radius>0);


    MPI_Init(&argc, &argv);                   /* setting up MPI */
	starttime = MPI_Wtime();

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* restriction on the number of processors */
    if (size > npls) {
        if (rank==0) {
            printf("Number of processors exceeds batches of work\n");
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
    pad = npls%size;
    collect = npls/size;
    if (rank==size-1) npls_y += pad;    /* the last PE takes the residual */
    pixels = (double *) malloc(npls*npls_y*2 * sizeof(double));
    for (j=0; j<collect; j++) {
        for (i=0; i<npls; i++) {
            pixels[j*npls*2+i*2]   = xmin + dx * (double) i;
            pixels[j*npls*2+i*2+1] = ymin + dy * (double) (j*size+rank); 
        }
    }
    /* account for padding at the last processor */
    if (pad != 0 && rank==size-1) {
        for (j=0; j<pad; j++) {
            for (i=0; i<npls; i++) {
                pixels[collect*npls*2 + j*npls*2+i*2]   = xmin + dx * (double) i;
                pixels[collect*npls*2 + j*npls*2+i*2+1] = ymin + dy * (double) j + dy*((double) collect*size);
            }
        }
    }

	/* local mset for storing results */
    m = (int *) malloc(npls*npls_y*sizeof(int));

    /* computing the mandelbrot set */
    mandelbrot_set(pixels, npls*npls_y, m);
    // print_matrix(m, npls_y, npls);
	
    /* preparing counts and displs for MPI_Gatherv */
    int counts[size];
    int displs[size];
    for (i=0; i<size; i++) {
        counts[i] = npls;
        displs[i] = npls*i;
    }

    /* gather results strip by strip */
    for (i=0; i<collect; i++) {
        MPI_Gatherv(&m[i*npls], npls, MPI_INT, &mset[i*npls*size], counts, displs, MPI_INT, 0, MPI_COMM_WORLD);
    }

    /* account for padding */
    if (pad != 0) {
        if (rank == size-1) {
            MPI_Send(&m[collect * npls], pad*npls, MPI_INT, 0, 11, MPI_COMM_WORLD);
        } 
        else if (rank==0) {
            MPI_Recv(&mset[collect*npls*size], pad*npls, MPI_INT, size-1, 11, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
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






