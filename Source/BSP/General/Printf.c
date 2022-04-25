#define PRINTF_C

#include <stdarg.h>
#include "LPC214x.H"
#include "Global_Enums.h"
#include "Global.h"
#include "Printf.h"
#include "UART_Drv.h"

static void Printf_SendString (char * pubString, unsigned int ulChannel)
{
	if (ulChannel == 0)
		UART0_Dump(pubString);
	else if (ulChannel == 1)
		UART1_Dump(pubString);
	else
		;
}

void U0_Printf(char * pubString, ...)
{
	va_list arguments;
	char * pubInputStringPtr = pubString;
	unsigned int ulTemp;
	unsigned char ubBuf[16];

	va_start (arguments, pubString);

	//UART0_Printf("\r\n U0");
	//UART0_Printf(pubString);
	
	while (*pubInputStringPtr)
	{
		if (*pubInputStringPtr == '%')
		{
			pubInputStringPtr ++;
			switch (*pubInputStringPtr)
			{
				case 'd':
				case 'D':
					ulTemp = va_arg (arguments, long);
					Global_ConvertDecimalToAscii (ulTemp, ubBuf, sizeof(ubBuf));
					Printf_SendString ((char *)(ubBuf), 0);
					break;

				case 'x':
				case 'X':
					ulTemp = va_arg (arguments, long);
					Global_ConvertHexToAscii (ulTemp, ubBuf, sizeof(ubBuf));
					Printf_SendString ((char *)(ubBuf), 0);
					break;
					
				case 'c':
				case 'C':
					ulTemp = va_arg (arguments, long);
					UART0_PutChar(ulTemp);
					break;
					
				case 's':
				case 'S':
					ulTemp = va_arg (arguments, long);
					Printf_SendString ((char *)(ulTemp), 0);
					break;
			}
		}
		else
		{
			UART0_PutChar(*pubInputStringPtr);
		}

		pubInputStringPtr ++;
	}

	va_end(arguments);	
}


void U1_Printf(char * pubString, ...)
{
	va_list arguments;
	char * pubInputStringPtr = pubString;
	unsigned int ulTemp;
	unsigned char ubBuf[16];

	va_start (arguments, pubString);

	//UART1_Printf("\r\n U1");
	//UART1_Printf(pubString);
	
	while (*pubInputStringPtr)
	{
		if (*pubInputStringPtr == '%')
		{
			pubInputStringPtr ++;
			switch (*pubInputStringPtr)
			{
				case 'd':
				case 'D':
					ulTemp = va_arg (arguments, long);
					Global_ConvertDecimalToAscii (ulTemp, ubBuf, sizeof(ubBuf));
					Printf_SendString ((char *)(ubBuf), 1);
					break;

				case 'x':
				case 'X':
					ulTemp = va_arg (arguments, long);
					Global_ConvertHexToAscii (ulTemp, ubBuf, sizeof(ubBuf));
					Printf_SendString ((char *)(ubBuf), 1);
					break;
					
				case 'c':
				case 'C':
					ulTemp = va_arg (arguments, long);
					UART1_PutChar(ulTemp);
					break;
					
				case 's':
				case 'S':
					ulTemp = va_arg (arguments, long);
					Printf_SendString ((char *)(ulTemp), 1);
					break;
			}
		}
		else
		{
			UART1_PutChar(*pubInputStringPtr);
		}

		pubInputStringPtr ++;
	}

	va_end(arguments);
}

void NULL_Printf(char * pubString, ...)
{
}

PRINTF_FUN_PTR PrintfGetFunctionPtr (unsigned int ulUartChannel)
{
	PRINTF_FUN_PTR pfn_Ux_Printf;

	if (ulUartChannel == 0)
		pfn_Ux_Printf = U0_Printf;
	else if (ulUartChannel == 1)
		pfn_Ux_Printf = U1_Printf;
	else
		pfn_Ux_Printf = NULL;

	return (pfn_Ux_Printf);
}


