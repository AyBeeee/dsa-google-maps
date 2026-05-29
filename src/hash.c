#include "hash.h"
#include "main.h"
#include "structs.h"
#include "utils.h"
#include <string.h>

// hash function
int hash_intersection(hashmap *hash, long long intersection_id) {
  return (int)(intersection_id % hash->table_size);
}

//helper function tells us if id is already present in the intersections list
bool intersection_seen(long long *intersections, int num_unique,
                              long long intersection_id) {
  for (int i = 0; i < num_unique; i++) {
    if (intersections[i] == intersection_id)
      return true;
  }
  return false;
}

//helper function to add the given id only if not present in the list already
void add_intersection_if_new(long long *intersections, int *num_unique,
                                    long long intersection_id) {
  if (!intersection_seen(intersections, *num_unique, intersection_id))
    intersections[(*num_unique)++] = intersection_id;
}

// count unique intersection ids (both from and to) to size the hashmap
int unique_intersections(list_map *list) {

  node *head = list->head_streets;
  int max_intersections = list->size_streets * 2; //worst case all from & to are diff
  long long *intersections = malloc(max_intersections * sizeof(long long));
  int num_unique = 0;

  while (head)
  {
    add_intersection_if_new(intersections, &num_unique,
                            head->street->from_intersection_id);
    add_intersection_if_new(intersections, &num_unique,
                            head->street->to_intersection_id);
    head = head->next;
  }

  free(intersections);
  return num_unique;
}

// initialize hashmap with buckets as NULL
hashmap *create_hashmap(list_map *list) {

  hashmap *graph = malloc(sizeof(hashmap));
  graph->table_size = unique_intersections(list);
  graph->table = malloc(graph->table_size * sizeof(intersection));
  if (graph->table == NULL)
    return NULL;

  for (int i = 0; i < graph->table_size; i++) {
    graph->table[i] = NULL;
  }

  return graph;
}

void add_street_to_intersection(hashmap *graph, long long intersection_id,
                                double intersection_lat,
                                double intersection_lon, node *curr) {

  int index = hash_intersection(graph, intersection_id); // find index bucket
  intersection *entry = in_intersection(graph, index, intersection_id);
  // if the bucket is null, create the bucket
  if (entry == NULL) {
    entry = malloc(sizeof(intersection));
    if (!entry)
      return;

    entry->intersection_id = intersection_id;
    entry->connected_streets = NULL;
    entry->next = graph->table[index];
    entry->intersection_lat = intersection_lat;
    entry->intersection_lon = intersection_lon;
    graph->table[index] = entry;
    // printf("--- INDEX NEW BUCKET AT %i\n", index); DEBUG
  }

  // if the bucket is not null, add the new node connected to the element in the
  // bucket since the streets will be connected
  node *new_node = malloc(sizeof(node));
  if (new_node == NULL) {
    return;
  }

  new_node->name = curr->name;
  new_node->street = curr->street;
  new_node->lat = curr->lat;
  new_node->lon = curr->lon;
  new_node->house = NULL;
  new_node->place = NULL;
  new_node->next = entry->connected_streets;

  entry->connected_streets = new_node;
}

// Load streets into the hashmap

void load_streets_into_graph(hashmap *graph, node *head_streets) {

  node *curr = head_streets;

  while (curr != NULL) {

    add_street_to_intersection(graph, curr->street->from_intersection_id,
                               curr->street->from_intersection_lat,
                               curr->street->from_intersection_lon, curr);
    add_street_to_intersection(graph, curr->street->to_intersection_id,
                               curr->street->to_intersection_lat,
                               curr->street->to_intersection_lon, curr);
    // printf("Loaded %s\n", curr->name); DEBUG LOADING IN HASH MAP
    curr = curr->next;
  }
}

intersection *in_intersection(hashmap *graph, int index, long long search_id) {
  intersection *current = graph->table[index];

  while (current != NULL) {
    if (current->intersection_id == search_id)
      return current;
    // printf("current intersection id is %lli\n", current->intersection_id);
    // DEBUG
    current = current->next;
  }

  return NULL;
}

// find connected street in a bucket
node *get_connected_streets(hashmap *graph, long long intersection_id) {

  int index = hash_intersection(graph, intersection_id); // find index bucket

  intersection *entry = in_intersection(graph, index, intersection_id);

  if (entry == NULL) {
    return NULL;
  }

  return entry->connected_streets;
}

// print connected streets in a bucket
void print_connected_streets(hashmap *graph, long long intersection_id,
                             char *name, bool notItself) {
  node *current = get_connected_streets(graph, intersection_id);

  while (current != NULL) {
    if (notItself && same_street_name(current->name, name) == true)
      ;
    else
      printf("- %s, length: %lf meters\n", current->name,
             current->street->length_meters);
    current = current->next;
  }
}
