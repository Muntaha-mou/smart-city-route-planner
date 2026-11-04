# Smart City Route & Infrastructure Planner

A C++17 command-line program that models a city road network as a weighted
graph and uses graph algorithms to explore the network, find shortest routes
and plan a minimum road network.

**Team:** Quad Core
**Course:** CSE 232 — Algorithms project

> **Status:** Milestone S5 — locations and roads can be added, shown, updated
> and deleted, the city is saved to CSV files, the network can be explored with
> BFS and DFS, the shortest route is found with Dijkstra's algorithm, and the
> minimum road network is found with Kruskal's algorithm.

## Features

| Feature | Status |
|---|---|
| Add and show locations | Done (S1) |
| Add and show roads | Done (S1) |
| Show the city network (adjacency list) | Done (S1) |
| Rename and delete locations (deleting also removes their roads) | Done (S2) |
| Update road distances and delete roads | Done (S2) |
| Save and load data (CSV files) | Done (S2) |
| Explore the network with BFS and DFS | Done (S3) |
| Shortest route with Dijkstra's algorithm | Done (S4) |
| Minimum spanning tree with Kruskal's algorithm | Done (S5) |

## Project Structure

```
CMakeLists.txt      Build instructions for CMake
include/graph.h     Edge and Road structs, Graph class (declarations)
include/storage.h   Saving and loading the city (declarations)
include/algorithms.h  Graph algorithms: BFS, DFS, Dijkstra, Kruskal (declarations)
src/graph.cpp       Graph functions (the city data)
src/storage.cpp     CSV save and load
src/algorithms.cpp  BFS, DFS, Dijkstra and Kruskal
src/main.cpp        Menu program: input, output and menu options
tests/tests.cpp     Automatic checks for the Graph, saving/loading and all algorithms
data/sample/        Illustrative sample data (10 Dhaka locations, 14 roads)
docs/               Project documentation
future-gui/         Qt 6 GUI prototype (not built; kept for a later version)
```

`graph.cpp`, `storage.cpp` and `algorithms.cpp` never print or read the
keyboard; only `main.cpp` does. This keeps the graph code reusable for a future Qt GUI.

## Data Files

The program loads the city from three files when it starts and saves them
again after every change:

| File | Format | Example line |
|---|---|---|
| `data/locations.csv` | `id,name` | `1,BUBT` |
| `data/roads.csv` | `from,to,distance` | `1,2,2.5` |
| `data/next_id.txt` | one number: the ID for the next new location | `11` |

- `next_id.txt` makes sure the ID of a deleted location is never given to a new
  location, even after a restart. If it is missing or invalid, the program uses
  "highest location ID + 1" and shows a note.

- If the files do not exist, the program starts with an empty city.
- Invalid lines (wrong number of values, non-numeric IDs, duplicate IDs or
  names, unknown locations, non-positive distances, duplicate roads) are
  skipped, and a note with the line number is shown at start-up.
- The paths are relative to the folder the program is started in, so start it
  from the project folder.
- These working files are not stored in Git (see `.gitignore`).

**Sample data:** to start with the sample network, copy the three sample files
into the `data` folder before starting the program:

```powershell
Copy-Item data\sample\* data\
```

The sample distances are **illustrative values**, not measured distances.

## Requirements (Windows 10/11)

- MinGW-w64 GCC with C++17 support (tested with GCC 15.1.0)
- CMake 3.21 or newer (tested with 3.30.5)
- Ninja (tested with 1.12.1)

## Build, Test and Run

Open **PowerShell** in the project folder:

```powershell
# Put the compiler, CMake and Ninja on the PATH (adjust to your install location)
$env:PATH = "S:\Qt\Tools\mingw1510_64\bin;S:\Qt\Tools\CMake_64\bin;S:\Qt\Tools\Ninja;$env:PATH"

cmake -S . -B build -G Ninja                 # configure (once)
cmake --build build                          # build
ctest --test-dir build --output-on-failure   # run the tests
.\build\SmartCityPlanner.exe                 # start the program (from the project folder)
```

The program needs the MinGW runtime DLLs, so start it from a terminal where the
compiler folder is on the PATH (as above).

## Documentation

| Document | Description |
|---|---|
| [docs/algorithms.md](docs/algorithms.md) | The graph data structure and the algorithms |
| [docs/user-guide.md](docs/user-guide.md) | How to use the program |
| [docs/development-notes.md](docs/development-notes.md) | Toolchain, code walkthrough, Git workflow, decisions |
| `docs/project-proposal.pdf` | Official project proposal *(to be added)* |
| `docs/project-report.pdf` | Official project report *(to be added)* |

---

Copyright © 2026 Quad Core. All rights reserved.
