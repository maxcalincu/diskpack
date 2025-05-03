#include <diskpack/constants.h>
#include <diskpack/generator.h>
#include <diskpack/codec.h>

#include <boost/program_options.hpp>
#include <chrono>
#include <iostream>
#include <fstream>

using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

using namespace diskpack;
namespace po = boost::program_options;

const size_t DEFAULT_SIZE_UPPER_BOUND = 200;
const BaseType DEFAULT_PACKING_RADIUS = 5;
const BaseType DEFAULT_PRECISION_UPPER_BOUND = 0.5;
const std::string DEFAULT_OUTPUT_FILE = "../images/default.svg";

std::vector<Interval> radii;

int main(int argc, char *argv[]) {
  bool use_custom_values = false;
  size_t size_upper_bound;
  size_t central_disk_type;
  BaseType packing_radius;
  BaseType precision_upper_bound;
  std::string output_file;

  try {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Show this help message\n")

        ("region-size,r", po::value<BaseType>()->default_value(DEFAULT_PACKING_RADIUS), "Sets an upper limit on the region size. Only a circular region of the plane with a given radius (provided by this flag) will be covered with disks\n")
        ("number-of-disks,n", po::value<size_t>()->default_value(DEFAULT_SIZE_UPPER_BOUND), "Sets an upper limit on the number of disks the packing contains\n")
        ("output,o", po::value<std::string>()->default_value(DEFAULT_OUTPUT_FILE), ".svg file to store the generated packing\n")
        
        ("precision,p", po::value<BaseType>()->default_value(DEFAULT_PRECISION_UPPER_BOUND), "Sets an upper limit on the precision of the disk coordinates\n")

        ("central-disk,c", po::value<size_t>(), "Sets the central disk's type (0 <= i < radii.size()) where i is the index of the corresponding radius in the set. By default the radii are sorted in decreasing order\n")
        ("i2", po::value<size_t>(), "Use the i'th radius r which allow a compact packing by discs of sizes 1, r and s (0 < i <= 9)\n")
        ("i3", po::value<size_t>(), "Use the i'th pair (r, s) which allow a compact packing by disks of sizes 1, r and s (0 < i <= 164)\n")
        ("input,i", po::value<std::string>(), "JSON file with the region to perform the search in")
      ;

        po::positional_options_description p;

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv)
            .options(desc)
            .positional(p)
            .run(), vm);

        if (vm.count("help")) {
              std::cout << "Usage of " << argv[0] << "\n";
              std::cout << desc << "\n";
              std::cout << "Examples of usage:\n";
              std::cout << argv[0] << " -p 0.2 -n 200 --i3 53 -r 15\n\n";
              std::cout << "\
The visualizer generates a compact disk packings with some radii set from a given region. svg images of the generated packing are located in images folder.\n\
The exection ends with one of the following statuses: 'complete', 'invalid', 'precision_error' and 'corona_error'\n\
\n\
'complete' status means that a packing was succesfully generated.\n\
'invalid' status means that the region does not contain any radii set that allows a compact disk packing\n\
'precision_error' status means that the upper limit on the width of the intervals of the disk coordinates was violated. Use -p flag to set it\n\
'corona_error' status means that the corona continuity assumption was violated. This almost certainly means that coordinate interval width became abnormally big\n\
\n\
Use -n and -r flags to set an upper limit on the number of disks and the size of the region covered respectively. Iff at least one of these limits is reached the generation stops with 'complete' status\n\
The --i2, --i3 and -i flags are used to establish the radii set used. Exactly one of these flags must be provided\n\
\n\
The generated packing is stored in an output file (see --output flag). The output file remains untouched iff the execution ends with 'invalid' status\n";

              return 0;
        }
        po::notify(vm);

        size_upper_bound = vm["number-of-disks"].as<size_t>();
        precision_upper_bound = vm["precision"].as<BaseType>();
        packing_radius = vm["region-size"].as<BaseType>();
        output_file = vm["output"].as<std::string>();

        if ((vm.count("i2") && vm.count("i3")) || 
            (vm.count("input") && vm.count("i3")) || 
            (vm.count("i2") && vm.count("input"))  ||
            (!vm.count("i2") && !vm.count("i3") && !vm.count("input"))
          ) {
            throw std::runtime_error("Exactly one flag from {i2, i3, i} must be provided");
          }
        if (vm.count("i2")) {
          auto i2 = vm["i2"].as<size_t>() - 1;
          if (i2 < 0 || i2 >= 9) {
            throw std::runtime_error("0 < i2 <= 9 constraint violated");
          }
          radii = std::vector<Interval>{one, two_radii[i2]};
        }
        if (vm.count("i3")) {
          auto i3 = vm["i3"].as<size_t>() - 1;
          if (i3 < 0 || i3 >= 164) {
            throw std::runtime_error("0 < i3 <= 164 constraint violated");
          }
          radii = std::vector<Interval>{one, three_radii[i3].first, three_radii[i3].second};
        }
        if (vm.count("input")) {
          std::vector<RadiiRegion> regions;
          std::string filename = vm["input"].as<std::string>();
          std::ifstream file(filename);
          if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filename);
          }
          DecodeRegionsJSON(file, regions);
          if (regions.size() != 1) {
            throw std::runtime_error("Exactly one region should be provided");
          }
          radii = regions[0].GetIntervals();
        }
        if (vm.count("central-disk")) {
          central_disk_type = vm["central-disk"].as<size_t>();
        } else {
          central_disk_type = std::distance(radii.begin(), std::min_element(radii.begin(), radii.end(), [](const Interval &a, const Interval &b) {
            return a.lower() < b.lower();
          }));
        }
        
  } catch (const po::error& e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  } catch (const std::exception& e) {
    std::cerr << "Unhandled exception: " << e.what() << "\n";
    return 1;
}
  
  std::sort(radii.begin(), radii.end(), [](const Interval& a, const Interval& b) {
    return cerlt(a, b);
  });
  std::cerr << "visualizer called on: " << EncodeRegionsJSON(std::vector<RadiiRegion> {
    {radii}
  });

  BasicGenerator generator{radii, packing_radius, precision_upper_bound, size_upper_bound};

  auto t1 = high_resolution_clock::now();
  auto status = generator.Generate(central_disk_type);
  auto t2 = high_resolution_clock::now();
  auto ms_int = duration_cast<milliseconds>(t2 - t1);
  
  std::cout << "status:  \t" << status << "\n";
  std::cout << "duration:\t" << ms_int.count() << "ms\n";

  if (status != PackingStatus::invalid) {
    WritePackingSVG(output_file, generator.GetPacking(), generator.GetGeneratedRadius() + 1);
  }

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