#include <diskpack/search.h>

namespace diskpack {
   
  /// CoronaChecker
    bool CoronaChecker::Inspect(const RadiiRegion &radii_) {
    SetRadii(radii_.GetIntervals());
    Reset();        

    for (size_t i = 0; i < radii.size(); ++i) {
        Reset();
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
    
    CoronaChecker::CoronaChecker(const BaseType &precision_upper_bound_):   BasicGenerator({},
                                                                            100,
                                                                            precision_upper_bound_,
                                                                            3,
                                                                            100){};

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
    if (radii_.IsTooWide(0.00001001)) {
      return true;
    }
    for (size_t i = 0; i < radii_.GetIntervals().size(); ++i) {
        auto status = basic_generator.Generate(i);
        if (status == PackingStatus::invalid) {
        return false;
        }
    }
    return true;
    }

///DSUFilter
    size_t DSUFilter::Get(size_t x) {
        parent[x] = (parent[x] == x ? x : Get(parent[x]));
        return parent[x];
    }
    void DSUFilter::Unite(size_t x, size_t y) {
        x = Get(x);
        y = Get(y);
        if (x == y) {
            return;
        }
        for (size_t j = 0; j < vals[x].size(); ++j) {
            vals[y][j] = Interval{std::min(vals[y][j].lower(), vals[x][j].lower()), std::max(vals[y][j].upper(), vals[x][j].upper())};
        }
        if (component_size[x] > component_size[y]) {
            std::swap(x, y);
        }
        parent[x] = y;
        component_size[y] += component_size[x];
    }
    DSUFilter::DSUFilter() {};
    void DSUFilter::operator()(std::vector<RadiiRegion> &elements) {
        component_size.resize(elements.size());
        parent.resize(elements.size());
        vals.resize(elements.size());
        for (size_t i = 0; i < elements.size(); ++i) {
            component_size[i] = 1;
            parent[i] = i;
            vals[i] = elements[i].GetIntervals();
        }
        for (size_t i = 0; i < elements.size(); ++i) {
            auto edge = vals[i];
            for (size_t j = 0; j < edge.size(); ++j) {
                auto interval = edge[j];
                edge[j] = Interval{interval.lower(), interval.lower()};
                auto it = edges.find(edge);
                if (it != edges.end()) {
                    Unite(i, it->second);
                } else {
                    edges[edge] = i;
                }

                edge[j] = Interval{interval.upper(), interval.upper()};
                it = edges.find(edge);
                if (it != edges.end()) {
                    Unite(i, it->second);
                } else {
                    edges[edge] = i;
                }
                edge[j] = interval;
            }
        }
        elements.clear();
        for (size_t i = 0; i < parent.size(); ++i) {
            if (parent[i] == i) {
                elements.emplace_back(vals[i]);
            }
        }
    }
}