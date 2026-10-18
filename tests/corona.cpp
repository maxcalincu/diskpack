#include "test_support.h"

#include <diskpack/generator.h>

#include <algorithm>
#include <array>
#include <cmath>

using namespace diskpack;

namespace {
DiskPointer Neighbor(BaseType x, BaseType y) {
  const BaseType padding = 1e-8L;
  return std::make_shared<Disk>(Interval{x - padding, x + padding},
                                Interval{y - padding, y + padding}, 1, 0);
}

std::vector<DiskPointer> UnitRing() {
  const auto root3 = std::sqrt(3.0L);
  return {Neighbor(2, 0), Neighbor(1, root3), Neighbor(-1, root3),
          Neighbor(-2, 0), Neighbor(-1, -root3), Neighbor(1, -root3)};
}

class AmbiguousGenerator : public BasicGenerator {
public:
  AmbiguousGenerator() : BasicGenerator({Interval{1}}, 10, 0.2, 100) {}

  PackingStatus GenerateAmbiguousCorona() {
    Reset();
    Push(Disk{0, 0, 1, 0}, 0);
    Push(Disk{Interval{1.8L, 2.2L}, Interval{-0.1L, 0.1L}, 1, 0}, 0);
    Push(Disk{Interval{1.8L, 2.2L}, Interval{-0.1L, 0.1L}, 1, 0}, 0);
    return AdvancePacking();
  }
};
} // namespace

TEST_CASE("Angular comparison is a strict weak ordering", "[corona][regression]") {
  Disk base{0, 0, 1, 0};
  DiskCounterclockwiseCompare compare{base};
  std::vector<DiskPointer> points;
  for (auto coordinates : std::vector<std::pair<BaseType, BaseType>>{
           {2, 0}, {1, 1}, {0, 2}, {-1, 1}, {-2, 0},
           {-1, -1}, {0, -2}, {1, -1}, {4, 0}}) {
    points.push_back(std::make_shared<Disk>(coordinates.first,
                                           coordinates.second, 1, 0));
  }
  auto equivalent = [&](const DiskPointer &a, const DiskPointer &b) {
    return !compare(a, b) && !compare(b, a);
  };
  for (const auto &a : points) {
    REQUIRE_FALSE(compare(a, a));
    for (const auto &b : points) {
      REQUIRE_FALSE((compare(a, b) && compare(b, a)));
      for (const auto &c : points) {
        REQUIRE((!(compare(a, b) && compare(b, c)) || compare(a, c)));
        REQUIRE((!(equivalent(a, b) && equivalent(b, c)) || equivalent(a, c)));
      }
    }
  }
}

TEST_CASE("Every permutation of a hexagonal corona closes", "[corona][regression]") {
  Disk base{0, 0, 1, 0};
  OperatorLookupTable lookup({Interval{1}});
  auto ring = UnitRing();
  std::array<size_t, 6> order{0, 1, 2, 3, 4, 5};
  do {
    CAPTURE(order);
    std::list<DiskPointer> packing;
    for (auto i : order) {
      packing.push_back(ring[i]);
    }
    Corona corona(base, packing, lookup);
    REQUIRE(corona.IsContinuous());
    REQUIRE(corona.IsCompleted());
  } while (std::next_permutation(order.begin(), order.end()));
}

TEST_CASE("Angular cut does not make a separated cyclic order ambiguous", "[corona]") {
  Disk base{0, 0, 1, 0};
  OperatorLookupTable lookup({Interval{1}});
  auto ring = UnitRing();
  Corona partial(base, {ring[5], ring[0], ring[1]}, lookup);
  REQUIRE(partial.IsContinuous());
  REQUIRE_FALSE(partial.IsCompleted());
}

TEST_CASE("Unresolved corona order is propagated without rejecting radii", "[corona]") {
  AmbiguousGenerator generator;
  REQUIRE(generator.GenerateAmbiguousCorona() == corona_error);
  OperatorLookupTable tiny_lookup({Interval{1e-12L}});
  ConnectivityGraph uncertain_graph(tiny_lookup);
  REQUIRE(uncertain_graph.HasOverflow());
}
