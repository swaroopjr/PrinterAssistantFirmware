//VIC_Main.c

#define VIC_MAIN_C

#include "LPC214x.H"
#include "Global_Enums.h"
#include "BSP_Macro_Defines.h"
#include "VIC_Main.h"

#include "UART_Drv.h"
#include "Timer.h"
#include "ExInt_Main.h"

#define VIC_VECTORED_IRQ_BIT         (0x01 << 5)

#define VIC_WDT_INDEX                (0)
#define VIC_TIMER0_INDEX             (4)
#define VIC_TIMER1_INDEX             (5)
#define VIC_UART0_INDEX              (6)
#define VIC_UART1_INDEX              (7)
#define VIC_PWM0_INDEX               (8)
#define VIC_I2C0_INDEX               (9)
#define VIC_SPI0_INDEX               (10)
#define VIC_SPI1_INDEX               (11)
#define VIC_PLL_INDEX                (12)
#define VIC_RTC_INDEX                (13)
#define VIC_EINT0_INDEX              (14)
#define VIC_EINT1_INDEX              (15)
#define VIC_EINT2_INDEX              (16)
#define VIC_EINT3_INDEX              (17)
#define VIC_AD0_INDEX                (18)
#define VIC_I2C1_INDEX               (19)
#define VIC_BOD_INDEX                (20)
#define VIC_AD1_INDEX                (21)
#define VIC_USB_INDEX                (22)

#define VIC_WDT_BIT                  (0x01 << VIC_WDT_INDEX)
#define VIC_TIMER0_BIT               (0x01 << VIC_TIMER0_INDEX)
#define VIC_TIMER1_BIT               (0x01 << VIC_TIMER1_INDEX)
#define VIC_UART0_BIT                (0x01 << VIC_UART0_INDEX)
#define VIC_UART1_BIT                (0x01 << VIC_UART1_INDEX)
#define VIC_PWM0_BIT                 (0x01 << VIC_PWM0_INDEX)
#define VIC_I2C0_BIT                 (0x01 << VIC_I2C0_INDEX)
#define VIC_SPI0_BIT                 (0x01 << VIC_SPI0_INDEX)
#define VIC_SPI1_BIT                 (0x01 << VIC_SPI1_INDEX)
#define VIC_PLL_BIT                  (0x01 << VIC_PLL_INDEX)
#define VIC_RTC_BIT                  (0x01 << VIC_RTC_INDEX)
#define VIC_EINT0_BIT                (0x01 << VIC_EINT0_INDEX)
#define VIC_EINT1_BIT                (0x01 << VIC_EINT1_INDEX)
#define VIC_EINT2_BIT                (0x01 << VIC_EINT2_INDEX)
#define VIC_EINT3_BIT                (0x01 << VIC_EINT3_INDEX)
#define VIC_AD0_BIT                  (0x01 << VIC_AD0_INDEX)
#define VIC_I2C1_BIT                 (0x01 << VIC_I2C1_INDEX)
#define VIC_BOD_BITX                 (0x01 << VIC_BOD_INDEX)
#define VIC_AD1_BIT                  (0x01 << VIC_AD1_INDEX)
#define VIC_USB_BIT                  (0x01 << VIC_USB_INDEX)


void VIC_Enable_ARM_Core_IRQ (void)
{
    U32 ulTemp;

    __asm
    {
        mrs ulTemp, CPSR
        bic ulTemp, ulTemp, #0x80
        msr CPSR_c, ulTemp
    }
}

void VIC_Disable_ARM_Core_IRQ (void)
{
    U32 ulTemp;
    
    __asm
    {
        mrs ulTemp, CPSR
        orr ulTemp, ulTemp, #0x80
        msr CPSR_c, ulTemp
    }
}

#ifdef VIC_VECTORED_INTERRUPT_SUPPORT
void VIC_Main_Init (void)
{
	//Configure All Interrupts to IRQ
	VICIntSelect = 0x00000000;

	//Disable all interrupts
	VICIntEnClr = 0xFFFFFFFF;

	//Enable UART0 and UART1 interrupt
	VICIntEnable |=  VIC_UART0_BIT | VIC_UART1_BIT;
	
	//Update UART0 and UART1 address
	VICVectAddr0 = (U32) UART0isr;
	VICVectAddr1 = (U32) UART1isr;
	
	// Configure UART0 and UART1 as vectored IRQ
	VICVectCntl0 = VIC_UART0_INDEX | VIC_VECTORED_IRQ_BIT;
	VICVectCntl1 = VIC_UART1_INDEX | VIC_VECTORED_IRQ_BIT;

	//VIC_Enable_ARM_Core_IRQ();
}

#else //for NONVECTORED INTERRUPTS

void VIC_ISR (U32 ulInterrupt)
{
    switch (ulInterrupt)
    {
        case VIC_WDT_INDEX:
            break;

        case VIC_TIMER0_INDEX:
			Timer0_Isr ();
            break;
        
        case VIC_TIMER1_INDEX:
			Timer1_Isr ();
            break;
        
        case VIC_UART0_INDEX:
            UART0isr ();
            break;
        
        case VIC_UART1_INDEX:
            UART1isr ();
            break;
        
        case VIC_PWM0_INDEX:
            break;

        case VIC_I2C0_INDEX:
            break;

        case VIC_SPI0_INDEX:
            break;

        case VIC_SPI1_INDEX:
            break;

        case VIC_PLL_INDEX:
            break;

        case VIC_RTC_INDEX:
            break;

        case VIC_EINT0_INDEX:
            External_Interrupts_EINT0_Isr ();
            break;

        case VIC_EINT1_INDEX:
            External_Interrupts_EINT1_Isr ();
            break;

        case VIC_EINT2_INDEX:
            External_Interrupts_EINT2_Isr ();			
            break;

        case VIC_EINT3_INDEX:
            External_Interrupts_EINT3_Isr ();			
            break;

        case VIC_AD0_INDEX:
            break;

        case VIC_I2C1_INDEX:
            break;

        case VIC_BOD_INDEX:
            break;

        case VIC_AD1_INDEX:
            break;

        case VIC_USB_INDEX:
            break;

        default:
            break;
    }
}

void VIC_Interrupt_Handler (void)
{
    U32 ulIRQ_Interrupts_Status;
	U32 ulInterrupt = 0;
    
    //Step-1 : Diable ARM core interrupts. Note that IRQ will be already disabled (masked) in CPSR at this point of time. This is not required!!
    //VIC_Disable_ARM_Core_IRQ ();
    
    //Step-2 : Get IRQ interrupt status. 
    ulIRQ_Interrupts_Status = (U32) VICIRQStatus;
    
    //Step-3 : Call all interrupts and acknowledge them
    while (ulIRQ_Interrupts_Status)
    {
        if (ulIRQ_Interrupts_Status & 0x01)
        {
            //call interrupt subroutine and Acknowledge interrupt execution
            VIC_ISR (ulInterrupt);
			VICVectAddr &= (~(0x01 << ulInterrupt));     // Acknowledge Interrupt
        }
        ulIRQ_Interrupts_Status = ulIRQ_Interrupts_Status >> 1;
		ulInterrupt ++;
    }

    //Step-4 : Enable interrupts in CPSR. Interrupts are enabled at end. We will loose any interrupts that have occured in between.
    //VIC_Enable_ARM_Core_IRQ();
}

void VIC_Main_Init (void)
{
	//Configure All Interrupts to IRQ
	VICIntSelect = 0x00000000;

	//Disable all interrupts
	VICIntEnClr = 0xFFFFFFFF;

	//Enable Required interrupts
	VICIntEnable |=  VIC_TIMER0_BIT | VIC_TIMER1_BIT | VIC_UART0_BIT | VIC_UART1_BIT | VIC_EINT1_BIT;
	
	//Update UART0 and UART1 address. This has to be updated only for vectored interrupts
	//VICVectAddr0 = (U32) VIC_Temp_Function;
	//VICVectAddr1 = (U32) VIC_Temp_Function;

	//Update Interrupt Handler address
	VICDefVectAddr = (U32) VIC_Interrupt_Handler;
	
	// Configure UART0 and UART1 as vectored IRQ. This has to be updated only for vectored interrupts
	//VICVectCntl0 = 0x00;
	//VICVectCntl1 = 0x00;

	VIC_Enable_ARM_Core_IRQ();
}
#endif


