#!/bin/sh

./mux_to_source.py enum ../data > ../lib/cv-bmuxtypes.ipp
./mux_to_source.py mux  ../data > ../lib/cv-bmux-data.cc

