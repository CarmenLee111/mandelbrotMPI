
#include "compute_mandel.h"
#include "utility.h"


int main(int argc, char *argv[]) {
    
    if (argc < 6) {
        printf("Usage: ./mandelserial <xcenter> <ycenter> <radius> <resolution> <maxiter>");
        return -1;
    }

    /* store the arguments */
    xmin = atof(argv[1]) - atof(argv[3]); 
    ymin = atof(argv[2]) - atof(argv[3]);
    radius = atof(argv[3]);
    npls = atof(argv[4]);
    maxiter = atoi(argv[5]);
    outputfile = argv[6];
    assert(radius>0);

    starttime = get_wall_seconds();

    /* compute the step */
    dx = 2.0f * radius / (double)(npls-1);
    dy = 2.0f * radius / (double)(npls-1);

    /* collection of the complex numbers for the mandelbrot set */
    pixels = (double *) malloc(npls*npls*2 * sizeof(double));
    for (i=0; i<npls; i++) {
        for (j=0; j<npls; j++) {
            pixels[i*npls*2+j*2]   = xmin + dx * (double)j;
            pixels[i*npls*2+j*2+1] = ymin + dy * (double)i; 
        }
    }

    /* computing the mandelbrot set */
    mset = (int *) malloc(npls*npls*sizeof(int));
    mandelbrot_set(pixels, npls*npls, mset);

    t = get_wall_seconds() - starttime;
   
    /* print wall time and output */
    printf("Wall time: %lf\n", t);
    write_output(npls, npls, mset, outputfile);

    /* free allocated memory */
    free(pixels);
    free(mset);

    return 0;
}






