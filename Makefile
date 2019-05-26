cc = mpicc
CFLAGS = -O3 -funroll-loops -march=native -ffast-math
LIBS = -lm

BINS = mandelbrot

all: $(BINS)

mandelbrot: main.c
	mpicc $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm $(BINS)