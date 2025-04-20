#include <diskpack/generator.h>

#include <atomic>
#include <iostream>
#include <thread>
#include <map>

#pragma once

namespace diskpack {
    class CoronaChecker: private BasicGenerator {        
    public:
        bool Inspect(const RadiiRegion &radii_);
        const std::list<DiskPointer> &GetPacking();
        CoronaChecker(const BaseType &precision_upper_bound_);
    };
    class BasicChecker {
    protected:
    const BaseType checker_packing_radius;
    const BaseType checker_precision_upper_bound;
    const size_t checker_size_upper_bound;

    public:
        bool Inspect(const RadiiRegion &radii_);
        BasicChecker(const BaseType &packing_radius_, const BaseType &precision_upper_bound_, const size_t &size_upper_bound_);
    };


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

    template <typename Checker>
    class Searcher {
        std::vector<RadiiRegion>& results;
        Checker c;
        BaseType lower_bound;                                                                       ///iff the region's size is less than lower_bound: push it into the list
        BaseType upper_bound;                                                                       ///iff the region's size is greater than upper_bound: don't inspect it with cheker 

        void ProcessRegion(const RadiiRegion& region, std::vector<RadiiRegion>& r);                                                    /// Inspect a region for potential compact disk packings.
                                                                                                          /// No false negative answers are produced
    public:
        Searcher(std::vector<RadiiRegion> &results_, Checker c_, BaseType lower_bound_, BaseType upper_bound_);    
        void StartProcessing(std::vector<Interval> region);                                                  /// Public function. Distributes the tasks between multiple threads
    };

    template <typename Checker>
    Searcher<Checker>::Searcher(std::vector<RadiiRegion> &results_, Checker c_, BaseType lower_bound_, BaseType upper_bound_): results{results_}, c{c_},
                                                                                                                upper_bound(upper_bound_), lower_bound(lower_bound_) {};
    
    template <typename Checker>
    void Searcher<Checker>::ProcessRegion(const RadiiRegion& region, std::vector<RadiiRegion>& r) {
        // std::cout << 
        if (!region.IsTooWide(upper_bound)){
            if (!c.Inspect(region.GetIntervals())) {
                return;
            }
        }
        if (region.IsNarrowEnough(lower_bound)) {
            r.emplace_back(region.GetIntervals());
            return;
        }

        std::vector<RadiiRegion> children_regions;
        region.GridSplit(children_regions, 2);
        for (auto& cr: children_regions) {
            ProcessRegion(cr, r);
        }
    }

    template <typename Checker>
    void Searcher<Checker>::StartProcessing(std::vector<Interval> intervals) {
        std::sort(intervals.begin(), intervals.end(), [](const Interval& a, const Interval& b) {
            return cerlt(a, b);
        });
        RadiiRegion region(intervals);
        std::vector<RadiiRegion> children_regions;
        size_t k = std::thread::hardware_concurrency();
        region.GridSplit(children_regions, k);
        std::vector<std::vector<RadiiRegion>> thread_results(k * 2);
        std::vector<std::thread> threads;
        
        std::atomic<size_t> task_index{0};

        for (size_t i = 0; i < k; ++i) {
            threads.emplace_back([&task_index, &thread_results, 
                                        &children_regions, this, i ] {
                while (true) {
                    size_t index = task_index.fetch_add(1, std::memory_order_relaxed);
                    if (index >= children_regions.size()) {
                        break;
                    }
                    std::cerr << "\r" + std::to_string(((index + 1)*100) / children_regions.size()) + "% ";
                    auto &x = children_regions[index];
                    ProcessRegion(children_regions[index], thread_results[i]);
                    std::cerr << "x";
                }
            });
        }
        size_t results_size = 0;
        for (size_t i = 0; i < k; ++i) {
            threads[i].join();
            results_size += thread_results[i].size();
        }
        results.clear();
        results.reserve(results_size);
        for (auto &r : thread_results) {
            results.insert(results.end(), r.begin(), r.end());
        }
        std::cerr << "\ninitial result size\t" << results.size() << "\n";
        
        DSUFilter{}(results);
        std::sort(results.begin(), results.end(), [](const RadiiRegion &a, const RadiiRegion &b) {
            return RadiiCompare{}(a.GetIntervals(), b.GetIntervals());
        });
    }
}