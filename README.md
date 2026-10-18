# diskpack: Algorithms for Compact Disk Packings

C++ library for finding radius configurations that admit compact disk packings on the plane. Implements interval arithmetic and computational geometry methods.


<table>
  <tr>
    <td width="70%" valign="top">

## 📖 About
The initial goal is to investigates the problem of finding radius triples `(r, s, t)` where disk packings with radii `{1, r, s, t}` exist such that:
- The contact graph is a triangulation
- All uncovered regions are curvilinear triangles

Some examples of packings can be seen in ``/images``

Relevance:
- Connects to aperiodic packing problems (Wang tiles)
- Applications in materials science and crystallography

  </td>
    <td width="30%" align="center" valign="middle">

  <img src="./images/gifs/animation_2.gif" alt="Description" width="200" />
  
  </td>
  </tr>
</table>

For a more detailed overview see ``/docs`` [in Russian]


Related papers:

- **Likos, C. N., & Henley, C. L.** (1992). *Complex alloy phases for binary hard-disc mixtures*. [Philosophical Magazine B](https://www.tandfonline.com/doi/abs/10.1080/13642819308215284)  
- **Daumas, M., Melquiond, G., & Munoz, C.** (2005). *Guaranteed proofs using interval arithmetic*. [IEEE](https://ieeexplore.ieee.org/document/1467639).
- **Kennedy, T.** (2004). *Compact packings of the plane with two sizes of discs*. [arXiv](https://arxiv.org/abs/math/0407145).  
- **Fernique, T., Hashemi, A., & Sizova, O.** (2020). *Compact packings of the plane with three sizes of discs*. [arXiv](https://arxiv.org/abs/1810.02231).  
- **Fernique, T.** (2023). *Packing unequal disks in the Euclidean plane*. [arXiv](https://arxiv.org/abs/2305.12919).
- **Messerschmidt, M.** (2022). *The number of configurations of radii that can occur in compact packings of the plane with discs of n sizes is finite*. [arXiv](https://arxiv.org/abs/2110.15831).  

## ⚙️ Features
- **Interval Search Engine**  
  Guaranteed testing of ε-neighborhoods in `(0,1)³` without false negatives
- **Packing Generator**  
  Constructs contact graphs with precision control
- **Parallel Computation**  
  Optimized parameter space processing

## 🚀 Building and Running
**Dependencies:**
- C++17 compatible compiler
- libboost
- nlohmann-json3

To get started, run the following commands from the root directory:
```bash
$ sudo apt-get install nlohmann-json3-dev
$ sudo apt install libboost-all-dev
$ ./build.sh
```

## visualizer
The visualizer generates a compact disk packing with some radii set from a given region. To use it, run the following commands:
```bash
$ cd build
$ make visualizer
$ ./visualizer --help
```

This will display a detailed description of the command-line interface. The ``svg`` image of the generated packing are located in ``images`` folder.

## finder
The finder identifies all of the small subregions within a given region which contain a radii set that allows a compact disk packing. To display the CLI description, run these commands:

```bash
$ cd build
$ make finder
$ ./finder --help
```

## Tests

Tests use Catch2 3.5.2, a shared `catch_main`, and `add_catch` CMake targets,
following [assembly-graphs](https://github.com/maxcalincu/assembly-graphs).
They require the normal project dependencies and CMake >= 3.16. Catch2 is
downloaded automatically during the first configuration with testing enabled.

Run the following commands from the repository root.

### Run all tests

```bash
cmake -S . -B build-tests -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build-tests --target test_diskpack -j 4
cmake -E chdir build-tests ctest --output-on-failure -j 4
```

`test_diskpack` includes the library tests and integration tests of `finder` and
`visualizer`. Both CLI executables are built automatically. CTest prints the
output of failed tests and returns a nonzero exit code if any test fails.

### Run one module or selected cases

Select cases from the aggregate executable using Catch2 tags:

```bash
./build-tests/test_diskpack "[geometry]"
./build-tests/test_diskpack "[regression]"
./build-tests/test_diskpack "[cli]"
```

Available module tags are `[regions]`, `[geometry]`, `[corona]`, `[graph]`,
`[generator]`, `[search]`, `[codec]`, and `[cli]`. List all cases with:

```bash
./build-tests/test_diskpack --list-tests
```

As in assembly-graphs, each module also has its own executable. Build the desired
target before running it:

```bash
cmake --build build-tests --target test_geometry test_cli -j 4
./build-tests/test_geometry
./build-tests/test_cli
```

### Run with sanitizers

The `ASAN` configuration enables AddressSanitizer and UndefinedBehaviorSanitizer:

```bash
cmake -S . -B build-asan -DCMAKE_BUILD_TYPE=ASAN -DBUILD_TESTING=ON
cmake --build build-asan --target test_diskpack -j 4
cmake -E chdir build-asan ctest --output-on-failure -j 4
```

CLI tests require a POSIX shell (Linux or macOS). Use `-DBUILD_TESTING=OFF` when
configuring a build without tests. For coverage reporting and the scope of the
checks, see [tests/README.md](tests/README.md).
