#!/bin/sh

for m in e50f gt75f gt150f gt300f gx25f sx50f sx120f; do
  printf "%-8s" $m
  ./routes-to-bin.py ../data ../lib/cv-rnodetypes.ipp $m ../gdata
done

