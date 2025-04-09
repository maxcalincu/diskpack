#include "diskpack/checkers.h"
#include <diskpack/search.h>
#include <iostream>

using namespace CDP;

const size_t DEFAULT_SIZE_UPPER_BOUND = 30;
const BaseType DEFAULT_PACKING_RADIUS = 5;
const BaseType DEFAULT_PRECISION_UPPER_BOUND = 0.15;

int main() {
    RadiiRegion region{std::vector<Interval> {
        one, 
        Interval{0.11, 0.13},
    }};
    BasicChecker checker{DEFAULT_PACKING_RADIUS, DEFAULT_PRECISION_UPPER_BOUND, DEFAULT_SIZE_UPPER_BOUND};
    RadiiList list;
    Searcher<BasicChecker> searcher{list, checker, 0.0001, 0.01};
    searcher.StartProcessing(region);
    std::cout << list.size() << "\n";
    for (auto &x : list) {
        std::cerr << x.GetIntervals()[1].lower() << " " << x.GetIntervals()[1].upper() << "\n";  
    }
    return 0;
}