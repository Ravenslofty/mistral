#!/bin/sh

cd ../docs
rm -rf ../docs-html ../docs-pdf
mkdir ../docs-html ../docs-pdf

sphinx-build -Eb singlehtml . ../docs-html
sphinx-build -Eb latex . ../docs-pdf
make -C ../docs-pdf
