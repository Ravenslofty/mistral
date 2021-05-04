#!/bin/sh

ml1="5CEBA4F23C7 5CGTFD5C5F23I7 5CGTFD7C5F23I7 5CGTFD9C5F23I7 5CGXFC3B6F23C6 5CSEBA4U23I7 5CSEBA6U23I7"
ml2="e50f gt75f gt150f gt300f gx25f sx50f sx120f"

for f in `seq 1 7`; do
  mm1=`echo $ml1 | awk '{print $'$f'}'`
  mm2=`echo $ml2 | awk '{print $'$f'}'`
  echo "  " $mm2
  ../lib/mistral-cv p2r  $mm1 > srcdoc/$mm2-p2r.txt
  ../lib/mistral-cv p2ri $mm1 > srcdoc/$mm2-p2ri.txt
  ../lib/mistral-cv p2p  $mm1 > srcdoc/$mm2-p2p.txt
done
