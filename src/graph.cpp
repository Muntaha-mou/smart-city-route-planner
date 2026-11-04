// Smart City Route & Infrastructure Planner
// Copyright © 2026 Quad Core. All rights reserved.
//
// graph.cpp - implementation of the Graph class (see graph.h).
//
// Complexity notes: V = number of locations, E = number of roads,
// deg = number of roads at one location. std::map lookups take O(log V).

#include "graph.h"

#include <cmath>

// True if the name is empty or contains only spaces/tabs.
// (Only used in this file.)
static bool isBlankName(const std::string& name)
{
    return name.find_first_not_of(" \t") == std::string::npos;
}

// ===== Locations =====

// Time: O(V) because nameExists() checks every name.
int Graph::addLocation(const std::string& name)
{
    if (isBlankName(name) || nameExists(name)) {
        return -1;
    }

    int id = nextId;
    nextId++;

    names[id] = name;
    adjacency[id] = std::vector<Edge>();   // a new location has no roads yet
    return id;
}

// Time: O(V) because nameExists() checks every name.
bool Graph::addLocationWithId(int id, const std::string& name)
{
    if (id <= 0 || hasLocation(id) || isBlankName(name) || nameExists(name)) {
        return false;
    }

    names[id] = name;
    adjacency[id] = std::vector<Edge>();

    // Make sure a location added later gets a new, unused ID.
    if (id >= nextId) {
        nextId = id + 1;
    }
    return true;
}

// Time: O(V) because nameExists() checks every name.
bool Graph::renameLocation(int id, const std::string& newName)
{
    if (!hasLocation(id) || isBlankName(newName)) {
        return false;
    }
    // The name may not belong to a DIFFERENT location.
    // (Keeping the same name for the same location is allowed.)
    if (nameExists(newName) && names[id] != newName) {
        return false;
    }

    names[id] = newName;
    return true;
}

// Time: O(deg x deg') - for each road of this location, the neighbour's
// road list is searched to remove the road from the other side too.
bool Graph::removeLocation(int id)
{
    if (!hasLocation(id)) {
        return false;
    }

    // Every road is stored in two lists. The copy in THIS location's list
    // disappears when the list is erased below, but the copy in each
    // neighbour's list must be removed one by one.
    for (const Edge& edge : adjacency[id]) {
        removeEdge(edge.to, id);
    }

    adjacency.erase(id);
    names.erase(id);
    return true;
}

bool Graph::hasLocation(int id) const
{
    return names.count(id) > 0;
}

bool Graph::nameExists(const std::string& name) const
{
    // In a std::map, each entry has a key (entry.first) and a value (entry.second).
    for (const auto& entry : names) {
        if (entry.second == name) {
            return true;
        }
    }
    return false;
}

std::string Graph::getName(int id) const
{
    if (!hasLocation(id)) {
        return "";
    }
    return names.at(id);
}

std::vector<int> Graph::getLocationIds() const
{
    std::vector<int> ids;
    for (const auto& entry : names) {
        ids.push_back(entry.first);   // std::map keeps keys sorted, so IDs come in order
    }
    return ids;
}

int Graph::locationCount() const
{
    return static_cast<int>(names.size());
}

int Graph::getNextId() const
{
    return nextId;
}

void Graph::setNextId(int id)
{
    // Only move forward: going back could give a new location an old ID.
    if (id > nextId) {
        nextId = id;
    }
}

// ===== Roads =====

// Time: O(log V + deg) - the duplicate check scans one adjacency list.
bool Graph::addRoad(int from, int to, double distance)
{
    if (!hasLocation(from) || !hasLocation(to)) {
        return false;
    }
    if (from == to) {
        return false;
    }
    if (!std::isfinite(distance) || distance <= 0) {
        return false;
    }
    if (hasRoad(from, to)) {
        return false;
    }

    // The road is two-way, so it is stored in the lists of BOTH locations.
    adjacency[from].push_back({to, distance});
    adjacency[to].push_back({from, distance});
    return true;
}

// Time: O(log V + deg)
bool Graph::updateRoad(int from, int to, double distance)
{
    if (!hasRoad(from, to)) {
        return false;
    }
    if (!std::isfinite(distance) || distance <= 0) {
        return false;
    }

    // Change the distance in both copies of the road.
    // `Edge& edge` is a reference, so changing it changes the stored edge.
    for (Edge& edge : adjacency[from]) {
        if (edge.to == to) {
            edge.distance = distance;
        }
    }
    for (Edge& edge : adjacency[to]) {
        if (edge.to == from) {
            edge.distance = distance;
        }
    }
    return true;
}

// Time: O(log V + deg)
bool Graph::removeRoad(int from, int to)
{
    if (!hasRoad(from, to)) {
        return false;
    }

    // Remove both copies of the road.
    removeEdge(from, to);
    removeEdge(to, from);
    return true;
}

void Graph::removeEdge(int from, int to)
{
    std::vector<Edge>& edges = adjacency[from];
    for (size_t i = 0; i < edges.size(); i++) {
        if (edges[i].to == to) {
            edges.erase(edges.begin() + i);
            return;   // there is at most one road between two locations
        }
    }
}

// Time: O(log V + deg)
bool Graph::hasRoad(int from, int to) const
{
    if (!hasLocation(from)) {
        return false;
    }
    for (const Edge& edge : adjacency.at(from)) {
        if (edge.to == to) {
            return true;
        }
    }
    return false;
}

std::vector<Edge> Graph::getNeighbors(int id) const
{
    if (!hasLocation(id)) {
        return std::vector<Edge>();
    }
    return adjacency.at(id);
}

// Time: O(V + E)
std::vector<Road> Graph::getRoads() const
{
    std::vector<Road> roads;
    for (const auto& entry : adjacency) {
        int from = entry.first;
        for (const Edge& edge : entry.second) {
            // Each road is stored twice (A->B and B->A).
            // Keep only the copy where from < to, so every road is listed once.
            if (from < edge.to) {
                roads.push_back({from, edge.to, edge.distance});
            }
        }
    }
    return roads;
}

int Graph::roadCount() const
{
    return static_cast<int>(getRoads().size());
}
