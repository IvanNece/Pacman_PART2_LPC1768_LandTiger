/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
IRQ_timer.c gestisce gli interrupt generati dai timer hardware (Timer0, Timer1, Timer2, Timer3).
Ogni timer ha una Interrupt Service Routine (ISR) che viene eseguita automaticamente quando il timer raggiunge il valore di match configurato.
Questo file si occupa di gestire i comportamenti specifici da eseguire in corrispondenza di tali eventi.
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "timer.h"
#include "../led/led.h"

/*
Ogni ISR ha lo stesso schema di base:

Controlla quale Match Register (MR) ha generato l’interrupt.
Esegue le azioni associate al Match Register corrispondente.
Resetta il flag di interrupt per consentire nuovi eventi.

Ogni timer ha 4 Match Register (MR0, MR1, MR2, MR3) che possono essere configurati per generare eventi a determinati intervalli.
*/


/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

#define WIDTH 28
#define HEIGHT 31
#define CHASE 0
#define FRIGHTENED 1

extern int countdown;
//extern volatile uint8_t countdown_changed;
extern int pacman_direction;
extern int labyrinth[HEIGHT][WIDTH];

extern int pacman_x;
extern int pacman_y;

extern int blinky_pos_y;
extern int blinky_pos_x;
extern volatile int ghost_start_x;
extern volatile int ghost_start_y;
extern volatile uint8_t blinky_behavior_mode;
extern volatile uint8_t blinky_is_active;

extern volatile unsigned int blinky_respawn_timer;				// contatore per il respawn
extern volatile unsigned int blinky_scared_timer;		// contatore per la modalit? spaventata

extern int next_power_pill_score;

volatile int pacman_move = 0;
volatile int ghost_move = 0;
volatile int ghost_current_speed = 7;
volatile int increase_ghost_speed_freq = 10;

extern unsigned char led_value;					/* defined in funct_led */

void TIMER0_IRQHandler (void)
{
	//LPC_TIM0->IR: Il registro di interrupt (Interrupt Register) indica quale MR ha generato l’interrupt:
	
	/* Match register 0 interrupt service routine */
		
	if (LPC_TIM0->IR & 0x01) { // Controlla se il timer 0 ha generato un interrupt
    // Aggiorna lo stato e la posizione di Pac-Man
    pacman_update(); // Funzione per gestire il movimento e le interazioni di Pac-Man
    pacman_move++;   // Incrementa il contatore dei passi di Pac-Man

    // Controlla se Pac-Man ha completato il suo movimento e attiva il fantasma
    if (pacman_move == ghost_current_speed) {
        pacman_move = 0;      // Resetta il contatore dei passi di Pac-Man
        ghost_move++;         // Incrementa il contatore dei passi del fantasma

        // Aumenta la velocità del fantasma a intervalli definiti
        if ((ghost_move % increase_ghost_speed_freq == 0) && ghost_current_speed > 0) {
            if (ghost_current_speed <= 1) {
								//LINEA IMPORTANTE, SENNO IL GHOST DIVENETREBBE VELOCE QUANTO PACMAN, E SAREBBE INGIOCABILE
                ghost_current_speed = 2; // Limita la velocità minima del fantasma
            } else {
                ghost_current_speed--;   // Riduce il tempo tra i movimenti del fantasma, accelerandolo
            }
        }

        // Gestisce il movimento del fantasma in base al suo stato attuale
        if (blinky_is_active) { // Controlla se il fantasma è attivo
            if (blinky_behavior_mode == CHASE) {
                // Modalità di inseguimento: calcola il percorso verso Pac-Man
                perform_a_star_search(blinky_pos_y, blinky_pos_x, pacman_y, pacman_x);
            } else {
                // Modalità di fuga: calcola il percorso lontano da Pac-Man
                initiate_ghost_escape(blinky_pos_y, blinky_pos_x, pacman_y, pacman_x);
            }
        }
    }

    LPC_TIM0->IR = 1; // Resetta il flag di interrupt del timer 0 per permettere nuovi interrupt
	}

		/* Match register 1 interrupt service routine */
	  /* it should be possible to access to both interrupt requests in the same procedure*/
	if(LPC_TIM0->IR & 02)
  {
	
		LPC_TIM0->IR =  2 ;			/* clear interrupt flag */	
	}
	/* Match register 2 interrupt service routine */
  /* it should be possible to access to both interrupt requests in the same procedure*/
	if(LPC_TIM0->IR & 4)
  {
		
		LPC_TIM0->IR =  4 ;			/* clear interrupt flag */	
	}
		/* Match register 3 interrupt service routine */
  	/* it should be possible to access to both interrupt requests in the same procedure*/
	if(LPC_TIM0->IR & 8)
  {
	 
		LPC_TIM0->IR =  8 ;			/* clear interrupt flag */	
	}
  return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
#include "labyrinth/labyrinth.h"
void TIMER1_IRQHandler (void)
{
  /* Match register 0 interrupt service routine */
		if (LPC_TIM1->IR & 0x01) {  // Controlla se il timer 1 ha generato un interrupt
    
			// Decrementa il timer del conto alla rovescia se maggiore di zero
			if (countdown > 0) {
					countdown--;  // Riduce il valore del timer
					// Aggiorna il display con il nuovo valore del countdown
					// display_timer();
			} else {
					// Quando il countdown raggiunge zero, verifica lo stato del gioco
					check_game_status();
			}

			// Gestisce il timer per il respawn del fantasma se non è attivo
			if (blinky_is_active == 0) {  // Verifica se il fantasma è inattivo
					changeGameMode(0);  // Cambia la modalità musicale a "CHASE"

					blinky_respawn_timer--;  // Decrementa il timer di respawn
					blinky_scared_timer = 10;  // Resetta il timer della modalità "spaventato"

					if (blinky_respawn_timer == 0) {
							// Resetta la posizione del fantasma alla posizione iniziale
							blinky_pos_x = ghost_start_x;
							blinky_pos_y = ghost_start_y;

							// Ripristina la modalità di inseguimento del fantasma
							blinky_behavior_mode = CHASE;

							// Ridisegna il fantasma sulla mappa
							draw_ghost(blinky_pos_y, blinky_pos_x);

							// Resetta il timer di respawn e riattiva il fantasma
							blinky_respawn_timer = 3;
							blinky_is_active = 1;  // Attiva il movimento del fantasma
					}
    }

    // Gestisce la modalità "spaventato" del fantasma
    if (blinky_behavior_mode == FRIGHTENED) {
        blinky_scared_timer--;  // Decrementa il timer della modalità spaventata

        if (blinky_scared_timer == 0) {
            // Quando il timer frightened scade, ritorna alla modalità "CHASE"
            blinky_scared_timer = 10;  // Resetta il timer per eventuali future interazioni
            blinky_behavior_mode = CHASE;  // Cambia modalità a inseguimento
        }
    }

    LPC_TIM1->IR = 1;  // Resetta il flag di interrupt del timer 1
	}

		/* Match register 1 interrupt service routine */
	  /* it should be possible to access to both interrupt requests in the same procedure*/
	if(LPC_TIM1->IR & 02)
  {
	
		LPC_TIM1->IR =  2 ;			/* clear interrupt flag */	
	}
	/* Match register 2 interrupt service routine */
  /* it should be possible to access to both interrupt requests in the same procedure*/
	if(LPC_TIM1->IR & 4)
  {
		
		LPC_TIM1->IR =  4 ;			/* clear interrupt flag */	
	}
		/* Match register 3 interrupt service routine */
  	/* it should be possible to access to both interrupt requests in the same procedure*/
	if(LPC_TIM1->IR & 8)
  {
	 
		LPC_TIM1->IR =  8 ;			/* clear interrupt flag */	
	}
  return;
}

/******************************************************************************
** Function name:		Timer2_IRQHandler
**
** Descriptions:		Timer/Counter 2 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

uint16_t SinTable[45] =                                     
{
    410, 467, 523, 576, 627, 673, 714, 749, 778,
    799, 813, 819, 817, 807, 789, 764, 732, 694, 
    650, 602, 550, 495, 438, 381, 324, 270, 217,
    169, 125, 87 , 55 , 30 , 12 , 2  , 0  , 6  ,   
    20 , 41 , 70 , 105, 146, 193, 243, 297, 353
};

void TIMER2_IRQHandler (void)
{
  /* Match register 0 interrupt service routine */
	if (LPC_TIM2->IR & 01)
	{	//code here
		
		//code from music template
		static int sineticks=0;
		
		/* DAC management */
		static int currentValue; 
		currentValue = SinTable[sineticks];
		currentValue -= 410;
		currentValue /= 1;
		currentValue += 410;
		LPC_DAC->DACR = currentValue <<6;
		sineticks++;
		if(sineticks==45) sineticks=0;
			
		LPC_TIM2->IR = 1;			/* clear interrupt flag */
	}
		/* Match register 1 interrupt service routine */
	  /* it should be possible to access to both interrupt requests in the same procedure*/
	if(LPC_TIM2->IR & 02)
  {
	
		LPC_TIM2->IR =  2 ;			/* clear interrupt flag */	
	}
	/* Match register 2 interrupt service routine */
  /* it should be possible to access to both interrupt requests in the same procedure*/
	if(LPC_TIM2->IR & 4)
  {
		
		LPC_TIM2->IR =  4 ;			/* clear interrupt flag */	
	}
		/* Match register 3 interrupt service routine */
  	/* it should be possible to access to both interrupt requests in the same procedure*/
	if(LPC_TIM2->IR & 8)
  {
	 
		LPC_TIM2->IR =  8 ;			/* clear interrupt flag */	
	}
  return;
}

/******************************************************************************
** Function name:		Timer3_IRQHandler
**
** Descriptions:		Timer/Counter 3 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER3_IRQHandler (void)
{
  /* Match register 0 interrupt service routine */
	if (LPC_TIM3->IR & 01)
	{
		//code from music template
		disable_timer(2);
		disable_timer(3);
		
		LPC_TIM3->IR = 1;			/* clear interrupt flag */
	}
		/* Match register 1 interrupt service routine */
	  /* it should be possible to access to both interrupt requests in the same procedure*/
	if(LPC_TIM3->IR & 02)
  {
	
		LPC_TIM3->IR =  2 ;			/* clear interrupt flag */	
	}
	/* Match register 2 interrupt service routine */
  /* it should be possible to access to both interrupt requests in the same procedure*/
	if(LPC_TIM3->IR & 4)
  {
		
		LPC_TIM3->IR =  4 ;			/* clear interrupt flag */	
	}
		/* Match register 3 interrupt service routine */
  	/* it should be possible to access to both interrupt requests in the same procedure*/
	if(LPC_TIM3->IR & 8)
  {
	 
		LPC_TIM3->IR =  8 ;			/* clear interrupt flag */	
	}
  return;
}



/******************************************************************************
**                            End Of File
******************************************************************************/
