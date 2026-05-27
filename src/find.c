#include "hash.h"
#include "main.h"
#include "utils.h"

node *find_coordinate(list_map *list, double x_target, double y_target,
                      bool verbose) {

  node *curr = list->head_streets;
  node *best_match = NULL;
  int best_dist = INT_MAX;
  for (int i = 0; i < list->size_streets; i++) {

    int dist =
        haversine(curr->street->midX, curr->street->midY, x_target, y_target);

    // printf("dist calculated is: %i\n", dist); //DEBUG
    if (dist < best_dist) {
      best_dist = dist;
      best_match = curr;
    }
    curr = curr->next;
  }

  if (verbose) {
    printf("Your coordinates: %f, %f\n", y_target, x_target);
    printf("Closest street to you: %s\n", best_match->name);
    printf("With coordinates between intersections %lld (%f, %f) and %lld (%f, "
           "%f)\n",
           best_match->street->from_intersection_id,
           best_match->street->from_intersection_lat,
           best_match->street->from_intersection_lon,
           best_match->street->to_intersection_id,
           best_match->street->to_intersection_lat,
           best_match->street->to_intersection_lon);

    print_connected_streets(list->graph,
                            best_match->street->from_intersection_id,
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

  node *best_match = NULL; // init best match
  int best_dist = 7;
  int threshold = 7;
  int street_found = 0;
  char *matched_street = street;
  char choice;
  int num = -1;
  int place_matches = 0;
  node *first_place_match = NULL;

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
      int dist = LevenshteinDistance(curr->name, street);
      // printf("dist calculated is: %i\n", dist); //DEBUG
      if (dist < best_dist) {
        best_dist = dist;
        best_match = curr;
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
        printf("%d. %s (%s) at %lf | %lf\n", option, curr->name,
               curr->place->amenity, curr->lat, curr->lon);
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

  if (best_dist < threshold && best_match != NULL) {
    printf("We couldn't find the address, did you mean: %s [Y-N]\n",
           best_match->name);
    scanf(" %c", &choice);
    if (choice == 'Y') {
      printf("Found at %lf | %lf\n", best_match->lat, best_match->lon);
      return best_match;
      return best_match;
    } else {
      printf("We couldn't find your location!\n");
      return NULL;
    }
  }

  if (street_found && ntype == HOUSES) {
    char num_str[16];
    char candidate_str[16];
    int best_num_dist = INT_MAX;
    node *best_num_match = NULL;

    printf("Valid numbers in %s: ", matched_street);

    snprintf(num_str, sizeof(num_str), "%d", num);

    curr = list->head_houses;
    while (curr != NULL) {
      if (strcmp(curr->name, matched_street) == 0) {
        int dist;
        // list with found number
        // if your num is in the list -> found
        // if not found -> print invalid num & print availablwe numbers
        // flow of selecting correct number

        snprintf(candidate_str, sizeof(candidate_str), "%d", curr->house->num);
        dist = LevenshteinDistance(candidate_str, num_str);

        if (dist < best_num_dist) {
          best_num_dist = dist;
          best_num_match = curr;
        }
      }
      curr = curr->next;
    }
    printf("\n");

    if (best_num_match != NULL) {
      if (best_num_match->house->num != num) {
        printf("We found the street, but number %d is not valid.\n", num);
        printf("Did you mean number %d? [Y-N]\n", best_num_match->house->num);
        scanf(" %c", &choice);
      }
      if (choice == 'Y' || choice == 'y' || best_num_match->house->num == num) {
        printf("Found at %lf | %lf\n", best_num_match->lat,
               best_num_match->lon);
        return best_num_match;
      }
    }

    printf("That number is not valid for this street\n");
    return NULL;
  }
  printf("We couldn't find your location!\n");
  return NULL;
}
