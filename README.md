Compact disk packing generator

## Clone and build project
```bash
$ sudo apt update
$ sudo apt upgrade
$ sudo apt install libboost-all-dev
$ git clone git@github.com:maxcalincu/diskpackings.git
$ cd diskpackings
$ mkdir build && cd build
$ cmake ..
```

## Visualizer
From build directory run
```bash
$ make visualizer
$ ./visualization-tool/visualizer <packing-number> <packing-radius>
```
To modify the behaiviour edit visualization-tool/src/main.cpp (should be straightforward)

## Python notebook
To produce an image with the generated packing use visualization-tool/notebook.ipynb