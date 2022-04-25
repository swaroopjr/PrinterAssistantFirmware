#define  UART_DRV_C

#include "LPC214x.H"
#include "Global_Enums.h"
#include "BSP_Macro_Defines.h"

#include "UART_Drv.h"
#include "Monitor.h"


#ifdef UART_ISR_SUPPORT
#if 1
volatile U8 gaubUART0_Buf[64];
volatile U8 gaubUART1_Buf[64];
volatile U32 gaubUART0_Head = 0;
volatile U32 gaubUART1_Head = 0;
volatile U32 gaubUART0_Tail = 0;
volatile U32 gaubUART1_Tail = 0;
#else
U8 gaubUART0_Buf[64];
U8 gaubUART1_Buf[64];
U32 gaubUART0_Head = 0;
U32 gaubUART1_Head = 0;
U32 gaubUART0_Tail = 0;
U32 gaubUART1_Tail = 0;
#endif

void UART1isr (void) 
{
    U8 ch;

    while (!(U1LSR & 0x01));
    {
        ch = U1RBR;
    }

	gaubUART1_Buf[gaubUART1_Head ++] = ch;

	if (gaubUART1_Head >= sizeof (gaubUART1_Buf))
		gaubUART1_Head = 0; 

	#ifdef VIC_VECTORED_INTERRUPT_SUPPORT
	VIC_Enable_ARM_Core_IRQ ();
	#endif
}

void UART0isr (void) 
{
    int ch;

    while (!(U0LSR & 0x01));
    {
        ch = U0RBR;
    }

	gaubUART0_Buf[gaubUART0_Head ++] = ch;

	if (gaubUART0_Head >= sizeof (gaubUART0_Buf))
		gaubUART0_Head = 0; 

	#ifdef VIC_VECTORED_INTERRUPT_SUPPORT
	VIC_Enable_ARM_Core_IRQ();
	#endif
}
#endif

void UART1_Initialize (void)	//P1 on board
{               	   
    PINSEL0 |= 0x00050000;         // Enable RxD1 and TxD1              
    PINSEL0 &= (~0x000A0000);         // Enable RxD1 and TxD1       

    U1LCR 	= 0x00000083;             // 8 bits, no Parity, 1 Stop bit     

	//U1DLL = (Osc Freq / VPBDIV) / (16 * Baudrate)
    U1DLL 	= 0x000000A;             // 38400 Baud Rate @ 12MHz Osc  ((12000000 / 2) / (16 * 38400))
    //U1DLL 	= 0x00000013;             // 19200 Baud Rate @ 12MHz Osc  ((12000000 / 2) / (16 * 19200))
    //U1DLL 	= 0x00000027;             // 9600 Baud Rate @ 12MHz Osc  ((12000000 / 2) / (16 * 9600))
    //U1DLL 	= 0x0000004E;       // 4800 Baud Rate @ 12MHz Osc  ((12000000/2) / (16 * 4800))
    //U1DLL 	= 0x0000009C;       // 2400 Baud Rate @ 12MHz Osc  ((12000000/2) / (16 * 2400))
    //U1DLL 	= 0x00000138;       // 1200 Baud Rate @ 12MHz Osc  ((12000000/2) / (16 * 1200))
    //U1DLL 	= 0x00000270;       // 600 Baud Rate @ 12MHz Osc  ((12000000/2) / (16 * 600))
    //U1DLL 	= 0x000004E0;       // 300 Baud Rate @ 12MHz Osc  ((12000000/2) / (16 * 300))


    U1DLM   = 0;
    U1LCR 	= 0x00000003;             // DLAB = 0
    #ifdef UART_ISR_SUPPORT
    U1IER   = 0x00000001;             // Enable UART1 Rx interrupt. Enable Data available interrupt for UART1
    #endif
}

void UART0_Initialize (void)
{               	   
    PINSEL0 |= 0x00000005;         // Enable RxD1 and TxD1              
    PINSEL0 &= (~0x000000A);         // Enable RxD1 and TxD1       

    U0LCR 	= 0x00000083;             // 8 bits, no Parity, 1 Stop bit     

	//U0DLL = (Osc Freq / VPBDIV) / (16 * Baudrate)
    U0DLL 	= 0x00000027;             // 9600 Baud Rate @ 12MHz Osc  ((12000000 / 2) / (16 * 9600))
    //U0DLL 	= 0x0000004E;       // 4800 Baud Rate @ 12MHz Osc  ((12000000/2) / (16 * 4800))
    //U0DLL 	= 0x0000009C;       // 2400 Baud Rate @ 12MHz Osc  ((12000000/2) / (16 * 2400))
    //U0DLL 	= 0x00000138;       // 1200 Baud Rate @ 12MHz Osc  ((12000000/2) / (16 * 1200))
    //U0DLL 	= 0x00000270;       // 600 Baud Rate @ 12MHz Osc  ((12000000/2) / (16 * 600))
    //U0DLL 	= 0x000004E0;       // 300 Baud Rate @ 12MHz Osc  ((12000000/2) / (16 * 300))

    U0DLM   = 0;
    U0LCR 	= 0x00000003;             // DLAB = 0
    #ifdef UART_ISR_SUPPORT
    U0IER   = 0x00000001;
	#endif
}


void UART1_PutChar (int ch) 			     // Write character to Serial Port   
{                  		
    while (!(U1LSR & 0x20));
        U1THR = ch;
}

void UART0_PutChar (int ch) 			     // Write character to Serial Port   
{                  		
    while (!(U0LSR & 0x20));
        U0THR = ch;
}

#ifdef UART_ISR_SUPPORT
char UART1_GetChar (void) 				// Read character from Serial Port   
{
    U8 ubCh;
    
    if (gaubUART1_Tail >= sizeof (gaubUART1_Buf))
    {
        gaubUART1_Tail = 0; 
    }
    
    while (gaubUART1_Head == gaubUART1_Tail);
    
    ubCh = gaubUART1_Buf[gaubUART1_Tail ++];

    return (ubCh);
}

char UART0_GetChar (void) 				// Read character from Serial Port   
{
    U8 ubCh;
    
    if (gaubUART0_Tail >= sizeof (gaubUART0_Buf))
    {
        gaubUART0_Tail = 0; 
    }
    
    while (gaubUART0_Head == gaubUART0_Tail);
    
    ubCh = gaubUART0_Buf[gaubUART0_Tail ++];
    
    return (ubCh);
}


char UART1_GetCharWithinTime (unsigned int ulCountLimit) 				// Read character from Serial Port   
{
    U32 ulCount = 0;
    U8 ubCh = 0xFF;
    
    if (gaubUART1_Tail >= sizeof (gaubUART1_Buf))
    {
        gaubUART1_Tail = 0; 
    }	
    
    while (1)
    {
        if (gaubUART1_Head != gaubUART1_Tail)
        {
            ubCh = gaubUART1_Buf[gaubUART1_Tail ++];
            break;
        }
        
        if (ulCount > ulCountLimit)
        {
            ubCh = 0xFF;
            break;
        }
        
        ulCount ++;
    }
    
    return (ubCh);
}

char UART0_GetCharWithinTime (unsigned int ulCountLimit) 				// Read character from Serial Port   
{
    U32 ulCount = 0;
    U8 ubCh = 0xFF;
    
    if (gaubUART0_Tail >= sizeof (gaubUART0_Buf))
    {
        gaubUART0_Tail = 0; 
    }	
    
    while (1)
    {
        if (gaubUART0_Head != gaubUART0_Tail)
        {
            ubCh = gaubUART0_Buf[gaubUART0_Tail ++];
            break;
        }
        
        if (ulCount > ulCountLimit)
        {
            ubCh = 0xFF;
            break;
        }
        
        ulCount ++;
    }
    
    return (ubCh);
}

/* This is for debugging purpose */
void UART_GetHeadAndTail (unsigned int *pulUART0_Head, unsigned int *pulUART0_Tail, unsigned int *pulUART1_Head, unsigned int *pulUART1_Tail)
{
	*pulUART0_Head = gaubUART0_Head;
	*pulUART0_Tail = gaubUART0_Tail;	
	*pulUART1_Head = gaubUART1_Head;
	*pulUART1_Tail = gaubUART1_Tail;	
}

#else

char UART1_GetChar (void) 				// Read character from Serial Port   
{                    
    while (!(U1LSR & 0x01));
        return (U1RBR);
}

char UART0_GetChar (void) 				// Read character from Serial Port   
{                    
    while (!(U0LSR & 0x01));
        return (U0RBR);
}

char UART1_GetCharWithinTime (unsigned int ulCountLimit) 				// Read character from Serial Port   
{
	unsigned int ulCount = 0;

	while (1)
	{
	    if (U1LSR & 0x01)
		{
	        return (U1RBR);
	    }

		if (ulCount > ulCountLimit)
		{
			return (0xFF);
		}
		
		ulCount ++;
	}
}

char UART0_GetCharWithinTime (unsigned int ulCountLimit) 				// Read character from Serial Port   
{   
	unsigned int ulCount = 0;

	while (1)
	{
	    if (U0LSR & 0x01)
		{
	        return (U0RBR);
	    }

		if (ulCount > ulCountLimit)
		{
			return (0xFF);
		}
		
		ulCount ++;
	}
}

void UART_GetHeadAndTail (unsigned int *pulUART0_Head, unsigned int *pulUART0_Tail, unsigned int *pulUART1_Head, unsigned int *pulUART1_Tail)
{
}
#endif

void UART1_FlushBuffer (void)
{
	unsigned int ulData = 0;
	while (U1LSR & 0x01)
	{
		ulData = U1RBR;
	}
}

void UART0_FlushBuffer (void)
{
	unsigned int ulData = 0;
	while (U0LSR & 0x01)
	{
		ulData = U0RBR;
	}
}

int UART1_Dump(char* pString)
{
    int i=0;
    while(*pString)
    {
        UART1_PutChar(*pString++);
        i++;
    }
    return i;
}

int UART0_Dump(char* pString)
{
    int i=0;
    while(*pString)
    {
        UART0_PutChar(*pString++);
        i++;
    }
    return i;
}

int UART1_Printf(char* pString)
{
    int i=0;
    while(*pString)
    {
        UART1_PutChar(*pString++);
        i++;
    }
    return i;
}

int UART0_Printf(char* pString)
{
    int i=0;
    while(*pString)
    {
        UART0_PutChar(*pString++);
        i++;
    }
    return i;
}

void UART_Main (void)
{
	char Data;

    VPBDIV = 0x02; 				   //Divide Pclk by two frequency = 30 Mhz
    UART0_Initialize();
    UART0_Printf("\r\n UART0 Initialized !");
    UART0_Printf("\r\n Press Enter within few seconds to enter monitor mode ...");
    UART1_Initialize();
    UART1_Printf("\r\n UART1 Initialized !");
    UART1_Printf("\r\n Press Enter within few seconds to enter monitor mode ...");

	MONITOR_CheckMonitorMode ();
    UART0_Printf("\r\n UART_Main End");
    UART1_Printf("\r\n UART_Main End");
}
