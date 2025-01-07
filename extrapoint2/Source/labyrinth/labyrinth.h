#ifndef LABYRINTH_H
#define LABYRINTH_H

#include <stdint.h>

#define EMPTY 0
#define WALL 1
#define STANDARD_PILL 2
#define POWER_PILL 3

#define WIDTH 28 // Numero di celle in larghezza
#define HEIGHT 31 // Numero di celle in altezza

// Definizioni
#define CELL_SIZE 8

#define Orange 0xFD20  // Definizione colore RGB565 Arancione
#define WALL_COLOR Blue
#define PILL_COLOR Yellow
#define POWER_PILL_COLOR Orange
#define EMPTY_COLOR Black

#define TEXT_COLOR White
#define BACKGROUND_COLOR Black
#define START_LIVES 1
#define PI 3.14159

#define PACMAN_COLOR Yellow

extern int countdown;
extern int score;
extern int lives;
extern int power_pills_generated; 
extern int labyrinth[HEIGHT][WIDTH];



// Dichiarazione del labirinto
//extern int labyrinth[HEIGHT][WIDTH];

// Prototipi delle funzioni

int return_element(int x, int y);
void draw_labyrinth(int labyrinth[HEIGHT][WIDTH]);
void draw_pacman_icon(int x, int y, int radius, uint16_t color);
void display_game_info(void);
void display_timer(void);
void display_score(void);
void display_lives(void);
void generate_power_pills(void);
void check_game_status(void);

#endif
