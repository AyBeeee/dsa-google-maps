#ifndef STRUCTS_H
# define STRUCTS_H
# include <stdlib.h>

enum nodetype { HOUSES, PLACES, STREETS };
enum searchtype { ORIGIN, DESTINATION };

typedef struct data_houses {
  int num;
} data_houses;

typedef struct data_places {
  char id[50];
  char amenity[50];
} data_places;

typedef struct data_streets {
  long long from_intersection_id;
  double from_intersection_lat, from_intersection_lon;
  long long to_intersection_id;
  double to_intersection_lat, to_intersection_lon;
  double length_meters;
  double midX;
  double midY;
} data_streets;

typedef struct node {
  char *name;
  double lat, lon;
  struct node *next;
  struct data_houses *house;
  struct data_places *place;
  struct data_streets *street;
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

  struct hashmap *graph;

} list_map;

typedef struct intersection {
  long long intersection_id;
  double intersection_lat, intersection_lon;

  node *connected_streets;
  struct intersection *next;
} intersection;

typedef struct hashmap {
  intersection **table;
  int table_size;
} hashmap;

typedef struct {
  node *head;
  node *tail;
  int queue_size;
} queue;

#endif
