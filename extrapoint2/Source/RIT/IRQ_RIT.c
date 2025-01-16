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
#include "music/music.h"
#include "CAN/CAN.h"

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

/***********************************************************************************************************************
GESTIONE MUSICA
***********************************************************************************************************************/

// Variabile globale per la modalità corrente
typedef enum {
    MODE_CHASE = 0,
    MODE_FRIGHTENED = 1,
    MODE_GAME_OVER = 2
} GameMode;

GameMode current_mode = 0;


// Definizione delle nuove melodie per le modalità di gioco Pac-Man
NOTE chase_mode[] = {
    // Melodia incalzante per modalità Chase
    {c4, time_biscroma},
    {e4, time_biscroma},
    {g4, time_biscroma},
    {c5, time_biscroma},
    {g4, time_biscroma},
    {e4, time_biscroma},
    {c4, time_biscroma},
    {pause, time_semicroma},
    {c4, time_biscroma},
    {d4, time_biscroma},
    {f4, time_biscroma},
    {a4, time_biscroma},
    {f4, time_biscroma},
    {d4, time_biscroma},
    {c4, time_biscroma}
};


NOTE frightened_mode[] = {
    {c4, time_biscroma},  // Nota iniziale
    {d4, time_biscroma},  // Leggera salita
    {c4, time_biscroma},  // Ritorno alla base
    {d4, time_biscroma},  // Ripetizione
    {pause, time_semicroma}, // Pausa breve
    {e4, time_biscroma},  // Piccola variazione verso l'alto
    {d4, time_biscroma},  // Ritorno
    {c4, time_biscroma},  // Base
    {pause, time_semicroma}, // Pausa breve
    {c3, time_biscroma},  // Tono basso per enfatizzare la tensione
    {d3, time_biscroma},  // Leggera salita
    {c3, time_biscroma},  // Ritorno
    {pause, time_semicroma}, // Pausa finale
};



NOTE game_over[] = {
    // Melodia breve per Game Over
    {g3, time_semicroma},
    {f3, time_semicroma},
    {e3, time_semicroma},
    {d3, time_semicroma},
    {c3, time_croma},
    {pause, time_semicroma},
    {c3, time_croma}
};

// Array per le lunghezze delle melodie (inizializzazione manuale)
int melody_lengths[] = {15, 13, 7}; // Chase, Frightened, Game Over

void changeGameMode(GameMode new_mode)
{
    if (current_mode != new_mode) { // Cambia modalità solo se è diversa
        current_mode = new_mode;   // Imposta la nuova modalità
        // Resetta l'indice della nota per iniziare dall'inizio della nuova melodia
        static int currentNote = 0;
    }
}

/***********************************************************************************************************************
***********************************************************************************************************************/

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
		
		//PREPARO IL MS CAN CHE DEVO INVIARE SU CAN2
		//Copia il valore del countdown nel campo data (in formato little-endian)
		CAN_TxMsg.data[0] = (score & 0xFF);            // Byte meno significativo
		CAN_TxMsg.data[1] = (score >> 8) & 0xFF;       // Byte successivo
		CAN_TxMsg.data[2] = lives & 0xFF;      // Terzo byte
		CAN_TxMsg.data[3] = countdown & 0xFF;      // Byte pi? significativo
		CAN_TxMsg.id = 2;  // ID del messaggio (ad esempio, 21)
		CAN_TxMsg.len = 4;  // Lunghezza dei dati (4 byte per countdown)
		CAN_TxMsg.format = STANDARD_FORMAT;  // Identificatore standard
		CAN_TxMsg.type = DATA_FRAME;  // Tipo di frame (DATA FRAME)
		// Invia il messaggio da CAN1 a CAN2
		CAN_wrMsg(1, &CAN_TxMsg);  // Trasmetti il messaggio da CAN1

	
		// Variabili globali per la musica
		static int currentNote = 0; // Indice della nota corrente
		static int ticks = 0;       // Contatore per i tick

    if (!isNotePlaying()) {
        ++ticks;
        if (ticks == 1) {
            ticks = 0;

            // Suona la nota in base alla modalità corrente
            switch (current_mode) {
                case MODE_CHASE:
                    playNote(chase_mode[currentNote++ % melody_lengths[0]]); // 15 è la lunghezza di chase_mode
                    //currentNote++;
                    break;

                case MODE_FRIGHTENED:
                    playNote(frightened_mode[currentNote++ % melody_lengths[1]]); 
                    //currentNote++;
                    break;

                case MODE_GAME_OVER:
                    playNote(game_over[currentNote++ % melody_lengths[2]]); 
                    //currentNote++;
                    break;

                default:
                    break;
            }
        }
    }

	
	
	static int J_select = 0;
	static int J_down = 0;
	static int J_left = 0;
	static int J_right = 0;
	static int J_up = 0;
	
/* JOYSTICK MANAGEMENT ******************************************************************************************************************************************************************/
	//Gestione del joystick, Il codice verifica lo stato dei pin GPIO associati ai movimenti del joystick. Ogni direzione è associata a un bit specifico dei registri GPIO.
	
	 
	 //**** SE VIENE USATO IL JOYSTICK, IL GIOCO PARTE E TIMER VENGONO ABILITATI***********************
	
	if((LPC_GPIO1->FIOPIN & (1<<25)) == 0 || // J_Select
    (LPC_GPIO1->FIOPIN & (1<<26)) == 0 || // J_Down
    (LPC_GPIO1->FIOPIN & (1<<27)) == 0 || // J_Left
    (LPC_GPIO1->FIOPIN & (1<<28)) == 0 || // J_Right
    (LPC_GPIO1->FIOPIN & (1<<29)) == 0)   // J_Up 
	{   
    if (timers_enabled == 0) {
        enable_timer(0); // Abilita il Timer0
        enable_timer(1); // Abilita il Timer1
				enable_timer(2);
        timers_enabled = 1; // Setta la flag per non eseguire più questa parte
    }
	}
	
	//*************************************************************************************************
	
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
