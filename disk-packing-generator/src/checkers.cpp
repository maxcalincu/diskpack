#include "diskpack/generator.h"
#include <algorithm>
#include <boost/numeric/interval/utility_fwd.hpp>
#include <diskpack/checkers.h>
// #include <iostream>

namespace CDP {
/// RadiiRegion
    void RadiiRegion::Split(std::vector<RadiiRegion> &regions, size_t k, size_t index) const {
        regions.clear();
        regions.reserve(k);
        auto intervals_copy = intervals;
        auto it = index == -1 ? std::max_element(intervals_copy.begin(), intervals_copy.end(), [](const Interval& a, const Interval& b) {
            return width(a) < width(b);
        }) : intervals_copy.begin() + index;

        auto initial = *it;
        auto sub_width = (initial.upper() - initial.lower())/k;
        for (size_t i = 0; i < k; ++i) {
            auto l =              initial.lower() + (i + 0) * sub_width;
            auto u = (i < k - 1 ? initial.lower() + (i + 1) * sub_width : initial.upper());
            *it = Interval{l, u};
            regions.emplace_back(intervals_copy);
        }
    }
    void RadiiRegion::GridSplit(std::vector<RadiiRegion> &regions, size_t k, size_t index) const {
      if (index == 0) {
        regions.clear();
        size_t s = 1;
        for (size_t i = 0; i + 1 < intervals.size(); ++i) {
          s *= k;
        }
        regions.reserve(s);
      }
      if (index + 1 >= intervals.size()) {
        regions.emplace_back(intervals);
        return;
      }
      std::vector<RadiiRegion> children_regions;
      Split(children_regions, k, index);
      for (auto &cr : children_regions) {
        cr.GridSplit(regions, k, index + 1);
      }
    }
    const std::vector<Interval>& RadiiRegion::GetIntervals() const { return intervals; }
    bool RadiiRegion::IsNarrowEnough(BaseType lower_bound) const {
        return !std::any_of(intervals.begin(), intervals.end(), [&lower_bound](const Interval& x) {
            return width(x) > lower_bound;
        }); 
    }

    bool RadiiRegion::IsTooWide(BaseType upper_bound) const {
        return std::any_of(intervals.begin(), intervals.end(), [&upper_bound](const Interval& x) {
            return width(x) > upper_bound;
        });
    }

    RadiiRegion::RadiiRegion(const std::vector<Interval> &intervals_): intervals(intervals_) {
      std::sort(intervals.begin(), intervals.end(), [](const Interval& a, const Interval& b) {
        return median(a) < median(b);
      });
    };
    RadiiRegion::RadiiRegion(std::vector<Interval> &&intervals_): intervals(std::move(intervals_)) {
      std::sort(intervals.begin(), intervals.end(), [](const Interval& a, const Interval& b) {
        return median(a) < median(b);
      });
    }
    Interval RadiiRegion::GetMinInterval() const {
      return intervals.front();
    }
    Interval RadiiRegion::GetMaxInterval() const {
      return intervals.back();
    }
  /// CoronaChecker
    bool CoronaChecker::SatisfiesConstraints() const {
        return *std::max_element(frequency_table.begin(), frequency_table.end()) != packing.size();
      };
      
      bool CoronaChecker::IsInBounds(const Disk *disk) const {
        return true;
      }

      bool CoronaChecker::IsLargeEnough() const {
        return packing.size() > 3;
      }
      
      bool CoronaChecker::Inspect(const RadiiRegion &radii_) {
        SetRadii(radii_.GetIntervals());
        Reset();        

        for (size_t i = 0; i < radii.size(); ++i) {
            Push(Disk(zero, zero, radii[i], i), i);
            for (size_t j = 0; j < radii.size(); ++j) {
              if (i != j) {
                Push(Disk(radii[i] + radii[j], zero, radii[j], j), j);
                auto status = AdvancePacking();
                Pop(j);
                if (status != PackingStatus::invalid) {
                  break;
                }
              }
              if (j + 1 == radii.size()) {
                return false;
              }
            }
      
            Pop(i);
        }

        return true;
      }
      
      CoronaChecker::CoronaChecker(const BaseType &precision_upper_bound_): BasicGenerator({}, 0, precision_upper_bound_, 0){};
  
  /// BasicChecker
      BasicChecker::BasicChecker( const BaseType &packing_radius_, 
                                  const BaseType &precision_upper_bound_,  
                                  const size_t &size_upper_bound_): checker_packing_radius{packing_radius_},
                                                                    checker_precision_upper_bound(precision_upper_bound_),
                                                                    checker_size_upper_bound(size_upper_bound_) {}
      bool BasicChecker::Inspect(const RadiiRegion& radii_) {
        auto corona_checker = CoronaChecker{checker_precision_upper_bound};
        if (!corona_checker.Inspect(radii_)) {
          return false;
        }
        auto basic_generator = BasicGenerator{radii_.GetIntervals(), checker_packing_radius, checker_precision_upper_bound, checker_size_upper_bound};
        for (size_t i = 0; i < radii_.GetIntervals().size(); ++i) {
          auto status = basic_generator.Generate(0);
          if (status == PackingStatus::invalid) {
            return false;
          }
        }
        return true;
      }

}