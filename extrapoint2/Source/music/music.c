#include "music.h"
#include "LPC17xx.h"
#include "../timer/timer.h"

// Funzione per riprodurre una singola nota musicale
void playNote(NOTE note)
{
    // Controlla che la frequenza della nota sia valida (non sia una pausa)
    if(note.freq != pause)
    {
        reset_timer(2);  // Resetta il timer 2 per garantire un inizio pulito
        init_timer(2, 0, 0, 3, note.freq);  // Configura il timer 2 con la frequenza della nota
        enable_timer(2);  // Abilita il timer 2 per iniziare a riprodurre la frequenza
    }

    reset_timer(3);  // Resetta il timer 3 per la durata della nota
    init_timer(3, 0, 0, 3, note.duration);  // Configura il timer 3 per il tempo di riproduzione
    enable_timer(3);  // Abilita il timer 3 per controllare la durata della nota
}

// Funzione per verificare se una nota è ancora in riproduzione
BOOL isNotePlaying(void)
{
    // Controlla lo stato dei timer associati alla riproduzione della nota
    // Se uno dei due timer è attivo, la nota è in riproduzione
    return ((LPC_TIM2->TCR != 0) || (LPC_TIM3->TCR != 0));
}
