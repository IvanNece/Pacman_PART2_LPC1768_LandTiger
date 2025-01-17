#include "labyrinth/labyrinth.h"
#include "GLCD/GLCD.h"
#include <stdlib.h>

#define CHASE 0
#define FRIGHTENED 1

extern int pacman_x;
extern int pacman_y;
extern volatile uint8_t map_matrix[HEIGHT][WIDTH];

void draw_ghost(int row, int col);
void clear_ghost(int row, int col);

void move_ghost(int new_row, int new_col);

void kill_pacman();
void kill_ghost();

void activate_blinky_scared_mode();
int fetch_blinky_color();
