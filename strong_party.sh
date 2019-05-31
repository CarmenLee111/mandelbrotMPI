#!/bin/bash -l

#SBATCH -A g2019010
#SBATCH --mail-type=END,FAIL
#SBATCH --mail-user=carmen.jing.lee@gmail.com
#SBATCH -p node -N 1 -n 1
#SBATCH -t 10:00

module load gcc openmpi

make clean
make all

c=1
write2file=0

center='0.2937117 -0.01973392'   # ass crack party
radius=0.1
f1=output1_s 
f2=output1_e 
f3=output1_m
r=4096
i=5000
echo Resolution $r \tMax iteration: $i \tCenter $center \tRadius $radius
for i in 1 2 3
do 
    echo serial    ; ./mandelserial $center $radius $r $i
    # echo equal  $c ;  mpirun --oversubscribe -n $c ./mandelequal $center $radius $r $i
    # echo master $c ; mpirun --oversubscribe -n $c ./mandelmaster $center $radius $r $i
done
