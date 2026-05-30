#include "hash.h"
#include "structs.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// idea is to keep going on this while loop until we encounter a new street
// this way we print out less directions (since a lot of streets are long and
// segmented into multiple intersections)
node *next_street(queue *path, node **from, node *to, double *distance) {
  // temp initialises to the "originally dequeued node"
  node *temp = to;
  while (path) {
    // from saves the value in temp, as it's currently the furthestmost point of
    // the long street
    free(*from);
    *from = temp;
    temp = dequeue(path);
    if (!temp || strcmp((*from)->name, temp->name) != 0)
      break;

    *distance += temp->street->length_meters;
  }
  // this always returns either NULL or the next street we need to get on. from
  // is now always the furthestmost point of the long street
  return temp;
}

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

void print_path(queue *path, node *origin, node *destination) {
  node *from;
  node *to;
  int direction;
  double distance;

  // hard_printpath(path); //DEBUG

  from = dequeue(path);
  to = NULL;
  printf("Start at %s\n", origin->name);
  while (path && from) {
    to = dequeue(path);
    if (!to)
      break;
    distance = from->street->length_meters;

    // if we need to keep going on the same street
    if (strcmp(from->name, to->name) == 0)
      to = next_street(path, &from, to, &distance);

    // truncate the decimals, we really dont care about centimeters
    printf("Walk on %s for %im", from->name, (int)distance);

    direction = cross_product(from, to);
    if (direction == RIGHT)
      printf(", then turn right towards ");
    else if (direction == LEFT)
      printf(", then turn left towards ");
    else
      printf("take a plane or something idk, to "); // error message lol

    printf("%s.\n", to->name);

    free(from);
    from = to;
  }
  free(from);
  free(path);
  printf("You have arrived at %s!\n", destination->name);
}

long long next_intersection(node *street, long long current_intersection) {
  if (street->street->from_intersection_id == current_intersection) {
    return street->street->to_intersection_id;
  }

  return street->street->from_intersection_id;
}

// helper function to make the BFS the tiniest bit less ugly
//  checks indiscriminately from and to ids to save lines
int node_in_visited(hashmap *visited, node *curr) {
  int missing = 0;
  int from_index =
      hash_intersection(visited, curr->street->from_intersection_id);
  int to_index = hash_intersection(visited, curr->street->to_intersection_id);

  if (in_intersection(visited, from_index,
                      curr->street->from_intersection_id) == NULL)
    missing++;

  if (in_intersection(visited, to_index, curr->street->to_intersection_id) ==
      NULL) {
    if (missing)
      return BOTH;
    else
      return TO;
  }
  if (missing)
    return FROM;
  return NONE;
}

hashmap *init_visited(hashmap *visited, int table_size) {
  visited = malloc(sizeof(hashmap));
  visited->table_size = table_size;
  visited->table = malloc(visited->table_size * sizeof(intersection));
  if (visited->table == NULL)
    return NULL;
  for (int i = 0; i < visited->table_size; i++)
    visited->table[i] = NULL;

  return visited;
}

// init one queue for all possible starting exploration paths
void init_exploration(explore_node **explore_head, explore_node **explore_tail,
                      node **head_street, long long origin_intersection) {
  while ((*head_street) != NULL) {
    queue *initial_path = malloc(sizeof(queue));
    init_queue(initial_path);
    enqueue(initial_path, (*head_street));

    explore_node *en = malloc(sizeof(explore_node));
    en->street = (*head_street);
    en->intersection_id =
        next_intersection((*head_street), origin_intersection);
    en->path_so_far = initial_path;
    en->next = NULL;
    if ((*explore_head) == NULL)
      (*explore_head) = en;
    if ((*explore_tail) != NULL)
      (*explore_tail)->next = en;
    (*explore_tail) = en;

    (*head_street) = (*head_street)->next;
  }
}

void add_path_to_explore(explore_node **curr, node **connected_street,
                         explore_node **head, explore_node **tail) {
  // this is a bit heavy on the memory, but it's way easier to free later
  queue *new_path = copy_queue((*curr)->path_so_far);
  enqueue(new_path, (*connected_street));

  explore_node *en = malloc(sizeof(explore_node));
  en->street = (*connected_street);
  en->intersection_id =
      next_intersection((*connected_street), (*curr)->intersection_id);
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
  node *head_street =
      get_connected_streets(graph, origin->street->from_intersection_id);
  init_exploration(&explore_head, &explore_tail, &head_street,
                   origin->street->from_intersection_id);
  head_street =
      get_connected_streets(graph, origin->street->to_intersection_id);
  init_exploration(&explore_head, &explore_tail, &head_street,
                   origin->street->to_intersection_id);

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
    if (strcmp(curr_street->name, destination->name) == 0) {
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
    int which_missing = node_in_visited(visited, curr_street);
    if (which_missing != NONE) {
      // mark them as visited depending on which ones weren't already
      if (which_missing == BOTH || which_missing == TO) {
        add_street_to_intersection(
            visited, curr_street->street->to_intersection_id,
            curr_street->street->to_intersection_lat,
            curr_street->street->to_intersection_lon, curr_street);
      }
      if (which_missing == BOTH || which_missing == FROM) {
        add_street_to_intersection(
            visited, curr_street->street->from_intersection_id,
            curr_street->street->from_intersection_lat,
            curr_street->street->from_intersection_lon, curr_street);
      }

      // grab the intersection's bucket, and iterate through all nodes, marking
      // them as TO BE explored if not already
      node *connected_street =
          get_connected_streets(graph, curr->intersection_id);
      while (connected_street != NULL) {
        int which_missing = node_in_visited(visited, connected_street);
        // this shit's important: make sure we create a new path for each of
        // these branching decisions
        if (which_missing != NONE)
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
