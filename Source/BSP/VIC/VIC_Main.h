#undef EXTERN
#ifdef VIC_MAIN_C
#define EXTERN 
#else
#define EXTERN  extern
#endif

EXTERN void VIC_Enable_ARM_Core_IRQ (void);
EXTERN void VIC_Disable_ARM_Core_IRQ (void);
#ifdef VIC_VECTORED_INTERRUPT_SUPPORT
EXTERN void VIC_Main_Init (void);
#else
EXTERN void VIC_ISR (U32 ulInterrupt);
EXTERN void VIC_Interrupt_Handler (void);
EXTERN void VIC_Main_Init (void);
#endif