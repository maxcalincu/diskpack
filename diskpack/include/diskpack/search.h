#include <diskpack/generator.h>
#include <map>
#include <thread>

#pragma once

namespace diskpack {

    /// DSUFilter is used to aggregate a set of small regions.

    /// Two regions are considered connected if they have an adjacent side.
    /// Every region of the resulting set represents the union of a connected  
    /// component of the initial set. Hence the DSU in the name of the class.

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

    /// Searcher exhaustively tests ε-neighborhoods in a given region to find 
    /// radius tuples admitting compact disk packings.
    /// The keypoints are:
    /// -- no false negatives are produced
    /// -- recursively splits the parameter space into smaller hypercubes. 
    /// -- terminates when region diameter ≤ ε
    /// -- prunes the branches that do not pass the test 
    /// -- processes independent subregions concurrently
    
    class Searcher {
        std::vector<RadiiRegion>& results;
        BaseType lower_bound;                                                                                                      ///iff the region's size is less than lower_bound: push it into the list
        BaseType upper_bound;                                                                                                      ///iff the region's size is greater than upper_bound: don't inspect it with cheker 
        bool ExpensiveCheck(const RadiiRegion& region);
        void ProcessRegion(const RadiiRegion& region, std::vector<RadiiRegion>& r, std::optional<ConnectivityGraph> &graph);       /// Inspect a region for potential compact disk packings.
    public:
        Searcher(std::vector<RadiiRegion> &results, BaseType lower_bound, BaseType upper_bound);    
        void StartProcessing(std::vector<Interval> region, size_t k = std::thread::hardware_concurrency());                        /// Public function. Distributes the tasks between multiple threads
    };
} // namespace diskpack