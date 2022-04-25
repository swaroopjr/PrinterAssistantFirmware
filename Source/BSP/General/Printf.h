#undef EXTERN
#ifdef PRINTF_C
	#define EXTERN 
#else
	#define EXTERN extern
#endif	

#ifndef PRINTF_H
#define PRINTF_H	

void U0_Printf(char * pubString, ...);
void U1_Printf(char * pubString, ...);
void NULL_Printf(char * pubString, ...);
typedef void (*PRINTF_FUN_PTR)(char *, ...);
PRINTF_FUN_PTR PrintfGetFunctionPtr (unsigned int ulUartChannel);

#undef EXTERN
#endif
