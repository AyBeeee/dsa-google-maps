#include "utils.h"
#include "main.h"
#include "structs.h"
#include <stdio.h>


// small util function to calculate which street our origin/destination
// nodes fall into, to make the route calc slightly easier
node *node_to_street(list_map *list, node *to_convert)
{
    double x = to_convert->lon;
    double y = to_convert->lat;


    /*node *temp = list->head_streets;
    while (temp)
    {
        printf("X im at: %f, X from: %f, X to %f\n", x, temp->street->from_intersection_lon, temp->street->to_intersection_lon);
        printf("Y im at: %f, Y from: %f, Y to %f\n", y, temp->street->from_intersection_lat, temp->street->to_intersection_lat);
        printf("--- name of street im checking %s\n", temp->name);
        if (((x >= temp->street->from_intersection_lon && x <= temp->street->to_intersection_lon)
            || (x >= temp->street->to_intersection_lon && x <= temp->street->from_intersection_lon))
            &&
            ((y >= temp->street->from_intersection_lat && y <= temp->street->to_intersection_lat)
                || (y >= temp->street->to_intersection_lat && y <= temp->street->from_intersection_lat)))
            return (temp);

        temp = temp->next;
    }
    printf("returning null\n");*/
    return find_coordinate(list, x, y, false);
}


void read_long_input(char *buffer, int size) {
  int ch;
  int newline;

  if (fgets(buffer, size, stdin) == NULL) {
    buffer[0] = '\0';
    return;
  }

  newline = strcspn(buffer, "\n");
  if (buffer[newline] == '\n') {
    buffer[newline] = '\0';
  } else {
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
  }
}

void free_map(list_map *list) {
  node *temp;
  while (list->head_houses) {
    temp = list->head_houses->next;
    free(list->head_houses->name);
    free(list->head_houses->house);
    free(list->head_houses);
    list->head_houses = temp;
  }
  list->head_houses = NULL;
  list->tail_houses = NULL;

  while (list->head_places) {
    temp = list->head_places->next;
    free(list->head_places->name);
    free(list->head_places->place);
    free(list->head_places);
    list->head_places = temp;
  }
  list->head_places = NULL;
  list->tail_places = NULL;

  while (list->head_streets) {
    temp = list->head_streets->next;
    free(list->head_streets->name);
    free(list->head_streets->street);
    free(list->head_streets);
    list->head_streets = temp;
  }
  list->head_streets = NULL;
  list->tail_streets = NULL;

  if (list->graph) {
    for (int i = 0; i < list->graph->table_size; i++) {
      if (list->graph->table[i] != NULL) {
        // printf("delete: %lli\n", list->graph->table[i]->intersection_id);
        // DEBUG

        node *head = list->graph->table[i]->connected_streets;
        node *temp = NULL;
        while (head) {
          temp = head->next;
          free(head);
          head = temp;
          // printf("delete inside: %i\n", j++); DEBUG
        }
      }
      free(list->graph->table[i]);
    }
    free(list->graph->table);
    free(list->graph);
  }
  list->graph = NULL;
}

double toDegrees(double radians) { return radians * (180.0 / M_PI); }

double toRadians(double degree) { return degree * (M_PI / 180.0); }

void midpoint(node *newnode) {
  double lat1 = toRadians(newnode->street->from_intersection_lat);
  double lon1 = toRadians(newnode->street->from_intersection_lon);
  double lat2 = toRadians(newnode->street->to_intersection_lat);
  double lon2 = toRadians(newnode->street->to_intersection_lon);

  double x1 = cos(lat1) * cos(lon1);
  double y1 = cos(lat1) * sin(lon1);
  double z1 = sin(lat1);

  double x2 = cos(lat2) * cos(lon2);
  double y2 = cos(lat2) * sin(lon2);
  double z2 = sin(lat2);

  double x = (x1 + x2) / 2.0;
  double y = (y1 + y2) / 2.0;
  double z = (z1 + z2) / 2.0;

  double lon = atan2(y, x);
  double hyp = sqrt(x * x + y * y);
  double lat = atan2(z, hyp);

  newnode->street->midX = toDegrees(lon);
  newnode->street->midY = toDegrees(lat);
}

double haversine(double lat_A, double lon_A, double lat_B, double lon_B) {
  double lat1 = toRadians(lat_A);
  double lon1 = toRadians(lon_A);
  double lat2 = toRadians(lat_B);
  double lon2 = toRadians(lon_B);

  double dLat = lat2 - lat1;
  double dLon = lon2 - lon1;
  double a =
      pow(sin(dLat / 2), 2) + cos(lat1) * cos(lat2) * pow(sin(dLon / 2), 2);
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));
  return 6371.0 * c; // EARTH_RADIUS * c
}

int findnchar(char *str, char c, int n) {
  if (!str)
    return -1;
  if (n <= 0)
    return 0;
  int i = 0;
  for (i = 0; str[i] && n > 0; i++) {
    if (str[i] == c)
      n--;
  }
  if (!str[i] || n != 0)
    return -1;
  return i;
}

char *skip_street_prefix(char *str) {
  if (strncmp(str, "c. ", 3) == 0)
    return str + 3;
  if (strncmp(str, "c ", 2) == 0)
    return str + 2;
  if (strncmp(str, "carrer ", 7) == 0)
    return str + 7;
  if (strncmp(str, "av. ", 4) == 0)
    return str + 4;
  if (strncmp(str, "av ", 3) == 0)
    return str + 3;
  if (strncmp(str, "avinguda ", 9) == 0)
    return str + 9;
  if (strncmp(str, "ptge. ", 6) == 0)
    return str + 6;
  if (strncmp(str, "ptge ", 5) == 0)
    return str + 5;
  if (strncmp(str, "passatge ", 9) == 0)
    return str + 9;
  if (strncmp(str, "pl. ", 4) == 0)
    return str + 4;
  if (strncmp(str, "pl ", 3) == 0)
    return str + 3;
  if (strncmp(str, "plaça ", 6) == 0)
    return str + 6;
  return str;
}

bool same_street_name(char *map_name, char *user_name) {
  return strcmp(skip_street_prefix(map_name), skip_street_prefix(user_name)) ==
         0;
}

int LevenshteinDistance(char *a, char *b) {
  int la = strlen(a);
  int lb = strlen(b);
  int cost;
  int matrix[la + 1][lb + 1];

  for (int i = 0; i <= la; i++) {
    matrix[i][0] = i;
  }

  for (int j = 0; j <= lb; j++) {
    matrix[0][j] = j;
  }

  for (int i = 1; i <= la; i++) {
    for (int j = 1; j <= lb; j++) {
      if (a[i - 1] == b[j - 1]) {
        cost = 0; // looks in diagonal and if the cost is 0 keeps the same
                  // number because it will be adding them until the end bottom
                  // right corner
      } else {
        cost = 1;
      }

      int del = matrix[i - 1][j] + 1;
      int ins = matrix[i][j - 1] + 1;
      int sub = matrix[i - 1][j - 1] + cost;

      if (del < ins && del < sub) {
        matrix[i][j] = del;
      } else if (ins < sub) {
        matrix[i][j] = ins;
      } else {
        matrix[i][j] = sub;
      }
    }
  }
  return matrix[la][lb];
}

bool enqueue(queue *q, node *curr_node) {

    node *newnode = malloc(sizeof(node));
    newnode->name = curr_node->name;
    newnode->lat = curr_node->lat;
    newnode->lon = curr_node->lon;
    newnode->street = curr_node->street;
    newnode->next = NULL;

    if(q->head == NULL) {
        q->head = newnode;
    }

    if(q->tail != NULL) {
        q->tail->next = newnode;
    }
    q->tail = newnode;

    return true;
}

int dequeue(queue *q) {

  node *curr = q->head;

  if (q->head == NULL) {
    return NULL;
  }

  q->head = q->head->next;

  if (q->head == NULL) {
      q->tail = NULL;
  }

  return curr;
}