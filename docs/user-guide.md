# User Guide

Smart City Route & Infrastructure Planner — Team Quad Core

This guide explains how to build, start and use the program. Sections are
completed as features are added.

---

## 1. Building and Starting the Program

In PowerShell, from the project folder:

```powershell
$env:PATH = "S:\Qt\Tools\mingw1510_64\bin;S:\Qt\Tools\CMake_64\bin;S:\Qt\Tools\Ninja;$env:PATH"
cmake -S . -B build -G Ninja
cmake --build build
.\build\SmartCityPlanner.exe
```

## 2. Main Menu

```
----------------- MAIN MENU ----------------------
  Locations: 4    Roads: 3

  Locations              Roads
  1. Add location        5. Add road
  2. Show locations      6. Show roads
  3. Rename location     7. Update road distance
  4. Delete location     8. Delete road

  Network
  9. Show city network
  10. Traverse network using BFS
  11. Traverse network using DFS
  12. Find shortest route (Dijkstra)
  13. Minimum Spanning Tree (Kruskal)

  0. Exit
--------------------------------------------------
Choose an option:
```

Type the number of an option and press **Enter**. If you type something that is
not a whole number (for example `abc` or `2x`), the program asks again.

Start the program from the **project folder**, so that it finds the `data`
folder. When it starts, it loads the saved city and shows how many locations
and roads were loaded.

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

```
  [1] BUBT
        <-> Mirpur 10 [2]  2.50 km
        <-> Uttara [4]  3.80 km
  [2] Mirpur 10
        <-> BUBT [1]  2.50 km
        <-> Agargaon [3]  4.20 km
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
  automatically** after every successful change; `(Saved)` is printed each time.
- If the files do not exist yet, the program starts with an empty city.
- If a file contains invalid lines, each one is skipped and a note such as
  `data/locations.csv line 3: the name "BUBT" is used twice. Line skipped.` is
  shown. The next save rewrites the files without the skipped lines.
- If `next_id.txt` is missing or invalid, the program uses "highest location
  ID + 1" and shows a note.

To start with the illustrative sample network (10 Dhaka locations, 14 roads),
copy the three sample files before starting the program:

```powershell
Copy-Item data\sample\* data\
```

The sample distances are illustrative values, not measured distances.

## 8. Exploring the Network (BFS and DFS)

- **Traverse network using BFS (10)** and **Traverse network using DFS (11)**
  show the locations in the table, then ask for a **start location ID**.
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

    1. BUBT [1]
    2. Mirpur 10 [2]
    3. Mirpur 1 [3]
    4. Agargaon [4]
    5. Airport [9]
    ...

  Order: BUBT -> Mirpur 10 -> Mirpur 1 -> Agargaon -> Airport -> Mohammadpur -> ...

  Reached 10 of 10 location(s).
```

See [algorithms.md](algorithms.md) for how BFS and DFS work.

## 9. Finding the Shortest Route (Dijkstra)

- **Find shortest route (12)** shows the locations, then asks for the **start
  location ID** and the **destination location ID**.
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
  Shortest Route:
  BUBT [1] -> Mirpur 10 [2] -> Agargaon [4] -> Farmgate [5] -> Gulshan [10]

    BUBT                   -> Mirpur 10                 2.50 km
    Mirpur 10              -> Agargaon                  4.50 km
    Agargaon               -> Farmgate                  3.00 km
    Farmgate               -> Gulshan                   5.00 km

  Total Distance: 15.00 km  (4 road(s))
```

See [algorithms.md](algorithms.md) for how Dijkstra's algorithm works.

## 10. Minimum Spanning Tree (Kruskal)

- **Minimum Spanning Tree (13)** needs no input. It selects existing roads that
  connect all locations with the **smallest total distance**, without any
  cycle.
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

  #   Road                                                  Distance
  --------------------------------------------------------------------
  1   BUBT [1] <-> Mirpur 1 [3]                             2.00 km
  2   Mirpur 10 [2] <-> Mirpur 1 [3]                        2.20 km
  ...
  9   Airport [9] <-> Gulshan [10]                          7.00 km

  Selected roads: 9 of 14
  Total distance: 35.20 km

  The network is connected: these roads connect all 10 location(s) with the smallest total distance.
```

The Minimum Spanning Tree is not the same as the shortest route between two
locations (option 12): it is the cheapest set of roads that keeps the whole
city connected. See [algorithms.md](algorithms.md) for how Kruskal's algorithm
works.

## 11. Troubleshooting

| Problem | Solution |
|---|---|
| "libstdc++-6.dll was not found" | Start the program from a terminal where the MinGW `bin` folder is on the PATH |
| The saved city is not loaded | Start the program from the project folder (the folder that contains `data`) |
| "Warning: the data could not be saved" | Check that the `data` folder and the CSV files are not read-only or open in another program |
| The program closes when input ends | Expected: pressing Ctrl+Z then Enter ends the input and closes the program |

---

Copyright © 2026 Quad Core. All rights reserved.
