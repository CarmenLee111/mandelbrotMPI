#!/bin/bash -l

#SBATCH -A g2019010
#SBATCH --mail-type=END,FAIL
#SBATCH --mail-user=carmen.jing.lee@gmail.com
#SBATCH -p node -N 1 -n 2
#SBATCH -t 10:00

module load gcc openmpi

make clean
make all

c=2

cmap='hot'
domain='-2 -1.5 3'
f1=outputall_s 
f2=outputall_e 
f3=outputall_m
r=2048
i=100
echo serial    $domain; ./mandelserial $domain $r $i $f1
echo equal  $c $domain;  mpirun -n 2 ./mandelequal $domain $r $i $f2
echo master $c $domain; mpirun -n 2 ./mandelmaster $domain $r $i $f3


domain='-0.7463 -0.1102 0.005'
f1=output1_s 
f2=output1_e 
f3=output1_m
r=2048
i=100
echo serial    $domain; ./mandelserial $domain $r $i $f1
echo equal  $c $domain;  mpirun -n 2 ./mandelequal $domain $r $i $f2
echo master $c $domain; mpirun -n 2 ./mandelmaster $domain $r $i $f3


domain='-0.0452407411 0.986816213 1.75E-7'
f1=output1_s 
f2=output1_e 
f3=output1_m
r=2048
i=100
# echo serial    $domain; ./mandelserial $domain $r $i $f1
# echo equal  $c $domain;  mpirun -n 2 ./mandelequal $domain $r $i $f2
# echo master $c $domain; mpirun -n 2 ./mandelmaster $domain $r $i $f3


