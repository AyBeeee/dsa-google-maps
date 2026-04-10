#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "parsing.h"

int load_map(list *house_list) {

    // create route
    char route[256] = "maps/";
    strcat(route, house_list->map);
    strcat(route, "/houses.txt");
    printf("Current route: %s\n", route);

    // open file
    FILE *fp = fopen(route,"r");

    if (fp == NULL) {
        printf("Couldn't find the file/or it is empty\n");
        return -1;
    }

    // load list
    char line[256];
    while(fgets(line, sizeof(line), fp)) { // load line by line with fgets

        node *newnode = malloc(sizeof(node)); // creating one list per address

        if (newnode == NULL) {
            fclose(fp);
            return -1;
        }

        sscanf(line, "%49[^,],%d,%lf,%lf", newnode->street, &newnode->num, &newnode->lat, &newnode->lon);

        for (int i=0; i<(int)strlen(newnode->street); i++) {
            newnode->street[i] = tolower(newnode->street[i]);
        }

        newnode->next = NULL; // initialize the next node to null

        if (house_list->head == NULL) {
            house_list->head = newnode;
            house_list->tail = newnode;
        }
        else {
            house_list->tail->next = newnode;
            house_list->tail = newnode;

        }
        house_list->size++;
    }
    fclose(fp);
    return house_list->size;
}

int find_address(list *house_list, char *street, int num) {

    node *curr = house_list->head; // start search at head
    node *best_match = NULL; // init best match
    int best_dist = 5;
    int threshold = 5;
    char choice;

    // convert to lower
    for (int i = 0; i<(int)strlen(street); i++) {
        street[i] = tolower(street[i]);
    }
    printf("Street to lower: %s\n", street);

    for(int i=0; i<house_list->size; i++) {

        // compare exact match
        if (strcmp(curr->street, street) == 0) {
            if (curr->num == num) {
                printf("Found at %lf | %lf\n", curr->lat, curr->lon);
                return 1;
            }
        }
        // compare using distance to find most similar one
        else {
            int dist = LevenshteinDistance(curr->street, street);

            if (dist < best_dist) {
                best_dist = dist;
                best_match = curr;
            }
        }
        curr = curr->next;
    }

    if (best_dist < threshold && best_match != NULL) {
        printf("We couldn't find the address, did you mean: %s %d [Y-N]\n", best_match->street, best_match->num);
        scanf(" %c", &choice);

        if (choice == 'Y') {
            printf("Found at %lf | %lf\n", best_match->lat, best_match->lon);
            return 1;
        }
        else {
            printf("We couldn't find your address\n");
            return -1;
        }
    }
    printf("We couldn't find your address\n");
    return -1;
}

int LevenshteinDistance(char *a, char *b) {
    // TO-DO Levenshtein distance
}
