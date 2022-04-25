#undef EXTERN
#ifdef EXINT_MAIN_C
#define EXTERN 
#else
#define EXTERN  extern
#endif

EXTERN void External_Interrupts_EINT0_Isr (void);
EXTERN void External_Interrupts_EINT1_Isr (void);
EXTERN void External_Interrupts_EINT2_Isr (void);
EXTERN void External_Interrupts_EINT3_Isr (void);
EXTERN void External_Interrupts_Initialize (void);


