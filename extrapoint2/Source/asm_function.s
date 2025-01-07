
			; i primi 4 parametri passati dalla funzione sono contenuti nei registri R0-R4
			; se ci fossero pi� di 4 parametri, dopo il quarto sono all'interno dello stack
			; il risultato finale � quello presente all'interno di R0

			;LDR/STR 32 bit (4 byte)
			;LDRB/STRB 8 bit (1 byte)
			;LDRH/STRH 16 bit (2 byte)
			;LDRSB 8 bit (signed 1 byte)
			;LDRSH 16 bit (signed 2 byte)
			;LDRD/STRD 2*32 bit (two words)
			;LDM/SDM multiple registers

			;unsigned char 32 bit

			;load/store <Rd>, [<Rn>], <offset> Rn è modificato aggiungendo l'offset dopo

			;SPACE 1 => alloca 1 byte (usato per riservare spazio per modificare un vettore/variabile)
			;DCB alloca uno o più byte
			;DCD alloca una o più parole, dove ogni parola è di 32 bit
			;DCW alloca una o più halfwords, dove ogni halfword è di 16 bit

			;prendere l'indirizzo di un vettore: LDR R3, =name_vector
			;prendere il contenuto sapendo l'indirizzo: LDR R2, [R3]

			;uso di EQU: name EQU 5

			;nostra area di memoria
			AREA my_area, CODE, READWRITE
			;nostra area per poter modificare le variabili in memoria

			AREA asm_functions, CODE, READONLY
			EXPORT name_function_assembler

name_function_assembler

			; save current SP for a faster access
			; to parameters in the stack
			MOV R12, SP
			; save volatile registers
			STMFD SP!, {R4-R8, R10-R11, LR}

			; my code here

			; setup a value for R0 to return
			MOV R0, R5
			; restore volatile registers
			LDMFD SP!, {R4-R8, R10-R11, PC}

;definizione literal pool
		;LTORG

;my_constant DCB 4,5,5

			END
