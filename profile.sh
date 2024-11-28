#!/bin/bash

# Install required tools
apt-get update
apt-get install -y valgrind graphviz python3-pip
pip3 install gprof2dot

# Build with debug symbols
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Run valgrind with callgrind
valgrind --tool=callgrind \
         --callgrind-out-file=raytracer.callgrind \
         --collect-systime=yes \
         --collect-jumps=yes \
         ./raytracer ../scenes/two-spheres-on-plane.json

# Generate call graph using gprof2dot and graphviz
gprof2dot -f callgrind raytracer.callgrind | dot -Tpng -o profile.png

# Generate a text-based summary
echo "=== Profiling Summary ===" > profile_summary.txt
echo "" >> profile_summary.txt

echo "Top 10 functions by self time:" >> profile_summary.txt
callgrind_annotate raytracer.callgrind --threshold=100 --inclusive=no | head -n 20 >> profile_summary.txt

echo "" >> profile_summary.txt
echo "Call tree (most expensive paths):" >> profile_summary.txt
callgrind_annotate raytracer.callgrind --tree=both --threshold=5 >> profile_summary.txt

# Print the summary to console
cat profile_summary.txt