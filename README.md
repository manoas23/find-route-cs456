# find_route — Uninformed Search on a Road System (CS 456, Assignment #2, Phase II)

## Problem
Given a text file describing roads between cities (`city1 city2 km`, last line `END OF INPUT`),
find a route between an origin city and a destination city using one of four uninformed search
algorithms: **BFS, DFS, IDT (Iterative Deepening DFS), UCS**. Roads are two-way.

## Build
```
g++ -std=c++11 -O2 -o find_route find_route.cpp
```

## Run
```
./find_route <algorithm> <input_filename> <origin_city> <destination_city>
```
`algorithm` = `BFS` | `DFS` | `IDT` | `UCS`

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
To measure the running time use the shell: `time ./find_route UCS input1.txt Munich Berlin`

## Notes
- Graph-Search: a city is not expanded twice (IDT re-searches in each iteration by design).
- Ties are broken alphabetically (neighbours are sorted by name; UCS ties on equal g(n) by name).
- BFS/DFS/IDT ignore km while searching; the printed distance is the km of the route they found.
