#include <diskpack/search.h>

namespace diskpack {

    bool RadiiCompare::operator()(const std::vector<Interval> &a, const std::vector<Interval> &b) const {
        for (size_t i = 0; i < a.size(); ++i) {
            if (a[i].lower() != b[i].lower()) {
                return a[i].lower() < b[i].lower();
            }
            if (a[i].upper() != b[i].upper()) {
                return a[i].upper() < b[i].upper();
            }
        }
        return false;
    }

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
    DSUFilter::DSUFilter(const std::vector<RadiiRegion> &elements): component_size(elements.size(), 1), parent(elements.size()), vals(elements.size()) {
        for (size_t i = 0; i < elements.size(); ++i) {
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
    }

    void DSUFilter::operator()(std::vector<RadiiRegion> &results) {
        results.clear();
        for (size_t i = 0; i < parent.size(); ++i) {
            if (parent[i] == i) {
                results.emplace_back(vals[i]);
            }
        }
    }
}