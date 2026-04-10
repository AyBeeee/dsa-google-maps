#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "main.h"

void createaleak() {
  char *foo = malloc(20 * sizeof(char));
  printf("Allocated leaking string: %s", foo);
}

int main() {
  ask_map();
  return 0;
}
