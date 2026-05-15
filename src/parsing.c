#include "parsing.h"
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// builds the houses linked list, allocating, assigning variables, and linking
// each node
int load_houses(list_map *list) {
  // create route
  char route[256] = "maps/";
  strcat(route, list->map);
  strcat(route, "/houses.txt");
  printf("Current route: %s\n", route); // DEBUG

  // open file
  FILE *fp = fopen(route, "r");

  if (fp == NULL) {
    printf("Couldn't find the file/or it is empty\n");
    return -1;
  }

  // load list
  char line[256];
  while (fgets(line, sizeof(line), fp)) { // load line by line with fgets

    node *newnode = malloc(sizeof(node));
    if (newnode == NULL) {
      fclose(fp);
      return -1;
    }

    newnode->house = malloc(sizeof(data_houses));
    newnode->place = NULL;
    newnode->street = NULL;

    if (newnode->house == NULL) {
      fclose(fp);
      free(newnode);
      return -1;
    }

    sscanf(line, "%49[^,],%d,%lf,%lf", newnode->name, &(newnode->house->num),
           &newnode->lat, &newnode->lon);

    for (int i = 0; i < (int)strlen(newnode->name); i++) {
      newnode->name[i] = tolower(newnode->name[i]);
    }

    newnode->next = NULL; // initialize the next node to null

    if (list->head_houses == NULL) {
      list->head_houses = newnode;
      list->tail_houses = newnode;
    } else {
      list->tail_houses->next = newnode;
      list->tail_houses = newnode;
    }
    list->size_houses++;
  }
  fclose(fp);
  return list->size_houses;
}

// builds the places linked list, allocating, assigning variables, and linking
// each node
int load_places(list_map *list) {

  char route[256] = "maps/";
  strcat(route, list->map);
  strcat(route, "/places.txt");
  printf("Current route: %s\n", route); // DEBUG

  // open file
  FILE *fp = fopen(route, "r");

  if (fp == NULL) {
    printf("Couldn't find the file/or it is empty\n");
    return -1;
  }

  // load list
  char line[256];
  while (fgets(line, sizeof(line), fp)) { // load line by line with fgets

    node *newnode = malloc(sizeof(node));
    if (newnode == NULL) {
      fclose(fp);
      return -1;
    }

    newnode->place = malloc(sizeof(data_places));
    newnode->house = NULL;
    newnode->street = NULL;


    if (newnode->place == NULL) {
      fclose(fp);
      free(newnode);
      return -1;
    }

    sscanf(line, "%49[^,],%49[^,],%49[^,],%lf,%lf", newnode->place->id,
           newnode->name, newnode->place->amenity, &newnode->lat,
           &newnode->lon);
    // printf("parsed and saved %s, %s, %s, %f, %f\n", newnode->place->id,
    // newnode->name, newnode->place->amenity, newnode->lat, newnode->lon);
    // //DEBUG

    for (int i = 0; i < (int)strlen(newnode->name); i++) {
      newnode->name[i] = tolower(newnode->name[i]);
    }

    newnode->next = NULL; // initialize the next node to null

    if (list->head_places == NULL) {
      list->head_places = newnode;
      list->tail_places = newnode;
    } else {
      list->tail_places->next = newnode;
      list->tail_places = newnode;
    }
    list->size_places++;
  }
  fclose(fp);
  return list->size_places;
}

int load_streets(list_map *list) {
  char route[256] = "maps/";
  strcat(route, list->map);
  strcat(route, "/streets.txt");
  printf("Current route: %s\n", route);

  // open file
  FILE *fp = fopen(route, "r");

  if (fp == NULL) {
    printf("Couldn't find the file/or it is empty\n");
    return -1;
  }

  // load list
  char line[256];
  while (fgets(line, sizeof(line), fp)) {

    node *newnode = malloc(sizeof(node));
    if (newnode == NULL) {
      fclose(fp);
      return -1;
    }

    newnode->street = malloc(sizeof(data_streets));
    newnode->house = NULL;
    newnode->place = NULL;

    if (newnode->street == NULL) {
      fclose(fp);
      free(newnode);
      return -1;
    }

    sscanf(line, "%d, %lf, %lf, %d, %lf, %lf, %d, %49[^,]",
           &newnode->street->from_intersection_id,
           &newnode->street->from_intersection_lat,
           &newnode->street->from_intersection_lon,
           &newnode->street->to_intersection_id,
           &newnode->street->to_intersection_lat,
           &newnode->street->to_intersection_lon,
           &newnode->street->length_meters, newnode->street->street_name);

    for (int i = 0; i < (int)strlen(newnode->street->street_name); i++) {
      newnode->street->street_name[i] =
          tolower(newnode->street->street_name[i]);
    }

    newnode->next = NULL;

    if (list->head_streets == NULL) {
      list->head_streets = newnode;
      list->tail_streets = newnode;
    }
    if (list->tail_streets != NULL) {
      list->tail_streets->next = newnode;
      list->tail_streets = newnode;
    }
    list->size_streets++;
  }
  fclose(fp);
  return list->size_streets;
}

int find_node(list_map *list, char *street, int stype) {

  node *curr = NULL;
  int size = 0;
  if (stype == HOUSES) {
    curr = list->head_houses;
    size = list->size_houses;
  } else if (stype == PLACES) {
    curr = list->head_places;
    size = list->size_places;
  }

  node *best_match = NULL; // init best match
  int best_dist = 7;
  int threshold = 7;
  int street_found = 0;
  char *matched_street = street;
  char choice;
  int num = -1;

  if (stype == HOUSES) {
    printf("Enter street number (e.g. '138'): \n");
    scanf("%d", &num);
  }
  // convert to lower
  for (int i = 0; i < (int)strlen(street); i++) {
    street[i] = tolower(street[i]);
  }
  // printf("Street to lower: %s.\n", street); // DEBUG
  for (int i = 0; i < size; i++) {
    // printf("checking input to: %s.\n", curr->name); // DEBUG

    // compare exact match
    if (strcmp(curr->name, street) == 0) {
      street_found = 1;
      matched_street = curr->name;
      // printf("Street's current NUM: %d.\n", curr->house->num); // DEBUG

      if (stype == HOUSES && curr->house->num == num) {
        printf("Found at %lf | %lf\n", curr->lat, curr->lon);
        return 1;
      } else if (stype != HOUSES) {
        printf("Found at %lf | %lf\n", curr->lat, curr->lon);
        return 1;
      }
    }
    // compare using distance to find most similar one
    else {
      int dist = LevenshteinDistance(curr->name, street);
      // printf("dist calculated is: %i\n", dist); //DEBUG
      if (dist < best_dist) {
        best_dist = dist;
        best_match = curr;
      }
    }
    curr = curr->next;
  }

  if (best_dist < threshold && best_match != NULL) {
    printf("We couldn't find the address, did you mean: %s [Y-N]\n",
           best_match->name);
    scanf(" %c", &choice);

    if (choice == 'Y') {
      street_found = 1;
      matched_street = best_match->name;
    } else {
      printf("We couldn't find your location!\n");
      return -1;
    }
  }

  if (street_found && stype == HOUSES) {
    char num_str[16];
    char candidate_str[16];
    int best_num_dist = INT_MAX;
    node *best_num_match = NULL;

    printf("We found the street, but number %d is not valid.\n", num);
    printf("Valid numbers in %s: ", matched_street);

    snprintf(num_str, sizeof(num_str), "%d", num);

    curr = list->head_houses;
    while (curr != NULL) {
      if (strcmp(curr->name, matched_street) == 0) {
        int dist;

        printf("%d ", curr->house->num);
        snprintf(candidate_str, sizeof(candidate_str), "%d", curr->house->num);
        dist = LevenshteinDistance(candidate_str, num_str);

        if (dist < best_num_dist) {
          best_num_dist = dist;
          best_num_match = curr;
        }
      }
      curr = curr->next;
    }
    printf("\n");

    if (best_num_match != NULL) {
      printf("Did you mean number %d? [Y-N]\n", best_num_match->house->num);
      scanf(" %c", &choice);

      if (choice == 'Y') {
        printf("Found at %lf | %lf\n", best_num_match->lat,
               best_num_match->lon);
        return 1;
      }
    }

    printf("That number is not valid for this street\n");
    return -1;
  }
  printf("We couldn't find your location!\n");
  return -1;
}

int LevenshteinDistance(char *a, char *b) {
  int la = strlen(a);
  int lb = strlen(b);
  int cost;
  int matrix[la + 1][lb + 1];

  for (int i = 0; i <= la; i++) {
    matrix[i][0] = i;
  }

  for (int j = 0; j <= lb; j++) {
    matrix[0][j] = j;
  }

  for (int i = 1; i <= la; i++) {
    for (int j = 1; j <= lb; j++) {
      if (a[i - 1] == b[j - 1]) {
        cost = 0; // looks in diagonal and if the cost is 0 keeps the same
                  // number because it will be adding them until the end bottom
                  // right corner
      } else {
        cost = 1;
      }

      int del = matrix[i - 1][j] + 1;
      int ins = matrix[i][j - 1] + 1;
      int sub = matrix[i - 1][j - 1] + cost;

      if (del < ins && del < sub) {
        matrix[i][j] = del;
      } else if (ins < sub) {
        matrix[i][j] = ins;
      } else {
        matrix[i][j] = sub;
      }
    }
  }
  return matrix[la][lb];
}

// hash function
int hash_intersection(int intersection_id) {
  return intersection_id % TABLE_SIZE;
}

// initialize hashmap with buckets as NULL
hashmap *create_hashmap() {

  hashmap *graph = malloc(sizeof(hashmap));

  for (int i = 0; i < TABLE_SIZE; i++) {
    graph->table[i] = NULL;
  }

  return graph;
}

// Find existing intersection in a bucket by going through the list
hashmap_entry *find_intersection(hashmap *graph, int intersection_id, int index) {
  
  hashmap_entry *current = graph->table[index];

  while (current != NULL) {
    if (current->intersection_id == intersection_id) {
      return current; // return bucket where intersection was found
    }

    current = current->next;
  }

  return NULL;
}

void add_street_to_intersection(hashmap *graph, int intersection_id, data_streets *street) {

  int index = hash_intersection(intersection_id); // find index bucket

  hashmap_entry *entry = find_intersection(graph, intersection_id, index); // find where the 

  // if the bucket is null, create the bucket
  if (entry == NULL) {
    entry = malloc(sizeof(hashmap_entry));

    entry->intersection_id = intersection_id;
    entry->connected_streets = NULL;

    entry->next = graph->table[index];
    graph->table[index] = entry;
  }

  // add the new node to the bucket
  node *new_node = malloc(sizeof(node));

  new_node->street = street;
  new_node->house = NULL;
  new_node->place = NULL;
  new_node->next = entry->connected_streets;

  entry->connected_streets = new_node;
}

// Load streets into the hashmap
void load_streets_into_graph(hashmap *graph, data_streets streets[], int number_of_streets) {
  
  for (int i = 0; i < number_of_streets; i++) {
    data_streets *street = &streets[i];

    add_street_to_intersection(graph, street->from_intersection_id, street);
    add_street_to_intersection(graph, street->to_intersection_id, street);
  }
}

// find connected street in a bucket
node *get_connected_streets(hashmap *graph, int intersection_id) {
  hashmap_entry *entry = find_intersection(graph, intersection_id);

  if (entry == NULL) {
    return NULL;
  }

  return entry->connected_streets;
}

void print_connected_streets(hashmap *graph, int intersection_id) {
  node *current = get_connected_streets(graph, intersection_id);

  printf("Intersection %d is connected to:\n", intersection_id);

  while (current != NULL) {
    printf("- %s, length: %d meters\n", current->street->street_name,
           current->street->length_meters);
    current = current->next;
  }
}