#ifndef PARSING_H
#define PARSING_H

#include "input.h"

int load_houses(list_map *l);
int load_places(list_map *l);
int load_streets(list_map *l);
int find_node(list_map *list, char *street, int stype);
int find_coordinate(list_map *list, double x_target, double y_target);

int LevenshteinDistance(char *a, char *b);
int hash_intersection(long long intersection_id);

hashmap *create_hashmap();
hashmap_entry *find_intersection(hashmap *graph, long long intersection_id, int index);
void add_street_to_intersection(hashmap *graph, long long intersection_id, node *curr);
void load_streets_into_graph(hashmap *graph, node *head_streets);
node *get_connected_streets(hashmap *graph, long long intersection_id);
void print_connected_streets(hashmap *graph, long long intersection_id);


#endif
