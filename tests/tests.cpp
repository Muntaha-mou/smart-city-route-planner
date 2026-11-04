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

    cout << "\n" << passed << " passed, " << failed << " failed\n";

    if (failed > 0) {
        return 1;
    }
    return 0;
}
