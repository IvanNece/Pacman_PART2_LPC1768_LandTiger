#include "Ghost.h"
#include "Pacman/Pacman.h"

// Definizione delle variabili globali per il fantasma
int blinky_pos_x = 15;  // Posizione iniziale X del fantasma
int blinky_pos_y = 14;  // Posizione iniziale Y del fantasma
volatile uint8_t blinky_behavior_mode = CHASE;  // Modalità iniziale del fantasma (inseguimento o fuga)
volatile uint8_t blinky_is_active = 1;  // Indica se il fantasma è attivo o "mangiato"

volatile unsigned int blinky_respawn_timer = 3;  // Timer per il respawn del fantasma
volatile unsigned int blinky_scared_timer = 7;  // Timer per la modalità frightened

volatile int ghost_start_x = 15;
volatile int ghost_start_y = 14;

volatile int pacman_start_x = 12;
volatile int pacman_start_y = 17;

// Variabili esterne per sincronizzare le posizioni e stati di Pac-Man
extern int offset_x;
extern int offset_y;

extern int pacman_x;
extern int pacman_y;

extern volatile uint8_t lives;

extern volatile uint16_t score;

extern int labyrinth[HEIGHT][WIDTH];

//**************************************************************************************************************************************

// Funzione per disegnare il fantasma nella sua nuova posizione
void draw_ghost(int row, int col) {
    int center_x, center_y, dx, dy;
    int radius = CELL_SIZE / 2; // Determina il raggio del cerchio che rappresenta il fantasma
    int radius_squared = radius * radius; // Calcola il quadrato del raggio per semplificare i calcoli

    // Calcola le coordinate centrali della cella in cui posizionare il fantasma
    center_x = col * CELL_SIZE + (CELL_SIZE / 2) + offset_x;
    center_y = row * CELL_SIZE + (CELL_SIZE / 2) + offset_y;

    // Disegna il fantasma come un cerchio colorato
    for (dx = -radius; dx <= radius; dx++) {
        for (dy = -radius; dy <= radius; dy++) {
            if (dx * dx + dy * dy <= radius_squared) {
                LCD_SetPoint(center_x + dx, center_y + dy, fetch_blinky_color()); // Colora il pixel
            }
        }
    }
}

// Funzione per cancellare il fantasma dalla schermata
void clear_ghost(int row, int col) {
    int center_x, center_y, dx, dy;
    int radius = CELL_SIZE / 2; // Determina il raggio del cerchio del fantasma
    int radius_squared = radius * radius;

    // Calcola le coordinate centrali della cella da cui eliminare il fantasma
    center_x = col * CELL_SIZE + (CELL_SIZE / 2) + offset_x;
    center_y = row * CELL_SIZE + (CELL_SIZE / 2) + offset_y;

    // Ripristina il colore di sfondo nella regione precedentemente occupata dal fantasma
    for (dx = -radius; dx <= radius; dx++) {
        for (dy = -radius; dy <= radius; dy++) {
            if (dx * dx + dy * dy <= radius_squared) {
                LCD_SetPoint(center_x + dx, center_y + dy, BACKGROUND_COLOR); // Cancella il pixel
            }
        }
    }

    // Ridisegna eventuali pillole sotto il fantasma per ripristinare il labirinto 
		//(FANTASMA NON DEVE MANGIARE LE PILLOLE)
    draw_pill(row, col);
}

// Funzione per gestire il movimento del fantasma
void move_ghost(int new_row, int new_col) {
    clear_ghost(blinky_pos_y, blinky_pos_x); // Cancella il fantasma dalla posizione attuale
    blinky_pos_y = new_row;  // Aggiorna la coordinata Y del fantasma
    blinky_pos_x = new_col;  // Aggiorna la coordinata X del fantasma

    // Gestisce l'interazione tra Pac-Man e il fantasma
    if (blinky_behavior_mode == CHASE && pacman_y == blinky_pos_y && pacman_x == blinky_pos_x) {
        kill_pacman(); // Pac-Man viene "mangiato" dal fantasma
    } else if (blinky_behavior_mode == FRIGHTENED && pacman_y == blinky_pos_y && pacman_x == blinky_pos_x) {
        kill_ghost(); // Il fantasma viene "mangiato" da Pac-Man
    }

    draw_ghost(blinky_pos_y, blinky_pos_x); // Ridisegna il fantasma nella nuova posizione
}

// Funzione per gestire la "cattura" di Pac-Man da parte del fantasma
void kill_pacman() {
    disable_timer(0); // Ferma temporaneamente il timer di gioco

    pacman_clear(); // Cancella Pac-Man dalla schermata
    pacman_x = pacman_start_x; // Riposiziona Pac-Man nella sua posizione iniziale
    pacman_y = pacman_start_y;

    lives--; // Decrementa il numero di vite disponibili

    pacman_draw(); // Ridisegna Pac-Man nella posizione iniziale
    enable_timer(0); // Riattiva il timer di gioco

    // Verifica se Pac-Man ha esaurito tutte le vite
    if (lives == 0) {
        LPC_TIM0->TCR = 0; // Ferma il timer 0
        LPC_TIM1->TCR = 0; // Ferma il timer 1
        display_game_over(); // Mostra la schermata di "Game Over"
    }
}

// Funzione per gestire la "cattura" del fantasma da parte di Pac-Man
void kill_ghost() {
    clear_ghost(blinky_pos_y, blinky_pos_x); // Cancella il fantasma dalla schermata

    // Resetta la posizione del fantasma alla sua posizione iniziale
    blinky_pos_x = ghost_start_x;
    blinky_pos_y = ghost_start_y;

    score += 100; // Incrementa il punteggio di Pac-Man

    blinky_is_active = 0; // Disabilita temporaneamente il fantasma
}

// Funzione per attivare la modalità "spaventata" per il fantasma
void activate_blinky_scared_mod() {
    blinky_behavior_mode = FRIGHTENED; // Imposta il fantasma in modalità "spaventata"
    changeGameMode(1); // Cambia la musica di gioco alla modalità spaventata
    blinky_scared_timer = 10; // Imposta il timer per la durata della modalità spaventata
}

// Funzione per determinare il colore del fantasma in base alla sua modalità attuale
int fetch_blinky_color() {
    if (blinky_behavior_mode == FRIGHTENED) {
        return Cyan; // Ritorna il colore per la modalità "spaventata"
    }
    return Red; // Ritorna il colore per la modalità "inseguimento"
}
