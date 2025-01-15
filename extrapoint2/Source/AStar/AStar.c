#include "Astar.h"

// Struttura per rappresentare una cella
typedef struct {
    uint16_t parent_i, parent_j;
    float f, g, h;
} Cell;

// Funzione per verificare se una cella ? valida
uint8_t isValid(int row, int col) {
    return (row >= 0 && row < HEIGHT && col >= 0 && col < WIDTH);
}

// Funzione per verificare se una cella non ? bloccata
uint8_t isUnBlocked(int row, int col) {
    return (labyrinth[row][col] != WALL && labyrinth[row][col] != 7);
}

// Funzione per verificare se una cella ? la destinazione
uint8_t isDestination(int row, int col, int dest_row, int dest_col) {
    return (row == dest_row && col == dest_col);
}

// Funzione per calcolare il valore euristico 'h'
float calculateHValue(int row, int col, int dest_row, int dest_col) {
    return sqrt((float)((row - dest_row) * (row - dest_row) + (col - dest_col) * (col - dest_col)));
}

// =========================================================================================================================================================
// Algoritmo A*

// Funzione principale dell'algoritmo A*
void ghost_a_star_search(int src_row, int src_col, int dest_row, int dest_col) {
    int fi, fj, i, j, k, d;

    if (!isValid(src_row, src_col) || !isValid(dest_row, dest_col)) {
        return;
    }
    if (!isUnBlocked(src_row, src_col) || !isUnBlocked(dest_row, dest_col)) {
        return;
    }
    if (isDestination(src_row, src_col, dest_row, dest_col)) {
        return;
    }
		
		// Prova aggiunta static
    static uint8_t closedList[HEIGHT][WIDTH];
    for (fi = 0; fi < HEIGHT; fi++) {
        for (fj = 0; fj < WIDTH; fj++) {
            closedList[fi][fj] = 0;  // Inizializzo a "false"
        }
    }
		
		// Prova aggiunta static
    static Cell cellDetails[HEIGHT][WIDTH];
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            cellDetails[i][j].f = FLT_MAX;
            cellDetails[i][j].g = FLT_MAX;
            cellDetails[i][j].h = FLT_MAX;
            cellDetails[i][j].parent_i = (uint16_t)-1;
            cellDetails[i][j].parent_j = (uint16_t)-1;
        }
    }
		
		// Prova aggiunta static
    static float openList[HEIGHT * WIDTH][3]; // [f, row, col]
    int openCount = 0;

    cellDetails[src_row][src_col].f = 0.0f;
    cellDetails[src_row][src_col].g = 0.0f;
    cellDetails[src_row][src_col].h = 0.0f;
    cellDetails[src_row][src_col].parent_i = src_row;
    cellDetails[src_row][src_col].parent_j = src_col;

    openList[openCount][0] = 0.0f;
    openList[openCount][1] = (float)src_row;
    openList[openCount][2] = (float)src_col;
    openCount++;

    uint8_t foundDest = 0;  // "false"

    while (openCount > 0) {
        int minIndex = 0;
        for (i = 1; i < openCount; i++) {
            if (openList[i][0] < openList[minIndex][0]) {
                minIndex = i;
            }
        }

        i = (int)openList[minIndex][1];
        j = (int)openList[minIndex][2];
        openCount--;
        for (k = minIndex; k < openCount; k++) {
            openList[k][0] = openList[k + 1][0];
            openList[k][1] = openList[k + 1][1];
            openList[k][2] = openList[k + 1][2];
        }

        closedList[i][j] = 1;  // "true"

        int directions[4][2] = {
            {-1, 0}, {1, 0}, {0, -1}, {0, 1}}; // Nord, Sud, Ovest, Est
        for (d = 0; d < 4; d++) {
            int new_i = i + directions[d][0];
            int new_j = j + directions[d][1];

            if (isValid(new_i, new_j)) {
                if (isDestination(new_i, new_j, dest_row, dest_col)) {
                    cellDetails[new_i][new_j].parent_i = i;
                    cellDetails[new_i][new_j].parent_j = j;

                    int first_i = new_i, first_j = new_j;
                    while (cellDetails[first_i][first_j].parent_i != src_row || cellDetails[first_i][first_j].parent_j != src_col) {
                        int temp_i = cellDetails[first_i][first_j].parent_i;
                        int temp_j = cellDetails[first_i][first_j].parent_j;
                        first_i = temp_i;
                        first_j = temp_j;
                    }
										
                    move_ghost(first_i, first_j);

                    foundDest = 1;  // "true"
                    return;
                } else if (!closedList[new_i][new_j] && isUnBlocked(new_i, new_j)) {
                    float gNew = cellDetails[i][j].g + 1.0f;
                    float hNew = calculateHValue(new_i, new_j, dest_row, dest_col);
                    float fNew = gNew + hNew;

                    if (cellDetails[new_i][new_j].f == FLT_MAX || cellDetails[new_i][new_j].f > fNew) {
                        openList[openCount][0] = fNew;
                        openList[openCount][1] = (float)new_i;
                        openList[openCount][2] = (float)new_j;
                        openCount++;

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
}




//=======================================================================================================================

// Funzione per far scappare il fantasma
void ghost_escape(int ghost_y, int ghost_x, int pacman_y, int pacman_x){
	int directions[4][2] = {
        {-1, 0}, {1, 0}, {0, -1}, {0, 1} // Nord, Sud, Ovest, Est
    };
    
    int best_y = ghost_y;
    int best_x = ghost_x;
    float max_distance = -1.0;
		int i;
    for (i = 0; i < 4; i++) {
        int new_y = ghost_y + directions[i][0];
        int new_x = ghost_x + directions[i][1];

        // Verifica se la cella ? valida e non bloccata
        if (isValid(new_y, new_x) && isUnBlocked(new_y, new_x)) {
            // Calcola la distanza inversa da Pac-Man
            float distance = calculateHValue(new_y, new_x, pacman_y, pacman_x);
            
            // Cerca la cella con la massima distanza
            if (distance > max_distance) {
                max_distance = distance;
                best_y = new_y;
                best_x = new_x;
            }
        }
    }

    // Muovi il fantasma nella direzione della massima distanza
    if (best_y != ghost_y || best_x != ghost_x) {
        move_ghost(best_y, best_x);
    }
}
