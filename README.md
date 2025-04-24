This project provides several tools for working with compact disk packings.


``diskpack`` is written in C++17. Please make sure that appropriate versions of ``gcc`` and ``clang`` are installed on your device before proceeding.

## Clone and build project
To get started, run the following commands:
```bash
$ sudo apt-get install nlohmann-json3-dev
$ sudo apt install libboost-all-dev
$ git clone git@github.com:maxcalincu/diskpack.git
$ cd diskpack
$ chmod +x build.sh
$ ./build.sh  # Run from the root directory
```

## visualizer
The visualizer generates a compact disk packing with some radii set from a given region. To use it, run the following commands:
```bash
$ cd build
$ make visualizer
$ ./visualizer --help
```

This will display a detailed description of the command-line interface. ``svg`` images of the generated packing are located in ``images`` folder.

## finder
The finder identifies all of the small subregions within a given region which contain a radii set that allows a compact disk packing. To use it, run these commands:

```bash
$ cd build
$ make finder
$ ./finder --help
```

This will display all currently available options.