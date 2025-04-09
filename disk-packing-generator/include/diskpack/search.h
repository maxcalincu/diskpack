#include <diskpack/checkers.h>
// #include <iostream>
#include <mutex>

#pragma once

namespace CDP {

    template <HasInspectMethod Checker>
    class Searcher {
        RadiiList& results;
        Checker c;
        BaseType lower_bound;                                                                       ///iff the region's size is less than lower_bound: push it into the list
        BaseType upper_bound;                                                                       ///iff the region's size is greater than upper_bound: don't inspect it with cheker 
        mutable std::mutex mu;

        void ProcessRegion(const RadiiRegion& region);                                                    /// Inspect a region for potential compact disk packings.
                                                                                                          /// No false negative answers are produced
    public:
        Searcher(RadiiList &results_, Checker c_, BaseType lower_bound_, BaseType upper_bound_);    
        void StartProcessing(const RadiiRegion& region);                                                  /// Public function. Distributes the tasks between multiple threads
    };

    template <HasInspectMethod Checker>
    Searcher<Checker>::Searcher(RadiiList &results_, Checker c_, BaseType lower_bound_, BaseType upper_bound_): results{results_}, c{c_},
                                                                                                                upper_bound(upper_bound_), lower_bound(lower_bound_) {};
    
    template <HasInspectMethod Checker>
    void Searcher<Checker>::ProcessRegion(const RadiiRegion& region) {
        if (!region.IsTooBig(upper_bound)){
            if (!c.Inspect(region.GetIntervals())) {
                return;
            }
        }
        if (region.IsSmallEnough(lower_bound)) {
            std::lock_guard<std::mutex> lock(mu);
            results.emplace_back(region.GetIntervals());
            return;
        }

        std::vector<RadiiRegion> children_regions;
        region.Split(children_regions);
        
        for (auto& cr: children_regions) {
            ProcessRegion(cr);
        }
    }

    template <HasInspectMethod Checker>
    void Searcher<Checker>::StartProcessing(const RadiiRegion& region) {
        std::vector<RadiiRegion> children_regions;
        region.Split(children_regions, 1);      /// k = hardware_concurency
        for (auto& cr: children_regions) {
            ProcessRegion(cr);                      ///TODO: implement multithreading
        }
    }
}

