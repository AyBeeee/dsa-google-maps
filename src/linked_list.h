#include <stdio.h>
#include <stdlib.h>

typedef struct node {

    char street[65];
    int address_num;
    float coordinate; 
    struct node *next;

} house;