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
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

class ColoredGraph {
 public:
  class Node;

  using iterator = std::vector<Node>::iterator;
  using const_iterator = std::vector<Node>::const_iterator;

  /**
   * Creates graph from vector of vectors of edges.
   * @param[in] nodes	Vector that consists of vectors containing edges.
   * 	Each edge vector represents one node.
   */
  ColoredGraph(std::vector<std::vector<size_t>>& nodes)
  {
    for (size_t i = 0; i < nodes.size(); ++i) {
      nodes_.push_back(Node(i, nodes[i], 0));
    }
    validateTransitions();
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
  }

  ColoredGraph(const ColoredGraph&) = default;
  ColoredGraph(ColoredGraph&&) noexcept = default;
  ColoredGraph& operator=(const ColoredGraph&) = default;
  ColoredGraph& operator=(ColoredGraph&&) noexcept = default;

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
      auto&& transitionList = node.transitions_;
      // sort and limit transitions to at most 1
      std::sort(transitionList.begin(), transitionList.end());
      transitionList.erase(
          std::unique(transitionList.begin(), transitionList.end()),
          transitionList.end());
      // validate transitions
      for (auto&& nextNode : transitionList) {
        if (nextNode >= size()) {
          throw std::invalid_argument("Node transition to nonexistent node.");
        }
      }
    }
  }
};

#endif
/*** End of file: graph.hpp ***/
