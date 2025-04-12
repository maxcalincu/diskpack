#include <atomic>
#include <diskpack/checkers.h>
#include <thread>
#include <iostream>

#pragma once

namespace CDP {

    template <typename Checker>
    // template <HasInspectMethod Checker>
    class Searcher {
        std::vector<RadiiRegion>& results;
        Checker c;
        BaseType lower_bound;                                                                       ///iff the region's size is less than lower_bound: push it into the list
        BaseType upper_bound;                                                                       ///iff the region's size is greater than upper_bound: don't inspect it with cheker 

        void ProcessRegion(const RadiiRegion& region, std::vector<RadiiRegion>& r);                                                    /// Inspect a region for potential compact disk packings.
                                                                                                          /// No false negative answers are produced
    public:
        Searcher(std::vector<RadiiRegion> &results_, Checker c_, BaseType lower_bound_, BaseType upper_bound_);    
        void StartProcessing(const RadiiRegion& region);                                                  /// Public function. Distributes the tasks between multiple threads
    };

    template <typename Checker>
    // template <HasInspectMethod Checker>
    Searcher<Checker>::Searcher(std::vector<RadiiRegion> &results_, Checker c_, BaseType lower_bound_, BaseType upper_bound_): results{results_}, c{c_},
                                                                                                                upper_bound(upper_bound_), lower_bound(lower_bound_) {
                                                                                                                    std::cerr << results.size() << " SIZE\n";
                                                                                                                };
    
    template <typename Checker>
    // template <HasInspectMethod Checker>
    void Searcher<Checker>::ProcessRegion(const RadiiRegion& region, std::vector<RadiiRegion>& r) {
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
    // template <HasInspectMethod Checker>
    void Searcher<Checker>::StartProcessing(const RadiiRegion& region) {
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
                    auto &x = children_regions[index];
                    ProcessRegion(children_regions[index], thread_results[i]);
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
        // ProcessRegion(region, results);
    }
}
