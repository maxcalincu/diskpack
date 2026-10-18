#include "test_support.h"

#include <diskpack/constants.h>
#include <diskpack/generator.h>

#include <cmath>
#include <sstream>

using namespace diskpack;

namespace {
class TestGenerator : public diskpack::BasicGenerator {
public:
  TestGenerator(const std::vector<Interval> &radii = {Interval{1}},
                BaseType radius = 3, BaseType precision = 0.2L,
                size_t size = 100, size_t ignored = 0)
      : BasicGenerator(radii, radius, precision, size, ignored) {
    g.seed(42);
  }

  using BasicGenerator::AdvancePacking;
  using BasicGenerator::GapFill;
  using BasicGenerator::HasIntersection;
  using BasicGenerator::IsInBounds;
  using BasicGenerator::PackingIsLargeEnough;
  using BasicGenerator::PackingSatisfiesConstraints;
  using BasicGenerator::Pop;
  using BasicGenerator::Push;
  void DropQueue() { disk_queue.clear(); }

  bool QueueHasOnly(const diskpack::DiskPointer &disk) const {
    return disk_queue.size() == 1 && *disk_queue.begin() == disk;
  }

  bool IsEmpty() const {
    return packing.empty() && disk_queue.empty() && frequency_table[0] == 0;
  }
};
} // namespace

TEST_CASE("Pop preserves other disks with equal priority", "[generator]") {
  TestGenerator generator;
  generator.Push(diskpack::Disk{2, 0, 1, 0}, 0);
  const auto first_disk = generator.GetPacking().back();
  generator.Push(diskpack::Disk{-2, 0, 1, 0}, 0);

  // Both disks have the same priority, but popping the second must preserve
  // the first disk and its queue entry.
  generator.Pop(0);
  REQUIRE(generator.GetPacking().size() == 1);
  REQUIRE(generator.GetPacking().back() == first_disk);
  REQUIRE(generator.QueueHasOnly(first_disk));

  generator.Pop(0);
  REQUIRE(generator.IsEmpty());
}

void VerifyPacking(BasicGenerator &generator, size_t types) {
  const auto &packing = generator.GetPacking();
  INFO("Generated packing extent");
  REQUIRE((!packing.empty() && generator.GetGeneratedRadius() > 0));
  std::vector<bool> seen(types, false);
  for (auto a = packing.begin(); a != packing.end(); ++a) {
    INFO("A disk must reference an input radius");
    REQUIRE(((*a)->get_type() < types));
    seen[(*a)->get_type()] = true;
    INFO("Finite positive disk geometry");
    REQUIRE(((*a)->get_radius().lower() > 0 &&
          std::isfinite((*a)->get_center_x().lower()) &&
          std::isfinite((*a)->get_center_y().upper())));
    for (auto b = std::next(a); b != packing.end(); ++b) {
      INFO("A packing cannot contain a certain overlap");
      REQUIRE((!(*a)->intersects(**b)));
    }
  }
  INFO("All requested radius types must occur");
  REQUIRE((std::all_of(seen.begin(), seen.end(), [](bool value) { return value; })));
}

TEST_CASE("Generate known packings and reject impossible radii", "[generator]") {
  for (const auto &radii : std::vector<std::vector<Interval>>{
           {one}, {two_radii[3], one},
           {three_radii[0].second, three_radii[0].first, one}}) {
    TestGenerator generator(radii, 3, 0.2L, 30);
    INFO("Known packing must generate");
    REQUIRE((generator.Generate(0) == complete));
    VerifyPacking(generator, radii.size());
  }
  TestGenerator impossible({Interval{0.6L}, one});
  INFO("An impossible binary ratio must be rejected");
  REQUIRE((impossible.Generate(0) == invalid));
}

TEST_CASE("Generator resume reset and uniform scaling", "[generator]") {
  TestGenerator generator({one}, 2, 0.2L, 7);
  INFO("An unstarted generator cannot resume");
  REQUIRE((generator.Resume() == invalid));
  INFO("Initial bounded generation");
  REQUIRE((generator.Generate(0) == complete));
  auto first_size = generator.GetPacking().size();
  generator.SetPackingRadius(4);
  generator.SetSizeUpperBound(100);
  INFO("Generation must resume after increasing limits");
  REQUIRE((generator.GetRadius() == 4 && generator.Resume() == complete));
  INFO("Resume must extend the packing");
  REQUIRE((generator.GetPacking().size() > first_size));
  VerifyPacking(generator, 1);
  generator.Reset();
  INFO("Reset clears all state");
  REQUIRE((generator.IsEmpty() && generator.GetGeneratedRadius() == 0));
  generator.SetRadii({Interval{2}});
  INFO("Uniformly scaled radii can be generated");
  REQUIRE((generator.Generate(0) == complete));
  INFO("SetRadii must update subsequent disks");
  REQUIRE((generator.GetPacking().front()->get_radius().lower() == 2));
}

TEST_CASE("Generator statuses bounds and frequency constraints", "[generator]") {
  TestGenerator generator;
  INFO("Empty processing queue");
  REQUIRE((generator.AdvancePacking() == invalid));
  generator.Push(Disk{Interval{-1, 1}, Interval{-1, 1}, 1, 0}, 0);
  INFO("Both wide coordinates exceed the bound");
  REQUIRE((generator.AdvancePacking() == precision_error));
  generator.Reset();
  generator.Push(Disk{0, 0, 1, 0}, 0);
  INFO("Intersection with existing packing");
  REQUIRE((generator.HasIntersection(Disk{0.5L, 0, 1, 0})));
  INFO("Separated candidate");
  REQUIRE((!generator.HasIntersection(Disk{4, 0, 1, 0})));
  INFO("Region boundary");
  REQUIRE((generator.IsInBounds(Disk{0, 0, 1, 0}) &&
        !generator.IsInBounds(Disk{4, 0, 1, 0})));
  INFO("The one radius is represented");
  REQUIRE((generator.PackingSatisfiesConstraints()));
  generator.SetSizeUpperBound(1);
  INFO("Disk count limit");
  REQUIRE((generator.PackingIsLargeEnough()));
  generator.DropQueue();
  test::Throws([&] { generator.Pop(0); }, "Missing queue entry must be diagnosed");
  INFO("Failed Pop must retain packing state");
  REQUIRE((generator.GetPacking().size() == 1));
  TestGenerator mixed({Interval{1}, Interval{1}}, 3, 0.2L, 100, 1);
  INFO("Too many missing radius types");
  REQUIRE((!mixed.PackingSatisfiesConstraints()));
  mixed.Push(Disk{0, 0, 1, 0}, 0);
  INFO("Configured missing-radius allowance");
  REQUIRE((mixed.PackingSatisfiesConstraints()));
  const std::vector<std::pair<PackingStatus, const char *>> names{
      {complete, "complete"}, {invalid, "invalid"},
      {precision_error, "precision_error"}, {corona_error, "corona_error"}};
  for (const auto &entry : names) {
    std::ostringstream output;
    output << entry.first;
    INFO("Stable status names");
    REQUIRE((output.str() == entry.second));
  }
}

TEST_CASE("Generator rejects blocked gap without mutation", "[generator]") {
  TestGenerator generator;
  Disk base{0, 0, 1, 0};
  OperatorLookupTable lookup({Interval{1}});
  std::list<DiskPointer> neighbors{std::make_shared<Disk>(2, 0, 1, 0)};
  Corona corona(base, neighbors, lookup);
  Disk candidate;
  INFO("Initial candidate");
  REQUIRE((corona.PeekNewDisk(candidate, 0)));
  generator.Push(Disk{candidate}, 0);
  INFO("A blocked continuation must be rejected");
  REQUIRE((generator.GapFill(corona) == invalid));
  INFO("Rejected continuation must not add disks");
  REQUIRE((generator.GetPacking().size() == 1));
}
