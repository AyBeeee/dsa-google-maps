#include "hash.h"
#include "main.h"
#include "utils.h"

// hash function
int hash_intersection(long long intersection_id) {
  return (int)(intersection_id % TABLE_SIZE);
}

// initialize hashmap with buckets as NULL
hashmap *create_hashmap() {

  hashmap *graph = malloc(sizeof(hashmap));

  for (int i = 0; i < TABLE_SIZE; i++) {
    graph->table[i] = NULL;
  }

  return graph;
}

// Find existing intersection in a bucket by going through the list

hashmap_entry *find_intersection(hashmap *graph, long long intersection_id,
                                 int index) {

  hashmap_entry *current =
      graph->table[index]; // current element in the bucket, starting at the
                           // first one

  while (current != NULL) {
    if (current->intersection_id == intersection_id) {
      return current; // return bucket where intersection was found
    }
    current = current->next;
  }

  return NULL;
}

void add_street_to_intersection(hashmap *graph, long long intersection_id,
                                node *curr) {

  int index = hash_intersection(intersection_id); // find index bucket

  hashmap_entry *entry = find_intersection(
      graph, intersection_id,
      index); // find a street in the hashmap with the same intersection id

  // if the bucket is null, create the bucket
  if (entry == NULL) {
    entry = malloc(sizeof(hashmap_entry));

    entry->intersection_id = intersection_id;
    entry->connected_streets = NULL;
    entry->next = graph->table[index];
    graph->table[index] = entry;
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

    add_street_to_intersection(graph, curr->street->from_intersection_id, curr);
    add_street_to_intersection(graph, curr->street->to_intersection_id, curr);
    // printf("Loaded %s\n", curr->name); DEBUG LOADING IN HASH MAP
    curr = curr->next;
  }
}

// find connected street in a bucket
node *get_connected_streets(hashmap *graph, long long intersection_id) {

  int index = hash_intersection(intersection_id); // find index bucket

  hashmap_entry *entry = find_intersection(graph, intersection_id, index);

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
