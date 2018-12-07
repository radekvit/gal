/**
 * Project: gal
 * @file main.cpp
 * @date 30. 9. 2018
 * @author xvitra00, xdocek09
 * @brief Benchmark of graph coloring algorithms.
 */
#include <fstream>
#include <iostream>
#include <memory>

#include <benchmark.hpp>
#include "coloring_alg.h"
#include "graph.hpp"

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr << "Usage:\n"
                 "./gal2018 input.txt [input2.txt ...] output.csv\n"
                 "This program produces a csv file with benchmarks for:"
                 "greedy coloring\nlargest degree ordering coloring\nincidence "
                 "degree coloring\n";
    if (argc == 2 && std::string(argv[1]) == "--help") {
      return 0;
    }
    return 1;
  }
  // load graphs from files
  std::vector<std::string> testNames;
  std::vector<ColoredGraph> test;
  for (int i = 1; i < argc - 1; ++i) {
    std::ifstream fileStream(argv[i]);
    if (!fileStream) {
      std::cerr << "Can not open " << argv[i] << " for reading.\n";
      continue;
    }
    test.push_back(ColoredGraph(fileStream));
    testNames.push_back(argv[i]);
  }

  std::cout << "Greedy Coloring\n";
  auto greedyResults = benchmark<GreedyColoring>(test);
  std::cout << "Largest Degree Ordering Coloring\n";
  auto ldocResults = benchmark<LargestDegreeOrderingColoring>(test);
  std::cout << "Incidence Degree Coloring\n";
  auto idcResults = benchmark<IncidenceDegreeColoring>(test);
  // pow(n, 3) complexity, way too large for sensible graphs
  /*std::cout << "SDO + LDO Coloring\n";
  auto sdoLdoResults = benchmark<SdoLdoColoring>(test);*/

  std::ofstream out(argv[argc - 1]);
  if (!out) {
    std::cerr << "Could not open " << argv[argc - 1] << " for writing.\n";
    return 2;
  }
  // dump results
  out << "NAME,GREEDYTIME,GREEDYCOLORS,LDOCTIME,LDOCCOLORS,IDCTIME,IDCCOLORS,"
         "\n";
  for (size_t i = 0; i < testNames.size(); ++i) {
    out << testNames[i] << ',' << greedyResults[i].median << ','
        << greedyResults[i].colorCount << ',' << ldocResults[i].median << ','
        << ldocResults[i].colorCount << ',' << idcResults[i].median << ','
        << idcResults[i].colorCount << "\n";
  }
  return 0;
}
/*** End of file: main.cpp ***/
