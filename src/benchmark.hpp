#ifndef GAL_BENCHMARK_HPP
#define GAL_BENCHMARK_HPP

#include <chrono>
#include <fstream>
#include <numeric>
#include <string>
#include <vector>

#include "coloring_alg.h"
#include "graph.hpp"

inline constexpr size_t BENCHMARK_ITERATIONS = 1;

struct BenchmarkResult {
  double min = 0.0;
  double max = 0.0;
  double average = 0.0;
  double median = 0.0;

  template <size_t i>
  void set(std::array<double, i>& a) {
    static_assert(i != 0,
                  "Benchmark results must be set with an array larger than 0.");

    std::sort(a.begin(), a.end());
    min = a.front();
    max = a.back();
    average = std::accumulate(a.begin(), a.end(), 0.0) / a.size();
    median = a[a.size() / 2];
  }
};

template <typename CG>
inline std::vector<BenchmarkResult> benchmark(
    const std::vector<std::string>& graphFilenames) {
  using namespace std::chrono;
  std::vector<BenchmarkResult> results;
	results.reserve(graphFilenames.size());

  for (auto&& filename : graphFilenames) {
    results.push_back({});
    auto&& result = results.back();
    std::ifstream is(filename);
    ColoredGraph g(is);
    std::array<double, BENCHMARK_ITERATIONS> timeArray{};

    for (size_t i = 0; i < BENCHMARK_ITERATIONS; ++i) {
      g.clearColors();
      auto begin = steady_clock::now();
      CG::color(g);
      auto end = steady_clock::now();
      double time = duration<double, std::milli>(end - begin).count();
      timeArray[i] = time;
    }
    result.set(timeArray);
  }
	return results;
}

#endif