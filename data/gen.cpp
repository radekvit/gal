/**
 * Project: gal
 * @file gen.cpp
 * @date 2. 12. 2018
 * @author xvitra00, xdocek09
 * @brief Generate test data.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include "coloring_alg.h"
#include "graph.hpp"

void insertMissingEdges(ColoredGraph& g, size_t edges) {
	size_t edgeCount = g.edgeCount();
	while (edgeCount < edges) {
		// try to add to a random node
		size_t selected = gal_rand(g.size());
		auto&& node = g[selected];
		// cannot add an edge
		if (node.edges().size() == g.size() - 1) {
			continue;
		}
		auto it = node.edges().begin();
		// find a missing edge and insert it
		size_t e = 0;
		for (; it != node.edges().end(); ++it, ++e) {
			if (e == selected) {
				++e;
			}
			if (*it != e) {
				// insert new edge
				if (g.insertEdge(selected, e)) {
					edgeCount += 2;
				}
				goto insertNext;
			}
		}
		// empty or at the end
		if (g.insertEdge(selected, e)) {
			edgeCount += 2;
		}
	insertNext:;
	}
}

int main(int argc, char* argv[]) {
	if (argc != 4) {
		std::cerr << "usage: ./gen nodeCount edgeCount outputName\n";
		return 1;
	}

	size_t nodeCount = std::stoul(argv[1]);
	size_t edgeCount = std::stoul(argv[2]);

	if (nodeCount < 1) {
		std::cerr << "There must be at least one node in the generated graph.\n";
		return 1;
	}
	if (edgeCount * 2 > nodeCount * (nodeCount - 1)) {
		std::cerr << "edgeCount * 2 must be at most nodeCount * (nodeCount - 1)\n";
		return 1;
	}
	double edgePropability = static_cast<double>(edgeCount) / (nodeCount * (nodeCount - 1));

	ColoredGraph generated(nodeCount, edgePropability, edgeCount);
	// add edges if missing
	insertMissingEdges(generated, edgeCount * 2);
	assert(generated.edgeCount() == edgeCount * 2);

	std::ofstream of(argv[3]);
	if (!of) {
		std::cerr << "Could not open output file " << argv[3] << ".\n";
		return 1;
	}
	generated.print(of);

	return 0;
}