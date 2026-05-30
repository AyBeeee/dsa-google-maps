#include "../src/hash.h"
#include "../src/parsing.h"
#include "../src/structs.h"
#include "../src/utils.h"
#include "utils.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
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
void test_find_place(list_map *list);
void test_streets_parse_list(list_map *list);
void free_map(list_map *list);
void test_queue(queue *q, node *test_node);
void test_bfs(void);
void test_cross_product(void);
void test_free_queue(void);
queue *BFS(hashmap *graph, node *origin, node *destination);

int main(void) {

  list_map list;
  init_list(&list);
  strcpy(list.map, "xs_1");

  // LAB 2 TESTS
  test_house_parse_list(&list);
  test_find_house(&list);

  // LAB 3,4 TESTS
  test_places_parse_list(&list);
  test_find_place(&list);
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
  test_free_queue();
  printf("Lab 6 Unit Tests Passed!\n\n");

  allsuccess();
}

// ######################### UNIT TESTS FOR LAB 2 #########################

// simply create the linked lists as per usual, if everything works it should
// return int != -1
void test_house_parse_list(list_map *list) {
  assert(load_houses(list) != -1);
  assert(list->head_houses != NULL);
  assert(list->tail_houses != NULL);
  assert(list->size_houses == 63);
  printf("Test Parse Houses.txt and Linked List Creation Passed!\n");
}

void test_find_house(list_map *list) {
  node *curr = list->head_houses;

  while (curr != NULL) {
    if (strcmp(curr->name, "c. pompeu fabra") == 0 &&
        curr->house->num == 9) {
      assert(curr->lat == 13.000000);
      assert(curr->lon == 34.333333);
      printf("Test Find House Successful\n");
      return;
    }

    curr = curr->next;
  }
}

// ######################### UNIT TESTS FOR LAB 3 #########################

void test_places_parse_list(list_map *list) {
  assert(load_places(list) != -1);
  assert(list->head_places != NULL);
  assert(list->tail_places != NULL);
  assert(list->size_places == 1);
  printf("Test Parse Places.txt and Linked List Creation Passed!\n");
}

void test_find_place(list_map *list) {
  node *curr = list->head_places;

  while (curr != NULL) {
    if (strcmp(curr->place->amenity, "amenity:university") == 0) {
      assert(curr->lat == 13.000000);
      assert(curr->lon == 34.333333);
      printf("Test Find Place Successful!\n");
      return;
    }

    curr = curr->next;
  }

  assert(0);
}

// ######################### UNIT TESTS FOR LAB 4 #########################

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
  assert(in_intersection(graph, idx, street_data.from_intersection_id) == NULL);

  // add street to the bucket
  add_street_to_intersection(graph, 10, street_node.lat, street_node.lon,
                             &street_node);

  // find bucket based off the street
  intersection *entry =
      in_intersection(graph, idx, street_data.from_intersection_id);
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

  printf("test free hashmap successful\n");
}

// ######################### UNIT TESTS FOR LAB 6 #########################

void test_queue(queue *q, node *test_node) {

  data_streets *test_street = malloc(sizeof(data_streets));

  test_node->name = "test_node";
  test_node->lat = 4.1023123;
  test_node->lon = 2.1023123;
  test_node->street = test_street;

  init_queue(q);
  enqueue(q, test_node);

  assert(q->head->name == test_node->name);
  assert(q->head->lat == test_node->lat);
  assert(q->head->lon == test_node->lon);
  assert(q->head->street == test_node->street);

  printf("test queue successful\n");
}

void test_bfs(void) {
  hashmap graph;
  graph.table_size = 10;
  graph.table = calloc(graph.table_size, sizeof(intersection *));
  data_streets *from_street_data = malloc(sizeof(data_streets));
  data_streets *to_street_data = malloc(sizeof(data_streets));

  from_street_data->from_intersection_id = 1;
  from_street_data->from_intersection_lat = 4.1023123;
  from_street_data->from_intersection_lon = 2.1023123;
  from_street_data->to_intersection_id = 2;
  from_street_data->to_intersection_lat = 0;
  from_street_data->to_intersection_lon = 0;
  from_street_data->length_meters = 10;
  from_street_data->midX = 0;
  from_street_data->midY = 0;

  to_street_data->from_intersection_id = 2;
  to_street_data->from_intersection_lat = 4.1023123;
  to_street_data->from_intersection_lon = 2.1023123;
  to_street_data->to_intersection_id = 3;
  to_street_data->to_intersection_lat = 0;
  to_street_data->to_intersection_lon = 0;
  to_street_data->length_meters = 10;
  to_street_data->midX = 0;
  to_street_data->midY = 0;

  node *from_street = malloc(sizeof(node));
  node *to_street = malloc(sizeof(node));

  from_street->name = "from_street";
  from_street->lat = 5.1023123;
  from_street->lon = 3.1023123;
  from_street->next = NULL;
  from_street->house = NULL;
  from_street->place = NULL;
  from_street->street = from_street_data;

  to_street->name = "to_street";
  to_street->lat = 5.1023123;
  to_street->lon = 3.1023123;
  to_street->next = NULL;
  to_street->house = NULL;
  to_street->place = NULL;
  to_street->street = to_street_data;

  add_street_to_intersection(&graph, 1, 0, 0, from_street);
  add_street_to_intersection(&graph, 2, 0, 0, from_street);

  add_street_to_intersection(&graph, 2, 0, 0, to_street);
  add_street_to_intersection(&graph, 3, 0, 0, to_street);

  queue *path = BFS(&graph, from_street, to_street);

  assert(path != NULL);
  assert(path->head != NULL);
  assert(strcmp(path->head->name, "from_street") == 0);
  assert(path->head->next != NULL);
  assert(strcmp(path->head->next->name, "to_street") == 0);

  printf("Test bfs passed successfully\n");
}

void test_free_queue() {
  queue *q = malloc(sizeof(queue));
  node *test_node = malloc(sizeof(node));
  data_streets *test_street = malloc(sizeof(data_streets));

  test_street->from_intersection_id = 1;
  test_street->to_intersection_id = 2;

  test_node->name = "test_node";
  test_node->lat = 0;
  test_node->lon = 0;
  test_node->next = NULL;
  test_node->house = NULL;
  test_node->place = NULL;
  test_node->street = test_street;

  init_queue(q);
  enqueue(q, test_node);

  node *removed = dequeue(q);

  assert(removed != NULL);
  assert(strcmp(removed->name, "test_node") == 0);
  assert(q->head == NULL);
  assert(q->tail == NULL);

  free(removed);
  free(test_node);
  free(test_street);
  free(q);

  printf("Test free queue passed successfully\n");
}
