Compact disk packing generator

## Clone and build project
```bash
$ sudo apt update
$ sudo apt upgrade
$ sudo apt install libboost-all-dev
$ git clone git@github.com:maxcalincu/diskpackings.git
$ cd diskpackings
$ chmod +x build.sh
$ ./build.sh  # should be executed from the root directory
```

## Visualizer
This is a tool used to generated compact disk packings with set of radii from a given region. To use it run these commands from build directory:
```bash
$ make visualizer
$ ./visualization-tool/visualizer --help>
```
This will display a detailed description. In order to produce an image with the generated packing use visualization-tool/notebook.ipynb