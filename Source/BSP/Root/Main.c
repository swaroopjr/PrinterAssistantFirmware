#include "LPC214x.H"
#include "Global_Enums.h"
#include "BSP_Macro_Defines.h"

#include "Main.h"

#include "Printf.h"
#include "IOPins.h"
#include "UART_Drv.h"
#include "LCD_Display.h"
#include "Flash_Main.h"
#include "EEPROM_I2C.h"
#include "I2C_PN532_NFC.h"
#include "VIC_Main.h"
#include "Timer.h"
#include "ExInt_Main.h"

void JumpToApp (void)
{
	U32 * pulBspInterfaceFunctions = (U32 *)(0x40007000);

	*pulBspInterfaceFunctions ++ = (U32)(U0_Printf);
	*pulBspInterfaceFunctions ++ = (U32)(UART0_PutChar);
	*pulBspInterfaceFunctions ++ = (U32)(UART0_GetChar);
	*pulBspInterfaceFunctions ++ = (U32)(UART0_GetCharWithinTime);

	*pulBspInterfaceFunctions ++ = (U32)(U1_Printf);
	*pulBspInterfaceFunctions ++ = (U32)(UART1_PutChar);
	*pulBspInterfaceFunctions ++ = (U32)(UART1_GetChar);
	*pulBspInterfaceFunctions ++ = (U32)(UART1_GetCharWithinTime);

	*pulBspInterfaceFunctions ++ = (U32)(UART_GetHeadAndTail);	

	*pulBspInterfaceFunctions ++ = (U32)(IOPins_Test);
	*pulBspInterfaceFunctions ++ = (U32)(IOPins_ControlTestLED);

	*pulBspInterfaceFunctions ++ = (U32)(LCD_DisplayStringInFirstLine);
	*pulBspInterfaceFunctions ++ = (U32)(LCD_DisplayStringInSecondLine);

	*pulBspInterfaceFunctions ++ = (U32)(FLASH_WriteFirmware);

	*pulBspInterfaceFunctions ++ = (U32)(EEPROM_I2C_Initialize);
	*pulBspInterfaceFunctions ++ = (U32)(EEPROM_I2C_WriteData);
	*pulBspInterfaceFunctions ++ = (U32)(EEPROM_I2C_ReadData);

	*pulBspInterfaceFunctions ++ = (U32)(PN532_NFC_Initialize);
	*pulBspInterfaceFunctions ++ = (U32)(PN532_NFC_ReadPassiveTargetID);
	*pulBspInterfaceFunctions ++ = (U32)(PN532_NFC_AuthenticateBlock);	
	*pulBspInterfaceFunctions ++ = (U32)(PN532_NFC_ReadDataBlock);
	*pulBspInterfaceFunctions ++ = (U32)(PN532_NFC_WriteDataBlock);

	((void (*)(void))0x00020000)();   // Jump to app
}

int main (void)
{
    Timer_Main ();
    External_Interrupts_Initialize ();

    VIC_Main_Init();
    UART_Main();

    //IOPins_Main();
    LCD_Main();
	//EEPROM_I2C_Initialize ();
	//EEPROM_I2C_Test ();
	//PN532_NFC_Initialize ();
 
	U0_Printf("\r\n Jump To App");
	U1_Printf("\r\n Jump To App");
	JumpToApp ();

 	while (1)
	{
		//U0_Printf("\r\n ===== BSP End ====== ");
		//U1_Printf("\r\n ===== BSP End ====== ");

	}
}

