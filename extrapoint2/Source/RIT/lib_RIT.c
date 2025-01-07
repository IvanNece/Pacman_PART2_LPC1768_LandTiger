/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           lib_RIT.h
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        atomic functions to be used by higher sw levels (file che contiene le funzioni per configurare e controllare il RIT. E' il nucleo che inizializza e gestisce il timer
** Correlated files:    lib_RIT.c, funct_RIT.c, IRQ_RIT.c
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "RIT.h"

//Il file lib_RIT.c contiene le funzioni principali per configurare, abilitare, disabilitare e resettare il Repetitive Interrupt Timer (RIT),
//fornendo un’interfaccia di basso livello per la sua gestione.

/******************************************************************************
** Function name:		enable_RIT
**
** Descriptions:		Enable RIT
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void enable_RIT( void )
{
	// Modifica il registro di controllo RICTRL del timer, impostando il bit 3 per abilitare il timer. 
  LPC_RIT->RICTRL |= (1<<3);	
  return;
}

/******************************************************************************
** Function name:		disable_RIT
**
** Descriptions:		Disable RIT
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void disable_RIT( void )
{
	LPC_RIT->RICTRL &= ~(1<<3);	
  return;
}

/******************************************************************************
** Function name:		reset_RIT
**
** Descriptions:		Reset RIT
**
** parameters:			RIT number: 0 or 1
** Returned value:		None
**
******************************************************************************/
void reset_RIT( void )
{
	//Imposta il registro RICOUNTER a 0, azzerando il contatore interno del timer.
  LPC_RIT->RICOUNTER = 0;          // Set count value to 0
  return;
}

//***********************************************************************************************************************************
//Inizializza e configura il timer RIT con un intervallo specifico.
uint32_t init_RIT ( uint32_t RITInterval )
{
  
	
  LPC_SC->PCLKSEL1  &= ~(3<<26);
  LPC_SC->PCLKSEL1  |=  (1<<26);   // RIT Clock = CCLK
	LPC_SC->PCONP     |=  (1<<16);   // Enable power for RIT
	
	//NB NB NB
	//Configura il valore di confronto del timer. Il valore passato (RITInterval) rappresenta il numero di cicli di clock che devono passare prima di generare un interrupt.
	LPC_RIT->RICOMPVAL = RITInterval;      // Set match value		
	LPC_RIT->RICTRL    = (1<<1) |    // Enable clear on match	
											 (1<<2) ;		 // Enable timer for debug	
	LPC_RIT->RICOUNTER = 0;          // Set count value to 0
	
	//Abilita gli interrupt del RIT nel Nested Vector Interrupt Controller (NVIC).
	//Imposta la priorità dell'interrupt a 0 (massima priorità).
	NVIC_EnableIRQ(RIT_IRQn);
	NVIC_SetPriority(RIT_IRQn, 0);
  return (0);
}

/******************************************************************************
**                            End Of File
******************************************************************************/
