#include <diskpack/constants.h>
#include <diskpack/generator.h>

#include <chrono>
#include <iostream>

using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;
std::string storage_file = "../storage/1.txt";

using namespace CDP;

int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cerr << "Usage:  " << argv[0] << " <packing-number> <packing-radius>" << std::endl;
    return -1;
  }

  int i;
  BaseType packing_radius, lower, upper;
  std::sscanf(argv[1], "%d", &i);
  std::sscanf(argv[2], "%Lf", &packing_radius);
  // std::sscanf(argv[3], "%Lf", &lower);
  // std::sscanf(argv[4], "%Lf", &upper);
  std::vector<size_t> coronal_code(0);
  std::vector<Interval> radii{one};
  // std::vector<Interval> radii{one, three_radii[i].first, three_radii[i].second};
  // std::vector<Interval> radii{one, two_radii[i]};
  radii.push_back(two_radii[i]);
  // radii.push_back(d[i].first); radii.push_back(d[i].second);
  // radii = proposed1;

  storage_file = std::string{"../storage/"} + char('0') + std::string{".txt"};
  BaseType precision_threshold = 0.5;
  PackingGenerator generator{radii, packing_radius, precision_threshold};

  PackingStatus status;
  auto t1 = high_resolution_clock::now();
  status = generator.Generate();
  auto t2 = high_resolution_clock::now();

  auto ms_int = duration_cast<milliseconds>(t2 - t1);
  std::cout << ms_int.count() << "ms\n";

  if (status != PackingStatus::invalid) {
    DumpPacking(storage_file, generator.GetPacking(), generator.GetRadius());
  }
  std::cout << status << "\n";

  return 0;
}

// c1 0.6375559772 1111r
// c2 0.5451510421 111rr
// c3 0.5332964167 1r1r1
// c4 0.4142135624 1111
// c5 0.3861061049 1rrr1
// c6 0.3491981862 1rr1r
// c7 0.2807764064 111r
// c8 0.1547005384 111
// c9 0.1010205144 11rr
