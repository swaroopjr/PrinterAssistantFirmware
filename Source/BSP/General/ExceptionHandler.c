//IRQHandler.c
#include "LPC214x.H"
#include "Global_Enums.h"
#include "BSP_Macro_Defines.h"

#include "Printf.h"

void DumpExceptionData (void)
{
    U32 ulRegister[13]; //r0 to r12
    U32 ulStackPointer; //
    U32 ulLinkRegister;
    U32 ulCpsr;
    U32 ulSpsr;
    U32 ulTemp;
    
    __asm
    {
        mov ulRegister[0], r0
        mov ulRegister[1], r1
        mov ulRegister[2], r2
        mov ulRegister[3], r3
        mov ulRegister[4], r4
        mov ulRegister[5], r5
        mov ulRegister[6], r6
        mov ulRegister[7], r7
        mov ulRegister[8], r8
        mov ulRegister[9], r9
        mov ulRegister[10], r10
        mov ulRegister[11], r11
        mov ulRegister[12], r12
        mov ulStackPointer, r13
        mov ulLinkRegister, r14
        mrs ulCpsr, CPSR			
        mrs ulSpsr, SPSR
    }
    
    for (ulTemp=0; ulTemp<=12; ulTemp++)
    {
        U0_Printf("\r\n r%d => [%x]", ulTemp, ulRegister[ulTemp]);
        U1_Printf("\r\n r%d => [%x]", ulTemp, ulRegister[ulTemp]);
    }	

    U0_Printf("\r\n Stack Pointer => [%x]", ulStackPointer);
    U1_Printf("\r\n Stack Pointer => [%x]", ulStackPointer);
    
    U0_Printf("\r\n Link Register => [%x]", ulLinkRegister);
    U1_Printf("\r\n Link Register => [%x]", ulLinkRegister);
    
    U0_Printf("\r\n CPSR Register => [%x]", ulCpsr);
    U1_Printf("\r\n CPSR Register => [%x]", ulCpsr);
    
    U0_Printf("\r\n SPSR Register => [%x]", ulSpsr);
    U1_Printf("\r\n SPSR Register => [%x]", ulSpsr);
}


void Undefined_HandlerMain (void)
{
	DumpExceptionData ();

	LCD_DisplayStringInFirstLine ("  Exception !!  ");
	LCD_DisplayStringInSecondLine ("Undefined");

	U0_Printf("\r\n Undefined_HandlerMain");
	U1_Printf("\r\n Undefined_HandlerMain");


	while (1);
}

void SWI_HandlerMain (void)
{
	DumpExceptionData ();

	LCD_DisplayStringInFirstLine ("  Exception !!  ");
	LCD_DisplayStringInSecondLine ("SWI");

	U0_Printf("\r\n SWI_HandlerMain");
	U1_Printf("\r\n SWI_HandlerMain");

	while (1);

}

void Prefetch_HandlerMain(void)
{
	DumpExceptionData ();

	LCD_DisplayStringInFirstLine ("  Exception !!  ");
	LCD_DisplayStringInSecondLine ("Prefetch");

	U0_Printf("\r\n Prefetch_HandlerMain");
	U1_Printf("\r\n Prefetch_HandlerMain");

	while (1);
}

void Abort_HandlerMain (void)
{
	DumpExceptionData ();

	LCD_DisplayStringInFirstLine ("  Exception !!  ");
	LCD_DisplayStringInSecondLine ("Abort");

	U0_Printf("\r\n Abort_HandlerMain");
	U1_Printf("\r\n Abort_HandlerMain");

	while (1);	
}

void IRQ_HandlerMain (void)
{
	//DumpExceptionData ();

    #ifndef VIC_VECTORED_INTERRUPT_SUPPORT
    VIC_Interrupt_Handler ();
    #endif
}

void FIQ_HandlerMain (void)
{
	//U0_Printf("\r\n FIQ_HandlerMain");
	//U1_Printf("\r\n FIQ_HandlerMain");
}



