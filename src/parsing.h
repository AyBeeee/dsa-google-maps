#ifndef PARSING_H
#define PARSING_H

#include "input.h"

int load_map(list *h);
int find_address(list *h, char *street, int num);
int LevenshteinDistance(char *a, char *b);

#endif
