#include "structs.h"

void init_list(list_map *list) {
  list->head_houses = NULL;
  list->tail_houses = NULL;
  list->size_houses = 0;

  list->head_places = NULL;
  list->tail_places = NULL;
  list->size_places = 0;

  list->head_streets = NULL;
  list->tail_streets = NULL;
  list->size_streets = 0;

  list->graph = NULL;
}
