#ifndef UTILS_H
#define UTILS_H

#include "structs.h"
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

int LevenshteinDistance(char *a, char *b);
void read_long_input(char *buffer, int size);
void free_map(list_map *list);
double toDegrees(double radians);
double toRadians(double degree);
void midpoint(node *newnode);
double haversine(double lat_A, double lon_A, double lat_B, double lon_B);
int findnchar(char *str, char c, int n);
char *skip_street_prefix(char *str);
bool same_street_name(char *map_name, char *user_name);
int LevenshteinDistance(char *a, char *b);
node *node_to_street(list_map *list, node *node);
void init_queue(queue *q);
void enqueue(queue *q, node *curr_node);
node *dequeue(queue *q);
node *node_in_queue(queue *q, node *curr);

#endif
