#include "Ghost.h"

// Variabile globale per il fantasma Blinky
Ghost blinky;

// Variabili per gestire la velocit? di Blinky
static int blinky_speed_counter = 0;    // Contatore per gestire la velocit?
static int blinky_speed_threshold = 10; // Soglia iniziale per il contatore

extern int labyrinth[HEIGHT][WIDTH];
extern int pacman_x, pacman_y;

void ghost_init(void)
{
    blinky.x = SPAWN_X;          // Posizione iniziale X
    blinky.y = SPAWN_Y;          // Posizione iniziale Y
    blinky.direction = 0;        // Ferma il movimento inizialmente
    blinky.mode = CHASE;         // Modalit? iniziale: CHASE
    blinky.speed = 1;            // Velocit? iniziale
    blinky.color = BLINKY_COLOR; // Colore assegnato al fantasma
    ghost_draw();
}

void ghost_draw(void)
{
    extern int offset_x; // Offset globale per il labirinto
    extern int offset_y;

    int pixel_x = offset_x + blinky.x * CELL_SIZE + CELL_SIZE / 2; // Posizione X centrata nella cella
    int pixel_y = offset_y + blinky.y * CELL_SIZE + CELL_SIZE / 2; // Posizione Y centrata nella cella

    // Disegna un cerchio per rappresentare Blinky
    int radius = CELL_SIZE / 2; // Raggio del cerchio
    int i, j;
    for (i = -radius; i <= radius; i++)
    {
        for (j = -radius; j <= radius; j++)
        {
            if (i * i + j * j <= radius * radius)
            { // Condizione per rimanere nel cerchio
                disable_RIT();
                LCD_SetPoint(pixel_x + i, pixel_y + j, blinky.color); // Colore di Blinky
                enable_RIT();
            }
        }
    }
}

void ghost_clear(void)
{
    extern int offset_x; // Offset globale per il labirinto
    extern int offset_y;

    int pixel_x = offset_x + blinky.x * CELL_SIZE + CELL_SIZE / 2; // Posizione X centrata
    int pixel_y = offset_y + blinky.y * CELL_SIZE + CELL_SIZE / 2; // Posizione Y centrata

    int radius = CELL_SIZE / 2; // Raggio del cerchio
    int i, j;
    for (i = -radius; i <= radius; i++)
    {
        for (j = -radius; j <= radius; j++)
        {
            if (i * i + j * j <= radius * radius)
            { // Condizione per rimanere nel cerchio
                disable_RIT();
                LCD_SetPoint(pixel_x + i, pixel_y + j, EMPTY_COLOR); // Colore di sfondo
                enable_RIT();
            }
        }
    }
}

void ghost_update_speed(void)
{
    if (blinky_speed_counter >= blinky_speed_threshold)
    {
        blinky_speed_counter = 0; // Resetta il contatore
        ghost_update();           // Aggiorna la posizione di Blinky

        // Diminuisci la soglia per aumentare progressivamente la velocit?
        if (blinky_speed_threshold > 1)
        {
            blinky_speed_threshold--;
        }
    }
    else
    {
        blinky_speed_counter++;
    }
}

void ghost_update(void)
{
    Pair ghost_pos = {blinky.x, blinky.y};
    Pair pacman_pos = {pacman_x, pacman_y};
    int i;

    // Se il fantasma ? in modalit? CHASE
    if (blinky.mode == CHASE)
    {
        Pair next_pos;

        // Calcola il prossimo passo usando A*
        aStarSearch(labyrinth, ghost_pos, pacman_pos);
        next_pos = tracePath(cellDetails, pacman_pos);

        // Verifica che la nuova posizione non sia un muro
        if (labyrinth[next_pos.y][next_pos.x] != WALL)
        {
            ghost_clear();
            blinky.x = next_pos.x;
            blinky.y = next_pos.y;
            ghost_draw();
        }
        else
        {
            // Se il fantasma ? bloccato, prova a muoverlo in una direzione casuale
            int directions[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}}; // Su, Gi?, Sinistra, Destra
            for (i = 0; i < 4; i++)
            {
                int new_x = blinky.x + directions[i][0];
                int new_y = blinky.y + directions[i][1];
                if (labyrinth[new_y][new_x] != WALL)
                {
                    ghost_clear();
                    blinky.x = new_x;
                    blinky.y = new_y;
                    ghost_draw();
                    break;
                }
            }
        }
    }
    // Se il fantasma ? in modalit? FRIGHTENED
    else if (blinky.mode == FRIGHTENED)
    {
        Pair opposite_pos = {HEIGHT - pacman_y - 1, WIDTH - pacman_x - 1};
        Pair next_pos;

        // Calcola il prossimo passo lontano da Pac-Man
        aStarSearch(labyrinth, ghost_pos, opposite_pos);
        next_pos = tracePath(cellDetails, opposite_pos);

        ghost_clear();
        blinky.x = next_pos.x;
        blinky.y = next_pos.y;
        ghost_draw();
    }
}
