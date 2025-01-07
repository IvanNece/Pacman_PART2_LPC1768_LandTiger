#ifndef GHOST_H
#define GHOST_H

#include "Pacman/Pacman.h"
#include "labyrinth/labyrinth.h"
#include "AStar/AStar.h"
#include "GLCD/GLCD.h"
#include <stdlib.h>
#include <stdint.h>  // Per tipi di dati come uint16_t

// Modalità del fantasma
#define CHASE 1
#define FRIGHTENED 2

// Colore del fantasma
#define BLINKY_COLOR 0xF800 // Rosso in formato RGB565

// Posizione iniziale del fantasma
#define SPAWN_X 14
#define SPAWN_Y 15

typedef struct {
    int x;           // Posizione X nel labirinto
    int y;           // Posizione Y nel labirinto
    int direction;   // Direzione attuale del fantasma
    int mode;        // Modalità: CHASE o FRIGHTENED
    int speed;       // Velocità del fantasma
    uint16_t color;  // Colore del fantasma
} Ghost;

// Funzioni pubbliche
void ghost_init(void);
void ghost_draw(void);
void ghost_clear(void);
void ghost_update(void);

#endif