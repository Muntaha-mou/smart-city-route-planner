// Smart City Route & Infrastructure Planner
// Copyright © 2026 Quad Core. All rights reserved.
//
// tests.cpp - automatic checks for the Graph class.
//
// How it works: check() receives a condition that should be true.
// It prints PASS if the condition is true and FAIL if it is false.
// At the end the program returns 1 if anything failed, so CTest
// reports the test as failed.

#include "graph.h"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int passed = 0;
int failed = 0;

void check(bool condition, const string& description)
{
    if (condition) {
        cout << "  PASS  " << description << "\n";
        passed++;
    } else {
        cout << "  FAIL  " << description << "\n";
        failed++;
    }
}

void testLocations()
{
    cout << "\nLocations\n";
    Graph city;
    check(city.locationCount() == 0, "a new graph has no locations");

    check(city.addLocation("BUBT") == 1, "first location gets ID 1");
    check(city.addLocation("Mirpur 10") == 2, "second location gets ID 2");
    check(city.addLocation("Farmgate") == 3, "third location gets ID 3");
    check(city.locationCount() == 3, "location count is 3");

    check(city.addLocation("") == -1, "empty name is refused");
    check(city.addLocation("BUBT") == -1, "duplicate name is refused");
    check(city.locationCount() == 3, "refused names are not added");
    check(city.addLocation("Uttara") == 4, "next ID continues after refused names");

    check(city.hasLocation(2), "location 2 exists");
    check(!city.hasLocation(99), "location 99 does not exist");
    check(city.getName(2) == "Mirpur 10", "name of location 2 is Mirpur 10");
    check(city.getName(99) == "", "name of an unknown location is empty");
    check(city.nameExists("Farmgate"), "the name Farmgate is found");

    vector<int> ids = city.getLocationIds();
    check(ids.size() == 4, "there are 4 location IDs");
    check(ids[0] == 1 && ids[3] == 4, "IDs are listed in order 1 to 4");
}

void testRoads()
{
    cout << "\nRoads\n";
    Graph city;
    city.addLocation("BUBT");        // ID 1
    city.addLocation("Mirpur 10");   // ID 2
    city.addLocation("Farmgate");    // ID 3

    check(city.addRoad(1, 2, 2.5), "add road BUBT - Mirpur 10");
    check(city.hasRoad(1, 2), "road exists from 1 to 2");
    check(city.hasRoad(2, 1), "road also exists from 2 to 1 (two-way)");
    check(city.getNeighbors(1).size() == 1, "BUBT has 1 neighbour");
    check(city.getNeighbors(2).size() == 1, "Mirpur 10 has 1 neighbour");
    check(city.getNeighbors(2)[0].to == 1, "Mirpur 10's neighbour is BUBT");
    check(city.getNeighbors(2)[0].distance == 2.5, "the stored distance is 2.5");

    // Roads that must be refused
    check(!city.addRoad(1, 2, 4.0), "duplicate road is refused");
    check(!city.addRoad(2, 1, 4.0), "duplicate road in reverse order is refused");
    check(!city.addRoad(1, 1, 3.0), "road from a location to itself is refused");
    check(!city.addRoad(1, 99, 3.0), "road to an unknown location is refused");
    check(!city.addRoad(99, 1, 3.0), "road from an unknown location is refused");
    check(!city.addRoad(1, 3, 0.0), "distance 0 is refused");
    check(!city.addRoad(1, 3, -5.0), "negative distance is refused");
    check(!city.addRoad(1, 3, INFINITY), "infinite distance is refused");
    check(!city.addRoad(1, 3, NAN), "NaN (not a number) distance is refused");
    check(city.roadCount() == 1, "refused roads are not added");

    check(city.addRoad(2, 3, 4.0), "add road Mirpur 10 - Farmgate");
    vector<Road> roads = city.getRoads();
    check(roads.size() == 2, "getRoads lists each road once");
    check(roads[0].from == 1 && roads[0].to == 2, "first road is 1 - 2");
    check(roads[1].from == 2 && roads[1].to == 3, "second road is 2 - 3");
    check(city.getNeighbors(2).size() == 2, "Mirpur 10 now has 2 neighbours");
    check(city.getNeighbors(99).empty(), "an unknown location has no neighbours");
}

int main()
{
    testLocations();
    testRoads();

    cout << "\n" << passed << " passed, " << failed << " failed\n";

    if (failed > 0) {
        return 1;
    }
    return 0;
}
