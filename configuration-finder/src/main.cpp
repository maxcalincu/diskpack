#include <diskpack/search.h>
#include <diskpack/codec.h>

#include <boost/program_options.hpp>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>

using namespace diskpack;
namespace po = boost::program_options;

const size_t DEFAULT_SIZE_UPPER_BOUND = 25;
const BaseType DEFAULT_PACKING_RADIUS = 4;
const BaseType DEFAULT_PRECISION_UPPER_BOUND = 0.2;
const BaseType DEFAULT_LOWER_BOUND = 0.0000001;
const BaseType DEFAULT_UPPER_BOUND = 0.008;

int main(int argc, char *argv[]) {
    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    using std::chrono::milliseconds;

    std::string output_file = "";
    size_t size_upper_bound, k;
    BaseType precision_upper_bound, packing_radius, lower_bound, upper_bound;

    RadiiRegion region{std::vector<Interval> {

        // {0.3, 0.99},
        // {0.3, 0.99},

        
        // {0.713, 0.714},
        // {0.627, 0.628},
        // {0.556, 0.557},

        // {0.71331, 0.71332}, 
        // {0.62746, 0.62747},
        // {0.55623, 0.55624},

        // {0.5, 0.6},
        // {0.60, 0.70},
        // {0.70, 0.8},

        
        // {0.70, 0.75},
        // {0.55, 0.60}, 

        // {0.40, 0.45},
        {0.50, 0.65},
        {0.50, 0.65},
        {0.50, 0.65},


        one, 
    }};

    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "Show this help message\n")
            
            ("lower-bound,l", po::value<BaseType>()->default_value(DEFAULT_LOWER_BOUND), "Region width lower bound for viability check in the search")
            ("upper-bound,u", po::value<BaseType>()->default_value(DEFAULT_UPPER_BOUND), "Region width upper bound for viability check in the search")

            ("concurrency,k", po::value<size_t>(), "Number of threads in the thread pool")

            ("input,i", po::value<std::string>(), "Path to JSON file with the region search in")
            ("output,o", po::value<std::string>(), "Path to JSON file to store the output in. If none provided regions will be outputed in std::cout")
        ;
    
            po::positional_options_description p;
    
            po::variables_map vm;
            po::store(po::command_line_parser(argc, argv)
                .options(desc)
                .positional(p)
                .run(), vm);
    
            if (vm.count("help")) {
                  std::cout << "Usage of " << argv[0] << "\n";
                  std::cout << std::setprecision(7) << desc << "\n";
                  std::cout << "Examples of usage:\n";
                  std::cout << argv[0] << " -p 0.2 -n 200 -r 15 -l 0.001 -u 0.1\n\n";
                  std::cout << "\
The finder identifies all of the small subregions within a given region which contain a radii set that allows a compact disk packing. \n\
Use -n and -r flags to set an upper limit on the number of disks and the size of the region covered respectively in viability checks.\n\
Use -p flag to set an upper bound on the interval precision during viability checks\n\
All regions with a smaller width than the lower_bound added to the results. All regions with a bigger width that the upper_bound are not checked for viability (use -u and -l flags to set them)\n";
    
                  return 0;
            }
            po::notify(vm);

            lower_bound = vm["lower-bound"].as<BaseType>();
            upper_bound = vm["upper-bound"].as<BaseType>();
            if (vm.count("concurrency")) {
                k = vm["concurrency"].as<size_t>();
            } else {
                k = std::thread::hardware_concurrency();
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
                region = regions[0].GetIntervals();
              }
            if (vm.count("output")) {
                output_file = vm["output"].as<std::string>();
            }
      } catch (const po::error& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
      } catch (const std::exception& e) {
        std::cerr << "Unhandled exception: " << e.what() << "\n";
        return 1;
    }

    std::vector<RadiiRegion> results;

    Searcher searcher{results, lower_bound, upper_bound};

    std::cerr << "finder called on:     \t" << EncodeRegionsJSON(std::vector<RadiiRegion> {
        region
    });
    std::cerr << "hardware concurrencry:\t" << std::thread::hardware_concurrency() << "\n";

    auto t1 = high_resolution_clock::now();    
    searcher.StartProcessing(region.GetIntervals(), k);
    auto t2 = high_resolution_clock::now();
  
    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    std::cerr << "duration:             \t" << ms_int.count()/60'000 << "m " << (ms_int.count()/1000)%60 << "s\n";

    std::cerr << "results size:         \t" << results.size() << "\n";
    auto encoded = EncodeRegionsJSON(results);
    if (output_file == "") {
        std::cout << encoded << "\n";
    } else {
        std::ofstream out(output_file);
        if (!out.is_open()) {
            std::cerr << "Failed to open file: " + output_file + "\n";
            std::cout << encoded << "\n";
        } else {
            out << encoded << "\n";
        }
    }
    // auto stream = std::stringstream(encoded);
    // DecodeRegionsJSON(stream, results);
    // for (auto &x : results) {
    //     for (size_t i = 0; i < x.GetIntervals().size(); ++i) {
    //         std::cerr << std::setprecision(10) << x.GetIntervals()[i].lower() << " " << x.GetIntervals()[i].upper() << " ";
    //     }
    //     std::cerr << "\n";
    // }

    return 0;
}