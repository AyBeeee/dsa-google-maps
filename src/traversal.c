#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include <limits.h>
#include "utils.c"
#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include <limits.h>
#include "utils.c"


queue* BFS(hashmap *graph, node *origin, node *destination) {

    queue *explore_queue = malloc(sizeof(queue));
    queue *paths_queue = malloc(sizeof(queue));

    init_queue(explore_queue);
    init_queue(paths_queue);

    node *head_street = get_connected_streets(graph, origin->street->from_intersection_id);

    while (head_street != NULL) {
        enqueue(explore_queue, head_street);
        head_street = head_street->next;
    }

    // create visited list
    queue *visited = malloc(sizeof(queue));
    init_queue(visited);

    while (explore_queue->head != NULL) {

        node *curr_street = dequeue(explore_queue);
        
        if (strcmp(curr_street->name, destination->name) == 0) {
            return paths_queue;
        }

    if (node_in_queue(visited, curr_street) == NULL) {
            enqueue(visited, curr_street);

            node *connected_street = get_connected_streets(graph, curr_street->street->to_intersection_id);

            while (connected_street != NULL) {
                if (node_in_queue(visited, connected_street) == NULL) {
                    enqueue(explore_queue, connected_street);
                }
                connected_street = connected_street->next;
            }
        }
    }