#include "labyrinth/labyrinth.h"
#include "Ghost/Ghost.h"
#include <float.h> 
#include <math.h>  

#define HEIGHT 31 
#define WIDTH 28	

extern int labyrinth[HEIGHT][WIDTH];

void perform_a_star_search(int start_row, int start_col, int goal_row, int goal_col);
void initiate_ghost_escape(int ghost_current_y, int ghost_current_x, int pacman_y, int pacman_x);