#include "button.h"
#include "LPC17xx.h"
#include "../RIT/RIT.h"

/*
Il file IRQ_button.c contiene le Interrupt Service Routine (ISR) per la gestione degli interrupt generati dai pulsanti INT0, KEY1 e KEY2.
Quando uno di questi pulsanti viene premuto, viene generato un interrupt esterno che attiva la rispettiva ISR. Queste routine si occupano di:

Disabilitare temporaneamente l'interrupt del pulsante.
Segnalare lo stato del pulsante tramite variabili globali.
Resettare il flag dell'interrupt per consentire nuovi eventi.
Resettare il RIT (Repetitive Interrupt Timer), se necessario.
*/

extern int down_0;
extern int down_1;
extern int down_2;

void EINT0_IRQHandler (void)	  	/* INT0														 */
{		
	//Disabilita temporaneamente gli interrupt per EINT0. Questo evita che nuovi eventi vengano generati prima che l’attuale sia completamente gestito.
	NVIC_DisableIRQ(EINT0_IRQn);		/* disable Button interrupts			 */
	//Configura il pin associato a INT0 (P2.10) come GPIO invece che come sorgente di interrupt.
	LPC_PINCON->PINSEL4    &= ~(1 << 20);     /* GPIO pin selection */
	//Aggiorna la variabile globale down_0 per indicare che il pulsante INT0 è stato premuto.
	down_0=1;
	//Resetta il flag dell’interrupt per EINT0 nel registro EXTINT, consentendo nuovi eventi in futuro.
	LPC_SC->EXTINT &= (1 << 0);     /* clear pending interrupt         */
	//Resetta il timer RIT, probabilmente per sincronizzare eventi o avviare un nuovo ciclo di temporizzazione.
	reset_RIT();
}


void EINT1_IRQHandler (void)	  	/* KEY1														 */
{
	NVIC_DisableIRQ(EINT1_IRQn);		/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4    &= ~(1 << 22);     /* GPIO pin selection */
	down_1=1;
	LPC_SC->EXTINT &= (1 << 1);     /* clear pending interrupt         */
	reset_RIT();
}

void EINT2_IRQHandler (void)	  	/* KEY2														 */
{
	NVIC_DisableIRQ(EINT2_IRQn);		/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4    &= ~(1 << 24);     /* GPIO pin selection */
	down_2=1;
  LPC_SC->EXTINT &= (1 << 2);     /* clear pending interrupt         */ 
	reset_RIT();
}


