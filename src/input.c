#include <stdio.h>
#include <stdlib.h>
#include "linked_list.h"


int ask_map() {
    
    char map[10];

    printf("Enter map name: ");
    scanf("%s", &map);
    
    return map;
}

house* ask_origin() {

    int origin_num;
    house *newnode = malloc(sizeof(house));

    printf("Where are you? Address (1), Place (2) or Coordinate (3)? ");
    scanf("%d", &origin_num);

    // ask the user for an address and store it in the house linked list.
    if (origin_num == 1) {
        printf("Enter street name (e.g. 'Carrer de Roc Boronat'): ");
        scanf("%s", &newnode->street);

        printf("Enter street number (e.g. '138'): ");
        scanf("%d", &newnode->address_num);
    }

    // if the options are different than one, not implemented.
    else {
        printf("Not implemented yet");
        return 1;
    }

    return newnode;
}

int main() {        

    ask_map();
    ask_origin();

}