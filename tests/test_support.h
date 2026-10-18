#pragma once

#include <diskpack/geometry.h>
#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <filesystem>
#include <stdexcept>
#include <string>

namespace test {
inline void Contains(const diskpack::Interval &interval, diskpack::BaseType value,
                     const std::string &message) {
  INFO(message);
  REQUIRE(interval.lower() <= value);
  REQUIRE(value <= interval.upper());
}

template <class Function> void Throws(Function function, const char *message) {
  INFO(message);
  REQUIRE_THROWS(function());
}

class TempDirectory {
public:
  std::filesystem::path path;
  TempDirectory() {
    const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
    for (size_t i = 0; i < 100; ++i) {
      path = std::filesystem::current_path() /
             ("diskpack-test-" + std::to_string(stamp) + "-" + std::to_string(i));
      if (std::filesystem::create_directory(path)) {
        return;
      }
    }
    throw std::runtime_error("Cannot create temporary test directory");
  }
  ~TempDirectory() {
    std::error_code error;
    std::filesystem::remove_all(path, error);
  }
};

} // namespace test
