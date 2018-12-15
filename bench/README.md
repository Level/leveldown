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

Run the benchmark:

```
node bench/write-random.js
```

This writes results to `bench/write-random.csv` by default. To save results from multiple runs, add `--out <filename>`. For example, you could switch branches (assuming that both branches have the same benchmark code):

```
node bench/write-random.js --out master.csv

git checkout wip
node-gyp rebuild
node bench/write-random.js --out wip.csv
```

Then plot both runs with `write-random-plot.sh <file, ..>` (the basenames of the files are used for the legend):

```
./bench/write-random-plot.sh *.csv
```

If you are on Windows, you can use git bash:

```
bash bench/write-random-plot.sh master.csv wip.csv
```

## `write-sorted`

Run the benchmark:

```
node bench/write-sorted.js
```

This writes results to `bench/write-sorted.csv` by default. To save results from multiple runs, add `--out <filename>`. For example, you could switch branches (assuming that both branches have the same benchmark code):

```
node bench/write-sorted.js --out master.csv

git checkout wip
node-gyp rebuild
node bench/write-sorted.js --out wip.csv
```

Then plot both runs with `write-sorted-plot.sh <file, ..>` (the basenames of the files are used for the legend):

```
./bench/write-sorted-plot.sh *.csv
```

If you are on Windows, you can use git bash:

```
bash bench/write-sorted-plot.sh master.csv wip.csv
```

## `memory`
