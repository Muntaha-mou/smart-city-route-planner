// Smart City Route & Infrastructure Planner
// Copyright © 2026 Quad Core. All rights reserved.
//
// main.cpp - the menu program.
//
// The program shows a menu, reads the user's choice and calls one function
// for each option. All city data is stored in a Graph object (see graph.h).

#include "graph.h"

#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// ===== Reading input =====

// Shows a prompt and reads one whole line typed by the user.
string readLine(const string& prompt)
{
    cout << prompt;
    string line;
    if (!getline(cin, line)) {
        // The input has ended (for example Ctrl+Z on Windows).
        cout << "\nInput ended. Goodbye!\n";
        exit(0);
    }
    return line;
}

// Reads a whole number. Keeps asking until the input is valid.
// "12" is accepted; "12abc", "1 2" and "abc" are rejected.
int readInt(const string& prompt)
{
    while (true) {
        string line = readLine(prompt);
        stringstream input(line);
        int number;
        char extra;
        // Valid only if a number is read AND nothing else follows it.
        if (input >> number && !(input >> extra)) {
            return number;
        }
        cout << "  Invalid input. Please enter a whole number.\n";
    }
}

// Reads a decimal number such as 4.5. Keeps asking until the input is valid.
double readDouble(const string& prompt)
{
    while (true) {
        string line = readLine(prompt);
        stringstream input(line);
        double number;
        char extra;
        // isfinite() rejects values that are too large to store ("infinity").
        if (input >> number && !(input >> extra) && isfinite(number)) {
            return number;
        }
        cout << "  Invalid input. Please enter a number, for example 4.5\n";
    }
}

// ===== Menu options =====

// Option 1: add a new location.
void addLocation(Graph& city)
{
    string name = readLine("Location name: ");

    // A name made only of spaces counts as empty.
    if (name.find_first_not_of(' ') == string::npos) {
        cout << "  Error: the name cannot be empty.\n";
    } else if (name.find(',') != string::npos) {
        cout << "  Error: the name cannot contain a comma.\n";
    } else if (city.nameExists(name)) {
        cout << "  Error: a location named \"" << name << "\" already exists.\n";
    } else {
        int id = city.addLocation(name);
        cout << "  Location \"" << name << "\" added with ID " << id << ".\n";
    }
}

// Option 2: show all locations in a table.
void showLocations(const Graph& city)
{
    if (city.locationCount() == 0) {
        cout << "  No locations yet.\n";
        return;
    }

    cout << "\n  " << left << setw(6) << "ID" << setw(25) << "Name" << "Roads\n";
    cout << "  " << string(36, '-') << "\n";
    for (int id : city.getLocationIds()) {
        cout << "  " << left << setw(6) << id << setw(25) << city.getName(id)
             << city.getNeighbors(id).size() << "\n";
    }
}

// Option 3: add a new road between two locations.
void addRoad(Graph& city)
{
    if (city.locationCount() < 2) {
        cout << "  Error: add at least two locations first.\n";
        return;
    }

    showLocations(city);
    cout << "\n";
    int from = readInt("From location ID: ");
    int to = readInt("To location ID: ");
    double distance = readDouble("Distance in km: ");

    // Check each rule and tell the user what is wrong.
    if (!city.hasLocation(from)) {
        cout << "  Error: location " << from << " does not exist.\n";
    } else if (!city.hasLocation(to)) {
        cout << "  Error: location " << to << " does not exist.\n";
    } else if (from == to) {
        cout << "  Error: a road must connect two different locations.\n";
    } else if (distance <= 0) {
        cout << "  Error: the distance must be greater than 0.\n";
    } else if (city.hasRoad(from, to)) {
        cout << "  Error: a road between these two locations already exists.\n";
    } else {
        city.addRoad(from, to, distance);
        cout << "  Road added: " << city.getName(from) << " <-> " << city.getName(to)
             << " (" << distance << " km)\n";
    }
}

// Option 4: show all roads in a table.
void showRoads(const Graph& city)
{
    vector<Road> roads = city.getRoads();
    if (roads.empty()) {
        cout << "  No roads yet.\n";
        return;
    }

    cout << "\n  " << left << setw(25) << "From" << setw(25) << "To" << "Distance (km)\n";
    cout << "  " << string(63, '-') << "\n";
    for (const Road& road : roads) {
        cout << "  " << left << setw(25) << city.getName(road.from)
             << setw(25) << city.getName(road.to) << road.distance << "\n";
    }
}

// Option 5: show the network as an adjacency list
// (every location followed by the locations it is connected to).
void showNetwork(const Graph& city)
{
    if (city.locationCount() == 0) {
        cout << "  The network is empty.\n";
        return;
    }

    cout << "\n";
    for (int id : city.getLocationIds()) {
        cout << "  [" << id << "] " << city.getName(id) << "\n";

        vector<Edge> roads = city.getNeighbors(id);
        if (roads.empty()) {
            cout << "        (no roads)\n";
        }
        for (const Edge& edge : roads) {
            cout << "        <-> " << city.getName(edge.to) << " [" << edge.to << "]  "
                 << edge.distance << " km\n";
        }
    }
}

// ===== Main program =====

int main()
{
    Graph city;

    // Show every decimal number with 2 digits after the point, e.g. 4.50
    cout << fixed << setprecision(2);

    cout << "==================================================\n";
    cout << "    SMART CITY ROUTE & INFRASTRUCTURE PLANNER\n";
    cout << "    Team Quad Core - CSE 232 Algorithms Project\n";
    cout << "==================================================\n";

    while (true) {
        cout << "\n----------------- MAIN MENU ----------------------\n";
        cout << "  Locations: " << city.locationCount()
             << "    Roads: " << city.roadCount() << "\n\n";
        cout << "  1. Add location\n";
        cout << "  2. Show locations\n";
        cout << "  3. Add road\n";
        cout << "  4. Show roads\n";
        cout << "  5. Show city network\n";
        cout << "  0. Exit\n";
        cout << "--------------------------------------------------\n";

        int choice = readInt("Choose an option: ");
        cout << "\n";

        if (choice == 1) {
            addLocation(city);
        } else if (choice == 2) {
            showLocations(city);
        } else if (choice == 3) {
            addRoad(city);
        } else if (choice == 4) {
            showRoads(city);
        } else if (choice == 5) {
            showNetwork(city);
        } else if (choice == 0) {
            cout << "Goodbye!\n";
            break;
        } else {
            cout << "  Please choose a number from the menu.\n";
        }
    }

    return 0;
}
