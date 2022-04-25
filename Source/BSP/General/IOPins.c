#include "LPC214x.H"
#include "IOPins.h"

/*************** IO Pins Code *********************************/
#define LED_PINS (0x01<<11)
#define FDI_PINS (0x07F<<16)

void IOPins_Delay (int ulValue)
{
    int i, j;
	for(i=0; i < ulValue; i++)
        j=0;
}

void IOPins_Init (void)
{
   PINSEL0 &= 0xFF3FF00F; // Make P0.11 as GPIO pins.. Bits 22 to 23 to be made 0 
   IODIR0 |= LED_PINS; //P0.11 as output pin -> 11th bit to be made 1

   //PINSEL2 &= 0xFFFFFFFB; // clear 3rd bit
   //IODIR1 |= FDI_PINS; //P1.16 to P1.22 as output pin -> 16th to 22nd bit to be made 1
}

void IOPins_ControlTestLED (unsigned int ulOnOFF)
{
	if (ulOnOFF)
	{
 		IOSET0 = LED_PINS;
 		//IOSET1 = FDI_PINS;
	}
	else
	{
		IOCLR0 = LED_PINS;
		//IOCLR1 = FDI_PINS;
	}
}
	
void IOPins_Test (void)
{
	unsigned int i = 0;
	IOPins_Init();
	
	for (i=0; i<3; i++)
	{
	   IOPins_ControlTestLED(1);
	   IOPins_Delay (10000);
	   
	   IOPins_ControlTestLED(0);
	   IOPins_Delay (10000);
	} 
}

void IOPins_Main (void)
{
    IOPins_Test();
}

