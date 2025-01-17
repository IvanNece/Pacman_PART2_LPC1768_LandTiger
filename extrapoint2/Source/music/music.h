#ifndef MUSIC_H
#define MUSIC_H

// Definizione del moltiplicatore per la velocità delle note
// Questo valore consente di accelerare o rallentare la riproduzione musicale
#define SPEEDUP 1.6

// Fattore di scalatura per i timer
#define TIMERSCALER 1

// Definizione della durata di un secondo basata sul valore scalato del timer
#define SECOND 0x17D7840 * TIMERSCALER

// Tipo booleano per una maggiore leggibilità
typedef char BOOL;
#define TRUE 1  // Valore booleano per "vero"
#define FALSE 0 // Valore booleano per "falso"

// Enumerazione per definire la durata delle note musicali
typedef enum note_durations
{
    time_semibiscroma = (unsigned int)(SECOND * SPEEDUP / 64.0f + 0.5), // 1/128 della durata di un secondo
    time_biscroma = (unsigned int)(SECOND * SPEEDUP / 32.0f + 0.5),    // 1/64
    time_semicroma = (unsigned int)(SECOND * SPEEDUP / 16.0f + 0.5),   // 1/32
    time_croma = (unsigned int)(SECOND * SPEEDUP / 8.0f + 0.5),        // 1/16
    time_semiminima = (unsigned int)(SECOND * SPEEDUP / 4.0f + 0.5),   // 1/4
    time_minima = (unsigned int)(SECOND * SPEEDUP / 2.0f + 0.5),       // 1/2
    time_semibreve = (unsigned int)(SECOND * SPEEDUP + 0.5)            // Durata di un secondo
} NOTE_DURATION;

// Enumerazione per le frequenze delle note musicali
// Ogni valore rappresenta un periodo del timer associato alla nota
typedef enum frequencies
{
    a2b = 5351,  // Frequenza 103Hz, identificata dal valore del timer
    b2 = 4500,   // Frequenza 123Hz
    c3b = 4370,  // Frequenza 127Hz
    c3 = 4240,   // Frequenza 131Hz
    d3 = 3779,   // Frequenza 147Hz
    e3 = 3367,   // Frequenza 165Hz
    f3 = 3175,   // Frequenza 175Hz
    g3 = 2834,   // Frequenza 196Hz
    a3b = 2670,  // Frequenza 208Hz
    a3 = 2525,   // Frequenza 220Hz
    b3 = 2249,   // Frequenza 247Hz
    c4 = 2120,   // Frequenza 262Hz
    d4 = 1890,   // Frequenza 294Hz
    e4 = 1684,   // Frequenza 330Hz
    f4 = 1592,   // Frequenza 349Hz
    g4 = 1417,   // Frequenza 392Hz
    a4 = 1263,   // Frequenza 440Hz
    b4 = 1125,   // Frequenza 494Hz
    c5 = 1062,   // Frequenza 523Hz
    pause = 0    // Valore per rappresentare una pausa (nessun suono)
} FREQUENCY;

// Struttura per rappresentare una nota musicale
// Include la frequenza e la durata della nota
typedef struct 
{
    FREQUENCY freq;        // Frequenza della nota
    NOTE_DURATION duration; // Durata della nota
} NOTE;

// Dichiarazioni delle funzioni per la gestione della musica
void playNote(NOTE note);   // Funzione per riprodurre una nota
BOOL isNotePlaying(void);   // Funzione per verificare se una nota è in riproduzione

#endif
/* EOF */
