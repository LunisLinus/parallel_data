#!/bin/bash

mpic++ task1.cpp -O3 -o task1 -lm

for N in 1000 1500 2000
do
    for MODE in default static dynamic
    do
        for P in 1 2 4 8
        do
            echo "--------------------------------"
            echo "N=$N MODE=$MODE P=$P"

            if [ "$MODE" = "dynamic" ]; then
                mpirun -np $P ./task1 $N $MODE 4
            else
                mpirun -np $P ./task1 $N $MODE
            fi
        done
    done
done