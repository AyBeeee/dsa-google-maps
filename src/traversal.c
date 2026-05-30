#include "hash.h"
#include "structs.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
function for debugging commented bc not needed

void hard_printpath(queue *path) {
  while (path) {
    node *temp = dequeue(path);
    if (!temp)
      return;
    printf("HARD PRINT: NODE IS %s, FROM ID %lli, TO ID%lli\n", temp->name,
           temp->street->from_intersection_id,
           temp->street->to_intersection_id);
  }
}
*/

void print_path(queue *path, node *origin, node *destination) {
  node *from = dequeue(path);
  node *to = dequeue(path);

  // hard_printpath(path); //DEBUG

  printf("\n--- ROUTE ---\n");
  printf("Start at %s\n", origin->name);

  while (from != NULL && to != NULL) {

    // calculate the distance of the street and the cross product to know where to turn
    int direction = cross_product(from, to);
    double distance = to->street->length_meters;

    // combine consecutive segments belonging to the same street
    node *current = to;
    node *next = dequeue(path);

    // add distance if the street is the same (we do this to avoid the segmentations of the streets)
    while (next != NULL && strcmp(current->name, next->name) == 0) {
      distance += next->street->length_meters;
      free(current);
      current = next;
      next = dequeue(path);
    }


    if (direction == RIGHT)
      printf("turn right to ");
    else if (direction == LEFT)
      printf("turn left to ");
    else
      printf("continue to ");

    // truncate the decimals, we really dont care about centimeters
    printf("%s and continue for %im\n", current->name, (int)distance);

    free(from);
    from = current;
    to = next;
    
  }
  free(from);
  free(path);
  printf("You have arrived at %s!\n", destination->name);
}

// helper function to make the BFS the tiniest bit less ugly
//  checks indiscriminately from and to ids to save lines
bool street_in_visited(hashmap *visited, node *street) {
  node *curr =
      get_connected_streets(visited, street->street->from_intersection_id);

  while (curr != NULL) {
    if (curr->street == street->street)
      return true;

    curr = curr->next;
  }

  return false;
}

hashmap *init_visited(hashmap *visited, int table_size) {
  visited = malloc(sizeof(hashmap));
  visited->table_size = table_size;
  visited->table = malloc(visited->table_size * sizeof(intersection *));
  if (visited->table == NULL)
    return NULL;
  for (int i = 0; i < visited->table_size; i++)
    visited->table[i] = NULL;

  return visited;
}

void add_path_to_explore(explore_node **curr, node **connected_street,
                         explore_node **head, explore_node **tail) {
  // this is a bit heavy on the memory, but it's way easier to free later
  queue *new_path = copy_queue((*curr)->path_so_far);
  enqueue(new_path, (*connected_street));

  explore_node *en = malloc(sizeof(explore_node));
  en->street = (*connected_street);
  en->intersection_id = (*connected_street)->street->to_intersection_id;
  en->path_so_far = new_path;
  en->next = NULL;
  if ((*head) == NULL)
    (*head) = en;
  if ((*tail) != NULL)
    (*tail)->next = en;
  (*tail) = en;
}

queue *BFS(hashmap *graph, node *origin, node *destination) {
  hashmap *visited = NULL;

  explore_node *explore_head = NULL;
  explore_node *explore_tail = NULL;

  visited = init_visited(visited, graph->table_size);

  // mark all possible paths from starting intersection to be explored
  queue *initial_path = malloc(sizeof(queue));
  init_queue(initial_path);
  enqueue(initial_path, origin);

  explore_node *initial = malloc(sizeof(explore_node));
  initial->street = origin;
  initial->intersection_id = origin->street->to_intersection_id;
  initial->path_so_far = initial_path;
  initial->next = NULL;

  explore_head = initial;
  explore_tail = initial;

  // loop BFS until there's nothing left to explore
  while (explore_head != NULL) {
    // grab a path to explore from the "to explore" queue, use it for this
    // iteration.
    explore_node *curr = explore_head;
    // this is changing the head of the explore queues to the next queue, so the
    // next iteration correctly grabs a different path
    explore_head = explore_head->next;
    if (explore_head == NULL)
      explore_tail = NULL;

    node *curr_street = curr->street;

    // if current street is our destination, we found the correct path, return
  if (curr_street->street == destination->street) {
      queue *result = curr->path_so_far;
      // make sure we have no leaks (fucking hell)
      free(curr);
      while (explore_head != NULL) {
        explore_node *temp = explore_head;
        explore_head = explore_head->next;
        free_queue(temp->path_so_far);
        free(temp);
      }
      free_hash(visited);
      return result;
    }

    // check whether the from_intersection or to_intersection are not visited
    if (!street_in_visited(visited, curr_street)) {
      add_street_to_intersection(
          visited, curr_street->street->from_intersection_id,
          curr_street->street->from_intersection_lat,
          curr_street->street->from_intersection_lon, curr_street);

      node *connected_street =
          get_connected_streets(graph, curr->intersection_id);

      while (connected_street != NULL) {
        if (!street_in_visited(visited, connected_street))
          add_path_to_explore(&curr, &connected_street, &explore_head,
                              &explore_tail);

        connected_street = connected_street->next;
      }
    }
    // if all nodes in current path have already been explored, free the whole
    // queue
    free_queue(curr->path_so_far);
    free(curr);
  }

  free_hash(visited);
  return NULL;
}
