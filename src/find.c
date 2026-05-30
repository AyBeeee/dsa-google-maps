#include "hash.h"
#include "main.h"
#include "utils.h"

node *find_coordinate(list_map *list, double x_target, double y_target,
                      bool verbose) {

  node *curr = list->head_streets;
  node *best_match = NULL;
  double best_dist = INT_MAX;
  for (int i = 0; i < list->size_streets; i++) {

    double dist =
        haversine(curr->street->midY, curr->street->midX, y_target, x_target);

    // printf("dist calculated is: %f\n", dist); //DEBUG
    if (dist < best_dist) {
      best_dist = dist;
      best_match = curr;
    }
    curr = curr->next;
  }

  // now with verbose flag for when i dont want to print the street info
  // and just find the best street instead.
  if (verbose) {
    printf("Closest street: %s\n", best_match->name);
    printf("Between %lld (%f, %f) and %lld (%f, %f)\n",
          best_match->street->from_intersection_id,
          best_match->street->from_intersection_lat,
          best_match->street->from_intersection_lon,
          best_match->street->to_intersection_id,
          best_match->street->to_intersection_lat,
          best_match->street->to_intersection_lon);

    printf("\nFrom this street segment, you can go to:\n");

    print_connected_streets(list->graph,
                            best_match->street->from_intersection_id,
                            best_match->name, true);

    print_connected_streets(list->graph,
                            best_match->street->to_intersection_id,
                            best_match->name, true);
  }
  return (best_match);
}

node *find_node(list_map *list, char *street, int ntype) {

  node *curr = NULL;
  int size = 0;
  if (ntype == HOUSES) {
    curr = list->head_houses;
    size = list->size_houses;
  } else if (ntype == PLACES) {
    curr = list->head_places;
    size = list->size_places;
  }

  int threshold = 7;
  int place_threshold = 10;
  int street_found = 0;
  char *matched_street = street;
  int num = -1;
  int place_matches = 0;
  node *first_place_match = NULL;
  // similar streets list implemented (this will give us the top 3 streets with the lowest dist)
  node *similar_streets[3] = {NULL, NULL, NULL};
  int similar_distances[3] = {INT_MAX, INT_MAX, INT_MAX};
  int similar_count = 0;
  // same idea as the streets but for places
  node *similar_places[3] = {NULL, NULL, NULL};
  int similar_place_distances[3] = {INT_MAX, INT_MAX, INT_MAX};
  int similar_place_count = 0;

  if (ntype == HOUSES) {
    printf("Enter street number (e.g. '138'): \n");
    scanf("%d", &num);
  }

  // convert to lower
  for (int i = 0; i < (int)strlen(street); i++) {
    street[i] = tolower(street[i]);
  }

  // printf("Street to lower: %s.\n", street); // DEBUG
  for (int i = 0; i < size; i++) {
    // printf("checking input to: %s.\n", curr->name); // DEBUG

    // compare exact match
    if ((ntype == HOUSES && same_street_name(curr->name, street)) ||
        (ntype == PLACES && strcmp(curr->name, street) == 0)) {
      street_found = 1;
      matched_street = curr->name;
      // printf("Street's current NUM: %d.\n", curr->house->num); // DEBUG

      if (ntype == HOUSES && curr->house->num == num) {
        printf("Found at %lf | %lf\n", curr->lat, curr->lon);
        return curr;
      } else if (ntype != HOUSES) {
        if (place_matches == 0) {
          first_place_match = curr;
        }
        place_matches++;
      }
    }

    // compare using distance to find most similar one
    else {
      int dist;

      // perform levenshtein distance with the houses to calculate the dist of
      // the curr street
      if (ntype == HOUSES) {
        dist = LevenshteinDistance(skip_street_prefix(curr->name),
                                   skip_street_prefix(street));
      }

      else {
        dist = LevenshteinDistance(curr->name, street);
      }

      // printf("dist calculated is: %i\n", dist); //DEBUG

      if (ntype == HOUSES) {
        bool already_added =
            false; // first time seeing the street mark as false

        for (int j = 0; j < similar_count; j++) {
          if (same_street_name(similar_streets[j]->name, curr->name)) {
            already_added = true; // mark alread_added as true if the street
                                  // name is found in the list of added streets
          }
        }

        // if the address hasn't been added to the list of addresses
        if (!already_added) {
          int position = similar_count;

          // if the levenshtein distance is lower than any of the current
          // distances stored, update the position where dist < current dist
          for (int j = 0; j < similar_count; j++) {
            if (dist < similar_distances[j]) {
              position = j;
              break;
            }
          }

          if (position < 3) {
            if (similar_count < 3) {
              similar_count++;
            }

            // insert the new address with low dist in the correct idx
            for (int j = similar_count - 1; j > position; j--) {
              similar_streets[j] = similar_streets[j - 1];
              similar_distances[j] = similar_distances[j - 1];
            }

            similar_streets[position] = curr;
            similar_distances[position] = dist;
          }
        }
      }

      if (ntype == PLACES) {
        int position = similar_place_count;

        for (int j = 0; j < similar_place_count; j++) {
          if (dist < similar_place_distances[j]) {
            position = j;
            break;
          }
        }

        if (position < 3) {
          if (similar_place_count < 3) {
            similar_place_count++;
          }

          for (int j = similar_place_count - 1; j > position; j--) {
            similar_places[j] = similar_places[j - 1];
            similar_place_distances[j] = similar_place_distances[j - 1];
          }

          similar_places[position] = curr;
          similar_place_distances[position] = dist;
        }
      }
    }
    curr = curr->next;
  }

  if (ntype == PLACES && place_matches == 1) {
    printf("Found at %lf | %lf\n", first_place_match->lat,
           first_place_match->lon);
    return first_place_match;
  }

  if (ntype == PLACES && place_matches > 1) {
    int option = 1;
    int selected;

    printf("There is more than one place with this name:\n");
    curr = list->head_places;
    while (curr != NULL) {
      if (strcmp(curr->name, street) == 0) {
        printf("%d. %s at %lf | %lf\n", option, curr->name, curr->lat, curr->lon);
        option++;
      }
      curr = curr->next;
    }

    printf("Choose one: ");
    scanf("%d", &selected);

    if (selected < 1 || selected > place_matches) {
      printf("Wrong input!\n");
      return NULL;
    }

    option = 1;
    curr = list->head_places;
    while (curr != NULL) {
      if (strcmp(curr->name, street) == 0) {
        if (option == selected) {
          printf("Found at %lf | %lf\n", curr->lat, curr->lon);
          return curr;
        }
        option++;
      }
      curr = curr->next;
    }
  }

  // if the street_found flag is false, the user is asking for houses, the
  if (!street_found && ntype == HOUSES && similar_count > 0 &&
      similar_distances[0] < threshold) {
    int selected;

    printf("We couldn't find that street. Did you mean:\n");

    // Print similar streets to the one entered by the user (print a number and
    // the address)
    for (int i = 0; i < similar_count; i++) {
      printf("%d. %s\n", i + 1, similar_streets[i]->name);
    }

    printf("Choose one: ");
    scanf("%d", &selected);

    if (selected < 1 || selected > similar_count) {
      printf("Wrong input\n");
      return NULL;
    }

    matched_street = similar_streets[selected - 1]->name;
    street_found = 1;
  }

  if (ntype == PLACES && similar_place_count > 0 &&
      similar_place_distances[0] < place_threshold) {
    int selected;

    printf("We couldn't find that place. Did you mean:\n");

    for (int i = 0; i < similar_place_count; i++) {
      printf("%d. %s at %lf | %lf\n", i + 1,
            similar_places[i]->name,
            similar_places[i]->lat, similar_places[i]->lon);
    }

    printf("Choose one: ");
    scanf("%d", &selected);

    if (selected < 1 || selected > similar_place_count) {
      printf("Wrong input\n");
      return NULL;
    }

    printf("Found at %lf | %lf\n", similar_places[selected - 1]->lat,
          similar_places[selected - 1]->lon);

    return similar_places[selected - 1];
  }

  if (street_found && ntype == HOUSES) {
    int best_num_dist = INT_MAX;
    node *best_num_match = NULL;

    curr = list->head_houses;
    while (curr != NULL) {
      if (strcmp(curr->name, matched_street) == 0) {
        int dist;
        // list with found number
        // if your num is in the list -> found
        // if not found -> print invalid num & print availablwe numbers
        // flow of selecting correct number

        dist = abs(curr->house->num - num);

        if (dist < best_num_dist) {
          best_num_dist = dist;
          best_num_match = curr;
        }
      }
      curr = curr->next;
    }
    printf("\n");

    if (best_num_match == NULL) {
      printf("We couldn't find your location!\n");
      return NULL;
    }

    if (best_num_match->house->num != num) {
      printf("We found the street, but number %d is not valid\n", num);
      printf("Valid numbers in %s: ", matched_street);

      curr = list->head_houses;
      while (curr != NULL) {
        if (strcmp(curr->name, matched_street) == 0) {
          printf("%d ", curr->house->num);
        }
        curr = curr->next;
      }

      // ask the user for a valid number from the printed numbers
      printf("\nChoose a valid number: ");
      scanf("%d", &num);

      curr = list->head_houses;
      while (curr != NULL) {
        if (strcmp(curr->name, matched_street) == 0 &&
            curr->house->num == num) {
          printf("Found at %lf | %lf\n", curr->lat, curr->lon);
          return curr;
        }
        curr = curr->next;
      }

      printf("That number is not valid for this street\n");
      return NULL;
    }

    printf("Found at %lf | %lf\n", best_num_match->lat, best_num_match->lon);
    return best_num_match;
  }

  printf("We couldn't find your location!\n");
  return NULL;
}
