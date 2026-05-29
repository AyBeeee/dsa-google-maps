#include "hash.h"
#include "structs.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void enqueue_path(path_node **head, path_node **tail, node *street,
                  long long intersection_id, path_node *parent) {

  path_node *new_path = malloc(sizeof(path_node));

  new_path->street = street;
  new_path->street->street = street->street;
  new_path->intersection_id = intersection_id;
  new_path->parent = parent;
  new_path->name = street->name;
  new_path->next = NULL;

  if (*head == NULL) {
    *head = new_path;
  }

  if (*tail != NULL) {
    (*tail)->next = new_path;
  }

  *tail = new_path;
}

path_node *dequeue_path(path_node **head, path_node **tail) {
  path_node *curr = *head;

  if (*head == NULL)
    return NULL;

  *head = (*head)->next;

  if (*head == NULL)
    *tail = NULL;

  curr->next = NULL;
  return curr;
}

// idea is to keep going on this while loop until we encounter a new street
// this way we print out less directions (since a lot of streets are long and segmented
// into multiple intersections)
node *next_street(queue *path, node **from, node *to, double *distance)
{
    //temp initialises to the "originally dequeued node"
    node *temp = to;
    while (path)
    {
        //from saves the value in temp, as it's currently the furthestmost point of the long street
        free(*from);
        *from = temp;
        temp = dequeue(path);
        if (!temp || strcmp((*from)->name, temp->name) != 0)
            break;

        *distance += temp->street->length_meters;

    }
    //this always returns either NULL or the next street we need to get on. from is now always the furthestmost point of the long street
    return temp;
}

void hard_printpath(queue *path)
{
    while (path)
    {
        node *temp = dequeue(path);
        if (!temp)
            return ;
        printf("HARD PRINT: NODE IS %s, FROM ID %lli, TO ID%lli\n", temp->name, temp->street->from_intersection_id, temp->street->to_intersection_id);
    }
}

void print_path(queue *path, node *origin, node *destination)
{
    node *from;
    node *to;
    int direction;
    double distance;

    //hard_printpath(path); //DEBUG

    from = dequeue(path);
    to = NULL;
    printf("Start at %s\n", origin->name);
    while (path && from)
    {
        to = dequeue(path);
        if (!to)
            break;
        distance = from->street->length_meters;

        //if we need to keep going on the same street
        if (strcmp(from->name, to->name) == 0)
            to = next_street(path, &from, to, &distance);

        //truncate the decimals, we really dont care about centimeters
        printf("Walk on %s for %im", from->name, (int)distance);

        direction = cross_product(from, to);
        if (direction == RIGHT)
            printf(", then turn right towards ");
        else if (direction == LEFT)
            printf(", then turn left towards ");
        else
            printf("take a plane or something idk, to "); //error message lol

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

void build_path(queue *path, path_node *curr) {
  if (curr == NULL) {
    return;
  }

  //printf("curr in build path is %s\n", curr->name);
  // build path in a recursive way by enqueueing
  build_path(path, curr->parent);
  enqueue(path, curr->street);
}



hashmap *init_visited(hashmap *visited, int table_size)
{
    visited = malloc(sizeof(hashmap));
    visited->table_size = table_size;
    visited->table = malloc(visited->table_size * sizeof(intersection));
    if (visited->table == NULL)
      return NULL;
    for (int i = 0; i < visited->table_size; i++)
      visited->table[i] = NULL;

    return visited;
}

queue *BFS(hashmap *graph, node *origin, node *destination)
{
  queue *path = malloc(sizeof(queue));
  hashmap *visited = NULL;

  path_node *explore_head = NULL;
  path_node *explore_tail = NULL;
  long long origin_intersection = origin->street->from_intersection_id;

  init_queue(path);
  visited = init_visited(visited, graph->table_size);

  // mark all possible paths from starting intersection to be explored
  node *head_street = get_connected_streets(graph, origin_intersection);
  while (head_street != NULL)
  {
    enqueue_path(&explore_head, &explore_tail, head_street, next_intersection(head_street, origin_intersection), NULL);
    head_street = head_street->next;
  }

  //loop BFA alg until there's nothing else left to explore
  while (explore_head != NULL)
  {
    //grab a path to explore from the "to explore" queue, and then grab the street from it
    path_node *curr_path = dequeue_path(&explore_head, &explore_tail);
    node *curr_street = curr_path->street;

    //if current street is our destination, we found the correct path
    if (strcmp(curr_street->name, destination->name) == 0) {
      build_path(path, curr_path);
      free_hash(visited);
      //TODO maybe free here
      return path;
    }

    //check whether the from_intersection or to_intersection are not visited
    int which_missing = node_in_visited(visited, curr_street);
    if (which_missing != NONE)
    {
        //mark them as visited depending on which ones weren't already
        if (which_missing == BOTH || which_missing == TO)
            add_street_to_intersection(visited, curr_street->street->to_intersection_id, curr_street->street->to_intersection_lat, curr_street->street->to_intersection_lon, curr_street);
        if (which_missing == BOTH || which_missing == FROM)
            add_street_to_intersection(visited, curr_street->street->from_intersection_id, curr_street->street->from_intersection_lat, curr_street->street->from_intersection_lon, curr_street);

        //grab the intersection's bucket, and iterate through all nodes, marking them as explored if not already
        node *connected_street = get_connected_streets(graph, curr_path->intersection_id);
        while (connected_street != NULL)
        {
            int which_missing = node_in_visited(visited, connected_street);
            if (which_missing != NONE)
            {
                enqueue_path(
                    &explore_head, &explore_tail, connected_street,
                    next_intersection(connected_street, curr_path->intersection_id),
                    curr_path);
            }
            connected_street = connected_street->next;
        }
    }
  }
  if (visited)
      free_hash(visited);

  return NULL;
}
