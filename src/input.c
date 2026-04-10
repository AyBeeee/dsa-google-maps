#include <stdio.h>
#include <stdlib.h>
#include "main.h" // already includes input.h so no need for the input.h file
#include "parsing.h"
#include <string.h>

list* ask_map() {

  list *house = malloc(sizeof(house));
  house->head = NULL;
  house->tail = NULL;
  house->size = 0;

  printf("Enter map name (e.g. 'xs_2' or 'xl_1'): ");
  scanf("%s", house->map);

  if (strcmp(house->map, "xs_1") == 0 || strcmp(house->map, "xs_2") == 0 ||
  strcmp(house->map, "md_1") == 0 || strcmp(house->map, "lg_1") == 0 ||
  strcmp(house->map, "xl_1") == 0 || strcmp(house->map, "2xl_1") == 0) {

    int loaded = load_map(house);

    if (loaded == -1) {
        printf("No list loaded\n");
        return NULL;
    }

    printf("list loaded %d", loaded);
    printf("\n");
    ask_address(house);
  }

  else {
    printf("Not found\n");
    free(house);
    return NULL;
  }

  return house;
}

void ask_address(list *house) { // pass the malloc from map

  int option;
  printf("--- ORIGIN ---\n");
  printf("Where are you? Address (1), Place (2) or Coordinate (3)?\n");
  scanf("%d", &option);

  if (option == 1) {

      char street[50];
      int num;

      printf("Enter street name (e.g. 'Carrer de Roc Boronat'): \n");
      scanf(" %49[^\n]", street);

      printf("Enter street number (e.g. '138'): \n");
      scanf("%d", &num);

      find_address(house, street, num);
  }

  else {
      printf("Not implemented yet\n");
  }
}
