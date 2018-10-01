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
  class ConstNode;

  /**
   * Creates graph from vector of vectors of edges.
   * @param[in] nodes	Vector that consists of vectors containing edges.
   * 	Each edge vector represents one node.
   */
  ColoredGraph(std::vector<std::vector<size_t>>& nodes)
      : nodeTransitions_(nodes), nodeColors_(nodes.size(), 0) {
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
    size_t node = 1;

    while (std::getline(is, line)) {
      nodeTransitions_.push_back({});
      nodeColors_.push_back(0);

      std::istringstream lineIss(line);

      size_t edge;
      while (lineIss >> edge)
        nodeTransitions_.back().push_back(edge);

      ++node;
    }

    validateTransitions();
  }

  ColoredGraph(const ColoredGraph&) = default;
  ColoredGraph(ColoredGraph&&) noexcept = default;
  ColoredGraph& operator=(const ColoredGraph&) = default;
  ColoredGraph& operator=(ColoredGraph&&) noexcept = default;

  size_t size() { return nodeTransitions_.size(); }

  Node node(size_t i) noexcept { return Node(*this, i); }
  Node operator[](size_t i) noexcept { return node(i); }

  ConstNode node(size_t i) const noexcept { return ConstNode(*this, i); }
  ConstNode operator[](size_t i) const noexcept { return node(i); }

  /**
   * Prints graph to ostream.
   *
   * @param[in] os	Stream the graph will be printed to.
   * @param[in] g	Graph for printing.
   * @return the stream
   */
  friend std::ostream& operator<<(std::ostream& os, const ColoredGraph& g) {
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

  class Node {
   public:
    Node(ColoredGraph& g, size_t i) noexcept
        : id_(i)
        , transitions_(&(g.nodeTransitions_[i]))
        , color_(&(g.nodeColors_[i])) {}

    size_t id() const noexcept { return id_; }
    explicit operator size_t() const noexcept { return id(); }

    const std::vector<size_t>& transitions() const noexcept {
      return *transitions_;
    }

    bool transitionsTo(size_t i) {
      auto it = std::lower_bound(transitions_->begin(), transitions_->end(), i);
      return it != transitions_->end() && *it == i;
    }

    size_t& color() noexcept { return *color_; }

   private:
    size_t id_;
    const std::vector<size_t>* transitions_;
    size_t* color_;
  };

  class ConstNode {
   public:
    ConstNode(const ColoredGraph& g, size_t i) noexcept
        : id_(i)
        , transitions_(&(g.nodeTransitions_[i]))
        , color_(g.nodeColors_[i]) {}

    size_t id() const noexcept { return id_; }
    explicit operator size_t() const noexcept { return id(); }

    const std::vector<size_t>& transitions() const noexcept {
      return *transitions_;
    }

    bool transitionsTo(size_t i) {
      auto it = std::lower_bound(transitions_->begin(), transitions_->end(), i);
      return it != transitions_->end() && *it == i;
    }

    size_t color() noexcept { return color_; }

   private:
    size_t id_;
    const std::vector<size_t>* transitions_;
    size_t color_;
  };

 private:
  std::vector<std::vector<size_t>> nodeTransitions_;
  std::vector<size_t> nodeColors_;
  size_t colors_ = 0;

  /**
   * Performs graph validation.
   *
   * @throw invalid_argument	When node have transition to nonexistent node.
   */
  void validateTransitions() {
    for (auto&& transitionList : nodeTransitions_) {
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
