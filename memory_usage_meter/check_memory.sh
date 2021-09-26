#!/bin/bash
rm massif.out
echo "Measuring memory usage"
valgrind --tool=massif --pages-as-heap=yes --massif-out-file=massif.out ../cmake-build-debug/openseekmap ../data/belgie-ascii.data < ../data/input.txt
echo "Opening massif-visualizer"
massif-visualizer massif.out