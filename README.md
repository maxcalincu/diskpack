This project provides several tools for working with compact disk packings.


``diskpack`` is written in C++17. Please make sure that appropriate versions of ``gcc`` and ``clang`` are installed on your device before proceeding.

## Clone and build project
To get started, run the following commands:
```bash
$ sudo apt update
$ sudo apt upgrade
$ sudo apt install libboost-all-dev
$ git clone git@github.com:maxcalincu/diskpack.git
$ cd diskpack
$ chmod +x build.sh
$ ./build.sh  # Run from the root directory
```


To pull the changes from remote repository run:

```bash
$ git checkout master
$ git pull origin master
```

## visualizer
The visualizer generates a compact disk packings with some radii set from a given region. To use it, run the following commands from ``build`` directory:
```bash
$ make visualizer
$ ./visualizer --help
```

This will display a detailed description of the command-line interface. To generate an image with the packing, use ``visualization-tool/notebook.ipynb``

## finder
The finder identifies all of the small subregions within a given region which contain a radii set that allows a compact disk packing. To use it, run these commands from ``build`` directory:

```bash
$ make finder
$ ./finder
```

Note: A command-line interface is not yet available, and additional features are still in development.