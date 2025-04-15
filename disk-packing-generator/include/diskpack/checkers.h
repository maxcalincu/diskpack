#include <diskpack/generator.h>

#pragma once

namespace CDP {
    class RadiiRegion {
        std::vector<Interval> intervals;
    public:
        
        RadiiRegion(const std::vector<Interval> &intervals_);
        RadiiRegion(std::vector<Interval> &&intervals_);
        
        const std::vector<Interval>& GetIntervals() const;
        
        bool IsNarrowEnough(BaseType lower_bound) const;
        bool IsTooWide(BaseType upper_bound) const;
        Interval GetMinInterval() const;
        Interval GetMaxInterval() const;

        void Split(std::vector<RadiiRegion> &regions, size_t k = 2, size_t index = -1) const;
        void GridSplit(std::vector<RadiiRegion> &regions, size_t k, size_t index = 0) const;
    };
    
    class CoronaChecker: private BasicGenerator {
    protected:    
        bool PackingSatisfiesConstraints() const override;
        bool IsInBounds(const Disk *disk) const override;
        bool PackingIsLargeEnough() const override;
    public:
        bool Inspect(const RadiiRegion &radii_);
        const std::list<Disk> &GetPacking();
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
}