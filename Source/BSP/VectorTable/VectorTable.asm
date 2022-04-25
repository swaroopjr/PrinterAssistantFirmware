
Mode_USR        EQU     0x10
Mode_FIQ        EQU     0x11
Mode_IRQ        EQU     0x12
Mode_SVC        EQU     0x13
Mode_ABT        EQU     0x17
Mode_UNDEF      EQU     0x1B
Mode_SYS        EQU     0x1F            ; only available on ARM Arch. v4

I_Bit           EQU     0x80
F_Bit           EQU     0x40

;Top_Stack       EQU     0x40004000
;UND_Stack_Size  EQU     0x00000004
;SVC_Stack_Size  EQU     0x00000004
;ABT_Stack_Size  EQU     0x00000004
;FIQ_Stack_Size  EQU     0x00000004
;IRQ_Stack_Size  EQU     0x00000080
;USR_Stack_Size  EQU     0x00000400

SVC_Stack       EQU     0x40002000 - 4
ABT_Stack       EQU     0x40001E00
UNDEF_Stack     EQU     0x40001C00
IRQ_Stack       EQU     0x40001A00
FIQ_Stack       EQU     0x40001600
USR_Stack       EQU     0x40001400

VICIntEnClr     EQU     0xFFFFF014

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        AREA Vect, CODE, READONLY

; *****************
; Vector Table
; *****************

        LDR     PC, Reset_Addr
        LDR     PC, Undefined_Addr
        LDR     PC, SWI_Addr
        LDR     PC, Prefetch_Addr
        LDR     PC, Abort_Addr
        NOP                        ; Reserved vector
        LDR     PC, IRQ_Addr       ; if VIC_VECTORED_INTERRUPT_SUPPORT is undefined
        ;LDR     PC, [PC, #-0x0FF0]       ; if VIC_VECTORED_INTERRUPT_SUPPORT is defined. Reads interrupt handler or interrupt subroutine address from VICDefVectAddr
        LDR     PC, FIQ_Addr
        
        IMPORT  Undefined_HandlerMain
        IMPORT  SWI_HandlerMain
        IMPORT  Prefetch_HandlerMain
        IMPORT  Abort_HandlerMain
        ;IMPORT  IRQ_HandlerMain
        IMPORT  FIQ_HandlerMain
        
Reset_Addr      DCD     Reset_Handler
Undefined_Addr  DCD     Undefined_HandlerMain
SWI_Addr        DCD     SWI_HandlerMain
Prefetch_Addr   DCD     Prefetch_HandlerMain
Abort_Addr      DCD     Abort_HandlerMain
                DCD     0               ; Reserved vector
IRQ_Addr        DCD     IRQ_Handler
FIQ_Addr        DCD     FIQ_HandlerMain

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        IMPORT  __main

Reset_Handler
; --- Initialize stack pointer registers
; Enter IRQ mode and set up the IRQ stack pointer
        MOV     r0, #Mode_IRQ:OR:I_Bit:OR:F_Bit ; No interrupts
        MSR     cpsr_c, r0
        LDR     sp, =IRQ_Stack

; Enter FIQ mode and set up the FIQ stack pointer
        MOV     r0, #Mode_FIQ:OR:I_Bit:OR:F_Bit ; No interrupts
        MSR     cpsr_c, r0
        LDR     sp, =FIQ_Stack

; Enter ABT mode and set up the ABT stack pointer
        MOV     r0, #Mode_ABT:OR:I_Bit:OR:F_Bit ; No interrupts
        MSR     cpsr_c, r0
        LDR     sp, =ABT_Stack

; Enter UNDEF mode and set up the IRQ stack pointer
        MOV     r0, #Mode_UNDEF:OR:I_Bit:OR:F_Bit ; No interrupts
        MSR     cpsr_c, r0
        LDR     sp, =UNDEF_Stack

; Set up the SVC stack pointer last and return to SVC mode
        MOV     r0, #Mode_SVC:OR:I_Bit:OR:F_Bit ; No interrupts
        MSR     cpsr_c, r0
        LDR     sp, =SVC_Stack

; Disable all interrupts in LPC2129 VIC
        LDR     r0, =0xFFFFFFFF
        LDR     r1, =VICIntEnClr
        STR     r0, [r1]
        
; Set up the User mode stack pointer and interrupts enable/disable bits in ARM CPSR
        ;MOV     r0, #Mode_USR:OR:I_Bit:OR:F_Bit ; IRQ enabled
        MOV     r0, #Mode_USR:OR:F_Bit ; IRQ not changed
        ;BIC     r0, r0, #0x00000080    ;Enable IRQ
        MSR     cpsr_c, r0
        LDR     sp, =USR_Stack

        ;MOV     PC, #0x00010000  ;jump to __main     
        bl     __main
        ;END


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        IMPORT  IRQ_HandlerMain

IRQ_Handler
        sub     r14, r14, #4           ; Adjust Link register. link register will have address of next instruction. So, it should be subtracted by 4.
        STMFD   sp!, {r0-r12, r14}     ; Push R0-R12 and return address to IRQ_stack

        bl      IRQ_HandlerMain        ; call interrupt handler function which is written in C

        LDMFD   sp!, {r0-r12, pc}^     ; Pop R0-R12 registers from IRQ_stack. Pop return address to pc. 
                                       ; By adding ^, SPSR is copied to CPSR. Interruts will also get enabled as interrupts were enabled in SPSR.
                                       ; Same method can be used to return from exception handlers as well. 
        END
      
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

