#include "main.h"

int parse_map_file(char *path, char *filename) {
  strcat(path, filename);
  FILE *mapfile = fopen(path, "r");
  if (!mapfile)
    return -1;

  char *buf = malloc(42 * sizeof(char));
  while (fgets(buf, 42, mapfile)) {
  }

  free(buf);
  return 0;
}

int parse_map_wrapper(char *map_name) {
  char path[42]; // TODO make this dynamic

  sprintf(path, "%s/maps/", getcwd(path, 42));
  strcat(path, map_name);
  strcat(path, "/");
  if (parse_map_file(path, "houses.txt") != 0)
    printf("FUUUCK\n");
  // TODO free path variable
  return 0;
}
