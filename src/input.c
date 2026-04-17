#include "main.h" // already includes input.h so no need for the input.h file
#include "parsing.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void free_list(list *house_list) {
  node *temp;
  while (house_list->head) {
    temp = house_list->head->next;
    free(house_list->head);
    house_list->head = temp;
  }
}

void ask_map() {

  list house_list;
  house_list.head = NULL;
  house_list.tail = NULL;
  house_list.size = 0;
  
  list list_places;
  list_places.head = NULL;
  list_places.tail = NULL;
  list_places.size = 0;

  printf("Enter map name (e.g. 'xs_2' or 'xl_1'): ");
  scanf("%s", house_list.map);

  if (strcmp(house_list.map, "xs_1") == 0 ||
      strcmp(house_list.map, "xs_2") == 0 ||
      strcmp(house_list.map, "md_1") == 0 ||
      strcmp(house_list.map, "lg_1") == 0 ||
      strcmp(house_list.map, "xl_1") == 0 ||
      strcmp(house_list.map, "2xl_1") == 0) {

    int loaded = load_map(&house_list);
    int loaded_places = load_places(&list_places);

    if (loaded == -1 || loaded_places == -1) {
      printf("No list loaded\n");
      return;
    }

    printf("list loaded %d", loaded);
    printf("\n");
    ask_address(&house_list);
  }

  else {
    printf("Not found\n");
  }
  free_list(&house_list);
}

void ask_address(list *house_list) { // pass the malloc from map

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

    find_address(house_list, street, num);
  }

  else if(option == 2){

    char street[50];
    int num;

    printf("Enter place name (e.g. 'Universitat Pompeu Fabra–Campus del Poblenou' or 'L'Illa Diagonal'):\n");
    scanf(" %49[^\n]", street);

    printf("Enter street number (e.g. '138'): \n");
    scanf("%d", &num);

    find_places(house_list, street, num);
  }

  else {
    printf("Not implemented yet\n");
  }

}
