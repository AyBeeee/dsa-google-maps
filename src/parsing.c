#include "linked_list.h"
#include <stdio.h>
#include <string.h>

int parse_wrapper(char *map_name)
{
    char *buf[42];

    sprintf(buf, )
}

int levenshtein(char a, char b){
    int m = strlen(a);
    int n = strlen(b);

    int matrix[m+1][n+1];

    for(int i = 0; i < m; i++){
        matrix[i][0] = i;
    }

    for(int j = 0; j < n; j++){
            matrix[0][j] = j;
        }

    for(int i = 1; i < m; i++){
        for(int j = 1; j < n; j++){
            if(a[i-1] = b[j-1]){
                int cost = 0;
            }
            else{
                int cost = 1;
            }
            matrix[i][j] = minimum(matrix[i-1][j] + 1, matrix[i][j-1] + 1, matrx[i-1][j-1] + cost);

        }
    }
    return matrix[m][n];
}
