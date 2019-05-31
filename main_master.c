
#include <mpi.h>
#include "compute_mandel.h"
#include "utility.h"

static inline void master(int* mset, int yi, int npls, int size, int WORKTAG, int DIETAG,
                          MPI_Status status, MPI_Datatype arrtype, char* outputfile);
static inline void slave(int DIETAG, int yi, int npls, int *m, double xmin, double ymin, double dx, double dy,
                         double *pixels, MPI_Status status, MPI_Datatype arrtype);


int main(int argc, char *argv[]) {

    int WORKTAG=1, DIETAG=2;
    int yi;

    if (argc < 6) {
        if (rank==0) {
            printf("Usage: ./mandelmaster <xcenter> <ycenter> <radius> <resolution> <maxiter> <outputfile>");
            return -1;
        }
    }

    /* store the arguments */
    xmin = atof(argv[1]) - atof(argv[3]); 
    ymin = atof(argv[2]) - atof(argv[3]);
    radius = atof(argv[3]);
    npls = atoi(argv[4]);
    maxiter = atoi(argv[5]);
    char *outputfile = argv[6];

    assert(radius>0);

    MPI_Status status;                     /* status for infor on the communication */
    MPI_Datatype arrtype;                  /* derived datatype for data transmission */

    MPI_Init(&argc, &argv);                /* setting up MPI */
    starttime = MPI_Wtime();

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* create a MPI type that is an array of lenghth npls */
    MPI_Type_contiguous(npls, MPI_INT, &arrtype);
    MPI_Type_commit(&arrtype);

    /* restriction on the number of processors */
    if (size < 2) {
        if (rank==0) {
            printf("Must have at least 2 processors\n");
            return -1;
        }
    }
    if (size > npls) {
        if (rank==0) {
            printf("Number of processors exceeds batches of work\n");
            return -1;
        }
    }

    /* compute the step */
    dx = 2.0f * radius / (double)(npls-1);
    dy = 2.0f * radius / (double)(npls-1);
    assert(dx!=0);
    assert(dy!=0);

    /* Master slave model for dynamic loop splitting */
    if (rank==0) {
        master(mset, yi, npls, size, WORKTAG, DIETAG, status, arrtype, outputfile);
    } else {
        slave(DIETAG, yi, npls, m, xmin, ymin, dx, dy, pixels, status, arrtype);
    }
	
    MPI_Finalize();

    return 0;
}


/**************************************************************************************************************
 * Master processor handing out work                                                                          *
 **************************************************************************************************************/

static inline void master(int* mset, int yi, int npls, int size, int WORKTAG, int DIETAG,
                          MPI_Status status, MPI_Datatype arrtype, char* outputfile) {

    /* memloc in master for the final mandel set */
    mset = (int *) malloc(npls * npls * sizeof(int));

    /* initial work allocation to each processor */
    for (int rank=1; rank<size; rank++) {
        MPI_Send(&yi, 1, MPI_INT, rank, WORKTAG, MPI_COMM_WORLD);
        yi++;
    }

    /* as long as there's work, master checks for completion and assgins work to the same slave*/
    while (yi<npls) {
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        MPI_Recv(&mset[npls*status.MPI_TAG], 1, arrtype, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);
        MPI_Send(&yi, 1, MPI_INT, status.MPI_SOURCE, WORKTAG, MPI_COMM_WORLD);
        yi++;
    }

    /* collect the rest of the work done*/
    for (int rank=1; rank<size; rank++) {
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        MPI_Recv(&mset[npls*status.MPI_TAG], 1, arrtype, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);
    }

    /* tell slaves to die. You guys are done now. */
    for (int rank=1; rank<size; rank++) {
        MPI_Send(0, 0, MPI_INT, rank, DIETAG, MPI_COMM_WORLD);
        // printf("Sends save dietage: %d\n", DIETAG);
    }

    /* print time and write output */
    t = MPI_Wtime() - starttime;
    printf("Wall time: %lf\n", t);
    write_output(npls, npls, mset, outputfile); 

    /* free memory */
    free(mset);

}

/**************************************************************************************************************
 * Slave processors working non-stop until master sends a msg telling them to kill themselves                 *
 **************************************************************************************************************/
static inline void slave(int DIETAG, int yi, int npls, int *m, double xmin, double ymin, double dx, double dy,
                         double *pixels, MPI_Status status, MPI_Datatype arrtype) {

    /* for storing the complex numbers and mandelbrot set values in the domain */
    pixels = (double *) malloc(npls*2 * sizeof(double));
    m = (int * ) malloc (npls * sizeof(int));

    /* while dietag is not changed from 2, slaves carry on */
    while (DIETAG == 2) {

        /* receives msg regarding work from master */
        MPI_Recv(&yi, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        // printf("Status has tag: %d\n", status.MPI_TAG);

        /* DIETAG, it's time my friend */
        if (status.MPI_TAG == DIETAG) {
            DIETAG = 0;
            // printf("Dietag is now: %d", DIETAG);
            break;
        }

        /* the complex number for computation */
        double y = ymin + (double) yi * dy;
        for (int i=0; i<npls; i++) {
            pixels[i*2] = xmin + dx * (double) i;
            pixels[i*2+1] = y;
        }

        /* computation of the mandelbrot set */
        mandelbrot_set(pixels, npls, m);

        /* send back to master at completion */
        MPI_Send(m, 1, arrtype, 0, yi, MPI_COMM_WORLD);
    }

    /* free memory */
    free(pixels);
    free(m);

}






