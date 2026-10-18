#include "test_support.h"

#include <diskpack/constants.h>
#include <diskpack/search.h>

using namespace diskpack;

std::vector<RadiiRegion> Search(const std::vector<Interval> &radii,
                               BaseType lower, BaseType upper, size_t concurrency) {
  std::vector<RadiiRegion> result;
  Searcher searcher(result, lower, upper);
  searcher.StartProcessing(radii, concurrency);
  return result;
}

TEST_CASE("Search retains all nine known binary radii", "[search]") {
  for (const auto &radius : two_radii) {
    auto result = Search({radius, one}, 1e-7L, 0.001L, 1);
    INFO("Every known binary radius must survive the search");
    REQUIRE((result.size() == 1));
    INFO("A known candidate must not be truncated");
    REQUIRE((result[0].GetIntervals()[0].lower() <= radius.lower() &&
          result[0].GetIntervals()[0].upper() >= radius.upper()));
  }
  INFO("An impossible radius must be pruned");
  REQUIRE((Search({Interval{0.6L}, one}, 1e-7L, 0.001L, 1).empty()));
}

TEST_CASE("Search result is independent of worker scheduling", "[search]") {
  const std::vector<Interval> region{{0.4L, 0.425L}, one};
  auto serial = Search(region, 0.0004L, 0.001L, 1);
  auto parallel = Search(region, 0.0004L, 0.001L, 2);
  INFO("Parallel search size");
  REQUIRE((!serial.empty() && serial.size() == parallel.size()));
  for (size_t i = 0; i < serial.size(); ++i) {
    const auto &a = serial[i].GetIntervals();
    const auto &b = parallel[i].GetIntervals();
    INFO("Worker scheduling must not change the search result");
    REQUIRE((!RadiiCompare{}(a, b) && !RadiiCompare{}(b, a)));
  }
}

TEST_CASE("Region aggregation preserves connected components", "[search]") {
  std::vector<RadiiRegion> regions;
  DSUFilter{}(regions);
  INFO("Empty region aggregation");
  REQUIRE((regions.empty()));
  for (BaseType x : {0.0L, 0.25L}) {
    for (BaseType y : {0.5L, 0.75L}) {
      regions.emplace_back(std::vector<Interval>{{x, x + 0.25L}, {y, y + 0.25L}, {1, 1}});
    }
  }
  regions.emplace_back(std::vector<Interval>{{2, 3}, {4, 5}, {6, 6}});
  DSUFilter{}(regions);
  INFO("Disconnected components must remain separate");
  REQUIRE((regions.size() == 2));
  std::sort(regions.begin(), regions.end(), [](const auto &a, const auto &b) {
    return RadiiCompare{}(a.GetIntervals(), b.GetIntervals());
  });
  const auto &box = regions.front().GetIntervals();
  INFO("A rectangular component must retain both coordinate ranges");
  REQUIRE((box[0].lower() == 0 && box[0].upper() == 0.5L &&
        box[1].lower() == 0.5L && box[1].upper() == 1));
}
