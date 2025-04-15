#include "diskpack/checkers.h"
#include <diskpack/search.h>
#include <iomanip>
#include <iostream>
#include <chrono>
#include <thread>


using namespace diskpack;

const size_t DEFAULT_SIZE_UPPER_BOUND = 100;
const BaseType DEFAULT_PACKING_RADIUS = 6;
const BaseType DEFAULT_PRECISION_UPPER_BOUND = 0.2;

int main() {
    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    using std::chrono::milliseconds;
    RadiiRegion region{std::vector<Interval> {
        // Interval{0.46, 0.48},
        // Interval{0.822, 0.827},

        // Interval{0.47, 0.471},
        // Interval{0.56, 0.561},
        // Interval{0.86, 0.862}
        // {0.2, 0.9},
        {0.713, 0.714},
        {0.627, 0.628},
        {0.556, 0.557},
        one, 
    }};
    BasicChecker checker{DEFAULT_PACKING_RADIUS, DEFAULT_PRECISION_UPPER_BOUND, DEFAULT_SIZE_UPPER_BOUND};
    std::vector<RadiiRegion> results;
    Searcher<BasicChecker> searcher{results, checker, 0.00001, 0.01};
   
    std::cerr << std::thread::hardware_concurrency() << "\n";
   
    auto t1 = high_resolution_clock::now();    
    searcher.StartProcessing(region);
    auto t2 = high_resolution_clock::now();
  
    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    std::cout << ms_int.count()/1000 << "s\n";

    std::cout << results.size() << "\n";
    std::vector<BaseType> a(region.GetIntervals().size()), A(region.GetIntervals().size());
    bool untouched = true;
    for (auto &x : results) {
        for (size_t i = 0; i < x.GetIntervals().size(); ++i) {
            std::cerr << std::setprecision(10) << x.GetIntervals()[i].lower() << " " << x.GetIntervals()[i].upper() << " ";
        }
        std::cerr << "\n";
                
        if (untouched) {
            for (size_t i = 0; i < a.size(); ++i) {
                a[i] = x.GetIntervals()[i].lower();
                A[i] = x.GetIntervals()[i].upper();
            }
            untouched = false;    
        }
        for (size_t i = 0; i < a.size(); ++i) {
            a[i] = std::min(a[i], x.GetIntervals()[i].lower());
            A[i] = std::max(A[i], x.GetIntervals()[i].upper());
        }
    }
    std::cerr << "\n";
    for (size_t i = 0; i < a.size(); ++i) {
        std::cerr << std::setprecision(10) << a[i] << " " << A[i] << " ";
    }
    std::cerr << "\n";

    return 0;
}