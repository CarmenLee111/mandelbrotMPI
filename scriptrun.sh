# make clean
# make

cmap='hot'

domain='-2 1 -1.5 1.5'
f=outputall
mpirun -n 1 ./mandelbrot $domain 50 $f
python3 plot.py $domain $f $cmap
open $f.pdf


# domain='-0.749 -0.750 -0.0605 -0.0615'
# f=output1
# # mpirun -n 1 ./mandelbrot $domain 1024 $f
# python3 plot.py $domain $f $cmap
# open $f.pdf
        
