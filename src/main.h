#ifndef MAIN_H
#define MAIN_H

#include "structs.h"
#include <stdio.h>

void ask_map();
void ask_address(list_map *h);
int find_node(list_map *list, char *street, int stype);
int find_coordinate(list_map *list, double x_target, double y_target);
void init_list(list_map *list);

#endif
