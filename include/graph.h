// Smart City Route & Infrastructure Planner
// Copyright © 2026 Quad Core. All rights reserved.
//
// graph.h - the city road network.
//
// The city is stored as an undirected, weighted graph using an ADJACENCY LIST:
//   - each location (vertex) has an ID and a name
//   - each location has a list of the roads (edges) that start from it
//   - a road's weight is its distance in kilometres
//
// This file has no console input/output, so it can be reused by a GUI later.

#ifndef GRAPH_H
#define GRAPH_H

#include <map>
#include <string>
#include <vector>

// One road, seen from one location: "this road goes to location `to`".
struct Edge {
    int to;            // ID of the location at the other end
    double distance;   // length of the road in km
};

// One road with both ends. Used when listing all roads.
struct Road {
    int from;
    int to;
    double distance;
};

class Graph {
public:
    // ----- Locations -----

    // Adds a location and returns its new ID (1, 2, 3, ...).
    // Returns -1 if the name is empty or already used.
    int addLocation(const std::string& name);

    bool hasLocation(int id) const;
    bool nameExists(const std::string& name) const;
    std::string getName(int id) const;          // "" if the ID does not exist
    std::vector<int> getLocationIds() const;    // all IDs in increasing order
    int locationCount() const;

    // ----- Roads -----

    // Adds a two-way road. Returns false (and adds nothing) if:
    //   - either location does not exist
    //   - both ends are the same location
    //   - the distance is not a positive number
    //   - a road between these two locations already exists
    bool addRoad(int from, int to, double distance);

    bool hasRoad(int from, int to) const;
    std::vector<Edge> getNeighbors(int id) const;   // roads starting at this location
    std::vector<Road> getRoads() const;             // every road once
    int roadCount() const;

private:
    std::map<int, std::string> names;            // location ID -> name
    std::map<int, std::vector<Edge>> adjacency;  // location ID -> its roads
    int nextId = 1;                              // ID for the next new location
};

#endif // GRAPH_H
