#include "labyrinth/labyrinth.h"
#include "Ghost/Ghost.h"

#include <float.h>  // Per FLT_MAX
#include <math.h>   // Per sqrt

#define HEIGHT 31  // HEIGHT
#define WIDTH 28	// WIDTH

extern int labyrinth[HEIGHT][WIDTH];

void ghost_a_star_search(int src_row, int src_col, int dest_row, int dest_col);
void ghost_escape(int ghost_y, int ghost_x, int pacman_y, int pacman_x);