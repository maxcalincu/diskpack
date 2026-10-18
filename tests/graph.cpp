#include "test_support.h"

#include <diskpack/corona.h>

#include <array>
#include <cmath>
#include <iostream>
#include <sstream>

using namespace diskpack;

std::vector<bool> Triangles(const ConnectivityGraph &graph, size_t count) {
  std::vector<bool> result;
  for (size_t i = 0; i < count; ++i) {
    for (size_t j = 0; j < count; ++j) {
      for (size_t k = 0; k < count; ++k) {
        result.push_back(graph.HasTriangle(i, j, k));
        INFO("Triangle compatibility must be symmetric");
        REQUIRE((graph.HasTriangle(i, j, k) == graph.HasTriangle(k, i, j)));
      }
    }
  }
  return result;
}

TEST_CASE("Connectivity graph refinements restore exact state", "[graph]") {
  OperatorLookupTable initial({Interval{0.414L, 0.415L}, one});
  ConnectivityGraph graph(initial);
  INFO("Known binary packing is viable");
  REQUIRE((!graph.HasOverflow() && graph.IsViable()));
  const auto initial_size = graph.Size();
  const auto initial_triangles = Triangles(graph, 2);
  INFO("No revision exists before refinement");
  REQUIRE((initial_size > 0 && !graph.Restore()));

  OperatorLookupTable narrow({Interval{0.4142135L, 0.4142137L}, one});
  graph.Refine(narrow);
  INFO("Narrowing keeps the known root");
  REQUIRE((graph.IsViable() && graph.Size() <= initial_size));
  const auto narrow_size = graph.Size();
  const auto narrow_triangles = Triangles(graph, 2);

  OperatorLookupTable excluded({Interval{0.4149L, 0.41491L}, one});
  graph.Refine(excluded);
  INFO("A region excluding the root must be rejected");
  REQUIRE((!graph.IsViable()));
  INFO("Inner refinement must restore");
  REQUIRE((graph.Restore()));
  INFO("Inner restore must recover signatures and edges");
  REQUIRE((graph.Size() == narrow_size && Triangles(graph, 2) == narrow_triangles &&
        graph.IsViable()));
  INFO("Outer refinement must restore");
  REQUIRE((graph.Restore()));
  INFO("Outer restore must recover the original graph");
  REQUIRE((graph.Size() == initial_size && Triangles(graph, 2) == initial_triangles));
  INFO("All saved revisions must have been consumed");
  REQUIRE((!graph.Restore()));
}

TEST_CASE("Connectivity graph viability and triangles", "[graph]") {
  OperatorLookupTable valid({Interval{1}}), invalid({Interval{0.6L}, one});
  ConnectivityGraph hexagonal(valid), impossible(invalid);
  INFO("Monodisperse packing");
  REQUIRE((!hexagonal.HasOverflow() && hexagonal.IsViable()));
  INFO("Hexagonal contact triangle");
  REQUIRE((hexagonal.HasTriangle(0, 0, 0)));
  INFO("Impossible binary ratio");
  REQUIRE((!impossible.HasOverflow() && !impossible.IsViable()));
  std::ostringstream captured;
  auto *previous = std::cerr.rdbuf(captured.rdbuf());
  hexagonal.DisplaySignatures();
  std::cerr.rdbuf(previous);
  INFO("Graph signature diagnostics");
  REQUIRE((!captured.str().empty()));
}

TEST_CASE("Corona construction signatures and backtracking", "[graph]") {
  Disk base{0, 0, 1, 0};
  OperatorLookupTable lookup({Interval{1}});
  std::list<DiskPointer> packing{std::make_shared<Disk>(2, 0, 1, 0)};
  Corona corona(base, packing, lookup);
  INFO("Initial corona");
  REQUIRE((&corona.GetBase() == &base && corona.IsContinuous()));
  test::Throws([&] { CoronaSignature incomplete(corona); },
               "An incomplete corona has no signature");
  for (size_t i = 0; i < 5; ++i) {
    Disk next;
    INFO("A regular corona must accept another disk");
    REQUIRE((corona.PeekNewDisk(next, 0)));
    INFO("Constructed tangencies");
    REQUIRE((base.tangent(next) &&
          std::any_of(packing.begin(), packing.end(),
                      [&](const auto &disk) { return disk->tangent(next); })));
    packing.push_back(std::make_shared<Disk>(next));
    corona.Push(packing.back(), 0);
  }
  INFO("Six unit neighbors must close");
  REQUIRE((corona.IsCompleted()));
  CoronaSignature signature(corona);
  INFO("Completed corona signature");
  REQUIRE((signature.GetBase() == 0 && signature.specimen_indexes.size() == 6 &&
        signature.disks[0]));
  INFO("A signature must accept its original radii");
  REQUIRE((signature.TestRadii(lookup)));
  OperatorLookupTable scaled({Interval{2}});
  INFO("Uniform scaling must preserve the signature");
  REQUIRE((signature.TestRadii(scaled)));
  CoronaSignature equivalent(corona);
  INFO("Signature equivalence");
  REQUIRE((signature == equivalent && !(signature < equivalent)));
  auto a = std::make_shared<CoronaSignature>(signature);
  auto b = std::make_shared<CoronaSignature>(equivalent);
  INFO("Pointer comparison uses signatures");
  REQUIRE((!CoronaSignaturePointerCompare(a, b)));
  Disk removed = *packing.back();
  corona.Pop();
  packing.pop_back();
  INFO("Pop must restore the open arc");
  REQUIRE((!corona.IsCompleted() && corona.IsContinuous()));
  Disk repeated;
  INFO("A popped step can be replayed");
  REQUIRE((corona.PeekNewDisk(repeated, 0)));
  INFO("Replayed coordinates must agree");
  REQUIRE((!empty(intersect(repeated.get_center_x(), removed.get_center_x())) &&
        !empty(intersect(repeated.get_center_y(), removed.get_center_y()))));
}

TEST_CASE("Empty and discontinuous coronas are unresolved", "[graph]") {
  Disk base{0, 0, 1, 0};
  OperatorLookupTable lookup({Interval{1}});
  Corona empty_corona(base, {}, lookup);
  Disk candidate;
  INFO("An empty corona must remain unresolved");
  REQUIRE((!empty_corona.IsContinuous() && !empty_corona.IsCompleted() &&
        !empty_corona.PeekNewDisk(candidate, 0)));
  std::list<DiskPointer> separated{
      std::make_shared<Disk>(2, 0, 1, 0), std::make_shared<Disk>(-2, 0, 1, 0)};
  Corona disconnected(base, separated, lookup);
  INFO("Separated neighbors do not form one open arc");
  REQUIRE((!disconnected.IsContinuous()));
}
