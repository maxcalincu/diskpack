#include "test_support.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <nlohmann/json.hpp>

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <sys/wait.h>

namespace {
std::string Quote(const std::string &value) {
  std::string result = "'";
  for (auto character : value) {
    result += character == '\'' ? "'\"'\"'" : std::string(1, character);
  }
  return result + "'";
}

std::string Read(const std::filesystem::path &path) {
  std::ifstream input(path);
  return {std::istreambuf_iterator<char>(input), {}};
}

void Write(const std::filesystem::path &path, const std::string &contents) {
  std::ofstream output(path);
  output << contents;
  REQUIRE(output.good());
}

struct Result {
  int code;
  std::string output, error;
};

Result Run(const test::TempDirectory &directory, const char *executable,
           const std::vector<std::string> &arguments) {
  auto command = "cd " + Quote(directory.path.string()) + " && " + Quote(executable);
  for (const auto &argument : arguments) {
    command += " " + Quote(argument);
  }
  command += " > stdout.txt 2> stderr.txt";
  const auto status = std::system(command.c_str());
  auto code = status >= 0 && WIFEXITED(status) ? WEXITSTATUS(status) : -1;
  return {code, Read(directory.path / "stdout.txt"), Read(directory.path / "stderr.txt")};
}

void Success(const Result &result) {
  INFO(result.output);
  INFO(result.error);
  REQUIRE(result.code == 0);
}

void Error(const Result &result, const std::string &message) {
  INFO(result.output);
  INFO(result.error);
  REQUIRE(result.code == 1);
  REQUIRE(result.error.find(message) != std::string::npos);
}

std::vector<nlohmann::json> Regions(const std::string &text) {
  std::vector<nlohmann::json> result;
  std::istringstream input(text);
  std::string line;
  while (std::getline(input, line)) {
    if (line.find_first_not_of(" \t\r") != std::string::npos) {
      result.push_back(nlohmann::json::parse(line));
    }
  }
  return result;
}

void ValidSvg(const std::filesystem::path &path) {
  boost::property_tree::ptree document;
  REQUIRE_NOTHROW(boost::property_tree::read_xml(path.string(), document));
  const auto &svg = document.get_child("svg");
  REQUIRE(svg.get<std::string>("<xmlattr>.xmlns") == "http://www.w3.org/2000/svg");
  const auto &group = svg.get_child("g");
  size_t circles = 0;
  for (const auto &element : group) {
    if (element.first != "circle") {
      continue;
    }
    ++circles;
    REQUIRE(std::isfinite(element.second.get<double>("<xmlattr>.cx")));
    REQUIRE(std::isfinite(element.second.get<double>("<xmlattr>.cy")));
    REQUIRE(element.second.get<double>("<xmlattr>.r") > 0);
    REQUIRE_FALSE(element.second.get<std::string>("<xmlattr>.fill").empty());
  }
  REQUIRE(circles >= 2);
}
} // namespace

TEST_CASE("Both command line tools display help", "[cli]") {
  test::TempDirectory temporary;
  for (const auto *tool : {DISKPACK_FINDER_PATH, DISKPACK_VISUALIZER_PATH}) {
    auto result = Run(temporary, tool, {"--help"});
    Success(result);
    REQUIRE(result.output.find("Usage") != std::string::npos);
  }
}

TEST_CASE("Both command line tools reject invalid options", "[cli]") {
  test::TempDirectory temporary;
  for (const auto *tool : {DISKPACK_FINDER_PATH, DISKPACK_VISUALIZER_PATH}) {
    Error(Run(temporary, tool, {"--not-an-option"}), "Error:");
  }
  Error(Run(temporary, DISKPACK_VISUALIZER_PATH, {"--i2", "4", "-n", "bad"}), "Error:");
}

TEST_CASE("Both command line tools diagnose missing and malformed input", "[cli]") {
  test::TempDirectory temporary;
  Write(temporary.path / "malformed.ndjson", "not json\n");
  Write(temporary.path / "multiple.ndjson", "[[1,1]]\n[[1,1]]\n");
  for (const auto *tool : {DISKPACK_FINDER_PATH, DISKPACK_VISUALIZER_PATH}) {
    Error(Run(temporary, tool, {"-i", "missing.ndjson"}), "Failed to open file");
    Error(Run(temporary, tool, {"-i", "malformed.ndjson"}), "JSON parse error");
    Error(Run(temporary, tool, {"-i", "multiple.ndjson"}), "Exactly one region");
  }
}

TEST_CASE("Visualizer validates its radius selectors", "[cli]") {
  test::TempDirectory temporary;
  Error(Run(temporary, DISKPACK_VISUALIZER_PATH, {}), "Exactly one flag");
  Error(Run(temporary, DISKPACK_VISUALIZER_PATH, {"--i2", "4", "--i3", "1"}),
        "Exactly one flag");
  for (const auto &selection : {std::vector<std::string>{"--i2", "0"},
                               {"--i2", "10"}, {"--i3", "0"}, {"--i3", "165"}}) {
    Error(Run(temporary, DISKPACK_VISUALIZER_PATH, selection), "constraint violated");
  }
}

TEST_CASE("Visualizer produces valid SVG for binary and ternary examples", "[cli]") {
  test::TempDirectory temporary;
  for (const auto &selection : {std::vector<std::string>{"--i2", "4"}, {"--i3", "53"}}) {
    auto arguments = selection;
    arguments.insert(arguments.end(), {"-r", "3", "-n", "30", "-c", "0",
                                       "-o", "packing with spaces.svg"});
    auto result = Run(temporary, DISKPACK_VISUALIZER_PATH, arguments);
    Success(result);
    REQUIRE(result.output.find("complete") != std::string::npos);
    ValidSvg(temporary.path / "packing with spaces.svg");
  }
}

TEST_CASE("Visualizer writes partial output on a precision stop", "[cli]") {
  test::TempDirectory temporary;
  auto result = Run(temporary, DISKPACK_VISUALIZER_PATH,
                    {"--i2", "4", "-p", "0.000000000000001", "-o", "partial.svg"});
  Success(result);
  REQUIRE(result.output.find("precision_error") != std::string::npos);
  ValidSvg(temporary.path / "partial.svg");
}

TEST_CASE("Visualizer reads JSON and preserves output for invalid radii", "[cli]") {
  test::TempDirectory temporary;
  Write(temporary.path / "radii.ndjson", "[[1,1]]\n");
  auto result = Run(temporary, DISKPACK_VISUALIZER_PATH,
                    {"-i", "radii.ndjson", "-r", "2", "-n", "15", "-o", "packing.svg"});
  Success(result);
  REQUIRE(result.output.find("complete") != std::string::npos);
  ValidSvg(temporary.path / "packing.svg");
  Write(temporary.path / "radii.ndjson", "[[0.6,0.6],[1,1]]\n");
  Write(temporary.path / "packing.svg", "previous output");
  result = Run(temporary, DISKPACK_VISUALIZER_PATH,
               {"-i", "radii.ndjson", "-r", "2", "-n", "15", "-o", "packing.svg"});
  Success(result);
  REQUIRE(result.output.find("invalid") != std::string::npos);
  REQUIRE(Read(temporary.path / "packing.svg") == "previous output");
}

TEST_CASE("Finder retains a known root and rejects impossible radii", "[cli]") {
  test::TempDirectory temporary;
  Write(temporary.path / "region.ndjson", "[[0.414213562,0.414213563],[1,1]]\n");
  auto result = Run(temporary, DISKPACK_FINDER_PATH,
                    {"-i", "region.ndjson", "-k", "1", "-l", "0.0000001"});
  Success(result);
  auto regions = Regions(result.output);
  REQUIRE(regions.size() == 1);
  const auto root = std::sqrt(2.0) - 1;
  REQUIRE(regions[0][0][0].get<double>() <= root);
  REQUIRE(regions[0][0][1].get<double>() >= root);
  Write(temporary.path / "region.ndjson", "[[0.6,0.6],[1,1]]\n");
  result = Run(temporary, DISKPACK_FINDER_PATH, {"-i", "region.ndjson", "-k", "1"});
  Success(result);
  REQUIRE(Regions(result.output).empty());
}

TEST_CASE("Finder output can be passed directly to the visualizer", "[cli][integration]") {
  test::TempDirectory temporary;
  Write(temporary.path / "region.ndjson", "[[0.414213562,0.414213563],[1,1]]\n");
  Success(Run(temporary, DISKPACK_FINDER_PATH,
              {"-i", "region.ndjson", "-k", "1", "-o", "found.ndjson"}));
  REQUIRE(Regions(Read(temporary.path / "found.ndjson")).size() == 1);
  auto result = Run(temporary, DISKPACK_VISUALIZER_PATH,
                    {"-i", "found.ndjson", "-r", "2", "-n", "15", "-c", "0",
                     "-o", "packing.svg"});
  Success(result);
  REQUIRE(result.output.find("complete") != std::string::npos);
  ValidSvg(temporary.path / "packing.svg");
}

TEST_CASE("Output file errors are reported by both tools", "[cli]") {
  test::TempDirectory temporary;
  Write(temporary.path / "region.ndjson", "[[0.414213562,0.414213563],[1,1]]\n");
  auto result = Run(temporary, DISKPACK_FINDER_PATH,
                    {"-i", "region.ndjson", "-k", "1", "-o", "missing/result.ndjson"});
  Success(result);
  REQUIRE(result.error.find("Failed to open file") != std::string::npos);
  REQUIRE(Regions(result.output).size() == 1);
  result = Run(temporary, DISKPACK_VISUALIZER_PATH,
               {"--i2", "4", "-r", "2", "-n", "15", "-o", "missing/packing.svg"});
  Success(result);
  REQUIRE(result.error.find("Error opening file") != std::string::npos);
  REQUIRE_FALSE(std::filesystem::exists(temporary.path / "missing/packing.svg"));
}
