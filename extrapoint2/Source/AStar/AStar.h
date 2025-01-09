#ifndef ASTAR_H
#define ASTAR_H

#include <stdbool.h>
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

#define ROW 31
#define COL 28

typedef struct {
    int parent_i, parent_j;  // Coordinate del nodo genitore
    double f, g, h;          // Costi: f = g + h
} Cell;

typedef struct {
    int x, y;
} Pair;

extern Cell cellDetails[ROW][COL];  // Dichiarazione globale

void aStarSearch(int grid[ROW][COL], Pair src, Pair dest);
Pair tracePath(Cell cellDetails[ROW][COL], Pair dest);

#endif
