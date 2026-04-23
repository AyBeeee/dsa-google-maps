#ifndef INPUT_H
#define INPUT_H
#include <stdlib.h>

enum type { HOUSES, PLACES };

typedef struct data_houses {
  int num;
} data_houses;

typedef struct data_places {
  char id[50];
  char amenity[50];
} data_places;

typedef struct data_streets {
  int from_intersaction_id;
  double from_intersection_lat, from_intersection_lon;
  int to_intersection_id;
  double to_intersection_lat, to_intersection_lon;
  int length_meters;
  char street_name[50];
} data_streets;

typedef struct node {
  char name[50];
  double lat, lon;
  struct node *next;
  struct data_houses *house;
  struct data_places *place;
  struct data_streets *street;
  // struct data_streets *street;
} node;

typedef struct {
  char map[10];

  node *head_houses;
  node *tail_houses;
  int size_houses;

  node *head_places;
  node *tail_places;
  int size_places;

  node *head_streets;
  node *tail_streets;
  int size_streets;

} list_map;

#endif
