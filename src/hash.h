#ifndef HASH_H
#define HASH_H

#include "structs.h"
#include <stdbool.h>

int hash_intersection(hashmap *hash, long long intersection_id);

hashmap *create_hashmap(list_map *list);
intersection *find_intersection(hashmap *graph, int index);
intersection *in_intersection(hashmap *graph, int index, long long search_id);
void add_street_to_intersection(hashmap *graph, long long intersection_id,
                                double intersection_lat,
                                double intersection_lon, node *curr);
void load_streets_into_graph(hashmap *graph, node *head_streets);
node *get_connected_streets(hashmap *graph, long long intersection_id);
void print_connected_streets(hashmap *graph, long long intersection_id,
                             char *name, bool notItself);

#endif
