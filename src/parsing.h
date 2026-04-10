#ifndef PARSING_H
#define PARSING_H

#include "input.h"

int load_map(houses *h);
int find_address(houses *h, char *street, int num);
int LevenshteinDistance(char *a, char *b);

#endif