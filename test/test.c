#include "sample_lib_test.c"
#include "utils.h"
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "./src/input.h"

int main() {
  {
    // Call all test modules you want to run here
    test_create_hashmap_type();
    test_hash_intersection(3.12318291210923);
    test_load_streets_into_graph();
    test_find_intersection_and_add_street();
    
    // free hashmap to-do
    sample_lib_test();
  }
  allsuccess();
}

// If create_hashmap() returns hashmap *, this compiles.
void test_create_hashmap_type(void) {
  hashmap *g = create_hashmap();
}

// If the intersection returns the same value as calculating it manually it works
bool test_hash_intersection(long long intersection_id) {
  int num = hash_intersection(intersection_id);

  if (num == intersection_id % TABLE_SIZE) {
    return true;
  }
  return false;
}

// Recreate the flow of creating->loading streets -> loading streets into graph
void test_load_streets_into_graph() {
  hashmap *graph = create_hashmap();
  list_map list;

  init_list(&list);
  strcpy(list.map, "xs_1");

  int loaded = load_streets(&list);

  assert(loaded > 0);
  assert(list.head_streets != NULL);

  node *first = list.head_streets;

  load_streets_into_graph(graph, list.head_streets);

  // create 3 nodes to test from, to and an uknown one
  node *from_list = get_connected_streets(graph, first->street->from_intersection_id);
  node *to_list   = get_connected_streets(graph, first->street->to_intersection_id);
  node *missing   = get_connected_streets(graph, -1);

  assert(from_list != NULL);
  assert(to_list != NULL);
  assert(missing == NULL);
  assert(from_list->street != NULL);
  assert(to_list->street != NULL);
}

// test find_intersection
void test_find_intersection_and_add_street() {
  hashmap *graph = create_hashmap();

  node street_node;
  data_streets street_data;

  // init fake values for the street
  street_node.name = "fake street";
  street_node.street = &street_data;
  street_node.house = NULL;
  street_node.place = NULL;
  street_node.next = NULL;
  street_node.lat = 0;
  street_node.lon = 0;

  // init fake data for the streets
  street_data.from_intersection_id = 10;
  street_data.to_intersection_id = 20;
  street_data.length_meters = 100.0;

  // test idx
  int idx = hash_intersection(10);
  assert(find_intersection(graph, 10, idx) == NULL);

  // add street to the bucket
  add_street_to_intersection(graph, 10, &street_node);

  // find bucket based off the street
  hashmap_entry *entry = find_intersection(graph, 10, idx);
  assert(entry != NULL);
  assert(entry->intersection_id == 10);
  assert(entry->connected_streets != NULL);

  // test the fake street stored
  node *connected = entry->connected_streets;
  assert(strcmp(connected->name, "fake street") == 0);
  assert(connected->street == &street_data);
  assert(connected->street->from_intersection_id == 10);
  assert(connected->street->to_intersection_id == 20);
}

// test free hashmap to-do