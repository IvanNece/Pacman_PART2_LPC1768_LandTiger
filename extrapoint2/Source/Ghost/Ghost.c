#include "Ghost.h"
#include "Pacman/Pacman.h"

int ghost_x = 15;
int ghost_y = 14;
volatile uint8_t ghost_mode = CHASE;  // Se ? in stato 0 segue pacman ed ? rosso, viceversa scappa ed ? ciano
volatile uint8_t ghost_active = 1; // Fantasma non ? mangiato

volatile unsigned int respawn_counter = 3;				// contatore per il respawn
volatile unsigned int frightened_counter = 7;		// contatore per la modalit? spaventata

extern int offset_x;
extern int offset_y;

extern int pacman_x;
extern int pacman_y;

volatile int ghost_start_x = 15;
volatile int ghost_start_y = 14;

volatile int pacman_start_x = 14;
volatile int pacman_start_y = 23;

extern volatile uint8_t lives;

extern volatile uint16_t score;

extern int labyrinth[HEIGHT][WIDTH];

void draw_ghost(int i, int j) {
    int x0, y0, x, y;
    int radius = CELL_SIZE / 2; // Raggio della testa del fantasma
    int radius_squared = radius * radius;

    // Calcola il centro della cella in cui si trova il fantasma
    x0 = j * CELL_SIZE + (CELL_SIZE / 2) + offset_x;
    y0 = i * CELL_SIZE + (CELL_SIZE / 2) + offset_y;
		
    for (x = -radius; x <= radius; x++) {
        for (y = -radius; y <= radius; y++) {
            if (x * x + y * y <= radius_squared) {
                LCD_SetPoint(x0 + x, y0 + y, get_ghost_color());
            }
        }
    }
}


void clear_ghost(int i, int j) {
    int x0, y0, x, y;
    int radius = CELL_SIZE / 2; // Raggio della testa del fantasma
    int radius_squared = radius * radius;

    // Calcola il centro della cella in cui si trova il fantasma
    x0 = j * CELL_SIZE + (CELL_SIZE / 2) + offset_x;
    y0 = i * CELL_SIZE + (CELL_SIZE / 2) + offset_y;

		// Cancella il cerchio ridisegnando la cella con il colore di sfondo
    for (x = -radius; x <= radius; x++) {
        for (y = -radius; y <= radius; y++) {
            if (x * x + y * y <= radius_squared) {
                LCD_SetPoint(x0 + x, y0 + y, EMPTY_COLOR);
            }
        }
    }
		
		// perchè il fantasma non deve magiare le pill su cui passa, quindi le ridisegno
		draw_pill(i, j);
}

int get_ghost_color(){
	if (ghost_mode == FRIGHTENED){
		return Cyan;
	}
	
	return Red;
}



// MOVIMENTO DEL FANTASMA ---------------------------------------------------------------------------
// Chiamato da a_star_search(...) che passa come argomento le nuove coordinate del fantasma
void move_ghost(int new_y, int new_x){
	clear_ghost(ghost_y, ghost_x);
	ghost_y = new_y;
	ghost_x = new_x;
	// Se il fantasma ? in modalit? SEARCH e le sue coordinate sono le stesse di pacman
	if (ghost_mode == CHASE && pacman_y == ghost_y && pacman_x == ghost_x){
		eat_pacman();
	}
	if (ghost_mode == FRIGHTENED && pacman_y == ghost_y && pacman_x == ghost_x){
			eat_ghost();
		}
	draw_ghost(ghost_y, ghost_x);
}


void eat_pacman(){
	disable_timer(0);
	// Cancello pacman
	pacman_clear();
	
	// Resetta la posizione di pacman
	pacman_x = pacman_start_x;
	pacman_y = pacman_start_y;
	
	//remove_life(lives);
	lives--;
	
	pacman_draw();
	enable_timer(0);
	
	// GAMEOVER
	if (lives == 0){
		LPC_TIM0->TCR = 0; // Stop Timer 0
		LPC_TIM1->TCR = 0; // Stop Timer 1
		LPC_TIM2->TCR = 0; // Stop Timer 2
		LPC_TIM3->TCR = 0; // Stop Timer 3
		display_game_over();
	}
}


void eat_ghost(){
	// Cancello il fantasma
	clear_ghost(ghost_y, ghost_x);
	
	// Resetto la posizione del fantasma
	ghost_x = ghost_start_x;
	ghost_y = ghost_start_y;
	
	// Incremento lo score
	score += 100;
	display_score();
	
	lives++;
	display_lives();
	
	// Disabilito il movimento del fantasma
	ghost_active = 0;
}


void activate_ghost_escape(){
	// Attiva la modalit? "frightened" per il fantasma
    ghost_mode = FRIGHTENED; 
		// Imposta il tempo (in secondi) per cui il fantasma deve essere spaventato
		frightened_counter = 10;
}






