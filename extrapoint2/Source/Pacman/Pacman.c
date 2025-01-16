#include "Pacman.h"
#include "labyrinth/labyrinth.h"
#include <stdio.h>
#include "Ghost/Ghost.h"

// Variabili globali
int pacman_x;
int pacman_y;
int pacman_direction;
int pacman_desired_direction = STOP;

// Aggiungi una variabile statica per tracciare l'ultima soglia di punteggio
static int score_threshold = 0;

extern volatile int game_paused;
extern int labyrinth[HEIGHT][WIDTH];
extern int next_power_pill_score;

extern int ghost_x;
extern int ghost_y;
extern volatile uint8_t ghost_mode; 

// Funzione per inizializzare Pac-Man
void pacman_init(void) {
    pacman_x = PACMAN_START_X;
    pacman_y = PACMAN_START_Y;
    pacman_direction = STOP;
    pacman_draw();
}

// Funzione per disegnare Pac-Man
void pacman_draw(void) {
    extern int offset_x;  // Usa l'offset globale definito in labyrinth.c
    extern int offset_y;

    int pixel_x = offset_x + pacman_x * CELL_SIZE + CELL_SIZE / 2; // Centrato sulla cella
    int pixel_y = offset_y + pacman_y * CELL_SIZE + CELL_SIZE / 2;

    draw_pacman_icon(pixel_x, pixel_y, PACMAN_RADIUS, PACMAN_COLOR);
}

// Funzione per cancellare Pac-Man
void pacman_clear(void) {
    extern int offset_x;  // Usa l'offset globale definito in labyrinth.c
    extern int offset_y;

		int i, j;
	
    int pixel_x = offset_x + pacman_x * CELL_SIZE + CELL_SIZE / 2; // Centrato sulla cella
    int pixel_y = offset_y + pacman_y * CELL_SIZE + CELL_SIZE / 2;

    // Cancella Pac-Man ridisegnando il colore dello sfondo
    draw_pacman_icon(pixel_x, pixel_y, PACMAN_RADIUS, EMPTY_COLOR);
}


// Aggiorna il movimento di Pac-Man
void pacman_update(void) {
	
		if (game_paused) {
        return; // Se il gioco è in pausa, non aggiornare Pac-Man
    }	
	
    int next_x = pacman_x;
    int next_y = pacman_y;
		
		// Controllo per applicare la direzione desiderata se possibile
    switch (pacman_desired_direction) {
        case UP:
            if (labyrinth[next_y-1][next_x] != WALL) {
                pacman_direction = UP;  // Aggiorna la direzione effettiva
            }
            break;
        case DOWN:
            if (labyrinth[next_y+1][next_x] != WALL) {
                pacman_direction = DOWN;
            }
            break;
        case LEFT:
            if (labyrinth[next_y][next_x-1] != WALL) {
                pacman_direction = LEFT;
            }
            break;
        case RIGHT:
            if (labyrinth[next_y][next_x+1]!= WALL) {
                pacman_direction = RIGHT;
            }
            break;
        default:
            break;
    }

		
		 // Controllo teletrasporto ai bordi
    if (next_x <= 0) {
        next_x = WIDTH - 1;  // Teletrasporto a destra
    } else if (next_x >= (WIDTH-1)) {
        next_x = 0;  // Teletrasporto a sinistra
    }
		
    // Calcola la prossima posizione
    switch (pacman_direction) {
        case UP:    next_y--; break;
        case DOWN:  next_y++; break;
        case LEFT:  next_x--; break;
        case RIGHT: next_x++; break;
				case STOP: 	break;
				default: break;
    }
		
    int element = labyrinth[next_y][next_x];
    if (element != WALL) {
        pacman_clear();  // Cancella la posizione precedente
        pacman_x = next_x;
        pacman_y = next_y;
			
		if (ghost_mode == CHASE && pacman_y == ghost_y && pacman_x == ghost_x){
			eat_pacman();
		}
		if (ghost_mode == FRIGHTENED && pacman_y == ghost_y && pacman_x == ghost_x){
			eat_ghost();
		}
			
        pacman_draw();   // Disegna Pac-Man nella nuova posizione
			
				// Controlla se Pac-Man ha mangiato una pillola
        if (labyrinth[pacman_y][pacman_x] == STANDARD_PILL) {
            score += 10;
            labyrinth[pacman_y][pacman_x] = EMPTY; // Rimuove la pillola
            //display_score();
        } else if (labyrinth[pacman_y][pacman_x] == POWER_PILL) {
            score += 50;
						activate_ghost_escape();
						//changeGameMode(1);
            labyrinth[pacman_y][pacman_x] = EMPTY; // Rimuove la power pill
            //display_score();
        }
				
				
				// **Controllo per aggiungere una vita ogni 1000 punti**
        if (score >= score_threshold + 1000) {
            lives++;
            score_threshold += 1000;  // Aggiorna il punteggio della soglia
            //display_lives();          // Aggiorna la visualizzazione delle vite
        }
				
				if (score >= next_power_pill_score && power_pills_generated < 6) {
					generate_power_pills();  // Genera una nuova Power Pill
					next_power_pill_score += 100; // Incrementa la soglia per la prossima Power Pill
				}
			
    } else {
        return; 
    }
		
		check_game_status();

}


// Funzione per cambiare la direzione
void pacman_set_direction(int new_direction) {
    pacman_desired_direction = new_direction;
}

