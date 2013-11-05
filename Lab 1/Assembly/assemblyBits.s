	AREA algorithms, CODE, READONLY
	IMPORT encodedArray
	EXPORT encryptAssemblyWrapper
	EXPORT decryptAssemblyWrapper


encryptAssemblyWrapper
	;Loop through array and call encrypt
	;R11 = encodedArray's address
	LDR R11, =encodedArray				;Address of the space made stored in R11
	POP {R2-R5}							;Pop the keys off of the stack
	POP{R0, R1}							;Pop off the first two words
	PUSH{LR}							;Save context of LR
	
	;Loop based off of how char arrays in C work. Loop until encountering a 0x00 in the last bits of R0
EncWrapLoop
	;Need register for =Encrypt, Test Section, D0, D1
	;R12 = Tester register
	;R11 = Position in encodedArray
	;R10 = Encrypt
	;R0 = D0, D0_Next
	;R1 = D1, D1_Next
	LDR R10, =Encrypt					;Load Encrypt's address into R10
	BLX R10								;Branch to Encrypt
	STR R0, [R11]						;Store R0 in encodedArray
	STR R1, [R11, #4]						;Store R1 in encodedArray
	ADD R11, R11, #8					;Advance position in encodedArray by 4
	POP{LR}								;Get LR out of the way of the next two words							
	POP {R0,R1}							;Pop next two words off of the stack
	PUSH{LR}							;Resave the context of LR
	
	;Test Section
	MOV R12, R0							;Move the first word into R12
	AND R12, R12, #0x000000FF			;R12 = R12 AND 0x000000FF
	CMP R12, #0							;If result is 0, we've reached the end of the character array (last char in char[] is 0x00)
	BNE EncWrapLoop						;If they're not equal, back to the top
	PUSH{R2-R5}							;Store keys on stack for return to C
	BX LR								;Branch to original LR

Encrypt
	;STILL TO DO: OPTIMIZE
	PUSH{R11, R12}				;Save context of wrapper
	LDR R12, =0x9E3779B9		;Move delta into R12
	;MOV R12, #0x79B9
	;MOVT R12, #0x9E37				;R12 = delta
	MOV R11, #0						;R11 = sum
	MOV R10, #32					;R10 = count = 32 initially
	
	;R0 = MESSAGE 0 (D0)
	;R1 = MESSAGE 1 (D1)
	;R2 = KEY 0
	;R3 = KEY 1
	;R4 = KEY 2
	;R5 = KEY 3
	;R6 = T4
	;R7 = Scratch register (ends up representing T2 and T3)
EncLoop
	ADD R11, R11, R12				;Sum = sum + delta
	LSL R6, R1, #4					;Shift D1 left 4, put in T4
	ADD R6, R6, R2					;Add K0 to T4. T4 now equal to T1
	LSR R7, R1, #5					;Shift D1 right 5 and put in T_Scratch
	ADD R7, R7, R3					;Add K1 to T_Scratch. T_Scratch now equal to T2
	EOR R6, R6, R7					;T4 = T4(Now eq T1) XOR T_Scratch(Now eq T2)
	ADD R7, R1, R11					;T_Scratch = D1 + Sum. T_Scratch now eq to T3
	EOR R6, R6, R7					;T4 = T4 XOR T_Scratch(Now eq T3)
	ADD R0, R0, R6					;D0 = D0 + T4
	;End D0 Section
	LSL R6, R0, #4					;Shift D0 left 4, put in T4
	ADD R6, R6, R4					;ADD K2 TO T4. T4 now eq T1
	LSR R7, R0, #5					;SHIFT D0 RIGHT 5, PUT IN T_Scratch
	ADD R7, R7, R5					;ADD K3 TO T_Scratch. T_Scratch now eq T2
	EOR R6, R6, R7					;T4 = T4(Now eq T1) XOR T_Scratch(Now eq T2) 
	ADD R7, R0, R11					;T_Scratch = D0 + SUM. T_Scratch now eq T3
	EOR R6, R6, R7					;T4 = T4 XOR T_Scratch(Now eq T3)
	ADD R1, R1, R6					;D1 = D1 + T4
	;End D1 Section
	SUB R10, R10, #1				;COUNT--
	CMP R10, #0						;IF COUNT != 0
	BNE EncLoop						;BRANCH EncLoop
	LDR R12, =EndEnc
	BX R12

decryptAssemblyWrapper
	LDR R11, =encodedArray			;Store address of encodedArray
	POP {R2-R5}						;Pop keys off of stack
	LDR R0, [R11]					;Load first word from encodedArray
	LDR R1, [R11, #4]					;Load second word
	PUSH{LR}
DecWrapLoop
	LDR R10, =Decrypt				;Store Decrypt address
	BLX R10							;Branch there
	STR R0, [R11]					;Overwrite encoded first word
	STR R1, [R11,#4]				;Overwrite encoded second word
	ADD R11, R11, #8				;Advance two words in encodedArray
	LDR R0, [R11]					;Load first word
	LDR R1, [R11, #4]				;Load second word
	MOV R12, R0						;Move first word into R12
	AND R12, R12, #0x000000FF		;R12 = R12 AND 0x000000FF
	CMP R12, #0						;R12==0?
	BNE DecWrapLoop					;If not, back to DecWrapLoop
	POP{LR}
	PUSH{R2-R5}							;Store keys on stack for return to C
	BX LR							;If true, back to LR stored above
	
Decrypt
	PUSH{R11, R12}				;Save context of wrapper
	LDR R12, =0x9E3779B9		;Move delta into R12
	;MOV R12, #0x79B9
	;MOVT R12, #0x9E37				;R12 = delta
	;MOV R11, #0x3720				;R11 = sum
	;MOVT R11, #0xC6EF
	LDR R11, =0xC6EF3720		;Move delta into R12
	MOV R10, #32					;R10 = count = 32 initially
	
DecLoop
	;R0 = MESSAGE 0 (D0)
	;R1 = MESSAGE 1 (D1)
	;R2 = KEY 0
	;R3 = KEY 1
	;R4 = KEY 2
	;R5 = KEY 3
	;R6 = T4
	;R7 = T_Scratch
	LSL R6, R0, #4					;Shift D0 left by 4 and put into T4
	ADD R6, R6, R4					;Add K2 to T1. At this point T4 = T1
	LSR R7, R0, #5					;Shift D0 right 5 and put in T_Scratch
	ADD R7, R7, R5					;Add K3 to T_Scratch. T_Scratch = T2
	EOR R6, R6, R7					;T4 = T4 XOR T_Scratch(T2)
	ADD R7, R0, R11					; T_Scratch = D0 + sum = T3
	EOR R6, R6, R7					;T4 = T4 XOR T_Scratch(Now eq T3)
	SUB R1, R1, R6					;D1 = D1-T4
	;End D1 section
	
	LSL R6, R1, #4					;Shift D1 left by 4 and place into T4
	ADD R6, R6, R2					;Add K0 to T4. T4 now eq T1
	LSR R7, R1, #5					;Shift D1 right by 5 and place into T_Scratch
	ADD R7, R7, R3					;Add K1 to T_Scratch. T_Scratch now eq T2
	EOR R6, R6, R7					;T4 = T4 XOR T_Scratch(T2)
	ADD R7, R1, R11					;T_Scratch=D1+sum. T_Scratch now eq T3
	EOR R6, R6, R7					;T4 = T4 XOR T_Scratch(T3)
	SUB R0, R0, R6					;D0 = D0-T4
	;End D0 section
	SUB R11, R11, R12				;Sum = sum-delta
	SUB R10, R10, #1				;COUNT--
	CMP R10, #0						;IF COUNT != 0
	BNE DecLoop						;BRANCH EncLoop
EndEnc
	POP{R11, R12}
	BX LR
	;Ending stuff here, probably same for decrypt
	;message DCB "Stufuf"
	END
	
	