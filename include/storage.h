// Smart City Route & Infrastructure Planner
// Copyright © 2026 Quad Core. All rights reserved.
//
// storage.h - saving and loading the city in three small files.
//
// locations.csv            roads.csv               next_id.txt
//   id,name                  from,to,distance        6
//   1,BUBT                   1,2,2.5
//   2,Mirpur 10              ...
//
// The first line of each CSV file is a header ("id,name" or "from,to,distance")
// and is skipped when loading. A file without a header is also accepted.
// next_id.txt holds the ID for the next new location, so that the ID of a
// deleted location is never used again, even after restarting the program.
// This file does not print anything; problems are returned as messages so
// that the menu program can show them.

#ifndef STORAGE_H
#define STORAGE_H

#include "graph.h"

#include <string>
#include <vector>

// Writes all locations, all roads and the next location ID to the three files.
// Creates the folder (for example "data") if it does not exist.
// Returns false if a file could not be written.
bool saveCity(const Graph& city, const std::string& locationsFile, const std::string& roadsFile,
              const std::string& nextIdFile);

// Reads the three files into `city`, which should be empty.
// Location IDs are kept exactly as saved.
// Invalid or duplicate lines are skipped, and a message is added to
// `messages` for each problem. A missing file is not an error: the city
// simply stays empty. If next_id.txt is missing or invalid, the next ID
// becomes "highest location ID + 1" (a message explains this).
void loadCity(Graph& city, const std::string& locationsFile, const std::string& roadsFile,
              const std::string& nextIdFile, std::vector<std::string>& messages);

#endif // STORAGE_H
