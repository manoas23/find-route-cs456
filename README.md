# find_route — Uninformed Search on a Road System (CS 456, Assignment #2, Phase II)

## Problem
Given a text file describing roads between cities (`city1 city2 km`, last line `END OF INPUT`),
find a route between an origin city and a destination city using one of four uninformed search
algorithms: **BFS, DFS, IDT (Iterative Deepening DFS), UCS**. Roads are two-way.
The program prints the length of the route and the list of cities on it.

## Files
| File | Purpose |
|---|---|
| `find_route.cpp` | the program (C++) |
| `input1.txt` | the road system given with the assignment (21 cities, 28 roads) |
| `verify_report.py` | re-runs the experiments and prints the tables of the report (distance, expanded, generated, memory) |
| `bench.py` | measures the running time (Table 5 of the report) |

## Design (object-oriented)
Each class keeps its data **private** (encapsulation); values are given when the object is constructed
and are read through getters.
- `State` – one search node: city, cost g(n), parent index. `State(city, cost, parent)`.
- `RoadMap` – the road system. `RoadMap(fileName, destinationCity)` reads the file, sorts the cities
  alphabetically and sets the goal. Provides `getCityName`, `getRoadLength`, `isGoal`, `move`, ...
- `Algorithms` – the four searches (`BFS`, `DFS`, `IDT`, `UCS`) and their counters.
  `Algorithms(roadMap)`; counters are read with `getExpandedCount()`, `getGeneratedCount()`, `getMaxFrontierSize()`.

## Authors and work split
Group of two students; each student prepared two of the four algorithms:
| Student | Algorithms |
|---|---|
| منتهى علي سحبون | BFS, DFS |
| صابرين عياد خليفة | IDT, UCS |

## Build
Command line (g++):
```
g++ -std=c++11 -O2 -o find_route find_route.cpp
```
Visual Studio developer command prompt:
```
cl /EHsc /O2 find_route.cpp
```

## Run
```
./find_route <algorithm> <input_filename> <origin_city> <destination_city>
```
(on Windows: `.\find_route.exe ...`). `algorithm` = `BFS` | `DFS` | `IDT` | `UCS`.
If the algorithm is omitted (3 arguments) UCS is used.

Examples:
```
./find_route UCS input1.txt Bremen Frankfurt
./find_route BFS input1.txt Munich Berlin
./find_route UCS input1.txt London Frankfurt     # -> distance: infinity / Route: none
```

## Output
```
distance: 455 km
route:
Bremen to Dortmund, 234 km
Dortmund to Frankfurt, 221 km
```
If no route exists: `distance: infinity` and `Route: none`.

Numbers for the report (expansion order, nodes expanded, nodes generated, max nodes in memory)
are printed to **stderr**, so stdout keeps the required format.
To save them: `./find_route UCS input1.txt Munich Berlin 2> stats.txt`
(to hide them: `2>/dev/null` on Linux/macOS, `2>$null` in PowerShell).

Definitions: *max nodes in memory* = largest frontier for BFS/UCS, largest recursion depth for DFS,
largest current path for IDT.

## Measuring running time
The search itself takes only microseconds on this small map, so the total time of one run
(about 2 ms) is dominated by starting the process. The times in the report (Table 5) are the
**average of 500 runs per case, in milliseconds, for the whole program**, measured with `bench.py`:
```
python bench.py          # prints the time table (ms) and the average over the 6 pairs that have a route
```

## Reproducing the report tables
```
python verify_report.py  # prints Tables 1, 2, 3, 4 and the distance / nodes / memory rows of Table 6 (the 7 city pairs of the report)
```
(compile `find_route` first and run the scripts from the same folder as `input1.txt`;
use `python3` if `python` is not found).

## Notes
- Graph-Search: a city is not expanded twice (IDT re-searches in each iteration by design and only
  avoids cycles on the path being explored).
- Ties are broken alphabetically (neighbours are visited in alphabetical order; UCS ties on equal g(n) by name).
- BFS/DFS/IDT ignore km while searching (every step costs 1); the printed distance is the km of the route they found.
- Limits: at most 50 cities and 1000 roads; larger files give an error message.
