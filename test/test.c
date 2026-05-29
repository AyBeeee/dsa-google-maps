#include "../src/hash.h"
#include "../src/parsing.h"
#include "../src/structs.h"
#include "utils.h"
#include "hash.h"
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
  printf("Lab 5 Unit Tests Passed!\n\n");

  // LAB 6 UNIT TESTS
  queue *q = malloc(sizeof(queue));
  node *test_node = malloc(sizeof(node));
  test_queue(q, test_node);
  test_bfs();


  test_freeing_hashmap(&list);



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
  add_street_to_intersection(graph, 10, street_node.lat, street_node.lon,
                             &street_node);

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


// ######################### UNIT TESTS FOR LAB 6 #########################

void test_queue(queue *q, node *test_node) {
  test_node->name = "test_node";
  test_node->lat = 4.1023123;
  test_node->lon = 2.1023123;
  test_node->street = "Carrer test node";

  init_queue(q);

  enqueue(q, test_node);

  assert(q->head->name = test_node->name);
  assert(q->head->lat = test_node->lat);
  assert(q->head->lon = test_node->lon);
  assert(q->head->street = test_node->street);
}

void test_bfs(void) {
  hashmap graph;
  graph.table_size = 10;
  graph.table = calloc(graph.table_size, sizeof(intersection *));

  data_streets *ab = malloc(sizeof(data_streets));
  data_streets *bc = malloc(sizeof(data_streets));

  ab->from_intersection_id = 1;
  ab->from_intersection_lat = 0;
  ab->from_intersection_lon = 0;
  ab->to_intersection_id = 2;
  ab->to_intersection_lat = 0;
  ab->to_intersection_lon = 0;
  ab->length_meters = 10;
  ab->midX = 0;
  ab->midY = 0;

  bc->from_intersection_id = 2;
  bc->from_intersection_lat = 0;
  bc->from_intersection_lon = 0;
  bc->to_intersection_id = 3;
  bc->to_intersection_lat = 0;
  bc->to_intersection_lon = 0;
  bc->length_meters = 10;
  bc->midX = 0;
  bc->midY = 0;

  node *street_ab = malloc(sizeof(node));
  node *street_bc = malloc(sizeof(node));

  street_ab->name = "street_ab";
  street_ab->lat = 0;
  street_ab->lon = 0;
  street_ab->next = NULL;
  street_ab->house = NULL;
  street_ab->place = NULL;
  street_ab->street = ab;

  street_bc->name = "street_bc";
  street_bc->lat = 0;
  street_bc->lon = 0;
  street_bc->next = NULL;
  street_bc->house = NULL;
  street_bc->place = NULL;
  street_bc->street = bc;

  add_street_to_intersection(&graph, 1, 0, 0, street_ab);
  add_street_to_intersection(&graph, 2, 0, 0, street_ab);

  add_street_to_intersection(&graph, 2, 0, 0, street_bc);
  add_street_to_intersection(&graph, 3, 0, 0, street_bc);
}

void test_cross_product(){

}

void test_free_queue() {

}