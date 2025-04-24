#include "diskpack/corona.h"
#include <diskpack/generator.h>
#include <map>

#pragma once

namespace diskpack {

    class DSUFilter {
        std::vector<size_t> component_size;
        std::vector<size_t> parent;
        std::map<std::vector<Interval>, size_t, RadiiCompare> edges;
        std::vector<std::vector<Interval>> vals;
        size_t Get(size_t x);
        void Unite(size_t x, size_t y);
    public:
        DSUFilter();
        void operator()(std::vector<RadiiRegion> &elements);
    };

    class Searcher {
        std::vector<RadiiRegion>& results;
        BaseType lower_bound;                                                                       ///iff the region's size is less than lower_bound: push it into the list
        BaseType upper_bound;                                                                       ///iff the region's size is greater than upper_bound: don't inspect it with cheker 
        bool ExpensiveCheck(const RadiiRegion& region);
        void ProcessRegion(const RadiiRegion& region, std::vector<RadiiRegion>& r, std::optional<ConnectivityGraph> &graph);                                                    /// Inspect a region for potential compact disk packings.
    public:
        Searcher(std::vector<RadiiRegion> &results, BaseType lower_bound, BaseType upper_bound);    
        void StartProcessing(std::vector<Interval> region);                                                  /// Public function. Distributes the tasks between multiple threads
    };

}