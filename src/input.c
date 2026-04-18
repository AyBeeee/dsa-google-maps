#include "main.h" // already includes input.h so no need for the input.h file
#include "parsing.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void free_map(list_map *list) {
  node *temp;
  while (list->head_houses) {
    temp = list->head_houses->next;
    free(list->head_houses->house);
    free(list->head_houses);
    list->head_houses = temp;
  }
  while (list->head_places) {
    temp = list->head_places->next;
    free(list->head_places->place);
    free(list->head_places);
    list->head_places = temp;
  }
  // TODO add for streets eventually
}

void ask_map() {

  list_map list;
  list.head_houses = NULL;
  list.tail_houses = NULL;
  list.size_houses = 0;

  list.head_places = NULL;
  list.tail_places = NULL;
  list.size_places = 0;

  printf("Enter map name (e.g. 'xs_2' or 'xl_1'): ");
  scanf("%s", list.map);

  if (strcmp(list.map, "xs_1") == 0 || strcmp(list.map, "xs_2") == 0 ||
      strcmp(list.map, "md_1") == 0 || strcmp(list.map, "lg_1") == 0 ||
      strcmp(list.map, "xl_1") == 0 || strcmp(list.map, "2xl_1") == 0) {

    int loaded_houses = load_houses(&list);
    int loaded_places = load_places(&list);

    if (loaded_houses == -1 || loaded_places == -1) {
      printf("No list loaded\n");
      return;
    }

    printf("Houses loaded: %d\n", loaded_houses);
    printf("Places loaded: %d\n", loaded_houses);
    ask_address(&list);
  }

  else {
    printf("Not found\n");
  }
  free_map(&list);
}

void ask_address(list_map *list) { // pass the malloc from ask_map

  int option;
  printf("--- ORIGIN ---\n");
  printf("Where are you? Address (1), Place (2) or Coordinate (3)?\n");
  scanf("%d", &option);

  if (option == 1) {

    char street[50];

    printf("Enter street name (e.g. 'Carrer de Roc Boronat'): \n");
    scanf(" %49[^\n]", street);

    find_node(list, street, HOUSES);
  }

  else if (option == 2) {

    char street[50];

    printf("Enter place name (e.g. 'Universitat Pompeu Fabra–Campus del "
           "Poblenou' or 'L'Illa Diagonal'):\n");
    scanf(" %49[^\n]", street);

    find_node(list, street, PLACES);
  }

  else {
    printf("Not implemented yet\n");
  }
}
