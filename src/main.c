#include "main.h"
#include "structs.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

void createaleak() {
  char *foo = malloc(20 * sizeof(char));
  printf("Allocated leaking string: %s", foo);
}

int main() {
  list_map list;
  ask_map(&list);

  if (list.graph == NULL) {
    printf("Graph not initialized\n");
    free_map(&list);
    return 1;
  }

  node *origin = ask(&list, ORIGIN);
  if (!origin) {
    printf("Exiting...\n");
    free_map(&list);
    return 0;
  }

  node *destination = ask(&list, DESTINATION);
  if (!destination) {
    printf("Exiting...\n");
    free_map(&list);
    return 0;
  }

  origin = node_to_street(&list, origin);
  destination = node_to_street(&list, destination);

  if (!origin || !destination) {
    printf("Could not find origin node or destination node");
    free_map(&list);
    return 1;
  }

  queue *path = BFS(list.graph, origin, destination);

  if (!path) {
    printf("Couldn't find a path to the destination");
    free_map(&list);
    return 0;
  }


  print_path(path, origin, destination);
  // DEBUG
  // printf("origin node %s is in street %s\n", origin->name,
  // node_to_street(&list, origin)->name); printf("destination node %s is in
  // street %s\n", destination->name, node_to_street(&list, destination)->name);

  // if (origin && destination)
  // find_route(node *origin, node *destination, &list);
  free_map(&list);
}
