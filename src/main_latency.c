#include "hash.h"
#include "main.h"
#include "parsing.h"
#include "structs.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

long long other_intersection(node *street, long long intersection_id) {
  if (street->street->from_intersection_id == intersection_id)
    return street->street->to_intersection_id;
  return street->street->from_intersection_id;
}

int was_visited(data_streets **visited, int visited_count,
                data_streets *street) {
  for (int i = 0; i < visited_count; i++) {
    if (visited[i] == street)
      return 1;
  }
  return 0;
}

int path_exists(list_map *list, node *origin, node *destination,
                int use_intersections_map) {
  int max_streets = list->size_streets + 1;
  node **queue = malloc(max_streets * sizeof(node *));
  long long *queue_intersections = malloc(max_streets * sizeof(long long));
  data_streets **visited = malloc(max_streets * sizeof(data_streets *));
  int queue_start = 0;
  int queue_end = 0;
  int visited_count = 0;
  long long start_intersection = origin->street->from_intersection_id;

  if (origin->street == destination->street) {
    free(queue);
    free(queue_intersections);
    free(visited);
    return 1;
  }

  if (use_intersections_map) {
    node *current = get_connected_streets(list->graph, start_intersection);
    while (current != NULL) {
      if (!was_visited(visited, visited_count, current->street)) {
        visited[visited_count++] = current->street;
        queue[queue_end] = current;
        queue_intersections[queue_end++] =
            other_intersection(current, start_intersection);
      }
      current = current->next;
    }
  } else {
    node *current = list->head_streets;
    while (current != NULL) {
      if ((current->street->from_intersection_id == start_intersection ||
           current->street->to_intersection_id == start_intersection) &&
          !was_visited(visited, visited_count, current->street)) {
        visited[visited_count++] = current->street;
        queue[queue_end] = current;
        queue_intersections[queue_end++] =
            other_intersection(current, start_intersection);
      }
      current = current->next;
    }
  }

  while (queue_start < queue_end) {
    node *current_street = queue[queue_start];
    long long current_intersection = queue_intersections[queue_start];
    queue_start++;

    if (current_street->street == destination->street) {
      free(queue);
      free(queue_intersections);
      free(visited);
      return 1;
    }

    if (use_intersections_map) {
      node *current = get_connected_streets(list->graph, current_intersection);
      while (current != NULL) {
        if (!was_visited(visited, visited_count, current->street) &&
            queue_end < max_streets) {
          visited[visited_count++] = current->street;
          queue[queue_end] = current;
          queue_intersections[queue_end++] =
              other_intersection(current, current_intersection);
        }
        current = current->next;
      }
    } else {
      node *current = list->head_streets;
      while (current != NULL) {
        if ((current->street->from_intersection_id == current_intersection ||
             current->street->to_intersection_id == current_intersection) &&
            !was_visited(visited, visited_count, current->street) &&
            queue_end < max_streets) {
          visited[visited_count++] = current->street;
          queue[queue_end] = current;
          queue_intersections[queue_end++] =
              other_intersection(current, current_intersection);
        }
        current = current->next;
      }
    }
  }

  free(queue);
  free(queue_intersections);
  free(visited);
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
        if (current->street->from_intersection_id == intersection_id ||
            current->street->to_intersection_id == intersection_id)
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
  printf("Origin: 41.403429, 2.193660\n");
  printf("Destination: 41.404986, 2.194211\n");

  for (int map_index = 0; map_index < q5_number_of_maps; map_index++) {
    list_map list;
    node *origin;
    node *destination;

    init_list(&list);
    strcpy(list.map, q5_maps[map_index]);

    load_streets(&list);
    list.graph = create_hashmap(&list);
    load_streets_into_graph(list.graph, list.head_streets);

    origin = find_coordinate(&list, 2.193660, 41.403429, false);
    destination = find_coordinate(&list, 2.194211, 41.404986, false);

    printf("\nMap: %s (%d street segments)\n", list.map, list.size_streets);
    printf("Origin street: %s\n", origin->name);
    printf("Destination street: %s\n", destination->name);

    printf("Path finding with sequential connected-street search:\n");
    for (int test = 1; test <= 10; test++) {
      struct timespec start, end;
      long long elapsed_ns;
      int found;

      clock_gettime(CLOCK_MONOTONIC, &start);

      found = path_exists(&list, origin, destination, 0);

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

      found = path_exists(&list, origin, destination, 1);

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
    node *destination =
        find_coordinate(&list, destination_lon[distance_index],
                        destination_lat[distance_index], false);
    double distance_meters =
        haversine(origin->lat, origin->lon, destination->lat, destination->lon) *
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

      found = path_exists(&list, origin, destination, 0);

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

      found = path_exists(&list, origin, destination, 1);

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
