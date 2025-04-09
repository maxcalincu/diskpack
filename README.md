This project provides several tools for working with compact disk packings.

## Clone and build project
To get started, run the following commands:
```bash
$ sudo apt update
$ sudo apt upgrade
$ sudo apt install libboost-all-dev
$ git clone git@github.com:maxcalincu/diskpackings.git
$ cd diskpackings
$ chmod +x build.sh
$ ./build.sh  # Run from the root directory
```

## visualizer
The visualizer generates a compact disk packings with some radii set from a given region. To use it, run the following commands from ``build`` directory:
```bash
$ make visualizer
$ ./visualization-tool/visualizer --help
```

This will display a detailed description of the command-line interface. To generate an image with the packing, use ``visualization-tool/notebook.ipynb``

## finder
The finder identifies all of the small subregions within a given region which contain a radii set that allows a compact disk packing. To use it, run these commands from ``build`` directory:

```bash
$ make finder
$ ./configuration-finder/finder
```

Note: A command-line interface is not yet available, and additional features are still in development.