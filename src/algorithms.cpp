// Smart City Route & Infrastructure Planner
// Copyright © 2026 Quad Core. All rights reserved.
//
// algorithms.cpp - BFS, DFS, Dijkstra, Kruskal and Floyd–Warshall
// (see algorithms.h).

#include "algorithms.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <map>
#include <queue>
#include <set>
#include <utility>

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

// ===== Dijkstra's shortest route =====

RouteResult dijkstra(const Graph& city, int startId, int endId)
{
    RouteResult result;   // found = false, distance = 0, empty path
    if (!city.hasLocation(startId) || !city.hasLocation(endId)) {
        return result;
    }

    // dist[id]   = shortest distance found so far from the start to `id`
    // parent[id] = the location just before `id` on that shortest route
    map<int, double> dist;
    map<int, int> parent;
    for (int id : city.getLocationIds()) {
        dist[id] = INFINITY;   // not reached yet
    }
    dist[startId] = 0.0;

    // Min-priority queue of (distance, location ID) pairs.
    // greater<> makes the SMALLEST distance come out first (a normal
    // priority_queue gives the largest first). If two distances are equal,
    // the smaller ID comes out first.
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> toVisit;
    toVisit.push({0.0, startId});

    while (!toVisit.empty()) {
        double currentDist = toVisit.top().first;
        int current = toVisit.top().second;
        toVisit.pop();

        // An outdated entry: a shorter route to `current` was already found.
        if (currentDist > dist[current]) {
            continue;
        }
        // The destination's distance is final once it leaves the queue.
        if (current == endId) {
            break;
        }

        // "Relax" each road: is the route through `current` shorter?
        for (const Edge& edge : city.getNeighbors(current)) {
            double newDist = currentDist + edge.distance;
            if (newDist < dist[edge.to]) {
                dist[edge.to] = newDist;
                parent[edge.to] = current;
                toVisit.push({newDist, edge.to});
            }
        }
    }

    if (dist[endId] == INFINITY) {
        return result;   // the destination was never reached: no route
    }

    // Rebuild the route by following the parents back from the destination,
    // then reverse it so it goes from start to destination.
    int step = endId;
    result.path.push_back(step);
    while (step != startId) {
        step = parent[step];
        result.path.push_back(step);
    }
    reverse(result.path.begin(), result.path.end());

    result.found = true;
    result.distance = dist[endId];
    return result;
}

// ===== Kruskal's minimum spanning tree =====

// Disjoint Set Union (DSU), also called Union-Find.
// It remembers which locations are already joined by the roads selected so
// far. Locations that are joined form a group; each group has one "root"
// location that represents the whole group.
struct DisjointSet {
    map<int, int> parent;   // parent[id]: the next location on the way to the root
    map<int, int> rank;     // rank[root]: roughly the height of the root's tree

    // Puts a location in a group of its own.
    void makeSet(int id)
    {
        parent[id] = id;
        rank[id] = 0;
    }

    // Returns the root of the group that `id` belongs to.
    // Path compression: every location on the way is linked directly to the
    // root, so the next search for it is much faster.
    int findRoot(int id)
    {
        if (parent[id] != id) {
            parent[id] = findRoot(parent[id]);
        }
        return parent[id];
    }

    // Joins the groups of `a` and `b`. Returns false if they are already in
    // the same group (then a road between them would make a cycle).
    // Union by rank: the lower tree is attached under the higher tree, so the
    // trees stay short.
    bool unite(int a, int b)
    {
        int rootA = findRoot(a);
        int rootB = findRoot(b);
        if (rootA == rootB) {
            return false;
        }
        if (rank[rootA] < rank[rootB]) {
            parent[rootA] = rootB;
        } else if (rank[rootA] > rank[rootB]) {
            parent[rootB] = rootA;
        } else {
            parent[rootB] = rootA;
            rank[rootA]++;
        }
        return true;
    }
};

// Sorting rule for roads: shorter roads first. Roads with the same distance
// are ordered by their location IDs, so Kruskal always gives the same result.
static bool isShorterRoad(const Road& a, const Road& b)
{
    if (a.distance != b.distance) {
        return a.distance < b.distance;
    }
    if (a.from != b.from) {
        return a.from < b.from;
    }
    return a.to < b.to;
}

SpanningResult kruskal(const Graph& city)
{
    SpanningResult result;

    // At the start, every location is a separate group (component).
    DisjointSet groups;
    for (int id : city.getLocationIds()) {
        groups.makeSet(id);
    }
    result.components = city.locationCount();

    // All roads, each one only once (getRoads() lists A-B but not B-A),
    // sorted from the shortest to the longest.
    vector<Road> roads = city.getRoads();
    sort(roads.begin(), roads.end(), isShorterRoad);

    for (const Road& road : roads) {
        // A road is selected only if it joins two DIFFERENT groups.
        // If both ends are already in the same group, it would make a cycle.
        if (groups.unite(road.from, road.to)) {
            result.roads.push_back(road);
            result.totalDistance += road.distance;
            result.components--;   // two groups became one
        }
    }

    result.connected = (result.components == 1);
    return result;
}

// ===== Floyd–Warshall: all-pairs shortest paths =====

AllPairsResult floydWarshall(const Graph& city)
{
    AllPairsResult result;

    // Give every location a row/column number in increasing ID order.
    // (IDs can have gaps, e.g. 1, 2, 5, so the ID itself cannot be used.)
    result.ids = city.getLocationIds();
    int n = static_cast<int>(result.ids.size());
    for (int i = 0; i < n; i++) {
        result.indexOf[result.ids[i]] = i;
    }

    // Start: no routes known (INFINITY, next = -1), ...
    result.dist.assign(n, vector<double>(n, INFINITY));
    result.next.assign(n, vector<int>(n, -1));
    for (int i = 0; i < n; i++) {
        // ... a location is 0 km from itself, ...
        result.dist[i][i] = 0.0;
        result.next[i][i] = i;
        // ... and each road is a route of one road (in both directions,
        // because the road list of each location contains it).
        for (const Edge& edge : city.getNeighbors(result.ids[i])) {
            int j = result.indexOf[edge.to];
            result.dist[i][j] = edge.distance;
            result.next[i][j] = j;   // from i, go straight to j
        }
    }

    // Step k: may the route from i to j go through location k?
    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            // No route from i to k: going through k cannot help, and
            // INFINITY + something must not be calculated.
            if (result.dist[i][k] == INFINITY) {
                continue;
            }
            for (int j = 0; j < n; j++) {
                if (result.dist[k][j] == INFINITY) {
                    continue;
                }
                double throughK = result.dist[i][k] + result.dist[k][j];
                // Only a STRICTLY shorter route replaces the old one, so equal
                // routes keep the first one found (deterministic result).
                if (throughK < result.dist[i][j]) {
                    result.dist[i][j] = throughK;
                    // The first stop towards j is the first stop towards k.
                    result.next[i][j] = result.next[i][k];
                }
            }
        }
    }
    return result;
}

RouteResult allPairsRoute(const AllPairsResult& result, int startId, int endId)
{
    RouteResult route;   // found = false, distance = 0, empty path
    auto start = result.indexOf.find(startId);
    auto end = result.indexOf.find(endId);
    if (start == result.indexOf.end() || end == result.indexOf.end()) {
        return route;   // unknown location
    }
    int i = start->second;
    int j = end->second;
    if (result.dist[i][j] == INFINITY) {
        return route;   // the locations are in different parts of the network
    }

    // Follow the "next" matrix from the start until the destination.
    // A route never has more than n locations. The checks below are only a
    // safety net for an inconsistent table: they report "no route" instead of
    // returning an incomplete route or reading outside the matrix.
    int n = static_cast<int>(result.ids.size());
    vector<int> path = {result.ids[i]};
    int current = i;
    while (current != j) {
        current = result.next[current][j];
        if (current < 0 || current >= n || static_cast<int>(path.size()) >= n) {
            return route;   // found is still false
        }
        path.push_back(result.ids[current]);
    }

    route.path = path;
    route.found = true;
    route.distance = result.dist[i][j];
    return route;
}
