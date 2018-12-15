#!/bin/sh

num_writes=10M
num_files=$#
platform=$(node -p process.platform)
arch=$(node -p process.arch)
node=$(node -p process.version)
png_suffix=$(date '+%Y%m%d-%H.%M.%S')

gnuplot <<EOF
  reset
  set terminal pngcairo truecolor enhanced font "Ubuntu Mono,13" size 1920, 1080 background rgb "#1b1b1b"
  set output "bench/write-sorted-$png_suffix.png"
  set datafile separator ','

  set autoscale y

  set xlabel "Time (seconds)" tc rgb "#777777"
  set ylabel "Milliseconds/write" tc rgb "#888888"

  set key outside tc rgb "#777777"
  set border lc rgb "#777777"
  set grid

  # To plot more than 5 files, add more line styles
  set style line 1 lt 7 ps 0.8 lc rgb "#00FFFF"
  set style line 2 lt 7 ps 0.8 lc rgb "#D84797"
  set style line 3 lt 7 ps 0.8 lc rgb "#23CE6B"
  set style line 4 lt 7 ps 0.8 lc rgb "#F5B700"
  set style line 5 lt 7 ps 0.8 lc rgb "#731DD8"

  set title "leveldown write-sorted, $num_writes writes, $platform $arch, node $node" tc rgb "#cccccc"

  filename(n) = word("$@", n)
  basename(n) = substr(word("$@", n), 0, strstrt(word("$@", n), ".csv") - 1)

  plot for [i=1:$num_files] filename(i) using (\$1/1000):(\$2/1000000) title basename(i) ls i axes x1y1

EOF
