//VIC_Main.c

#define EXINT_MAIN_C

#include "LPC214x.H"
#include "Global_Enums.h"
#include "BSP_Macro_Defines.h"
#include "ExInt_Main.h"

#include "Printf.h"

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void External_Interrupts_EINT0_Isr (void)
{
    U0_Printf("\r\n EINT-0 ISR");
    U1_Printf("\r\n EINT-0 ISR");		
    
    EXTINT |= (0x01<<0); //Acknowledge EINT0. This bit is cleared by writing a one to it. 
}

void External_Interrupts_EINT1_Isr (void)
{
    U0_Printf("\r\n EINT-1 ISR");
    U1_Printf("\r\n EINT-1 ISR");	
    
    EXTINT |= (0x01<<1); //Acknowledge EINT1. This bit is cleared by writing a one to it. 
}

void External_Interrupts_EINT2_Isr (void)
{
    U0_Printf("\r\n EINT-2 ISR");
    U1_Printf("\r\n EINT-2 ISR");	
    
    EXTINT |= (0x01<<2); //Acknowledge EINT2. This bit is cleared by writing a one to it. 
}

void External_Interrupts_EINT3_Isr (void)
{
    U0_Printf("\r\n EINT-3 ISR");
    U1_Printf("\r\n EINT-3 ISR");	
    
    EXTINT |= (0x01<<3); //Acknowledge EINT3. This bit is cleared by writing a one to it. 	
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

static void External_Interrupts_Initialize_EINT0 (void)
{

}

static void External_Interrupts_Initialize_EINT1 (void)
{
    // Initialize EINT1 (P0.3 or P0.14. We are using P0.14 for EINT1)
    PINSEL0 &= ~(0x01<<28); // Make 28th bit 0
    PINSEL0 |= (0x01<<29);  // make 29th bit 1. This will make P0.14 pin as EINT1 pin.
    
    INTWAKE = 0; //wakeup on external events are disabled as of now.
    
    EXTMODE |= (0x01<<1); //EINT1 is configured as edge sensitive
    EXTPOLAR |= (0x01<<1); //EINT1 is configured to trigger at falling edge (high to low transition)
}

static void External_Interrupts_Initialize_EINT2 (void)
{

}

static void External_Interrupts_Initialize_EINT3 (void)
{

}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void External_Interrupts_Initialize (void)
{
    External_Interrupts_Initialize_EINT0 ();
    External_Interrupts_Initialize_EINT1 ();
    External_Interrupts_Initialize_EINT2 ();	
    External_Interrupts_Initialize_EINT3 ();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

