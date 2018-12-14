# Benchmarks

## `db-bench`

Run the benchmark:

```
node bench/db-bench.js
```

This writes results to `bench/db-bench.csv` by default. To save results from multiple runs, add `--out <filename>`. For example, you could switch branches (assuming that both branches have the same benchmark code):

```
node bench/db-bench.js --out master.csv

git checkout wip
node-gyp rebuild
node bench/db-bench.js --out wip.csv
```

Then plot both runs with `db-bench-plot.sh <file, ..>` (the basenames of the files are used for the legend):

```
./bench/db-bench-plot.sh *.csv
```

If you are on Windows, you can use git bash:

```
bash bench/db-bench-plot.sh master.csv wip.csv
```

## `write-random`
## `write-sorted`
## `memory`
