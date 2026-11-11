# User Guide

Smart City Route & Infrastructure Planner — Team Quad Core

This guide explains how to build, start and use the program.

---

## 1. Building and Starting the Program

You need MinGW-w64 GCC (with C++17 support) and CMake on the Windows PATH.
Qt is not needed. The commands below are for the Windows **Command Prompt
(CMD)**.

**1. Go to the project folder.** Replace the path with the folder where the
project is on your PC (`/d` also changes the drive):

```cmd
cd /d "P:\BUBT\1-4\CSE 232\smart-city-route-planner"
```

**2. Configure (only the first time) and build:**

```cmd
cmake -S . -B build-mingw -G "MinGW Makefiles"
cmake --build build-mingw
```

The program is created as `build-mingw\SmartCityPlanner.exe`. After changing
the code, run only `cmake --build build-mingw` again.

**3. Copy the sample data (optional, first run only).** The program does this
itself on the first run (see section 7), so this step is not required. Each
file is copied only if it does not exist yet, so your saved data is never
overwritten:

```cmd
if not exist data mkdir data
if not exist data\locations.csv copy data\sample\locations.csv data\locations.csv
if not exist data\roads.csv copy data\sample\roads.csv data\roads.csv
if not exist data\next_id.txt copy data\sample\next_id.txt data\next_id.txt
```

**4. Start the program** from the project folder (the folder that contains
`data`). If you start it from somewhere else (for example inside
`build-mingw`, or by double-clicking the `.exe`), it uses the project's `data`
folder next to the program and shows which folder it uses:

```cmd
build-mingw\SmartCityPlanner.exe
```

**5. Run the automated tests (optional):**

```cmd
ctest --test-dir build-mingw --output-on-failure
```

A successful run ends with `100% tests passed out of 1`.

To build with PowerShell or with the optional Ninja generator, see the README.

## 2. Main Menu

The main menu is a dashboard: a title box, the number of locations and roads,
the data folder, and the options grouped by category. In plain-text mode it
looks like this (in colour mode the same layout uses colours and box lines):

```
  +====================================================================+
  |                                                                    |
  |             SMART CITY ROUTE & INFRASTRUCTURE PLANNER              |
  |                      Algorithms Lab - CSE 232                      |
  |                           Team Quad Core                           |
  |                                                                    |
  +====================================================================+

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
  ----------------------------------------------------------------------
  Graph: adjacency list  |  BFS, DFS, Dijkstra, Kruskal + DSU  |  C++17

  > Choose an option:
```

Type the number of an option and press **Enter**. If you type something that is
not a whole number (for example `abc` or `2x`), the program asks again. A number
that is not on the menu is shown as an error under the menu.

Each option opens its own screen with a title line (for example
`SMART CITY PLANNER > ALGORITHMS > BFS Traversal`). When the screen is
finished, press **Enter** to return to the main menu.

**Messages** start with a coloured tag: `[ OK ]` (green) for success,
`[ERROR]` (red) for invalid input, `[ WARN ]` (yellow) for warnings such as a
disconnected network, and `[ INFO ]` (cyan) for information. Deleting a
location shows a red `[DELETE]` warning before asking for confirmation.

### Colours and plain-text mode

- In Windows Terminal or the Windows console (Windows 10/11), the program uses
  a **dark navy theme**: white titles, cyan and blue accents, purple numbers,
  green results, red errors and yellow warnings.
- If the terminal cannot show colours, or the output is redirected to a file,
  the program automatically uses **plain text** with ASCII borders and does not
  clear the screen or wait for Enter.
- To choose plain text yourself, start the program with `--plain`
  (`build-mingw\SmartCityPlanner.exe --plain`) or set the `NO_COLOR`
  environment variable.
- When you exit, the terminal returns to its normal colours.

## 3. Adding and Showing Locations

- **Add location:** type a name, for example `BUBT`. The program gives it the
  next ID automatically (1, 2, 3, ...).
- A name cannot be empty, cannot contain a comma, and cannot be used twice.
  Spaces at the start and end are removed, and upper/lower case is ignored
  when checking for duplicates: `BUBT`, ` BUBT ` and `bubt` count as the same
  name. The name is shown with the capital letters you typed.
- **Show locations:** prints a table with each location's ID, name and number
  of roads.

```
  ID    Name                     Roads
  ------------------------------------
  1     BUBT                     2
  2     Mirpur 10                2
```

## 4. Adding and Showing Roads

- **Add road:** the program shows the locations, then asks for the *From* ID,
  the *To* ID and the distance in km.
- Roads are two-way: a road from BUBT to Mirpur 10 can be used in both
  directions.
- A road is refused if a location ID does not exist, both IDs are the same,
  the distance is not greater than 0 or is more than 10,000 km, or a road
  between the two locations already exists.
- Numbers must be typed on their own: `12abc` or `4.5km` is not accepted, and
  the program asks again.
- **Show roads:** prints a table of every road with the IDs and names of both
  locations and the distance.

## 5. Showing the City Network

Shows the network as an **adjacency list**: each location followed by the
locations it is directly connected to, with the road distances.

Example (sample data, plain-text mode; colour mode uses tree lines):

```
  [1] BUBT  (2 road(s))
      |- Mirpur 10 [2]                     2.50 km
      `- Mirpur 1 [3]                      2.00 km
  [2] Mirpur 10  (4 road(s))
      |- BUBT [1]                          2.50 km
      |- Mirpur 1 [3]                      2.20 km
      |- Agargaon [4]                      4.50 km
      `- Airport [9]                       8.50 km
```

## 6. Editing and Deleting

- **Rename location (3):** choose a location ID and type the new name. The
  same rules apply as when adding (not empty, no comma, not used by another
  location). The location keeps its ID and its roads.
- **Delete location (4):** choose a location ID. The program shows how many
  roads will be deleted with it and asks `Are you sure? (y/n)`. Only `y`
  deletes it. All roads connected to the location are deleted as well.
- **Update road distance (7):** the program shows the roads, then asks for the
  two location IDs of the road (in either order) and the new distance, which
  must be greater than 0 and at most 10,000 km.
- **Delete road (8):** the program shows the roads and asks for the two
  location IDs of the road to delete. The locations themselves are kept.
- IDs of deleted locations are never given to new locations, even after the
  program is restarted.

## 7. Saving and Loading Data

- The city is stored in `data/locations.csv`, `data/roads.csv` and
  `data/next_id.txt` (the ID for the next new location).
- It is **loaded automatically** when the program starts and **saved
  automatically** after every successful change; `Saved automatically to the
  data folder.` is printed each time.
- **First run:** if none of the three files exists yet, the program copies the
  sample city (10 locations, 14 roads) from `data/sample/` and shows
  `First run: no saved city was found, so the sample city was loaded` on the
  start screen. If any of the files exists, nothing is copied, so your saved
  city is never replaced. (If the sample files are missing too, the program
  starts with an empty city.)
- The program uses the `data` folder in the folder it is started in, or, if
  there is none, the project's `data` folder next to the program.
- If a file contains invalid lines, each one is skipped and a note such as
  `data/locations.csv line 3: the name "BUBT" is used twice. Line skipped.` is
  shown. The next save rewrites the files without the skipped lines.
- If `next_id.txt` is missing or invalid, the program uses "highest location
  ID + 1" and shows a note.

To start again with the illustrative sample network (10 Dhaka locations, 14
roads) later, copy the three sample files before starting the program. In
PowerShell, this command **replaces** any working files already in `data`:

```powershell
Copy-Item data\sample\* data\
```

The sample distances are illustrative values, not measured distances.

## 8. Exploring the Network (BFS and DFS)

- **BFS Traversal (10)** and **DFS Traversal (11)** show the locations in the
  table, then ask for a **start location ID**.
- The program prints the locations in the order the algorithm visits them
  (with names and IDs), the same order on one line, and how many locations
  were reached.
- **BFS** visits the nearest locations first (level by level). **DFS** follows
  one path as far as possible before going back.
- Neighbours are always visited in increasing ID order, so the same city always
  gives the same result.
- If some locations cannot be reached from the start (the network is in
  separate parts), they are listed as **Not reachable**.
- An ID that does not exist gives an error message. If the city is empty, the
  program says so.

Example (sample data, BFS from BUBT):

```
  BFS (Breadth-First Search) from BUBT [1]
  --------------------------------------------
     1. BUBT [1]
     2. Mirpur 10 [2]
     3. Mirpur 1 [3]
     4. Agargaon [4]
     5. Airport [9]
     ...

  Order
  ---------
  BUBT -> Mirpur 10 -> Mirpur 1 -> Agargaon -> Airport -> Mohammadpur ->
  Farmgate -> Uttara -> Gulshan -> Dhanmondi

  [ OK ] Reached 10 of 10 location(s).
```

If some locations cannot be reached, the result line is a yellow `[ WARN ]`
and the unreachable locations are listed below it.

See [algorithms.md](algorithms.md) for how BFS and DFS work.

## 9. Finding the Shortest Route (Dijkstra)

- **Dijkstra's Shortest Route (12)** shows the locations, then asks for the
  **start location ID** and the **destination location ID**.
- The program prints the shortest route with names and IDs, each road on the
  route with its distance, and the **total distance** in km.
- "Shortest" means the smallest **total distance**, not the fewest roads. A
  route through several short roads can be shorter than one long direct road.
- If the start and destination are the same, the route is just that location
  and the distance is 0 km.
- If the two locations are in different parts of the network, the program
  prints `No route exists between these locations.`
- An ID that does not exist gives an error message.

Example (sample data, from BUBT to Gulshan):

```
  Shortest Route
  ------------------
  BUBT [1] -> Mirpur 10 [2] -> Agargaon [4] -> Farmgate [5] ->
  Gulshan [10]

  Road by Road
  ----------------
    1. BUBT                  -> Mirpur 10             2.50 km
    2. Mirpur 10             -> Agargaon              4.50 km
    3. Agargaon              -> Farmgate              3.00 km
    4. Farmgate              -> Gulshan               5.00 km

  ----------------------------------------------------------------------
  Total Distance: 15.00 km  (4 road(s))
  ----------------------------------------------------------------------
```

See [algorithms.md](algorithms.md) for how Dijkstra's algorithm works.

## 10. Minimum Spanning Tree (Kruskal)

- **Kruskal's Minimum Spanning Tree (13)** needs no input. It selects existing
  roads that connect all locations with the **smallest total distance**,
  without any cycle.
- The program prints each selected road (both locations with names and IDs)
  and its distance, how many of the city's roads were selected, and the total
  distance.
- If the whole network is connected, the result is a **Minimum Spanning Tree**.
- If the network is in separate parts, no existing roads can join them, so the
  result is a **Minimum Spanning Forest**: one minimum tree for each part. The
  program shows how many parts (components) there are.
- Roads are never created: Kruskal only chooses from the roads that exist.
- If the city is empty, the program says so.

Example (sample data):

```
  Minimum Spanning Tree (Kruskal)
  -----------------------------------
  #    Road                                                Distance
  ----------------------------------------------------------------------
  1    BUBT [1] <-> Mirpur 1 [3]                           2.00 km
  2    Mirpur 10 [2] <-> Mirpur 1 [3]                      2.20 km
  ...
  9    Airport [9] <-> Gulshan [10]                        7.00 km
  ----------------------------------------------------------------------

  Selected roads: 9 of 14
  Total distance: 35.20 km

  [ OK ] The network is connected.
  These roads connect all 10 location(s) with the smallest total distance.
```

For a disconnected network the title is **Minimum Spanning Forest** and the
result is a yellow `[ WARN ]` with the number of components.

The Minimum Spanning Tree is not the same as the shortest route between two
locations (option 12): it is the cheapest set of roads that keeps the whole
city connected. See [algorithms.md](algorithms.md) for how Kruskal's algorithm
works.

## 11. All-Pairs Shortest Paths (Floyd–Warshall)

- **All-Pairs Shortest Paths (14)** calculates the shortest distance between
  **every** pair of locations at once (Floyd–Warshall algorithm).
- It shows the locations, then a **distance matrix**: one row and one column
  per location ID. `0` means the same location and `--` means there is no
  route (the locations are in different parts of the network).
- If the city has too many locations for the table to fit on the screen, the
  program says so and skips the table.
- Then it asks for a **start location ID** and a **destination location ID**
  and shows the shortest route, each road with its distance, and the total
  distance, in the same way as option 12. The distances are always the same
  as Dijkstra's.
- If the network is in separate parts, a yellow `[ WARN ]` says so, and a
  route between the parts is reported as `No route exists between these
  locations.` An ID that does not exist gives an error message.

Example (sample data, first rows of the matrix):

```
  Distance Matrix (km)
  ------------------------
           [1]   [2]   [3]   [4]   [5]   [6]   [7]   [8]   [9]  [10]
  ----------------------------------------------------------------------
  [1]        0  2.50  2.00  7.00 10.00 10.30  7.50 16.50 11.00 15.00
  [2]     2.50     0  2.20  4.50  7.50 10.50  7.70 14.00  8.50 12.50
  ...
```

Row `[1]` (BUBT) shows that Gulshan `[10]` is 15.00 km away, the same as the
Dijkstra example in section 9. See [algorithms.md](algorithms.md) for how
Floyd–Warshall works.

## 12. Best Facility Location

- **Best Facility Location (15)** needs no input. It shows where a hospital,
  fire station or emergency centre should be built: the location whose
  **farthest** location is as close as possible (the network **centre**).
- The table lists, for every location, the distance to its farthest location
  (its **eccentricity**, using the Floyd–Warshall distances). The centre is
  marked `<- centre` (a green `◄ centre` in colour mode).
- Below the table: the **best location(s)**, the distance to the farthest
  location from there (the **radius**), and the **diameter** (the two
  locations farthest apart). Equally good locations and equally long pairs
  are all shown (a long list ends with "and N more"). Each pair is listed
  once, with the smaller ID first.
- If the network is in separate parts, a yellow `[ WARN ]` says that there is
  no single centre and the diameter is infinite. The table gets a **Part**
  column, and the centre and diameter are shown **for each part**, because
  each part needs its own facility. Locations without roads are listed as
  **isolated**; they are never shown as the best location.
- If the city is empty, the program says so. With one location, that location
  is the centre.

Example (sample data):

```
  ID      Location                      Farthest (km)
  ----------------------------------------------------------------------
  [1]     BUBT                                  16.50
  [2]     Mirpur 10                             14.00   <- centre
  ...
  [8]     Uttara                                21.70
  ...
  Network Centre and Diameter
  -------------------------------
  Best location: Mirpur 10 [2]
  Farthest location from it: 14.00 km  (radius)
  Diameter: 21.70 km  (the two locations farthest apart)
    - Mohammadpur [7] <-> Uttara [8]
```

See [algorithms.md](algorithms.md) for the definitions and how the centre is
calculated.

## 13. Troubleshooting

| Problem | Solution |
|---|---|
| "'g++' (or 'cmake') is not recognized as an internal or external command" | Add the MinGW `bin` folder (and CMake) to the Windows PATH, then open a new terminal |
| "libstdc++-6.dll was not found" | Start the program from a terminal where the MinGW `bin` folder is on the PATH |
| The saved city is not loaded | Start the program from the project folder (the folder that contains `data`); the start screen and the dashboard show which data folder is used |
| "The data could not be saved to the data folder" | Check that the `data` folder and the CSV files are not read-only or open in another program |
| Strange characters such as `←[38;2;...m` appear | The terminal does not support colours: use `--plain` or set `NO_COLOR` |
| The program closes when input ends | Expected: pressing Ctrl+Z then Enter ends the input and closes the program |

---

Copyright © 2026 Quad Core. All rights reserved.
