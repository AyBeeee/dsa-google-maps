#ifndef PARSING_H
#define PARSING_H

#include "input.h"

int load_houses(list_map *l);
int load_places(list_map *l);
int load_streets(list_map *l);
int find_node(list_map *list, char *street, int stype);
int LevenshteinDistance(char *a, char *b);

#endif
