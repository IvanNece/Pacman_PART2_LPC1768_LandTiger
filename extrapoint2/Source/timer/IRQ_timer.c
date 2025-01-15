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

extern int ghost_y;
extern int ghost_x;
extern volatile int ghost_start_x;
extern volatile int ghost_start_y;
extern volatile uint8_t ghost_mode;
extern volatile uint8_t ghost_active;

extern volatile unsigned int respawn_counter;				// contatore per il respawn
extern volatile unsigned int frightened_counter;		// contatore per la modalit? spaventata

extern int next_power_pill_score;

volatile int pacman_move = 0;
volatile int ghost_move = 0;
volatile int ghost_current_speed = 10;
volatile int increase_ghost_speed_freq = 13;

extern unsigned char led_value;					/* defined in funct_led */

void TIMER0_IRQHandler (void)
{
	//LPC_TIM0->IR: Il registro di interrupt (Interrupt Register) indica quale MR ha generato l’interrupt:
	
	/* Match register 0 interrupt service routine */
	if (LPC_TIM0->IR & 01)
	{
		//code here
		// Aggiorna la posizione di Pac-Man
		pacman_update();
		pacman_move++;
		
		if (pacman_move == ghost_current_speed){
			pacman_move = 0;
			ghost_move++;
			if (ghost_move % increase_ghost_speed_freq == 0 && ghost_current_speed>0){
				ghost_current_speed--; // Incrementa la velocit? del fantasma
			}
			
			// Movimento fantasma
			if (ghost_active){
				if (ghost_mode == CHASE){
					ghost_a_star_search(ghost_y, ghost_x, pacman_y, pacman_x);
				}
				else{ 
					ghost_escape(ghost_y, ghost_x, pacman_y, pacman_x);
				}
			}
			
		}
		
		LPC_TIM0->IR = 1;			/* clear interrupt flag */
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
	if (LPC_TIM1->IR & 01)
	{	//code here
		if(countdown > 0){
			countdown--;
			// Aggiorna il display con il nuovo valore del timer
			display_timer();
		} else {
			 // Verifica lo stato del gioco
			check_game_status(); 
		}
		
				// Conta i 3 secondi per il respawn
		if(ghost_active == 0){  // se il fantasma non ? attivo
			respawn_counter -= 1;
			// Se lo mangio non mi interessa pi? il contatore frightened
			frightened_counter = 10;
			
			if(respawn_counter == 0){
				// Resetto la posizione e la modalit? del fantasma
				ghost_x = ghost_start_x;
				ghost_y = ghost_start_y;
				ghost_mode = CHASE;		
				
				// Disegno il fantasma sulla mappa
				draw_ghost(ghost_y, ghost_x);
				
				respawn_counter = 3;
				// Riattivo il movimento del fantasma
				ghost_active = 1;
			}
		}
		
		if (ghost_mode == FRIGHTENED){ // Ghost mode = FRIGHTENED
			frightened_counter -= 1;
			if (frightened_counter == 0){
				frightened_counter = 10;
				ghost_mode = CHASE;
			}
		}
		
		LPC_TIM1->IR = 1;			/* clear interrupt flag */
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
void TIMER2_IRQHandler (void)
{
  /* Match register 0 interrupt service routine */
	if (LPC_TIM2->IR & 01)
	{	//code here
			
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
