/**
 * Project: gal
 * @file coloring_alg.hpp
 * @date 3. 10. 2018
 * @author xvitra00, xdocek09
 * @brief Header file of module containing coloring algorithms.
 * Such as:
 * 	Greedy coloring
 * 	Largest Degree Ordering
 * 	Incidence Degree Ordering
 */

#ifndef SRC_COLORING_ALG_HPP_
#define SRC_COLORING_ALG_HPP_

#include <vector>
#include "graph.hpp"

inline constexpr size_t ALGORITHM_COUNT = 4;

/**
 * Is greedy coloring algorithm with adjustable node permutation.
 * Source: http://www.new-npac.org/users/fox/pdftotal/sccs-0666.pdf
 *
 * @param[in|out] graph you want to color.
 * @param[in] nodesPermut	Permutation that defines order of nodes. Ordered
 * sequence of nodes ids.
 */
void greedyColoring(ColoredGraph& graph, const std::vector<size_t> nodesPermut);

/**
 * Color the graph with greedy coloring algorithm.
 *
 *
 * @param[in|out] graph you want to color.
 */
void greedyColoring(ColoredGraph& graph);

/**
 * Color the graph with greedy coloring algorithm.
 *
 * @param[in|out] graph you want to color.
 */
void greedyColoringWithSet(ColoredGraph& graph);

/**
 * Color the graph with largest Degree Ordering algorithm.
 *
 * Source: http://dergipark.gov.tr/download/article-file/254140
 *
 * @param[in|out] graph you want to color.
 */
void largestDegreeOrderingColoring(ColoredGraph& graph);

/**
 * Color the graph with Incidence Degree Ordering algorithm.
 *
 * Source: http://dergipark.gov.tr/download/article-file/254140
 *
 * @param[in|out] graph you want to color.
 */
void incidenceDegreeOrdering(ColoredGraph& graph);

class GreedyColoring {
 public:
  static void color(ColoredGraph& graph) noexcept {
    return greedyColoring(graph);
  }
};

class GreedyColoringWithSet {
 public:
  static void color(ColoredGraph& graph) noexcept {
    return greedyColoringWithSet(graph);
  }
};

class LargestDegreeOrderingColoring {
 public:
  static void color(ColoredGraph& graph) noexcept {
    return largestDegreeOrderingColoring(graph);
  }
};

class IncidenceDegreeColoring {
 public:
  static void color(ColoredGraph& graph) noexcept {
    return incidenceDegreeOrdering(graph);
  }
};

#endif /* SRC_COLORING_ALG_HPP_ */

/*** End of file: coloring_alg.hpp ***/
