#include "test_support.h"

#include <algorithm>
#include <array>
#include <cmath>

using namespace diskpack;
using test::Contains;

TEST_CASE("Disk relations and precision", "[geometry]") {
  Disk origin{0, 0, 1, 0};
  for (const auto &entry : std::vector<std::pair<BaseType, int>>{
           {1.5L, -1}, {2, 0}, {3, 1}}) {
    Disk other{entry.first, 0, 1, 1};
    INFO("Overlap classification");
    REQUIRE((origin.intersects(other) == (entry.second < 0)));
    INFO("Tangency classification");
    REQUIRE((origin.tangent(other) == (entry.second == 0)));
    INFO("Separation classification");
    REQUIRE((origin.disjoint(other) == (entry.second > 0)));
    INFO("Symmetric overlap");
    REQUIRE((origin.intersects(other) == other.intersects(origin)));
  }
  Disk uncertain{Interval{1.9L, 2.1L}, 0, 1, 1};
  INFO("An uncertain gap must allow tangency");
  REQUIRE((origin.tangent(uncertain)));
  INFO("An uncertain gap must not be classified as certain overlap/separation");
  REQUIRE((!origin.intersects(uncertain) && !origin.disjoint(uncertain)));
  Disk disk{3, 4, 2, 7};
  Contains(disk.get_norm(), 25, "Squared center norm");
  INFO("Disk metadata");
  REQUIRE((disk.get_radius().lower() == 2 && disk.get_type() == 7));
  Disk copied(disk), moved(std::move(copied)), assigned;
  assigned = moved;
  Disk move_assigned;
  move_assigned = std::move(assigned);
  Contains(move_assigned.get_norm(), 25, "Copy and move must preserve geometry");
  Disk anisotropic{Interval{0, 1}, Interval{0, 100}, 1, 0};
  INFO("The current min-width precision is preserved");
  REQUIRE((anisotropic.precision() == 1));
}

TEST_CASE("Similarity operators construct two tangencies", "[geometry]") {
  const std::array<Interval, 3> radii{Interval{0.25L}, Interval{0.5L}, Interval{1}};
  // Each branch for equal and different disk types must put the new center
  // at the intersection of the two independently specified tangency circles.
  for (size_t b = 0; b < radii.size(); ++b) {
    for (size_t p = 0; p < radii.size(); ++p) {
      for (size_t n = 0; n < radii.size(); ++n) {
        SpiralSimilarityOperator op(radii[b], radii[p], radii[n], b, p, n);
        auto previous_x = radii[b] + radii[p];
        auto center = op * IntervalPair{previous_x, Interval{0}};
        Contains(square(center.first) + square(center.second),
                 std::pow(radii[b].lower() + radii[n].lower(), 2),
                 "New disk must touch the base disk");
        Contains(square(center.first - previous_x) + square(center.second),
                 std::pow(radii[p].lower() + radii[n].lower(), 2),
                 "New disk must touch the previous disk");
        INFO("The default turn must be counterclockwise");
        REQUIRE((center.second.lower() > 0));
      }
    }
  }
}

TEST_CASE("Interval operators enclose sampled radii", "[geometry]") {
  std::vector<Interval> radii{{0.24L, 0.26L}, {0.49L, 0.51L}, {0.99L, 1.01L}};
  OperatorLookupTable lookup(radii);
  for (size_t b = 0; b < 3; ++b) {
    for (size_t p = 0; p < 3; ++p) {
      for (size_t n = 0; n < 3; ++n) {
        auto center = lookup(b, p, n).get() *
                      IntervalPair{radii[b] + radii[p], Interval{0}};
        for (unsigned mask = 0; mask < 8; ++mask) {
          std::array<BaseType, 3> sample;
          for (size_t i = 0; i < 3; ++i) {
            sample[i] = (mask & (1U << i)) ? radii[i].upper() : radii[i].lower();
          }
          auto a = sample[b] + sample[p];
          auto c = sample[b] + sample[n];
          auto opposite = sample[p] + sample[n];
          auto x = (a * a + c * c - opposite * opposite) / (2 * a);
          auto y = std::sqrt(c * c - x * x);
          Contains(center.first, x, "Interval x must enclose sampled exact geometry");
          Contains(center.second, y, "Interval y must enclose sampled exact geometry");
        }
      }
    }
  }
}

TEST_CASE("Operator composition and lookup caching", "[geometry]") {
  SpiralSimilarityOperator identity, quarter_turn(0, 1), scale(2, 0);
  IntervalPair vector{Interval{3}, Interval{4}};
  auto result = (quarter_turn * scale) * vector;
  Contains(result.first, -8, "Rotation and scaling x");
  Contains(result.second, 6, "Rotation and scaling y");
  auto unchanged = identity * vector;
  Contains(unchanged.first, 3, "Identity x");
  Contains(unchanged.second, 4, "Identity y");
  SpiralSimilarityOperator copied(quarter_turn), moved(std::move(copied));
  identity = moved;
  scale = std::move(identity);
  Contains((scale * vector).first, -4, "Operator value semantics");
  OperatorLookupTable lookup({Interval{0.5L}, Interval{1}});
  auto *cached = &lookup(0, 1, 0).get();
  INFO("Lookup must reuse a cached operator");
  REQUIRE((cached == &lookup(0, 1, 0).get()));
  Contains((lookup().get() * vector).second, 4, "Lookup identity");
}

TEST_CASE("Radius regions split without losing volume", "[geometry]") {
  RadiiRegion region(std::vector<Interval>{{0.125L, 0.25L}, {0.5L, 0.75L}, {1, 1}});
  INFO("Width predicates");
  REQUIRE((region.IsTooWide(0.125L) && !region.IsNarrowEnough(0.125L)));
  INFO("Inclusive width bound");
  REQUIRE((region.IsNarrowEnough(0.25L)));
  Contains(region.GetMinInterval(), 0.125L, "Minimum interval");
  Contains(region.GetMaxInterval(), 1, "Maximum interval");
  std::vector<RadiiRegion> parts;
  region.Split(parts, 2, 0);
  INFO("Explicit coordinate split");
  REQUIRE((parts.size() == 2));
  INFO("Split must not leave a gap");
  REQUIRE((parts[0].GetIntervals()[0].upper() == parts[1].GetIntervals()[0].lower()));
  region.Split(parts, 2, std::nullopt);
  INFO("Automatic split must select the widest coordinate");
  REQUIRE((parts.size() == 2 && width(parts[0].GetIntervals()[1]) == 0.125L));
  region.GridSplit(parts, 2);
  INFO("Two variable coordinates require four cells");
  REQUIRE((parts.size() == 4));
  BaseType area = 0;
  for (const auto &part : parts) {
    area += width(part.GetIntervals()[0]) * width(part.GetIntervals()[1]);
  }
  INFO("Grid cells must preserve the region volume");
  REQUIRE((area == 0.125L * 0.25L));
  RadiiRegion reversed(std::vector<Interval>{{0.75L, 1}, {0.25L, 0.5L}});
  reversed.Split(parts, 2, 0);
  INFO("Regions incompatible with radius ordering must be skipped");
  REQUIRE((parts.empty()));
  RadiiCompare compare;
  INFO("Lexicographic upper endpoints");
  REQUIRE((compare({Interval{0, 1}}, {Interval{0, 2}})));
  INFO("Lexicographic lower endpoints");
  REQUIRE((compare({Interval{0, 2}}, {Interval{1, 2}})));
  INFO("Region comparison is strict");
  REQUIRE((!compare({Interval{0, 1}}, {Interval{0, 1}})));
}
