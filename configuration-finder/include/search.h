// #include <atomic>
#include <diskpack/geometry.h>
#include <list>
#include <mutex>
#include <vector>

namespace CDP {
    class Radii {
        std::vector<Interval> radii;
    public:
        const std::vector<Interval> GetIntervals() const;
        void Split(std::vector<Radii> &regions, int k = 2) const;
        bool IsSmallEnough(BaseType lower_bound) const;
        bool IsTooBig(BaseType upper_bound) const;
        Radii(std::vector<Interval> radii_);
    };

    using RadiiList = std::list<Radii>;

    template <typename Checker>
    class Searcher {
        RadiiList& results;
        Checker c;
        BaseType lower_bound;
        BaseType upper_bound;
        mutable std::mutex mu;
    public:
        Searcher(RadiiList &results_, Checker c_, BaseType lower_bound_, BaseType upper_bound_);
        void ProcessRegion(const Radii& region);
    };
}

