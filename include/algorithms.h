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

#include <map>
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

// The result of a shortest-route search.
struct RouteResult {
    bool found = false;      // true if a route exists
    double distance = 0.0;   // total length of the route in km
    std::vector<int> path;   // location IDs from start to destination
};

// Dijkstra's algorithm: finds the route with the smallest TOTAL DISTANCE
// from `startId` to `endId` (not necessarily the route with the fewest roads).
// Uses a min-priority queue. All road distances are positive.
// - If start and destination are the same: found, distance 0, path {start}.
// - If an ID does not exist or no route exists: found is false.
// - If several routes are equally short, the result is always the same one.
// Time: O((V + E) log V), Space: O(V + E)
RouteResult dijkstra(const Graph& city, int startId, int endId);

// The result of Kruskal's algorithm.
struct SpanningResult {
    std::vector<Road> roads;      // the selected roads, shortest first
    double totalDistance = 0.0;   // sum of the selected road distances in km
    int components = 0;           // number of separate parts of the network
    bool connected = false;       // true if the network is one single part
};

// Kruskal's algorithm: selects existing roads with the smallest total distance
// that connect all locations, without forming any cycle.
// - Connected network (1 component): the result is a Minimum Spanning Tree
//   with (V - 1) roads.
// - Disconnected network: the result is a Minimum Spanning Forest, one minimum
//   tree for each part, with (V - components) roads.
// - Empty city: no roads, 0 components, not connected.
// Equal road distances are ordered by location IDs, so the result is always
// the same. Uses a Disjoint Set Union (DSU) to detect cycles.
// Time: O(E log E + V log V), which is O(E log E) for a connected network
// (E >= V - 1). Space: O(V + E)
SpanningResult kruskal(const Graph& city);

// The result of the Floyd–Warshall algorithm: the shortest distance between
// EVERY pair of locations.
// Location IDs may have gaps (for example 1, 2, 5), so each location gets a
// row/column number (index) 0, 1, 2, ... in increasing ID order:
//   ids[index]   = the location ID of that row/column
//   indexOf[id]  = the row/column of a location ID
struct AllPairsResult {
    std::vector<int> ids;
    std::map<int, int> indexOf;
    // dist[i][j] = shortest distance in km from ids[i] to ids[j];
    //              INFINITY if there is no route; 0 on the diagonal.
    std::vector<std::vector<double>> dist;
    // next[i][j] = index of the location that comes right after ids[i] on a
    //              shortest route to ids[j]; -1 if there is no route.
    std::vector<std::vector<int>> next;
};

// Floyd–Warshall algorithm (dynamic programming): shortest distances between
// all pairs of locations. Step k allows the k-th location as a stop in
// between: dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]).
// Locations are tried as stops in increasing ID order, and a route is only
// replaced by a STRICTLY shorter one, so the result is always the same.
// Time: O(V^3), Space: O(V^2)
AllPairsResult floydWarshall(const Graph& city);

// The shortest route from `startId` to `endId`, rebuilt from the `next`
// matrix of an AllPairsResult. Same rules as dijkstra():
// - start and destination the same: found, distance 0, path {start}
// - unknown ID or no route: found is false
// Time: O(V) (at most V locations on a route)
RouteResult allPairsRoute(const AllPairsResult& result, int startId, int endId);

#endif // ALGORITHMS_H
