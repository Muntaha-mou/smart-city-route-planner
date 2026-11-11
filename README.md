# Smart City Route & Infrastructure Planner

A C++17 console application that models a city road network as a **weighted, undirected graph** and applies classic graph algorithms to answer planning
questions: which locations can be reached, what is the shortest route, which
roads keep the city connected at the lowest total length, how far apart every
pair of locations is, and where a facility such as a hospital should be built.

**Course:** Algorithms Lab – CSE 232 · Department of Computer Science and
Engineering (CSE) · Bangladesh University of Business and Technology (BUBT)
**Team:** Quad Core

> **Status:** all planned features are implemented and tested (development
> milestones S1–S8.3). The automated test suite contains **400 checks**, all
> passing with both supported build configurations.

## Contents

1. [Project Overview](#project-overview)
2. [Objectives](#objectives)
3. [Key Features](#key-features)
4. [Algorithms Implemented](#algorithms-implemented)
5. [How the Algorithms Are Used](#how-the-algorithms-are-used)
6. [Technology Stack](#technology-stack)
7. [System Architecture and Project Structure](#system-architecture-and-project-structure)
8. [Development Methodology](#development-methodology)
9. [Prerequisites and Compiler Requirements](#prerequisites-and-compiler-requirements)
10. [Installing MinGW-w64 (WinLibs)](#installing-mingw-w64-winlibs)
11. [Building with CMake](#building-with-cmake)
12. [Running the Application](#running-the-application)
13. [Sample Data and Testing](#sample-data-and-testing)
14. [Application Workflow](#application-workflow)
15. [Documentation](#documentation)
16. [Team](#team)
17. [Limitations](#limitations)
18. [Future Improvements](#future-improvements)
19. [License](#license)

---

## Project Overview

The city is stored as a graph: **locations are vertices**, **roads are
edges**, and each road is weighted by its **distance in kilometres**. Roads are
two-way. The user builds and edits the network from a menu, the network is
saved automatically to CSV files, and every algorithm works on the same saved
network, so their results can be compared directly.

This is an **educational simulation**, not a real-world navigation or urban
planning tool. The included sample network of ten Dhaka locations uses
**illustrative distances**, not measured ones.

## Objectives

- Represent a city road network as a weighted, undirected graph using an
  **adjacency list**.
- Provide complete management of locations and roads (add, view, update,
  delete) with input validation.
- Explore connectivity with **BFS** and **DFS**.
- Find shortest routes with **Dijkstra's algorithm** (one pair) and the
  **Floyd–Warshall algorithm** (all pairs).
- Find a minimum road network with **Kruskal's algorithm** and a **Disjoint
  Set Union (DSU)**.
- Suggest the **best facility location** using eccentricity, network centre,
  radius and diameter.
- Store the network in **CSV files** and verify every algorithm with
  **automated tests**.

## Key Features

| Area | Features |
|---|---|
| Locations | Add, show, rename and delete locations. IDs are assigned automatically and never reused. Names are trimmed and must be unique (ignoring upper/lower case). Deleting a location also deletes its roads. |
| Roads | Add, show, update and delete two-way roads. Both locations must exist, a road cannot join a location to itself, at most one road per pair, distance greater than 0 and at most 10,000 km. |
| Network display | The whole network as an adjacency list with road distances. |
| Algorithms | BFS, DFS, Dijkstra, Kruskal with DSU, Floyd–Warshall and the best facility location (menu options 10–15). |
| Disconnected networks | Unreachable locations are listed; Kruskal returns a Minimum Spanning Forest; the facility analysis reports each connected part separately and lists isolated locations. |
| Data storage | Automatic CSV saving after every change; invalid lines in the files are skipped with a message; the sample network is loaded on the first run. |
| Console interface | Menu-driven dashboard with a colour theme on Windows terminals and an automatic plain-text mode. |
| Testing | 400 automated checks run through CTest. |

## Algorithms Implemented

V = number of locations, E = number of roads.

| Algorithm | Technique | Menu option | Time | Extra memory |
|---|---|---|---|---|
| Breadth-First Search (BFS) | Queue, level by level | 10 | O(V + E) | O(V) |
| Depth-First Search (DFS) | Recursion, backtracking | 11 | O(V + E) | O(V) |
| Dijkstra's shortest path | Greedy, min-priority queue | 12 | O((V + E) log V) | O(V + E) |
| Kruskal's minimum spanning tree | Greedy, sorting + Disjoint Set Union (path compression, union by rank) | 13 | O(E log E) | O(V + E) |
| Floyd–Warshall all-pairs shortest paths | Dynamic programming, distance and next-hop matrices | 14 | O(V³) | O(V²) |
| Best facility location (eccentricity, centre, radius, diameter) | Analysis of the Floyd–Warshall distance matrix | 15 | O(V²) after Floyd–Warshall | O(V) |

BFS and DFS also sort each neighbour list so that the visiting order is always
the same, which adds O(E log V) in the worst case. Step-by-step explanations
and worked examples are in [docs/algorithms.md](docs/algorithms.md).

## How the Algorithms Are Used

- **BFS and DFS** start from a chosen location and show the visiting order,
  how many locations were reached and which locations cannot be reached
  (connectivity of the road network).
- **Dijkstra's algorithm** finds the route with the smallest total distance
  between two locations and shows every road on the route.
- **Kruskal's algorithm** selects existing roads with the smallest total length
  that keep all locations connected (a Minimum Spanning Tree). If the network
  is in several parts, it returns a Minimum Spanning Forest and reports the
  number of parts. The DSU detects roads that would form a cycle.
- **Floyd–Warshall** computes the shortest distance between every pair of
  locations, shows a distance matrix for small networks and rebuilds the route
  between any two locations. Its distances always agree with Dijkstra's
  (checked by the tests).
- **Best facility location** uses the Floyd–Warshall distances. A location's
  *eccentricity* is the distance to its farthest location; the *network
  centre* (smallest eccentricity, the *radius*) is the best place for a
  hospital or fire station, and the *diameter* is the largest shortest
  distance. Values within 1e-9 km count as equal, so ties are all reported.

## Technology Stack

| Component | Details |
|---|---|
| Language | C++17 (standard library only; Windows console API for colours) |
| Compiler | GCC 16.2.0, WinLibs MinGW-w64 (tested) |
| Build system | CMake 3.21 or newer (tested with 4.4.4) |
| Build generators | MinGW Makefiles (`mingw32-make`) or Ninja |
| Testing | Custom `check()` test program run through CTest |
| Data storage | CSV and text files |
| Platform | Windows 10 / 11 (development and testing platform) |
| Version control | Git and GitHub |

## System Architecture and Project Structure

The program has three layers. The **data and algorithm layer** never reads the
keyboard or prints anything; it only returns results. Only the **menu layer**
talks to the user, using the screen helpers of the **UI layer**. This keeps
the algorithms testable and reusable (for example by a future GUI).

```
smart-city-route-planner/
├── CMakeLists.txt        Build configuration (C++17, -Wall -Wextra)
├── LICENSE               MIT License
├── include/
│   ├── graph.h           Edge, Road, Graph class, validation rules
│   ├── storage.h         CSV save/load, data folder, first-run sample copy
│   ├── algorithms.h      BFS, DFS, Dijkstra, Kruskal, Floyd–Warshall, facility location
│   └── ui.h              Console colours and screen layout helpers
├── src/
│   ├── graph.cpp         Adjacency-list graph (the city data)
│   ├── storage.cpp       CSV files
│   ├── algorithms.cpp    All graph algorithms
│   ├── ui.cpp            Colours, tables, messages, plain-text fallback
│   └── main.cpp          Menu program: input, output, one function per option
├── tests/
│   └── tests.cpp         400 automated checks
├── data/
│   └── sample/           Sample network: 10 Dhaka locations, 14 roads
├── docs/                 Algorithms, user guide, development notes, proposal/
└── future-gui/           Parked Qt 6 prototype (not built)
```

The graph is stored as `std::map<int, std::vector<Edge>>` (location ID → its
roads) plus `std::map<int, std::string>` for the names. Every road appears in
the lists of both of its locations.

## Development Methodology

The project follows the **Incremental Development Model**: each increment was
designed, coded, tested with the automated test suite and committed to the
`dev` branch before the next one began.

| Increment | Content |
|---|---|
| S1 | Graph structure, add/view locations and roads, menu, network display, tests |
| S2 | Update/delete operations, CSV save/load, sample data |
| S3 | BFS and DFS |
| S4 | Dijkstra's shortest route |
| S5 | Kruskal's minimum spanning tree with DSU |
| S6 | Validation improvements (name and distance rules), audit, documentation |
| S7 | Colour console interface, first-run sample data |
| S8 | Floyd–Warshall all-pairs shortest paths, best facility location |

The full history, commit references and design decisions are recorded in
[docs/development-notes.md](docs/development-notes.md).

## Prerequisites and Compiler Requirements

| Requirement | Version used and tested |
|---|---|
| Operating system | Windows 10 or Windows 11 |
| C++ standard | C++17 (`CMAKE_CXX_STANDARD 17`, required) |
| Compiler | GCC 16.2.0 (WinLibs MinGW-w64); any MinGW-w64 GCC with C++17 support should work |
| CMake | 3.21 or newer (`cmake_minimum_required(VERSION 3.21)`); tested with 4.4.4 |
| Build tool | `mingw32-make` (GNU Make 4.4.1) or Ninja 1.13.2 |
| Git | Optional, to clone the repository |

Compiler package used for development:

| Property | Value |
|---|---|
| Distribution | WinLibs MinGW-w64 (standalone build by Brecht Sanders) |
| Architecture | x86_64 (64-bit) |
| GCC version | 16.2.0 |
| MinGW-w64 version | 14.0.0 (the Windows runtime headers and libraries, not the compiler version) |
| Thread model | POSIX |
| Exception model | SEH |
| C runtime | UCRT (Universal C Runtime) |

This WinLibs package also contains **CMake 4.4.4**, **Ninja 1.13.2** and **GNU
Make 4.4.1** in its `bin` folder, so no separate installation of these tools is
needed.

## Installing MinGW-w64 (WinLibs)

1. **Download** the ZIP archive (about 276 MB):

   <https://github.com/brechtsanders/winlibs_mingw/releases/download/16.2.0posix-23.1.3-14.0.0-ucrt-r3/winlibs-x86_64-posix-seh-gcc-16.2.0-mingw-w64ucrt-14.0.0-r3.zip>

2. **Extract** it to a short path without spaces, for example `C:\`. This
   creates the folder `C:\mingw64`, with the tools in `C:\mingw64\bin`.

3. **Add `C:\mingw64\bin` to the Windows PATH.** Either open *Start → "Edit
   the system environment variables" → Environment Variables → Path → Edit →
   New*, or run this once in PowerShell (it adds the folder to your user PATH):

   ```powershell
   $bin = "C:\mingw64\bin"
   $userPath = [Environment]::GetEnvironmentVariable("Path", "User")
   [Environment]::SetEnvironmentVariable("Path", "$userPath;$bin", "User")
   ```

4. **Open a new PowerShell window** (old windows keep the old PATH) and
   verify the installation:

   ```powershell
   g++ --version
   gcc --version
   cmake --version
   ```

   The first line of `g++ --version` should be:

   ```
   g++.exe (MinGW-W64 x86_64-ucrt-posix-seh, built by Brecht Sanders, r3) 16.2.0
   ```

   If a command "is not recognized", the `bin` folder is not on the PATH.

## Building with CMake

Open PowerShell in the project folder (the folder that contains
`CMakeLists.txt`):

```powershell
cd "P:\BUBT\1-4\CSE 232\smart-city-route-planner"   # use your own project path
```

### Option A: MinGW Makefiles (main configuration)

```powershell
cmake -S . -B build-mingw -G "MinGW Makefiles"   # configure (once)
cmake --build build-mingw                        # build
ctest --test-dir build-mingw --output-on-failure # run the automated tests
```

### Option B: Ninja

```powershell
cmake -S . -B build -G Ninja                     # configure (once)
cmake --build build                              # build
ctest --test-dir build --output-on-failure       # run the automated tests
```

Each build folder contains `SmartCityPlanner.exe` (the application) and
`tests.exe` (the test program). A build folder can only use one generator, so
the two options use separate folders. After changing the code, run only the
`cmake --build` command again. Build folders are not stored in Git.

## Running the Application

```powershell
.\build-mingw\SmartCityPlanner.exe          # or .\build\SmartCityPlanner.exe
.\build-mingw\SmartCityPlanner.exe --plain  # plain text, no colours
```

- Start the program from the **project folder**. It uses the `data` folder
  there; if it is started elsewhere (for example by double-clicking the
  `.exe`), it looks for the project's `data` folder next to the program and
  shows which folder it uses.
- The program needs the MinGW runtime DLLs, so `C:\mingw64\bin` must be on the
  PATH.
- After each screen, press **Enter** to return to the main menu. Choose `0` to
  exit. The terminal colours and settings are restored on exit and on Ctrl+C.
- Colours are switched off automatically when output is redirected, and can be
  switched off with `--plain` or the `NO_COLOR` environment variable.

## Sample Data and Testing

### Data files

The working data is stored in the `data` folder. These files are **not stored
in Git**; the repository only contains the sample in `data/sample/`.

| File | Content | Example |
|---|---|---|
| `data/locations.csv` | header `id,name`, then one location per line | `1,BUBT` |
| `data/roads.csv` | header `from,to,distance` (location IDs and km), one road per line, each road once | `1,2,2.5` |
| `data/next_id.txt` | the ID the next new location will get | `11` |

- **First launch:** if **none** of the three working files exists, the program
  copies the sample network from `data/sample/` (10 locations, 14 roads) and
  says so on the start screen. If any working file exists, nothing is copied,
  so saved data is **never overwritten**.
- The program saves automatically after every successful change.
- Invalid lines (wrong number of values, unknown or duplicate IDs, duplicate
  names, invalid distances, duplicate roads) are skipped with a message that
  names the file and line.

The sample network (`data/sample/`) contains these locations: 1 BUBT,
2 Mirpur 10, 3 Mirpur 1, 4 Agargaon, 5 Farmgate, 6 Dhanmondi, 7 Mohammadpur,
8 Uttara, 9 Airport and 10 Gulshan, joined by 14 roads.

### Testing safely in a separate folder

To test without touching your own data, run the program in a temporary
folder. It finds the copied sample there and loads it on the first run:

```powershell
$test = "$env:TEMP\smartcity-test"
New-Item -ItemType Directory "$test\data\sample" -Force | Out-Null
Copy-Item data\sample\* "$test\data\sample\"
Copy-Item build-mingw\SmartCityPlanner.exe $test
Set-Location $test
.\SmartCityPlanner.exe
```

To start that test again with the original sample, delete the three working
files **in the test folder only** and restart the program:

```powershell
Remove-Item "$test\data\locations.csv", "$test\data\roads.csv", "$test\data\next_id.txt"
```

To test an **empty** network, use a test folder without the `sample` folder.

### A small custom test network

Create these three files in the `data` folder of a test folder (not in your
working data):

```
locations.csv        roads.csv            next_id.txt
id,name              from,to,distance     4
1,A                  1,2,4
2,B                  2,3,3
3,C
```

Expected results: shortest route A → C = **7.00 km** via B (option 12);
minimum spanning tree = both roads, **7.00 km** (option 13); best facility
location = **B [2]**, farthest location 4.00 km, diameter **7.00 km** between
A [1] and C [3] (option 15).

### Automated tests

```powershell
ctest --test-dir build-mingw --output-on-failure
```

The expected result is `100% tests passed out of 1`: CTest runs one program,
`tests.exe`, which performs all **400 checks**. Running it directly shows every
check and ends with `400 passed, 0 failed`:

```powershell
.\build-mingw\tests.exe
```

The tests write temporary files to a `test_data` folder (ignored by Git) and
never touch `data/`.

### Manual test scenarios (sample network)

Run these in a separate test folder (see above). The expected values come from
the sample network.

| # | Scenario | Steps | Expected result |
|---|---|---|---|
| 1 | Connected network | Option 2, then 9 | 10 locations; 14 roads in the adjacency list |
| 2 | Shortest path | Option 12, start 1, destination 10 | BUBT → Mirpur 10 → Agargaon → Farmgate → Gulshan, **15.00 km** (4 roads) |
| 3a | BFS | Option 10, start 1 | BUBT, Mirpur 10, Mirpur 1, Agargaon, Airport, Mohammadpur, Farmgate, Uttara, Gulshan, Dhanmondi; reached 10 of 10 |
| 3b | DFS | Option 11, start 1 | BUBT, Mirpur 10, Mirpur 1, Mohammadpur, Agargaon, Farmgate, Dhanmondi, Gulshan, Uttara, Airport; reached 10 of 10 |
| 4 | Minimum spanning tree | Option 13 | 9 of 14 roads selected, total **35.20 km**, network connected |
| 5 | All-pairs shortest paths | Option 14, start 1, destination 10 | 10 × 10 distance matrix (row [1] ends with 15.00); same 15.00 km route as scenario 2 |
| 6 | Best facility location | Option 15 | Best location **Mirpur 10 [2]**, farthest location 14.00 km; diameter **21.70 km** between Mohammadpur [7] and Uttara [8] |
| 7 | Isolated location | Option 8, delete road 8–9; option 8, delete road 8–10; then options 10 (start 1), 13 and 15 | BFS reaches 9 of 10, Uttara [8] not reachable; Kruskal: Minimum Spanning Forest, 2 parts, 8 of 12 roads, 29.70 km; option 15: 2 separate parts, Part 1 best location Farmgate [5] (12.00 km), Uttara listed as **isolated** |
| 8 | Multiple components | After scenario 7: add locations "Purbachal" and "Kanchan" (IDs 11 and 12), add road 11–12 of 2 km, then option 15 | 3 separate parts; Part 3 shows Purbachal [11] and Kanchan [12] as equally good (2.00 km) |
| 9 | Single location | Empty test folder: add one location, then option 15 | That location is the centre; radius 0.00 km, diameter 0.00 km |
| 10 | Empty network | Empty test folder: options 10–15 | Each option says that the network is empty |
| 11 | Invalid input | Type `abc` at the menu; type `99`; add location `  bubt  `; add a road of `20000` km | "Invalid input. Please enter a whole number."; "Please choose a number from the menu."; "A location named "BUBT" already exists."; "The distance must be greater than 0 and at most 10000.00 km." |
| 12 | Persistence | Add a location, exit with 0, start again | The new location is still there; a deleted location's ID is never given to a new location |

## Application Workflow

On start, the program loads (or, on the first run, creates) the city and shows
the main menu. In plain-text mode it looks like this:

```
   LOCATIONS 10   |   ROADS 14   |   DATA data  (saved automatically)

  LOCATIONS                          ROADS
  ------------------------------     ----------------------------------
    1  Add Location                   5  Add Road
    2  Show Locations                 6  Show Roads
    3  Rename Location                7  Update Road Distance
    4  Delete Location                8  Delete Road

  NETWORK & ALGORITHMS
  ------------------------------
    9  Show City Network             12  Dijkstra's Shortest Route
   10  BFS Traversal                 13  Kruskal's Minimum Spanning Tree
   11  DFS Traversal
   14  All-Pairs Shortest Paths (Floyd-Warshall)
   15  Best Facility Location

    0  Exit
```

A typical session: check the network (options 2, 6, 9), edit it (options 1–8;
every change is saved immediately), then run the algorithms (options 10–15).
Each screen asks only for the IDs it needs and explains invalid input. The
[user guide](docs/user-guide.md) describes every option with examples.

## Documentation

| Document | Content |
|---|---|
| [docs/algorithms.md](docs/algorithms.md) | Graph representation and every algorithm, with steps, worked examples and complexity |
| [docs/user-guide.md](docs/user-guide.md) | Building (Command Prompt) and using every menu option |
| [docs/development-notes.md](docs/development-notes.md) | Toolchain, code walkthrough, tests, Git workflow, decision log and milestones |
| [Project proposal (PDF)](docs/proposal/smart-city-route-planner-proposal-revised.pdf) | Revised project proposal |
| [Project proposal (Word)](docs/proposal/smart-city-route-planner-proposal-revised.docx) | Revised project proposal, editable version |

## Team

**Team Quad Core**, Algorithms Lab – CSE 232, Department of CSE, BUBT.

| Student ID | Name | Role |
|---|---|---|
| 20254103245 | MUNTAHA MOU JIM | Member |
| 20254103248 | OALID KHAN | Member |
| 20254103269 | MD. RAFIUL ISLAM | Member |
| 20254103279 | MAKHZUM-BIN-HARUN | Team Leader |

## Limitations

- Console interface only; the network is an abstract graph, not a map.
- Sample distances are illustrative, not measured.
- Each road has one weight (distance); travel time, traffic and cost are not
  considered. All roads are two-way, with at most one road per pair.
- The best facility location chooses among existing locations only and
  minimises the farthest distance; population and demand are not considered.
- The full distance matrix is shown only for small networks (about 10
  locations).
- Developed and tested on Windows only.

## Future Improvements

These are **not implemented**:

- A graphical user interface with interactive map visualisation (a parked
  Qt 6 prototype is in `future-gui/`).
- The real-world Dhaka road network and real map data.
- Traffic-aware route optimisation and emergency vehicle route planning.
- Advanced facility-location models (several facilities, population weights).
- Real-time traffic and infrastructure data.

## License

This project is released under the **MIT License**: it may be used, copied,
modified and distributed, provided the copyright notice and license text are
kept. See [LICENSE](LICENSE) for the full text.

Copyright (c) 2026 Quad Core
