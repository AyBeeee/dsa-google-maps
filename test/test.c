#include "../src/hash.h"
#include "../src/parsing.h"
#include "../src/structs.h"
#include "utils.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

void test_house_parse_list(list_map *list);
void test_find_house(list_map *list);

void init_list(list_map *list);
void test_create_hashmap_type(list_map *list);
bool test_hash_intersection(list_map *list, long long intersection_id);
void test_load_streets_into_graph(list_map *list);
void test_find_intersection_and_add_street(list_map *list);
void test_freeing_hashmap(list_map *list);
void test_places_parse_list(list_map *list);
void test_streets_parse_list(list_map *list);
void free_map(list_map *list);

int main(void) {

  list_map list;
  init_list(&list);
  strcpy(list.map, "xs_1");

  // LAB 2,3,4 TESTS
  test_house_parse_list(&list);
  test_places_parse_list(&list);
  test_streets_parse_list(&list);

  // LAB 5 UNIT TESTS
  test_create_hashmap_type(&list);
  assert(test_hash_intersection(&list, 3));
  printf("Test Hashmap Intersection Successful!\n");
  test_load_streets_into_graph(&list);
  test_find_intersection_and_add_street(&list);
  test_freeing_hashmap(&list);
  printf("Lab 5 Unit Tests Passed!\n\n");

  allsuccess();
}

// ######################### UNIT TESTS FOR LAB 2,3,4 #########################

// simply create the linked lists as per usual, if everything works it should
// return int != -1
void test_house_parse_list(list_map *list) {
  assert(load_houses(list) != -1);
  printf("Test Parse Houses.txt and Linked List Creation Passed!\n");
}

void test_places_parse_list(list_map *list) {
  assert(load_places(list) != -1);
  printf("Test Parse Places.txt and Linked List Creation Passed!\n");
}

void test_streets_parse_list(list_map *list) {
  assert(load_streets(list) != -1);
  printf("Test Parse Streets.txt and Linked List Creation Passed!\n");
}

// ######################### UNIT TESTS FOR LAB 5 #########################

// If create_hashmap() returns hashmap *, this compiles.
void test_create_hashmap_type(list_map *list) {

  hashmap *g = create_hashmap(list);
  assert(g != NULL);
  list->graph = g;
  printf("Test Create Hashmap Successful!\n");
}

// If the intersection returns the same value as calculating it manually it
// works
bool test_hash_intersection(list_map *list, long long intersection_id) {

  hashmap *graph = list->graph;
  int num = hash_intersection(graph, intersection_id);

  if (num == intersection_id % graph->table_size) {
    return true;
  }
  return false;
}

// Recreate the flow of creating->loading streets -> loading streets into graph
void test_load_streets_into_graph(list_map *list) {

  hashmap *graph = list->graph;

  node *first = list->head_streets;

  load_streets_into_graph(graph, list->head_streets);

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
void test_find_intersection_and_add_street(list_map *list) {
  hashmap *graph = create_hashmap(list);

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
  add_street_to_intersection(graph, 10, street_node.lat, street_node.lon, &street_node);

  // find bucket based off the street
  intersection *entry = find_intersection(graph, idx);
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

void test_freeing_hashmap(list_map *list) {

  free_map(list);

  assert(list->head_houses == NULL);
  assert(list->tail_houses == NULL);

  assert(list->head_places == NULL);
  assert(list->tail_places == NULL);

  assert(list->head_streets == NULL);
  assert(list->tail_streets == NULL);

  assert(list->graph == NULL);
}
