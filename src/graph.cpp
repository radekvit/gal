/**
 * Project: gal
 * @file graph.cpp
 * @date 1. 10. 2018
 * @author xvitra00, xdocek09
 * @brief Module containing colored graph representation.
 */
#include "graph.hpp"
#include <sstream>
#include <string>

ColoredGraph ColoredGraph::readFrom(std::istream& is) {
  std::string line;
  size_t node = 1;

  std::vector<std::vector<size_t>> graph;

  while (std::getline(is, line)) {
    graph.push_back(std::vector<size_t>());

    std::istringstream lineIss(line);

    size_t edge;
    while (lineIss >> edge)
      graph.back().push_back(edge);

    ++node;
  }

  return ColoredGraph(graph);
}

std::ostream& operator<<(std::ostream& os, const ColoredGraph& g) {
  for (const auto& node : g.nodeTransitions_) {
    auto iterEdges = node.begin();
    if (iterEdges != node.end()) {
      os << *iterEdges++;
    }
    while (iterEdges != node.end()) {
      os << " " << *iterEdges++;
    }
    os << "\n";
  }
  return os;
}

/*** End of file: graph.cpp ***/
