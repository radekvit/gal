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
#if 0
  std::istream* input = &std::cin;  // we register cin as default input
  std::ifstream fileStream;

  if (argc > 2) {
    std::cerr << "This program is expecting at most one argument with path to "
                 "file containing graph. If you run without arguments, than "
                 "graph is expected on stdin."
              << std::endl;
    return 1;
  } else if (argc == 2) {
    // user wants to read graph from given file
    fileStream.open(argv[1]);
    if (fileStream) {
      input = &fileStream;
    } else {
      std::cerr << "Can not open " << argv[1] << " for reading." << std::endl;
      return 2;
    }
  }

  ColoredGraph graph(*input);

  greedyColoring(graph);
  std::cout << graph;
  std::cout << "Valid: " << graph.validateColors() << std::endl;
  graph.clearColors();
  largestDegreeOrderingColoring(graph);
  std::cout << graph;
  std::cout << "Valid: " << graph.validateColors() << std::endl;
  graph.clearColors();
  incidenceDegreeOrdering(graph);
  std::cout << graph;
  std::cout << "Valid: " << graph.validateColors() << std::endl;
#endif
  std::vector<ColoredGraph> test{
      {100, 0.5}, {200, 0.5}, {300, 0.5}, {400, 0.5}, {1000, 0.5}};

  std::cout << "Greedy Coloring\n";
  for (auto&& b : benchmark<GreedyColoring>(test)) {
    std::cout << "----\nresult valid:     " << b.resultValid
              << "\nnumber of colors: " << b.colorCount
              << "\n----\nsum:     " << b.sum << "\nmin:     " << b.min
              << "\nmax:     " << b.max << "\naverage: " << b.average
              << "\nmedian:  " << b.median << "\n";
  }
  std::cout << "Largest Degree Ordering Coloring\n";
  for (auto&& b : benchmark<LargestDegreeOrderingColoring>(test)) {
    std::cout << "----\nresult valid:     " << b.resultValid
              << "\nnumber of colors: " << b.colorCount
              << "\n----\nsum:     " << b.sum << "\nmin:     " << b.min
              << "\nmax:     " << b.max << "\naverage: " << b.average
              << "\nmedian:  " << b.median << "\n";
  }
  std::cout << "Incidence Degree Coloring\n";
  for (auto&& b : benchmark<IncidenceDegreeColoring>(test)) {
    std::cout << "----\nresult valid:     " << b.resultValid
              << "\nnumber of colors: " << b.colorCount
              << "\n----\nsum:     " << b.sum << "\nmin:     " << b.min
              << "\nmax:     " << b.max << "\naverage: " << b.average
              << "\nmedian:  " << b.median << "\n";
  }

  return 0;
}
/*** End of file: main.cpp ***/
