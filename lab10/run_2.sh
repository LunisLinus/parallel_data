#!/bin/bash

mpic++ task2.cpp -O3 -o task2 -lm

for N in 1000 1500 2000
do
    for MODE in default static dynamic
    do
        for P in 1 2 4 8
        do
            echo "--------------------------------"
            echo "N=$N MODE=$MODE P=$P"

            if [ "$MODE" = "dynamic" ]; then
                mpirun -np $P ./task2 $N $MODE 1
            else
                mpirun -np $P ./task2 $N $MODE
            fi
        done
    done
done