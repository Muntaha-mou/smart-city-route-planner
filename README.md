# Smart City Route & Infrastructure Planner

A C++17 command-line program that models a city road network as a weighted
graph. Later milestones add graph algorithms to explore the network, find
shortest routes and plan a minimum road network.

**Team:** Quad Core
**Course:** CSE 232 — Algorithms project

> **Status:** Milestone S1 — add and show locations and roads, show the city
> network. Data is not saved yet.

## Features

| Feature | Status |
|---|---|
| Add and show locations | Done (S1) |
| Add and show roads | Done (S1) |
| Show the city network (adjacency list) | Done (S1) |
| Edit and delete locations and roads | Planned |
| Save and load data (CSV files) | Planned |
| Explore the network with BFS and DFS | Planned |
| Shortest route with Dijkstra's algorithm | Planned |
| Minimum spanning tree with Kruskal's algorithm | Planned |

## Project Structure

```
CMakeLists.txt      Build instructions for CMake
include/graph.h     Edge and Road structs, Graph class (declarations)
src/graph.cpp       Graph functions (the city data)
src/main.cpp        Menu program: input, output and menu options
tests/tests.cpp     Automatic checks for the Graph class
docs/               Project documentation
future-gui/         Qt 6 GUI prototype (not built; kept for a later version)
```

`graph.cpp` never prints or reads the keyboard; only `main.cpp` does. This keeps
the graph code reusable for a future Qt GUI.

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
.\build\SmartCityPlanner.exe                 # start the program
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
