#ifndef INPUT_H
#define INPUT_H
#include <stdlib.h>

typedef struct node {
  char street[50];
  int num;
  double lat, lon;
  struct node *next;
} node;

typedef struct {
  char map[10];
  node *head;
  node *tail;
  int size;
} list;

typedef struct node {
  char id[50];
  char name[50];
  char amenity[50];
  double lat, lon;
  struct node *next;
} node_places;

typedef struct {
  char place[10];
  node *head;
  node *tail;
  int size;
} list_places;

#endif
