#include "../include/search.h"
#include "diskpack/geometry.h"
#include <algorithm>
#include <concepts>
#include <type_traits>

namespace CDP {

    template <typename Checker>
    concept HasInspectMethod = requires(Checker c, Radii r) {
        {c.Inspect(r)} -> std::same_as<bool>;
    };

    void Radii::Split(std::vector<Radii> &regions, int k) const {
        assert(k > 1);
        regions.clear();
        regions.reserve(k);
        auto radii_copy = radii;
        auto largest_interval = std::max_element(radii_copy.begin(), radii_copy.end(), [](const Interval& a, const Interval& b) {
            return width(a) < width(b);
        });
        auto initial = *largest_interval;
        for (size_t i = 0; i < k; ++i) {
            *largest_interval = Interval{   (k - i - 0) * initial.lower() + (i + 0) * initial.upper(), 
                                            (k - i - 1) * initial.lower() + (i + 1) * initial.upper()};
            regions.emplace_back(radii_copy);
        }
    }
    const std::vector<Interval> Radii::GetIntervals() const { return radii; }
    bool Radii::IsSmallEnough(BaseType lower_bound) const {
        return !std::any_of(radii.begin(), radii.end(), [&lower_bound](const Interval& x) {
            return width(x) > lower_bound;
        });
    }

    bool Radii::IsTooBig(BaseType upper_bound) const {
        return std::any_of(radii.begin(), radii.end(), [&upper_bound](const Interval& x) {
            return width(x) > upper_bound;
        });
    }

    Radii::Radii(std::vector<Interval> radii_): radii(radii_) {};

    template <typename Checker>
    Searcher<Checker>::Searcher(RadiiList &results_, Checker c_, BaseType lower_bound_, BaseType upper_bound_): results{results_}, c{c_},
                                                                                                                upper_bound(upper_bound_), lower_bound(lower_bound_) {};
    
    template <typename Checker>
    void Searcher<Checker>::ProcessRegion(const Radii& region) {
        if (!region.IsTooBig(upper_bound)){
            if (c.IsInvalid(region.GetIntervals())) {
                return;
            }
        }
        if (region.IsSmallEnough(lower_bound)) {
            std::lock_guard<std::mutex> lock(mu);
            results.emplace_back(region.GetIntervals());
            return;
        }
        std::vector<Radii> resulting_regions;
        region.Split(resulting_regions);
        for (auto& r: resulting_regions) {
            ProcessRegion(r);
        }
    }
}