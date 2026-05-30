# Report

## 1. Initializing the intersections map

I use `S` for the number of street segments and `I` for the number of different intersections.

In our program, the intersections map is initialized after loading the streets. First, `unique_intersections` counts how many different intersection ids there are, because this number is used as the size of the hash table. Then `create_hashmap` allocates the table and initializes every bucket to `NULL`. Finally, `load_streets_into_graph` goes through the street list and inserts each street in the corresponding intersection bucket.

The best case is `O(S + I)`, which simplifies to `O(S)`, because each street is processed a constant number of times and the table is initialized once.

The average and worst cases in the current implementation are `O(S^2)`. The reason is that `unique_intersections` uses a nested search: for each street, it can scan the array of already found intersections to check if the id was already inserted. If many streets have different intersection ids, this becomes quadratic. After that, loading the streets into the hash map is average `O(S)` if the hash function distributes ids well, but the `O(S^2)` part dominates the total cost.

If the unique intersections were counted with another hash set instead of an array search, the average case could be improved to `O(S)`.

## 2. Finding the coordinates of a street or place

I use `H` for the number of houses, `P` for the number of places, and `L` for the length of the name string.

The program stores houses and places in linked lists. To find an address, it scans the houses list until it finds a node with the same street name and house number. To find a place, it scans the places list until it finds a matching place name. This is a sequential search.

For an address, the best case is `O(1)` if the correct house is the first element in the list. The average case is `O(H)` because, on average, the program may need to check around half of the houses. The worst case is `O(H)` if the house is at the end of the list or is not found.

For a place, the best case is also `O(1)` if the place is found at the beginning of the places list. The average case is `O(P)`, and the worst case is `O(P)`.

If the program does not find an exact match, it may also calculate the Levenshtein distance to suggest a similar name. Comparing two strings with Levenshtein distance costs `O(L^2)` if both strings have length around `L`. In that case, the worst case becomes `O(H * L^2)` for houses or `O(P * L^2)` for places. If we treat name length as small and limited, the main cost is still the linear scan through the list.

## 3. Path-finding algorithm

The path-finding algorithm is Breadth First Search (BFS). I use `V` for the number of streets or intersections visited by BFS, and `E` for the number of connections explored.

BFS starts from the origin street and explores the graph level by level using a queue. First it checks all streets that are one step away, then all streets that are two steps away, and so on. It also keeps a `visited` list so that the same street is not processed many times. With BFS, the first route found is the route with the smallest number of street segments, but not necessarily the shortest route in meters.

The best case is `O(1)` if the origin is already the destination or if the destination is found immediately.

If connected streets are found with the intersections map and `visited` checks were constant time, the normal BFS complexity would be `O(V + E)`, because every visited node and every explored connection is processed once.

However, if `visited` is stored as a linked list, checking whether a street was already visited costs `O(V)` in the worst case. Since this check can happen for many explored connections, the current version can cost up to `O(E * V)` in the worst case. In a road map, where the number of connections is usually close to linear in the number of intersections, this often behaves closer to `O(V^2)`.

If connected streets were found by sequentially scanning the whole street list instead of using the intersections map, the algorithm would be even slower, because each BFS step could require scanning all `S` street segments.

## 4. Connected streets: list search vs intersections map by map size

| Map | Street segments | Avg sequential search (ms) | Avg intersections map (ms) |
|---|---:|---:|---:|
| xs_1 | 13 | 0.0001 | 0.0003 |
| xs_2 | 71 | 0.0025 | 0.0001 |
| md_1 | 1318 | 0.0040 | 0.0000 |
| lg_1 | 3797 | 0.0139 | 0.0001 |
| xl_1 | 18944 | 0.0534 | 0.0000 |
| 2xl_1 | 50623 | 0.1479 | 0.0003 |

![Average lookup latency vs map size](report_images/latency_vs_map_size.png)

The graph shows that the average latency of the sequential list search increases as the map size grows. This happens because sequential search has to scan through the street segment list until it finds the correct segment, so larger maps with more street segments require more comparisons.

In contrast, the intersections map search stays almost constant, even when the map becomes much larger. This is because the map approach does not scan all street segments. Instead, it uses the intersection id to directly access the corresponding bucket and retrieve the connected street segments.

Therefore, the intersections map is more scalable than sequential search. As the number of street segments increases, sequential search becomes slower, while the intersections map keeps the lookup time very low.

### Raw Data

The following table contains the raw latency measurements used to calculate the averages shown above. All values are in milliseconds.

<details>
<summary>Click to show raw latency data</summary>

| Test | xs_1 sequential | xs_1 map | xs_2 sequential | xs_2 map | md_1 sequential | md_1 map | lg_1 sequential | lg_1 map | xl_1 sequential | xl_1 map | 2xl_1 sequential | 2xl_1 map |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| Test 1 | 0.001 | 0.000 | 0.001 | 0.000 | 0.004 | 0.000 | 0.021 | 0.000 | 0.052 | 0.000 | 0.245 | 0.002 |
| Test 2 | 0.000 | 0.000 | 0.018 | 0.000 | 0.004 | 0.000 | 0.020 | 0.001 | 0.053 | 0.000 | 0.144 | 0.000 |
| Test 3 | 0.000 | 0.000 | 0.001 | 0.000 | 0.004 | 0.000 | 0.018 | 0.000 | 0.062 | 0.000 | 0.138 | 0.000 |
| Test 4 | 0.000 | 0.001 | 0.001 | 0.000 | 0.004 | 0.000 | 0.012 | 0.000 | 0.050 | 0.000 | 0.136 | 0.001 |
| Test 5 | 0.000 | 0.000 | 0.001 | 0.000 | 0.004 | 0.000 | 0.011 | 0.000 | 0.052 | 0.000 | 0.138 | 0.000 |
| Test 6 | 0.000 | 0.001 | 0.001 | 0.000 | 0.004 | 0.000 | 0.011 | 0.000 | 0.055 | 0.000 | 0.136 | 0.000 |
| Test 7 | 0.000 | 0.001 | 0.001 | 0.000 | 0.004 | 0.000 | 0.012 | 0.000 | 0.053 | 0.000 | 0.134 | 0.000 |
| Test 8 | 0.000 | 0.000 | 0.001 | 0.000 | 0.004 | 0.000 | 0.012 | 0.000 | 0.052 | 0.000 | 0.137 | 0.000 |
| Test 9 | 0.000 | 0.000 | 0.000 | 0.000 | 0.004 | 0.000 | 0.011 | 0.000 | 0.052 | 0.000 | 0.134 | 0.000 |
| Test 10 | 0.000 | 0.000 | 0.000 | 0.001 | 0.004 | 0.000 | 0.011 | 0.000 | 0.053 | 0.000 | 0.137 | 0.000 |

</details>

## 5. Path finding: list search vs intersections map by map size

TODO: Add a plot comparing the latency of finding a path by looking for connected streets sequentially and by using the intersections map, depending on the map size. Keep the same origin and destination for this experiment.

TODO: Add the raw data used for the plot.

TODO: Explain the results.

## 6. Path finding: latency by distance

TODO: Add a plot comparing the latency of finding a path between close points and far points in the same map. Compare the sequential list version and the intersections map version.

TODO: Add the raw data used for the plot.

TODO: Explain the results.

TODO: Fit a curve and justify it using the path-finding complexity from question 3.

## 7. Improving the visited data structure in BFS

TODO: Describe an improvement to the visited data structure in BFS.

TODO: Justify which data structure you would use or have used instead of a list.

TODO: Describe the current runtime complexity and the improved runtime complexity.

TODO: Describe the latency or memory trade-offs.

## 8. Improving closest street search from latitude and longitude

TODO: Describe an improvement to the algorithm that finds the closest street segment from a latitude and longitude.

TODO: Justify which data structure or algorithm you would use or have used.

TODO: Describe the current runtime complexity and the improved runtime complexity.

TODO: Describe the latency or memory trade-offs.
