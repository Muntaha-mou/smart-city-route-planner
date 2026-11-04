// Smart City Route & Infrastructure Planner
// Copyright © 2026 Quad Core. All rights reserved.
//
// algorithms.cpp - BFS and DFS (see algorithms.h).

#include "algorithms.h"

#include <algorithm>
#include <queue>
#include <set>

using namespace std;

// Returns the IDs of the locations directly connected to `id`, sorted from
// the smallest ID to the largest. Sorting makes BFS and DFS always visit
// neighbours in the same order, no matter in which order roads were added.
static vector<int> sortedNeighborIds(const Graph& city, int id)
{
    vector<int> ids;
    for (const Edge& edge : city.getNeighbors(id)) {
        ids.push_back(edge.to);
    }
    sort(ids.begin(), ids.end());
    return ids;
}

// ===== Breadth-First Search =====

vector<int> bfs(const Graph& city, int startId)
{
    vector<int> order;
    if (!city.hasLocation(startId)) {
        return order;   // empty: the start location does not exist
    }

    set<int> visited;     // locations already found
    queue<int> toVisit;   // locations waiting to be visited (first in, first out)

    visited.insert(startId);
    toVisit.push(startId);

    while (!toVisit.empty()) {
        // Take the location that has waited the longest.
        int current = toVisit.front();
        toVisit.pop();
        order.push_back(current);

        // Add every neighbour that has not been found yet to the back of the
        // queue. It is marked as visited NOW, so it can never be added twice.
        for (int neighbor : sortedNeighborIds(city, current)) {
            if (visited.count(neighbor) == 0) {
                visited.insert(neighbor);
                toVisit.push(neighbor);
            }
        }
    }
    return order;
}

// ===== Depth-First Search =====

// Visits `current`, then goes deep into each neighbour that has not been
// visited yet. When all neighbours are done, the function returns: this
// return is the "backtracking" step.
static void dfsVisit(const Graph& city, int current, set<int>& visited, vector<int>& order)
{
    visited.insert(current);
    order.push_back(current);

    for (int neighbor : sortedNeighborIds(city, current)) {
        if (visited.count(neighbor) == 0) {
            dfsVisit(city, neighbor, visited, order);
        }
    }
}

vector<int> dfs(const Graph& city, int startId)
{
    vector<int> order;
    if (!city.hasLocation(startId)) {
        return order;   // empty: the start location does not exist
    }

    set<int> visited;
    dfsVisit(city, startId, visited, order);
    return order;
}
