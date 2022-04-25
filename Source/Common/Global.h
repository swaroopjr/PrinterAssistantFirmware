#ifdef GLOBAL_C
	#define EXTERN 
#else
	#define EXTERN extern
#endif

#ifndef GLOBAL_H
#define GLOBAL_H

EXTERN void Global_Delay(unsigned long int ulValue);
EXTERN void Global_Memset (unsigned char * pubOutputBuff, unsigned char ubData, unsigned int ulSize);
EXTERN void Global_Memcpy (unsigned char * pubOutputBuff, unsigned char * pubInputBuff, unsigned int ulSize);
EXTERN unsigned int Global_Memcmp (unsigned char * pubBuff1, unsigned char * pubBuff2, unsigned int ulSize);
EXTERN int Global_Strlen(unsigned char* pubString);
EXTERN unsigned int Global_ChangeEndian (unsigned int ulValue);
EXTERN void Global_StringReverse (unsigned char * pubOutputBuff);
EXTERN unsigned int Global_ConvertDecimalToAscii (unsigned int ulDecimalValue, unsigned char * pubOutputBuff, unsigned int ulBufSize);
EXTERN unsigned int Global_ConvertHexToAscii (unsigned int ulDecimalValue, unsigned char * pubOutputBuff, unsigned int ulBufSize);
EXTERN unsigned int Global_ConvertAsciiToHex (unsigned char *pubInputBuff, unsigned int *pulDecimalValue);
EXTERN void Global_SPrintf(char * pubOutputString, const char * pubInputString, ... );
EXTERN void Global_NULL_Printf(char * pubString, ...);

#undef EXTERN
#endif

