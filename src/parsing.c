#include "parsing.h"
#include "utils.h"
#include <stdio.h>

// builds the houses linked list, allocating, assigning variables, and linking
// each node
int load_houses(list_map *list) {
  // create route
  char route[256] = "maps/";
  strcat(route, list->map);
  strcat(route, "/houses.txt");
  // printf("Current route: %s\n", route); // DEBUG

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
    sscanf(line, "%255[^,],%d,%lf,%lf", newnode->name, &(newnode->house->num),
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
  // printf("Current route: %s\n", route); // DEBUG

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
    sscanf(line, "%255[^,],%[^,],%255[^,],%lf,%lf", newnode->place->id,
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
  list->size_streets = 0;
  strcat(route, list->map);
  strcat(route, "/streets.txt");
  // printf("Current route: %s\n", route);

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
    sscanf(line, "%lld, %lf, %lf, %lld, %lf, %lf, %lf, %255[^\n]",
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
    newnode->lat = newnode->street->midY;
    newnode->lon = newnode->street->midX;

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
