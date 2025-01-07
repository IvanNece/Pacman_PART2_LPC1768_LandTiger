
#include "button.h"
#include "LPC17xx.h"

/**
 * @brief  Function that initializes Buttons
 La funzione BUTTON_init configura i pin GPIO associati ai pulsanti (INT0, KEY1, KEY2)
 per essere utilizzati come linee di interrupt. Questo consente di rilevare eventi hardware,
 come la pressione o il rilascio dei pulsanti, in modo efficiente e asincrono.
 */
 
 /*
	Comportamento del sistema
		Dopo aver chiamato BUTTON_init, il sistema è configurato per gestire gli interrupt generati dai pulsanti:
		Quando uno dei pulsanti viene premuto, l’interrupt associato (EINT0, EINT1, EINT2) viene attivato.
		Il controllo passa alla rispettiva ISR definita in IRQ_button.c, dove puoi gestire l’evento.

 */
 
void BUTTON_init(void) {

	//1. Configurazione dei pin GPIO
	//Ogni pulsante è associato a un pin del microcontrollore configurato come ingresso e come sorgente di interrupt.
	
	//PINSEL4: Configura il pin P2.10 per funzionare come sorgente di interrupt EINT0.
	//FIODIR: Imposta il pin P2.10 come ingresso.
  LPC_PINCON->PINSEL4    |= (1 << 20);		 /* External interrupt 0 pin selection */
  LPC_GPIO2->FIODIR      &= ~(1 << 10);    /* PORT2.10 defined as input          */

  LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
  LPC_GPIO2->FIODIR      &= ~(1 << 11);    /* PORT2.11 defined as input          */
  
  LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
  LPC_GPIO2->FIODIR      &= ~(1 << 12);    /* PORT2.12 defined as input          */

	//2. Configurazione della modalità di interrupt
	/*
		0x7 corrisponde a:
		Bit 0: Abilita edge-sensitive per EINT0.
		Bit 1: Abilita edge-sensitive per EINT1.
		Bit 2: Abilita edge-sensitive per EINT2.
	*/
  LPC_SC->EXTMODE = 0x7;

	//3. Abilitazione degli interrupt nel NVIC
	//NVIC_SetPriority: Imposta la priorità dell’interrupt. Un valore inferiore corrisponde a una priorità più alta.
	NVIC_EnableIRQ(EINT0_IRQn);              /* enable irq in nvic                 */
	//NVIC_SetPriority(EINT0_IRQn, 0);				 /* decreasing priority	from EINT2->0	 */
  NVIC_EnableIRQ(EINT1_IRQn);              /* enable irq in nvic                 */
	//NVIC_SetPriority(EINT1_IRQn, 0);				 
	NVIC_EnableIRQ(EINT2_IRQn);              /* enable irq in nvic                 */
	//NVIC_SetPriority(EINT2_IRQn, 0);				 /* priority, the lower the better     */
}
