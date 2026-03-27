#include "main.h"

void createaleak() {
  char *foo = malloc(20 * sizeof(char));
  printf("Allocated leaking string: %s", foo);
}

int main() {
  printf("*****************\nWelcome to DSA!\n*****************\n");

  // how to import and call a function
  parse_map_wrapper("md_1");

  // uncomment and run "make v" to see how valgrind detects memory leaks
  // createaleak();

  return 0;
}
