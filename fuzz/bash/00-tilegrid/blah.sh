#!/usr/bin/env bash

mkdir -p out
rm "$1-todo.txt"
for n in N0 N3 N6 N9 N12 N15 N18 N21 N24 N27 N30 N33 N36 N39 N42 N45 N48 N51 N54 N57; do
    for lut in "FFFFFFFFFFFFFFFF" "FFFFFFFF00000000" "FFFF0000FFFF0000" "FF00FF00FF00FF00" "F0F0F0F0F0F0F0F0" "CCCCCCCCCCCCCCCC" "5555555555555555" "AAAAAAAAAAAAAAAA"; do
        echo "$1_$n $lut" >> "$1-todo.txt"
    done
done

xargs -P 6 -L 1 -a "$1-todo.txt" -t -- bash worker.sh 
