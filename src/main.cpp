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
#include "graph.hpp"

int main(int argc, char* argv[]) {
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

  graph.greedyColoring();

  std::cout << graph;

  return 0;
}
/*** End of file: main.cpp ***/
