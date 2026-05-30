#ifndef UTILS_H
#define UTILS_H

#define EARTH_RADIUS 6371.0

#include "structs.h"
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

enum turndirection { LEFT, RIGHT, STRAIGHT };
enum idmissing { FROM, TO, BOTH, NONE };

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
bool street_in_visited(hashmap *visited, node *street);
int cross_product(node *from, node *to);
void free_hash(hashmap *graph);
int is_prime(int n);
queue *copy_queue(queue *original);
void free_queue(queue *q);

#endif
