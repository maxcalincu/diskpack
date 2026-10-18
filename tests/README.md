# Tests

The test system follows [assembly-graphs](https://github.com/maxcalincu/assembly-graphs):
Catch2 **3.5.2** is fetched by `tools/cmake/FindCatch.cmake`,
`tools/catch_main.cpp` provides the shared entry point, and `add_catch` creates
an aggregate executable plus individual module executables. Assertions remain
active in Release builds. CTest discovers the aggregate executable's test cases.

## Build and run

The normal project dependencies (Boost and nlohmann-json), CMake >= 3.16, and a
C++17 compiler are required. The first configuration downloads Catch2. CLI tests
run the built `finder` and `visualizer` using a POSIX shell, supporting Linux and macOS.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target test_diskpack -j 4
cd build
ctest --output-on-failure -j 4
./test_diskpack "[geometry]"
```

Other tags are `[regions]`, `[corona]`, `[graph]`, `[generator]`, `[search]`,
`[codec]`, `[cli]`, and `[regression]`. Individual executables (`test_geometry`,
`test_corona`, etc.) are separate build targets, just as in assembly-graphs.
Only the aggregate executable is registered with CTest, avoiding duplicate runs.
Use `-DBUILD_TESTING=OFF` to build without fetching Catch2.

## Scope

| Component | Checks |
|---|---|
| Geometry | Disk contact predicates, uncertain gaps, current `min` precision contract, operator composition and caching, tangencies for all equal/different type combinations, enclosure of sampled radii, region partition coverage |
| Coronas | Strict weak ordering, coordinate axes, 720 input permutations, angular cut, completion, push/pop replay, signatures, unresolved order |
| Connectivity graph | Valid and impossible configurations, triangle symmetry, nested `Refine`/`Restore`, state restoration, uncertainty disabling pruning |
| Generator | Equal-priority rollback, known one/two/three-radius packings, pairwise overlap checks, resume/reset, uniform scaling, constraints, statuses, blocked continuations |
| Search | All nine known binary radii, rejection of an impossible ratio, recursive search with one/two workers, DSU components and all 24 regression permutations |
| JSON/SVG | NDJSON round trips for binary-exact endpoints, blank/malformed input, SVG geometry/colors, missing output paths |
| CLI | Help, invalid options/selectors/input, JSON input, parsed SVG output, preservation of existing output on `invalid`, finder-to-visualizer integration |

Generated packings are checked through geometric invariants rather than SVG
snapshots: randomized choices need not produce identical disk arrangements.
Library generator tests seed the PRNG for repeatability. Tests use small bounded
problems; they do not repeat the long four-radius classification experiment or
prove completeness of the search algorithm. Testing all components does not mean
100% branch coverage. Changing the number/types of radii through `SetRadii`,
invalid numeric search parameters, and exhaustive numerical boundary cases need
additional work beyond the cases listed above.

## Sanitizers

As in assembly-graphs, the custom `ASAN` configuration enables AddressSanitizer
and UndefinedBehaviorSanitizer; `TSAN` enables ThreadSanitizer.

```sh
cmake -S . -B build-asan -DCMAKE_BUILD_TYPE=ASAN
cmake --build build-asan --target test_diskpack -j 4
cd build-asan
ctest --output-on-failure -j 4
```

## Coverage

`-DDISKPACK_ENABLE_COVERAGE=ON` instruments the library, both CLIs and test
executables. Clang uses LLVM coverage; GCC uses gcov-compatible `--coverage`.
Keep the build in Debug mode for readable source-level reports.

For Clang, run tests with a distinct profile name for each process, including
the CLI subprocesses:

```sh
cmake -S . -B build-coverage -DCMAKE_BUILD_TYPE=Debug -DDISKPACK_ENABLE_COVERAGE=ON
cmake --build build-coverage --target test_diskpack -j 4
cd build-coverage
LLVM_PROFILE_FILE="$PWD/coverage/%m-%p.profraw" ctest --output-on-failure -j 4
llvm-profdata merge -sparse coverage/*.profraw -o coverage/merged.profdata
llvm-cov report diskpack/libdiskpack.so -object finder -object visualizer \
  -instr-profile=coverage/merged.profdata \
  -ignore-filename-regex='/(tests|_deps)/'
```

On macOS the library is `diskpack/libdiskpack.dylib`; Apple LLVM tools can be
located with `xcrun --find llvm-cov` and `xcrun --find llvm-profdata`. Filter system
and dependency headers as needed when reporting only project source files.
