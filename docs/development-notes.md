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
| `include/storage.h`, `src/storage.cpp` | `saveCity()` and `loadCity()`: CSV files | No (returns messages instead) |
| `include/algorithms.h`, `src/algorithms.cpp` | `bfs()`, `dfs()`, `dijkstra()` and `kruskal()` | No |
| `src/main.cpp` | Input functions, one function per menu option, `main()` | Yes |
| `tests/tests.cpp` | Automatic checks for `Graph`, saving/loading, BFS, DFS, Dijkstra and Kruskal | Prints PASS/FAIL |

**Main rule:** `graph.cpp`, `storage.cpp` and `algorithms.cpp` never print and
never read the keyboard. They only store data and return answers.
`main.cpp` talks to the user. This lets a future Qt GUI reuse this code
unchanged.

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
| `addLocation(name)` | Trims the name and refuses it if it is empty or already used, ignoring upper/lower case (returns -1). Otherwise gives the location the next ID, stores the trimmed name, creates an empty road list, and returns the ID. |
| `hasLocation(id)` | `true` if a location with this ID exists. |
| `findLocationId(name)` | The ID of the location with this name, or -1. Names are trimmed and compared ignoring upper/lower case (`trimName()` and a lower-case copy). |
| `nameExists(name)` | `true` if any location already has this name (same rules as `findLocationId`). |
| `trimName(name)`, `isValidDistance(d)` | Shared rules (free functions in `graph.h`): remove spaces at both ends of a name; a distance must be finite, greater than 0 and at most `MAX_ROAD_DISTANCE` (10,000 km). Used by `Graph`, the menu and the CSV loader. |
| `getName(id)` | The location's name, or `""` if the ID does not exist. |
| `getLocationIds()` | All IDs in increasing order. |
| `locationCount()` | Number of locations. |
| `addRoad(from, to, distance)` | Refuses the road (returns `false`) if a location is missing, both ends are the same, the distance is not valid (`isValidDistance`: greater than 0 and at most 10,000 km), or the road already exists. Otherwise adds an `Edge` to **both** locations' lists. |
| `hasRoad(from, to)` | Looks through `from`'s list for an edge going to `to`. |
| `getNeighbors(id)` | The list of roads starting at this location. |
| `getRoads()` | Every road once: each road is stored twice, so only the copy where `from < to` is kept. |
| `roadCount()` | Number of roads. |
| `addLocationWithId(id, name)` | Used only when loading: adds a location with its **saved** ID, so roads still point to the right locations. Moves `nextId` past the highest ID. |
| `getNextId()`, `setNextId(id)` | Read the next ID (for saving) and restore it (when loading). `setNextId` can only **raise** the value, so an ID is never reused. |
| `renameLocation(id, newName)` | Trims the new name and changes it. Refused if the location is missing, the name is empty, or **another** location already has it (ignoring case); a location may change the capitals of its own name. |
| `removeLocation(id)` | Each road is stored in two lists, so it first removes this location from every **neighbour's** list, then erases the location and its own list. |
| `updateRoad(from, to, distance)` | Changes the distance in **both** copies of the road. Refused if the road does not exist or the distance is not valid (`isValidDistance`). |
| `removeRoad(from, to)` | Removes **both** copies of the road. The locations are kept. |

Functions marked `const` only *read* the graph; they never change it.

### Saving and loading (in `storage.cpp`)

| Function | What it does |
|---|---|
| `saveCity(city, locationsFile, roadsFile, nextIdFile)` | Creates the `data` folder if needed, then writes a header line and one line per location (`id,name`) and per road (`from,to,distance`), and the next location ID into `next_id.txt`. Returns `false` if a file cannot be written. |
| `loadCity(city, locationsFile, roadsFile, nextIdFile, messages)` | Loads locations **first** (roads refer to their IDs), then roads, then the next ID. Each line is split at commas and checked; a bad line is skipped and a message with its line number is added to `messages`. A missing file just leaves the city empty. The saved next ID is used only if it is larger than "highest ID + 1"; otherwise a note is added. |
| `splitByComma`, `textToInt`, `textToDouble` | Small helpers used only inside `storage.cpp`. |

### Algorithms (in `algorithms.cpp`)

| Function | What it does |
|---|---|
| `bfs(city, startId)` | Breadth-First Search with a `queue`. Returns the IDs in visiting order, or an empty list if the start does not exist. A location is marked visited when it is **added** to the queue, so it is never added twice. |
| `dfs(city, startId)` | Depth-First Search. Calls `dfsVisit()`, which visits a location and then calls itself for each unvisited neighbour (recursion); returning from the call is the backtracking step. |
| `sortedNeighborIds(city, id)` | The neighbour IDs of a location, sorted. Both algorithms use it, so the visiting order is always the same for the same city. |
| `dijkstra(city, startId, endId)` | Shortest route by total distance. Uses `map<int, double> dist`, `map<int, int> parent` and a min-priority queue of (distance, ID) pairs (`priority_queue` with `greater<>`). Outdated queue entries are skipped; the search stops when the destination leaves the queue. The route is rebuilt by following `parent` back from the destination and reversing. Returns a `RouteResult { found, distance, path }`. |
| `kruskal(city)` | Minimum Spanning Tree (or Forest). Sorts `getRoads()` (each road once) with `isShorterRoad()`, then keeps a road only if `DisjointSet::unite()` joins two different groups. Counts components (V minus one per kept road). Returns a `SpanningResult { roads, totalDistance, components, connected }`. |
| `DisjointSet` | Disjoint Set Union with two `map`s (`parent`, `rank`): `makeSet()`, `findRoot()` with path compression, and `unite()` with union by rank, which returns `false` when both locations are already in the same group (the road would make a cycle). |
| `isShorterRoad(a, b)` | Sorting rule for Kruskal: shorter distance first; equal distances by smaller `from`, then smaller `to` ID. |

See `docs/algorithms.md` for step-by-step explanations and examples.

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
| `renameLocation(city)`, `deleteLocation(city)` | Ask for a location ID (and a new name, or a y/n confirmation), check it, then call the `Graph` function. |
| `updateRoad(city)`, `deleteRoad(city)` | Show the roads, ask for the road's two location IDs (`readExistingRoad`), then call the `Graph` function. |
| `isValidName(city, name, ownId)` | The name rules shared by add and rename (not empty, no comma, not used by another location). |
| `saveChanges(city)` | Called after every successful change; saves the city and prints `(Saved)` or a warning. |
| `traverseNetwork(city, useBfs)` | Options 10 and 11. Asks for a start ID, runs `bfs()` or `dfs()`, prints the visiting order with names and IDs, how many locations were reached, and the locations that are not reachable. |
| `findShortestRoute(city)` | Option 12. Asks for a start and a destination ID, runs `dijkstra()`, and prints the route (names and IDs), each road with its distance (`roadDistance()`), and the total distance, or "No route exists between these locations." |
| `showMinimumSpanningTree(city)` | Option 13. Runs `kruskal()` and prints the selected roads (names, IDs and distances), how many of the city's roads were selected, the total distance, and whether the result is a Minimum Spanning Tree (connected) or a Minimum Spanning Forest (with the number of components). |
| `main()` | Creates the `Graph`, loads the saved city (showing any notes), shows the menu in a loop, and calls the function for the chosen option until the user chooses 0. |

The menu functions check the user's input and explain any problem; `Graph`
also refuses invalid data as a safety net.

### Tests (in `tests/tests.cpp`)

`check(condition, description)` prints **PASS** if the condition is true and
**FAIL** if not. Each `test...()` function calls `check()` many times: adding,
renaming and deleting locations; adding, updating and deleting roads; and
saving and loading (empty city, full city with ID gaps, missing files, and a
file with 16 invalid lines, files without a header, an impossible save folder,
and ID persistence after deleting and restarting); and BFS and DFS (empty city,
unknown start, single location, known visiting orders on a network with a
cycle, disconnected parts, each location visited once, and the same result
after changes and after saving/loading); and Dijkstra (empty city, single
location, same start and destination, unknown IDs, unreachable locations,
direct and indirect routes, a longer direct road versus a shorter route with
more roads, equally short routes, decimal distances, route reconstruction
checked against the real roads, and results after adding, updating and
deleting roads and locations, and after saving/loading); and Kruskal (empty
city, one location, two locations, locations without roads, connected networks
with known results, a triangle of equal short roads where DSU must skip one,
disconnected networks giving a forest with the right number of components,
equal distances, decimal distances, changes to the city, and saving/loading).
Every Kruskal result is also checked with `isSpanningForest()`: real roads,
exactly V − components roads, the correct total, and the same connected parts
as the full city. The save/load tests write temporary files into a `test_data`
folder. `main()` returns 1 if anything failed, so `ctest` reports
the failure.

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
| 2026-11-04 | Names are trimmed and compared ignoring upper/lower case (S6.2) | "BUBT", " BUBT " and "bubt" looked like different locations; the typed capitals are kept for display |
| 2026-11-04 | Road distances are limited to 10,000 km (`MAX_ROAD_DISTANCE`, S6.2) | Larger values are not realistic and could make Dijkstra and Kruskal totals overflow to infinity |
| 2026-11-04 | Algorithms in scope: BFS, DFS, Dijkstra, Kruskal | Core graph algorithms of the course; Kruskal handles disconnected networks naturally |
| 2026-11-04 | Excluded for now: budget planning, Floyd–Warshall, Prim, Merge Sort, transport modes, fares | Keep the first version small; may return if the official proposal requires them |
| 2026-11-04 | No static linking | Keep CMake simple; run from a terminal with MinGW on the PATH |
| 2026-11-04 | CSV code in its own file (`storage.cpp`), not in `Graph` or the menu | `Graph` stays free of input/output; file handling can be explained and tested separately |
| 2026-11-04 | Save automatically after every successful change | No "save" option to forget; data survives if the window is closed |
| 2026-11-04 | Location IDs are saved and kept when loading (`addLocationWithId`) | Roads in `roads.csv` refer to location IDs |
| 2026-11-04 | The next location ID is saved in `data/next_id.txt` | IDs of deleted locations are never reused, even after a restart; a missing or invalid file falls back safely to "highest ID + 1" |
| 2026-11-04 | Invalid CSV lines are skipped with a message, not fatal | One bad line should not lose the rest of the city |
| 2026-11-04 | Working files `data/locations.csv` and `data/roads.csv` are not in Git; sample files are in `data/sample/` | Test changes do not end up in commits; everyone can start from the same sample |
| 2026-11-04 | Deleting a location asks for confirmation (y/n) | It also deletes all its roads |
| 2026-11-04 | BFS and DFS visit neighbours in increasing ID order | Roads are stored in the order they were added, which changes after saving and loading; sorting gives the same, testable result every time |
| 2026-11-04 | DFS uses recursion | Shortest and clearest code for students; the city networks in this project are small |
| 2026-11-04 | Dijkstra uses `priority_queue` with `greater<>` and skips outdated entries | Standard C++ min-priority queue; no extra data structure needed to update priorities |
| 2026-11-04 | Dijkstra replaces a parent only for a strictly shorter distance | With equally short routes, the same route is always chosen (also after saving and loading) |
| 2026-11-04 | Kruskal uses a DSU with `map`s, path compression and union by rank | Location IDs are not 0..V-1 (deleted IDs leave gaps), so maps are simpler than arrays |
| 2026-11-04 | A disconnected network gives a Minimum Spanning Forest, with the number of components | Kruskal only uses existing roads and never invents new ones |
| 2026-11-04 | Kruskal sorts equal distances by location IDs | The same roads are always selected (also after saving and loading) |
| 2026-11-04 | Dijkstra returns a `RouteResult` struct (found, distance, path) | The menu (and a future GUI) can show the result without the algorithm printing anything |

## 6. Milestones

| Milestone | Content | Status |
|---|---|---|
| M1 | Project foundation (CMake, Qt prototype) | Done (`78f875e`) |
| S1 | Add/show locations and roads, city network display, plain console menu, tests | Done (`6f76f77`) |
| S2 | Rename/delete locations, update/delete roads, CSV save/load, sample data | Done (`c3fc761`) |
| S3 | BFS and DFS | Done (`f416196`) |
| S4 | Dijkstra's shortest route | Done (`85f59c7`) |
| S5 | Kruskal's minimum spanning tree | In review |
| S6 | Final tests, documentation, Pull Request to `master` | Planned |

---

Copyright © 2026 Quad Core. All rights reserved.
