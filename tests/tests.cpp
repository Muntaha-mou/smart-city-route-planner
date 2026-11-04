// Smart City Route & Infrastructure Planner
// Copyright © 2026 Quad Core. All rights reserved.
//
// tests.cpp - automatic checks for the Graph class.
//
// How it works: check() receives a condition that should be true.
// It prints PASS if the condition is true and FAIL if it is false.
// At the end the program returns 1 if anything failed, so CTest
// reports the test as failed.

#include "algorithms.h"
#include "graph.h"
#include "storage.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
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

// ===== S2: update and delete =====

// A small city used by several tests:
//   1 BUBT - 2 Mirpur 10 (2.5 km), 2 Mirpur 10 - 3 Farmgate (4.0 km),
//   1 BUBT - 3 Farmgate (6.0 km), 4 Uttara has no roads.
Graph makeSmallCity()
{
    Graph city;
    city.addLocation("BUBT");        // ID 1
    city.addLocation("Mirpur 10");   // ID 2
    city.addLocation("Farmgate");    // ID 3
    city.addLocation("Uttara");      // ID 4
    city.addRoad(1, 2, 2.5);
    city.addRoad(2, 3, 4.0);
    city.addRoad(1, 3, 6.0);
    return city;
}

void testRenameLocation()
{
    cout << "\nRename location\n";
    Graph city = makeSmallCity();

    check(city.renameLocation(1, "BUBT Campus"), "rename BUBT to BUBT Campus");
    check(city.getName(1) == "BUBT Campus", "the new name is stored");
    check(!city.nameExists("BUBT"), "the old name is gone");
    check(city.renameLocation(1, "BUBT Campus"), "keeping the same name is allowed");
    check(!city.renameLocation(1, "Farmgate"), "a name used by another location is refused");
    check(!city.renameLocation(1, ""), "an empty name is refused");
    check(!city.renameLocation(99, "Nowhere"), "renaming an unknown location is refused");
    check(city.getName(1) == "BUBT Campus", "refused renames change nothing");
    check(city.getNeighbors(1).size() == 2, "roads are not affected by renaming");
}

void testRemoveLocation()
{
    cout << "\nDelete location\n";
    Graph city = makeSmallCity();

    check(city.removeLocation(2), "delete Mirpur 10 (ID 2)");
    check(!city.hasLocation(2), "location 2 no longer exists");
    check(city.locationCount() == 3, "3 locations remain");
    check(!city.hasRoad(1, 2) && !city.hasRoad(2, 1), "road BUBT - Mirpur 10 is gone");
    check(!city.hasRoad(3, 2), "road Farmgate - Mirpur 10 is gone from Farmgate's list");
    check(city.hasRoad(1, 3), "the unrelated road BUBT - Farmgate is kept");
    check(city.roadCount() == 1, "only 1 road remains");
    check(city.getNeighbors(1).size() == 1, "BUBT now has 1 neighbour");
    check(city.getNeighbors(3).size() == 1, "Farmgate now has 1 neighbour");

    check(!city.removeLocation(2), "deleting it again is refused");
    check(!city.removeLocation(99), "deleting an unknown location is refused");
    check(city.removeLocation(4), "a location without roads can be deleted");

    check(city.addLocation("Gulshan") == 5, "a new location gets a new ID (5), not a deleted one");
    check(city.addLocation("Mirpur 10") == 6, "the deleted name can be used again");
}

void testUpdateRoad()
{
    cout << "\nUpdate road\n";
    Graph city = makeSmallCity();

    check(city.updateRoad(1, 2, 3.5), "change BUBT - Mirpur 10 to 3.5 km");
    check(city.getNeighbors(1)[0].distance == 3.5, "the new distance is stored at BUBT");
    check(city.getNeighbors(2)[0].distance == 3.5, "the new distance is stored at Mirpur 10 too");
    check(city.updateRoad(2, 1, 1.5), "the road can also be updated in reverse order");
    check(city.getRoads()[0].distance == 1.5, "getRoads shows the updated distance");

    check(!city.updateRoad(1, 4, 2.0), "updating a road that does not exist is refused");
    check(!city.updateRoad(1, 2, 0.0), "distance 0 is refused");
    check(!city.updateRoad(1, 2, -3.0), "negative distance is refused");
    check(!city.updateRoad(1, 2, INFINITY), "infinite distance is refused");
    check(!city.updateRoad(1, 2, NAN), "NaN distance is refused");
    check(city.getRoads()[0].distance == 1.5, "refused updates change nothing");
    check(city.roadCount() == 3, "the number of roads is unchanged");
}

void testRemoveRoad()
{
    cout << "\nDelete road\n";
    Graph city = makeSmallCity();

    check(city.removeRoad(2, 1), "delete BUBT - Mirpur 10 (given in reverse order)");
    check(!city.hasRoad(1, 2) && !city.hasRoad(2, 1), "the road is gone in both directions");
    check(city.roadCount() == 2, "2 roads remain");
    check(city.hasLocation(1) && city.hasLocation(2), "both locations still exist");
    check(!city.removeRoad(1, 2), "deleting it again is refused");
    check(!city.removeRoad(1, 4), "deleting a road that does not exist is refused");
    check(city.addRoad(1, 2, 5.0), "the road can be added again");
}

void testAddLocationWithId()
{
    cout << "\nAdd location with a given ID (used when loading)\n";
    Graph city;

    check(city.addLocationWithId(5, "Gulshan"), "add Gulshan with ID 5");
    check(city.addLocationWithId(2, "Agargaon"), "add Agargaon with ID 2");
    check(!city.addLocationWithId(5, "Banani"), "an ID that is already used is refused");
    check(!city.addLocationWithId(7, "Gulshan"), "a name that is already used is refused");
    check(!city.addLocationWithId(0, "Banani"), "ID 0 is refused");
    check(!city.addLocationWithId(-1, "Banani"), "a negative ID is refused");
    check(!city.addLocationWithId(8, ""), "an empty name is refused");
    check(!city.addLocationWithId(8, "   "), "a name of only spaces is refused");
    check(city.addLocation("Banani") == 6, "the next new location gets ID 6 (after the highest ID)");
}

void testNextId()
{
    cout << "\nNext location ID\n";
    Graph city;
    check(city.getNextId() == 1, "a new graph starts with next ID 1");
    city.addLocation("BUBT");
    check(city.getNextId() == 2, "after one location the next ID is 2");

    city.setNextId(10);
    check(city.getNextId() == 10, "setNextId can raise the next ID to 10");
    city.setNextId(4);
    check(city.getNextId() == 10, "setNextId cannot lower the next ID");
    check(city.addLocation("Uttara") == 10, "the next location gets ID 10");
    check(city.addLocation("\t ") == -1, "a name of only spaces and tabs is refused");
}

// ===== S2: saving and loading =====

const string TEST_LOCATIONS = "test_data/locations.csv";
const string TEST_ROADS = "test_data/roads.csv";
const string TEST_NEXT_ID = "test_data/next_id.txt";

// Writes text into a file (used to create test CSV files).
void writeFile(const string& file, const string& text)
{
    ofstream output(file);
    output << text;
}

void testSaveAndLoadEmpty()
{
    cout << "\nSave and load an empty city\n";
    Graph empty;
    check(saveCity(empty, TEST_LOCATIONS, TEST_ROADS, TEST_NEXT_ID), "an empty city is saved");

    Graph loaded;
    vector<string> messages;
    loadCity(loaded, TEST_LOCATIONS, TEST_ROADS, TEST_NEXT_ID, messages);
    check(loaded.locationCount() == 0, "no locations are loaded");
    check(loaded.roadCount() == 0, "no roads are loaded");
    check(messages.empty(), "there are no warnings");

    // '<', '>' and '|' are not allowed in Windows folder names.
    bool saved = saveCity(empty, "test_data/bad<|>/locations.csv", "test_data/bad<|>/roads.csv",
                          "test_data/bad<|>/next_id.txt");
    check(!saved, "saving to an impossible folder returns false (and does not crash)");
}

void testSaveAndLoadCity()
{
    cout << "\nSave and load a city\n";
    Graph city = makeSmallCity();
    city.removeLocation(2);              // leaves IDs 1, 3 and 4 (a gap at 2)
    city.addLocation("Gulshan");         // ID 5
    city.addRoad(3, 5, 7.25);
    check(saveCity(city, TEST_LOCATIONS, TEST_ROADS, TEST_NEXT_ID), "the city is saved");

    Graph loaded;
    vector<string> messages;
    loadCity(loaded, TEST_LOCATIONS, TEST_ROADS, TEST_NEXT_ID, messages);
    check(messages.empty(), "there are no warnings");
    check(loaded.locationCount() == 4, "4 locations are loaded");
    check(loaded.roadCount() == 2, "2 roads are loaded");

    vector<int> ids = loaded.getLocationIds();
    check(ids.size() == 4 && ids[0] == 1 && ids[1] == 3 && ids[2] == 4 && ids[3] == 5,
          "location IDs are kept exactly (1, 3, 4, 5)");
    check(loaded.getName(5) == "Gulshan", "names are kept (ID 5 is Gulshan)");
    check(loaded.hasRoad(1, 3) && loaded.hasRoad(3, 1), "road BUBT - Farmgate is loaded in both directions");
    check(loaded.hasRoad(3, 5), "road Farmgate - Gulshan is loaded");
    check(loaded.getRoads()[1].distance == 7.25, "the distance 7.25 is loaded exactly");
    check(loaded.addLocation("Banani") == 6, "after loading, the next new location gets ID 6");
}

void testLoadMissingFiles()
{
    cout << "\nLoad when the files do not exist\n";
    Graph city;
    vector<string> messages;
    loadCity(city, "test_data/missing_locations.csv", "test_data/missing_roads.csv",
             "test_data/missing_next_id.txt", messages);
    check(city.locationCount() == 0 && city.roadCount() == 0, "the city stays empty");
    check(messages.size() == 2, "a note for each missing CSV file (no note for next_id of an empty city)");
    check(city.addLocation("BUBT") == 1, "a new empty city starts at ID 1");
}

void testIdsNotReusedAfterRestart()
{
    cout << "\nIDs are not reused after deleting and restarting\n";

    // First run: add three locations, delete the one with the highest ID.
    Graph firstRun;
    firstRun.addLocation("BUBT");        // ID 1
    firstRun.addLocation("Mirpur 10");   // ID 2
    firstRun.addLocation("Farmgate");    // ID 3
    firstRun.removeLocation(3);
    check(saveCity(firstRun, TEST_LOCATIONS, TEST_ROADS, TEST_NEXT_ID), "the city is saved");

    // Second run ("restart"): load the files into a new Graph.
    Graph secondRun;
    vector<string> messages;
    loadCity(secondRun, TEST_LOCATIONS, TEST_ROADS, TEST_NEXT_ID, messages);
    check(messages.empty(), "there are no warnings");
    check(secondRun.locationCount() == 2, "2 locations are loaded");
    check(secondRun.getNextId() == 4, "the saved next ID (4) is loaded");
    check(secondRun.addLocation("Gulshan") == 4, "the new location gets ID 4, not the deleted ID 3");

    // Delete every location, save, and restart again.
    secondRun.removeLocation(1);
    secondRun.removeLocation(2);
    secondRun.removeLocation(4);
    saveCity(secondRun, TEST_LOCATIONS, TEST_ROADS, TEST_NEXT_ID);

    Graph thirdRun;
    messages.clear();
    loadCity(thirdRun, TEST_LOCATIONS, TEST_ROADS, TEST_NEXT_ID, messages);
    check(thirdRun.locationCount() == 0, "the city is empty after deleting everything");
    check(thirdRun.addLocation("Uttara") == 5, "even an emptied city continues with ID 5");
}

void testInvalidNextIdFile()
{
    cout << "\nMissing or invalid next_id.txt\n";
    // Saved locations with IDs 1 and 5, so "highest ID + 1" is 6.
    writeFile(TEST_LOCATIONS, "id,name\n1,BUBT\n5,Gulshan\n");
    writeFile(TEST_ROADS, "from,to,distance\n");

    // Each case: the content of next_id.txt, the expected next ID,
    // and whether a warning is expected.
    vector<string> contents = {"abc", "", "0", "-3", "3", "6x", "10", "6"};
    vector<int> expectedIds = {6, 6, 6, 6, 6, 6, 10, 6};
    vector<bool> expectWarning = {true, true, true, true, true, true, false, false};

    for (size_t i = 0; i < contents.size(); i++) {
        writeFile(TEST_NEXT_ID, contents[i] + "\n");
        Graph city;
        vector<string> messages;
        loadCity(city, TEST_LOCATIONS, TEST_ROADS, TEST_NEXT_ID, messages);
        bool ok = city.getNextId() == expectedIds[i] && (messages.size() == 1) == expectWarning[i];
        check(ok, "next_id.txt containing \"" + contents[i] + "\" gives next ID " +
                  to_string(expectedIds[i]));
    }

    // The file does not exist at all.
    remove(TEST_NEXT_ID.c_str());
    Graph city;
    vector<string> messages;
    loadCity(city, TEST_LOCATIONS, TEST_ROADS, TEST_NEXT_ID, messages);
    check(city.getNextId() == 6, "a missing next_id.txt gives next ID 6 (highest ID + 1)");
    check(messages.size() == 1, "a note explains that next_id.txt is missing");
}

void testLoadInvalidRecords()
{
    cout << "\nLoad files with invalid lines\n";
    writeFile(TEST_LOCATIONS,
              "id,name\n"
              "1,BUBT\n"
              "2,Mirpur 10\r\n"        // Windows line ending: accepted
              "\n"                     // empty line: ignored
              "abc,Farmgate\n"         // ID is not a number
              "3\n"                    // missing name
              "4,Uttara,extra\n"       // too many values
              "1,Gulshan\n"            // duplicate ID
              "5,BUBT\n"               // duplicate name
              "0,Banani\n"             // ID not positive
              "6, \n"                  // empty name
              "7,Agargaon\n"
              "8,\t\n");               // name of only a tab
    writeFile(TEST_NEXT_ID, "8\n");
    writeFile(TEST_ROADS,
              "from,to,distance\n"
              "1,2,2.5\n"
              "2,1,9\n"                // same road again (reverse order)
              "1,7,3.5\n"
              "1,1,1\n"                // road to itself
              "1,99,4\n"               // unknown location
              "2,7,-2\n"               // negative distance
              "2,7,abc\n"              // distance is not a number
              "2,7,inf\n"              // not a finite number
              "2,7\n"                  // missing distance
              "x,7,3\n");              // ID is not a number

    Graph city;
    vector<string> messages;
    loadCity(city, TEST_LOCATIONS, TEST_ROADS, TEST_NEXT_ID, messages);
    check(city.locationCount() == 3, "only the 3 valid locations are loaded (1, 2, 7)");
    check(city.hasLocation(1) && city.hasLocation(2) && city.hasLocation(7), "IDs 1, 2 and 7 exist");
    check(city.getName(2) == "Mirpur 10", "the Windows line ending is removed from the name");
    check(city.roadCount() == 2, "only the 2 valid roads are loaded");
    check(city.hasRoad(1, 2) && city.hasRoad(1, 7), "roads 1 - 2 and 1 - 7 exist");
    check(city.getRoads()[0].distance == 2.5, "the duplicate road did not replace the first one");
    check(messages.size() == 8 + 8, "one warning for each of the 16 invalid lines");
    check(city.getNextId() == 8, "the next ID is 8 (highest loaded ID 7 + 1, as saved)");
}

void testLoadWithoutHeader()
{
    cout << "\nLoad files without a header line\n";
    writeFile(TEST_LOCATIONS, "1,BUBT\n2,Mirpur 10\n");
    writeFile(TEST_ROADS, "1,2,2.5\n");
    writeFile(TEST_NEXT_ID, "3\n");

    Graph city;
    vector<string> messages;
    loadCity(city, TEST_LOCATIONS, TEST_ROADS, TEST_NEXT_ID, messages);
    check(city.locationCount() == 2, "the first line is read as a location, not skipped");
    check(city.roadCount() == 1, "the first line is read as a road, not skipped");
    check(messages.empty(), "there are no warnings");
}

// ===== S3: BFS and DFS =====

// The test network (roads are added in a mixed-up order on purpose, to check
// that neighbours are always visited in increasing ID order):
//
//    1 --- 2
//    |     |          6 (no roads)        7 --- 8
//    3 --- 4 --- 5
//
Graph makeTraversalCity()
{
    Graph city;
    for (int i = 1; i <= 8; i++) {
        city.addLocation("L" + to_string(i));   // IDs 1 to 8
    }
    city.addRoad(4, 5, 1.0);
    city.addRoad(1, 3, 1.0);
    city.addRoad(3, 4, 1.0);
    city.addRoad(2, 4, 1.0);
    city.addRoad(1, 2, 1.0);
    city.addRoad(8, 7, 1.0);
    return city;
}

// True if no ID appears twice in the list.
bool allDifferent(const vector<int>& ids)
{
    for (size_t i = 0; i < ids.size(); i++) {
        for (size_t j = i + 1; j < ids.size(); j++) {
            if (ids[i] == ids[j]) {
                return false;
            }
        }
    }
    return true;
}

void testBfs()
{
    cout << "\nBreadth-First Search (BFS)\n";
    Graph empty;
    check(bfs(empty, 1).empty(), "BFS on an empty city returns nothing");

    Graph city = makeTraversalCity();
    check(bfs(city, 99).empty(), "BFS from an unknown location returns nothing");
    check(bfs(city, 0).empty(), "BFS from ID 0 returns nothing");

    Graph single;
    single.addLocation("BUBT");
    check(bfs(single, 1) == vector<int>{1}, "BFS in a city with one location visits only it");

    check(bfs(city, 1) == vector<int>({1, 2, 3, 4, 5}), "BFS from 1 gives 1, 2, 3, 4, 5 (level by level)");
    check(bfs(city, 5) == vector<int>({5, 4, 2, 3, 1}), "BFS from 5 gives 5, 4, 2, 3, 1");
    check(bfs(city, 3) == vector<int>({3, 1, 4, 2, 5}), "BFS from 3 gives 3, 1, 4, 2, 5");

    // Disconnected parts: only the start's own part is visited.
    check(bfs(city, 6) == vector<int>{6}, "BFS from the isolated location 6 visits only 6");
    check(bfs(city, 7) == vector<int>({7, 8}), "BFS from 7 visits only its part: 7, 8");
    check(bfs(city, 1).size() == 5, "BFS from 1 does not reach 6, 7 or 8");

    // The cycle 1-2-4-3-1 must not cause a location to be visited twice.
    check(allDifferent(bfs(city, 1)), "BFS visits each location only once (even with a cycle)");
}

void testDfs()
{
    cout << "\nDepth-First Search (DFS)\n";
    Graph empty;
    check(dfs(empty, 1).empty(), "DFS on an empty city returns nothing");

    Graph city = makeTraversalCity();
    check(dfs(city, 99).empty(), "DFS from an unknown location returns nothing");
    check(dfs(city, -1).empty(), "DFS from ID -1 returns nothing");

    Graph single;
    single.addLocation("BUBT");
    check(dfs(single, 1) == vector<int>{1}, "DFS in a city with one location visits only it");

    check(dfs(city, 1) == vector<int>({1, 2, 4, 3, 5}), "DFS from 1 gives 1, 2, 4, 3, 5 (deep first)");
    check(dfs(city, 5) == vector<int>({5, 4, 2, 1, 3}), "DFS from 5 gives 5, 4, 2, 1, 3");
    check(dfs(city, 3) == vector<int>({3, 1, 2, 4, 5}), "DFS from 3 gives 3, 1, 2, 4, 5");
    check(dfs(city, 1) != bfs(city, 1), "DFS and BFS give different orders from location 1");

    check(dfs(city, 6) == vector<int>{6}, "DFS from the isolated location 6 visits only 6");
    check(dfs(city, 8) == vector<int>({8, 7}), "DFS from 8 visits only its part: 8, 7");
    check(allDifferent(dfs(city, 1)), "DFS visits each location only once (even with a cycle)");
}

void testTraversalAfterChanges()
{
    cout << "\nBFS and DFS after changing the city\n";
    Graph city = makeTraversalCity();

    city.removeRoad(4, 5);   // 5 is now cut off from 1
    check(bfs(city, 1) == vector<int>({1, 2, 3, 4}), "after deleting road 4-5, BFS from 1 no longer reaches 5");
    check(dfs(city, 5) == vector<int>{5}, "DFS from 5 now visits only 5");

    city.addRoad(6, 7, 2.0);   // joins 6 to the part 7-8
    check(bfs(city, 6) == vector<int>({6, 7, 8}), "after adding road 6-7, BFS from 6 gives 6, 7, 8");

    city.removeLocation(2);   // removes roads 1-2 and 2-4
    check(bfs(city, 1) == vector<int>({1, 3, 4}), "after deleting location 2, BFS from 1 gives 1, 3, 4");
    check(dfs(city, 4) == vector<int>({4, 3, 1}), "after deleting location 2, DFS from 4 gives 4, 3, 1");
    check(bfs(city, 2).empty(), "BFS from the deleted location 2 returns nothing");

    // The order does not depend on the order in which roads were loaded.
    check(saveCity(city, TEST_LOCATIONS, TEST_ROADS, TEST_NEXT_ID), "the changed city is saved");
    Graph loaded;
    vector<string> messages;
    loadCity(loaded, TEST_LOCATIONS, TEST_ROADS, TEST_NEXT_ID, messages);
    check(bfs(loaded, 1) == bfs(city, 1) && dfs(loaded, 4) == dfs(city, 4),
          "BFS and DFS give the same order after saving and loading");
}

// ===== S4: Dijkstra's shortest route =====

// Compares two decimal numbers (sums such as 1.1 + 2.2 can differ very
// slightly from 3.3, so exact == is not safe for them).
bool nearlyEqual(double a, double b)
{
    return fabs(a - b) < 1e-9;
}

// True if `path` is a real route in the city whose road distances add up to
// `distance`: every two neighbouring IDs on the path must be joined by a road.
bool isValidRoute(const Graph& city, const vector<int>& path, double distance)
{
    double total = 0.0;
    for (size_t i = 0; i + 1 < path.size(); i++) {
        bool roadFound = false;
        for (const Edge& edge : city.getNeighbors(path[i])) {
            if (edge.to == path[i + 1]) {
                total += edge.distance;
                roadFound = true;
            }
        }
        if (!roadFound) {
            return false;
        }
    }
    return nearlyEqual(total, distance);
}

// The test network for Dijkstra (distances in km):
//
//         4
//    1 ------- 2           5 (no roads)
//    |  \      |
//    1    2    1           roads: 1-2 4, 1-3 1, 3-2 2, 2-4 1, 3-4 5
//    |      \  |
//    3 ------- 4   (3-4 is 5 km)
//
Graph makeRouteCity()
{
    Graph city;
    for (int i = 1; i <= 5; i++) {
        city.addLocation("R" + to_string(i));   // IDs 1 to 5
    }
    city.addRoad(1, 2, 4.0);
    city.addRoad(1, 3, 1.0);
    city.addRoad(3, 2, 2.0);
    city.addRoad(2, 4, 1.0);
    city.addRoad(3, 4, 5.0);
    return city;
}

void testDijkstraBasics()
{
    cout << "\nDijkstra: special cases\n";
    Graph empty;
    check(!dijkstra(empty, 1, 2).found, "an empty city has no route");

    Graph single;
    single.addLocation("BUBT");
    RouteResult same = dijkstra(single, 1, 1);
    check(same.found && same.distance == 0.0 && same.path == vector<int>{1},
          "one location: the route from 1 to 1 is just {1} with distance 0");
    check(!dijkstra(single, 1, 2).found, "one location: a route to an unknown ID is not found");

    Graph city = makeRouteCity();
    RouteResult self = dijkstra(city, 3, 3);
    check(self.found && self.distance == 0.0 && self.path == vector<int>{3},
          "start = destination gives distance 0 and the route {3}");

    check(!dijkstra(city, 99, 1).found, "an unknown start ID gives no route");
    check(!dijkstra(city, 1, 99).found, "an unknown destination ID gives no route");
    check(!dijkstra(city, 0, -1).found, "IDs 0 and -1 give no route");
    RouteResult invalid = dijkstra(city, 99, 1);
    check(invalid.path.empty() && invalid.distance == 0.0, "a route that is not found has an empty path");

    RouteResult none = dijkstra(city, 1, 5);
    check(!none.found && none.path.empty(), "no route to the unconnected location 5");
    check(!dijkstra(city, 5, 2).found, "no route from the unconnected location 5");
}

void testDijkstraRoutes()
{
    cout << "\nDijkstra: shortest routes\n";
    Graph city = makeRouteCity();

    RouteResult direct = dijkstra(city, 1, 3);
    check(direct.found && nearlyEqual(direct.distance, 1.0) && direct.path == vector<int>({1, 3}),
          "1 -> 3 uses the direct road: {1, 3}, 1 km");

    RouteResult indirect = dijkstra(city, 1, 2);
    check(nearlyEqual(indirect.distance, 3.0), "1 -> 2 is 3 km, shorter than the direct 4 km road");
    check(indirect.path == vector<int>({1, 3, 2}), "1 -> 2 goes through 3: {1, 3, 2}");

    RouteResult longRoute = dijkstra(city, 1, 4);
    check(nearlyEqual(longRoute.distance, 4.0), "1 -> 4 is 4 km");
    check(longRoute.path == vector<int>({1, 3, 2, 4}),
          "1 -> 4 uses 3 roads {1, 3, 2, 4} instead of a 2-road route (5 or 6 km)");
    check(isValidRoute(city, longRoute.path, longRoute.distance),
          "the route 1 -> 4 is made of real roads that add up to 4 km");

    RouteResult backwards = dijkstra(city, 4, 1);
    check(nearlyEqual(backwards.distance, 4.0) && backwards.path == vector<int>({4, 2, 3, 1}),
          "4 -> 1 is the same route backwards: {4, 2, 3, 1}, 4 km");

    RouteResult fromThree = dijkstra(city, 3, 4);
    check(nearlyEqual(fromThree.distance, 3.0) && fromThree.path == vector<int>({3, 2, 4}),
          "3 -> 4 is 3 km through 2, not the direct 5 km road");

    // Several equally short routes: 1-2-4 and 1-3-4 are both 2 km.
    // Roads are added in a mixed-up order on purpose.
    Graph square;
    for (int i = 1; i <= 4; i++) {
        square.addLocation("S" + to_string(i));
    }
    square.addRoad(3, 4, 1.0);
    square.addRoad(1, 3, 1.0);
    square.addRoad(2, 4, 1.0);
    square.addRoad(1, 2, 1.0);
    RouteResult tie = dijkstra(square, 1, 4);
    check(nearlyEqual(tie.distance, 2.0) && tie.path.size() == 3,
          "two equally short routes: the distance is 2 km with 2 roads");
    check(isValidRoute(square, tie.path, tie.distance), "the chosen route is made of real roads");
    check(tie.path == vector<int>({1, 2, 4}), "with a tie, the route through the smaller ID (2) is chosen");

    // Decimal distances add up correctly.
    Graph decimals;
    decimals.addLocation("A");
    decimals.addLocation("B");
    decimals.addLocation("C");
    decimals.addRoad(1, 2, 1.1);
    decimals.addRoad(2, 3, 2.2);
    decimals.addRoad(1, 3, 3.4);
    RouteResult sum = dijkstra(decimals, 1, 3);
    check(nearlyEqual(sum.distance, 3.3) && sum.path == vector<int>({1, 2, 3}),
          "1.1 + 2.2 = 3.3 km beats the direct 3.4 km road");
}

void testDijkstraAfterChanges()
{
    cout << "\nDijkstra after changing the city\n";
    Graph city = makeRouteCity();

    city.updateRoad(1, 3, 10.0);   // 1-3 becomes long: the direct road 1-2 wins
    RouteResult afterUpdate = dijkstra(city, 1, 2);
    check(nearlyEqual(afterUpdate.distance, 4.0) && afterUpdate.path == vector<int>({1, 2}),
          "after 1-3 becomes 10 km, 1 -> 2 uses the direct 4 km road");

    city.removeRoad(1, 2);   // now 1 can only leave through 3
    RouteResult afterDelete = dijkstra(city, 1, 2);
    check(nearlyEqual(afterDelete.distance, 12.0) && afterDelete.path == vector<int>({1, 3, 2}),
          "after deleting road 1-2, 1 -> 2 is 12 km through 3");

    city.addRoad(1, 5, 1.0);
    city.addRoad(5, 4, 1.0);   // a new short way: 1-5-4-2
    RouteResult afterAdd = dijkstra(city, 1, 2);
    check(nearlyEqual(afterAdd.distance, 3.0) && afterAdd.path == vector<int>({1, 5, 4, 2}),
          "after adding roads 1-5 and 5-4, 1 -> 2 is 3 km through 5 and 4");

    city.removeLocation(5);   // removes roads 1-5 and 5-4
    RouteResult afterRemove = dijkstra(city, 1, 2);
    check(nearlyEqual(afterRemove.distance, 12.0) && afterRemove.path == vector<int>({1, 3, 2}),
          "after deleting location 5, 1 -> 2 is 12 km through 3 again");
    check(!dijkstra(city, 1, 5).found, "there is no route to the deleted location 5");

    city.removeLocation(3);   // 1 now has no roads at all
    check(!dijkstra(city, 1, 2).found, "after deleting location 3, location 1 is cut off");

    // The same results after saving and loading.
    Graph original = makeRouteCity();
    check(saveCity(original, TEST_LOCATIONS, TEST_ROADS, TEST_NEXT_ID), "the route city is saved");
    Graph loaded;
    vector<string> messages;
    loadCity(loaded, TEST_LOCATIONS, TEST_ROADS, TEST_NEXT_ID, messages);
    RouteResult before = dijkstra(original, 1, 4);
    RouteResult after = dijkstra(loaded, 1, 4);
    check(after.found && after.path == before.path && nearlyEqual(after.distance, before.distance),
          "after saving and loading, 1 -> 4 is the same route with the same distance");
}

// ===== S5: Kruskal's minimum spanning tree =====

// Builds a city with the same locations as `city` but only the roads that
// Kruskal selected.
Graph buildTreeCity(const Graph& city, const SpanningResult& tree)
{
    Graph treeCity;
    for (int id : city.getLocationIds()) {
        treeCity.addLocationWithId(id, city.getName(id));
    }
    for (const Road& road : tree.roads) {
        treeCity.addRoad(road.from, road.to, road.distance);
    }
    return treeCity;
}

// Checks that Kruskal's result is a correct spanning forest of `city`:
//  - every selected road is a real road with the right distance,
//  - there are exactly (V - components) roads (so there is no cycle),
//  - the total distance is the sum of the selected roads,
//  - every location reaches exactly the same locations as in the full city.
bool isSpanningForest(const Graph& city, const SpanningResult& tree)
{
    double total = 0.0;
    for (const Road& road : tree.roads) {
        if (!city.hasRoad(road.from, road.to)) {
            return false;
        }
        bool sameDistance = false;
        for (const Edge& edge : city.getNeighbors(road.from)) {
            if (edge.to == road.to && edge.distance == road.distance) {
                sameDistance = true;
            }
        }
        if (!sameDistance) {
            return false;
        }
        total += road.distance;
    }
    if (static_cast<int>(tree.roads.size()) != city.locationCount() - tree.components) {
        return false;
    }
    if (!nearlyEqual(total, tree.totalDistance)) {
        return false;
    }

    Graph treeCity = buildTreeCity(city, tree);
    for (int id : city.getLocationIds()) {
        vector<int> inCity = bfs(city, id);
        vector<int> inTree = bfs(treeCity, id);
        sort(inCity.begin(), inCity.end());
        sort(inTree.begin(), inTree.end());
        if (inCity != inTree) {
            return false;
        }
    }
    return true;
}

// True if the result contains the road between a and b (in either order).
bool selectsRoad(const SpanningResult& tree, int a, int b)
{
    for (const Road& road : tree.roads) {
        if ((road.from == a && road.to == b) || (road.from == b && road.to == a)) {
            return true;
        }
    }
    return false;
}

void testKruskalSmallCases()
{
    cout << "\nKruskal: small cases\n";
    Graph empty;
    SpanningResult none = kruskal(empty);
    check(none.roads.empty() && none.totalDistance == 0.0, "an empty city selects no roads");
    check(none.components == 0 && !none.connected, "an empty city has 0 components and is not connected");

    Graph single;
    single.addLocation("BUBT");
    SpanningResult one = kruskal(single);
    check(one.roads.empty() && one.totalDistance == 0.0, "one location: no roads are needed");
    check(one.components == 1 && one.connected, "one location: 1 component, connected");

    Graph pair;
    pair.addLocation("BUBT");
    pair.addLocation("Mirpur 10");
    pair.addRoad(1, 2, 2.5);
    SpanningResult two = kruskal(pair);
    check(two.roads.size() == 1 && selectsRoad(two, 1, 2), "two connected locations: their road is selected");
    check(nearlyEqual(two.totalDistance, 2.5) && two.components == 1 && two.connected,
          "two connected locations: 2.5 km, 1 component, connected");

    Graph noRoads;
    noRoads.addLocation("A");
    noRoads.addLocation("B");
    noRoads.addLocation("C");
    SpanningResult apart = kruskal(noRoads);
    check(apart.roads.empty() && apart.components == 3 && !apart.connected,
          "three locations without roads: no roads, 3 components");
}

void testKruskalConnected()
{
    cout << "\nKruskal: connected networks\n";

    // 1-2 (1), 2-3 (2), 1-3 (3), 3-4 (4), 2-4 (5)
    Graph city;
    for (int i = 1; i <= 4; i++) {
        city.addLocation("K" + to_string(i));
    }
    city.addRoad(2, 4, 5.0);
    city.addRoad(1, 3, 3.0);
    city.addRoad(3, 4, 4.0);
    city.addRoad(1, 2, 1.0);
    city.addRoad(2, 3, 2.0);
    SpanningResult tree = kruskal(city);
    check(tree.connected && tree.components == 1, "the network is connected (1 component)");
    check(tree.roads.size() == 3, "a tree of 4 locations has 3 roads (V - 1)");
    check(nearlyEqual(tree.totalDistance, 7.0), "the minimum total distance is 1 + 2 + 4 = 7 km");
    check(selectsRoad(tree, 1, 2) && selectsRoad(tree, 2, 3) && selectsRoad(tree, 3, 4),
          "roads 1-2, 2-3 and 3-4 are selected");
    check(!selectsRoad(tree, 1, 3) && !selectsRoad(tree, 2, 4),
          "roads 1-3 and 2-4 are skipped (each would make a cycle)");
    check(tree.roads[0].distance <= tree.roads[1].distance &&
          tree.roads[1].distance <= tree.roads[2].distance, "the roads are listed shortest first");
    check(isSpanningForest(city, tree), "the result is a valid spanning tree of the city");

    // The three shortest roads form a triangle (a cycle). Taking the 3
    // shortest roads would be wrong: DSU must skip one of them.
    Graph triangle;
    for (int i = 1; i <= 4; i++) {
        triangle.addLocation("T" + to_string(i));
    }
    triangle.addRoad(1, 2, 1.0);
    triangle.addRoad(2, 3, 1.0);
    triangle.addRoad(1, 3, 1.0);
    triangle.addRoad(3, 4, 10.0);
    SpanningResult triangleTree = kruskal(triangle);
    check(triangleTree.roads.size() == 3 && nearlyEqual(triangleTree.totalDistance, 12.0),
          "triangle + long road: 3 roads, 1 + 1 + 10 = 12 km");
    check(selectsRoad(triangleTree, 3, 4), "the long road 3-4 is needed to reach location 4");
    check(!selectsRoad(triangleTree, 2, 3), "one triangle road (2-3) is skipped to avoid a cycle");
    check(isSpanningForest(triangle, triangleTree), "the triangle result is a valid spanning tree");

    // Decimal distances.
    Graph decimals;
    decimals.addLocation("A");
    decimals.addLocation("B");
    decimals.addLocation("C");
    decimals.addRoad(1, 2, 1.1);
    decimals.addRoad(2, 3, 2.2);
    decimals.addRoad(1, 3, 3.4);
    SpanningResult decimalTree = kruskal(decimals);
    check(nearlyEqual(decimalTree.totalDistance, 3.3), "decimal distances: 1.1 + 2.2 = 3.3 km");
}

void testKruskalDisconnected()
{
    cout << "\nKruskal: disconnected networks (Minimum Spanning Forest)\n";

    // Part A: 1-2 (1), 2-3 (2), 1-3 (3)   Part B: 4-5 (4)   Part C: 6 alone
    Graph city;
    for (int i = 1; i <= 6; i++) {
        city.addLocation("F" + to_string(i));
    }
    city.addRoad(1, 2, 1.0);
    city.addRoad(2, 3, 2.0);
    city.addRoad(1, 3, 3.0);
    city.addRoad(4, 5, 4.0);
    SpanningResult forest = kruskal(city);
    check(!forest.connected && forest.components == 3, "the network has 3 components and is not connected");
    check(forest.roads.size() == 3, "the forest has 3 roads (V - components = 6 - 3)");
    check(nearlyEqual(forest.totalDistance, 7.0), "the forest total is 1 + 2 + 4 = 7 km");
    check(selectsRoad(forest, 4, 5) && !selectsRoad(forest, 1, 3),
          "road 4-5 is selected and road 1-3 is skipped");
    check(isSpanningForest(city, forest), "the result is a valid spanning forest");

    city.addRoad(3, 4, 9.0);   // joins part A and part B
    SpanningResult joined = kruskal(city);
    check(joined.components == 2 && joined.roads.size() == 4 && nearlyEqual(joined.totalDistance, 16.0),
          "after adding road 3-4: 2 components, 4 roads, 16 km");

    city.addRoad(6, 1, 0.5);   // joins location 6 as well
    SpanningResult tree = kruskal(city);
    check(tree.connected && tree.roads.size() == 5 && nearlyEqual(tree.totalDistance, 16.5),
          "after adding road 6-1: connected, 5 roads, 16.5 km");
}

void testKruskalEqualDistances()
{
    cout << "\nKruskal: equal distances and changes\n";

    // A square with one diagonal, all roads 1 km, added in a mixed-up order.
    // Sorted by (distance, from, to): 1-2, 1-3, 1-4, 2-3, 3-4.
    Graph square;
    for (int i = 1; i <= 4; i++) {
        square.addLocation("Q" + to_string(i));
    }
    square.addRoad(3, 4, 1.0);
    square.addRoad(2, 3, 1.0);
    square.addRoad(1, 4, 1.0);
    square.addRoad(1, 3, 1.0);
    square.addRoad(1, 2, 1.0);
    SpanningResult tree = kruskal(square);
    check(tree.roads.size() == 3 && nearlyEqual(tree.totalDistance, 3.0), "all roads equal: 3 roads, 3 km");
    check(selectsRoad(tree, 1, 2) && selectsRoad(tree, 1, 3) && selectsRoad(tree, 1, 4),
          "with equal distances, the roads with smaller IDs are chosen (1-2, 1-3, 1-4)");
    check(isSpanningForest(square, tree), "the equal-distance result is a valid spanning tree");

    // Same result after saving and loading.
    check(saveCity(square, TEST_LOCATIONS, TEST_ROADS, TEST_NEXT_ID), "the square city is saved");
    Graph loaded;
    vector<string> messages;
    loadCity(loaded, TEST_LOCATIONS, TEST_ROADS, TEST_NEXT_ID, messages);
    SpanningResult loadedTree = kruskal(loaded);
    bool sameRoads = loadedTree.roads.size() == tree.roads.size();
    for (size_t i = 0; sameRoads && i < tree.roads.size(); i++) {
        sameRoads = loadedTree.roads[i].from == tree.roads[i].from &&
                    loadedTree.roads[i].to == tree.roads[i].to;
    }
    check(sameRoads, "after saving and loading, the same roads are selected in the same order");

    // Changes to the city change the result.
    square.updateRoad(1, 3, 5.0);   // the diagonal becomes long
    SpanningResult afterUpdate = kruskal(square);
    check(!selectsRoad(afterUpdate, 1, 3) && nearlyEqual(afterUpdate.totalDistance, 3.0),
          "after the diagonal 1-3 becomes 5 km, it is not selected (still 3 km)");

    square.removeRoad(1, 2);
    square.removeRoad(1, 4);   // now 1 is only reachable through the diagonal
    SpanningResult afterDelete = kruskal(square);
    check(selectsRoad(afterDelete, 1, 3) && nearlyEqual(afterDelete.totalDistance, 7.0),
          "after deleting roads 1-2 and 1-4, the diagonal is needed (5 + 1 + 1 = 7 km)");

    square.removeLocation(3);   // removes 1-3, 2-3 and 3-4
    SpanningResult afterRemove = kruskal(square);
    check(afterRemove.roads.empty() && afterRemove.components == 3,
          "after deleting location 3: no roads left, 3 components");
}

// ===== S6.2: name rules and maximum road distance =====

void testNameTrimmingAndCase()
{
    cout << "\nNames: trimming and upper/lower case\n";
    check(trimName("  Mirpur 10  ") == "Mirpur 10", "trimName removes spaces at both ends");
    check(trimName("\tBUBT\r\n") == "BUBT", "trimName removes tabs and line breaks");
    check(trimName("Mirpur  10") == "Mirpur  10", "trimName keeps spaces inside the name");
    check(trimName("   ") == "", "a name of only spaces becomes empty");

    Graph city;
    check(city.addLocation("  BUBT  ") == 1, "a name with spaces around it is added");
    check(city.getName(1) == "BUBT", "the name is stored without the spaces");
    check(city.addLocation("BUBT ") == -1, "\"BUBT \" (trailing space) is a duplicate of BUBT");
    check(city.addLocation(" BUBT") == -1, "\" BUBT\" (leading space) is a duplicate of BUBT");
    check(city.addLocation("bubt") == -1, "\"bubt\" (lower case) is a duplicate of BUBT");
    check(city.addLocation("BuBt") == -1, "\"BuBt\" (mixed case) is a duplicate of BUBT");
    check(city.addLocation(" \t ") == -1, "a name that is empty after trimming is refused");
    check(city.locationCount() == 1, "none of the duplicates were added");

    check(city.addLocation("Mirpur 10") == 2, "a different name is still accepted");
    check(city.nameExists("MIRPUR 10") && city.nameExists("  mirpur 10 "),
          "nameExists ignores case and surrounding spaces");
    check(city.findLocationId("mirpur 10") == 2, "findLocationId finds Mirpur 10 as \"mirpur 10\"");
    check(city.findLocationId("Farmgate") == -1, "findLocationId returns -1 for an unknown name");
    check(city.getName(2) == "Mirpur 10", "the original capital letters are kept for display");

    Graph loaded;
    check(loaded.addLocationWithId(5, "  Gulshan ") && loaded.getName(5) == "Gulshan",
          "addLocationWithId trims the name");
    check(!loaded.addLocationWithId(6, "GULSHAN"), "addLocationWithId refuses a name that differs only in case");
    check(!loaded.addLocationWithId(7, "   "), "addLocationWithId refuses a name that is empty after trimming");
}

void testRenameWithNameRules()
{
    cout << "\nRename: trimming and upper/lower case\n";
    Graph city;
    city.addLocation("BUBT");        // ID 1
    city.addLocation("Farmgate");    // ID 2

    check(city.renameLocation(1, "Bubt"), "a location may change the capitals of its own name");
    check(city.getName(1) == "Bubt", "the new capitals are stored");
    check(city.renameLocation(1, "  BUBT Campus  ") && city.getName(1) == "BUBT Campus",
          "rename trims the new name");
    check(!city.renameLocation(1, "farmgate"), "rename to another location's name in lower case is refused");
    check(!city.renameLocation(1, " FARMGATE "), "rename to another location's name with spaces is refused");
    check(!city.renameLocation(1, "   "), "rename to a name that is empty after trimming is refused");
    check(city.getName(1) == "BUBT Campus", "refused renames change nothing");
}

void testLoadDuplicateLookingNames()
{
    cout << "\nLoad: names that look the same\n";
    writeFile(TEST_LOCATIONS,
              "id,name\n"
              "1,  BUBT  \n"          // spaces around the name: trimmed
              "2,bubt\n"              // same name in lower case: duplicate
              "3,BUBT \n"             // trailing space: duplicate
              "4, Mirpur 10\n"        // leading space: trimmed
              "5,MIRPUR 10\n"         // same name in upper case: duplicate
              "6,Farmgate\n");
    writeFile(TEST_ROADS, "from,to,distance\n1,4,2.5\n4,6,3\n");
    writeFile(TEST_NEXT_ID, "7\n");

    Graph city;
    vector<string> messages;
    loadCity(city, TEST_LOCATIONS, TEST_ROADS, TEST_NEXT_ID, messages);
    check(city.locationCount() == 3, "only 3 locations are loaded (1, 4, 6)");
    check(city.getName(1) == "BUBT" && city.getName(4) == "Mirpur 10", "the loaded names are trimmed");
    check(!city.hasLocation(2) && !city.hasLocation(3) && !city.hasLocation(5),
          "the duplicate-looking names (IDs 2, 3, 5) are skipped");
    check(messages.size() == 3, "one warning for each of the 3 duplicate names");
    check(city.roadCount() == 2, "the roads between the loaded locations are kept");

    // Saving writes the trimmed names, so the file is clean afterwards.
    check(saveCity(city, TEST_LOCATIONS, TEST_ROADS, TEST_NEXT_ID), "the city is saved");
    Graph reloaded;
    messages.clear();
    loadCity(reloaded, TEST_LOCATIONS, TEST_ROADS, TEST_NEXT_ID, messages);
    check(messages.empty() && reloaded.getName(1) == "BUBT" && reloaded.getName(4) == "Mirpur 10",
          "after saving and loading again, the names are clean and there are no warnings");
}

void testMaximumRoadDistance()
{
    cout << "\nMaximum road distance (" << MAX_ROAD_DISTANCE << " km)\n";
    check(MAX_ROAD_DISTANCE == 10000.0, "the maximum road distance is 10,000 km");
    check(isValidDistance(0.5) && isValidDistance(MAX_ROAD_DISTANCE), "0.5 km and exactly 10,000 km are valid");
    check(!isValidDistance(0.0) && !isValidDistance(-1.0), "0 km and negative distances are not valid");
    check(!isValidDistance(10000.001) && !isValidDistance(1e308), "distances above 10,000 km are not valid");
    check(!isValidDistance(INFINITY) && !isValidDistance(NAN), "infinity and NaN are not valid");

    Graph city;
    city.addLocation("A");   // 1
    city.addLocation("B");   // 2
    city.addLocation("C");   // 3

    check(!city.addRoad(1, 2, 0.0), "addRoad refuses 0 km");
    check(!city.addRoad(1, 2, -5.0), "addRoad refuses a negative distance");
    check(!city.addRoad(1, 2, INFINITY), "addRoad refuses infinity");
    check(!city.addRoad(1, 2, NAN), "addRoad refuses NaN");
    check(!city.addRoad(1, 2, 10000.5), "addRoad refuses 10,000.5 km");
    check(!city.addRoad(1, 2, 1e308), "addRoad refuses 1e308 km");
    check(city.roadCount() == 0, "no refused road was added");
    check(city.addRoad(1, 2, 10000.0), "addRoad accepts exactly 10,000 km");

    check(!city.updateRoad(1, 2, 0.0), "updateRoad refuses 0 km");
    check(!city.updateRoad(1, 2, -1.0), "updateRoad refuses a negative distance");
    check(!city.updateRoad(1, 2, INFINITY), "updateRoad refuses infinity");
    check(!city.updateRoad(1, 2, NAN), "updateRoad refuses NaN");
    check(!city.updateRoad(1, 2, 10001.0), "updateRoad refuses 10,001 km");
    check(city.getRoads()[0].distance == 10000.0, "refused updates change nothing");
    check(city.updateRoad(1, 2, 9999.5) && city.getRoads()[0].distance == 9999.5,
          "updateRoad accepts 9,999.5 km");
    check(city.updateRoad(1, 2, 10000.0), "updateRoad accepts exactly 10,000 km");

    // Loading: a distance above the maximum is skipped like any invalid line.
    writeFile(TEST_LOCATIONS, "id,name\n1,A\n2,B\n3,C\n");
    writeFile(TEST_ROADS, "from,to,distance\n1,2,10000\n2,3,10000.01\n1,3,1e308\n");
    writeFile(TEST_NEXT_ID, "4\n");
    Graph loaded;
    vector<string> messages;
    loadCity(loaded, TEST_LOCATIONS, TEST_ROADS, TEST_NEXT_ID, messages);
    check(loaded.roadCount() == 1 && loaded.hasRoad(1, 2), "only the 10,000 km road is loaded");
    check(messages.size() == 2, "the two roads above 10,000 km give one warning each");
}

void testAlgorithmsWithLargeDistances()
{
    cout << "\nDijkstra and Kruskal with the largest allowed distances\n";
    // A line of 4 locations joined by 10,000 km roads, plus a 1 km shortcut
    // road from 1 to 4. Before the limit, huge values (such as 1e308) could
    // add up to infinity; now every total is an ordinary number.
    Graph city;
    for (int i = 1; i <= 4; i++) {
        city.addLocation("M" + to_string(i));
    }
    city.addRoad(1, 2, MAX_ROAD_DISTANCE);
    city.addRoad(2, 3, MAX_ROAD_DISTANCE);
    city.addRoad(3, 4, MAX_ROAD_DISTANCE);

    RouteResult longRoute = dijkstra(city, 1, 4);
    check(longRoute.found && nearlyEqual(longRoute.distance, 30000.0),
          "1 -> 4 over three 10,000 km roads is found: 30,000 km");
    check(longRoute.path == vector<int>({1, 2, 3, 4}), "the route is {1, 2, 3, 4}");

    SpanningResult tree = kruskal(city);
    check(tree.connected && nearlyEqual(tree.totalDistance, 30000.0),
          "Kruskal total of three 10,000 km roads is 30,000 km");

    city.addRoad(1, 4, 1.0);
    RouteResult shortcut = dijkstra(city, 1, 4);
    check(nearlyEqual(shortcut.distance, 1.0) && shortcut.path == vector<int>({1, 4}),
          "with a 1 km road, 1 -> 4 uses it: 1 km");
    SpanningResult newTree = kruskal(city);
    // Sorted: 1-4 (1 km), then the equal roads 1-2, 2-3, 3-4 by ID.
    // 1-4, 1-2 and 2-3 connect everything, so 3-4 would make a cycle.
    check(nearlyEqual(newTree.totalDistance, 20001.0) && !selectsRoad(newTree, 3, 4),
          "Kruskal now keeps 1-4, 1-2 and 2-3 (20,001 km), skipping 3-4");
    check(isSpanningForest(city, newTree), "the result is still a valid spanning tree");
}

int main()
{
    testLocations();
    testRoads();
    testRenameLocation();
    testRemoveLocation();
    testUpdateRoad();
    testRemoveRoad();
    testAddLocationWithId();
    testNextId();
    testSaveAndLoadEmpty();
    testSaveAndLoadCity();
    testLoadMissingFiles();
    testIdsNotReusedAfterRestart();
    testInvalidNextIdFile();
    testLoadInvalidRecords();
    testLoadWithoutHeader();
    testBfs();
    testDfs();
    testTraversalAfterChanges();
    testDijkstraBasics();
    testDijkstraRoutes();
    testDijkstraAfterChanges();
    testKruskalSmallCases();
    testKruskalConnected();
    testKruskalDisconnected();
    testKruskalEqualDistances();
    testNameTrimmingAndCase();
    testRenameWithNameRules();
    testLoadDuplicateLookingNames();
    testMaximumRoadDistance();
    testAlgorithmsWithLargeDistances();

    cout << "\n" << passed << " passed, " << failed << " failed\n";

    if (failed > 0) {
        return 1;
    }
    return 0;
}
