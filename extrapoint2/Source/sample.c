/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               
** Descriptions:            
**
**--------------------------------------------------------------------------------------------------------
** Created by:             
** Created date:            
** Version:                 
** Descriptions:            
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "GLCD/GLCD.h"
#include "GLCD/HzLib.h"
#include "TouchPanel/TouchPanel.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "button_EXINT/button.h"
#include "joystick/joystick.h"
#include "led/led.h"
#include "labyrinth/labyrinth.h"
#include "Pacman/Pacman.h"
#include "music/music.h"

#define SIMULATOR 1

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif

/*
* COSE DA RICORDARE
*
* per funzione assembler, definirla nel file asm_functions.s e definire il la funzione come extern
*
* timer2 e timer3 attivare tramite wizard in system_LPC17xx.c in Power Control for Peripherals Register
*
* La frequenza nel nostro caso ? 25MHz = 25.000.000 Hz
* count = [s] * [Hz]
* 1 ms = 10^-3 s       1 ns = 10^-9 s
* 1 MHz = 10^6 Hz
*
* quando si chiede un led che lampeggi si pu? usare:
* - met? valore del match register per raggiugere il periodo richiesto
* - si dimezza il periodo richiesto ed utilizziamo una variabile locale per gestire l'on e l'off
*
* inizializzazione timer: init_timer(uint8_t timer_num, uint32_t Prescaler, uint8_t MatchReg, uint8_t SRImatchReg, uint32_t TimerInterval)
* dove SRImatchReg :
* - 1? bit (LSB) : interrupt
* - 2? bit       : reset
* - 3? bit (MSB) : stop
* (di default il registro MSR ? settato a 3 (interrup e reset)
*
* shift sinistro : <<
* shift destro : >>
*
* per prendere il valore del TimerCounter (TC) con il Timer 1:
*       val_int = LPC_TIM1 -> TC
*/

//scheletro funzione assembler
extern void name_function_assembler(int r0, int r1, int r2, int r3);

extern int ghost_x;
extern int ghost_y;

// Display landtinger è 240x320 pixel
// La mappa disegnata è di 28*8+31*8 =  224x248 pixel
// Labirinto
int labyrinth[HEIGHT][WIDTH] = {
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
{1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
{1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1},
{1, 2, 1, 0, 0, 0, 1, 2, 1, 0, 0, 1, 2, 1, 1, 2, 1, 0, 0, 1, 2, 1, 0, 0, 0, 1, 2, 1},
{1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1},
{1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
{1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1},
{1, 2, 1, 0, 0, 1, 2, 1, 1, 2, 1, 1, 0, 0, 0, 0, 1, 1, 2, 1, 1, 2, 1, 0, 0, 1, 2, 1},
{1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 2, 1, 0, 0, 0, 0, 1, 2, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1},
{1, 2, 2, 2, 2, 2, 2, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 2, 2, 2, 2, 2, 1}, 
{1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 2, 2, 0, 0, 2, 2, 2, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1},
{0, 0, 0, 0, 0, 1, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 1, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 1, 2, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 2, 1, 0, 0, 0, 0, 0},
{1, 1, 1, 1, 1, 1, 2, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 2, 1, 1, 1, 1, 1, 1},
{0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0}, 
{1, 1, 1, 1, 1, 1, 2, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 2, 1, 1, 1, 1, 1, 1},
{0, 0, 0, 0, 0, 1, 2, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 2, 1, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 1, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 1, 0, 0, 0, 0, 0},
{1, 1, 1, 1, 1, 1, 2, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 2, 1, 1, 1, 1, 1, 1},
{1, 2, 2, 2, 2, 2, 2, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 2, 2, 2, 2, 2, 2, 1}, 
{1, 2, 1, 1, 1, 1, 2, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 2, 1, 1, 1, 1, 2, 1},
{1, 2, 1, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 2, 1, 0, 0, 1, 2, 1},
{1, 2, 1, 0, 0, 1, 2, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 2, 1, 0, 0, 1, 2, 1},
{1, 2, 1, 0, 0, 1, 2, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 2, 1, 0, 0, 1, 2, 1},
{1, 2, 1, 0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 0, 1, 2, 1}, 
{1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1},
{1, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 1, 0, 0, 1, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 1},
{1, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0, 2, 1, 0, 0, 1, 2, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 1},
{1, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0, 2, 1, 1, 1, 1, 2, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 1},
{1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 1}, 
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}	
};

int main(void)
{
  SystemInit();  												/* System Initialization (i.e., PLL)  */
	BUTTON_init();												/* Inizializzazione Buttons 					*/
  //LCD_Initialization();								/* Inizializzazione Display 					*/
	//TP_Init();													/* Inizializzazione TouchPanel 				*/
	joystick_init();											/* Inizializzazione Joystick 					*/
	//LED_init();
	//TouchPanel_Calibrate();							/* Calibrazione touch display         */
	CAN_Init();
	
	//Inizializzazione display
	LCD_Initialization();
	LCD_Clear(Black); 
	
	//init_timer(nr.timer, prescaler, nr.match.register MRnum, SRI (stop reset interrupt), timerInterval del match register)
	init_timer(0, 0, 0, 3, 0x2FAF08); 	// Timer0 inizializzazione con periodo 0,125 sec
	//funct(nr timer da abilitare)
	//enable_timer(0);
	
	init_timer(1, 0, 0, 3, 0x017D7840); 	// Timer1 inizializzazione con periodo 1 sec
	//enable_timer(1);
	
	//l'abilitazione dei timer è fatta nel IRQ_RIT.c
	
		/* power-down	*/
	LPC_SC->PCON |= 0x1;								
	LPC_SC->PCON &= ~(0x2);
	
	//INIZIALIZZAZIONE DAC
	LPC_PINCON->PINSEL1 |= (1<<21);
	LPC_PINCON->PINSEL1 &= ~(1<<20);
	LPC_GPIO0->FIODIR |= (1<<26);
	
/***********************************************************************************************************************/
	//Disegno il labirinto con le varie etichette
	display_game_info();
	draw_labyrinth(labyrinth);  
	draw_ghost(ghost_y, ghost_x);
	//Il gioco parte in modalità CHASE, deve selezionare la musica corretta
	changeGameMode(0);
	
	init_RIT(0x004C4B40);									/* RIT Initialization 50 msec       	*/
	enable_RIT();

	pacman_init();
	
  while (1)	
  {
		__ASM("wfi");
  }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
