#include "labyrinth/labyrinth.h"
#include "GLCD/GLCD.h"
#include <stdlib.h>

#define CHASE 0
#define FRIGHTENED 1

extern int pacman_x;
extern int pacman_y;
extern volatile uint8_t map_matrix[HEIGHT][WIDTH];


void draw_ghost(int i, int j);
void move_ghost(int new_y, int new_x);
void clear_ghost(int y, int x);
int get_ghost_color();
void eat_pacman();
void eat_ghost();
void activate_ghost_escape();
