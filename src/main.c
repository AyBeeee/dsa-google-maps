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
  node *origin = ask(&list, ORIGIN);
  if (!origin)
    printf("Exiting...\n");
  node *destination = ask(&list, DESTINATION);
  if (!destination)
    printf("Exiting...\n");
  origin = node_to_street(&list, origin);
  destination = node_to_street(&list, destination);
  queue *path = BFS(list.graph, origin, destination);
  print_path(path, origin, destination);
  // DEBUG
  // printf("origin node %s is in street %s\n", origin->name,
  // node_to_street(&list, origin)->name); printf("destination node %s is in
  // street %s\n", destination->name, node_to_street(&list, destination)->name);

  // if (origin && destination)
  // find_route(node *origin, node *destination, &list);
  free_map(&list);
}
