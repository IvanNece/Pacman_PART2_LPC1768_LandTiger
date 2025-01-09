#ifndef ASTAR_H
#define ASTAR_H

#include <stdbool.h>
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define ROW 31
#define COL 28

typedef struct {
    int parent_i, parent_j;  // Coordinate del nodo genitore
    double f, g, h;          // Costi: f = g + h
} Cell;

typedef struct {
    int x, y;
} Pair;

/*
cellDetails è una matrice di tipo Cell, utilizzata dall'algoritmo A* per tracciare i dettagli delle celle
(come i costi f, g, h e i genitori delle celle). Serve come "mappa" che registra le informazioni necessarie
per calcolare il percorso ottimale dalla posizione corrente alla destinazione.
*/

// Dichiarazione globale di cellDetails
static Cell cellDetails[ROW][COL];

void aStarSearch(int grid[ROW][COL], Pair src, Pair dest);
Pair tracePath(Cell cellDetails[ROW][COL], Pair dest);

#endif
