#include "structs.h"
#include <stdbool.h>

int hash_intersection(long long intersection_id);

hashmap *create_hashmap();
hashmap_entry *find_intersection(hashmap *graph, long long intersection_id,
                                 int index);
void add_street_to_intersection(hashmap *graph, long long intersection_id,
                                node *curr);
void load_streets_into_graph(hashmap *graph, node *head_streets);
node *get_connected_streets(hashmap *graph, long long intersection_id);
void print_connected_streets(hashmap *graph, long long intersection_id,
                             char *name, bool notItself);
