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
    // Returns -1 if the name is empty (or only spaces) or already used.
    int addLocation(const std::string& name);

    // Adds a location with a given ID. Used when loading saved data, so that
    // IDs stay the same and saved roads still point to the right locations.
    // Returns false if the ID is not positive or already used, or the name
    // is empty or already used.
    bool addLocationWithId(int id, const std::string& name);

    // Changes a location's name. Returns false if the location does not exist,
    // or the new name is empty or used by another location.
    bool renameLocation(int id, const std::string& newName);

    // Deletes a location and every road connected to it.
    // Returns false if the location does not exist.
    bool removeLocation(int id);

    bool hasLocation(int id) const;
    bool nameExists(const std::string& name) const;
    std::string getName(int id) const;          // "" if the ID does not exist
    std::vector<int> getLocationIds() const;    // all IDs in increasing order
    int locationCount() const;

    // The ID that the next new location will get. It only ever increases, so
    // the ID of a deleted location is never given to another location.
    int getNextId() const;

    // Used when loading saved data. Raises the next ID to `id`; a value lower
    // than the current next ID is ignored, so an ID can never be reused.
    void setNextId(int id);

    // ----- Roads -----

    // Adds a two-way road. Returns false (and adds nothing) if:
    //   - either location does not exist
    //   - both ends are the same location
    //   - the distance is not a positive number
    //   - a road between these two locations already exists
    bool addRoad(int from, int to, double distance);

    // Changes the distance of an existing road (in both directions).
    // Returns false if the road does not exist or the distance is not positive.
    bool updateRoad(int from, int to, double distance);

    // Deletes an existing road (in both directions).
    // Returns false if the road does not exist.
    bool removeRoad(int from, int to);

    bool hasRoad(int from, int to) const;
    std::vector<Edge> getNeighbors(int id) const;   // roads starting at this location
    std::vector<Road> getRoads() const;             // every road once
    int roadCount() const;

private:
    // Removes the road to `to` from the road list of `from` (one direction only).
    void removeEdge(int from, int to);

    std::map<int, std::string> names;            // location ID -> name
    std::map<int, std::vector<Edge>> adjacency;  // location ID -> its roads
    int nextId = 1;                              // ID for the next new location
};

#endif // GRAPH_H
