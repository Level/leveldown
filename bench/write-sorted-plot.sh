#!/bin/sh

gnuplot <<EOF
  reset
  set terminal png size 1600, 1000
  set output "write_sorted_times.png"
  set datafile separator ','

  set yrange [0:0.6]

  set xlabel "Seconds"
  set ylabel "Milliseconds per write"

  set title "1.3G / 10,000,000 writes"
  set key below
  set grid

  plot "write_sorted_times.csv" using (\$1/1000):(\$2/1000000)
EOF