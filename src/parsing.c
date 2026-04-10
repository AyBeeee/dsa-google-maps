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

int levenshtein(char a, char b) {
  int m = strlen(a);
  int n = strlen(b);

  int matrix[m + 1][n + 1];

  for (int i = 0; i < m; i++) {
    matrix[i][0] = i;
  }

  for (int j = 0; j < n; j++) {
    matrix[0][j] = j;
  }

  for (int i = 1; i < m; i++) {
    for (int j = 1; j < n; j++) {
      if (a[i - 1] = b[j - 1]) {
        int cost = 0;
      } else {
        int cost = 1;
      }
      matrix[i][j] = minimum(matrix[i - 1][j] + 1, matrix[i][j - 1] + 1,
                             matrx[i - 1][j - 1] + cost);
    }
  }
  return matrix[m][n];
}