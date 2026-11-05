// Smart City Route & Infrastructure Planner
// Copyright © 2026 Quad Core. All rights reserved.
//
// storage.cpp - saving and loading the city in CSV files (see storage.h).

#include "storage.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>

using namespace std;

// ===== Small helpers (only used in this file) =====

// Splits a line such as "1,2,2.5" into the parts "1", "2" and "2.5".
static vector<string> splitByComma(const string& line)
{
    vector<string> parts;
    stringstream input(line);
    string part;
    while (getline(input, part, ',')) {
        parts.push_back(part);
    }
    return parts;
}

// Converts text to a whole number. Returns false if the text is not
// exactly a whole number ("12" is fine, "12abc" and "" are not).
static bool textToInt(const string& text, int& number)
{
    stringstream input(text);
    char extra;
    return (input >> number) && !(input >> extra);
}

// Converts text to a decimal number, in the same way.
static bool textToDouble(const string& text, double& number)
{
    stringstream input(text);
    char extra;
    return (input >> number) && !(input >> extra);
}

// Files edited on Windows may end each line with '\r'. Remove it.
static void removeCarriageReturn(string& line)
{
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
}

// Creates the folder that will contain `file`, if it does not exist yet.
// If the folder cannot be created, nothing happens here; opening the file
// will then fail and saveCity() returns false.
static void createFolderFor(const string& file)
{
    filesystem::path folder = filesystem::path(file).parent_path();
    if (!folder.empty()) {
        error_code error;   // receives the error instead of stopping the program
        filesystem::create_directories(folder, error);
    }
}

// ===== Saving =====

bool saveCity(const Graph& city, const string& locationsFile, const string& roadsFile,
              const string& nextIdFile)
{
    createFolderFor(locationsFile);
    createFolderFor(roadsFile);
    createFolderFor(nextIdFile);

    ofstream locations(locationsFile);
    if (!locations) {
        return false;
    }
    locations << "id,name\n";
    for (int id : city.getLocationIds()) {
        locations << id << "," << city.getName(id) << "\n";
    }
    locations.close();

    ofstream roads(roadsFile);
    if (!roads) {
        return false;
    }
    // setprecision(15) keeps distances such as 2.35 exactly as typed.
    roads << setprecision(15);
    roads << "from,to,distance\n";
    for (const Road& road : city.getRoads()) {
        roads << road.from << "," << road.to << "," << road.distance << "\n";
    }
    roads.close();

    ofstream nextId(nextIdFile);
    if (!nextId) {
        return false;
    }
    nextId << city.getNextId() << "\n";
    nextId.close();

    return !locations.fail() && !roads.fail() && !nextId.fail();
}

// ===== Loading =====

// Reads locations.csv. Each valid line adds one location with its saved ID.
static void loadLocations(Graph& city, const string& file, vector<string>& messages)
{
    ifstream input(file);
    if (!input) {
        messages.push_back("No saved locations found (" + file + "). Starting with an empty city.");
        return;
    }

    string line;
    int lineNumber = 0;
    while (getline(input, line)) {
        lineNumber++;
        removeCarriageReturn(line);
        if ((lineNumber == 1 && line == "id,name") || line.empty()) {
            continue;   // skip the header line and empty lines
        }

        string where = file + " line " + to_string(lineNumber) + ": ";
        vector<string> parts = splitByComma(line);
        int id;
        string name;
        if (parts.size() == 2) {
            name = trimName(parts[1]);   // " BUBT " is stored as "BUBT"
        }

        if (parts.size() != 2) {
            messages.push_back(where + "expected 2 values (id,name). Line skipped.");
        } else if (!textToInt(parts[0], id) || id <= 0) {
            messages.push_back(where + "the ID must be a positive whole number. Line skipped.");
        } else if (name.empty()) {
            messages.push_back(where + "the name is empty. Line skipped.");
        } else if (city.hasLocation(id)) {
            messages.push_back(where + "ID " + to_string(id) + " is used twice. Line skipped.");
        } else if (city.nameExists(name)) {
            // Names are compared ignoring upper/lower case.
            messages.push_back(where + "the name \"" + name + "\" is used twice. Line skipped.");
        } else if (!city.addLocationWithId(id, name)) {
            messages.push_back(where + "invalid location. Line skipped.");
        }
    }
}

// Reads roads.csv. Each valid line adds one two-way road.
static void loadRoads(Graph& city, const string& file, vector<string>& messages)
{
    ifstream input(file);
    if (!input) {
        messages.push_back("No saved roads found (" + file + ").");
        return;
    }

    string line;
    int lineNumber = 0;
    while (getline(input, line)) {
        lineNumber++;
        removeCarriageReturn(line);
        if ((lineNumber == 1 && line == "from,to,distance") || line.empty()) {
            continue;   // skip the header line and empty lines
        }

        string where = file + " line " + to_string(lineNumber) + ": ";
        vector<string> parts = splitByComma(line);
        int from;
        int to;
        double distance;

        if (parts.size() != 3) {
            messages.push_back(where + "expected 3 values (from,to,distance). Line skipped.");
        } else if (!textToInt(parts[0], from) || !textToInt(parts[1], to)) {
            messages.push_back(where + "location IDs must be whole numbers. Line skipped.");
        } else if (!textToDouble(parts[2], distance) || !isValidDistance(distance)) {
            messages.push_back(where + "the distance must be a number greater than 0 and at most " +
                               to_string(static_cast<int>(MAX_ROAD_DISTANCE)) + " km. Line skipped.");
        } else if (!city.hasLocation(from) || !city.hasLocation(to)) {
            messages.push_back(where + "the road uses a location ID that does not exist. Line skipped.");
        } else if (from == to) {
            messages.push_back(where + "a road cannot connect a location to itself. Line skipped.");
        } else if (city.hasRoad(from, to)) {
            messages.push_back(where + "this road is listed twice. Line skipped.");
        } else if (!city.addRoad(from, to, distance)) {
            messages.push_back(where + "invalid road. Line skipped.");
        }
    }
}

// Reads next_id.txt. After loadLocations(), the next ID is already
// "highest ID + 1". The saved value is used only if it is larger, which
// happens when the location with the highest ID was deleted earlier.
static void loadNextId(Graph& city, const string& file, vector<string>& messages)
{
    ifstream input(file);
    if (!input) {
        // Only worth a note if there are locations; a new city needs no file.
        if (city.locationCount() > 0) {
            messages.push_back("No saved next ID found (" + file +
                               "). The next ID is the highest location ID + 1.");
        }
        return;
    }

    string line;
    getline(input, line);
    removeCarriageReturn(line);
    int savedId;

    if (!textToInt(line, savedId) || savedId <= 0) {
        messages.push_back(file + ": the next ID must be a positive whole number. "
                           "The next ID is the highest location ID + 1.");
    } else if (savedId < city.getNextId()) {
        messages.push_back(file + ": the next ID " + to_string(savedId) +
                           " is already used by a location. The next ID is the highest location ID + 1.");
    } else {
        city.setNextId(savedId);
    }
}

void loadCity(Graph& city, const string& locationsFile, const string& roadsFile,
              const string& nextIdFile, vector<string>& messages)
{
    // Locations must be loaded first, because roads refer to location IDs.
    loadLocations(city, locationsFile, messages);
    loadRoads(city, roadsFile, messages);
    loadNextId(city, nextIdFile, messages);
}

// ===== Data folder and first run =====

string findDataFolder(const vector<string>& candidates)
{
    error_code error;   // receives problems instead of stopping the program
    for (const string& folder : candidates) {
        filesystem::path path(folder);
        if (filesystem::exists(path / "locations.csv", error) ||
            filesystem::is_directory(path / "sample", error)) {
            return folder;
        }
    }
    return candidates.empty() ? "data" : candidates[0];
}

bool initializeFromSample(const string& sampleFolder, const string& locationsFile,
                          const string& roadsFile, const string& nextIdFile)
{
    error_code error;
    const vector<string> workingFiles = {locationsFile, roadsFile, nextIdFile};
    const vector<string> sampleNames = {"locations.csv", "roads.csv", "next_id.txt"};

    // Only on a real first run: if ANY working file exists, do nothing.
    for (const string& file : workingFiles) {
        if (filesystem::exists(file, error)) {
            return false;
        }
    }
    // All three sample files must be there.
    for (const string& name : sampleNames) {
        if (!filesystem::exists(filesystem::path(sampleFolder) / name, error)) {
            return false;
        }
    }

    createFolderFor(locationsFile);
    for (size_t i = 0; i < workingFiles.size(); i++) {
        // copy_options::none never overwrites: copying fails if the file exists.
        bool copied = filesystem::copy_file(filesystem::path(sampleFolder) / sampleNames[i],
                                            workingFiles[i], filesystem::copy_options::none,
                                            error);
        if (!copied) {
            // Undo the files copied in this call, so the next start is a clean first run.
            for (size_t j = 0; j < i; j++) {
                filesystem::remove(workingFiles[j], error);
            }
            return false;
        }
    }
    return true;
}
