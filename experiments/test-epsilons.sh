#!/bin/bash -ex

n=1000000
epsilons="0.69 0.68 0.67 0.66 0.65 0.64 0.63 0.62 0.61 0.6 0.59 0.58 0.57 0.56 0.55 0.54 0.53 0.52 0.51 0.5 0.49 0.48 0.47 0.46 0.45 0.44 0.43 0.42 0.41 0.4 0.39 0.38 0.37 0.36 0.35 0.34 0.33 0.32 0.31 0.3 0.29 0.28 0.27 0.26 0.25 0.24 0.23 0.22 0.21 0.2 0.19 0.18 0.17 0.16 0.15 0.14 0.13 0.12 0.11 0.1 0.09 0.08 0.07 0.06 0.05 0.04 0.03 0.02"

for eps in $epsilons; do
    echo epsilon=$eps
    ../cpp-bin/main -eps=$eps -$n -todolist2 > tmp.dat
    echo `grep 'ADD' tmp.dat` $eps >> epsilon-add.dat
    echo `grep 'FIND' tmp.dat` $eps >> epsilon-find.dat
done
gnuplot epsilon.gp
