#include "AStar.h"

// Funzione per verificare se una cella è valida
bool isValid(int row, int col) {
    return (row >= 0) && (row < ROW) && (col >= 0) && (col < COL);
}

// Funzione per verificare se una cella non è bloccata
bool isUnBlocked(int grid[ROW][COL], int row, int col) {
    return (grid[row][col] == 1);
}

// Funzione per verificare se la cella è la destinazione
bool isDestination(int row, int col, Pair dest) {
    return (row == dest.x && col == dest.y);
}

// Funzione per calcolare l'euristica (distanza Euclidea)
double calculateHValue(int row, int col, Pair dest) {
    return sqrt((row - dest.x) * (row - dest.x) + (col - dest.y) * (col - dest.y));
}

// Funzione per tracciare il percorso (puoi aggiornarla per segnare il percorso nel labirinto)
Pair tracePath(Cell cellDetails[ROW][COL], Pair dest) {
    int row = dest.x;
    int col = dest.y;

    while (!(cellDetails[row][col].parent_i == row && cellDetails[row][col].parent_j == col)) {
        // SI PUO AGGIORNARE IL LABIRITNO O LE VARIABILI DI STATO QUA
        int temp_row = cellDetails[row][col].parent_i;
        int temp_col = cellDetails[row][col].parent_j;
        row = temp_row;
        col = temp_col;
    }
		
		// Restituisci il primo passo verso la destinazione
    return (Pair){row, col};
}

// Funzione principale per A*
void aStarSearch(int grid[ROW][COL], Pair src, Pair dest) {
    int i, j, k, dir;

    if (!isValid(src.x, src.y) || !isValid(dest.x, dest.y)) {
        return;
    }

    if (!isUnBlocked(grid, src.x, src.y) || !isUnBlocked(grid, dest.x, dest.y)) {
        return;
    }

    if (isDestination(src.x, src.y, dest)) {
        return;
    }

    bool closedList[ROW][COL] = { false };

    Cell cellDetails[ROW][COL];
    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            cellDetails[i][j].f = FLT_MAX;
            cellDetails[i][j].g = FLT_MAX;
            cellDetails[i][j].h = FLT_MAX;
            cellDetails[i][j].parent_i = -1;
            cellDetails[i][j].parent_j = -1;
        }
    }

    i = src.x;
    j = src.y;
    cellDetails[i][j].f = 0.0;
    cellDetails[i][j].g = 0.0;
    cellDetails[i][j].h = 0.0;
    cellDetails[i][j].parent_i = i;
    cellDetails[i][j].parent_j = j;

    Pair openList[ROW * COL];
    int openListSize = 0;

    openList[openListSize++] = src;

    bool foundDest = false;

    while (openListSize > 0) {
        Pair current = openList[0];
        int index = 0;
        for (k = 1; k < openListSize; k++) {
            if (cellDetails[openList[k].x][openList[k].y].f < cellDetails[openList[index].x][openList[index].y].f) {
                index = k;
            }
        }

        Pair node = openList[index];
        for (k = index; k < openListSize - 1; k++) {
            openList[k] = openList[k + 1];
        }
        openListSize--;

        i = node.x;
        j = node.y;
        closedList[i][j] = true;

        int rowNum[] = { -1, 0, 0, 1 };
        int colNum[] = { 0, -1, 1, 0 };

        for (dir = 0; dir < 4; dir++) {
            int new_i = i + rowNum[dir];
            int new_j = j + colNum[dir];

            if (isValid(new_i, new_j)) {
                if (isDestination(new_i, new_j, dest)) {
                    cellDetails[new_i][new_j].parent_i = i;
                    cellDetails[new_i][new_j].parent_j = j;
                    tracePath(cellDetails, dest);
                    foundDest = true;
                    return;
                } else if (!closedList[new_i][new_j] && isUnBlocked(grid, new_i, new_j)) {
                    double gNew = cellDetails[i][j].g + 1.0;
                    double hNew = calculateHValue(new_i, new_j, dest);
                    double fNew = gNew + hNew;

                    if (cellDetails[new_i][new_j].f == FLT_MAX || cellDetails[new_i][new_j].f > fNew) {
                        openList[openListSize++] = (Pair){ new_i, new_j };
                        cellDetails[new_i][new_j].f = fNew;
                        cellDetails[new_i][new_j].g = gNew;
                        cellDetails[new_i][new_j].h = hNew;
                        cellDetails[new_i][new_j].parent_i = i;
                        cellDetails[new_i][new_j].parent_j = j;
                    }
                }
            }
        }
    }

    if (!foundDest) {
        // Nessuna azione se la destinazione non è trovata
    }
}
