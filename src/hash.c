#include "hash.h"
#include "main.h"
#include "structs.h"
#include "utils.h"
#include <string.h>

// hash function
int hash_intersection(hashmap *hash, long long intersection_id) {
  return (int)(intersection_id % hash->table_size);
}

// count the amount of unique intersection_id variables, to size the hashmap
// correctly
int unique_intersections(list_map *list) {
  node *head = list->head_streets;
  int *intersections = malloc(list->size_streets * sizeof(int));
  for (int i = 0; i < list->size_streets; i++)
    intersections[i] = -1;
  int num_unique = 0;

  while (head) {
    int i = 0;
    for (i = i; i < list->size_streets && i >= 0; i++)
      if (head->street->from_intersection_id == intersections[i]) {
        i = -42;
        break;
      }
    if (i != -42)
      intersections[num_unique++] = head->street->from_intersection_id;

    head = head->next;
  }
  free(intersections);
  printf("%i\n", num_unique);
  return (num_unique);
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

// Find existing intersection in a bucket by going through the list

intersection *find_intersection(hashmap *graph, int index) {

  intersection *current =
      graph->table[index]; // current element in the bucket, starting at the
                           // first one

  while (current != NULL) {
    if (current->next == NULL)
      return current;
    // printf("current intersection id is %lli\n", current->intersection_id);
    // DEBUG
    current = current->next;
  }

  return NULL;
}

void add_street_to_intersection(hashmap *graph, long long intersection_id, double intersection_lat,
                                double intersection_lon, node *curr) {

  int index = hash_intersection(graph, intersection_id); // find index bucket
  intersection *entry = find_intersection(graph, index);
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

    add_street_to_intersection(graph, curr->street->from_intersection_id, curr->street->from_intersection_lat, curr->street->from_intersection_lon, curr);
    add_street_to_intersection(graph, curr->street->to_intersection_id, curr->street->to_intersection_lat, curr->street->to_intersection_lon, curr);
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
