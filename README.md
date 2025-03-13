Repository to generate compact disk packings

## Clone and build project
```bash
$ sudo apt update
$ sudo apt upgrade
$ sudo apt install libboost-all-dev
$ git clone git@github.com:maxcalincu/diskpackings.git
$ cd diskpackings
$ mkdir build && cd build
$ cmake ../compute/
$ cmake --build .
```
## Run packing generator
From build directory run
```bash
$ ./main <packing-number> <packing-radius>
```
To modify the behaiviour edit compute/main.cpp (should be straightforward)

## Visualize the results
Once a packing has been generated run the cells of visualize/notebook.ipynb to produce an image