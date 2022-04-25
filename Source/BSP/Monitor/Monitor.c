//Monitor Code

#include "Global_Enums.h"
#include "Monitor.h"
#include "UART_Drv.h"
#include "Printf.h"
#include "Flash_Main.h"
#include "Global.h"
#include "IOPins.h"
#include <string.h>
#include "LCD_Display.h"
#include "EEPROM_I2C.h"

#define FIRMWARE_DOWN_RAM_ADDR 0x40004000

static unsigned char gaubCmd[16];

typedef struct
{
	unsigned int ulSignature;
	unsigned int ulFlashStartAddr;
	unsigned int ulFirmwareSize;
	unsigned int ulFirmwareCheckSum;
} T_FIRMWARE_HEADER;

unsigned int MONITOR_GetFirmware (unsigned int ulUartChannel)
{
	unsigned char * pubFirmwareDownRamPtr = (unsigned char *)FIRMWARE_DOWN_RAM_ADDR;
	PRINTF_FUN_PTR pfn_Ux_Printf;

	pfn_Ux_Printf = PrintfGetFunctionPtr (ulUartChannel);
	pfn_Ux_Printf("\r\n Firmware Receive Mode%d>", ulUartChannel);

	while (1)
	{
		if (ulUartChannel == 0)
		{
			*pubFirmwareDownRamPtr = UART0_GetChar();
		}
		else if (ulUartChannel == 1)
		{
			*pubFirmwareDownRamPtr = UART1_GetChar();
		}
		else
		{
			pfn_Ux_Printf("\r\n Unknown Channel %x - Return False", ulUartChannel);
			return FALSE;
		}
			
		if ( (*(pubFirmwareDownRamPtr-3)== '%') &&
			 (*(pubFirmwareDownRamPtr-2)== 'I') &&
			 (*(pubFirmwareDownRamPtr-1)== 'M') &&
			 (*(pubFirmwareDownRamPtr-0)== 'G'))
		{
			pfn_Ux_Printf("\r\n Firmware Copied To RAM Size = %x", pubFirmwareDownRamPtr - FIRMWARE_DOWN_RAM_ADDR);
			return TRUE;
		}

		pubFirmwareDownRamPtr ++;
	}
}

unsigned int MONITOR_ValidateFirmware (unsigned int * pulFlashStartAddress, unsigned int * pulFirmwareSize, unsigned int ulUartChannel)
{
	unsigned char * pubFirmwareDownRamPtr = (unsigned char *)FIRMWARE_DOWN_RAM_ADDR;
	unsigned int ulIdx;
	unsigned char ubCalChkSum = (unsigned char) 0;
	T_FIRMWARE_HEADER stFirmwareHeader;
	T_FIRMWARE_HEADER * pstRawFirmwareHeader = (T_FIRMWARE_HEADER *)(FIRMWARE_DOWN_RAM_ADDR);
	PRINTF_FUN_PTR pfn_Ux_Printf;

	pfn_Ux_Printf = PrintfGetFunctionPtr (ulUartChannel);
	pfn_Ux_Printf("\r\n Validate Firmware");

	stFirmwareHeader.ulSignature = Global_ChangeEndian (pstRawFirmwareHeader->ulSignature);
	stFirmwareHeader.ulFlashStartAddr = Global_ChangeEndian (pstRawFirmwareHeader->ulFlashStartAddr);
	stFirmwareHeader.ulFirmwareSize = Global_ChangeEndian (pstRawFirmwareHeader->ulFirmwareSize);
	stFirmwareHeader.ulFirmwareCheckSum = Global_ChangeEndian (pstRawFirmwareHeader->ulFirmwareCheckSum);
	

	pfn_Ux_Printf("\r\n Signature[%x] %c%c%c%c", stFirmwareHeader.ulSignature, stFirmwareHeader.ulSignature & 0xFF, (stFirmwareHeader.ulSignature>>8) & 0xFF, (stFirmwareHeader.ulSignature>>16) & 0xFF, stFirmwareHeader.ulSignature>>24);
	pfn_Ux_Printf("\r\n Firmware Start Address %x", stFirmwareHeader.ulFlashStartAddr);
	pfn_Ux_Printf("\r\n Firmware Size %x", stFirmwareHeader.ulFirmwareSize);
	pfn_Ux_Printf("\r\n Firmware Checksum %x", stFirmwareHeader.ulFirmwareCheckSum);

	if ( stFirmwareHeader.ulSignature != 0x24494D47) //$IMG
	{
		pfn_Ux_Printf("\r\n $IMG Signature Missing in firmware");
		return FALSE;
	}

	// Firmware Start address check
	if (stFirmwareHeader.ulFlashStartAddr < 0x00020000)
	{
		pfn_Ux_Printf("\r\n Firmware can be downloaded 0x00020000 onwards");
		return FALSE;
	}

	// Firmware Size check
	if ((stFirmwareHeader.ulFirmwareSize == 0) || 
		(stFirmwareHeader.ulFirmwareSize > 32*1024))
	{
		pfn_Ux_Printf("\r\n Firmware Size Error");
		return FALSE;
	}
	
	// Firmware checksum validation
	for (ulIdx = 0; ulIdx < stFirmwareHeader.ulFirmwareSize; ulIdx++)
	{
		ubCalChkSum += (*(pubFirmwareDownRamPtr + sizeof(T_FIRMWARE_HEADER) + ulIdx));
	}

	if (ubCalChkSum != stFirmwareHeader.ulFirmwareCheckSum)
	{
		pfn_Ux_Printf("\r\n Firmware checksum error! Calculated Checksum = %x", ubCalChkSum);
		return FALSE;
	}

	// Firmware OK .. all test passed
	__asm{mov ulIdx, sp};
	pfn_Ux_Printf("\r\n Firmware OK %x", ulIdx);
	*pulFlashStartAddress = stFirmwareHeader.ulFlashStartAddr;
	*pulFirmwareSize = stFirmwareHeader.ulFirmwareSize;
	return TRUE;
}

void MONITOR_FirmwareDownload (unsigned int ulUartChannel)
{
	unsigned int ulRetValue = 0;
	unsigned int ulFlashStartAddr = 0xFFFFFFFF, ulFirmwareSize = 0;
	PRINTF_FUN_PTR pfn_Ux_Printf;

	pfn_Ux_Printf = PrintfGetFunctionPtr (ulUartChannel);
	pfn_Ux_Printf("\r\n Send Firmware%d>", ulUartChannel);

	ulRetValue = MONITOR_GetFirmware (ulUartChannel);

	if (ulRetValue)
	{
		if (MONITOR_ValidateFirmware (&ulFlashStartAddr, &ulFirmwareSize, ulUartChannel))
		{
			if(FLASH_WriteFirmware ((unsigned int)(FIRMWARE_DOWN_RAM_ADDR + sizeof(T_FIRMWARE_HEADER)), ulFlashStartAddr, ulFirmwareSize))
			{
				pfn_Ux_Printf("\r\n Flash Firmware Download OK !!");
			}
			else
			{
				pfn_Ux_Printf("\r\n Flash Deneyed Firmware Download");
			}
		}
		else
		{
			pfn_Ux_Printf("\r\n Firmware Validation Fail");
		}			
	}
	else
	{
		pfn_Ux_Printf("\r\n Firmware Download Fail");
	}
}

void MONITOR_EEPROM_WRITE (unsigned int ulUartChannel)
{
	unsigned int ulSlaveAddress = 0, ulAddress = 0, ulByteCount = 0, ulResult = 0;
	unsigned char aubData[16] = {0};
	unsigned int i = 0;
	PRINTF_FUN_PTR pfn_Ux_Printf;

	pfn_Ux_Printf = PrintfGetFunctionPtr (ulUartChannel);

	/*  Get Slave Address */
	pfn_Ux_Printf("\r\n Enter Slave Address in Hex (without 0x): ");
	MONITOR_GetCommand (ulUartChannel);
	ulResult = Global_ConvertAsciiToHex (gaubCmd, &ulSlaveAddress);
	if (!ulResult)
	{
		pfn_Ux_Printf("\r\n Invalid Entry");
		return;
	}

	/*  Get EEPROM location Address */
	pfn_Ux_Printf("\r\n Enter Address in Hex (without 0x): ");
	MONITOR_GetCommand (ulUartChannel);
	ulResult = Global_ConvertAsciiToHex (gaubCmd, &ulAddress);
	if (!ulResult)
	{
		pfn_Ux_Printf("\r\n Invalid Entry");
		return;
	}

	/*  Get Number of bytes to be written */
	pfn_Ux_Printf("\r\n Enter Byte count to be written in Hex (without 0x): ");
	MONITOR_GetCommand (ulUartChannel);
	ulResult = Global_ConvertAsciiToHex (gaubCmd, &ulByteCount);
	if ((!ulResult) || (ulByteCount > sizeof(aubData)))
	{
		pfn_Ux_Printf("\r\n Invalid Entry. Byte count has to be 1 to %d", sizeof (aubData));
		return;
	}	
	
	/*  Get data to be written */
	pfn_Ux_Printf("\r\n Enter Data to be written in Hex (without 0x): ");
	for (i=0; i<ulByteCount; i++)
	{
		unsigned int ulTempData = 0;
		MONITOR_GetCommand (ulUartChannel);
		pfn_Ux_Printf("\r\n");
		ulResult = Global_ConvertAsciiToHex (gaubCmd, (unsigned int *)(&ulTempData));
		if (!ulResult)
		{
			pfn_Ux_Printf("\r\n Invalid Entry");
			return;
		}
		aubData[i] = (unsigned char) ulTempData;
	}

	/*  Display data to be written  */
	pfn_Ux_Printf("\r\n Write eeprom SlaveAddress[%d], Address[%x]", ulSlaveAddress, ulAddress);
	for (i=0; i<ulByteCount; i++)
	{
		pfn_Ux_Printf("\t [%x]", aubData[i]);
	}

	/*  Write EEPROM  */
	EEPROM_I2C_WriteData(ulSlaveAddress, ulAddress, (unsigned char *)(aubData), ulByteCount);
}

void MONITOR_EEPROM_READ (unsigned int ulUartChannel)
{
	unsigned int ulSlaveAddress = 0, ulAddress = 0, ulByteCount = 0, ulResult = 0;
	unsigned char aubData[16] = {0};
	unsigned int i = 0;
	PRINTF_FUN_PTR pfn_Ux_Printf;

	pfn_Ux_Printf = PrintfGetFunctionPtr (ulUartChannel);

	/*  Get Slave Address */
	pfn_Ux_Printf("\r\n Enter Slave Address in Hex (without 0x): ");
	MONITOR_GetCommand (ulUartChannel);
	ulResult = Global_ConvertAsciiToHex (gaubCmd, &ulSlaveAddress);
	if (!ulResult)
	{
		pfn_Ux_Printf("\r\n Invalid Entry");
		return;
	}
	
	/*  Get EEPROM location Address */
	pfn_Ux_Printf("\r\n Enter Address in Hex (without 0x): ");
	MONITOR_GetCommand (ulUartChannel);
	ulResult = Global_ConvertAsciiToHex (gaubCmd, &ulAddress);
	if (!ulResult)
	{
		pfn_Ux_Printf("\r\n Invalid Entry");
		return;
	}
	
	/*  Get Number of bytes to be read */
	pfn_Ux_Printf("\r\n Enter Byte count to be read in Hex (without 0x): ");
	MONITOR_GetCommand (ulUartChannel);
	ulResult = Global_ConvertAsciiToHex (gaubCmd, &ulByteCount);
	if ((!ulResult) || (ulByteCount > sizeof(aubData)))
	{
		pfn_Ux_Printf("\r\n Invalid Entry. Byte count has to be 1 to 0x%x", sizeof (aubData));
		return;
	}

	/*  Read from EEPROM */
	EEPROM_I2C_ReadData(ulSlaveAddress, ulAddress, (unsigned char *)(aubData), ulByteCount);

	/*  Display Data */
	pfn_Ux_Printf("\r\n Read eeprom SlaveAddress [%d], Address[%x]", ulSlaveAddress, ulAddress);
	for (i=0; i<ulByteCount; i++)
	{
		pfn_Ux_Printf("\t [%x]", aubData[i]);
	}
}

void MONITOR_LCD_Disp (unsigned int ulUartChannel, unsigned int ulLine)
{
	PRINTF_FUN_PTR pfn_Ux_Printf;
	pfn_Ux_Printf = PrintfGetFunctionPtr (ulUartChannel);

	pfn_Ux_Printf("\r\n Enter String: ");

	MONITOR_GetCommand (ulUartChannel);

	if (ulLine == 1)
		LCD_DisplayStringInFirstLine ((char *)gaubCmd);
	else if (ulLine == 2)
		LCD_DisplayStringInSecondLine ((char *)gaubCmd);
}

unsigned int MONITOR_ProcessCommands (unsigned char * paubCmd, unsigned int ulUartChannel)
{
	PRINTF_FUN_PTR pfn_Ux_Printf;
	pfn_Ux_Printf = PrintfGetFunctionPtr (ulUartChannel);	

	if (!strcmp((const char *)(paubCmd), (const char *)("quit")))
	{
		return FALSE;
	}
	else if (!strcmp((const char *)(paubCmd), (const char *)("exit")))
	{
		return FALSE;
	}
	else if (!strcmp((const char *)(paubCmd), (const char *)("fl")))
	{
		MONITOR_FirmwareDownload (ulUartChannel);
		return TRUE;
	}
	else if (!strcmp((const char *)(paubCmd), (const char *)("blink")))
	{
		IOPins_Main ();
		return TRUE;
	}
	else if (!strcmp((const char *)(paubCmd), (const char *)("e2pini")))
	{
		EEPROM_I2C_Initialize ();
		return TRUE;
	}	
	else if (!strcmp((const char *)(paubCmd), (const char *)("e2pwr")))
	{
		MONITOR_EEPROM_WRITE (ulUartChannel);
		return TRUE;
	}	
	else if (!strcmp((const char *)(paubCmd), (const char *)("e2prd")))
	{
		MONITOR_EEPROM_READ (ulUartChannel);
		return TRUE;
	}	
	else if (!strcmp((const char *)(paubCmd), (const char *)("lcdini")))
	{
		LCD_Init ();
		return TRUE;
	}	
	else if (!strcmp((const char *)(paubCmd), (const char *)("lcd1")))
	{
		MONITOR_LCD_Disp (ulUartChannel, 1);
		return TRUE;
	}	
	else if (!strcmp((const char *)(paubCmd), (const char *)("lcd2")))
	{
		MONITOR_LCD_Disp (ulUartChannel, 2);
		return TRUE;
	}
	else 
	{
		pfn_Ux_Printf("\r\n Invalid Command");
		return TRUE;
	}	
}

void MONITOR_GetCommand (unsigned int ulUartChannel)
{
	unsigned char ubData = 0;
	unsigned int ulIdx;

	for (ulIdx = 0; ulIdx < sizeof (gaubCmd); ulIdx++)
	{
		gaubCmd[ulIdx] = NULL;
	}
	ulIdx = 0;

	if (ulUartChannel == 0)
	{
		while (1)
		{
			if (ulIdx == sizeof(gaubCmd))
				break;

			ubData = UART0_GetChar();
			UART0_PutChar(ubData);

			if ((ubData == 0x0D) || (ubData == 0x0A)) // Enter key carriage return
				break;

			gaubCmd[ulIdx ++] = ubData;
		}
	}
	else if (ulUartChannel == 1)
	{
		while (1)
		{
			if (ulIdx == sizeof(gaubCmd))
				break;
		
			ubData = UART1_GetChar();
			UART1_PutChar(ubData);

			if ((ubData == 0x0D) || (ubData == 0x0A)) // Enter key carriage return
				break;
		
			gaubCmd[ulIdx ++] = ubData;
		}
	}
	else
	{
		return;
	}
}

void MONITOR_Entry (unsigned int ulUartChannel)
{
	unsigned int ulRetValue = 0;

	if (ulUartChannel == 0)
	{
		UART0_Printf("\r\n Welcome to monitor mode (Uart - 0) ... ");
		LCD_DisplayStringInFirstLine ("Monitor Mode ...");
		LCD_DisplayStringInSecondLine ("Uart - 0        ");
		while (1)
		{
			UART0_Printf("\r\n PROBE0>");
			MONITOR_GetCommand (0);
			ulRetValue = MONITOR_ProcessCommands (gaubCmd,  0);
			if (ulRetValue == 0)
				break;
		}
	}
	else if (ulUartChannel == 1)
	{
		UART1_Printf("\r\n Welcome to monitor mode (Uart - 1) ... ");
		LCD_DisplayStringInFirstLine ("Monitor Mode ...");
		LCD_DisplayStringInSecondLine ("Uart - 1        ");
		while (1)
		{
			UART1_Printf("\r\n PROBE1>");
			MONITOR_GetCommand (1);
			ulRetValue = MONITOR_ProcessCommands (gaubCmd, 1);
			if (ulRetValue == 0)
				break;
		}
	}
	else
	{
		UART0_Printf("\r\n Unknown Uart channel. Return Monitor mode ... ");
		UART1_Printf("\r\n Unknown Uart channel. Return Monitor mode ... ");
		return;
	}

}

void MONITOR_CheckMonitorMode (void)
{
	unsigned int ulMonitorModeCnt = 0;
	char Ch0_Data = 0, Ch1_Data = 0;

	UART0_FlushBuffer ();
	UART1_FlushBuffer ();

	while (1)
	{
		Ch0_Data = UART0_GetCharWithinTime(50);
		Ch1_Data = UART1_GetCharWithinTime(500);
		//U0_Printf("\r\n Ch0[%x], Ch1[%x], ulMonitorModeCnt[%d]", Ch0_Data, Ch1_Data, ulMonitorModeCnt);
		//U1_Printf("\r\n Ch0[%x], Ch1[%x], ulMonitorModeCnt[%d]", Ch0_Data, Ch1_Data, ulMonitorModeCnt);

		if ((Ch0_Data == 0x0D) || (Ch0_Data == 0x0A)) // Enter key carriage return
		{
			MONITOR_Entry (0);
			break;
		}
		else if ((Ch1_Data == 0x0D) || (Ch1_Data == 0x0A)) // Enter key carriage return
		{
			MONITOR_Entry (1);
			break;
		}
		else
		{
			ulMonitorModeCnt ++;
		}

		if (ulMonitorModeCnt > 1000)
		{
			break;
		}
	}
}


