make clean
make all

c=4
cmap='hot'


#center='-0.7437098 -0.107842'   # horse valley
center='0.2937117 -0.01973392'   # crack
radius=0.1
# f1=output1_s 
f2=output1_e 
f3=output1_m
r=40
i=50
# echo serial    $center $radius; ./mandelserial $center $radius $r $i $f1
echo equal  $c $center $radius;  mpirun --oversubscribe -n $c ./mandelequal $center $radius $r $i $f2
# echo master $c $center $radius; mpirun --oversubscribe -n $c ./mandelmaster $center $radius $r $i $f3
# python3 plot.py $center $radius $f1 $cmap
# python3 plot.py $center $radius $f2 $cmap
# python3 plot.py $center $radius $f3 $cmap
# open $f1.pdf
# open $f2.pdf
# open $f3.pdf





        
