# Report

## 1. Initializing the intersections map

I use `S` for the number of street segments and `I` for the number of different intersections.

In our program, the intersections map is initialized after loading the streets. First, `unique_intersections` counts how many different intersection ids appear in the street list. This value is used to choose the size of the hash table. Then `create_hashmap` allocates the table and initializes every bucket to `NULL`. Finally, `load_streets_into_graph` goes through the street list and inserts each street into the hash table using its intersection id.

The best case is `O(S + I)`, which simplifies to `O(S)` because the number of intersections is at most proportional to the number of street segments. This happens when checking whether an intersection was already seen stays cheap, for example when many streets share the same intersections.

The average and worst cases in the current implementation are `O(S^2)`. The reason is that `unique_intersections` stores the ids it has already found in an array. For each street, it may scan that array to check if the `from` or `to` intersection id was already inserted. If many streets have different intersection ids, this nested search becomes quadratic.

After the table is created, loading the streets into the hash map is `O(S)` on average if the hash function distributes ids well. However, the `O(S^2)` cost of counting unique intersections dominates the total complexity. If the unique intersections were counted with a hash set instead of an array search, the average initialization cost could be improved to `O(S)`.

## 2. Finding the coordinates of a street or place

I use `H` for the number of houses, `P` for the number of places, and `L` for the length of the name string.

The program stores houses and places in linked lists. To find an address, it scans the houses list until it finds a node with the same street name and house number. To find a place, it scans the places list until it finds a matching place name. Both operations are sequential searches.

For an address, the best case is `O(1)` if the correct house is the first element in the list. The average case is `O(H)` because the program may need to check many houses before finding the correct one. The worst case is `O(H)` if the house is at the end of the list or is not found.

For a place, the best case is also `O(1)` if the place is found at the beginning of the places list. The average case is `O(P)`, and the worst case is `O(P)`. If there are multiple places with the same name, the program may scan the places list again to print and select the correct option, but this is still linear.

If the program does not find an exact match, it calculates Levenshtein distance to suggest similar street or place names. Comparing two strings with Levenshtein distance costs `O(L^2)` if both strings have length around `L`. In that case, the worst case becomes `O(H * L^2)` for houses or `O(P * L^2)` for places. If we treat name length as small and limited, the main cost is still the linear scan through the list.

## 3. Path-finding algorithm

The path-finding algorithm used in the final program is Dijkstra's algorithm. I use `V` for the number of visited street segments, and `E` for the number of street connections explored.

Dijkstra starts from the origin street and always explores the currently shortest known path first. In our implementation, the weight of a street connection is the street segment length in meters. This means the path found is based on distance, not only on the number of street segments.

The algorithm keeps a `visited` hash table so that the same street segment is not processed many times. It also keeps a pending list of paths to explore, ordered by total distance. When a new path is created, it is inserted into this list in the correct position.

The best case is `O(1)` if the origin is already the destination or if the destination is found immediately.

Finding connected streets with the intersections map is `O(1)` on average for each lookup, and checking `visited` is also `O(1)` on average. However, our pending paths are stored in a sorted linked list, not in a binary heap. Inserting a new path can take `O(E)` in the worst case because the program may need to scan through the pending list to find the correct position.

Because of this sorted linked list, the average and worst case complexity of our Dijkstra implementation can be up to `O(E^2)`. In a road map, where the number of connections is usually close to the number of visited street segments, this often behaves closer to `O(V^2)`. If the pending paths were stored in a binary heap priority queue, the complexity could be improved to `O((V + E) log V)`.

## 4. Connected streets: list search vs intersections map by map size

This experiment measures the time needed to find the outgoing street segments connected to one intersection. The sequential version scans the street segment list. The intersections map version uses the intersection id to directly access the streets stored for that intersection.

All values are in milliseconds. A value of `0.000` means the measured time was below the precision recorded in milliseconds, not that the operation did no work.

| Map | Street segments | Avg sequential search (ms) | Avg intersections map (ms) |
|---|---:|---:|---:|
| xs_1 | 13 | 0.0000 | 0.0002 |
| xs_2 | 71 | 0.0006 | 0.0002 |
| md_1 | 1318 | 0.0187 | 0.0000 |
| lg_1 | 3797 | 0.0203 | 0.0000 |
| xl_1 | 18944 | 0.0644 | 0.0000 |
| 2xl_1 | 50623 | 0.1839 | 0.0000 |

![Connected streets lookup latency vs map size](report_images/connected_streets_latency_vs_map_size.png)

The sequential list search becomes slower as the map size grows because it scans the whole street segment list to find all outgoing streets connected to the chosen intersection. Larger maps have more street segments, so more comparisons are needed.

The intersections map stays almost constant because the intersection id is used to access the corresponding bucket directly. This avoids scanning all street segments, so it scales much better on larger maps.

### Raw data

<details>
<summary>Click to show raw latency data</summary>

| Test | xs_1 sequential | xs_1 map | xs_2 sequential | xs_2 map | md_1 sequential | md_1 map | lg_1 sequential | lg_1 map | xl_1 sequential | xl_1 map | 2xl_1 sequential | 2xl_1 map |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| Test 1 | 0.000 | 0.000 | 0.000 | 0.000 | 0.016 | 0.000 | 0.021 | 0.000 | 0.065 | 0.000 | 0.208 | 0.000 |
| Test 2 | 0.000 | 0.000 | 0.001 | 0.001 | 0.094 | 0.000 | 0.022 | 0.000 | 0.062 | 0.000 | 0.188 | 0.000 |
| Test 3 | 0.000 | 0.000 | 0.000 | 0.000 | 0.020 | 0.000 | 0.020 | 0.000 | 0.064 | 0.000 | 0.190 | 0.000 |
| Test 4 | 0.000 | 0.000 | 0.001 | 0.000 | 0.008 | 0.000 | 0.020 | 0.000 | 0.063 | 0.000 | 0.177 | 0.000 |
| Test 5 | 0.000 | 0.001 | 0.001 | 0.001 | 0.009 | 0.000 | 0.020 | 0.000 | 0.063 | 0.000 | 0.177 | 0.000 |
| Test 6 | 0.000 | 0.000 | 0.001 | 0.000 | 0.008 | 0.000 | 0.020 | 0.000 | 0.065 | 0.000 | 0.178 | 0.000 |
| Test 7 | 0.000 | 0.000 | 0.001 | 0.000 | 0.008 | 0.000 | 0.020 | 0.000 | 0.062 | 0.000 | 0.177 | 0.000 |
| Test 8 | 0.000 | 0.000 | 0.000 | 0.000 | 0.008 | 0.000 | 0.020 | 0.000 | 0.065 | 0.000 | 0.179 | 0.000 |
| Test 9 | 0.000 | 0.001 | 0.000 | 0.000 | 0.008 | 0.000 | 0.020 | 0.000 | 0.063 | 0.000 | 0.181 | 0.000 |
| Test 10 | 0.000 | 0.000 | 0.001 | 0.000 | 0.008 | 0.000 | 0.020 | 0.000 | 0.072 | 0.000 | 0.184 | 0.000 |

</details>

## 5. Path finding: list search vs intersections map by map size

This experiment measures the time needed to find a path with Dijkstra's algorithm. The same origin and destination were used for every measured map, but the connected-street lookup was changed: one version uses sequential search through the street list, and the other version uses the intersections map.

Origin: `41.401801, 2.196913`
Destination: `41.404192, 2.195074`

`xs_1` was not included in this path-finding experiment, so the comparison starts with `xs_2`.

All values are in milliseconds.

| Map | Street segments | Avg Dijkstra sequential search (ms) | Avg Dijkstra intersections map search (ms) |
|---|---:|---:|---:|
| xs_2 | 71 | 0.0152 | 0.0074 |
| md_1 | 1318 | 0.4159 | 0.0207 |
| lg_1 | 3797 | 0.9687 | 0.0270 |
| xl_1 | 18944 | 4.4390 | 0.0642 |
| 2xl_1 | 50623 | 3.0649 | 0.0957 |

![Dijkstra path finding latency vs map size](report_images/dijkstra_latency_vs_map_size.png)

The results show that sequential connected-street search becomes much slower as the map size increases. This happens because the algorithm repeatedly has to find the streets connected to an intersection, and doing this sequentially requires scanning through the street segments.

Using the intersections map is much faster because the connected streets are already grouped by intersection. Instead of scanning the whole street segment list, the algorithm directly accesses the bucket for the current intersection.

The intersections map scales much better. Even though its latency increases slightly in the largest map, it remains much lower than the sequential approach. The `2xl_1` sequential average is lower than the `xl_1` sequential average in this run. In the code, the latitude and longitude are converted to the closest street segment separately for each map, so the exact street sequence explored is not guaranteed to be identical in every map even when the input coordinates are the same.

### Raw data

<details>
<summary>Click to show raw latency data</summary>

| Test | xs_2 sequential | xs_2 map | md_1 sequential | md_1 map | lg_1 sequential | lg_1 map | xl_1 sequential | xl_1 map | 2xl_1 sequential | 2xl_1 map |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| Test 1 | 0.014 | 0.013 | 0.420 | 0.027 | 0.998 | 0.038 | 4.330 | 0.083 | 2.802 | 0.083 |
| Test 2 | 0.036 | 0.008 | 0.462 | 0.021 | 0.974 | 0.026 | 4.360 | 0.061 | 2.778 | 0.156 |
| Test 3 | 0.012 | 0.007 | 0.434 | 0.020 | 0.977 | 0.027 | 4.308 | 0.059 | 3.321 | 0.097 |
| Test 4 | 0.011 | 0.007 | 0.418 | 0.020 | 0.973 | 0.025 | 4.490 | 0.062 | 3.721 | 0.101 |
| Test 5 | 0.018 | 0.007 | 0.420 | 0.019 | 0.946 | 0.025 | 4.761 | 0.060 | 2.762 | 0.095 |
| Test 6 | 0.011 | 0.006 | 0.402 | 0.020 | 1.044 | 0.025 | 4.344 | 0.059 | 3.093 | 0.079 |
| Test 7 | 0.011 | 0.007 | 0.421 | 0.019 | 0.942 | 0.029 | 4.658 | 0.070 | 3.236 | 0.091 |
| Test 8 | 0.016 | 0.006 | 0.399 | 0.022 | 0.943 | 0.025 | 4.303 | 0.068 | 3.272 | 0.090 |
| Test 9 | 0.011 | 0.006 | 0.400 | 0.019 | 0.953 | 0.025 | 4.277 | 0.060 | 2.793 | 0.089 |
| Test 10 | 0.012 | 0.007 | 0.383 | 0.020 | 0.937 | 0.025 | 4.559 | 0.060 | 2.871 | 0.076 |

</details>

## 6. Path finding: latency by distance

This experiment measures the time needed to find a path between close, medium, and far destinations in the same map. The map used for every test was `xl_1`.

All values are in milliseconds.

| Distance case | Approximate distance (m) | Avg Dijkstra sequential search (ms) | Avg Dijkstra intersections map search (ms) |
| ------------- | -----------------------: | ------------------------------------------: | --------------------------------: |
| Close | 179.64 | 7.1556 | 0.0780 |
| Medium | 2324.36 | 263.2435 | 1.7021 |
| Far | 5163.80 | 570.2355 | 3.8840 |

![Dijkstra path finding latency vs path distance](report_images/dijkstra_latency_vs_distance.png)

The results show that path finding latency increases as the origin and destination become farther apart. This happens because longer paths require the algorithm to explore more intersections and street segments before reaching the destination.

The sequential connected-street search is slower because, every time the algorithm needs the streets connected to an intersection, it may have to scan through the street segment list. The intersections map version is faster because the connected streets are already grouped by intersection, so the algorithm can access them directly.

The fitted curve in the graph is a degree-1 polynomial, which is a straight trend line. This does not mean that Dijkstra is always linear in distance. According to the complexity from question 3, the runtime depends on how many street segments and connections are explored, and our sorted pending list can make the algorithm behave up to `O(E^2)` in the worst case. In this specific map and with these three routes, increasing the physical distance also increased the explored part of the graph in a fairly regular way, so the measured points follow an almost straight increasing trend.

### Raw data

<details>
<summary>Click to show raw latency data</summary>

| Test | Close sequential | Close map | Medium sequential | Medium map | Far sequential | Far map |
|---|---:|---:|---:|---:|---:|---:|
| Test 1 | 7.021 | 0.108 | 263.434 | 1.910 | 570.247 | 3.827 |
| Test 2 | 7.396 | 0.076 | 263.330 | 1.579 | 566.787 | 4.315 |
| Test 3 | 7.206 | 0.069 | 262.601 | 1.582 | 575.578 | 3.757 |
| Test 4 | 7.000 | 0.070 | 263.621 | 1.936 | 567.752 | 3.712 |
| Test 5 | 7.317 | 0.074 | 264.564 | 1.787 | 573.763 | 3.545 |
| Test 6 | 7.081 | 0.079 | 262.341 | 1.627 | 568.749 | 3.576 |
| Test 7 | 7.241 | 0.090 | 262.587 | 1.652 | 567.198 | 4.268 |
| Test 8 | 7.017 | 0.071 | 262.533 | 1.580 | 570.534 | 3.807 |
| Test 9 | 7.270 | 0.074 | 263.920 | 1.582 | 567.925 | 3.611 |
| Test 10 | 7.007 | 0.069 | 263.504 | 1.786 | 573.822 | 4.422 |

</details>

## 7. Improving the visited data structure in BFS

The improvement for the `visited` data structure is to use a hash table instead of a list/queue. In our final implementation, visited street segments are stored in a hash table. The bucket is chosen using an intersection id, and the stored street segment is compared to know if it was already visited.

This is useful because BFS checks `visited` very often. If `visited` is a list, the program has to scan the list to know whether a street was already visited. With a hash table, the program goes directly to the corresponding bucket and only checks the streets stored there.

With a list approach, checking if a street is already in `visited` costs `O(V)` in the worst case, where `V` is the number of visited streets. Since BFS performs this check many times while exploring connected streets, the total cost can become `O(E * V)`, where `E` is the number of connections explored. In road maps, `E` is usually close to `V`, so this can behave like `O(V^2)`.

With the hash table, checking if a street was already visited is `O(1)` on average. Focusing on the visited checks, this improves that part of BFS from `O(E * V)` to `O(E)` on average, before considering other costs such as storing the paths. The worst case can still be worse if many keys collide in the same bucket, but with a good table size the average case is much better than using a list.

The main trade-off is memory usage. A hash table needs extra memory for the buckets, while a list only stores the visited streets one after another. It also makes the code a little more complex, because we need a hash function and insert/search operations. For larger maps, this trade-off is worth it because BFS checks `visited` very often.

## 8. Improving closest street search from latitude and longitude

The current algorithm to find the closest street segment checks every street segment in the map. For each street, it calculates the distance between the user's coordinate and the midpoint of that street segment. It keeps the street with the smallest distance and returns it at the end. This is simple and correct, but it is slow for large maps because it always scans the full street list.

The improvement I would use is a spatial grid index. The idea is to divide the map into small rectangular cells using latitude and longitude. When the map is loaded, each street segment is inserted into the cell where its midpoint is located. Then, when the user gives a coordinate, the program checks the streets in the same cell and nearby cells instead of checking the whole map.

I would choose a grid index because it is simpler to implement and understand than a more advanced structure like a k-d tree or quadtree. It also fits this project because the street data is already based on latitude and longitude.

With the current list approach, the runtime complexity is `O(S)` for every search, where `S` is the number of street segments. This is because the program must calculate the distance to every street midpoint.

With a spatial grid, building the grid would cost `O(S)` when the map is loaded, because every street segment must be inserted once. After that, searching for the closest street would be much faster on average. If only `K` street segments are in the nearby cells that we check, the average search cost becomes `O(K)`, where `K` is usually much smaller than `S`. In the best case, the correct nearby cell contains very few streets, so the search is close to `O(1)`. The worst case can still be `O(S)` if many streets are in the same cell, or if the program has to expand the search to many cells.

The main trade-off is memory usage. The grid stores extra lists of street segments for each cell, so it uses more memory than only keeping one linked list of all streets. There is also an extra preprocessing step when loading the map. Another downside is that the cell size must be chosen carefully. If the cells are too large, each cell contains too many streets and the search is still slow. If the cells are too small, the program may need to check many neighboring cells to avoid missing the closest street.
