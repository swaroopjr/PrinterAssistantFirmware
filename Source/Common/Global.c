
#include <stdarg.h>
#include "LPC214x.H"
#include "Global_Enums.h"
#define GLOBAL_C

void Global_Delay(unsigned long int ulValue)
{
    while(ulValue > 0) 
    {
        ulValue--;	// Loop Decrease Counter	
    }
}

void Global_Memset (unsigned char * pubOutputBuff, unsigned char ubData, unsigned int ulSize)
{
	while (ulSize --)
	{
		*pubOutputBuff ++ = ubData;
	}
}

void Global_Memcpy (unsigned char * pubOutputBuff, unsigned char * pubInputBuff, unsigned int ulSize)
{
	while (ulSize --)
	{
		*pubOutputBuff ++ = *pubInputBuff ++;
	}
}

unsigned int Global_Memcmp (unsigned char * pubBuff1, unsigned char * pubBuff2, unsigned int ulSize)
{
	while (ulSize --)
	{
		if (*pubBuff1 == *pubBuff2)
		{
			pubBuff1 ++;
			pubBuff2 ++;
		}
		else
		{
			return FALSE;
		}
	}

	return TRUE;
}


int Global_Strlen(unsigned char* pubString)
{
    unsigned int  ulLength = 0;
    while(*pubString++)
    {
        ulLength ++;
    }
    return ulLength;
}

unsigned int Global_ChangeEndian (unsigned int ulValue)
{
	unsigned int ulRetValue;
	ulRetValue = ulValue >> 24;
	ulRetValue |= ((ulValue >> 8) & 0x0000FF00);
	ulRetValue |= ((ulValue << 8) & 0x00FF0000);
	ulRetValue |= ((ulValue << 24) & 0xFF000000);

	return (ulRetValue);
}


void Global_StringReverse (unsigned char * pubOutputBuff)
{
	unsigned char * pubStartptr = pubOutputBuff;
	unsigned char * pubEndptr = pubOutputBuff;
	unsigned char ubTemp;

	while (*pubEndptr)
	{
		pubEndptr ++;
	}
	pubEndptr --;

	while (pubStartptr < pubEndptr)
	{
		ubTemp = *pubStartptr;
		*pubStartptr = *pubEndptr;
		*pubEndptr = ubTemp;

		pubEndptr --;
		pubStartptr ++;
	}
}

unsigned int Global_ConvertDecimalToAscii (unsigned int ulDecimalValue, unsigned char * pubOutputBuff, unsigned int ulBufSize)
{
	unsigned char * pubStartBufPtr = pubOutputBuff;

	if (ulBufSize == 0)
	{
		return FALSE;
	}

	Global_Memset (pubOutputBuff, 0x00, ulBufSize);

	while (1)
	{
		*pubOutputBuff++ = (ulDecimalValue % 10) + '0';
		ulDecimalValue = ulDecimalValue / 10;
		ulBufSize --;

		if ((!ulDecimalValue) || (!ulBufSize))
			break;
	}

	Global_StringReverse (pubStartBufPtr);
	return TRUE;
}

unsigned int Global_ConvertHexToAscii (unsigned int ulDecimalValue, unsigned char * pubOutputBuff, unsigned int ulBufSize)
{
	unsigned char * pubStartBufPtr = pubOutputBuff;

	if (ulBufSize == 0)
	{
		return FALSE;
	}

	Global_Memset (pubOutputBuff, 0x00, ulBufSize);

	while (1)
	{
		if ((ulDecimalValue % 16) < 10)
		{
			*pubOutputBuff++ = (ulDecimalValue % 16) + '0';
		}
		else
		{
			switch ((ulDecimalValue % 16))
			{
				case 0x0A:
					*pubOutputBuff++ = 'A';
					break;

				case 0x0B:
					*pubOutputBuff++ = 'B';
					break;


				case 0x0C:
					*pubOutputBuff++ = 'C';
					break;


				case 0x0D:
					*pubOutputBuff++ = 'D';
					break;


				case 0x0E:
					*pubOutputBuff++ = 'E';
					break;


				case 0x0F:
					*pubOutputBuff++ = 'F';
					break;
			}
		}

		ulDecimalValue = ulDecimalValue / 16;
		ulBufSize --;
		if ((!ulDecimalValue) || (!ulBufSize))
			break;
	}

	Global_StringReverse (pubStartBufPtr);
	return TRUE;
}

unsigned int Global_ConvertAsciiToHex (unsigned char * pubInputBuff, unsigned int *pulDecimalValue)
{
	unsigned int ulMultiplier = 1;
	unsigned int ulStringIndex = NULL;

	*pulDecimalValue = NULL;

	while (1)
	{
		if (pubInputBuff[ulStringIndex + 1])
			ulStringIndex ++;
		else
		{
			break;
		}
	}

	if (!pubInputBuff)
	{
		return FALSE;
	}

	while (1)
	{
		if ((pubInputBuff[ulStringIndex] <= 'F') && (pubInputBuff[ulStringIndex] >= 'A'))
		{
			*pulDecimalValue += ((pubInputBuff[ulStringIndex] - 'A') + 10) * ulMultiplier;
		}
		else if ((pubInputBuff[ulStringIndex] <= 'f') && (pubInputBuff[ulStringIndex] >= 'a'))
		{
			*pulDecimalValue += ((pubInputBuff[ulStringIndex] - 'a') + 10) * ulMultiplier;
		}
		else if ((pubInputBuff[ulStringIndex] <= '9') && (pubInputBuff[ulStringIndex] >= '0'))
		{
			*pulDecimalValue += (pubInputBuff[ulStringIndex] - '0') * ulMultiplier;
		}
		else
		{
			*pulDecimalValue = NULL;
			return FALSE;
		}

		if (ulStringIndex == 0)
			break;

		ulStringIndex --;

		ulMultiplier = ulMultiplier * 0x10;
	}

	return TRUE;
}

void Global_SPrintf(char * pubOutputString, char * pubInputString, ... )    
{
	char * pubInputStringPtr = pubInputString;
	char * pubOutStringPtr = pubOutputString;
	unsigned int ulTemp;
	unsigned char aubBuf[16];	
	unsigned int ulLength;
	va_list arguments;

	va_start (arguments, pubInputString);

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
					Global_ConvertDecimalToAscii (ulTemp, aubBuf, sizeof(aubBuf));
					ulLength = Global_Strlen ((unsigned char *) aubBuf);
					Global_Memcpy ((unsigned char *)pubOutStringPtr, aubBuf, ulLength);
					pubOutStringPtr += ulLength;
					break;

				case 'x':
				case 'X':
					ulTemp = va_arg (arguments, long);
					Global_ConvertHexToAscii (ulTemp, aubBuf, sizeof(aubBuf));
					ulLength = Global_Strlen ((unsigned char *) aubBuf);
					Global_Memcpy ((unsigned char *)pubOutStringPtr, aubBuf, ulLength);
					pubOutStringPtr += ulLength;
					break;
					
				case 'c':
				case 'C':
					ulTemp = va_arg (arguments, long);
					*pubOutStringPtr = ulTemp;
					pubOutStringPtr ++;
					break;
					
				case 's':
				case 'S':
					ulTemp = va_arg (arguments, long);
					ulLength = Global_Strlen ((unsigned char *)(ulTemp));
					Global_Memcpy ((unsigned char *)pubOutStringPtr, (unsigned char *)(ulTemp), ulLength);
					pubOutStringPtr += ulLength;
					break;
			}
		}
		else
		{
			*pubOutStringPtr = *pubInputStringPtr;
			pubOutStringPtr ++;
		}

		pubInputStringPtr ++;
	}

	*pubOutStringPtr = NULL;
	va_end(arguments);
}

void Global_NULL_Printf(char * pubString, ...)
{
}

