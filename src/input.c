#include "hash.h"
#include "main.h" // already includes input.h so no need for the input.h file
#include "parsing.h"
#include "utils.h"

void ask_map(list_map *list) {

  init_list(list);

  printf("Enter map name (e.g. 'xs_2' or 'xl_1'): ");
  scanf("%s", list->map);

  if (strcmp(list->map, "xs_1") == 0 || strcmp(list->map, "xs_2") == 0 ||
      strcmp(list->map, "md_1") == 0 || strcmp(list->map, "lg_1") == 0 ||
      strcmp(list->map, "xl_1") == 0 || strcmp(list->map, "2xl_1") == 0) {

    int loaded_houses = load_houses(list);
    int loaded_places = load_places(list);
    int loaded_streets = load_streets(list);

    if (loaded_houses == -1 || loaded_places == -1 || loaded_streets == -1) {
      printf("Error loading list\n");
      return;
    }

    printf("Houses loaded: %d\n", loaded_houses);
    printf("Places loaded: %d\n", loaded_places);
    printf("Streets loaded: %d\n", loaded_streets);
    hashmap *graph = create_hashmap(list); // init hashmap, named graph
    load_streets_into_graph(
        graph,
        list->head_streets); // load streets into the hashmap
    list->graph = graph;
  } else {
    printf("Not found\n");
  }
}

// small wrapper for ask_node for the origin/destination
node *ask(list_map *list, int stype) {
  if (stype == ORIGIN)
    printf("--- ORIGIN ---\n");
  else if (stype == DESTINATION)
    printf("--- DESTINATION ---\n");
  return (ask_node(list));
}

node *ask_node(list_map *list) { // pass the malloc from ask_map

  int option;
  printf("Where are you? Address (1), Place (2) or Coordinate (3)?\n");
  scanf("%d", &option);

  if (option != 1 && option != 2 && option != 3)
    printf("Wrong input!\n");
  else if (option == 1) {

    char street[50];

    printf("Enter street name (e.g. 'Carrer de Roc Boronat'): \n");
    scanf(" %49[^\n]", street);

    node *location = find_node(list, street, HOUSES);

    if (location != NULL) {
      find_coordinate(list, location->lon, location->lat, true);
    }

    return location;
  }

  else if (option == 2) {

    char street[256];
    int ch;

    printf("Enter place name (e.g. 'Universitat Pompeu Fabra–Campus del "
           "Poblenou' or 'L'Illa Diagonal'):\n");
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
    read_long_input(street, sizeof(street));

    node *location = find_node(list, street, PLACES);

    if (location != NULL) {
      find_coordinate(list, location->lon, location->lat, true);
    }

    return location;
  }

  else {
    double x_coord, y_coord;

    printf("Enter Y Coordinate (e.g. '41.4134202'):\n");
    scanf(" %lf", &y_coord);
    printf("Enter X Coordinate (e.g. '2.1886874'):\n");
    scanf(" %lf", &x_coord);

    printf("Found at (%f, %f)\n", y_coord, x_coord);
    return (find_coordinate(list, x_coord, y_coord, true));
  }
  return (NULL);
}
