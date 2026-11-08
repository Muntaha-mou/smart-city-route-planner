// Smart City Route & Infrastructure Planner
// Copyright © 2026 Quad Core. All rights reserved.
//
// main.cpp - the menu program.
//
// The program shows a menu, reads the user's choice and calls one function
// for each option. All city data is stored in a Graph object (see graph.h).
// The data is loaded from CSV files when the program starts and saved again
// after every change (see storage.h). Colours, borders and other screen
// helpers come from ui.h.

#include "algorithms.h"
#include "graph.h"
#include "storage.h"
#include "ui.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// Where the city is saved. main() chooses the folder (see chooseDataFolder).
string dataFolder = "data";
string locationsFile = "data/locations.csv";
string roadsFile = "data/roads.csv";
string nextIdFile = "data/next_id.txt";

// ===== Small display helpers =====

// "[3]" for location 3.
string idText(int id)
{
    return "[" + to_string(id) + "]";
}

// "BUBT [1]"
string nameWithId(const Graph& city, int id)
{
    return city.getName(id) + " " + idText(id);
}

// The symbol for a two-way road.
string twoWay()
{
    return colorsOn() ? "↔" : "<->";
}

// One table cell: the text fitted to `width` characters, in a colour.
string cell(const string& text, int width, Color color)
{
    return paint(fitText(text, width), color);
}

// ===== Reading input =====

// Shows a prompt and reads one whole line typed by the user.
string readLine(const string& question)
{
    cout << promptText(question);
    string line;
    if (!getline(cin, line)) {
        // The input has ended (for example Ctrl+Z on Windows).
        endInput();
        cout << "\n\n  Input ended. Goodbye!\n";
        exit(0);
    }
    endInput();
    return line;
}

// Reads a whole number. Keeps asking until the input is valid.
// "12" is accepted; "12abc", "1 2" and "abc" are rejected.
int readInt(const string& question)
{
    while (true) {
        string line = readLine(question);
        stringstream input(line);
        int number;
        char extra;
        // Valid only if a number is read AND nothing else follows it.
        if (input >> number && !(input >> extra)) {
            return number;
        }
        printError("Invalid input. Please enter a whole number.");
    }
}

// Reads a decimal number such as 4.5. Keeps asking until the input is valid.
double readDouble(const string& question)
{
    while (true) {
        string line = readLine(question);
        stringstream input(line);
        double number;
        char extra;
        // isfinite() rejects values that are too large to store ("infinity").
        if (input >> number && !(input >> extra) && isfinite(number)) {
            return number;
        }
        printError("Invalid input. Please enter a number, for example 4.5");
    }
}

// Asks a yes/no question. Returns true only for "y" or "Y".
bool readYes(const string& question)
{
    string answer = readLine(question);
    return answer == "y" || answer == "Y";
}

// Checks a location name typed by the user and prints the problem, if any.
// The name should already be trimmed (see trimName in graph.h).
// `ownId` is the location being renamed (0 when adding a new location),
// so that a location may keep its own name.
bool isValidName(const Graph& city, const string& name, int ownId)
{
    if (name.empty()) {
        printError("The name cannot be empty.");
        return false;
    }
    if (name.find(',') != string::npos) {
        printError("The name cannot contain a comma.");
        return false;
    }
    // Names are compared ignoring upper/lower case: "bubt" is the same as "BUBT".
    int owner = city.findLocationId(name);
    if (owner != -1 && owner != ownId) {
        printError("A location named \"" + city.getName(owner) + "\" already exists.");
        return false;
    }
    return true;
}

// The message shown when a distance is not allowed.
void printDistanceError()
{
    printError("The distance must be greater than 0 and at most " +
               formatNumber(MAX_ROAD_DISTANCE) + " km.");
}

// Saves the city after a change and reports a problem if saving failed.
void saveChanges(const Graph& city)
{
    if (saveCity(city, locationsFile, roadsFile, nextIdFile)) {
        printHint("Saved automatically to the data folder.");
    } else {
        printWarning("The data could not be saved to the data folder.");
    }
}

// ===== Tables =====

// The table of all locations (used by several screens).
void printLocationsTable(const Graph& city)
{
    const vector<int> widths = {8, 36, 8};
    printTableRow({"ID", "Location", "Roads"}, widths, Color::Cyan);
    printLine();
    for (int id : city.getLocationIds()) {
        cout << "  " << cell(idText(id), widths[0], Color::Blue)
             << cell(city.getName(id), widths[1], Color::Normal)
             << cell(to_string(city.getNeighbors(id).size()), widths[2], Color::Purple) << "\n";
    }
    printLine();
}

// The table of all roads (used by several screens).
void printRoadsTable(const Graph& city)
{
    const vector<int> widths = {6, 22, 6, 22, 14};
    printTableRow({"From", "Location", "To", "Location", "Distance"}, widths, Color::Cyan);
    printLine();
    for (const Road& road : city.getRoads()) {
        cout << "  " << cell(idText(road.from), widths[0], Color::Blue)
             << cell(city.getName(road.from), widths[1], Color::Normal)
             << cell(idText(road.to), widths[2], Color::Blue)
             << cell(city.getName(road.to), widths[3], Color::Normal)
             << cell(formatNumber(road.distance) + " km", widths[4], Color::Purple) << "\n";
    }
    printLine();
}

// ===== Locations =====

// Option 1: add a new location.
void addLocation(Graph& city)
{
    printScreenHeader("LOCATIONS", "Add Location");
    printHint("Spaces at the start and end are removed. Upper/lower case is ignored");
    printHint("when checking for duplicates. A name cannot contain a comma.");
    printBlankLine();

    string name = trimName(readLine("Location name: "));   // remove spaces at both ends
    if (!isValidName(city, name, 0)) {
        return;
    }

    int id = city.addLocation(name);
    printBlankLine();
    printSuccess("Location \"" + name + "\" added with ID " + to_string(id) + ".");
    saveChanges(city);
}

// Option 2: show all locations in a table.
void showLocations(const Graph& city)
{
    printScreenHeader("LOCATIONS", "Show Locations");
    if (city.locationCount() == 0) {
        printWarning("No locations yet. Choose option 1 to add one.");
        return;
    }
    printLocationsTable(city);
    printHint(to_string(city.locationCount()) + " location(s) in the city.");
}

// Option 3: change the name of a location.
void renameLocation(Graph& city)
{
    printScreenHeader("LOCATIONS", "Rename Location");
    if (city.locationCount() == 0) {
        printWarning("No locations yet.");
        return;
    }

    printLocationsTable(city);
    printBlankLine();
    int id = readInt("ID of the location to rename: ");
    if (!city.hasLocation(id)) {
        printError("Location " + to_string(id) + " does not exist.");
        return;
    }

    string oldName = city.getName(id);
    string newName = trimName(readLine("New name: "));
    if (!isValidName(city, newName, id)) {
        return;
    }

    city.renameLocation(id, newName);
    printBlankLine();
    printSuccess("Location " + to_string(id) + " renamed from \"" + oldName + "\" to \"" +
                 newName + "\".");
    saveChanges(city);
}

// Option 4: delete a location and all of its roads.
void deleteLocation(Graph& city)
{
    printScreenHeader("LOCATIONS", "Delete Location");
    if (city.locationCount() == 0) {
        printWarning("No locations yet.");
        return;
    }

    printLocationsTable(city);
    printBlankLine();
    int id = readInt("ID of the location to delete: ");
    if (!city.hasLocation(id)) {
        printError("Location " + to_string(id) + " does not exist.");
        return;
    }

    string name = city.getName(id);
    size_t roads = city.getNeighbors(id).size();
    printBlankLine();
    cout << "  " << paint("[DELETE]", Color::Error) << " "
         << paint("\"" + name + "\" and its " + to_string(roads) + " road(s) will be deleted.",
                  Color::Error)
         << "\n";
    if (!readYes("Are you sure? (y/n): ")) {
        printInfo("Nothing was deleted.");
        return;
    }

    city.removeLocation(id);
    printSuccess("Location \"" + name + "\" and its " + to_string(roads) +
                 " road(s) were deleted.");
    saveChanges(city);
}

// ===== Roads =====

// Option 5: add a new road between two locations.
void addRoad(Graph& city)
{
    printScreenHeader("ROADS", "Add Road");
    if (city.locationCount() < 2) {
        printError("Add at least two locations first.");
        return;
    }

    printLocationsTable(city);
    printHint("Roads are two-way. Distance: greater than 0 and at most " +
              formatNumber(MAX_ROAD_DISTANCE) + " km.");
    printBlankLine();
    int from = readInt("From location ID: ");
    int to = readInt("To location ID: ");
    double distance = readDouble("Distance in km: ");
    printBlankLine();

    // Check each rule and tell the user what is wrong.
    if (!city.hasLocation(from)) {
        printError("Location " + to_string(from) + " does not exist.");
    } else if (!city.hasLocation(to)) {
        printError("Location " + to_string(to) + " does not exist.");
    } else if (from == to) {
        printError("A road must connect two different locations.");
    } else if (!isValidDistance(distance)) {
        printDistanceError();
    } else if (city.hasRoad(from, to)) {
        printError("A road between these two locations already exists.");
    } else {
        city.addRoad(from, to, distance);
        printSuccess("Road added: " + city.getName(from) + " " + twoWay() + " " +
                     city.getName(to) + " (" + formatNumber(distance) + " km)");
        saveChanges(city);
    }
}

// Option 6: show all roads in a table.
void showRoads(const Graph& city)
{
    printScreenHeader("ROADS", "Show Roads");
    if (city.roadCount() == 0) {
        printWarning("No roads yet. Choose option 5 to add one.");
        return;
    }
    printRoadsTable(city);
    printHint(to_string(city.roadCount()) + " road(s) in the city. Every road is two-way.");
}

// Asks for the two location IDs of an existing road.
// Returns false (after printing the problem) if there is no such road.
bool readExistingRoad(const Graph& city, int& from, int& to)
{
    from = readInt("From location ID: ");
    to = readInt("To location ID: ");
    if (!city.hasRoad(from, to)) {
        printError("There is no road between location " + to_string(from) + " and location " +
                   to_string(to) + ".");
        return false;
    }
    return true;
}

// Option 7: change the distance of a road.
void updateRoad(Graph& city)
{
    printScreenHeader("ROADS", "Update Road Distance");
    if (city.roadCount() == 0) {
        printWarning("No roads yet.");
        return;
    }

    printRoadsTable(city);
    printHint("Enter the two location IDs of the road, in either order.");
    printBlankLine();
    int from;
    int to;
    if (!readExistingRoad(city, from, to)) {
        return;
    }

    double distance = readDouble("New distance in km: ");
    printBlankLine();
    if (!isValidDistance(distance)) {
        printDistanceError();
        return;
    }

    city.updateRoad(from, to, distance);
    printSuccess("Road " + city.getName(from) + " " + twoWay() + " " + city.getName(to) +
                 " now has a distance of " + formatNumber(distance) + " km.");
    saveChanges(city);
}

// Option 8: delete a road.
void deleteRoad(Graph& city)
{
    printScreenHeader("ROADS", "Delete Road");
    if (city.roadCount() == 0) {
        printWarning("No roads yet.");
        return;
    }

    printRoadsTable(city);
    printHint("Enter the two location IDs of the road. The locations are kept.");
    printBlankLine();
    int from;
    int to;
    if (!readExistingRoad(city, from, to)) {
        return;
    }

    city.removeRoad(from, to);
    printBlankLine();
    printSuccess("Road " + city.getName(from) + " " + twoWay() + " " + city.getName(to) +
                 " deleted.");
    saveChanges(city);
}

// ===== City network =====

// Option 9: show the network as an adjacency list
// (every location followed by the locations it is connected to).
void showNetwork(const Graph& city)
{
    printScreenHeader("NETWORK", "City Network (Adjacency List)");
    if (city.locationCount() == 0) {
        printWarning("The network is empty.");
        return;
    }
    printHint("Each location is followed by the locations it is directly connected to.");
    printBlankLine();

    string branch = colorsOn() ? "├─ " : "|- ";
    string lastBranch = colorsOn() ? "└─ " : "`- ";

    for (int id : city.getLocationIds()) {
        vector<Edge> roads = city.getNeighbors(id);
        cout << "  " << paint(idText(id), Color::Blue) << " " << paint(city.getName(id), Color::Title)
             << "  " << paint("(" + to_string(roads.size()) + " road(s))", Color::Muted) << "\n";

        if (roads.empty()) {
            cout << "      " << paint("(no roads)", Color::Warning) << "\n";
        }
        for (size_t i = 0; i < roads.size(); i++) {
            const Edge& edge = roads[i];
            string symbol = (i + 1 == roads.size()) ? lastBranch : branch;
            cout << "      " << paint(symbol, Color::Muted)
                 << cell(nameWithId(city, edge.to), 34, Color::Normal)
                 << paint(formatNumber(edge.distance) + " km", Color::Purple) << "\n";
        }
    }
    printBlankLine();
    printLine();
    printHint(to_string(city.locationCount()) + " location(s), " + to_string(city.roadCount()) +
              " road(s).");
}

// ===== Traversal (BFS and DFS) =====

// Options 10 and 11: traverse the network from a chosen location.
// `useBfs` is true for BFS (option 10) and false for DFS (option 11).
void traverseNetwork(const Graph& city, bool useBfs)
{
    string algorithmName = useBfs ? "BFS (Breadth-First Search)" : "DFS (Depth-First Search)";
    printScreenHeader("ALGORITHMS", useBfs ? "BFS Traversal" : "DFS Traversal");

    if (city.locationCount() == 0) {
        printWarning("The network is empty. Add some locations first.");
        return;
    }

    if (useBfs) {
        printHint("BFS visits the nearest locations first, level by level (uses a queue).");
    } else {
        printHint("DFS follows one path as far as possible, then goes back (uses recursion).");
    }
    printBlankLine();
    printLocationsTable(city);
    printBlankLine();
    int start = readInt("Start location ID: ");
    if (!city.hasLocation(start)) {
        printError("Location " + to_string(start) + " does not exist.");
        return;
    }

    vector<int> order;
    if (useBfs) {
        order = bfs(city, start);
    } else {
        order = dfs(city, start);
    }

    // The visiting order, one location per line.
    printBlankLine();
    printSectionTitle(algorithmName + " from " + nameWithId(city, start), Color::Title);
    for (size_t i = 0; i < order.size(); i++) {
        ostringstream number;
        number << setw(4) << (i + 1) << ".";
        cout << "  " << paint(number.str(), Color::Blue) << " " << nameWithId(city, order[i])
             << "\n";
    }

    // The same order on one line, e.g.  BUBT -> Mirpur 10 -> Farmgate
    printBlankLine();
    printSectionTitle("Order", Color::Cyan);
    vector<string> names;
    for (int id : order) {
        names.push_back(city.getName(id));
    }
    printChain(names, Color::Title);
    printBlankLine();

    string reached = "Reached " + to_string(order.size()) + " of " +
                     to_string(city.locationCount()) + " location(s).";
    if (static_cast<int>(order.size()) == city.locationCount()) {
        printSuccess(reached);
        return;
    }

    // Locations that were never visited cannot be reached from the start:
    // they are in a different part of the network (disconnected).
    printWarning(reached);
    vector<string> unreachable;
    for (int id : city.getLocationIds()) {
        bool wasVisited = false;
        for (int visitedId : order) {
            if (visitedId == id) {
                wasVisited = true;
            }
        }
        if (!wasVisited) {
            unreachable.push_back(nameWithId(city, id));
        }
    }
    cout << "  " << paint("Not reachable from " + city.getName(start) + ":", Color::Warning) << "\n";
    for (const string& name : unreachable) {
        cout << "    " << paint("- " + name, Color::Warning) << "\n";
    }
}

// ===== Shortest route (Dijkstra) =====

// Returns the distance of the road between two neighbouring locations.
double roadDistance(const Graph& city, int from, int to)
{
    for (const Edge& edge : city.getNeighbors(from)) {
        if (edge.to == to) {
            return edge.distance;
        }
    }
    return 0.0;   // not reached for roads on a route found by Dijkstra
}

// Prints a found route: the locations in order, each road with its distance,
// and the total distance. Used by Dijkstra (option 12) and Floyd–Warshall
// (option 14).
void printRouteDetails(const Graph& city, const RouteResult& route)
{
    // The route, e.g.  BUBT [1] -> Mirpur 10 [2] -> Agargaon [4]
    printSectionTitle("Shortest Route", Color::Title);
    vector<string> stops;
    for (int id : route.path) {
        stops.push_back(nameWithId(city, id));
    }
    printChain(stops, Color::Success);

    // Each road of the route with its own distance.
    if (route.path.size() > 1) {
        printBlankLine();
        printSectionTitle("Road by Road", Color::Cyan);
        for (size_t i = 0; i + 1 < route.path.size(); i++) {
            int from = route.path[i];
            int to = route.path[i + 1];
            ostringstream number;
            number << setw(3) << (i + 1) << ".";
            cout << "  " << paint(number.str(), Color::Blue) << " "
                 << cell(city.getName(from), 22, Color::Normal) << paint(arrow(), Color::Muted)
                 << " " << cell(city.getName(to), 22, Color::Normal)
                 << paint(formatNumber(roadDistance(city, from, to)) + " km", Color::Purple)
                 << "\n";
        }
    }

    printBlankLine();
    printLine();
    cout << "  " << paint("Total Distance: ", Color::Title)
         << paint(formatNumber(route.distance) + " km", Color::Success) << "  "
         << paint("(" + to_string(route.path.size() - 1) + " road(s))", Color::Muted) << "\n";
    printLine();
}

// Option 12: find the shortest route between two locations.
void findShortestRoute(const Graph& city)
{
    printScreenHeader("ALGORITHMS", "Dijkstra's Shortest Route");
    if (city.locationCount() == 0) {
        printWarning("The network is empty. Add some locations first.");
        return;
    }

    printHint("Finds the route with the smallest total distance (not the fewest roads).");
    printBlankLine();
    printLocationsTable(city);
    printBlankLine();
    int start = readInt("Start location ID: ");
    if (!city.hasLocation(start)) {
        printError("Location " + to_string(start) + " does not exist.");
        return;
    }
    int destination = readInt("Destination location ID: ");
    if (!city.hasLocation(destination)) {
        printError("Location " + to_string(destination) + " does not exist.");
        return;
    }

    RouteResult route = dijkstra(city, start, destination);
    printBlankLine();
    if (!route.found) {
        printWarning("No route exists between these locations.");
        printHint("(" + city.getName(start) + " and " + city.getName(destination) +
                  " are in different parts of the network.)");
        return;
    }

    printRouteDetails(city, route);
}

// ===== Minimum spanning tree (Kruskal) =====

// Option 13: select the shortest set of roads that connects all locations.
void showMinimumSpanningTree(const Graph& city)
{
    printScreenHeader("ALGORITHMS", "Kruskal's Minimum Spanning Tree");
    if (city.locationCount() == 0) {
        printWarning("The network is empty. Add some locations first.");
        return;
    }

    printHint("Selects existing roads that connect all locations with the smallest");
    printHint("total distance, without any cycle (DSU detects cycles).");
    printBlankLine();

    SpanningResult tree = kruskal(city);

    if (tree.connected) {
        printSectionTitle("Minimum Spanning Tree (Kruskal)", Color::Title);
    } else {
        printSectionTitle("Minimum Spanning Forest (Kruskal)", Color::Warning);
    }

    if (tree.roads.empty()) {
        string text = "No roads were selected";
        if (city.roadCount() == 0) {
            text += " (the city has no roads)";
        }
        printWarning(text + ".");
    } else {
        const vector<int> widths = {5, 52, 13};
        printTableRow({"#", "Road", "Distance"}, widths, Color::Cyan);
        printLine();
        int number = 1;
        for (const Road& road : tree.roads) {
            string roadText = nameWithId(city, road.from) + " " + twoWay() + " " +
                              nameWithId(city, road.to);
            cout << "  " << cell(to_string(number), widths[0], Color::Blue)
                 << cell(roadText, widths[1], Color::Normal)
                 << cell(formatNumber(road.distance) + " km", widths[2], Color::Purple) << "\n";
            number++;
        }
        printLine();
    }

    printBlankLine();
    cout << "  " << paint("Selected roads: ", Color::Title)
         << paint(to_string(tree.roads.size()) + " of " + to_string(city.roadCount()), Color::Cyan)
         << "\n";
    cout << "  " << paint("Total distance: ", Color::Title)
         << paint(formatNumber(tree.totalDistance) + " km", Color::Success) << "\n\n";

    if (tree.connected) {
        printSuccess("The network is connected.");
        printHint("These roads connect all " + to_string(city.locationCount()) +
                  " location(s) with the smallest total distance.");
    } else {
        printWarning("The network is NOT connected: it has " + to_string(tree.components) +
                     " separate parts (components).");
        printHint("This is a Minimum Spanning Forest: one minimum tree for each part.");
        printHint("No existing roads can join the parts together.");
    }
}

// ===== All-pairs shortest paths (Floyd–Warshall) =====

// "Floyd–Warshall" with an en dash in colour mode, "Floyd-Warshall" in plain mode.
string floydWarshallName()
{
    return colorsOn() ? "Floyd–Warshall" : "Floyd-Warshall";
}

// True if every two neighbouring locations on the route are joined by a road.
bool routeUsesRealRoads(const Graph& city, const RouteResult& route)
{
    for (size_t i = 0; i + 1 < route.path.size(); i++) {
        if (!city.hasRoad(route.path[i], route.path[i + 1])) {
            return false;
        }
    }
    return true;
}

// The text right-aligned in a field `width` characters wide.
string rightAligned(const string& text, int width)
{
    int space = width - displayWidth(text);
    return string(max(0, space), ' ') + text;
}

// Prints the distance matrix: one row and one column per location ID.
// "0" = the same location, "--" = no route.
// Returns false (and prints nothing) if the table is too wide for the screen.
bool printDistanceMatrix(const AllPairsResult& table)
{
    int n = static_cast<int>(table.ids.size());

    // Every column is as wide as its longest entry plus one space.
    int width = 5;
    for (int i = 0; i < n; i++) {
        width = max(width, static_cast<int>(idText(table.ids[i]).size()) + 1);
        for (int j = 0; j < n; j++) {
            if (table.dist[i][j] != INFINITY) {
                width = max(width, static_cast<int>(formatNumber(table.dist[i][j]).size()) + 1);
            }
        }
    }
    const int labelWidth = 6;   // the row label, e.g. "[10]  "
    if (labelWidth + n * width > SCREEN_WIDTH - 2) {
        return false;
    }

    // Header row: the location IDs.
    cout << "  " << string(labelWidth, ' ');
    for (int j = 0; j < n; j++) {
        cout << paint(rightAligned(idText(table.ids[j]), width), Color::Cyan);
    }
    cout << "\n";
    printLine();

    // One row per location.
    for (int i = 0; i < n; i++) {
        cout << "  " << cell(idText(table.ids[i]), labelWidth, Color::Blue);
        for (int j = 0; j < n; j++) {
            if (i == j) {
                cout << paint(rightAligned("0", width), Color::Muted);
            } else if (table.dist[i][j] == INFINITY) {
                cout << paint(rightAligned("--", width), Color::Warning);
            } else {
                cout << rightAligned(formatNumber(table.dist[i][j]), width);
            }
        }
        cout << "\n";
    }
    printLine();
    return true;
}

// Option 14: shortest distances between all pairs of locations, and the
// shortest route between two chosen locations.
void showAllPairsShortestPaths(const Graph& city)
{
    printScreenHeader("ALGORITHMS", "All-Pairs Shortest Paths (" + floydWarshallName() + ")");
    if (city.locationCount() == 0) {
        printWarning("The network is empty. Add some locations first.");
        return;
    }

    printHint(floydWarshallName() + " (dynamic programming) finds the shortest distance");
    printHint("between EVERY pair of locations. Step k allows location k as a stop");
    printHint("in between. Time O(V^3), memory O(V^2).");
    printBlankLine();

    AllPairsResult table = floydWarshall(city);

    printLocationsTable(city);
    printBlankLine();
    printSectionTitle("Distance Matrix (km)", Color::Title);
    if (printDistanceMatrix(table)) {
        printHint("Rows and columns are location IDs. 0 = same location, -- = no route.");
    } else {
        printInfo("The city has " + to_string(city.locationCount()) +
                  " locations: the full table is too wide for the screen.");
        printHint("Choose two locations below to see their shortest distance and route.");
    }

    // Is any pair of locations unreachable?
    bool allReachable = true;
    for (const vector<double>& row : table.dist) {
        for (double distance : row) {
            if (distance == INFINITY) {
                allReachable = false;
            }
        }
    }
    if (!allReachable) {
        printWarning("Some locations cannot reach each other: the network is in separate parts.");
    }
    printBlankLine();

    int start = readInt("Start location ID: ");
    if (!city.hasLocation(start)) {
        printError("Location " + to_string(start) + " does not exist.");
        return;
    }
    int destination = readInt("Destination location ID: ");
    if (!city.hasLocation(destination)) {
        printError("Location " + to_string(destination) + " does not exist.");
        return;
    }

    RouteResult route = allPairsRoute(table, start, destination);
    printBlankLine();
    if (!route.found) {
        printWarning("No route exists between these locations.");
        printHint("(" + city.getName(start) + " and " + city.getName(destination) +
                  " are in different parts of the network.)");
        return;
    }
    // Safety check: the rebuilt route must only use roads that exist.
    if (!routeUsesRealRoads(city, route)) {
        printError("The rebuilt route uses a road that does not exist (program error).");
        return;
    }
    printRouteDetails(city, route);
}

// ===== Best facility location (network centre) =====

// Prints a list one item per line ("    - item"), at most 5 items, then
// "... and N more". One item per line keeps long names on the screen.
void printListLines(const vector<string>& items, Color color)
{
    const size_t limit = 5;
    for (size_t i = 0; i < items.size() && i < limit; i++) {
        cout << "    " << paint("- " + items[i], color) << "\n";
    }
    if (items.size() > limit) {
        printHint("  ... and " + to_string(items.size() - limit) + " more");
    }
}

// Prints the centre and the diameter of one connected part (2 or more locations).
void printPartSummary(const Graph& city, const NetworkPart& part)
{
    if (part.centres.size() == 1) {
        cout << "  " << paint("Best location: ", Color::Title)
             << paint(nameWithId(city, part.centres[0]), Color::Success) << "\n";
    } else {
        // Tied centres: one per line.
        cout << "  " << paint("Best locations (equally good):", Color::Title) << "\n";
        vector<string> centres;
        for (int id : part.centres) {
            centres.push_back(nameWithId(city, id));
        }
        printListLines(centres, Color::Success);
    }
    string fromText = part.centres.size() == 1 ? "Farthest location from it: " : "Farthest location from each: ";
    cout << "  " << paint(fromText, Color::Title)
         << paint(formatNumber(part.radius) + " km", Color::Success)
         << paint("  (radius)", Color::Muted) << "\n";
    cout << "  " << paint("Diameter: ", Color::Title)
         << paint(formatNumber(part.diameter) + " km", Color::Cyan)
         << paint("  (the two locations farthest apart)", Color::Muted) << "\n";
    vector<string> pairs;
    for (const pair<int, int>& ends : part.diameterPairs) {
        pairs.push_back(nameWithId(city, ends.first) + " " + twoWay() + " " +
                        nameWithId(city, ends.second));
    }
    printListLines(pairs, Color::Normal);
}

// Option 15: the best place for a facility such as a hospital or fire station.
void showBestFacilityLocation(const Graph& city)
{
    printScreenHeader("ALGORITHMS", "Best Facility Location");
    if (city.locationCount() == 0) {
        printWarning("The network is empty. Add some locations first.");
        return;
    }

    printHint("The best place for a hospital or fire station is the network centre:");
    printHint("the location whose FARTHEST location is as close as possible.");
    printHint("Eccentricity = distance to the farthest location (" + floydWarshallName() + ").");
    printBlankLine();

    // The shortest distances are calculated once, by Floyd–Warshall.
    AllPairsResult table = floydWarshall(city);
    FacilityResult result = bestFacilityLocation(table);

    // Which part each location is in, and whether it is a centre.
    map<int, size_t> partNumber;
    map<int, bool> isCentre;
    for (size_t p = 0; p < result.parts.size(); p++) {
        for (int id : result.parts[p].locations) {
            partNumber[id] = p + 1;
        }
        for (int id : result.parts[p].centres) {
            isCentre[id] = true;
        }
    }

    // The eccentricity table. The "Part" column is only needed when the
    // network is in separate parts.
    string centreMark = colorsOn() ? "◄ centre" : "<- centre";
    vector<int> widths = {8, 30, 16};
    vector<string> header = {"ID", "Location", "Farthest (km)"};
    if (!result.connected) {
        widths = {8, 30, 6, 16};
        header = {"ID", "Location", "Part", "Farthest (km)"};
    }
    printTableRow(header, widths, Color::Cyan);
    printLine();
    for (int id : city.getLocationIds()) {
        size_t part = partNumber[id];
        bool isolated = result.parts[part - 1].locations.size() == 1;
        cout << "  " << cell(idText(id), 8, Color::Blue) << cell(city.getName(id), 30, Color::Normal);
        if (!result.connected) {
            cout << cell(to_string(part), 6, Color::Purple);
        }
        cout << paint(rightAligned(formatNumber(result.eccentricity[id]), 13), Color::Normal);
        if (isolated && city.locationCount() > 1) {
            cout << "   " << paint("isolated", Color::Warning);
        } else if (isCentre[id]) {
            cout << "   " << paint(centreMark, Color::Success);
        }
        cout << "\n";
    }
    printLine();
    printBlankLine();

    // Connected network: one centre and one diameter.
    if (result.connected) {
        printSectionTitle("Network Centre and Diameter", Color::Title);
        if (city.locationCount() == 1) {
            printInfo("Only one location: " + nameWithId(city, city.getLocationIds()[0]) +
                      " is the centre.");
            cout << "  " << paint("Radius: ", Color::Title) << paint("0.00 km", Color::Success)
                 << paint("   Diameter: ", Color::Title) << paint("0.00 km", Color::Cyan)
                 << paint("  (no other location)", Color::Muted) << "\n";
            return;
        }
        printPartSummary(city, result.parts[0]);
        printBlankLine();
        printHint("A facility here keeps the farthest location as close as possible.");
        return;
    }

    // Separate parts: no single centre, so each part is analysed on its own.
    printWarning("The network has " + to_string(result.parts.size()) +
                 " separate parts: there is no single centre.");
    printHint("Its diameter is infinite. Each part needs its own facility:");
    vector<int> isolatedIds;
    for (size_t p = 0; p < result.parts.size(); p++) {
        const NetworkPart& part = result.parts[p];
        if (part.locations.size() == 1) {
            isolatedIds.push_back(part.locations[0]);
            continue;
        }
        printBlankLine();
        printSectionTitle("Part " + to_string(p + 1) + " (" + to_string(part.locations.size()) +
                              " locations)",
                          Color::Purple);
        printPartSummary(city, part);
    }
    if (!isolatedIds.empty()) {
        // One per line, in the same style as the "Not reachable" list of BFS/DFS.
        printBlankLine();
        printWarning("Isolated locations (no road reaches them):");
        vector<string> isolated;
        for (int id : isolatedIds) {
            isolated.push_back(nameWithId(city, id));
        }
        printListLines(isolated, Color::Warning);
        printHint("Each isolated location needs its own facility.");
    }
}

// ===== Main menu =====

// One menu entry, e.g. " 1  Add Location", padded to `width` characters.
string menuItem(int number, const string& text, Color color, int width)
{
    string numberText = (number < 10 ? " " : "") + to_string(number);
    int used = displayWidth(numberText) + 2 + displayWidth(text);
    return paint(numberText, color) + "  " + text + string(max(0, width - used), ' ');
}

// Two menu entries side by side (an empty `rightText` leaves the right side empty).
void printMenuRow(int leftNumber, const string& leftText, int rightNumber, const string& rightText,
                  Color leftColor, Color rightColor)
{
    if (rightText.empty()) {
        cout << "   " << menuItem(leftNumber, leftText, leftColor, 0) << "\n";
        return;
    }
    cout << "   " << menuItem(leftNumber, leftText, leftColor, 34)
         << menuItem(rightNumber, rightText, rightColor, 0) << "\n";
}

// A category heading with a short line under it; two headings side by side.
void printMenuHeadings(const string& left, Color leftColor, const string& right, Color rightColor)
{
    string line = colorsOn() ? "─" : "-";
    string leftLine;
    string rightLine;
    for (int i = 0; i < 30; i++) {
        leftLine += line;
    }
    for (int i = 0; i < 34; i++) {
        rightLine += line;
    }
    if (right.empty()) {
        cout << "  " << paint(left, leftColor) << "\n";
        cout << "  " << paint(leftLine, Color::Muted) << "\n";
        return;
    }
    cout << "  " << paint(fitText(left, 35), leftColor) << paint(right, rightColor) << "\n";
    cout << "  " << paint(leftLine, Color::Muted) << "     " << paint(rightLine, Color::Muted)
         << "\n";
}

void showMenu(const Graph& city, const string& notice)
{
    clearScreen();
    printBanner();

    // Dashboard: the size of the city and where it is saved.
    string divider = colorsOn() ? "   │   " : "   |   ";
    cout << "\n   " << paint("LOCATIONS ", Color::Muted)
         << paint(to_string(city.locationCount()), Color::Title) << paint(divider, Color::Muted)
         << paint("ROADS ", Color::Muted) << paint(to_string(city.roadCount()), Color::Title)
         << paint(divider, Color::Muted) << paint("DATA ", Color::Muted) << dataFolder
         << paint("  (saved automatically)", Color::Muted) << "\n\n";

    printMenuHeadings("LOCATIONS", Color::Cyan, "ROADS", Color::Blue);
    printMenuRow(1, "Add Location", 5, "Add Road", Color::Cyan, Color::Blue);
    printMenuRow(2, "Show Locations", 6, "Show Roads", Color::Cyan, Color::Blue);
    printMenuRow(3, "Rename Location", 7, "Update Road Distance", Color::Cyan, Color::Blue);
    printMenuRow(4, "Delete Location", 8, "Delete Road", Color::Cyan, Color::Blue);
    printBlankLine();

    printMenuHeadings("NETWORK & ALGORITHMS", Color::Purple, "", Color::Purple);
    printMenuRow(9, "Show City Network", 12, "Dijkstra's Shortest Route", Color::Purple,
                 Color::Purple);
    printMenuRow(10, "BFS Traversal", 13, "Kruskal's Minimum Spanning Tree", Color::Purple,
                 Color::Purple);
    printMenuRow(11, "DFS Traversal", 0, "", Color::Purple, Color::Purple);
    cout << "   "
         << menuItem(14, "All-Pairs Shortest Paths (" + floydWarshallName() + ")", Color::Purple, 0)
         << "\n";
    cout << "   " << menuItem(15, "Best Facility Location", Color::Purple, 0) << "\n";
    printBlankLine();
    cout << "   " << menuItem(0, "Exit", Color::Error, 0) << "\n";

    printLine();
    printHint("Graph: adjacency list  |  BFS, DFS, Dijkstra, Kruskal + DSU  |  C++17");
    printBlankLine();

    if (!notice.empty()) {
        printError(notice);
    }
}

// ===== Start and exit =====

// Chooses the data folder: "data" in the current folder if it is there,
// otherwise the project's "data" folder next to the program (for example
// when the program is started from inside build-mingw).
void chooseDataFolder(const char* programPath)
{
    vector<string> candidates = {"data"};

    error_code error;
    filesystem::path program = filesystem::absolute(programPath, error);
    if (!error) {
        filesystem::path programFolder = program.parent_path();
        candidates.push_back((programFolder / "data").lexically_normal().string());
        candidates.push_back((programFolder.parent_path() / "data").lexically_normal().string());
    }

    dataFolder = findDataFolder(candidates);

    // Show a long folder path relative to the current folder, e.g. "..\data".
    filesystem::path relative =
        filesystem::path(dataFolder).lexically_relative(filesystem::current_path(error));
    if (!error && !relative.empty() && relative.string().size() < dataFolder.size()) {
        dataFolder = relative.string();
    }

    locationsFile = (filesystem::path(dataFolder) / "locations.csv").string();
    roadsFile = (filesystem::path(dataFolder) / "roads.csv").string();
    nextIdFile = (filesystem::path(dataFolder) / "next_id.txt").string();
}

// The exit screen uses the terminal's own background, so the terminal looks
// normal again after the program ends.
void showExitScreen(const Graph& city)
{
    leaveTheme();
    cout << "\n  " << paint("SMART CITY ROUTE & INFRASTRUCTURE PLANNER", Color::Cyan)
         << paint("  |  Team Quad Core", Color::Purple) << "\n\n";
    printSuccess("Your city is saved: " + to_string(city.locationCount()) + " location(s), " +
                 to_string(city.roadCount()) + " road(s).");
    printHint("Data folder: " + dataFolder);
    printBlankLine();
    cout << "  " << paint("Thank you for using the Smart City Planner. Goodbye!", Color::Cyan)
         << "\n\n";
}

int main(int argc, char* argv[])
{
    // "--plain" switches off colours (useful for old terminals or screenshots).
    bool plain = false;
    for (int i = 1; i < argc; i++) {
        if (string(argv[i]) == "--plain") {
            plain = true;
        }
    }
    initConsole(plain);

    // Show every decimal number with 2 digits after the point, e.g. 4.50
    cout << fixed << setprecision(2);

    chooseDataFolder(argv[0]);

    // First run: if no working files exist yet, start with the sample city.
    // Existing data is never overwritten.
    bool sampleCopied = initializeFromSample((filesystem::path(dataFolder) / "sample").string(),
                                             locationsFile, roadsFile, nextIdFile);

    // Load the saved city and collect any problems found in the files.
    Graph city;
    vector<string> messages;
    loadCity(city, locationsFile, roadsFile, nextIdFile, messages);

    // A short start screen, only when there is something to report.
    if (sampleCopied || !messages.empty() || dataFolder != "data") {
        clearScreen();
        printBanner();
        printBlankLine();
        if (dataFolder != "data") {
            printInfo("Using the project's data folder: " + dataFolder);
        }
        if (sampleCopied) {
            printInfo("First run: no saved city was found, so the sample city was loaded");
            printHint("         (" + to_string(city.locationCount()) + " locations, " +
                      to_string(city.roadCount()) + " roads). Your changes are saved as usual.");
        }
        for (const string& message : messages) {
            printWarning(message);
        }
        printInfo("Loaded " + to_string(city.locationCount()) + " location(s) and " +
                  to_string(city.roadCount()) + " road(s).");
        waitForEnter("Press Enter to open the main menu...");
    }

    string notice;   // an error shown under the menu (for example a wrong option)
    while (true) {
        showMenu(city, notice);
        notice = "";
        int choice = readInt("Choose an option: ");

        if (choice == 0) {
            showExitScreen(city);
            break;
        }
        if (choice < 0 || choice > 15) {
            notice = "Please choose a number from the menu.";
            continue;
        }

        clearScreen();
        if (choice == 1) {
            addLocation(city);
        } else if (choice == 2) {
            showLocations(city);
        } else if (choice == 3) {
            renameLocation(city);
        } else if (choice == 4) {
            deleteLocation(city);
        } else if (choice == 5) {
            addRoad(city);
        } else if (choice == 6) {
            showRoads(city);
        } else if (choice == 7) {
            updateRoad(city);
        } else if (choice == 8) {
            deleteRoad(city);
        } else if (choice == 9) {
            showNetwork(city);
        } else if (choice == 10) {
            traverseNetwork(city, true);
        } else if (choice == 11) {
            traverseNetwork(city, false);
        } else if (choice == 12) {
            findShortestRoute(city);
        } else if (choice == 13) {
            showMinimumSpanningTree(city);
        } else if (choice == 14) {
            showAllPairsShortestPaths(city);
        } else if (choice == 15) {
            showBestFacilityLocation(city);
        }
        waitForEnter();
    }

    return 0;
}
