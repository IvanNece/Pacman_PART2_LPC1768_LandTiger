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
 
volatile uint8_t old_countdown = 0;  // Flag per tracciare se il countdown ? cambiato
volatile uint16_t old_score = 0;
volatile uint8_t old_lives = 1;
uint8_t first = 0;

extern volatile uint16_t score;
extern int countdown;
extern volatile uint8_t lives;


//can2 to receive and can1 to send 
void CAN_IRQHandler (void) {

      /* check CAN controller 2 */
    icr = 0;
    icr = (LPC_CAN2->ICR | icr) & 0xFF;  /* clear interrupts */

    if (icr & (1 << 0)) {  /* CAN Controller #2 message is received */
        CAN_rdMsg(2, &CAN_RxMsg);  /* Read the message */
        LPC_CAN2->CMR = (1 << 2);  /* Release receive buffer */

        // Controlla se il messaggio ricevuto ha l'ID 2
        if (CAN_RxMsg.id == 2) {
            // Estrai i valori dai dati del messaggio
            uint16_t score = (CAN_RxMsg.data[1] << 8) | CAN_RxMsg.data[0]; // Score: primi 2 byte
            uint8_t lives = CAN_RxMsg.data[2];                             // Lives: terzo byte
            uint8_t countdown = CAN_RxMsg.data[3];                         // Countdown: quarto byte
						
					if (first == 0){
            // Aggiorna lo schermo con i nuovi valori
						display_timer();
            display_score();
            display_lives();
						first++;
					}
					
					if(old_countdown != countdown){
						display_timer();
						old_countdown = countdown;
					}
					
					// Se il mio punteggio ? variato lo aggiorno
					if(old_score != score){
						display_score();
						old_score = score;
					}
					
					// Se ho guadagnato una vita
					if(old_lives < lives){ 
						display_lives();
						old_lives = lives;
					}
					
					// Se ho perso una vita
					if(old_lives > lives){
						display_lives();
						old_lives = lives;
					}
					
					// GAMEOVER
					if (countdown == 0 || lives == 0){
						LPC_TIM0->TCR = 0; // Stop Timer 0
						LPC_TIM1->TCR = 0; // Stop Timer 1
						LPC_TIM2->TCR = 0; // Stop Timer 2
						LPC_TIM3->TCR = 0; // Stop Timer 3
						display_game_over();
						disable_RIT();
					}
        }
	}

    if (icr & (1 << 1)) {  /* CAN Controller #2 message is transmitted */
        // Do nothing in this example
    }
}
