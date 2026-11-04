# Algorithms

Smart City Route & Infrastructure Planner — Team Quad Core

This document explains the data structure and the algorithms used in the
project. Each algorithm section is written when the algorithm is implemented.

---

## 1. Graph Representation — Adjacency List

**Problem:** store the city so that we can quickly find the roads that leave
any location.

**Idea:** the city is a **graph**. Locations are **vertices** and roads are
**edges**. Each road has a **weight**: its distance in km. Roads are two-way,
so the graph is **undirected**.

We use an **adjacency list**: for every location we keep a list of the roads
that start there.

```
1 BUBT       : (to 2, 2.5 km), (to 3, 3.0 km)
2 Mirpur 10  : (to 1, 2.5 km), (to 4, 4.2 km)
3 Mirpur 1   : (to 1, 3.0 km), (to 4, 3.8 km)
4 Agargaon   : (to 2, 4.2 km), (to 3, 3.8 km), (to 5, 2.9 km)
5 Farmgate   : (to 4, 2.9 km)
```

Because roads are two-way, the road BUBT–Mirpur 10 appears in the list of
BUBT **and** in the list of Mirpur 10.

**Data structures (in `graph.h`):**

| Name | Type | Meaning |
|---|---|---|
| `names` | `std::map<int, std::string>` | location ID → name |
| `adjacency` | `std::map<int, std::vector<Edge>>` | location ID → its roads |
| `Edge` | `struct { int to; double distance; }` | one road, seen from one location |
| `nextId` | `int` | the ID given to the next new location |

**Why an adjacency list?** A city has many locations but each location
connects to only a few others. An adjacency list stores only the roads that
exist, using O(V + E) memory. An adjacency matrix would need V × V cells,
most of them empty.

**Main operations** (V = locations, E = roads, deg = roads at one location):

| Operation | How it works | Time |
|---|---|---|
| Add location | Store the name, create an empty road list | O(V) (checks the name is not already used) |
| Add road | Check the rules, add an `Edge` to both lists | O(log V + deg) |
| Check if a road exists | Scan one location's list | O(log V + deg) |
| List all roads | Visit every list, keep each road once (`from < to`) | O(V + E) |
| Rename location | Check the name is not used, then replace it | O(V) |
| Update road distance | Find the road in both lists and change both copies | O(log V + deg) |
| Delete road | Remove the road from both lists | O(log V + deg) |
| Delete location | For each of its roads, remove the copy in the neighbour's list; then erase the location | O(deg × deg′) where deg′ is the largest neighbour degree |
| Save to CSV | Write every location and every road once | O(V + E) |
| Load from CSV | Read each line, check it, add it | O(V² + E·deg) in the worst case, because each line is checked for duplicates |

## 2. Breadth-First Search (BFS)

**Problem:** starting from one location, which locations can be reached, and in
what order are they found if we always look at the closest ones first?

**Idea:** BFS explores the network **level by level**, like ripples in water:
first the start location, then every location **one road** away, then every
location **two roads** away, and so on. It uses a **queue** (first in, first
out): locations found first are visited first.

**Steps:**

```
BFS(start):
    mark start as visited
    put start in the queue
    while the queue is not empty:
        current = take the location at the front of the queue
        add current to the visiting order
        for each neighbour of current (in increasing ID order):
            if the neighbour is not visited:
                mark it as visited
                put it at the back of the queue
    return the visiting order
```

A location is marked as visited **when it is put in the queue**, so it can never
be added to the queue twice, even if the network has cycles.

**Data structures:** `queue<int>` (locations waiting to be visited),
`set<int>` (locations already found), `vector<int>` (the visiting order).

**Complexity:** every location is put in the queue at most once, and every road
is looked at twice (once from each end), so BFS takes **O(V + E)** time and
**O(V)** extra memory. Our implementation also sorts each neighbour list (see
the note below), which adds O(E log V) in the worst case.

**Example** (the test network used in `tests.cpp`):

```
 1 --- 2
 |     |          6 (no roads)        7 --- 8
 3 --- 4 --- 5
```

| Step | Visit | Queue after the step |
|---|---|---|
| 1 | 1 | 2, 3 |
| 2 | 2 | 3, 4 |
| 3 | 3 | 4 |
| 4 | 4 | 5 |
| 5 | 5 | (empty) |

BFS from 1: **1, 2, 3, 4, 5**. Locations 6, 7 and 8 are not reached, because
no road connects them to location 1 (the network is disconnected).

**Where in the code:** `bfs()` in `src/algorithms.cpp`.

## 3. Depth-First Search (DFS)

**Problem:** the same question as BFS (which locations can be reached?), but
exploring by following one path as far as possible.

**Idea:** DFS goes **deep first**: from the current location it takes the first
road to an unvisited location and keeps going. When a location has no more
unvisited neighbours, DFS **backtracks** (goes back) to the previous location
and tries its next road. Our implementation uses **recursion**: each function
call remembers where to go back to.

**Steps:**

```
DFS(start):
    visit(start)
    return the visiting order

visit(current):
    mark current as visited
    add current to the visiting order
    for each neighbour of current (in increasing ID order):
        if the neighbour is not visited:
            visit(neighbour)          <- go deeper
    (when the loop ends, the function returns: this is backtracking)
```

**Data structures:** `set<int>` (visited locations), `vector<int>` (the
visiting order) and the function-call stack used by recursion.

**Complexity:** every location is visited once and every road is looked at
twice, so DFS takes **O(V + E)** time and **O(V)** extra memory (the visited
set and up to V nested function calls). Sorting the neighbour lists adds
O(E log V) in the worst case.

**Example** (same network as above), DFS from 1:

```
1 -> 2 -> 4 -> 3        (3's neighbours 1 and 4 are visited: go back to 4)
          4 -> 5        (all done: go back to 2, then to 1)
```

DFS from 1: **1, 2, 4, 3, 5**. BFS from 1 gave 1, 2, **3**, 4, 5: BFS visits 3
early because it is one road from the start, while DFS first goes deep along
1 → 2 → 4.

**Where in the code:** `dfs()` and `dfsVisit()` in `src/algorithms.cpp`.

### Note: neighbours in increasing ID order

In the adjacency list, roads are stored in the order they were added. After the
city is saved and loaded again, that order can change. To make BFS and DFS
always give the **same result for the same city**, both algorithms visit the
neighbours of a location in increasing ID order (`sortedNeighborIds()`).

### BFS and DFS compared

| | BFS | DFS |
|---|---|---|
| Explores | Level by level (nearest first) | One path as deep as possible, then backtracks |
| Data structure | Queue | Recursion (call stack) |
| Time | O(V + E) | O(V + E) |
| Extra memory | O(V) | O(V) |
| Finds | All locations reachable from the start | All locations reachable from the start |

## 4. Dijkstra's Shortest Path

*To be written in milestone S4.*

## 5. Kruskal's Minimum Spanning Tree

*To be written in milestone S5.*

---

Copyright © 2026 Quad Core. All rights reserved.
