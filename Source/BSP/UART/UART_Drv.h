#undef EXTERN
#ifdef UART_DRV_C
#define EXTERN 
#else
#define EXTERN  extern
#endif

EXTERN void UART1_PutChar (int ch);
EXTERN void UART0_PutChar (int ch);
EXTERN char UART1_GetChar (void);
EXTERN char UART0_GetChar (void);
EXTERN char UART1_GetCharWithinTime (unsigned int ulCount);
EXTERN char UART0_GetCharWithinTime (unsigned int ulCount);
EXTERN void UART_GetHeadAndTail (unsigned int *pulUART0_Head, unsigned int *pulUART0_Tail, unsigned int *pulUART1_Head, unsigned int *pulUART1_Tail);
EXTERN void UART1_FlushBuffer (void);
EXTERN void UART0_FlushBuffer (void);
EXTERN int UART1_Dump(char* pString);
EXTERN int UART0_Dump(char* pString);
EXTERN int UART1_Printf(char* pString);
EXTERN int UART0_Printf(char* pString);

EXTERN void UART1isr (void);
EXTERN void UART0isr (void); 


