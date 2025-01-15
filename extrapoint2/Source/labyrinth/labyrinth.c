#include <stdio.h>
#include "math.h"
#include "labyrinth.h"
#include "GLCD/GLCD.h"
#include "RIT/RIT.h"

volatile uint16_t score = 0;
int countdown = 60;  // Countdown iniziale a 60 secondi
volatile uint8_t lives = 1;

int power_pills_generated = 0; // 0 = non generate, 1 = già generate
int next_power_pill_score = 350; // Prima soglia per generare una Power Pill



int offset_x = (240 - WIDTH * CELL_SIZE) / 2;    // Offset orizzontale centrato
int offset_y = (320 - HEIGHT * CELL_SIZE) / 2;   // Offset verticale centrato


//int return_element(int x, int y) {
    //if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
  //      int element = labyrinth[y][x];
    //    return element;
    //}
    //return -1;
//}



// Funzione per disegnare il labirinto
void draw_labyrinth(int labyrinth[HEIGHT][WIDTH]) {
    int x, y;  // Coordinate pixel
    int i, j;  // Iteratori sulla matrice
		int px, py;


    for (i = 0; i < HEIGHT; i++) {       // Itera sulle righe
        for (j = 0; j < WIDTH; j++) {    // Itera sulle colonne
						x = offset_x + j * CELL_SIZE; // Posizione X della cella centrata
            y = offset_y + i * CELL_SIZE; // Posizione Y della cella centrata
            
            switch (labyrinth[i][j]) {
                case WALL:
                    // Disegna il muro come un blocco pieno
                    for (px = 0; px < CELL_SIZE; px++) {
                        for (py = 0; py < CELL_SIZE; py++) {
                            LCD_SetPoint(x + px, y + py, WALL_COLOR);
                        }
                    }
                    break;

                case STANDARD_PILL:
										// Disegna una pillola standard più spessa e arancione
                    for (px = -1; px <= 1; px++) {
                        for (py = -1; py <= 1; py++) {
                            if (px * px + py * py <= 1) {  // Condizione per rimanere nel cerchio
                                LCD_SetPoint(x + CELL_SIZE / 2 + px, y + CELL_SIZE / 2 + py, PILL_COLOR);
                            }
                        }
                    }
                    break;

                case POWER_PILL:
                    // Disegna la power pill più spessa e rossa
                    for (px = -3; px <= 3; px++) {
                        for (py = -3; py <= 3; py++) {
                            if (px * px + py * py <= 9) {  // Condizione per rimanere nel cerchio
                                LCD_SetPoint(x + CELL_SIZE / 2 + px, y + CELL_SIZE / 2 + py, POWER_PILL_COLOR);
                            }
                        }
                    }
                    break;

                default:
                    // Disegna lo sfondo nero per le celle vuote
                    for (px = 0; px < CELL_SIZE; px++) {
                        for (py = 0; py < CELL_SIZE; py++) {
                            LCD_SetPoint(x + px, y + py, EMPTY_COLOR);
                        }
                    }
                    break;
            }
        }
    }
}


void draw_pacman_icon(int x, int y, int radius, uint16_t color) {
    int i, j;

    // Disegna il cerchio completo
    for (i = -radius; i <= radius; i++) {
        for (j = -radius; j <= radius; j++) {
            // Controlla se il punto (i, j) è all'interno del cerchio
            if (i * i + j * j <= radius * radius) {
                // Disegna il punto (x + i, y + j) con il colore specificato
                LCD_SetPoint(x + i, y + j, color);
            }
        }
    }
}



void display_game_info(void) {
		int i;
	
    // Pulisci la parte superiore e inferiore per le informazioni
    LCD_Clear(BACKGROUND_COLOR);

    // Mostra il countdown timer in alto a sinistra
		//disable_RIT();
    GUI_Text(10, 10, (uint8_t *)"TIMER:", TEXT_COLOR, BACKGROUND_COLOR);
		//enable_RIT();
    char timer_str[10];
    sprintf(timer_str, "%02d", countdown);
		//disable_RIT();
    GUI_Text(70, 10, (uint8_t *)timer_str, TEXT_COLOR, BACKGROUND_COLOR);
		//enable_RIT();

    // Mostra lo score in alto a destra
		//disable_RIT();
    GUI_Text(140, 10, (uint8_t *)"SCORE:", TEXT_COLOR, BACKGROUND_COLOR);
		//enable_RIT();
    char score_str[10];
    sprintf(score_str, "%d", 0);
		//disable_RIT();
    GUI_Text(210, 10, (uint8_t *)score_str, TEXT_COLOR, BACKGROUND_COLOR);
		//enable_RIT();

    // Mostra le vite rimanenti in basso
		//disable_RIT();
    GUI_Text(10, 300, (uint8_t *)"LIVES:", TEXT_COLOR, BACKGROUND_COLOR);
		//enable_RIT();
    for (i = 0; i < lives; i++) {
        // Disegna l'icona Pac-Man per ogni vita
        draw_pacman_icon(70 + (i * 20), 310, 8, Yellow);  // Posizione centrata, raggio 8
    }
}

void display_timer(void) {
    char timer_str[10]; // Buffer per la stringa
    sprintf(timer_str, "%02d", countdown); // Converte il valore in stringa

    // Cancella la vecchia scritta e mostra il nuovo countdown
		disable_RIT();
    GUI_Text(70, 10, (uint8_t *)"    ", TEXT_COLOR, BACKGROUND_COLOR); // Pulisce l'area
    GUI_Text(70, 10, (uint8_t *)timer_str, TEXT_COLOR, BACKGROUND_COLOR);
		enable_RIT();
}

void display_score(void) {
    char score_str[10];
    sprintf(score_str, "%d", score); // Converte il punteggio in stringa

    // Pulisce la vecchia area dello score
    LCD_ClearWindow(210, 10, 50, 15, BACKGROUND_COLOR);

    // Mostra lo score aggiornato
    //GUI_Text(210, 10, (uint8_t *)"SCORE:", TEXT_COLOR, BACKGROUND_COLOR);
		disable_RIT();
    GUI_Text(190, 10, (uint8_t *)score_str, TEXT_COLOR, BACKGROUND_COLOR);
		enable_RIT();
	
		/*Il disable_RIT() e enable_RIT() intorno a GUI_Text garantiscono che la funzione di aggiornamento del display non
  	interferisca con il timer ripetitivo. Questo è fondamentale per evitare "flickering" sul display.*/
}

void display_lives(void) {
    int i;
		
		disable_RIT();
    //Cancella l'area delle vite
    LCD_ClearWindow(70, 300, 100, 20, BACKGROUND_COLOR);
		enable_RIT();

    // Disegna le vite aggiornate
    for (i = 0; i < lives; i++) {
			
        draw_pacman_icon(70 + (i * 20), 310, 8, PACMAN_COLOR);
				
    }
}


void generate_power_pills(void) {
    int x, y, attempts, px, py;

    // Inizializza il generatore casuale con un seed basato sullo score e il timer
    srand(score + LPC_TIM0->TC);

    // Trova una posizione valida casualmente
    for (attempts = 0; attempts < 100; attempts++) { // Limita il numero di tentativi
        x = rand() % WIDTH;
        y = rand() % HEIGHT;

        if (labyrinth[y][x] == STANDARD_PILL) {
            labyrinth[y][x] = POWER_PILL; // Sostituisci la pillola standard con una Power Pill

            // Calcola la posizione del pixel per la cella interessata
            int pixel_x = offset_x + x * CELL_SIZE + CELL_SIZE / 2;
            int pixel_y = offset_y + y * CELL_SIZE + CELL_SIZE / 2;

            // Disegna la nuova Power Pill direttamente
            for (px = -3; px <= 3; px++) {
                for (py = -3; py <= 3; py++) {
                    if (px * px + py * py <= 9) { // Condizione per rimanere nel cerchio
												disable_RIT();
                        LCD_SetPoint(pixel_x + px, pixel_y + py, POWER_PILL_COLOR);
												enable_RIT();
                    }
                }
            }

            power_pills_generated++; // Incrementa il conteggio delle Power Pills generate
            return;                  // Esci dalla funzione dopo aver generato la Power Pill
        }
    }
}


void check_game_status(void) {
    int i, j;
    int remaining_pills = 0;

    // Controlla se ci sono ancora pillole nel labirinto
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            if (labyrinth[i][j] == STANDARD_PILL || labyrinth[i][j] == POWER_PILL) {
                remaining_pills++;
            }
        }
    }

    // Coordinate centrali del labirinto
    int center_x = offset_x + (224 / 2);
    int center_y = offset_y + (248 / 2);

    if (remaining_pills == 0) {
        // Vittoria
        disable_RIT();
				GUI_Text(center_x - 32, center_y, (uint8_t *)"Victory!", White, Black); // 64 pixel di larghezza testo
				disable_timer(0);
				disable_timer(1);
        return;
    }

    if (countdown == 0) {
        // Game Over
        disable_RIT();
				disable_timer(0);
				disable_timer(1);
        GUI_Text(center_x - 40, center_y, (uint8_t *)"Game Over!", Red, Black); // 80 pixel di larghezza testo
        return;
    }
}

void display_game_over(){
	
	// Coordinate centrali del labirinto
    int center_x = offset_x + (224 / 2);
    int center_y = offset_y + (248 / 2);
	
		// Game Over
		disable_RIT();
		disable_timer(0);
		disable_timer(1);
		GUI_Text(center_x - 40, center_y, (uint8_t *)"Game Over!", Red, Black); // 80 pixel di larghezza testo
}


void draw_pill(int i, int j){
	int x0, y0, x, y;
	
	if(labyrinth[i][j] == 2){
		x0 = offset_x + j * CELL_SIZE + (CELL_SIZE / 2);
		y0 = offset_y + i * CELL_SIZE + (CELL_SIZE / 2);
		
		LCD_SetPoint(x0, y0, PILL_COLOR);
	}
	
	if(labyrinth[i][j] == 3){ // Assumi che '2' rappresenti le bonus pills
		// Centro della cella
		x0 = j * CELL_SIZE + (CELL_SIZE / 2) + offset_x;
		y0 = i * CELL_SIZE + (CELL_SIZE / 2)+ offset_y;
		
		// Disegna un "cerchio" (approssimato) pi? grande
		for(y = y0 - 2; y <= y0 + 2; y++){ // Raggio bonus pill (adatta se necessario)
				for(x = x0 - 2; x <= x0 + 2; x++){
						LCD_SetPoint(x, y, POWER_PILL_COLOR); // Colore della bonus pill
				}
		}
	}
	
	
}


