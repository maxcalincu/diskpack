Repository to generate compact disk packings

## Clone and build project
```
$ git clone git@github.com:maxcalincu/diskpackings.git
$ cd diskpackings
$ mkdir build && cd build
$ cmake ../compute/
$ cmake --build .
```
## Run packing generator
From build directory run
```
./main <packing-number> <packing-radius>
```
To modify the behaiviour edit compute/main.cpp (should be straightforward)

## Visualize the results
Once a packing has been generated run the cells of visualize/notebook.ipynb to produce an image