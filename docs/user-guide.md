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

  1. Add location
  2. Show locations
  3. Add road
  4. Show roads
  5. Show city network
  0. Exit
--------------------------------------------------
Choose an option:
```

Type the number of an option and press **Enter**. If you type something that is
not a whole number (for example `abc` or `2x`), the program asks again.

## 3. Adding and Showing Locations

- **Add location:** type a name, for example `BUBT`. The program gives it the
  next ID automatically (1, 2, 3, ...).
- A name cannot be empty, cannot contain a comma, and cannot be used twice.
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
  the distance is not greater than 0, or a road between the two locations
  already exists.
- Numbers must be typed on their own: `12abc` or `4.5km` is not accepted, and
  the program asks again.
- **Show roads:** prints a table of every road with its distance.

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

*To be added in a later milestone.*

## 7. Saving and Loading Data

*To be added in a later milestone.* At the moment, data is lost when the
program closes.

## 8. Exploring the Network (BFS and DFS)

*To be added in a later milestone.*

## 9. Finding the Shortest Route (Dijkstra)

*To be added in a later milestone.*

## 10. Minimum Spanning Tree (Kruskal)

*To be added in a later milestone.*

## 11. Troubleshooting

| Problem | Solution |
|---|---|
| "libstdc++-6.dll was not found" | Start the program from a terminal where the MinGW `bin` folder is on the PATH |
| The program closes when input ends | Expected: pressing Ctrl+Z then Enter ends the input and closes the program |

---

Copyright © 2026 Quad Core. All rights reserved.
