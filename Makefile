cc = gcc
CFLAGS = -O3 -funroll-loops -march=native -ffast-math -std=c99
LIBS = -lm

BINS = mandelserial mandelequal mandelmaster mandelmaster2 mandelmaster3

all: $(BINS)

mandelserial: main_serial.c compute_mandel.h utility.h
	$(cc) $(CFLAGS) -o $@ $< $(LIBS)

mandelequal: main_equal.c compute_mandel.h utility.h
	mpicc $(CFLAGS) -o $@ $< $(LIBS)

mandelmaster: main_master.c compute_mandel.h utility.h
	mpicc $(CFLAGS) -o $@ $< $(LIBS)

mandelmaster2: main_master2.c compute_mandel.h utility.h
	mpicc $(CFLAGS) -o $@ $< $(LIBS)

mandelmaster3: main_master3.c compute_mandel.h utility.h
	mpicc $(CFLAGS) -o $@ $< $(LIBS)

clean:
	rm $(BINS) $(OBJS)