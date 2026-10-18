#include "test_support.h"

#include <diskpack/codec.h>

#include <fstream>
#include <sstream>

using namespace diskpack;

TEST_CASE("Region JSON round trips and blank lines", "[codec]") {
  std::vector<RadiiRegion> input{
      RadiiRegion(std::vector<Interval>{{0.125L, 0.25L}, {1, 1}}),
      RadiiRegion(std::vector<Interval>{{0.5L, 0.75L}, {1, 1}})};
  std::stringstream stream(EncodeRegionsJSON(input));
  std::vector<RadiiRegion> decoded;
  DecodeRegionsJSON(stream, decoded);
  INFO("NDJSON must retain every region");
  REQUIRE((decoded.size() == input.size()));
  INFO("Binary-exact endpoints must survive a JSON round trip");
  REQUIRE((EncodeRegionsJSON(decoded) == EncodeRegionsJSON(input)));
  INFO("An empty result has no JSON lines");
  REQUIRE((EncodeRegionsJSON({}).empty()));
  std::stringstream blank("\n\n");
  DecodeRegionsJSON(blank, decoded);
  INFO("Empty input must clear previous results");
  REQUIRE((decoded.empty()));
  std::stringstream spaced("\n[[0.125,0.25],[1,1]]\n\n");
  DecodeRegionsJSON(spaced, decoded);
  INFO("Blank lines must be ignored");
  REQUIRE((decoded.size() == 1));
}

TEST_CASE("Malformed region JSON is rejected", "[codec]") {
  for (const auto &text : {"not json", "{}", "[1]", "[[1]]", "[[1,2,3]]",
                           "[[\"bad\",2]]", "[[null,2]]"}) {
    test::Throws([&] {
      std::stringstream stream(text);
      std::vector<RadiiRegion> regions;
      DecodeRegionsJSON(stream, regions);
    }, "Malformed JSON input must throw");
  }
}

TEST_CASE("SVG output and file errors", "[codec]") {
  test::TempDirectory temporary;
  auto output = temporary.path / "packing.svg";
  std::list<DiskPointer> packing{
      std::make_shared<Disk>(-1, 2, 0.5L, 0),
      std::make_shared<Disk>(3, -4, 1, 1)};
  WritePackingSVG(output.string(), packing, 5);
  std::ifstream file(output);
  std::string svg((std::istreambuf_iterator<char>(file)), {});
  INFO("SVG viewport");
  REQUIRE((svg.find("viewBox=\"-5 -5 10 10\"") != std::string::npos));
  INFO("SVG disk coordinates");
  REQUIRE((svg.find("cx=\"-1\" cy=\"2\" r=\"0.5\"") != std::string::npos));
  INFO("SVG colors by disk type");
  REQUIRE((svg.find("#ff8080a0") != std::string::npos &&
        svg.find("#aa0087a0") != std::string::npos));
  INFO("SVG Cartesian orientation");
  REQUIRE((svg.find("scale(1, -1)") != std::string::npos));
  INFO("SVG closing element");
  REQUIRE((svg.find("</svg>") != std::string::npos));
  auto missing = temporary.path / "missing" / "packing.svg";
  WritePackingSVG(missing.string(), packing, 5);
  INFO("Failed output must not create a file");
  REQUIRE((!std::filesystem::exists(missing)));
  WritePackingSVG(output.string(), {}, 1);
  std::ifstream empty_file(output);
  svg.assign(std::istreambuf_iterator<char>(empty_file), {});
  INFO("An empty packing has no circles");
  REQUIRE((svg.find("<circle") == std::string::npos));
}
