# Smart City Route & Infrastructure Planner

A C++17 command-line program that models a city road network as a weighted
graph and uses graph algorithms to explore the network, find shortest routes
and plan a minimum road network.

**Team:** Quad Core
**Course:** CSE 232 — Algorithms project

> **Status:** Milestone S6 (final review) — all planned features are complete:
> locations and roads can be added, shown, updated and deleted, the city is
> saved to CSV files, the network can be explored with BFS and DFS, the shortest
> route is found with Dijkstra's algorithm, and the minimum road network is found
> with Kruskal's algorithm.

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
  names, unknown locations, distances not between 0 and 10,000 km, duplicate
  roads) are skipped, and a note with the line number is shown at start-up.
- The paths are relative to the folder the program is started in, so start it
  from the project folder.
- These working files are not stored in Git (see `.gitignore`).

**Sample data:** to start with the sample network, copy the three sample files
into the `data` folder before starting the program. In PowerShell:

```powershell
Copy-Item data\sample\* data\
```

This **replaces** any working files already in `data`. To copy only the
missing files in CMD, see step 5 of
[Windows: Build and Run (CMD)](#windows-build-and-run-cmd).

The sample distances are **illustrative values**, not measured distances.

## Requirements (Windows 10/11)

- MinGW-w64 GCC with C++17 support (currently tested with WinLibs GCC 16.2.0)
- CMake 3.21 or newer (currently tested with 4.4.4)
- MinGW Makefiles (`mingw32-make`, included with MinGW-w64): the main build
  method
- Ninja (optional): another build generator that can be used instead
- Git (optional, only needed to clone the repository)

(Earlier milestones were built with GCC 15.1.0, CMake 3.30.5 and Ninja 1.12.1;
see [docs/development-notes.md](docs/development-notes.md).)

The program is a console application: Qt is **not** needed. It has been built
and tested on Windows only.

There are two ways to build, described below: **Command Prompt (CMD)** with
MinGW Makefiles, or **PowerShell** with Ninja. Both create the same program.

## Windows: Build and Run (CMD)

All commands in this section are for the Windows **Command Prompt** (`cmd.exe`).

### 1. Prerequisites

Install MinGW-w64 GCC and CMake, and add their `bin` folders to the Windows
PATH. Git is optional. Open a **new** Command Prompt and check that both tools
are found:

```cmd
g++ --version
cmake --version
```

Each command should print a version number. If you see "is not recognized as
an internal or external command", the tool is not on the PATH.

### 2. Go to the project folder

```cmd
cd /d "P:\BUBT\1-4\CSE 232\smart-city-route-planner"
```

Replace the path with the folder where the project is on your PC. The `/d`
option lets `cd` change the drive (for example from `C:` to `P:`) and the
folder in one step. Keep the quotes, because the path contains spaces.

### 3. Configure the project (once)

```cmd
cmake -S . -B build-mingw -G "MinGW Makefiles"
```

This reads `CMakeLists.txt` and prepares the build files in the `build-mingw`
folder. (Build folders are not stored in Git.)

### 4. Build the project

```cmd
cmake --build build-mingw
```

This compiles the program to `build-mingw\SmartCityPlanner.exe` (and the test
program to `build-mingw\tests.exe`).

### 5. Prepare the sample data (first run only)

The program reads and saves its data in the `data` folder (`locations.csv`,
`roads.csv` and `next_id.txt`). Sample files are provided in `data\sample`.
These commands copy each sample file **only if** the working file does not
exist yet, so your own saved data is never overwritten:

```cmd
if not exist data mkdir data
if not exist data\locations.csv copy data\sample\locations.csv data\locations.csv
if not exist data\roads.csv copy data\sample\roads.csv data\roads.csv
if not exist data\next_id.txt copy data\sample\next_id.txt data\next_id.txt
```

This step is optional: without these files, the program starts with an empty
city.

### 6. Run the program

```cmd
build-mingw\SmartCityPlanner.exe
```

Always start the program from the **project folder** (the folder that contains
`data`), because it opens the data files with paths relative to the current
folder. Choose `0` in the menu to exit.

### 7. Run the automated tests

```cmd
ctest --test-dir build-mingw --output-on-failure
```

If everything works, the output looks like this (the times will differ):

```
1/1 Test #1: tests ............................   Passed    0.04 sec

100% tests passed out of 1
```

CTest runs one test program (`tests.exe`) that performs all the checks, so
"out of 1" is expected. If any check fails, the test is marked **Failed** and
`--output-on-failure` shows every check with **PASS** or **FAIL**.

### 8. Rebuild after changing the code

You do not need to configure again. Build and run:

```cmd
cmake --build build-mingw
build-mingw\SmartCityPlanner.exe
```

## Windows: Build, Test and Run (PowerShell, Ninja)

Open **PowerShell** in the project folder:

```powershell
# Put the compiler, CMake and Ninja on the PATH.
# The folders below are examples: use the locations on your own PC.
$env:PATH = "C:\mingw64\bin;C:\Program Files\CMake\bin;C:\Ninja;$env:PATH"

cmake -S . -B build -G Ninja                 # configure (once)
cmake --build build                          # build
ctest --test-dir build --output-on-failure   # run the tests
.\build\SmartCityPlanner.exe                 # start the program (from the project folder)
```

**Without Ninja:** configure with MinGW Makefiles instead; the other commands
stay the same. (In CMD, follow the section above.)

```powershell
cmake -S . -B build -G "MinGW Makefiles"
```

A `build` folder can only use one generator. To switch, delete the `build`
folder first.

The program needs the MinGW runtime DLLs, so start it from a terminal where the
compiler folder is on the PATH (as above).

## Documentation

| Document | Description |
|---|---|
| [docs/algorithms.md](docs/algorithms.md) | The graph data structure and the algorithms |
| [docs/user-guide.md](docs/user-guide.md) | How to use the program |
| [docs/development-notes.md](docs/development-notes.md) | Toolchain, code walkthrough, Git workflow, decisions |
| Project proposal | Official project proposal *(to be added before submission)* |
| Project report | Official project report *(to be added before submission)* |

---

Copyright © 2026 Quad Core. All rights reserved.
