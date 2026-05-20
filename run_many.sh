#!/usr/bin/env bash

N_RUNS=20

echo "#run seed diff" > results/diffs.txt
for ((i=1; i<=N_RUNS; i++)); do
    seed=$i
    out=$(./simulate "$seed" | grep "MC_DIFF" | awk '{print $2}')
    echo "$i $seed $out" >> results/diffs.txt
done
