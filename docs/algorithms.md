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

**Problem:** what is the shortest route (smallest total distance in km) from a
start location to a destination?

**Important:** Dijkstra finds the route with the smallest **total distance**,
not the route with the fewest roads. A route with three short roads can be
shorter than a route with one long road. (BFS finds the route with the fewest
roads, because it ignores distances.)

**Idea:** Dijkstra keeps, for every location, the shortest distance found so far
from the start. It repeatedly takes the **unfinished location with the
smallest distance**; that distance is now final, because all roads are
positive, so no later detour can make it shorter. From that location it checks
each road: if going through it gives a neighbour a shorter distance, the
neighbour's distance and **parent** (the location before it) are updated. This
check is called **relaxing** a road.

A **min-priority queue** gives the location with the smallest distance quickly.

**Steps:**

```
Dijkstra(start, destination):
    dist[every location] = infinity
    dist[start] = 0
    put (0, start) in the priority queue

    while the queue is not empty:
        (d, current) = take the pair with the smallest distance
        if d > dist[current]: skip it (an outdated entry)
        if current == destination: stop (its distance is final)
        for each road from current to neighbour with length w:
            if dist[current] + w < dist[neighbour]:
                dist[neighbour] = dist[current] + w
                parent[neighbour] = current
                put (dist[neighbour], neighbour) in the queue

    if dist[destination] is still infinity: there is no route
    otherwise: follow parent[] back from the destination to the start,
               then reverse that list to get the route
```

**Data structures:**

| Name | Type | Meaning |
|---|---|---|
| `dist` | `map<int, double>` | shortest distance found so far for each location |
| `parent` | `map<int, int>` | the location before each one on its shortest route |
| `toVisit` | `priority_queue<pair<double, int>, ..., greater<...>>` | (distance, location) pairs; `greater` makes the **smallest** distance come out first |
| `RouteResult` | `struct { bool found; double distance; vector<int> path; }` | the answer returned to the menu |

**Why it needs positive distances:** once a location is taken from the queue,
Dijkstra never changes its distance again. With a negative road, a later detour
could still make it shorter, so the answer could be wrong. Our graph only allows
distances greater than 0 (and at most 10,000 km, so totals always stay ordinary
numbers).

**Complexity:** each road can add one entry to the priority queue, and each
queue operation takes O(log V). Dijkstra therefore takes **O((V + E) log V)**
time and **O(V + E)** extra memory (the maps and the queue). The `map` lookups
also take O(log V) each, which keeps the same total.

**Example** (the test network used in `tests.cpp`):

```
         4
    1 ------- 2           roads: 1-2 4 km, 1-3 1 km, 3-2 2 km,
    |  \      |                  2-4 1 km, 3-4 5 km
    1    2    1
    |      \  |
    3 ------- 4   (3-4 is 5 km)
```

Shortest route from 1 to 4:

| Step | Take from queue | Final distance | Updates |
|---|---|---|---|
| 1 | 1 | 0 | 2 = 4 (via 1), 3 = 1 (via 1) |
| 2 | 3 | 1 | 2 = 3 (via 3, shorter than 4), 4 = 6 (via 3) |
| 3 | 2 | 3 | 4 = 4 (via 2, shorter than 6) |
| 4 | 2 (old entry with distance 4) | — | skipped: 2 already has the shorter distance 3 |
| 5 | 4 | 4 | destination reached: stop |

Step 4 shows why outdated entries are skipped: when location 2 got the shorter
distance 3, its old entry (distance 4) stayed in the queue.

Following the parents back: 4 → 2 → 3 → 1, reversed: **1 → 3 → 2 → 4, total
4 km**. This route uses 3 roads, while the routes 1 → 2 → 4 (5 km) and
1 → 3 → 4 (6 km) use only 2 roads but are longer.

**Special cases:**

- Start and destination are the same: distance 0, route = just the start.
- Unknown start or destination ID, or no road connection between them: no
  route (`found` is `false`).
- Several equally short routes: a parent is only replaced by a **strictly**
  shorter distance, and equal distances leave the queue in increasing ID
  order, so the same route is always chosen (also after saving and loading).

**Where in the code:** `dijkstra()` in `src/algorithms.cpp`; menu option 12
(`findShortestRoute()` in `src/main.cpp`).

## 5. Kruskal's Minimum Spanning Tree

**Problem:** which roads should we keep so that **every location is still
connected**, using the **smallest total road distance**? For example: which
roads should be maintained first if the city can only afford to maintain a
minimum network?

**Spanning tree:** a set of roads that connects all V locations without any
cycle (loop). It always has exactly **V − 1** roads. A **Minimum Spanning Tree
(MST)** is the spanning tree with the smallest total distance.

**Idea:** Kruskal looks at the roads from **shortest to longest** and keeps a
road only if it joins two locations that are **not yet connected** by the roads
already kept. A road between two locations that are already connected would
create a cycle, so it is skipped. Kruskal only chooses from the **existing**
roads; it never creates new ones.

**Why sorting is needed:** taking the shortest useful road first is what makes
the total as small as possible (a greedy choice that is proven to be correct
for this problem). Without sorting, a long road might be kept while a shorter
road that does the same job is skipped.

**Steps:**

```
Kruskal(city):
    put every location in a group of its own (DSU)
    components = number of locations
    sort all roads by distance (shortest first)

    for each road (a, b) in sorted order:
        if a and b are in different groups:
            keep the road and add its distance to the total
            join the two groups
            components = components - 1
        else:
            skip the road (it would make a cycle)

    connected = (components == 1)
```

### Disjoint Set Union (DSU)

To check quickly whether two locations are already connected, Kruskal uses a
**Disjoint Set Union** (also called Union-Find). Every group of connected
locations has one **root** location that represents it.

| Operation | Meaning |
|---|---|
| `makeSet(id)` | put a location in a group of its own (it is its own root) |
| `findRoot(id)` | follow the parent links to the root of the group |
| `unite(a, b)` | join the two groups; returns `false` if `a` and `b` already have the same root |

**How DSU prevents cycles:** two locations have the same root exactly when the
roads already kept connect them. A road between them would close a loop, so
`unite()` returns `false` and Kruskal skips the road.

Two simple improvements keep `findRoot` fast:

- **Path compression:** after finding the root, every location on the way is
  linked directly to the root, so the next search is shorter.
- **Union by rank:** when two groups are joined, the shorter tree is placed
  under the taller tree, so the trees stay flat.

**Data structures:** `vector<Road>` (all roads, from `getRoads()`, which lists
each two-way road only once), `map<int, int> parent` and `map<int, int> rank`
(the DSU), and `SpanningResult { roads, totalDistance, components, connected }`
(the answer returned to the menu).

**Complexity:** sorting the E roads takes **O(E log E)**. Each DSU operation is
almost constant time with path compression and union by rank (the `map` adds a
O(log V) lookup), and putting the V locations into the DSU takes O(V log V).
In total Kruskal takes **O(E log E + V log V)** time, which simplifies to
**O(E log E)** for a connected network (where E ≥ V − 1), and **O(V + E)**
extra memory.

**Example** (the test network used in `tests.cpp`):

```
    1 --(1)-- 2            roads: 1-2 1 km, 2-3 2 km, 1-3 3 km,
    |       / |                   3-4 4 km, 2-4 5 km
   (3)   (2) (5)
    |   /     |
    3 --(4)-- 4
```

| Road (sorted) | Groups before | Decision | Groups after |
|---|---|---|---|
| 1-2 (1 km) | {1} {2} {3} {4} | keep | {1, 2} {3} {4} |
| 2-3 (2 km) | {1, 2} {3} {4} | keep | {1, 2, 3} {4} |
| 1-3 (3 km) | {1, 2, 3} {4} | **skip**: 1 and 3 are already connected | unchanged |
| 3-4 (4 km) | {1, 2, 3} {4} | keep | {1, 2, 3, 4} |
| 2-4 (5 km) | {1, 2, 3, 4} | **skip**: would make a cycle | unchanged |

Result: roads **1-2, 2-3, 3-4**, total **7 km**, 3 roads (= V − 1), connected.

### Disconnected networks: Minimum Spanning Forest

If some parts of the network are not joined by any road, no set of existing
roads can connect all locations, so there is **no** spanning tree. Kruskal then
returns a **Minimum Spanning Forest (MSF)**: one minimum spanning tree for each
part (component). The number of selected roads is **V − components**.

Example: locations 1, 2, 3 joined by roads 1-2 (1), 2-3 (2), 1-3 (3); locations
4 and 5 joined by road 4-5 (4); location 6 has no roads. Kruskal selects 1-2,
2-3 and 4-5 (7 km, 3 roads) and reports **3 components**.

**Special cases:**

- Empty city: no roads, 0 components, not connected.
- One location: no roads are needed; 1 component, connected.
- Roads with equal distances: they are sorted by location IDs, so the same
  roads are always chosen (also after saving and loading).

**MST compared with Dijkstra:** the MST is the cheapest set of roads that keeps
the whole city connected. It does **not** give the shortest route between two
particular locations: on the sample data, Dijkstra's shortest route from BUBT
to Gulshan uses the road BUBT–Mirpur 10, which the MST does not include.

**Where in the code:** `kruskal()`, `DisjointSet` and `isShorterRoad()` in
`src/algorithms.cpp`; menu option 13 (`showMinimumSpanningTree()` in
`src/main.cpp`).

## 6. Floyd–Warshall: All-Pairs Shortest Paths

**Problem:** what is the shortest distance between **every** pair of
locations, and what is the route between any two of them?

**Idea (dynamic programming):** number the locations 0, 1, ..., V − 1. Let
`dist[i][j]` be the shortest distance from i to j when only the first k
locations may be used as stops **in between**. At the start (k = 0) no stop is
allowed, so `dist` only knows the direct roads. Then the locations are allowed
as stops one by one. When location k is allowed, a route from i to j either
does **not** use k (the old value stays) or goes i → k → j:

```
dist[i][j] = min( dist[i][j],  dist[i][k] + dist[k][j] )
```

Both parts `dist[i][k]` and `dist[k][j]` are already the best routes using the
earlier stops, so each step builds on the results of the previous one. This
reuse of smaller answers is what makes it **dynamic programming**.

**Steps:**

```
FloydWarshall(city):
    give every location a row/column number in increasing ID order
    dist[i][j] = INFINITY,  next[i][j] = -1        (no route known)
    dist[i][i] = 0,         next[i][i] = i
    for each road i - j with length w:
        dist[i][j] = w,     next[i][j] = j          (go straight to j)

    for k = 0 to V - 1:                 (allow k as a stop)
        for i = 0 to V - 1:
            if dist[i][k] is INFINITY: skip i
            for j = 0 to V - 1:
                if dist[k][j] is INFINITY: skip j
                if dist[i][k] + dist[k][j] < dist[i][j]:
                    dist[i][j] = dist[i][k] + dist[k][j]
                    next[i][j] = next[i][k]
```

**Data structures:**

| Name | Type | Meaning |
|---|---|---|
| `ids`, `indexOf` | `vector<int>`, `map<int, int>` | row/column number ↔ location ID (IDs can have gaps such as 1, 3, 5, so the ID itself cannot be the row number) |
| `dist` | `vector<vector<double>>` (V × V) | shortest distance; `INFINITY` = no route |
| `next` | `vector<vector<int>>` (V × V) | the first stop after i on a shortest route to j; `-1` = no route |
| `AllPairsResult` | `struct { ids, indexOf, dist, next }` | the answer returned to the menu |

**Unreachable locations:** two locations in different parts of the network
keep `INFINITY`. The algorithm skips a stop k when `dist[i][k]` or `dist[k][j]`
is `INFINITY`, so it never calculates `INFINITY + something` and never invents
a road.

**Route reconstruction:** the `next` matrix stores only the **first stop** of
each shortest route. To get the whole route from i to j, start at i and jump
to `next[current][j]` until j is reached. This works because the rest of a
shortest route is itself a shortest route (from that stop to j). When a
shorter route through k is found, its first stop is the first stop towards k:
`next[i][j] = next[i][k]`.

**Complexity:** three nested loops over V locations: **O(V³)** time. The two
V × V matrices need **O(V²)** memory. Rebuilding one route takes O(V).

**Example** (the Dijkstra test network, locations 1–4):

```
         4
    1 ------- 2           roads: 1-2 4 km, 1-3 1 km, 3-2 2 km,
    |  \      |                  2-4 1 km, 3-4 5 km
    1    2    1
    |      \  |
    3 ------- 4   (3-4 is 5 km)
```

| Step | Stop allowed | Changes |
|---|---|---|
| start | none | direct roads only; 1–4 is ∞ |
| k = 1 | location 1 | no change |
| k = 2 | location 2 | 1–4 = 4 + 1 = **5** (via 2); 3–4 = 2 + 1 = **3** (via 2, shorter than 5) |
| k = 3 | location 3 | 1–2 = 1 + 2 = **3** (via 3, shorter than 4); 1–4 = 1 + 3 = **4** (via 3, shorter than 5) |
| k = 4 | location 4 | no change |

Final distance matrix (km):

```
        1    2    3    4
   1    0    3    1    4
   2    3    0    2    1
   3    1    2    0    3
   4    4    1    3    0
```

Route 1 → 4: `next[1][4] = 3`, `next[3][4] = 2`, `next[2][4] = 4`, so the route
is **1 → 3 → 2 → 4, 4 km**: the same answer as Dijkstra.

**Floyd–Warshall compared with Dijkstra:**

| | Dijkstra | Floyd–Warshall |
|---|---|---|
| Answers | one start → destination | every pair at once |
| Method | greedy (closest unfinished location first) | dynamic programming (allow one more stop each step) |
| Time | O((V + E) log V) for one start | O(V³) for all pairs |
| Memory | O(V + E) | O(V²) |
| Best when | a few route questions, large network | many route questions, small network |

Running Dijkstra once from every location also gives all pairs, in
O(V (V + E) log V). For the small city networks of this project both are fast;
the tests check that both always give the **same distances**. With several
equally short routes they can choose different (equally short) routes:
Floyd–Warshall keeps the first route found (stops are tried in increasing ID
order and only a strictly shorter route replaces it), so its result is also
always the same.

**Where in the code:** `floydWarshall()` and `allPairsRoute()` in
`src/algorithms.cpp`; menu option 14 (`showAllPairsShortestPaths()` in
`src/main.cpp`).

## 7. Best Facility Location: Eccentricity, Centre and Diameter

**Problem:** where should a hospital, fire station or emergency centre be
built, so that even the **farthest** location can be reached quickly?

**Definitions** (d(u, v) = shortest distance, from the Floyd–Warshall table):

| Term | Meaning | Formula |
|---|---|---|
| Eccentricity e(v) | distance from v to the location farthest away | e(v) = max d(v, u) over all locations u |
| Radius | the smallest eccentricity | min e(v) |
| Centre | the location(s) whose eccentricity equals the radius | argmin e(v) |
| Diameter | the largest distance between two locations | max d(u, v) = max e(v) |

A facility at the **centre** keeps the farthest location as close as possible
(the "minimax" choice, suitable for emergency services).

**Idea:** Floyd–Warshall (section 6) already knows every shortest distance, so
no shortest path has to be calculated again: each eccentricity is simply the
largest value in that location's row of the distance matrix.

**Steps:**

```
BestFacilityLocation(table):
    find the connected parts: two locations are in the same part
        exactly when their distance is finite (not INFINITY)
    for each location v:
        e(v) = largest FINITE distance in row v
    for each part:
        radius   = smallest e(v) in the part
        centres  = every v with e(v) within 1e-9 km of the radius
        diameter = largest e(v) in the part
        diameter pairs = every pair (a, b) with a < b and
                         d(a, b) within 1e-9 km of the diameter
```

**Disconnected networks:** if some locations cannot reach each other, their
distance is ∞, so **every** eccentricity of the whole network is ∞: there is
no single centre and the diameter is infinite. The program says so and then
analyses **each connected part on its own**, using only the distances inside
that part: each part needs its own facility. A location without roads is a
part on its own (eccentricity 0); it is reported as **isolated**, never as the
best location. An empty city has no parts; a city with one location has that
location as its centre (eccentricity 0, diameter 0).

**Equal values (ties):** distances are decimal numbers, and the computer
cannot store every decimal exactly (0.1 + 0.2 is not exactly 0.3). Two values
that differ by less than **1e-9 km** (`DISTANCE_TOLERANCE`) are therefore
treated as equal. All tied centres are shown, in increasing ID order. Each
diameter pair is listed once, with the smaller ID first, and the pairs are
sorted. The parts are ordered by their smallest location ID.

**Data structures:** `NetworkPart { locations, centres, radius, diameter,
diameterPairs }` for each part, and `FacilityResult { eccentricity (map ID →
km), parts, connected }`.

**Complexity:** finding the parts, the eccentricities and the diameter pairs
each look at every cell of the V × V matrix once: **O(V²)** time, plus the
O(V³) Floyd–Warshall run that produces the matrix (done once). Extra memory:
**O(V)**, plus the list of diameter pairs (at most V(V − 1)/2 when every
distance is equal).

**Example** (the network of section 6, final distance matrix):

| Location | Distances to 1, 2, 3, 4 | e(v) |
|---|---|---|
| 1 | 0, 3, 1, 4 | 4 |
| 2 | 3, 0, 2, 1 | **3** |
| 3 | 1, 2, 0, 3 | **3** |
| 4 | 4, 1, 3, 0 | 4 |

Radius 3 km: the centres are locations **2 and 3** (a tie). Diameter 4 km:
the only pair that far apart is **(1, 4)**.

On the sample data the centre is **Mirpur 10 [2]** (its farthest location is
14 km away) and the diameter is 21.7 km, between Mohammadpur [7] and
Uttara [8].

**Limitation:** the centre is always one of the existing locations. The
mathematically best point could lie part-way along a road, and minimising the
**total** distance to all locations (the "median", useful for a warehouse) is
a different question that is not part of this project.

**Where in the code:** `bestFacilityLocation()` in `src/algorithms.cpp`; menu
option 15 (`showBestFacilityLocation()` in `src/main.cpp`).

## 8. Complexity Summary

| Algorithm | Time | Extra memory | Data structures |
|---|---|---|---|
| BFS | O(V + E) | O(V) | queue, set |
| DFS | O(V + E) | O(V) | recursion, set |
| Dijkstra | O((V + E) log V) | O(V + E) | min-priority queue, maps |
| Kruskal | O(E log E + V log V); O(E log E) if connected | O(V + E) | sorted vector, DSU |
| Floyd–Warshall | O(V³) | O(V²) | distance matrix, next-hop matrix |
| Best facility location | O(V²) after Floyd–Warshall | O(V) + diameter pairs | distance matrix, parts, map of eccentricities |

(BFS and DFS also sort each neighbour list for a stable order, which adds
O(E log V) in the worst case.)

---

Copyright © 2026 Quad Core. All rights reserved.
