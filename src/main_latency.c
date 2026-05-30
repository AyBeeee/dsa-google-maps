#include "hash.h"
#include "main.h"
#include "parsing.h"
#include "structs.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct pending_path {
  node *street;
  long long intersection_id;
  double distance;
  struct pending_path *next;
} pending_path;

hashmap *create_empty_hashmap(int table_size) {
  hashmap *map = malloc(sizeof(hashmap));

  if (map == NULL)
    return NULL;

  map->table_size = table_size;
  map->table = malloc(table_size * sizeof(intersection *));
  if (map->table == NULL) {
    free(map);
    return NULL;
  }

  for (int i = 0; i < table_size; i++)
    map->table[i] = NULL;

  return map;
}

void add_pending_path(pending_path **head, node *street, double distance) {
  pending_path *new_path = malloc(sizeof(pending_path));

  if (new_path == NULL)
    return;

  new_path->street = street;
  new_path->intersection_id = street->street->to_intersection_id;
  new_path->distance = distance;
  new_path->next = NULL;

  if (*head == NULL || distance < (*head)->distance) {
    new_path->next = *head;
    *head = new_path;
    return;
  }

  pending_path *current = *head;
  while (current->next != NULL && current->next->distance <= distance)
    current = current->next;

  new_path->next = current->next;
  current->next = new_path;
}

pending_path *pop_pending_path(pending_path **head) {
  pending_path *current = *head;

  if (*head == NULL)
    return NULL;

  *head = (*head)->next;
  current->next = NULL;
  return current;
}

void free_pending_paths(pending_path *head) {
  while (head != NULL) {
    pending_path *next = head->next;
    free(head);
    head = next;
  }
}

int latency_street_in_visited(hashmap *visited, node *street) {
  node *current =
      get_connected_streets(visited, street->street->from_intersection_id);

  while (current != NULL) {
    if (current->street == street->street)
      return 1;
    current = current->next;
  }

  return 0;
}

int dijkstra_path_exists(list_map *list, node *origin, node *destination,
                         int use_intersections_map) {
  hashmap *visited = create_empty_hashmap(list->graph->table_size);
  pending_path *pending = NULL;

  if (visited == NULL)
    return 0;

  add_pending_path(&pending, origin, 0.0);

  while (pending != NULL) {
    pending_path *current_path = pop_pending_path(&pending);
    node *current_street = current_path->street;

    if (current_street->street == destination->street) {
      free(current_path);
      free_pending_paths(pending);
      free_hash(visited);
      return 1;
    }

    if (!latency_street_in_visited(visited, current_street)) {
      add_street_to_intersection(
          visited, current_street->street->from_intersection_id,
          current_street->street->from_intersection_lat,
          current_street->street->from_intersection_lon, current_street);

      if (use_intersections_map) {
        node *connected =
            get_connected_streets(list->graph, current_path->intersection_id);

        while (connected != NULL) {
          if (!latency_street_in_visited(visited, connected)) {
            add_pending_path(&pending, connected,
                             current_path->distance +
                                 connected->street->length_meters);
          }
          connected = connected->next;
        }
      } else {
        node *connected = list->head_streets;

        while (connected != NULL) {
          if (connected->street->from_intersection_id ==
                  current_path->intersection_id &&
              !latency_street_in_visited(visited, connected)) {
            add_pending_path(&pending, connected,
                             current_path->distance +
                                 connected->street->length_meters);
          }
          connected = connected->next;
        }
      }
    }

    free(current_path);
  }

  free_hash(visited);
  return 0;
}

int main() {
  char *q4_maps[] = {"xs_1", "xs_2", "md_1", "lg_1", "xl_1", "2xl_1"};
  int q4_number_of_maps = 6;

  printf("\n--- QUESTION 4: CONNECTED STREETS ---\n");

  for (int map_index = 0; map_index < q4_number_of_maps; map_index++) {
    list_map list;
    node *middle_street;
    long long intersection_id;

    init_list(&list);
    strcpy(list.map, q4_maps[map_index]);

    load_streets(&list);
    list.graph = create_hashmap(&list);
    load_streets_into_graph(list.graph, list.head_streets);

    middle_street = list.head_streets;
    for (int i = 0; i < list.size_streets / 2; i++)
      middle_street = middle_street->next;

    intersection_id = middle_street->street->from_intersection_id;

    printf("\nMap: %s (%d street segments)\n", list.map, list.size_streets);
    printf("Intersection tested: %lld\n", intersection_id);

    printf("Sequential list search:\n");
    for (int test = 1; test <= 10; test++) {
      struct timespec start, end;
      long long elapsed_ns;
      int connected_count = 0;

      clock_gettime(CLOCK_MONOTONIC, &start);

      node *current = list.head_streets;
      while (current != NULL) {
        if (current->street->from_intersection_id == intersection_id)
          connected_count++;
        current = current->next;
      }

      clock_gettime(CLOCK_MONOTONIC, &end);

      elapsed_ns = (end.tv_sec - start.tv_sec) * 1000000000LL +
                   (end.tv_nsec - start.tv_nsec);

      printf("  Test %d: %.6f ms (%d connected streets)\n", test,
             elapsed_ns / 1000000.0, connected_count);
    }

    printf("Intersections map search:\n");
    for (int test = 1; test <= 10; test++) {
      struct timespec start, end;
      long long elapsed_ns;
      int connected_count = 0;

      clock_gettime(CLOCK_MONOTONIC, &start);

      node *current = get_connected_streets(list.graph, intersection_id);
      while (current != NULL) {
        connected_count++;
        current = current->next;
      }

      clock_gettime(CLOCK_MONOTONIC, &end);

      elapsed_ns = (end.tv_sec - start.tv_sec) * 1000000000LL +
                   (end.tv_nsec - start.tv_nsec);

      printf("  Test %d: %.6f ms (%d connected streets)\n", test,
             elapsed_ns / 1000000.0, connected_count);
    }

    free_map(&list);
  }

  char *q5_maps[] = {"xs_2", "md_1", "lg_1", "xl_1", "2xl_1"};
  int q5_number_of_maps = 5;

  printf("\n--- QUESTION 5: PATH FINDING ---\n");
  printf("Same coordinates are used in every map:\n");
  printf("Origin: 41.401801, 2.196913\n");
  printf("Destination: 41.404192, 2.195074\n");

  for (int map_index = 0; map_index < q5_number_of_maps; map_index++) {
    list_map list;
    node *origin;
    node *destination;

    init_list(&list);
    strcpy(list.map, q5_maps[map_index]);

    load_streets(&list);
    list.graph = create_hashmap(&list);
    load_streets_into_graph(list.graph, list.head_streets);

    origin = find_coordinate(&list, 2.196913, 41.401801, false);
    destination = find_coordinate(&list, 2.195074, 41.404192, false);

    printf("\nMap: %s (%d street segments)\n", list.map, list.size_streets);
    printf("Origin street: %s\n", origin->name);
    printf("Destination street: %s\n", destination->name);

    printf("Path finding with sequential connected-street search:\n");
    for (int test = 1; test <= 10; test++) {
      struct timespec start, end;
      long long elapsed_ns;
      int found;

      clock_gettime(CLOCK_MONOTONIC, &start);

      found = dijkstra_path_exists(&list, origin, destination, 0);

      clock_gettime(CLOCK_MONOTONIC, &end);

      elapsed_ns = (end.tv_sec - start.tv_sec) * 1000000000LL +
                   (end.tv_nsec - start.tv_nsec);

      printf("  Test %d: %.6f ms (path found: %d)\n", test,
             elapsed_ns / 1000000.0, found);
    }

    printf("Path finding with intersections map search:\n");
    for (int test = 1; test <= 10; test++) {
      struct timespec start, end;
      long long elapsed_ns;
      int found;

      clock_gettime(CLOCK_MONOTONIC, &start);

      found = dijkstra_path_exists(&list, origin, destination, 1);

      clock_gettime(CLOCK_MONOTONIC, &end);

      elapsed_ns = (end.tv_sec - start.tv_sec) * 1000000000LL +
                   (end.tv_nsec - start.tv_nsec);

      printf("  Test %d: %.6f ms (path found: %d)\n", test,
             elapsed_ns / 1000000.0, found);
    }

    free_map(&list);
  }

  printf("\n--- QUESTION 6: PATH FINDING BY DISTANCE ---\n");
  printf("Same map is used for every test: xl_1\n");
  printf("Origin: 41.403429, 2.193660\n");

  list_map list;
  node *origin;
  double destination_lat[] = {41.404986, 41.391900, 41.389559};
  double destination_lon[] = {2.194211, 2.170000, 2.135112};
  char *distance_name[] = {"close", "medium", "far"};
  int number_of_distances = 3;

  init_list(&list);
  strcpy(list.map, "xl_1");

  load_streets(&list);
  list.graph = create_hashmap(&list);
  load_streets_into_graph(list.graph, list.head_streets);

  origin = find_coordinate(&list, 2.193660, 41.403429, false);

  for (int distance_index = 0; distance_index < number_of_distances;
       distance_index++) {
    node *destination = find_coordinate(&list, destination_lon[distance_index],
                                        destination_lat[distance_index], false);
    double distance_meters = haversine(origin->lat, origin->lon,
                                       destination->lat, destination->lon) *
                             1000.0;

    printf("\nDistance case: %s\n", distance_name[distance_index]);
    printf("Destination: %.6f, %.6f\n", destination_lat[distance_index],
           destination_lon[distance_index]);
    printf("Approximate distance: %.2f meters\n", distance_meters);
    printf("Origin street: %s\n", origin->name);
    printf("Destination street: %s\n", destination->name);

    printf("Path finding with sequential connected-street search:\n");
    for (int test = 1; test <= 10; test++) {
      struct timespec start, end;
      long long elapsed_ns;
      int found;

      clock_gettime(CLOCK_MONOTONIC, &start);

      found = dijkstra_path_exists(&list, origin, destination, 0);

      clock_gettime(CLOCK_MONOTONIC, &end);

      elapsed_ns = (end.tv_sec - start.tv_sec) * 1000000000LL +
                   (end.tv_nsec - start.tv_nsec);

      printf("  Test %d: %.6f ms (path found: %d)\n", test,
             elapsed_ns / 1000000.0, found);
    }

    printf("Path finding with intersections map search:\n");
    for (int test = 1; test <= 10; test++) {
      struct timespec start, end;
      long long elapsed_ns;
      int found;

      clock_gettime(CLOCK_MONOTONIC, &start);

      found = dijkstra_path_exists(&list, origin, destination, 1);

      clock_gettime(CLOCK_MONOTONIC, &end);

      elapsed_ns = (end.tv_sec - start.tv_sec) * 1000000000LL +
                   (end.tv_nsec - start.tv_nsec);

      printf("  Test %d: %.6f ms (path found: %d)\n", test,
             elapsed_ns / 1000000.0, found);
    }
  }

  free_map(&list);

  return 0;
}
