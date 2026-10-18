#include "test_support.h"

#include <diskpack/search.h>

#include <algorithm>
#include <array>

TEST_CASE("DSU bounds survive every input permutation", "[regions][regression]") {
  std::array<size_t, 4> order{0, 1, 2, 3};
  do {
    CAPTURE(order);
    std::vector<diskpack::RadiiRegion> regions;
    for (auto i : order) {
      regions.emplace_back(std::vector<diskpack::Interval>{
          {i / 4.0L, (i + 1) / 4.0L}, {1, 1}});
    }
    diskpack::DSUFilter{}(regions);
    REQUIRE(regions.size() == 1);
    REQUIRE(regions.front().GetIntervals()[0].lower() == 0);
    REQUIRE(regions.front().GetIntervals()[0].upper() == 1);
    REQUIRE(regions.front().GetIntervals()[1].lower() == 1);
    REQUIRE(regions.front().GetIntervals()[1].upper() == 1);
  } while (std::next_permutation(order.begin(), order.end()));
}
