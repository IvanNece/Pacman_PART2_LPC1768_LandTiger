#ifndef PACMAN_H
#define PACMAN_H

#include "GLCD/GLCD.h"
#include "Ghost/Ghost.h"

// Definizione delle coordinate iniziali di Pac-Man
#define PACMAN_START_X 12
#define PACMAN_START_Y 17

// Direzioni
#define STOP 0
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

// Colore e dimensione
#define PACMAN_COLOR Yellow
#define PACMAN_RADIUS 3

// Coordinate globali e direzione
extern int pacman_x;
extern int pacman_y;
extern int pacman_direction;

// Funzioni
void pacman_init(void);
void pacman_draw(void);
void pacman_update(void);  // Aggiorna il movimento
void pacman_clear(void);   // Cancella Pac-Man nella posizione precedente
void pacman_set_direction(int new_direction);
void redraw_cell(int x, int y);

#endif
