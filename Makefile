cc = gcc
CFLAGS = -O3 -funroll-loops -march=native -ffast-math
LIBS = -lm

BINS = mandelserial mandelequal mandelmaster

all: $(BINS)

mandelserial: main_serial.c compute_mandel.h utility.h
	$(cc) $(CFLAGS) -o $@ $< $(LIBS)

mandelequal: main_equal.c compute_mandel.h utility.h
	mpicc $(CFLAGS) -o $@ $< $(LIBS)

mandelmaster: main_master.c compute_mandel.h utility.h
	mpicc $(CFLAGS) -o $@ $< $(LIBS)

clean:
	rm $(BINS) $(OBJS)