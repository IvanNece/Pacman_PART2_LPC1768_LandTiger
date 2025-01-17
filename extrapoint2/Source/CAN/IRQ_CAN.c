/*----------------------------------------------------------------------------
 * Name:    Can.c
 * Purpose: CAN interface for for LPC17xx with MCB1700
 * Note(s): see also http://www.port.de/engl/canprod/sv_req_form.html
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <LPC17xx.h>                  /* LPC17xx definitions */
#include "CAN.h"                      /* LPC17xx CAN adaption layer */
#include "labyrinth/labyrinth.h"
#include "../GLCD/GLCD.h"

extern uint8_t icr ; 										//icr and result must be global in order to work with both real and simulated landtiger.
extern uint32_t result;
extern CAN_msg       CAN_TxMsg;    /* CAN message for sending */
extern CAN_msg       CAN_RxMsg;    /* CAN message for receiving */                                

/*
static int puntiRicevuti1 = 0;
static int puntiInviati1 = 0;

static int puntiRicevuti2 = 0;
static int puntiInviati2 = 0;
*/

uint16_t val_RxCoordX = 0;            /* Locals used for display */
uint16_t val_RxCoordY = 0;

/*----------------------------------------------------------------------------
  CAN interrupt handler
 *----------------------------------------------------------------------------*/
 
// Variabili esterne che rappresentano lo stato corrente del gioco
extern volatile uint16_t score;
extern int countdown;
extern volatile uint8_t lives;

// Per evitare il lag
volatile uint8_t previous_countdown = 0;  // Memorizza il valore precedente del countdown per rilevare variazioni
volatile uint16_t previous_score = 0;    // Memorizza il punteggio precedente per il confronto
volatile uint8_t previous_lives = 1;     // Salva il numero di vite precedente per monitorare cambiamenti
uint8_t initialization_flag = 0;         // Utilizzato per aggiornare i valori iniziali una sola volta


void CAN_IRQHandler (void) {
			
		// CAN2 per ricevere e CAN1 per trasmettere
    /* check CAN controller 2 */
    icr = 0;
    icr = (LPC_CAN2->ICR | icr) & 0xFF;  /* clear interrupts found */

    if (icr & (1 << 0)) {  /* CAN Controller #2 message is received */
        CAN_rdMsg(2, &CAN_RxMsg);  /* Read the message */
        LPC_CAN2->CMR = (1 << 2);  /* Release receive buffer */

        // Controlla se l'ID del messaggio ricevuto corrisponde a 2
        if (CAN_RxMsg.id == 2) {
						// Estrai i dati dal messaggio ricevuto
						// Il messaggio contiene:
						// - Punteggio (Score): primi 2 byte (little-endian)
						// - Numero di vite (Lives): terzo byte
						// - Timer del conto alla rovescia (Countdown): quarto byte
            uint16_t new_score = (CAN_RxMsg.data[1] << 8) | CAN_RxMsg.data[0]; 
            uint8_t new_lives = CAN_RxMsg.data[2];                              
            uint8_t new_countdown = CAN_RxMsg.data[3];                         
						
            // Inizializza il display alla prima ricezione dei dati
						// Questa sezione garantisce che i valori iniziali del gioco vengano mostrati correttamente
            if (initialization_flag == 0) {
                display_timer();
                display_score();
                display_lives();
                initialization_flag = 1; // Evita di ripetere l'inizializzazione
            }

            // Aggiorna il timer solo se è cambiato
						// Questo controllo evita aggiornamenti inutili sul display, riducendo i lag
            if (previous_countdown != new_countdown) {
                display_timer();
                previous_countdown = new_countdown;
            }
						
						// Controlla se il numero di vite è variato (aumento o diminuzione)
						// Questo controllo aggiorna il display solo se necessario
						if (previous_lives != new_lives) {
								display_lives();
								previous_lives = new_lives;  // Aggiorna il valore salvato delle vite
						}

            // Aggiorna il punteggio solo se è diverso dal precedente
						// Minimizza l'accesso al display, evitando rallentamenti
            if (previous_score != new_score) {
                display_score();
                previous_score = new_score;
            }
					
						// Verifica se il gioco è terminato (GAME OVER)
            if (new_countdown == 0 || new_lives == 0) {
                LPC_TIM0->TCR = 0; // Ferma il Timer 0
                LPC_TIM1->TCR = 0; // Ferma il Timer 1
                display_game_over(); // Mostra il messaggio di fine gioco
            }
        }
	}

    if (icr & (1 << 1)) {  /* CAN Controller #2 message is transmitted */
        // Do nothing in this example
    }
}
