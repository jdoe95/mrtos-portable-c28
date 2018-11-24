;-----------------------------------------------------------
; Global symbols defined in this file
;-----------------------------------------------------------
	.def _osport_idle
	.def _osport_breakpoint
	.def _osport_enable_int
	.def _osport_disable_int
	.def _osport_contextsw_req
	.def _osport_start
	.def _ISR_RTOSINT
	.def _ISR_USER1

;-----------------------------------------------------------
; Global symbols referenced in this file
;-----------------------------------------------------------
	.ref __stack
	.ref _g_sch
	.ref _CPUTIMER2

	.sect ".text"
;-----------------------------------------------------------
; The do nothing function
; Puts the CPU into a low power sleep mode unitl interrupt
;-----------------------------------------------------------
_osport_idle:
$L1:
	IDLE
	B $L1, UNC

;-----------------------------------------------------------
; Halts the debugger
;-----------------------------------------------------------
_osport_breakpoint:
	ESTOP1
	LRETR

;-----------------------------------------------------------
; Disables interrupts
;-----------------------------------------------------------
_osport_disable_int:
	; cache flushed automatically
	DINT
	LRETR

;-----------------------------------------------------------
; Enables interrupts
;-----------------------------------------------------------
_osport_enable_int:
	EINT
	LRETR

;-----------------------------------------------------------
; Request a context switch
; The context switcher is implemented as RTOSINT, trigger
; the interrupt.
;-----------------------------------------------------------
_osport_contextsw_req:
	INTR RTOSINT
	LRETR

;-----------------------------------------------------------
; Start the operating system kernel
;-----------------------------------------------------------
_osport_start:

	PUSH ST1  ; save EALLOW status
	PUSH DP
	EALLOW    ; enable modification

	MOVW DP, #_CPUTIMER2+4      ; load data page of CPUTIMER2 TCR
	AND @_CPUTIMER2+4, #0xFFEF  ; clear bit 4 of CPUTIMER2 TCR

	POP DP
	POP ST1   ; restore EALLOW status

	; generate interrupt #20 (USER1)
	TRAP #20

	LRETR
;-----------------------------------------------------------
; USER1 interrupt handler, prepare the CPU for multithreading
;-----------------------------------------------------------
_ISR_USER1:
	; interrupt disabled automatically here

	; load the first thread's control block
	MOVW DP, #_g_sch        ; load data page of g_sch
	MOVL XAR0, @_g_sch      ; XAR0 <- &(g_sch.p_current->p_sp)
	MOVL ACC, *+XAR0[0]     ; ACC <- g_sch.p_current->p_sp
	MOV  SP, ACC            ; SP <- ACC

	; pop the registers not automatically
	; popped by hardware on interrupt return
	; FPU registers
	MOV32 R7H, *--SP
	MOV32 R6H, *--SP
	MOV32 R5H, *--SP
	MOV32 R4H, *--SP
	MOV32 R3H, *--SP
	MOV32 R2H, *--SP
	MOV32 R1H, *--SP
	MOV32 R0H, *--SP
	MOV32 STF, *--SP
	POP RB

	; regular cpu registers
	POP XT
	POP XAR7
	POP XAR6
	POP XAR5
	POP XAR4
	POP XAR3
	POP XAR2
	POP AR1H:AR0H
	POP RPC

	IRET
	; interrupts enabled automatically here

;-----------------------------------------------------------
; RTOSINT handler
; The context switcher
;-----------------------------------------------------------
_ISR_RTOSINT:
	; interrupts disabled automatically here

	ASP ; align stack pointer

	; context save, CPU registers
	PUSH RPC
	PUSH AR1H:AR0H
	PUSH XAR2
	PUSH XAR3
	PUSH XAR4
	PUSH XAR5
	PUSH XAR6
	PUSH XAR7
	PUSH XT

	; context save, FPU registers
	PUSH RB
	MOV32 *SP++, STF
	MOV32 *SP++, R0H
	MOV32 *SP++, R1H
	MOV32 *SP++, R2H
	MOV32 *SP++, R3H
	MOV32 *SP++, R4H
	MOV32 *SP++, R5H
	MOV32 *SP++, R6H
	MOV32 *SP++, R7H

	; store a snapshot of the stack pointer into
	; current thread's TCB block
	MOVW DP, #_g_sch        ; load data page of g_sch
	MOVL XAR0, @_g_sch      ; XAR0 <- &(g_sch.p_current->p_sp)
	MOVZ AR1, SP            ; XAR1 <- SP
	MOVL *+XAR0[0], XAR1    ; g_sch.p_current->p_sp = XAR1


	; load the next thread's control block
	MOVW DP, #_g_sch+2      ; load data page of g_sch+2
	MOVL XAR0, @_g_sch+2    ; XAR0 <- &(g_sch.p_next->p_sp)
	MOVL ACC, *+XAR0[0]     ; ACC <- g_sch.p_next->p_sp
	MOV  SP, ACC            ; SP <- ACC

	MOVW DP, #_g_sch        ; load data page of g_sch
	MOVL @_g_sch, XAR0      ; g_sch->p_current = XAR0

	; pop the registers not automatically
	; popped by hardware on interrupt return
	; FPU registers
	MOV32 R7H, *--SP
	MOV32 R6H, *--SP
	MOV32 R5H, *--SP
	MOV32 R4H, *--SP
	MOV32 R3H, *--SP
	MOV32 R2H, *--SP
	MOV32 R1H, *--SP
	MOV32 R0H, *--SP
	MOV32 STF, *--SP
	POP RB

	; regular cpu registers
	POP XT
	POP XAR7
	POP XAR6
	POP XAR5
	POP XAR4
	POP XAR3
	POP XAR2
	POP AR1H:AR0H
	POP RPC

	NASP ; undo align stack pointer

	IRET
	; interrupts enabled automatically here
