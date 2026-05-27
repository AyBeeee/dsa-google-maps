#ifndef MAIN_H
# define MAIN_H

# include "structs.h"
# include <stdio.h>
# include <stdbool.h>

node *ask(list_map *list, int stype);
node *ask_node(list_map *list);
void ask_map(list_map *list);
void ask_address(list_map *h);
node *find_node(list_map *list, char *street, int stype);
node *find_coordinate(list_map *list, double x_target, double y_target, bool verbose);
void init_list(list_map *list);

#endif
