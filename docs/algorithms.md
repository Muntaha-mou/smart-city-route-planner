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

## 2. Breadth-First Search (BFS)

*To be written in milestone S3.*

## 3. Depth-First Search (DFS)

*To be written in milestone S3.*

## 4. Dijkstra's Shortest Path

*To be written in milestone S4.*

## 5. Kruskal's Minimum Spanning Tree

*To be written in milestone S5.*

---

Copyright © 2026 Quad Core. All rights reserved.
