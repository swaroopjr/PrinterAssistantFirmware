#undef EXTERN
#ifdef TIMER_C
#define EXTERN 
#else
#define EXTERN  extern
#endif

EXTERN void Timer0_Isr (void);
EXTERN void Timer1_Isr (void);
EXTERN void Timer_EnableTimer0 (void);
EXTERN void Timer_EnableTimer1 (void);
EXTERN void Timer_Main (void);




