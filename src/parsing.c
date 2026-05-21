#include "parsing.h"
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO esto todo en utils.c
//
double toDegrees(double radians) { return radians * (180.0 / M_PI); }

double toRadians(double degree) { return degree * (M_PI / 180.0); }

void midpoint(node *newnode) {
  double lat1 = toRadians(newnode->street->from_intersection_lat);
  double lon1 = toRadians(newnode->street->from_intersection_lon);
  double lat2 = toRadians(newnode->street->to_intersection_lat);
  double lon2 = toRadians(newnode->street->to_intersection_lon);

  double x1 = cos(lat1) * cos(lon1);
  double y1 = cos(lat1) * sin(lon1);
  double z1 = sin(lat1);

  double x2 = cos(lat2) * cos(lon2);
  double y2 = cos(lat2) * sin(lon2);
  double z2 = sin(lat2);

  double x = (x1 + x2) / 2.0;
  double y = (y1 + y2) / 2.0;
  double z = (z1 + z2) / 2.0;

  double lon = atan2(y, x);
  double hyp = sqrt(x * x + y * y);
  double lat = atan2(z, hyp);

  newnode->street->midX = toDegrees(lat);
  newnode->street->midY = toDegrees(lon);
}

int findnchar(char *str, char c, int n) {
  if (!str)
    return -1;
  if (n <= 0)
    return 0;
  int i = 0;
  for (i = 0; str[i] && n > 0; i++) {
    if (str[i] == c)
      n--;
  }
  if (!str[i] || n != 0)
    return -1;
  return i;
}

char *skip_street_prefix(char *str) {
  if (strncmp(str, "c. ", 3) == 0)
    return str + 3;
  if (strncmp(str, "c ", 2) == 0)
    return str + 2;
  if (strncmp(str, "carrer ", 7) == 0)
    return str + 7;
  if (strncmp(str, "av. ", 4) == 0)
    return str + 4;
  if (strncmp(str, "av ", 3) == 0)
    return str + 3;
  if (strncmp(str, "avinguda ", 9) == 0)
    return str + 9;
  if (strncmp(str, "ptge. ", 6) == 0)
    return str + 6;
  if (strncmp(str, "ptge ", 5) == 0)
    return str + 5;
  if (strncmp(str, "passatge ", 9) == 0)
    return str + 9;
  return str;
}

int same_street_name(char *map_name, char *user_name) {
  return strcmp(skip_street_prefix(map_name), skip_street_prefix(user_name)) ==
         0;
}

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

    // calloc between beginning of string and first comma
    newnode->name = calloc(
        findnchar(line, ',', 1) - findnchar(line, ',', 0) + 1, sizeof(char));
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
  char line[1024];
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

    // calloc for length of name string (in this case between second and first
    // comma characters)
    newnode->name = calloc(
        findnchar(line, ',', 2) - findnchar(line, ',', 1) + 1, sizeof(char));
    sscanf(line, "%49[^,],%[^,],%49[^,],%lf,%lf", newnode->place->id,
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

    // calloc between the end of the line and seventh comma
    newnode->name =
        calloc(strlen(line) - findnchar(line, ',', 7) + 1, sizeof(char));
    sscanf(line, "%lli, %lf, %lf, %lli, %lf, %lf, %lf, %49[^\n]",
           &newnode->street->from_intersection_id,
           &newnode->street->from_intersection_lat,
           &newnode->street->from_intersection_lon,
           &newnode->street->to_intersection_id,
           &newnode->street->to_intersection_lat,
           &newnode->street->to_intersection_lon,
           &newnode->street->length_meters, newnode->name);

    for (int i = 0; i < (int)strlen(newnode->name); i++) {
      newnode->name[i] = tolower(newnode->name[i]);
    }
    midpoint(newnode);
    // printf("midpoint x%f y%f\n", newnode->street->midX,
    // newnode->street->midY); //DEBUG

    newnode->next = NULL;

    if (list->head_streets == NULL) {
      list->head_streets = newnode;
      list->tail_streets = newnode;
    } else {
      list->tail_streets->next = newnode;
      list->tail_streets = newnode;
    }
    list->size_streets++;
  }

  fclose(fp);
  return list->size_streets;
}

int find_coordinate(list_map *list, double x_target, double y_target) {
  // convert the input coordinates into strings for the laveshtein distance
  // stuff
  char s_xtarget[16];
  char s_ytarget[16];
  snprintf(s_xtarget, sizeof(s_xtarget), "%f", x_target);
  snprintf(s_ytarget, sizeof(s_ytarget), "%f", y_target);

  node *curr = list->head_streets;
  node *best_match = NULL;
  int best_dist = 999;
  for (int i = 0; i < list->size_streets; i++) {
    char s_xmidpoint[16];
    char s_ymidpoint[16];
    snprintf(s_xmidpoint, sizeof(s_xmidpoint), "%f", curr->street->midX);
    snprintf(s_ymidpoint, sizeof(s_ymidpoint), "%f", curr->street->midY);

    int xDist = LevenshteinDistance(s_xmidpoint, s_xtarget);
    int yDist = LevenshteinDistance(s_xmidpoint, s_xtarget);

    // printf("dist calculated is: %i\n", dist); //DEBUG
    if (xDist + yDist < best_dist) {
      best_dist = xDist + yDist;
      best_match = curr;
    }
    printf("distances: %i + %i = %i\n", xDist, yDist, xDist + yDist);
    curr = curr->next;
  }

  printf("Your coordinates: %f, %f\n", x_target, y_target);
  printf("Closest street to you: %s\n", best_match->name);
  printf("With coordinates between intersections %lli (%f, %f) and %lli (%f, "
         "%f)\n",
         best_match->street->from_intersection_id,
         best_match->street->from_intersection_lat,
         best_match->street->from_intersection_lon,
         best_match->street->to_intersection_id,
         best_match->street->to_intersection_lat,
         best_match->street->to_intersection_lon);

  return (0);
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
  int place_matches = 0;
  node *first_place_match = NULL;

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
    if ((stype == HOUSES && same_street_name(curr->name, street)) ||
        (stype == PLACES && strcmp(curr->name, street) == 0)) {
      street_found = 1;
      matched_street = curr->name;
      // printf("Street's current NUM: %d.\n", curr->house->num); // DEBUG

      if (stype == HOUSES && curr->house->num == num) {
        printf("Found at %lf | %lf\n", curr->lat, curr->lon);
        return 1;
      } else if (stype != HOUSES) {
        if (place_matches == 0) {
          first_place_match = curr;
        }
        place_matches++;
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

  if (stype == PLACES && place_matches == 1) {
    printf("Found at %lf | %lf\n", first_place_match->lat,
           first_place_match->lon);
    return 1;
  }

  if (stype == PLACES && place_matches > 1) {
    int option = 1;
    int selected;

    printf("There is more than one place with this name:\n");
    curr = list->head_places;
    while (curr != NULL) {
      if (strcmp(curr->name, street) == 0) {
        printf("%d. %s (%s) at %lf | %lf\n", option, curr->name,
               curr->place->amenity, curr->lat, curr->lon);
        option++;
      }
      curr = curr->next;
    }

    printf("Choose one: ");
    scanf("%d", &selected);

    if (selected < 1 || selected > place_matches) {
      printf("Wrong input!\n");
      return -1;
    }

    option = 1;
    curr = list->head_places;
    while (curr != NULL) {
      if (strcmp(curr->name, street) == 0) {
        if (option == selected) {
          printf("Found at %lf | %lf\n", curr->lat, curr->lon);
          return 1;
        }
        option++;
      }
      curr = curr->next;
    }
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

      if (choice == 'Y' || choice == 'y') {
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

// ################# HASH RELATED FUNCTIONS ###############

// hash function
int hash_intersection(long long intersection_id) {
  return (int)(intersection_id % TABLE_SIZE);
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

hashmap_entry *find_intersection(hashmap *graph, long long intersection_id,
                                 int index) {

  hashmap_entry *current =
      graph->table[index]; // current element in the bucket, starting at the
                           // first one

  while (current != NULL) {
    if (current->intersection_id == intersection_id) {
      return current; // return bucket where intersection was found
    }
    current = current->next;
  }

  return NULL;
}

void add_street_to_intersection(hashmap *graph, long long intersection_id,
                                node *curr) {

  int index = hash_intersection(intersection_id); // find index bucket

  hashmap_entry *entry = find_intersection(
      graph, intersection_id,
      index); // find a street in the hashmap with the same intersection id

  // if the bucket is null, create the bucket
  if (entry == NULL) {
    entry = malloc(sizeof(hashmap_entry));

    entry->intersection_id = intersection_id;
    entry->connected_streets = NULL;
    entry->next = graph->table[index];
    graph->table[index] = entry;
  }

  // if the bucket is not null, add the new node connected to the element in the
  // bucket since the streets will be connected
  node *new_node = malloc(sizeof(node));
  if (new_node == NULL) {
    return;
  }

  new_node->name = curr->name;
  new_node->street = curr->street;
  new_node->lat = curr->lat;
  new_node->lon = curr->lon;
  new_node->house = NULL;
  new_node->place = NULL;
  new_node->next = entry->connected_streets;

  entry->connected_streets = new_node;
}

// Load streets into the hashmap

void load_streets_into_graph(hashmap *graph, node *head_streets) {

  node *curr = head_streets;

  while (curr != NULL) {

    add_street_to_intersection(graph, curr->street->from_intersection_id, curr);
    add_street_to_intersection(graph, curr->street->to_intersection_id, curr);
    // printf("Loaded %s\n", curr->name); DEBUG LOADING IN HASH MAP
    curr = curr->next;
  }
}

// find connected street in a bucket
node *get_connected_streets(hashmap *graph, long long intersection_id) {

  int index = hash_intersection(intersection_id); // find index bucket

  hashmap_entry *entry = find_intersection(graph, intersection_id, index);

  if (entry == NULL) {
    return NULL;
  }

  return entry->connected_streets;
}

// print connected streets in a bucket
void print_connected_streets(hashmap *graph, long long intersection_id) {
  node *current = get_connected_streets(graph, intersection_id);

  printf("Intersection %lld is connected to:\n", intersection_id);

  while (current != NULL) {
    printf("- %s, length: %lf meters\n", current->name,
           current->street->length_meters);
    current = current->next;
  }
}
