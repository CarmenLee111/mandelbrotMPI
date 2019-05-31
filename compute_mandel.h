/*******************************************************************
 * This header file contains variables and functions               *
 *          for the computation of the Mandelbrot set              *
 *******************************************************************/




/*********************************************************************************
 * Global variables pertaining to the computation of Mandelbrot set              *
 *********************************************************************************/
const double threshold = 4.0f;       /* threshold if converges compare w/ modulus**2 */
int          npls;                   /* resolution of the image */
int          maxiter;                /* max number of iteration */
double       xmin, ymin, radius;       /* domain for the computation */
double       dx, dy, *pixels;
int          *m, *mset; 


double modulus_sq(double x, double y);
void complex_sq(double *x, double *y);
void mandelbrot_set (double *pixels, int npls, int *m);


/*********************************************************************************
 * |Z|^2, square of the modulus of complex number Z.                             *
 * args:                                                                         *
 *      x - real(Z)                                                              *
 *      y - img(Z)                                                               *
 *********************************************************************************/
double modulus_sq(double x, double y){
    return x*x + y*y;
}


/*********************************************************************************
 * Z^2, square of complex number Z.                                              *
 * args:                                                                         *
 *      x - real(Z)                                                              *
 *      y - img(Z)                                                               *
 *********************************************************************************/
void complex_sq(double *x, double *y){
    double tmpx = (*x)*(*x) - (*y)*(*y);
    double tmpy = 2*(*x)*(*y);
    *x = tmpx;
    *y = tmpy;
}


/*********************************************************************************
 * Compute the Mandelbrot set                                                    *
 * args:                                                                         *
 *      pixels - stores the complex number C to be used in Zn+1 = Zn^2+C         *
 *      npls   - number of rows/columns in pixels                                *
 *      m      - stores the Mandelbrot set                                       *
 *********************************************************************************/
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
            
            double md = modulus_sq(x, y);
            // printf("At iteration: %d, modulus squared: %f\n", iter, m);
            if (md > threshold) break;
            iter++;
        }
        // printf("iter: %d\n", iter);

        /* set value of this pixel in the mandel_set */
        m[i] = (iter==maxiter)? -1: iter;
    }
}
