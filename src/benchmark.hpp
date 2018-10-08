#ifndef GAL_BENCHMARK_HPP
#define GAL_BENCHMARK_HPP

#include <chrono>
#include <fstream>
#include <numeric>
#include <string>
#include <vector>

#include "coloring_alg.h"
#include "graph.hpp"

inline constexpr size_t BENCHMARK_ITERATIONS = 51;

struct BenchmarkResult {
  double sum = 0.0;
  double min = 0.0;
  double max = 0.0;
  double average = 0.0;
  double median = 0.0;

  bool resultValid;
  size_t colorCount;

  template <size_t i>
  void set(std::array<double, i>& a, const ColoredGraph& g) {
    static_assert(i != 0,
                  "Benchmark results must be set with an array larger than 0.");

    std::sort(a.begin(), a.end());
    min = a.front();
    max = a.back();
    sum = std::accumulate(a.begin(), a.end(), 0.0);
    average = sum / a.size();
    median = a[a.size() / 2];

    resultValid = g.validateColors();
    colorCount = g.colorCount();
  }
};

template <typename CG>
inline std::vector<BenchmarkResult> benchmark(
    const std::vector<std::string>& graphFilenames) {
  std::vector<ColoredGraph> graphs{};

  for (auto&& filename : graphFilenames) {
    std::ifstream is(filename);
    graphs.push_back({is});
  }

  return std::move(benchmark<CG>(std::move(graphs)));
}

template <typename CG>
inline std::vector<BenchmarkResult> benchmark(
    std::vector<ColoredGraph> graphs) {
  using namespace std::chrono;
  std::vector<BenchmarkResult> results;
  std::array<double, BENCHMARK_ITERATIONS> timeArray{};
  results.reserve(graphs.size());

  for (auto&& g : graphs) {
    results.push_back({});
    auto&& result = results.back();

    for (size_t i = 0; i < BENCHMARK_ITERATIONS; ++i) {
      g.clearColors();
      auto begin = steady_clock::now();
      CG::color(g);
      auto end = steady_clock::now();
      double time = duration<double, std::milli>(end - begin).count();
      timeArray[i] = time;
    }
    g.clearColors();
    CG::color(g);
    result.set(timeArray, g);
  }
  return std::move(results);
}

#endif