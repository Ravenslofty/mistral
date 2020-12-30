#!/bin/sh

for b in cbuf dll dqs16 dsp fpll gpio hip hmc hps_clocks hssi lab lvl m10k mlab opt serpar term; do
  ./mkmuxdoc.py ../data/${b}-mux.txt srcdoc/${b}-cfg.txt srcdoc/${b}-doc.txt > gendoc/${b}-dmux.rst
done

b=cmuxvr; ./mkmuxdoc.py ../data/cmuxv-mux.txt srcdoc/${b}-cfg.txt srcdoc/cmux-doc.txt > gendoc/${b}-dmux.rst
b=cmuxvg; ./mkmuxdoc.py ../data/cmuxv-mux.txt srcdoc/${b}-cfg.txt srcdoc/cmux-doc.txt > gendoc/${b}-dmux.rst
b=cmuxhr; ./mkmuxdoc.py ../data/cmuxh-mux.txt srcdoc/${b}-cfg.txt srcdoc/cmux-doc.txt > gendoc/${b}-dmux.rst
b=cmuxhg; ./mkmuxdoc.py ../data/cmuxh-mux.txt srcdoc/${b}-cfg.txt srcdoc/cmux-doc.txt > gendoc/${b}-dmux.rst
b=cmuxc;  ./mkmuxdoc.py ../data/cmuxc-mux.txt srcdoc/${b}-cfg.txt srcdoc/cmux-doc.txt > gendoc/${b}-dmux.rst

b=pma3; ./mkmuxdoc.py ../data/${b}-mux.txt,../data/${b}-c-mux.txt srcdoc/${b}-cfg.txt srcdoc/${b}-doc.txt > gendoc/${b}-dmux.rst

cd ../docs
rm -rf ../docs-html ../docs-pdf
mkdir ../docs-html ../docs-pdf

sphinx-build -Eb singlehtml . ../docs-html
sphinx-build -Eb latex . ../docs-pdf
make -C ../docs-pdf
