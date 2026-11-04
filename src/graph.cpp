// Smart City Route & Infrastructure Planner
// Copyright © 2026 Quad Core. All rights reserved.
//
// graph.cpp - implementation of the Graph class (see graph.h).
//
// Complexity notes: V = number of locations, E = number of roads,
// deg = number of roads at one location. std::map lookups take O(log V).

#include "graph.h"

#include <cmath>

// ===== Locations =====

// Time: O(V) because nameExists() checks every name.
int Graph::addLocation(const std::string& name)
{
    if (name.empty() || nameExists(name)) {
        return -1;
    }

    int id = nextId;
    nextId++;

    names[id] = name;
    adjacency[id] = std::vector<Edge>();   // a new location has no roads yet
    return id;
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
