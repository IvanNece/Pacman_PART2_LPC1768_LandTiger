#include "Astar.h"

// Definizione della struttura per la gestione delle celle
typedef struct {
    uint16_t parent_i, parent_j;
    float f, g, h; // Valori utilizzati dall'algoritmo per calcolare il percorso
} Cell;

// **********************************************************************************************************************************
// Utility function per l'algoritmo A*

// Funzione per controllare se la cella si trova all'interno del labirinto
uint8_t is_cell_valid(int row, int col) {
    return (row >= 0 && row < HEIGHT && col >= 0 && col < WIDTH);
}

// Funzione per verificare se una cella è accessibile
uint8_t is_cell_walkable(int row, int col) {
    return (labyrinth[row][col] != WALL && labyrinth[row][col] != 7);
}

// Funzione per controllare se una cella è la destinazione
uint8_t check_if_destination(int row, int col, int target_row, int target_col) {
    return (row == target_row && col == target_col);
}

// Funzione per calcolare il valore euristico per una cella
float heuristic_value(int row, int col, int target_row, int target_col) {
    return sqrt((float)((row - target_row) * (row - target_row) + (col - target_col) * (col - target_col)));
}

// *******************************************************************************************************************************************
// Implementazione dell'algoritmo A*

void perform_a_star_search(int start_row, int start_col, int goal_row, int goal_col) {
    int i, j, k, direction;

    // Controlla la validità delle coordinate iniziali e finali
    if (!is_cell_valid(start_row, start_col) || !is_cell_valid(goal_row, goal_col)) {
        return;
    }
    if (!is_cell_walkable(start_row, start_col) || !is_cell_walkable(goal_row, goal_col)) {
        return;
    }
    if (check_if_destination(start_row, start_col, goal_row, goal_col)) {
        return;
    }

    // Inizializzazione delle liste e delle strutture dati
    static uint8_t visited_nodes[HEIGHT][WIDTH];
    static Cell cell_info[HEIGHT][WIDTH];
    static float open_list[HEIGHT * WIDTH][3];
    int open_list_size = 0;
		
		
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            visited_nodes[i][j] = 0;
        }
    }

    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            cell_info[i][j].f = FLT_MAX;
            cell_info[i][j].g = FLT_MAX;
            cell_info[i][j].h = FLT_MAX;
            cell_info[i][j].parent_i = (uint16_t)-1;
            cell_info[i][j].parent_j = (uint16_t)-1;
        }
    }

    cell_info[start_row][start_col].f = 0.0f;
    cell_info[start_row][start_col].g = 0.0f;
    cell_info[start_row][start_col].h = 0.0f;
    cell_info[start_row][start_col].parent_i = start_row;
    cell_info[start_row][start_col].parent_j = start_col;

    open_list[open_list_size][0] = 0.0f;
    open_list[open_list_size][1] = (float)start_row;
    open_list[open_list_size][2] = (float)start_col;
    open_list_size++;

    uint8_t path_found = 0;

    while (open_list_size > 0) {
        int best_node = 0;

        // Trova il nodo con il valore f più basso
        for (i = 1; i < open_list_size; i++) {
            if (open_list[i][0] < open_list[best_node][0]) {
                best_node = i;
            }
        }

        // Recupera le coordinate del miglior nodo
        int current_row = (int)open_list[best_node][1];
        int current_col = (int)open_list[best_node][2];

        // Rimuove il nodo dalla open list
        open_list_size--;
        for (k = best_node; k < open_list_size; k++) {
            open_list[k][0] = open_list[k + 1][0];
            open_list[k][1] = open_list[k + 1][1];
            open_list[k][2] = open_list[k + 1][2];
        }

        visited_nodes[current_row][current_col] = 1;

        // Direzioni di movimento: su, giù, sinistra, destra
        int movement[4][2] = {
            {-1, 0}, {1, 0}, {0, -1}, {0, 1}
        };

        for (direction = 0; direction < 4; direction++) {
            int next_row = current_row + movement[direction][0];
            int next_col = current_col + movement[direction][1];

            if (is_cell_valid(next_row, next_col)) {
                if (check_if_destination(next_row, next_col, goal_row, goal_col)) {
                    cell_info[next_row][next_col].parent_i = current_row;
                    cell_info[next_row][next_col].parent_j = current_col;

                    int move_to_row = next_row;
                    int move_to_col = next_col;

                    while (cell_info[move_to_row][move_to_col].parent_i != start_row ||
                           cell_info[move_to_row][move_to_col].parent_j != start_col) {
                        int temp_row = cell_info[move_to_row][move_to_col].parent_i;
                        int temp_col = cell_info[move_to_row][move_to_col].parent_j;
                        move_to_row = temp_row;
                        move_to_col = temp_col;
                    }

                    move_ghost(move_to_row, move_to_col);
                    path_found = 1;
                    return;
                } else if (!visited_nodes[next_row][next_col] && is_cell_walkable(next_row, next_col)) {
                    float g_new = cell_info[current_row][current_col].g + 1.0f;
                    float h_new = heuristic_value(next_row, next_col, goal_row, goal_col);
                    float f_new = g_new + h_new;

                    if (cell_info[next_row][next_col].f == FLT_MAX || cell_info[next_row][next_col].f > f_new) {
                        open_list[open_list_size][0] = f_new;
                        open_list[open_list_size][1] = (float)next_row;
                        open_list[open_list_size][2] = (float)next_col;
                        open_list_size++;

                        cell_info[next_row][next_col].f = f_new;
                        cell_info[next_row][next_col].g = g_new;
                        cell_info[next_row][next_col].h = h_new;
                        cell_info[next_row][next_col].parent_i = current_row;
                        cell_info[next_row][next_col].parent_j = current_col;
                    }
                }
            }
        }
    }
}

// *******************************************************************************************************************************************
// Funzione per far fuggire il fantasma
void initiate_ghost_escape(int ghost_current_y, int ghost_current_x, int pacman_y, int pacman_x) {
	
		int option;
	
    int movement_options[4][2] = {
        {-1, 0}, {1, 0}, {0, -1}, {0, 1} // Direzioni: sopra, sotto, sinistra, destra
    };

    int optimal_y = ghost_current_y;
    int optimal_x = ghost_current_x;
    float furthest_distance = -1.0;

    for (option = 0; option < 4; option++) {
        int potential_y = ghost_current_y + movement_options[option][0];
        int potential_x = ghost_current_x + movement_options[option][1];

        // Verifica che la cella sia valida e accessibile
        if (is_cell_valid(potential_y, potential_x) && is_cell_walkable(potential_y, potential_x)) {
            float distance_from_pacman = heuristic_value(potential_y, potential_x, pacman_y, pacman_x);

            // Trova la cella più lontana da Pac-Man
            if (distance_from_pacman > furthest_distance) {
                furthest_distance = distance_from_pacman;
                optimal_y = potential_y;
                optimal_x = potential_x;
            }
        }
    }

    // Muovi il fantasma verso la posizione ottimale
    if (optimal_y != ghost_current_y || optimal_x != ghost_current_x) {
        move_ghost(optimal_y, optimal_x);
    }
}
