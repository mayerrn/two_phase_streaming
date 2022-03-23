# Two-Phase Partitioner

The implementation of Two-Phase Streaming, an out-of-core edge partitioner. The work is published at IEEE ICDE 2022:

Ruben Mayer, Kamil Orujzade, and Hans-Arno Jacobsen. “Out-of-Core Edge Partitioning at Linear Run-Time”. In Proceedings of the 2022 IEEE 38th International Conference on Data Engineering (ICDE ‘22). 14 pages.

In this repo, we also implemented the below methods and a tool in C++.
* High Degrees Replicated First (HDRF) - Based on the following [publication](http://midlab.diag.uniroma1.it/articoli/PQDKI15CIKM.pdf): F. Petroni, L. Querzoni, G. Iacoboni, K. Daudjee and S. Kamali: "Hdrf: Efficient stream-based partitioning for power-law graphs". CIKM, 2015.
* Degree-Based Hashing (DBH) - Based on the following [publication](http://papers.nips.cc/paper/5396-distributed-power-law-graph-computing-theoretical-and-empirical-analysis.pdf): C. Xie, L. Yan, W. Li and Z. Zhang: "Distributed Power-law Graph Computing:
Theoretical and Empirical Analysis". NIPS, 2014


## Installation

We tested the programs (``twophasepartitioner``, ``dbh``) on Ubuntu 20.04 LTS and Debian 10.0.

The programs require the below packages: `g++`, `cmake`, `glog`, `gflags`, `boost`:
```
sudo apt-get install libgoogle-glog-dev libgflags-dev libboost-all-dev cmake g++
```

## Build Programs
```
cd twophasepartitioner
mkdir build && cd build
cmake ..
make
```
### Optimized Compilation

2PS and HDRF methods store the states of the vertices in a vertex-to-partition table. To optimize memory usage, we can define the number of partitions when the programs are built and allocate only the required amount of memory. The above build process will allocate the memory for 256 partitions. While it is still okay to use the fewer number of partitions when running `twophasepartitioner`, you will see the below warning message due to using memory inefficient:
```
#warning MAX NUMBER OF PARTITIONS is defined to 256.
```

To avoid this warning, use the below build step instead of just `make `:
```
make CXX_FLAGS="-Wall -std=c++14 -mcx16 -DSTATS -O3 -DNDEBUG -DMAX_NUM_PARTITION=<THE NUMBER OF PARTITIONS>"
```
Please change `<THE NUMBER OF PARTITIONS>` with the number of partitions you need.

Note: You need to add all the required CXX_FLAGS since they override the existing flags in CMakeLists.txt

Note: `-DSTATS` flag helps to print the replication factor and balancing value out to the console for DBH partitioner. However, printing out stats increases run-time and memory usage. Therefore, please remove this flag if you benchmark run-time and memory usage of DBH.

## Usage

### Two Phase partitioners and HDRF

Parameters:
* `filename`: path to the edge list file.
* `p`: the number of partitions that the graph will be divided into.
* `prepartitioner_type`: name of the clustering algorithm as a pre-partitioner. default: streamcom.

Optional:
* `use_hdrf`: use HDRF scoring instead of linear scoring for the second phase
* `lambda`: lambda value for HDRF. default: 1.
* `shuffle`: shuffle the dataset. default: `false`.
* `memsize`: memory size in MB. memsize is used as a chunk size while shuffling data and batch size while partitioning edges. default: 100
* `balance_ratio`: imbalance factor for ensuring that the largest partition does not exceed the expected ratio. default: 1.05
* `print_coms`: print clusters (communities) to a file in ascending order of vertex ids. default: false.
    * Please note that when you enable this flag, the program will stop after completing the clustering step.
* `coms_filename`: the path to the output file to store the clusters in the ascending order of the vertex ids. default: "".
* `str_iters`: the number of iterations to run streamcom clustering in the first phase: default: 1.

2PS-L Examples:
```
./twophasepartitioner -filename ../sample/input.txt -p 4 -prepartitioner_type streamcom

./twophasepartitioner -filename ../sample/input.txt -p 4 -shuffle false -memsize 100 -prepartitioner_type streamcom -balance_ratio 1.05 -str_iters 2 
```

2PS-HDRF Example:
```
./twophasepartitioner -filename ../sample/input.txt -p 4 -prepartitioner_type streamcom -use_hdrf

```

HDRF example:
```
./twophasepartitioner -filename ../sample/input.txt -p 4 -use_hdrf -lambda 1.1
```

### DBH

Parameters:
* `filename`: path to the edge list file.
* `p`: the number of partitions that the graph will be divided into.

Optional:
* `memsize`: memory size in MB. Memsize is used as a chunk size while shuffling and batch size while partitioning. default: 100

DBH example:
```
./dbh -filename ../sample/input.txt -p 4
```

Note: `twophasepartitioner` and `dbh` programs  uses converter library to convert a given input file to the binary format to increase the efficiency of read-time and rename the vertex ids to ascending order of numerical values starting from 0. You can find the converted file in the same directory as the original input file. The extension of the binary file is .binedgelist.



### Further Notes w.r.t Implementations 

* We used the below repository as a reference when implementing HDRF and DBH in C++. The reposity is owned by the author(s) of HDRF paper.
    * Fabio Petroni, https://github.com/fabiopetroni/VGP
*  The implementation for converting input file to binary edge list (converter/*) is modification of the respective implementation in the below reposity. The reposity is owned by the author(s) of Neighborhood Expansion paper. 
    * ANSR Lab, [https://github.com/ansrlab/edgepart](https://github.com/ansrlab/edgepart)
 

### Data sets used in the paper

Here are links to the data sets that we used in the paper:
* OK: https://snap.stanford.edu/data/com-Orkut.html
* WI: http://konect.cc/networks/wikipedia_link_en/
* IT: http://law.di.unimi.it/webdata/it-2004/
* TW: https://snap.stanford.edu/data/twitter-2010.html
* FR: https://snap.stanford.edu/data/com-Friendster.html
* UK: http://law.di.unimi.it/webdata/uk-2007-05/
* GSH: http://law.di.unimi.it/webdata/gsh-2015/
* WDC: http://webdatacommons.org/hyperlinkgraph/2014-04/download.html

