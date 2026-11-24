#pragma once
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>
#include "valtcommand/runtime.hpp"

namespace tests
{
  class NullOutputBuffer final : public std::streambuf
  {
    int overflow(int character) override { return character; }
  };

  struct Suite
  {
    std::string name;
    int passed = 0;
    int failed = 0;
    int skipped = 0;
  };

  inline std::vector<Suite> &suites()
  {
    static std::vector<Suite> all;
    return all;
  }

  inline Suite &suite(const char *name)
  {
    suites().push_back({name});
    return suites().back();
  }

  inline void check(Suite &result, const std::string &name, bool ok,
                    const std::string &expected, const std::string &actual)
  {
    if (ok)
    {
      ++result.passed;
      return;
    }
    ++result.failed;
    std::cerr << "FAIL [" << result.name << "] " << name
              << "\n  expected: " << expected
              << "\n  actual:   " << actual << '\n';
  }

  // Compare rounded values so platform-specific long-double precision is harmless.
  inline bool approx_equal(long double value, long double expected, int digits = 8)
  {
    const auto scale = std::pow(10.0L, digits);
    return std::isfinite(value) && std::round(value * scale) == std::round(expected * scale);
  }

  inline std::string describe(const vltc::Runtime::RunResult &result)
  {
    return result.type + ":" + result.value;
  }

  // Each case gets a clean runtime; assertions inspect the variable through Runtime API.
  inline void run_ok(Suite &result, vltc::Runtime::RunTime &runtime,
                     const std::string &name, const std::string &code,
                     const std::string &variable, const std::string &type,
                     const std::string &expected)
  {
    runtime.reset();
    const auto run = runtime.run(code);
    if (!run)
    {
      check(result, name, false, "success / " + type + ":" + expected,
            run.error.type + ": " + run.error.message);
      return;
    }

    const auto actual = runtime.get_variable(variable);
    bool matches = actual && actual.type == type;
    if (matches && type == "float")
    {
      try
      {
        matches = approx_equal(std::stold(actual.value), std::stold(expected));
      }
      catch (...)
      {
        matches = false;
      }
    }
    else if (matches)
    {
      matches = actual.value == expected;
    }
    check(result, name, matches, type + ":" + expected,
          actual ? describe(actual) : actual.error.message);
  }

  // Match the public RunResult error and its detailed ErrorInfo source position.
  inline void error(Suite &result, vltc::Runtime::RunTime &runtime,
                    const std::string &name, const std::string &code,
                    const std::string &type, const std::string &message_fragment,
                    int expected_line = -1, int expected_column = -1,
                    vltc::Runtime::ErrorStage expected_stage = vltc::Runtime::ErrorStage::None)
  {
    const auto run = runtime.run(code, vltc::Runtime::ScopeMode::Fresh);
    const bool has_error = !run;
    const auto &error = run.error;
    const bool line_matches = expected_line < 0 || error.info.line == static_cast<std::size_t>(expected_line);
    const bool column_matches = expected_column < 0 || error.info.column == static_cast<std::size_t>(expected_column);
    const bool stage_matches = expected_stage == vltc::Runtime::ErrorStage::None || error.stage == expected_stage;
    const bool detail_matches = error.info.message.find(message_fragment) != std::string::npos &&
                                error.info.file.empty();
    const bool matches = has_error && error.type == type &&
        error.info.type == type &&
                         error.message.find(message_fragment) != std::string::npos &&
                         line_matches && column_matches && stage_matches && detail_matches;

    std::string expected = type + " containing '" + message_fragment + "'";
    if (expected_line >= 0)
      expected += " at " + std::to_string(expected_line);
    if (expected_column >= 0)
      expected += ":" + std::to_string(expected_column);
    const std::string actual = has_error
                                   ? error.type + " / " + error.message + " @" +
                                         std::to_string(error.info.line) + ":" + std::to_string(error.info.column)
                                   : "success";
    check(result, name, matches, expected, actual);
  }

  void test_results();
  void test_functions();
  void test_errors();
void test_documented_examples();
  void test_runtime_api();
}
