# Development Notes

Smart City Route & Infrastructure Planner — Team Quad Core

Practical notes for team members: toolchain, build steps, code rules, Git
workflow and the decisions made during development.

---

## 1. Toolchain (verified)

| Tool | Version | Location used by the team lead |
|---|---|---|
| GCC (MinGW-w64, 64-bit) | 15.1.0 | `S:\Qt\Tools\mingw1510_64\bin` |
| CMake | 3.30.5 | `S:\Qt\Tools\CMake_64\bin` |
| Ninja | 1.12.1 | `S:\Qt\Tools\Ninja` |
| C++ standard | C++17 | set in `CMakeLists.txt` |

These tools were installed with the Qt Online Installer, but the CLI version of
the project does **not** use the Qt library. Any MinGW-w64 GCC with C++17
support, CMake 3.21 or newer and Ninja will work.

### If the build or the program is blocked by Windows

On some Windows 11 PCs, a Windows application-control policy can block
newly compiled programs. Symptoms seen during setup:

- Linking failed: `liblto_plugin.dll: error loading plugin`
- Running a compiled program failed: `Permission denied` /
  "An Application Control policy has blocked this file"

These errors come from the PC's security settings, not from the project code.
If you see them, ask the team lead or your lab administrator for help.

## 2. Build, Test and Run

PowerShell, from the project folder:

```powershell
$env:PATH = "S:\Qt\Tools\mingw1510_64\bin;S:\Qt\Tools\CMake_64\bin;S:\Qt\Tools\Ninja;$env:PATH"
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build --output-on-failure
.\build\SmartCityPlanner.exe
```

Replace `S:\Qt\Tools\...` with the location of your own compiler, CMake and Ninja.
To start again from a clean state, delete the `build` folder.

The program and tests use the MinGW runtime DLLs (for example `libstdc++-6.dll`),
so run them from a terminal where the MinGW `bin` folder is on the PATH.

## 3. Code Walkthrough

### Files

| File | What it contains | Prints or reads the keyboard? |
|---|---|---|
| `include/graph.h` | `Edge` and `Road` structs, `Graph` class declaration | No |
| `src/graph.cpp` | The code of the `Graph` functions | No |
| `src/main.cpp` | Input functions, one function per menu option, `main()` | Yes |
| `tests/tests.cpp` | Automatic checks for `Graph` | Prints PASS/FAIL |

**Main rule:** `graph.cpp` (and later the algorithm code) never prints and never
reads the keyboard. It only stores data and returns answers. `main.cpp` talks
to the user. This lets a future Qt GUI reuse the graph code unchanged.

### Data structures (in `graph.h`)

```cpp
struct Edge { int to; double distance; };            // a road, seen from one location
struct Road { int from; int to; double distance; };  // a road with both ends

class Graph {
    map<int, string>       names;      // location ID -> name,  e.g. 1 -> "BUBT"
    map<int, vector<Edge>> adjacency;  // location ID -> list of its roads
    int nextId = 1;                    // ID for the next new location
};
```

- A `map` stores pairs of *key → value* and keeps the keys sorted, so
  locations are always listed by ID.
- `adjacency` is the **adjacency list**: for each location, a `vector` of the
  roads that start there.
- Roads are two-way, so the road BUBT–Mirpur 10 is stored twice: once in
  BUBT's list (`to` = Mirpur 10) and once in Mirpur 10's list (`to` = BUBT).

### Graph functions (in `graph.cpp`)

| Function | What it does |
|---|---|
| `addLocation(name)` | Refuses an empty or already-used name (returns -1). Otherwise gives the location the next ID, stores its name, creates an empty road list, and returns the ID. |
| `hasLocation(id)` | `true` if a location with this ID exists. |
| `nameExists(name)` | `true` if any location already has this name. |
| `getName(id)` | The location's name, or `""` if the ID does not exist. |
| `getLocationIds()` | All IDs in increasing order. |
| `locationCount()` | Number of locations. |
| `addRoad(from, to, distance)` | Refuses the road (returns `false`) if a location is missing, both ends are the same, the distance is not a positive number, or the road already exists. Otherwise adds an `Edge` to **both** locations' lists. |
| `hasRoad(from, to)` | Looks through `from`'s list for an edge going to `to`. |
| `getNeighbors(id)` | The list of roads starting at this location. |
| `getRoads()` | Every road once: each road is stored twice, so only the copy where `from < to` is kept. |
| `roadCount()` | Number of roads. |

Functions marked `const` only *read* the graph; they never change it.

### Menu program (in `main.cpp`)

| Function | What it does |
|---|---|
| `readLine(prompt)` | Prints the prompt and reads one whole line. Closes the program if input ends. |
| `readInt(prompt)` | Reads a line and tries to read a whole number from it. Accepts it only if **nothing else** follows the number (`12` is fine, `12abc` is not). Asks again until valid. |
| `readDouble(prompt)` | Same, for decimal numbers. Also refuses values too large to store (`isfinite`). |
| `addLocation(city)` | Asks for a name, explains any problem, otherwise adds the location. |
| `showLocations(city)` | Prints a table: ID, name, number of roads. |
| `addRoad(city)` | Shows the locations, asks for two IDs and a distance, checks each rule with a clear message, then adds the road. |
| `showRoads(city)` | Prints a table: from, to, distance. |
| `showNetwork(city)` | Prints the adjacency list: each location, then the locations it connects to. |
| `main()` | Creates the `Graph`, shows the menu in a loop, and calls the function for the chosen option until the user chooses 0. |

The menu functions check the user's input and explain any problem; `Graph`
also refuses invalid data as a safety net.

### Tests (in `tests/tests.cpp`)

`check(condition, description)` prints **PASS** if the condition is true and
**FAIL** if not. `testLocations()` and `testRoads()` call `check()` many times.
`main()` returns 1 if anything failed, so `ctest` reports the failure.

## 4. Git Workflow

- `master` — stable branch. Updated **only** through Pull Requests from `dev`.
- `dev` — all development work.
- Run `git pull` before starting work; commit only code that builds and passes
  all tests.
- Never push directly to `master`, never force-push, never rewrite pushed history.
- Pull Requests (`dev` → `master`) are reviewed and merged by the team lead.

## 5. Decision Log

| Date | Decision | Reason |
|---|---|---|
| 2026-11-04 | Use the GCC 15.1.0 / CMake / Ninja toolchain from the Qt installer | Verified working on Windows 11 |
| 2026-11-04 | Develop as a CLI application first; Qt GUI later | Simpler for a beginner team; focus on algorithms |
| 2026-11-04 | Keep the Qt prototype in `future-gui/`, outside the build | Preserved for the later GUI |
| 2026-11-04 | Simplified design with separate `include/` and `src/` folders | Every team member must understand and explain all the code |
| 2026-11-04 | Plain console output (`cout`, `cin`, `setw` tables); no colours, box drawing or Windows-specific console code | Easier to read and explain; works in every terminal |
| 2026-11-04 | A location is an automatic ID plus a name | IDs never need to be typed when adding; no extra fields |
| 2026-11-04 | One weight per road: distance in km | Used by both Dijkstra and Kruskal; easy to explain |
| 2026-11-04 | At most one road between two locations; roads are two-way | Simple undirected graph |
| 2026-11-04 | Names may not be empty, repeated or contain commas | Clear choices in menus; simple CSV files |
| 2026-11-04 | Algorithms in scope: BFS, DFS, Dijkstra, Kruskal | Core graph algorithms of the course; Kruskal handles disconnected networks naturally |
| 2026-11-04 | Excluded for now: budget planning, Floyd–Warshall, Prim, Merge Sort, transport modes, fares | Keep the first version small; may return if the official proposal requires them |
| 2026-11-04 | No static linking | Keep CMake simple; run from a terminal with MinGW on the PATH |

## 6. Milestones

| Milestone | Content | Status |
|---|---|---|
| M1 | Project foundation (CMake, Qt prototype) | Done (`78f875e`) |
| S1 | Add/show locations and roads, city network display, plain console menu, tests | Done |
| — | Edit and delete locations and roads | Planned |
| S2 | Save and load CSV files, sample data | Planned |
| S3 | BFS and DFS | Planned |
| S4 | Dijkstra's shortest route | Planned |
| S5 | Kruskal's minimum spanning tree | Planned |
| S6 | Final tests, documentation, Pull Request to `master` | Planned |

---

Copyright © 2026 Quad Core. All rights reserved.
