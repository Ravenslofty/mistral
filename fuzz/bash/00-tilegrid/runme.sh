#!/bin/bash

# LAB columns
# TODO: discover from quartus.
X_LEFT="X1 X3 X4 X5 X6 X7 X9 X10 X12 X13 X14 X16 X17 X18 X19 X20 X21 X23 X24 X25 X26"
X="$X_LEFT 28 X29 X31 X32 X34 X35 X36 X37 X39 X40 X41 X42 X43 X44 X45 X47 X48 X49 X50 X52 X53"
Y="Y1 Y2 Y3 Y4 Y5 Y6 Y7 Y8 Y9 Y10 Y11"
Y="$Y Y12 Y13 Y14 Y15 Y16 Y17 Y18 Y19 Y20 Y21 Y22"
Y="$Y Y23 Y24 Y25 Y26 Y27 Y28 Y29 Y30 Y31 Y32 Y33"
Y="$Y Y34 Y35 Y36 Y37 Y38 Y39 Y40 Y41 Y42 Y43 Y44"
N="N0 N3 N6 N9 N12 N15 N18 N21 N24 N27 N30 N33 N36 N39 N42 N45 N48 N51 N54 N57"

rm labs.txt

echo "building LAB table..."
for x in $X; do
    for y in "Y1"; do
        for n in $N; do
            echo "LABCELL_$x\_$y\_$n" >> labs.txt
        done
    done
done

echo "now synthesizing:"
xargs -t -P "$(nproc)" -n 1 -a labs.txt -- ./worker.sh
