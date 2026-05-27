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
  new_path->intersection_id = intersection_id;
  new_path->parent = parent;
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

  // build path in a recursive way by enqueueing
  build_path(path, curr->parent);
  enqueue(path, curr->street);
}

queue *BFS(hashmap *graph, node *origin, node *destination) {
  queue *path = malloc(sizeof(queue));
  queue *visited = malloc(sizeof(queue));
  path_node *explore_head = NULL;
  path_node *explore_tail = NULL;
  long long origin_intersection = origin->street->from_intersection_id;

  init_queue(path);
  init_queue(visited);

  node *head_street = get_connected_streets(graph, origin_intersection);

  while (head_street != NULL) {
    enqueue_path(&explore_head, &explore_tail, head_street,
                 next_intersection(head_street, origin_intersection), NULL);
    head_street = head_street->next;
  }

  while (explore_head != NULL) {
    path_node *curr_path = dequeue_path(&explore_head, &explore_tail);
    node *curr_street = curr_path->street;

    if (curr_street->street == destination->street ||
        strcmp(curr_street->name, destination->name) == 0) {
      build_path(path, curr_path);
      return path;
    }

    if (node_in_queue(visited, curr_street) == NULL) {
      enqueue(visited, curr_street);

      node *connected_street =
          get_connected_streets(graph, curr_path->intersection_id);

      while (connected_street != NULL) {
        if (node_in_queue(visited, connected_street) == NULL) {
          enqueue_path(
              &explore_head, &explore_tail, connected_street,
              next_intersection(connected_street, curr_path->intersection_id),
              curr_path);
        }
        connected_street = connected_street->next;
      }
    }
  }

  return NULL;
}
