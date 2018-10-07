/**
 * Project: gal
 * @file coloring_alg.cpp
 * @date 3. 10. 2018
 * @author xvitra00, xdocek09
 * @brief Source file of module containing coloring algorithms.
 */

#include "coloring_alg.h"

#include <algorithm>
#include <map>
#include <set>
#include <tuple>
#include <unordered_set>
#include <utility>
/**
 * Finds smallest color that any of provided neighbors does not have.
 *
 * @param[in] neighbors	for color search.
 * @param[in|out] neighboursColors
 * 	This parameter is here to save allocation by reusing same vector in multiple
 * calls. It is used as vector of flags where index is color and value is
 * used(true)/not used(false). Recommended size: Number of nodes in graph.
 * 	!!! Do not forget to pass only cleared vector (all false).
 * 	This function will clear that vector after work is done.
 * @param[in] graph Graph we are working on.
 * @return Smallest unused color among the neighbors. If graph.NO_COLOR is
 * returned than no color is unused.
 */
size_t findSmallestUnusedColor(const std::vector<size_t>& neighbors,
                               std::vector<bool>& neighboursColors,
                               const ColoredGraph& graph) {
  for (const auto& neighbourIndex : neighbors) {
    if (graph[neighbourIndex].color() != graph.NO_COLOR)
      neighboursColors[graph[neighbourIndex].color() - 1] = true;
  }
  size_t smallest = graph.NO_COLOR;
  for (size_t i = 0; i < neighboursColors.size(); ++i) {
    // find the smallest unused color
    if (!neighboursColors[i]) {
      smallest = graph.NO_COLOR + i + 1;
      break;
    }
  }
  // clear
  for (const auto& neighbourIndex : neighbors) {
    neighboursColors[graph[neighbourIndex].color() - 1] = false;
  }
  return smallest;
}

void greedyColoring(ColoredGraph& graph,
                    const std::vector<size_t> nodesPermut) {
  auto iterNodes = nodesPermut.begin();
  if (iterNodes == nodesPermut.end())
    return;  // yeah, my work is done

  std::vector<bool> neighboursColors(graph.size(), false);
  graph[*iterNodes].color() = graph.NO_COLOR + 1;
  graph.colorCount() = 1;

  for (++iterNodes; iterNodes != nodesPermut.end(); ++iterNodes) {
    // find the smallest unused color
    graph[*iterNodes].color() = findSmallestUnusedColor(
        graph[*iterNodes].transitions(), neighboursColors, graph);
    if (graph[*iterNodes].color() > graph.colorCount())
      // we have brand new color here
      ++graph.colorCount();
  }
}

void greedyColoring(ColoredGraph& graph) {
  // we prepare data for more general function, that allows concrete node
  std::vector<size_t> nodePerm;
  nodePerm.reserve(graph.size());

  std::transform(graph.begin(),
                 graph.end(),
                 std::back_inserter(nodePerm),
                 [](const auto& n) { return n.id(); });

  greedyColoring(graph, nodePerm);
}

void greedyColoringWithSet(ColoredGraph& graph) {
  for (auto& n : graph) {
    std::set<size_t> neighboursColors;
    for (const auto& neighbourIndex : n.transitions())
      neighboursColors.insert(graph[neighbourIndex].color());

    // instead of if in for loop we just remove NO_COLOR after
    neighboursColors.erase(graph.NO_COLOR);

    if (neighboursColors.size() < graph.colorCount()) {
      // we can recycle smallest unused color
      size_t tryColor =
          graph.NO_COLOR + 1;  // we are starting with smallest possible color
      for (size_t usedColor : neighboursColors) {
        // neighboursColors is order so we can check just difference
        if (usedColor - tryColor > 0) {
          // we found the color
          break;
        } else {
          // ok we are moving forward
          ++tryColor;
        }
      }
      n.color() = tryColor;
    } else {
      // ok, we need a new one
      ++graph.colorCount();
      n.color() = graph.colorCount();
    }
  }
}

void largestDegreeOrderingColoring(ColoredGraph& graph) {
  std::vector<bool> colors(graph.size(), false);

  // Get nodes and their degree sorted in descending order according to their
  // degree.
  std::vector<std::pair<size_t, size_t>>
      nodeDeg;  // the pair is: node degree, node id
  // get degree of each vertex
  for (const auto& node : graph)
    nodeDeg.push_back(std::make_pair(node.transitions().size(), node.id()));

  // sort DESC, because we are starting with the largest degree
  std::sort(nodeDeg.begin(), nodeDeg.end(), std::greater<>());

  // this algorithm is same as greedy coloring but with defined permutation of
  // nodes
  std::vector<size_t> nodePerm;
  nodePerm.reserve(nodeDeg.size());

  std::transform(nodeDeg.begin(),
                 nodeDeg.end(),
                 std::back_inserter(nodePerm),
                 [](const std::pair<size_t, size_t>& p) { return p.second; });

  greedyColoring(graph, nodePerm);
}

void incidenceDegreeOrdering(ColoredGraph& graph) {
  if (graph.size() == 0)
    return;  // yeah, my work is done

  // create set with not colored nodes
  std::unordered_set<size_t> notColoredNodes;
  notColoredNodes.reserve(graph.size());
  for (const auto& node : graph)
    notColoredNodes.insert(node.id());

  // map Node -> degree
  // + selecting of max degree node

  std::map<size_t, size_t> nodeDeg;
  size_t maxDegreeNode = 0;
  size_t maxDegree = 0;

  for (const auto& node : graph) {
    size_t degree = node.transitions().size();
    nodeDeg[node.id()] = degree;
    if (degree > maxDegree) {
      maxDegreeNode = node.id();
      maxDegree = degree;
    }
  }

  // set color to max degree node
  graph[maxDegreeNode].color() = graph.NO_COLOR + 1;
  graph.colorCount() = 1;

  // delete the colored one
  notColoredNodes.erase(maxDegreeNode);

  std::vector<bool> neighboursColors(graph.size(), false);

  while (notColoredNodes.size() > 0) {
    // find node with greatest number of colored neighbors

    auto iterNode = notColoredNodes.cbegin();
    size_t theChosenOne = *iterNode;

    size_t theChosenOneColoredNeighborsCnt = 0;
    for (const auto& edge : graph[*iterNode].transitions()) {
      if (graph[edge].color() != graph.NO_COLOR)
        ++theChosenOneColoredNeighborsCnt;
    }

    for (++iterNode; iterNode != notColoredNodes.cend(); ++iterNode) {
      size_t coloredCnt = 0;
      for (const auto& edge : graph[*iterNode].transitions()) {
        if (graph[edge].color() != graph.NO_COLOR)
          ++coloredCnt;
      }

      if (coloredCnt > theChosenOneColoredNeighborsCnt) {
        // actual node has more colored neighbors
        theChosenOne = *iterNode;
        theChosenOneColoredNeighborsCnt = coloredCnt;
      } else if (coloredCnt == theChosenOneColoredNeighborsCnt &&
                 nodeDeg[*iterNode] > nodeDeg[theChosenOne]) {
        // actual node has same number of colored neighbors, but have bigger
        // degree
        theChosenOne = *iterNode;
      }
    }

    // we have chosen node
    // now is time to try color

    graph[theChosenOne].color() = findSmallestUnusedColor(
        graph[theChosenOne].transitions(), neighboursColors, graph);

    if (graph[theChosenOne].color() > graph.colorCount())
      // we have brand new color here
      ++graph.colorCount();

    // ok, the node is now colored
    notColoredNodes.erase(theChosenOne);
  }
}

/*** End of file: coloring_alg.cpp ***/
