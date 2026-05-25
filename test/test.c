#include "../src/hash.h"
#include "../src/parsing.h"
#include "../src/structs.h"
#include "utils.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

void init_list(list_map *list);
void test_create_hashmap_type(void);
bool test_hash_intersection(long long intersection_id);
void test_load_streets_into_graph(void);
void test_find_intersection_and_add_street(void);

int main(void) {

  test_create_hashmap_type();
  assert(test_hash_intersection(3));
  printf("Test Hashmap Intersection (3) Successful!\n");
  test_load_streets_into_graph();
  test_find_intersection_and_add_street();
  allsuccess();
}

// ######################### UNIT TESTS FOR LAB 5 #########################

// If create_hashmap() returns hashmap *, this compiles.
void test_create_hashmap_type() {

  list_map list;
  init_list(&list);

strcpy(list.map, "xs_1");
    load_streets(&list);
  hashmap *g = create_hashmap(&list);
  assert(g != NULL);
  printf("Test Create Hashmap Successful!\n");

}

// If the intersection returns the same value as calculating it manually it
// works
bool test_hash_intersection(long long intersection_id) {
    list_map list;
    init_list(&list);
  strcpy(list.map, "xs_1");
  load_streets(&list);
  hashmap *graph = create_hashmap(&list);

    int num = hash_intersection(graph, intersection_id);


  if (num == intersection_id % graph->table_size) {
    return true;
  }
  return false;
}

// Recreate the flow of creating->loading streets -> loading streets into graph
void test_load_streets_into_graph() {

    list_map list;
    init_list(&list);
  strcpy(list.map, "xs_1");
  int loaded = load_streets(&list);

  hashmap *graph = create_hashmap(&list);




  assert(loaded > 0);
  assert(list.head_streets != NULL);

  node *first = list.head_streets;

  load_streets_into_graph(graph, list.head_streets);

  // create 3 nodes to test from, to and an unknown one
  node *from_list =
      get_connected_streets(graph, first->street->from_intersection_id);
  node *to_list =
      get_connected_streets(graph, first->street->to_intersection_id);
  node *missing = get_connected_streets(graph, 999999);

  assert(from_list != NULL);
  assert(to_list != NULL);
  assert(missing == NULL);
  assert(from_list->street != NULL);
  assert(to_list->street != NULL);
  printf("Test Load Streets into Graph Successful!\n");

}

// test find_intersection
void test_find_intersection_and_add_street() {
    list_map list;
    init_list(&list);
  strcpy(list.map, "xs_1");
  load_streets(&list);
  hashmap *graph = create_hashmap(&list);

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
  int idx = hash_intersection(graph, 10);
  assert(find_intersection(graph, idx) == NULL);

  // add street to the bucket
  add_street_to_intersection(graph, 10, &street_node);

  // find bucket based off the street
  hashmap_entry *entry = find_intersection(graph, idx);
  assert(entry != NULL);
  assert(entry->intersection_id == 10);
  assert(entry->connected_streets != NULL);

  // test the fake street stored
  node *connected = entry->connected_streets;
  assert(strcmp(connected->name, "fake street") == 0);
  assert(connected->street == &street_data);
  assert(connected->street->from_intersection_id == 10);
  assert(connected->street->to_intersection_id == 20);
  printf("Test Find Intersection and Add Street Successful!\n");

}

// test free hashmap to-do
