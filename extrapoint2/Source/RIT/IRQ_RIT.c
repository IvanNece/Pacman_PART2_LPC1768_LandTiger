/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "RIT.h"
#include "../led/led.h"
#include "../timer/timer.h"
#include "../GLCD/GLCD.h"
#include "joystick/joystick.h"
#include "Pacman/Pacman.h"

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler (file che gestisce la ISR (interrupt service routine) per il RIT), ie codice eseguito ogni volta che il timer genera una interrupt
**
** parameters:			None
** Returned value:		None
**
**  Ruolo complessivo del file
Joystick: Permette di rilevare pressioni e movimenti del joystick per aggiornare, ad esempio, la posizione di un oggetto o navigare nei menu.
Pulsanti: Gestisce eventi hardware esterni come la pausa (INT0) o azioni particolari nei pulsanti KEY1 e KEY2.
Gestione periodica: La ISR viene chiamata con una cadenza temporale specificata, garantendo un controllo periodico delle periferiche.
******************************************************************************/

//Variabili gloabili VOLATILI che tracciano lo stato dei pulsanti
//Indicano per quanto tempo il pulsante è stato premuto e sono usate per evitare ripetizioni multiple dello stesso evento (DEBOUNCING SOFTWARE)
volatile int down_0 = 0;
volatile int down_1 = 0;
volatile int down_2 = 0;

//Una variabile esterna che potrebbe rappresentare lo stato dei LED sul sistema. Non è manipolata direttamente nella ISR ma può essere aggiornata altrove.
extern char led_value;

//VARIABILE GLOBALE DI PAUSA DEL GIOCO
volatile int game_paused = 0; // 0: Pausa disattiva all'avvio

//VARIABILE TIMER ABILITATI
static int timers_enabled = 0; // Flag per controllare se i timer sono stati attivati

// Questa è la ISR associata al timer RIT. Ogni volta che il timer scade, questa funzione viene eseguita.
void RIT_IRQHandler (void)
{					
	static int J_select = 0;
	static int J_down = 0;
	static int J_left = 0;
	static int J_right = 0;
	static int J_up = 0;
	
	
/* JOYSTICK MANAGEMENT ******************************************************************************************************************************************************************/
	//Gestione del joystick, Il codice verifica lo stato dei pin GPIO associati ai movimenti del joystick. Ogni direzione è associata a un bit specifico dei registri GPIO.
	
	
	if((LPC_GPIO1->FIOPIN & (1<<25)) == 0 || // J_Select
    (LPC_GPIO1->FIOPIN & (1<<26)) == 0 || // J_Down
    (LPC_GPIO1->FIOPIN & (1<<27)) == 0 || // J_Left
    (LPC_GPIO1->FIOPIN & (1<<28)) == 0 || // J_Right
    (LPC_GPIO1->FIOPIN & (1<<29)) == 0)   // J_Up 
	{   
    if (timers_enabled == 0) {
        enable_timer(0); // Abilita il Timer0
        enable_timer(1); // Abilita il Timer1
				//enable_timer(2);
        timers_enabled = 1; // Setta la flag per non eseguire più questa parte
    }
	}
	
	// Legge il valore del registro dei pin GPI01 e controllare il 25 bit associato al movimento J-Select, ==0 vuol dire premuto
	if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){	
		/* Joytick J_Select pressed p1.25*/
		
		//Il contatore J_select viene incrementato ogni volta che il pin rimane basso (pulsante premuto). Questo serve per prevenire il rimbalzo dei contatti
		//o per distinguere tra pressioni brevi e lunghe.
		J_select++;
		switch(J_select){
			case 1:
				//Esegui azione al primo scatto del joystick
 				//code here
			   pacman_set_direction(STOP); // Ferma Pac-Man
				break;
			default:
				break;
		}
	}
	else{
		//Quando il joystick viene rilasciato, il contatore viene resettato:
			J_select=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){	
		/* Joytick J_Down pressed p1.26 --> using J_DOWN due to emulator issues*/
		J_down++;
		switch(J_down){
			case 1:
				//code here
			  pacman_set_direction(DOWN); // Sposta Pac-Man verso il basso
				break;
			default:
				break;
		}
	}
	else{
			J_down=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){	
		/* Joytick J_Left pressed p1.27*/
		J_left++;
		switch(J_left){
			case 1:
				//code here
			  pacman_set_direction(LEFT); // Sposta Pac-Man a sinistra
				break;
			default:
				break;
		}
	}
	else{
			J_left=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){	
		/* Joytick J_right pressed p1.28*/
		
		J_right++;
		switch(J_right){
			case 1:
				//code here
			  pacman_set_direction(RIGHT); // Sposta Pac-Man a destra
				break;
			default:
				break;
		}
	}
	else{
			J_right=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	
		/* Joytick J_up pressed p1.29*/
		
		J_up++;
		switch(J_up){
			case 1:
				//code here
			  pacman_set_direction(UP); // Sposta Pac-Man verso l'alto
				break;
			default:
				break;
		}
	}
	else{
			J_up=0;
	}
	
	// Aggiorna la posizione di Pac-Man
  //pacman_update();
	
/* BUTTON MANAGEMENT ******************************************************************************************************************************************************************/
	//Gestione dei pulsanti (INT0, KEY1, KEY2)
	//Ogni pulsante viene gestito con logica simile al joystick, utilizzando variabili contatrici (down_0, down_1, down_2) per monitorare lo stato dei pulsanti e implementare un debouncing.
	
	//INT0 (pin 2.10):
	//Gli interrupt esterni associati a INT0 vengono disabilitati quando il pulsante è premuto, e riabilitati al rilascio.
	if(down_0 != 0){
		down_0++; 
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){	/* INT0 pressed */			
			switch(down_0){
				case 2:		
					// Azione al secondo scatto del pulsante
					//code here
					game_paused = !game_paused;
					if (game_paused) {
							// Mostra il messaggio di pausa con posizione centrata e dimensioni ristrette
						  disable_RIT();
							GUI_Text_Narrow(103, 148, (uint8_t *)"PAUSE", White, Black);
							enable_RIT();
							disable_timer(0);
							disable_timer(1);
					} else {
							// Cancella il messaggio PAUSE con un rettangolo più stretto
							LCD_ClearWindow(103, 148, 35, 15, Black); // Rettangolo più piccolo
							enable_timer(0);
							enable_timer(1);
					}
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down_0=0;			
			NVIC_EnableIRQ(EINT0_IRQn);							 /* RE-enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */ /* Ripristina configurazione del pin */
		}
	}
	
	//KEY1 (pin 2.11)
	if(down_1!=0){ 
		down_1++;
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){	/* KEY1 pressed */			
			switch(down_1){
				case 2:				
					
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down_1=0;			
			NVIC_EnableIRQ(EINT1_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
		}
	}
	
	//KEY2 (pin 2.12)
	if(down_2 != 0){
		down_2++;	
		if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){	/* KEY2 pressed */			
			switch(down_2){
				case 2:		
					//code here
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down_2=0;			
			NVIC_EnableIRQ(EINT2_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
		}
		
	}
	
/* RESET AND CLEANING******************************************************************************************************************************************************************/
	//Funzione che ferma e riavvia il timer (definita in lib_RIT.c).
	reset_RIT();
	//Resetta il flag di interrupt per permettere al timer di generare nuovi interrupt.
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
