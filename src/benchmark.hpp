#ifndef GAL_BENCHMARK_HPP
#define GAL_BENCHMARK_HPP

#include <chrono>
#include <fstream>
#include <numeric>
#include <string>
#include <vector>

#include <cassert>
#include "coloring_alg.h"
#include "graph.hpp"

inline constexpr size_t BENCHMARK_ITERATIONS = 5;

struct BenchmarkResult {
  double sum = 0.0;
  double min = 0.0;
  double max = 0.0;
  double average = 0.0;
  double median = 0.0;

  bool resultValid;
  size_t colorCount;

  /**\
   * Set the benchmark results from a range of measurements.
   */
  template <typename Iterator>
  void set(Iterator begin, Iterator end, const ColoredGraph& g) {
    size_t size = end - begin;
    assert(size != 0);

    std::sort(begin, end);
    min = *(end - 1);
    max = *begin;
    sum = std::accumulate(begin, end, 0.0);
    average = sum / size;
    median = *(begin + size / 2);

    resultValid = g.validateColors();
    colorCount = g.colorCount();
  }
};

/**
 * Benchmark a set of graphs given by filenames. The algorithm class is given in
 * the template argument.
 */
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

/**
 * Benchmark a vector of graphs. The algorithm class is given in the tepmlate
 * argument.
 */
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
    if (!g.validateColors()) {
      std::cerr << "error: coloring not valid.\n";
    }
    g.clearColors();
    CG::color(g);
    result.set(timeArray.begin(), timeArray.end(), g);
  }
  return std::move(results);
}

#endif