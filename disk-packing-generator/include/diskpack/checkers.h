#include <diskpack/generator.h>

#pragma once

namespace CDP {
    class RadiiRegion {
        std::vector<Interval> intervals;
    public:
        const std::vector<Interval>& GetIntervals() const;
        void Split(std::vector<RadiiRegion> &regions, int k = 2) const;
        bool IsSmallEnough(BaseType lower_bound) const;
        bool IsTooBig(BaseType upper_bound) const;
        RadiiRegion(std::vector<Interval> radii_);
    };

    template <typename Checker>
    concept HasInspectMethod = requires(Checker c, RadiiRegion r) {
        {c.Inspect(r)} -> std::same_as<bool>;
    };

    using RadiiList = std::list<RadiiRegion>;
    
    class CoronaChecker: private BasicGenerator {
    protected:    
        bool SatisfiesConstraints() const;
        bool IsInBounds(const Disk *disk) const;
        bool IsLargeEnough() const;
    public:
        bool Inspect(const RadiiRegion &radii_);
        const std::list<Disk> &GetPacking();
        CoronaChecker(const BaseType &precision_upper_bound_);
    };
    class BasicChecker {
    protected:
    const BaseType packing_radius;
    const BaseType precision_upper_bound;
    const size_t size_upper_bound;

    public:
        bool Inspect(const RadiiRegion &radii_);
        BasicChecker(const BaseType &packing_radius_, const BaseType &precision_upper_bound_, const size_t &size_upper_bound_);
    };
}