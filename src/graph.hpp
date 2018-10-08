/**
 * Project: gal
 * @file graph.hpp
 * @date 30. 9. 2018
 * @author xvitra00, xdocek09
 * @brief Header file of module containing colored graph representation.
 */
#ifndef GAL_GRAPH_HPP
#define GAL_GRAPH_HPP

#include <algorithm>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <random.h>

class ColoredGraph {
 public:
  class Node;

  using iterator = std::vector<Node>::iterator;
  using const_iterator = std::vector<Node>::const_iterator;

  static constexpr size_t NO_COLOR = 0;

  /**
   * Creates graph from vector of vectors of edges.
   * @param[in] nodes	Vector that consists of vectors containing edges.
   * 	Each edge vector represents one node.
   */
  ColoredGraph(std::vector<std::vector<size_t>>& nodes) {
    for (size_t i = 0; i < nodes.size(); ++i) {
      nodes_.push_back(Node(i, nodes[i], NO_COLOR));
    }
    validateTransitions();
    minimizeTransitions();
    toUndirected();
  }

  /**
   * Reads graph from input stream.
   *
   * @param[in] is	Stream with graph text representation.
   * 	Format:
   * 		Each line represents edges of one node. Node name is derived from
   * 		line number. Edge is represented with number of target node.
   * 	Example:
   * 		0 1 2
   * 		1 2
   * 		2
   *
   * 		This example represents graph with three nodes (0-2) and 6 edges.
   */
  ColoredGraph(std::istream& is) {
    std::string line;
    size_t id = 0;

    while (std::getline(is, line)) {
      nodes_.push_back(Node(id, {}, 0));

      std::istringstream lineIss(line);

      size_t edge;
      while (lineIss >> edge)
        nodes_.back().transitions_.push_back(edge);

      ++id;
    }

    validateTransitions();
    minimizeTransitions();
    toUndirected();
  }

  ColoredGraph(size_t size, double edgePropability) {
    for (size_t i = 0; i < size; ++i) {
      nodes_.push_back(Node(i, {}, NO_COLOR));
    }
    // leads to undirected, unique, sorted transitions
    for (size_t i = 0; i < size; ++i) {
      for (size_t j = i + 1; j < size; ++j) {
        if (gal_rand(1.0) < edgePropability) {
          nodes_[i].transitions_.push_back(j);
          nodes_[j].transitions_.push_back(i);
        }
      }
    }
  }

  ColoredGraph(const ColoredGraph&) = default;
  ColoredGraph(ColoredGraph&&) noexcept = default;
  ColoredGraph& operator=(const ColoredGraph&) = default;
  ColoredGraph& operator=(ColoredGraph&&) noexcept = default;

  static ColoredGraph randomGraph(size_t size, double edgePropability) {
    return std::move(ColoredGraph(size, edgePropability));
  }

  size_t size() { return nodes_.size(); }

  iterator begin() { return nodes_.begin(); }
  iterator end() { return nodes_.end(); }

  const_iterator begin() const { return nodes_.cbegin(); }
  const_iterator end() const { return nodes_.cend(); }

  Node& node(size_t i) noexcept { return nodes_[i]; }
  Node& operator[](size_t i) noexcept { return node(i); }

  const Node& node(size_t i) const noexcept { return nodes_[i]; }
  const Node& operator[](size_t i) const noexcept { return node(i); }

  /**
   * Prints graph to ostream.
   *
   * @param[in] os	Stream the graph will be printed to.
   * @param[in] g	Graph for printing.
   * @return the stream
   */
  friend std::ostream& operator<<(std::ostream& os, const ColoredGraph& g) {
    for (const auto& node : g.nodes_) {
      os << node.id() << ") color: " << node.color() << ", edges: ";
      auto&& transitions = node.transitions();
      auto it = transitions.begin();

      if (it != transitions.end()) {
        os << *it++;
      }
      while (it != transitions.end()) {
        os << " " << *it++;
      }
      os << "\n";
    }
    return os;
  }

  class Node {
   public:
    friend class ColoredGraph;
    Node(size_t id,
         const std::vector<size_t>& transitions,
         size_t color) noexcept
        : id_(id), transitions_(transitions), color_(color) {}

    size_t id() const noexcept { return id_; }
    explicit operator size_t() const noexcept { return id(); }

    const std::vector<size_t>& transitions() const noexcept {
      return transitions_;
    }

    bool transitionsTo(size_t i) noexcept {
      auto it = std::lower_bound(transitions_.begin(), transitions_.end(), i);
      return it != transitions_.end() && *it == i;
    }

    size_t color() const noexcept { return color_; }
    size_t& color() noexcept { return color_; }

   private:
    size_t id_;
    std::vector<size_t> transitions_;
    size_t color_;
  };

  /**
   * Clears colors from all nodes.
   */
  void clearColors() {
    for (auto& n : *this)
      n.color_ = NO_COLOR;
    colorCount_ = 0;
  }

  size_t& colorCount() noexcept { return colorCount_; }
  const size_t& colorCount() const noexcept { return colorCount_; }

  /**
   * Checks if graph is colored.
   * 	All nodes have colors and no two connected nodes have same color.
   *
   * @return True -> colored. False -> something fishy.
   */
  bool validateColors() const {
    for (const auto& node : nodes_) {
      if (node.color() == NO_COLOR)
        return false;
      for (const auto& edge : node.transitions())
        if (nodes_[edge].color() == node.color())
          return false;
    }
    return true;
  }

 private:
  std::vector<Node> nodes_;
  size_t colorCount_ = 0;

  /**
   * Performs graph validation.
   *
   * @throw invalid_argument	When node have transition to nonexistent node.
   */
  void validateTransitions() {
    for (auto&& node : nodes_) {
      // validate transitions
      for (auto&& nextNode : node.transitions_) {
        if (nextNode >= size()) {
          throw std::invalid_argument("Node transition to nonexistent node.");
        }
      }
    }
  }

  void minimizeTransitions() {
    for (auto&& node : nodes_) {
      auto&& transitionList = node.transitions_;
      // sort and limit transitions to at most 1
      std::sort(transitionList.begin(), transitionList.end());
      transitionList.erase(
          std::unique(transitionList.begin(), transitionList.end()),
          transitionList.end());
    }
  }

  /**
   * Converts this graph to undirected one.
   */
  void toUndirected() {
    edgeSymmetrization();
    // remove the loops and edges that are there multiple times
    std::vector<size_t> connectedWith(nodes_.size(), nodes_.size());
    for (auto& node : nodes_) {
      std::vector<size_t> newEdges;
      for (auto edge : node.transitions()) {
        if (connectedWith[edge] != node.id() && edge != node.id()) {
          newEdges.push_back(edge);
          connectedWith[edge] = node.id();
        }
      }
      node.transitions_.swap(newEdges);
    }
  }

  /**
   * Performs symmetrization of edges.
   */
  void edgeSymmetrization() {
    for (auto& node : nodes_) {
      // num of edges on the start
      size_t numOfEdges = node.transitions_.size();
      for (size_t i = 0; i < numOfEdges; ++i) {
        if (node.transitions_[i] < nodes_.size()) {
          // original edge
          if (node.id() < node.transitions_[i]) {
            // needs marking because of future processing
            nodes_[node.transitions_[i]].transitions_.push_back(nodes_.size() +
                                                                node.id());
          } else {
            // no need for, mark because the node was already processed
            nodes_[node.transitions_[i]].transitions_.push_back(node.id());
          }
        } else {
          // this edge was symmetrized
          // lets remove the mark
          node.transitions_[i] -= nodes_.size();
        }
      }
    }
  }
};

#endif
/*** End of file: graph.hpp ***/
