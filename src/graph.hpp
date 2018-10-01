#ifndef GAL_GRAPH_HPP
#define GAL_GRAPH_HPP

#include <algorithm>
#include <stdexcept>
#include <vector>

class ColoredGraph {
 public:
  class Node;
  class ConstNode;

  explicit ColoredGraph(std::initializer_list<std::vector<size_t>> il)
      : nodeTransitions_(il), nodeColors_(il.size(), 0) {
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
  ColoredGraph(const ColoredGraph&) = default;
  ColoredGraph(ColoredGraph&&) noexcept = default;
  ColoredGraph& operator=(const ColoredGraph&) = default;
  ColoredGraph& operator=(ColoredGraph&&) noexcept = default;

  size_t size() { return nodeTransitions_.size(); }

  Node node(size_t i) noexcept { return Node(*this, i); }
  Node operator[](size_t i) noexcept { return node(i); }

  ConstNode node(size_t i) const noexcept { return ConstNode(*this, i); }
  ConstNode operator[](size_t i) const noexcept { return node(i); }

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
};

#endif