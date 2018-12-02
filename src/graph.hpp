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

/**
 * Unoriented, colored graph.
 */
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
    validateEdges();
    minimizeEdges();
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
        nodes_.back().edges_.push_back(edge);

      ++id;
    }

    validateEdges();
    minimizeEdges();
    toUndirected();
  }

  /**
   * Generate a random graph.
   *
   * @param size The number of nodes.
   * @param edgePropability The propability of an edge between two nodes.
   * @param limit The optional upper limit for the number of edges.
   */
  ColoredGraph(size_t size,
               double edgePropability,
               size_t limit = std::numeric_limits<size_t>::max()) {
    for (size_t i = 0; i < size; ++i) {
      nodes_.push_back(Node(i, {}, NO_COLOR));
    }
    // leads to undirected, unique, sorted edges
    for (size_t i = 0; i < size; ++i) {
      for (size_t j = i + 1; j < size; ++j) {
        if (gal_rand(1.0) < edgePropability && limit != 0) {
          nodes_[i].edges_.push_back(j);
          nodes_[j].edges_.push_back(i);
          --limit;
        }
      }
    }
  }

  ColoredGraph(const ColoredGraph&) = default;
  ColoredGraph(ColoredGraph&&) noexcept = default;
  ColoredGraph& operator=(const ColoredGraph&) = default;
  ColoredGraph& operator=(ColoredGraph&&) noexcept = default;

  /**
   * Create a random graph.
   * @param size The number of nodes.
   * @param edgePropability The propability of an edge between two nodes.
   * @param limit The optional upper limit for the number of edges.
   */
  static ColoredGraph randomGraph(
      size_t size,
      double edgePropability,
      size_t limit = std::numeric_limits<size_t>::max()) {
    return std::move(ColoredGraph(size, edgePropability, limit));
  }

  size_t size() { return nodes_.size(); }
  size_t edgeCount() {
    size_t s = 0;
    for (auto&& node : nodes_) {
      s += node.edges().size();
    }
    return s;
  }

  iterator begin() { return nodes_.begin(); }
  iterator end() { return nodes_.end(); }

  const_iterator begin() const { return nodes_.cbegin(); }
  const_iterator end() const { return nodes_.cend(); }

  /**
   * Get the Nth node.
   */
  Node& operator[](size_t i) noexcept { return nodes_[i]; }
  Node& node(size_t i) noexcept { return nodes_[i]; }

  const Node& operator[](size_t i) const noexcept { return nodes_[i]; }
  const Node& node(size_t i) const noexcept { return nodes_[i]; }

  bool insertEdge(size_t a, size_t b) {
    if (a == b || a >= size() || b >= size()) {
      return false;
    }
    auto&& n1 = node(a);
    auto&& n2 = node(b);
    if (n1.edgeTo(b)) {
      return false;
    }
    auto it = std::lower_bound(n1.edges_.begin(), n1.edges_.end(), b);
    n1.edges_.insert(it, b);
    it = std::lower_bound(n2.edges_.begin(), n2.edges_.end(), a);
    n2.edges_.insert(it, a);

    return true;
  }

  /**
   * Prints graph to ostream.
   *
   * @param[in] os Stream the graph will be printed to.
   * @param[in] g Graph for printing.
   * @return the stream
   */
  friend std::ostream& operator<<(std::ostream& os, const ColoredGraph& g) {
    for (const auto& node : g.nodes_) {
      os << node.id() << ") color: " << node.color() << ", edges: ";
      auto&& edges = node.edges();
      auto it = edges.begin();

      if (it != edges.end()) {
        os << *it++;
      }
      while (it != edges.end()) {
        os << " " << *it++;
      }
      os << "\n";
    }
    return os;
  }
  /**
   * Prints graph to ostream in the same format it's read in the constructor.
   * (So without colors.)
   *
   * @param[out] os Stream the graph will be printed to.
   */
  void print(std::ostream& os) {
    for (const auto& node : nodes_) {
      auto&& edges = node.edges();
      auto it = edges.begin();

      if (it != edges.end()) {
        os << *it++;
      }
      while (it != edges.end()) {
        os << " " << *it++;
      }
      os << "\n";
    }
  }

  class Node {
   public:
    friend class ColoredGraph;
    Node(size_t id, const std::vector<size_t>& edges, size_t color) noexcept
        : id_(id), edges_(edges), color_(color) {}

    size_t id() const noexcept { return id_; }
    explicit operator size_t() const noexcept { return id(); }

    const std::vector<size_t>& edges() const noexcept { return edges_; }

    /**
     * Returns true if this node has a transition to a different node.
     */
    bool edgeTo(size_t i) noexcept {
      auto it = std::lower_bound(edges_.begin(), edges_.end(), i);
      return it != edges_.end() && *it == i;
    }

    size_t color() const noexcept { return color_; }
    size_t& color() noexcept { return color_; }

   private:
    size_t id_;
    std::vector<size_t> edges_;
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
      for (const auto& edge : node.edges())
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
  void validateEdges() {
    for (auto&& node : nodes_) {
      // validate edges
      for (auto&& nextNode : node.edges_) {
        if (nextNode >= size()) {
          throw std::invalid_argument("Node transition to nonexistent node.");
        }
      }
    }
  }
  /**
   * Transforms the transition lists of all nodes to unique sorted lists.
   */
  void minimizeEdges() {
    for (auto&& node : nodes_) {
      auto&& transitionList = node.edges_;
      // sort and limit edges to at most 1
      std::sort(transitionList.begin(), transitionList.end());
      transitionList.erase(
          std::unique(transitionList.begin(), transitionList.end()),
          transitionList.end());
    }
  }

  /**
   * Converts the directed graph to an undirected one.
   */
  void toUndirected() {
    edgeSymmetrization();
    // remove the loops and edges that are there multiple times
    std::vector<size_t> connectedWith(nodes_.size(), nodes_.size());
    for (auto& node : nodes_) {
      std::vector<size_t> newEdges;
      for (auto edge : node.edges()) {
        if (connectedWith[edge] != node.id() && edge != node.id()) {
          newEdges.push_back(edge);
          connectedWith[edge] = node.id();
        }
      }
      node.edges_.swap(newEdges);
    }
  }

  /**
   * Performs the symmetrization of edges.
   */
  void edgeSymmetrization() {
    for (auto& node : nodes_) {
      // num of edges on the start
      size_t numOfEdges = node.edges_.size();
      for (size_t i = 0; i < numOfEdges; ++i) {
        if (node.edges_[i] < nodes_.size()) {
          // original edge
          if (node.id() < node.edges_[i]) {
            // needs marking because of future processing
            nodes_[node.edges_[i]].edges_.push_back(nodes_.size() + node.id());
          } else {
            // no need for, mark because the node was already processed
            nodes_[node.edges_[i]].edges_.push_back(node.id());
          }
        } else {
          // this edge was symmetrized
          // lets remove the mark
          node.edges_[i] -= nodes_.size();
        }
      }
    }
  }
};

#endif
/*** End of file: graph.hpp ***/
