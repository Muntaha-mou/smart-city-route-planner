// Smart City Route & Infrastructure Planner
// Copyright © 2026 Quad Core. All rights reserved.
//
// algorithms.h - graph algorithms that work on the city road network.
//
// Like graph.h, this file has no console input/output: each algorithm only
// returns its result, and the menu program (main.cpp) displays it.
//
// Notation: V = number of locations, E = number of roads.

#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "graph.h"

#include <vector>

// Breadth-First Search (BFS) starting at `startId`.
// Visits locations level by level: first the start, then all locations one
// road away, then all locations two roads away, and so on. Uses a queue.
// Returns the location IDs in the order they were visited.
// Returns an empty list if `startId` does not exist.
// Time: O(V + E) visits (plus sorting each neighbour list), Space: O(V)
std::vector<int> bfs(const Graph& city, int startId);

// Depth-First Search (DFS) starting at `startId`.
// Follows one road as far as possible, then goes back (backtracks) and tries
// the next road. Uses recursion.
// Returns the location IDs in the order they were visited.
// Returns an empty list if `startId` does not exist.
// Time: O(V + E) visits (plus sorting each neighbour list), Space: O(V)
std::vector<int> dfs(const Graph& city, int startId);

// In both algorithms, the neighbours of a location are visited in
// increasing ID order, so the result is always the same for the same city.

#endif // ALGORITHMS_H
