#define  TIMER_C

#include "LPC214x.H"
#include "Global_Enums.h"
#include "BSP_Macro_Defines.h"

#include "Timer.h"
#include "UART_Drv.h"

void Timer0_Isr (void) 
{
	//U0_Printf("\r\n Timer-0 ISR");
	//U1_Printf("\r\n Timer-0 ISR");
	T0IR   = 0x01;       // Clear Timer0 Match interrupt flag
}

void Timer1_Isr (void) 
{
	//U0_Printf("\r\n Timer-1 ISR");
	//U1_Printf("\r\n Timer-1 ISR");
	T1IR   = 0x01;       // Clear Timer0 Match interrupt flag
}


void Timer_EnableTimer0 (void)
{
    T0PR		= 6000;	        //Load prescaler.Load value for 1milli sec. If this is configured for 1micro second then timer accuracy will reduce drastically.
                                //How many Pclk pulse is required for 1milli sec. 
                                //6000 pulses of 6 MHz clock (12Mhz /2 = 6MHz as VPBDIV is set to half of processor clock) will make 1 milli sec.
    T0TCR		= 0x00000002;   //Reset counter and prescaler
    T0MCR		= 0x00000003;   //On match reset the counter and generate an interrupt
    T0MR0		= 1000;         //Set the cycle time. Howmany prescalar counts will result in 1 sec. 1millisec * 1000 = 1 second
    T0TCR		= 0x00000001;   //enable timer
}

void Timer_EnableTimer1 (void)
{
    T1PR		= 6000;         //Load prescaler.Load value for 1milli sec. If this is configured for 1micro second then timer accuracy will reduce drastically.
                                //How many Pclk pulse is required for 1milli sec. 
                                //6000 pulses of 6 MHz clock (12Mhz /2 = 6MHz as VPBDIV is set to half of processor clock) will make 1 milli sec.
    T1TCR		= 0x00000002;   //Reset counter and prescaler
    T1MCR		= 0x00000003;   //On match reset the counter and generate an interrupt
    T1MR0		= 5 * 1000;     //Set the cycle time. Howmany prescalar counts will result in 5 sec. 1millisec * 1000 * 5 = 5 second
    T1TCR		= 0x00000001;   //enable timer
}
	
void Timer_Main (void) 
{
	VPBDIV		= 0x00000002;  // APBDIV -> 2 means APB bus clock is one half of processor clock. This means Pclk is 12MHz /2 = 6MHz
    Timer_EnableTimer0();
    Timer_EnableTimer1();
}


