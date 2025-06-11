#include <diskpack/codec.h>
#include <diskpack/search.h>

#include <atomic>
#include <iostream>
#include <string>
#include <thread>

namespace diskpack {

/// DSUFilter
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
    vals[y][j] = Interval{std::min(vals[y][j].lower(), vals[x][j].lower()),
                          std::max(vals[y][j].upper(), vals[x][j].upper())};
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

// Searcher
Searcher::Searcher(std::vector<RadiiRegion> &results_, BaseType lower_bound_,
                   BaseType upper_bound_)
    : results{results_}, upper_bound(upper_bound_),
      lower_bound(lower_bound_) {};

void Searcher::ProcessRegion(const RadiiRegion &region,
                             std::vector<RadiiRegion> &r,
                             std::optional<ConnectivityGraph> &graph) {
  if (!region.IsTooWide(upper_bound)) {
    if (!graph.has_value()) {
      OperatorLookupTable lookup_table(region.GetIntervals());
      graph.emplace(lookup_table);

      if (graph->HasOverflow()) {
        graph.reset();
      } else {
        if (!graph->IsViable()) {
          graph.reset();
          return;
        }
      }
    } else {
      OperatorLookupTable lookup_table(region.GetIntervals());
      graph->Refine(lookup_table);
      if (!graph->IsViable()) {
        graph->Restore();
        return;
      }
    }
  }

  if (region.IsNarrowEnough(lower_bound)) {
    if (ExpensiveCheck(region.GetIntervals())) {
      r.emplace_back(region.GetIntervals());
    }
    if (graph.has_value()) {
      if (!graph->Restore()) {
        graph.reset();
      }
    }
    return;
  }

  std::vector<RadiiRegion> children_regions;
  region.GridSplit(children_regions, 2);
  for (auto &cr : children_regions) {
    ProcessRegion(cr, r, graph);
  }

  if (graph.has_value()) {
    if (!graph->Restore()) {
      graph.reset();
    }
  }
}

bool Searcher::ExpensiveCheck(const RadiiRegion &region) { /// TODO: implement
                                                           /// it
  return true;
}

void Searcher::StartProcessing(std::vector<Interval> intervals, size_t k) {
  std::sort(intervals.begin(), intervals.end(),
            [](const Interval &a, const Interval &b) { return cerlt(a, b); });

  RadiiRegion region(intervals);
  std::vector<RadiiRegion> children_regions;
  region.GridSplit(children_regions, 1 * k);
  std::vector<std::vector<RadiiRegion>> thread_results(k);
  std::vector<std::thread> threads;

  std::atomic<size_t> task_index{0};

  for (size_t i = 0; i < k; ++i) {
    threads.emplace_back(
        [&task_index, &thread_results, &children_regions, this, i] {
          while (true) {
            size_t index = task_index.fetch_add(1, std::memory_order_relaxed);
            if (index >= children_regions.size()) {
              break;
            }
            auto progress = ((index + 1) * 100'000) / children_regions.size();
            std::cerr << "\r" + std::to_string(progress / 1000) + "." +
                             (progress % 1000 < 10 ? "0" : "") +
                             (progress % 1000 < 100 ? "0" : "") +
                             std::to_string(progress % 1000) + "% ";
            auto &x = children_regions[index];
            std::optional<ConnectivityGraph> graph = std::nullopt;
            ProcessRegion(children_regions[index], thread_results[i], graph);
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
  std::cerr << "\ninitial result size:\t" << results.size() << "\n";

  DSUFilter{}(results);
  std::sort(results.begin(), results.end(),
            [](const RadiiRegion &a, const RadiiRegion &b) {
              return RadiiCompare{}(a.GetIntervals(), b.GetIntervals());
            });
}

} // namespace diskpack