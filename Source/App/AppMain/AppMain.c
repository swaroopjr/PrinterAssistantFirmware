
#include "AppMain.h"
#include "LPC214x.H"
#include "Global.h"
#include "Global_Enums.h"

#define SYSTEM_DATA_EEP_ID 0x1
#define SYSTEM_DATA_EEP_ADDR 0x50
#define USER_NAME_NFC_BLOCK_NUMBER 0x1
#define USER_DATA_NFC_BLOCK_NUMBER 0x2

#define DBG_ERR pfnU1_Printf
#define DBG_PRT_ALWAYS pfnU1_Printf
#define DBG_PRT pfnU1_Printf
//#define DBG_PRT Global_NULL_Printf

typedef struct
{
	U8 ubVeryHighDotCountMSBCostPerPrint;
	U8 ubVeryHighDotCountLSBCostPerPrint;
	U8 ubHighDotCountMSBCostPerPrint;
	U8 ubHighDotCountLSBCostPerPrint;
	U8 ubMediumDotCountMSBCostPerPrint;
	U8 ubMediumDotCountLSBCostPerPrint;
	U8 ubLowDotCountMSBCostPerPrint;
	U8 ubLowDotCountLSBCostPerPrint;
	U8 ubVeryLowDotCountMSBCostPerPrint;
	U8 ubVeryLowDotCountLSBCostPerPrint;

} TPrintAssistant_SystemData;

typedef struct
{
	U8 aubUserName [16+1];
	U8 ubMSBBalance;
	U8 ubLSBBalance;
} TPrintAssistant_UserData;

enum TPrintAssistant_UserDataBitFlags
{
	USER_NAME_ONLY = 1,
	USER_COUNTS_ONLY = 2,
	USER_NAME_AND_COUNTS = 3
};


void (* pfnU0_Printf) (char *, ...);
char (* pfnUART0_GetChar) (void);
void (* pfnUART0_PutChar) (int);
char (* pfnUART0_GetCharWithinTime) (unsigned int);

void (* pfnU1_Printf) (char *, ...);
char (* pfnUART1_GetChar) (void);
void (* pfnUART1_PutChar) (int);
char (* pfnUART1_GetCharWithinTime) (unsigned int);

void (* pfnUART_GetHeadAndTail) (unsigned int *pulUART0_Head, unsigned int *pulUART0_Tail, unsigned int *pulUART1_Head, unsigned int *pulUART1_Tail);

void (* pfnIOPins_Test) (void);
void (* pfnIOPins_ControlTestLED) (U32);

void (* pfnLCD_DisplayStringInFirstLine) (char *);
void (* pfnLCD_DisplayStringInSecondLine) (char *);

U32 (* pfnFLASH_WriteFirmware) (U32, U32, U32);

void (*pfn_EEPROM_I2C_Initialize) (void);
void (*pfn_EEPROM_I2C_WriteData) (U32 ulSlaveAddress, U32 Address, U8 * BufferPtr, U32 SizeInBytes);
void (*pfn_EEPROM_I2C_ReadData) (U32 ulSlaveAddress, U32 Address, U8 * BufferPtr, U32 SizeInBytes);

void (*pfn_PN532_NFC_Initialize) (void);
unsigned int (*pfn_PN532_NFC_ReadPassiveTargetID) (void);
unsigned int (*pfn_PN532_NFC_AuthenticateBlock) (unsigned int ulBlockNumber);
unsigned int (*pfn_PN532_NFC_ReadDataBlock) (unsigned int ulBlockNumber, unsigned char *pubData);
unsigned int (*pfn_PN532_NFC_WriteDataBlock) (unsigned int ulBlockNumber, unsigned char *pubData);

TPrintAssistant_UserData stUserData;
TPrintAssistant_SystemData stSystemData;
BOOL fNFCUserCardDetected = FALSE;

void App_Delay (int ulValue)
{
    int i, j;
	for(i=0; i < ulValue; i++)
        j=0;
}

void GetBspInterfaceFunctionPtrs (void)
{
	U32 * pulBspInterfaceFunctions = (U32 *)(0x40007000);

	pfnU0_Printf = (void (*) (char *, ...))(*pulBspInterfaceFunctions ++);
	pfnUART0_PutChar = (void (*) (int))(*pulBspInterfaceFunctions ++);
	pfnUART0_GetChar = (char (*) (void))(*pulBspInterfaceFunctions ++); 
	pfnUART0_GetCharWithinTime = (char (*) (unsigned int))(*pulBspInterfaceFunctions ++); 

	pfnU1_Printf = (void (*) (char *, ...))(*pulBspInterfaceFunctions ++);
	pfnUART1_PutChar = (void (*) (int))(*pulBspInterfaceFunctions ++);
	pfnUART1_GetChar = (char (*) (void))(*pulBspInterfaceFunctions ++); 
	pfnUART1_GetCharWithinTime = (char (*) (unsigned int))(*pulBspInterfaceFunctions ++); 

	pfnUART_GetHeadAndTail = (void (*) (unsigned int *, unsigned int *, unsigned int *, unsigned int *))(*pulBspInterfaceFunctions ++); 

	pfnIOPins_Test = (void (*) (void))(*pulBspInterfaceFunctions ++);
	pfnIOPins_ControlTestLED = (void (*) (U32))(*pulBspInterfaceFunctions ++);

	pfnLCD_DisplayStringInFirstLine = (void (*) (char *))(*pulBspInterfaceFunctions ++);
	pfnLCD_DisplayStringInSecondLine = (void (*) (char *))(*pulBspInterfaceFunctions ++);

	pfnFLASH_WriteFirmware = (U32 (*)(U32, U32, U32))(*pulBspInterfaceFunctions ++);

	pfn_EEPROM_I2C_Initialize = (void (*) (void))(*pulBspInterfaceFunctions ++);
	pfn_EEPROM_I2C_WriteData = (void (*) (U32, U32, U8 *, U32))(*pulBspInterfaceFunctions ++);
	pfn_EEPROM_I2C_ReadData = (void (*) (U32, U32, U8 *, U32))(*pulBspInterfaceFunctions ++);

	pfn_PN532_NFC_Initialize = (void (*) (void))(*pulBspInterfaceFunctions ++);
	pfn_PN532_NFC_ReadPassiveTargetID = (unsigned int (*) (void))(*pulBspInterfaceFunctions ++);
	pfn_PN532_NFC_AuthenticateBlock = (unsigned int (*) (unsigned int))(*pulBspInterfaceFunctions ++);
	pfn_PN532_NFC_ReadDataBlock = (unsigned int (*) (unsigned int, unsigned char *))(*pulBspInterfaceFunctions ++);
	pfn_PN532_NFC_WriteDataBlock = (unsigned int (*) (unsigned int, unsigned char *))(*pulBspInterfaceFunctions ++);	
}



void App_DisplayUserData (U32 ulBitFlags)
{
	U32 i;
	U8 aubLCDDispArray [16+1];

	DBG_PRT_ALWAYS("\r\n App_DisplayUserData: Name[%s], Balance[%d.%d]", stUserData.aubUserName, stUserData.ubMSBBalance, stUserData.ubLSBBalance);

	if (ulBitFlags & 0x1)
	{
		Global_Memset (aubLCDDispArray, 0x00, sizeof (aubLCDDispArray));
		Global_SPrintf ((char *)aubLCDDispArray, "%s", stUserData.aubUserName);
		for (i = Global_Strlen(aubLCDDispArray); i<16; i++) aubLCDDispArray [i] = ' ';
		pfnLCD_DisplayStringInFirstLine ((char *)aubLCDDispArray);
	}

	if (ulBitFlags & 0x2)
	{
		Global_Memset (aubLCDDispArray, 0x00, sizeof (aubLCDDispArray));
		Global_SPrintf ((char *)aubLCDDispArray, "Bal : %d.%d %s", stUserData.ubMSBBalance, stUserData.ubLSBBalance, "INR");
		for (i = Global_Strlen(aubLCDDispArray); i<16; i++) aubLCDDispArray [i] = ' ';
		pfnLCD_DisplayStringInSecondLine ((char *)aubLCDDispArray);
	}
}

void App_ReadUserData (void)
{
	unsigned char aubUserReadData[16];

	Global_Memset ((unsigned char *)(&stUserData), 0x00, sizeof (TPrintAssistant_UserData));

	if(pfn_PN532_NFC_AuthenticateBlock (USER_NAME_NFC_BLOCK_NUMBER))
	{
		Global_Memset (aubUserReadData, 0x00, sizeof (aubUserReadData));
		pfn_PN532_NFC_ReadDataBlock (USER_NAME_NFC_BLOCK_NUMBER, aubUserReadData);
		Global_Memcpy((U8 *)(stUserData.aubUserName), aubUserReadData, Global_Strlen(aubUserReadData));
	}

	if(pfn_PN532_NFC_AuthenticateBlock (USER_DATA_NFC_BLOCK_NUMBER))
	{
		Global_Memset (aubUserReadData, 0x00, sizeof (aubUserReadData));
		pfn_PN532_NFC_ReadDataBlock (USER_DATA_NFC_BLOCK_NUMBER, aubUserReadData);
		stUserData.ubMSBBalance = aubUserReadData[0];
		stUserData.ubLSBBalance = aubUserReadData[1];
	}
}

void App_WriteUserData (U32 ulBitFlags)
{
	unsigned char aubUserWriteData[16];

	if (ulBitFlags & 0x1)
	{
		if(pfn_PN532_NFC_AuthenticateBlock (USER_NAME_NFC_BLOCK_NUMBER))
		{
			Global_Memset (aubUserWriteData, 0x00, sizeof (aubUserWriteData));
			Global_Memcpy(aubUserWriteData, (U8 *)(stUserData.aubUserName), Global_Strlen(stUserData.aubUserName));
			pfn_PN532_NFC_WriteDataBlock (USER_NAME_NFC_BLOCK_NUMBER, aubUserWriteData);
		}
	}

	if (ulBitFlags & 0x2)
	{	
		if(pfn_PN532_NFC_AuthenticateBlock (USER_DATA_NFC_BLOCK_NUMBER))
		{
			Global_Memset (aubUserWriteData, 0x00, sizeof (aubUserWriteData));
			aubUserWriteData[0] = stUserData.ubMSBBalance;
			aubUserWriteData[1] = stUserData.ubLSBBalance;
			pfn_PN532_NFC_WriteDataBlock (USER_DATA_NFC_BLOCK_NUMBER, aubUserWriteData);
		}
	}
}

BOOL App_CheckIfNfcCardPresent (void)
{
	if (pfn_PN532_NFC_ReadPassiveTargetID ())
		return TRUE;
	else
		return FALSE;
}

void App_ManualUpdateUserAndSystemData (void)
{
#if 1 //def SYSTEM_DATA_MANUAL_UPDATE
	Global_Memset ((unsigned char *)(&stSystemData), 0x00, sizeof (TPrintAssistant_SystemData));
	stSystemData.ubVeryHighDotCountMSBCostPerPrint = 2;
	stSystemData.ubVeryHighDotCountLSBCostPerPrint = 50;
	stSystemData.ubHighDotCountMSBCostPerPrint = 2;
	stSystemData.ubHighDotCountLSBCostPerPrint = 0;
	stSystemData.ubMediumDotCountMSBCostPerPrint = 1;
	stSystemData.ubMediumDotCountLSBCostPerPrint = 50;
	stSystemData.ubLowDotCountMSBCostPerPrint = 1;
	stSystemData.ubLowDotCountLSBCostPerPrint = 0;
	stSystemData.ubVeryLowDotCountMSBCostPerPrint = 0;
	stSystemData.ubVeryLowDotCountLSBCostPerPrint = 50;

	pfn_EEPROM_I2C_WriteData (SYSTEM_DATA_EEP_ID, SYSTEM_DATA_EEP_ADDR, (U8 *)(&stSystemData), sizeof (TPrintAssistant_SystemData));
#endif

#if 1 //def USER_DATA_MANUAL_UPDATE
	Global_Memset ((unsigned char *)(&stUserData), 0x00, sizeof (TPrintAssistant_UserData));
	Global_Memcpy(stUserData.aubUserName, (U8 *)("Swaroop"), sizeof("Swaroop"));
	stUserData.ubMSBBalance = 37;
	stUserData.ubLSBBalance = 28;

	DBG_PRT_ALWAYS ("\r\n Check and write NFC Card");
	if (App_CheckIfNfcCardPresent())
	{
		DBG_PRT_ALWAYS("\r\n User data Manual update: Block[%d,%d], Size[%d]", USER_NAME_NFC_BLOCK_NUMBER, USER_DATA_NFC_BLOCK_NUMBER, sizeof(stUserData));
		App_WriteUserData (USER_NAME_AND_COUNTS);

		App_ReadUserData ();
		App_DisplayUserData (USER_NAME_AND_COUNTS);
	}
#endif
}

void App_ReadSystemCost (void)
{
	pfn_EEPROM_I2C_ReadData (SYSTEM_DATA_EEP_ID, SYSTEM_DATA_EEP_ADDR, (U8 *)(&stSystemData), sizeof (TPrintAssistant_SystemData));
	DBG_PRT ("\r\n System Costs: Very High [%d.%d], High [%d.%d], Medium [%d.%d] Low [%d.%d], Very Low [%d.%d]", 
																					stSystemData.ubVeryHighDotCountMSBCostPerPrint,
																					stSystemData.ubVeryHighDotCountLSBCostPerPrint,
																					stSystemData.ubHighDotCountMSBCostPerPrint,
																					stSystemData.ubHighDotCountLSBCostPerPrint,
																					stSystemData.ubMediumDotCountMSBCostPerPrint,
																					stSystemData.ubMediumDotCountLSBCostPerPrint,
																					stSystemData.ubLowDotCountMSBCostPerPrint,
																					stSystemData.ubLowDotCountLSBCostPerPrint,
																					stSystemData.ubVeryLowDotCountMSBCostPerPrint,
																					stSystemData.ubVeryLowDotCountLSBCostPerPrint);

}

void App_DisplayPopupString (U8 *pubPopupStringL1, U8 *pubPopupStringL2)
{
	U32 i;
	U8 aubLCDDispArray [16+1];
	
	Global_Memset (aubLCDDispArray, 0x00, sizeof (aubLCDDispArray));
	Global_SPrintf ((char *)aubLCDDispArray, "%s", pubPopupStringL1);
	for (i = Global_Strlen(aubLCDDispArray); i<16; i++) aubLCDDispArray [i] = ' ';
	pfnLCD_DisplayStringInFirstLine ((char *)aubLCDDispArray);

	Global_Memset (aubLCDDispArray, 0x00, sizeof (aubLCDDispArray));
	Global_SPrintf ((char *)aubLCDDispArray, "%s", pubPopupStringL2);
	for (i = Global_Strlen(aubLCDDispArray); i<16; i++) aubLCDDispArray [i] = ' ';
	pfnLCD_DisplayStringInSecondLine ((char *)aubLCDDispArray);


	for (i=0; i<50; i++)	App_Delay (10000);
	if (fNFCUserCardDetected == TRUE)	App_DisplayUserData (USER_NAME_AND_COUNTS); 
}

U8 App_GetSerialChar (void)
{
	U8 ubData = 0xFF;
	ubData = pfnUART0_GetCharWithinTime(10000);
	DBG_PRT ("\r\n SerialDataReceived[%c][0x%x]", ubData, ubData);
	return ubData;
}

U8 App_GetSerialCmd (void)
{
	U8 ubData = 0xFF;
	BOOL fValidPacket = FALSE;
	//U32 i;

	if ('[' == App_GetSerialChar())
	{
		if ('{' == App_GetSerialChar())
		{
			if ('<' == App_GetSerialChar())
			{
				ubData = App_GetSerialChar();

				if ('>' == App_GetSerialChar())
				{
					if ('}' == App_GetSerialChar())
					{
						if (']' == App_GetSerialChar())
						{
							fValidPacket = TRUE;
						}
					}
				}
			}
		}
	}

	if (fValidPacket)
	{
		/*U8 aubLCDDispArray [16+1];
		Global_Memset (aubLCDDispArray, 0x00, sizeof (aubLCDDispArray));
		Global_SPrintf ((char *)aubLCDDispArray, "Cmd : %d", ubData);
		for (i = Global_Strlen(aubLCDDispArray); i<16; i++) aubLCDDispArray [i] = ' ';
		pfnLCD_DisplayStringInSecondLine ((char *)aubLCDDispArray);*/
		DBG_PRT ("\r\n Cmd[%c]", ubData);
		return ubData;
	}
	else
	{
		DBG_ERR ("\r\n Invalid Packet");
		//App_DisplayPopupString ((U8 *)("    Invalid     "), (U8 *)("     Packet     "));
		return 0xFF;
	}
}

void App_GetAndSetSerialUserData (void)
{
	TPrintAssistant_UserData stTempUserData;
	BOOL fValidPacket = FALSE;
	U32 i;

	Global_Memset ((unsigned char *)(&stTempUserData), 0x00, sizeof (TPrintAssistant_UserData));

	if ('[' == App_GetSerialChar())
	{
		if ('{' == App_GetSerialChar())
		{
			if ('<' == App_GetSerialChar())
			{
				for (i=0; i<sizeof(stTempUserData.aubUserName)-1 ; i++)
					stTempUserData.aubUserName[i] = App_GetSerialChar();

				stTempUserData.ubMSBBalance = App_GetSerialChar();
				stTempUserData.ubLSBBalance = App_GetSerialChar();

				if ('>' == App_GetSerialChar())
				{
					if ('}' == App_GetSerialChar())
					{
						if (']' == App_GetSerialChar())
						{
							fValidPacket = TRUE;
						}
					}
				}
			}
		}
	}

	if (fValidPacket)
	{
		DBG_PRT_ALWAYS ("\r\n Check and write NFC Card");
		if (fNFCUserCardDetected == TRUE)
		{
			DBG_PRT_ALWAYS("\r\n User data update: User Name[%s], Balance[%d.%d]", stTempUserData.aubUserName, stTempUserData.ubMSBBalance, stTempUserData.ubLSBBalance);
			Global_Memcpy((U8 *)(&stUserData), (U8 *)(&stTempUserData), sizeof(TPrintAssistant_UserData));
			App_WriteUserData (USER_NAME_AND_COUNTS);
			App_DisplayPopupString ((U8 *)("   User Data    "), (U8 *)("    Updated     "));
		}
		else
		{
			DBG_PRT_ALWAYS("\r\n NFC Card Not Present");
			App_DisplayPopupString ((U8 *)("    NFC Card    "), (U8 *)("  Not Present   "));
		}
	}
	else
	{
		DBG_ERR ("\r\n App_GetAndSetSerialUserData: Invalid Packet");
		App_DisplayPopupString ((U8 *)("    Invalid     "), (U8 *)("     Packet     "));
	}
}

void App_GetAndSetSerialMachineData (void)
{
	TPrintAssistant_SystemData stTempSystemData;
	BOOL fValidPacket = FALSE;

	Global_Memset ((unsigned char *)(&stTempSystemData), 0x00, sizeof (TPrintAssistant_SystemData));

	if ('[' == App_GetSerialChar())
	{
		if ('{' == App_GetSerialChar())
		{
			if ('<' == App_GetSerialChar())
			{
				stTempSystemData.ubVeryHighDotCountMSBCostPerPrint = App_GetSerialChar();
				stTempSystemData.ubVeryHighDotCountLSBCostPerPrint = App_GetSerialChar();
				stTempSystemData.ubHighDotCountMSBCostPerPrint     = App_GetSerialChar();
				stTempSystemData.ubHighDotCountLSBCostPerPrint     = App_GetSerialChar();
				stTempSystemData.ubMediumDotCountMSBCostPerPrint   = App_GetSerialChar();
				stTempSystemData.ubMediumDotCountLSBCostPerPrint   = App_GetSerialChar();
				stTempSystemData.ubLowDotCountMSBCostPerPrint      = App_GetSerialChar();
				stTempSystemData.ubLowDotCountLSBCostPerPrint      = App_GetSerialChar();
				stTempSystemData.ubVeryLowDotCountMSBCostPerPrint  = App_GetSerialChar();
				stTempSystemData.ubVeryLowDotCountLSBCostPerPrint  = App_GetSerialChar();

				if ('>' == App_GetSerialChar())
				{
					if ('}' == App_GetSerialChar())
					{
						if (']' == App_GetSerialChar())
						{
							fValidPacket = TRUE;
						}
					}
				}
			}
		}
	}

	if (fValidPacket)
	{
		Global_Memcpy((U8 *)(&stSystemData), (U8 *)(&stTempSystemData), sizeof(TPrintAssistant_SystemData));
		pfn_EEPROM_I2C_WriteData (SYSTEM_DATA_EEP_ID, SYSTEM_DATA_EEP_ADDR, (U8 *)(&stSystemData), sizeof (TPrintAssistant_SystemData));	
		App_DisplayPopupString ((U8 *)(" Machine Data  "), (U8 *)("    Updated     "));
	}
	else
	{
		DBG_ERR ("\r\n App_GetAndSetSerialMachineData: Invalid Packet");
		App_DisplayPopupString ((U8 *)("    Invalid     "), (U8 *)("     Packet     "));
	}
}

void App_SendSerialUserData (void)
{
	int i;
	
	if (fNFCUserCardDetected == TRUE)
	{
		pfnUART0_PutChar ('[');
		pfnUART0_PutChar ('{');
		pfnUART0_PutChar ('<');
		
		for (i=0;i<16;i++)
			pfnUART0_PutChar (stUserData.aubUserName[i]);

		pfnUART0_PutChar (stUserData.ubMSBBalance);
		pfnUART0_PutChar (stUserData.ubLSBBalance);
	
		pfnUART0_PutChar ('>');
		pfnUART0_PutChar ('}');
		pfnUART0_PutChar (']');

	}
	else
	{
		DBG_PRT_ALWAYS("\r\n NFC Card Not Present");
		App_DisplayPopupString ((U8 *)("	NFC Card	"), (U8 *)("  Not Present	"));
	}
}

void App_SendSerialMachineData (void)
{
	pfnUART0_PutChar ('[');
	pfnUART0_PutChar ('{');
	pfnUART0_PutChar ('<');

	pfnUART0_PutChar (stSystemData.ubVeryHighDotCountMSBCostPerPrint);
	pfnUART0_PutChar (stSystemData.ubVeryHighDotCountLSBCostPerPrint);
	pfnUART0_PutChar (stSystemData.ubHighDotCountMSBCostPerPrint);
	pfnUART0_PutChar (stSystemData.ubHighDotCountLSBCostPerPrint);
	pfnUART0_PutChar (stSystemData.ubMediumDotCountMSBCostPerPrint);
	pfnUART0_PutChar (stSystemData.ubMediumDotCountLSBCostPerPrint);
	pfnUART0_PutChar (stSystemData.ubLowDotCountMSBCostPerPrint);
	pfnUART0_PutChar (stSystemData.ubLowDotCountLSBCostPerPrint);
	pfnUART0_PutChar (stSystemData.ubVeryLowDotCountMSBCostPerPrint);
	pfnUART0_PutChar (stSystemData.ubVeryLowDotCountLSBCostPerPrint);

	pfnUART0_PutChar ('>');
	pfnUART0_PutChar ('}');
	pfnUART0_PutChar (']');
}


void App_ProcessSerialCmd (U8 ubSerialCmd)
{
	U32 ulCurrentUserBalance;
	U32 ulCostToBeDecremented;
	U32 ulNewUserBalance;
	BOOL fUpdateUserCounts = FALSE;

	if (ubSerialCmd == 0xFF)
		return;

	switch(ubSerialCmd)
	{
		case '0':
			DBG_PRT ("\r\n Reserved Cmd");
		break;

		case '1':
		{
			DBG_PRT ("\r\n Paper Empty");
			App_DisplayPopupString ((U8 *)("     Paper      "), (U8 *)("     Empty      "));
		}
		break;

		case '2':
		{
			unsigned int ulPrintEnabled = FALSE;
			DBG_PRT ("\r\n Check if print Enabled");
			if (fNFCUserCardDetected == TRUE)
			{
				if (((stUserData.ubMSBBalance*100) + stUserData.ubLSBBalance) >= ((stSystemData.ubVeryHighDotCountMSBCostPerPrint*100) + stSystemData.ubVeryHighDotCountLSBCostPerPrint))
				{
					ulPrintEnabled = TRUE;
				}
			}

			pfnUART0_PutChar ('[');
			pfnUART0_PutChar ('{');
			pfnUART0_PutChar ('<');

			if (ulPrintEnabled == TRUE)
				pfnUART0_PutChar (0x1);
			else
				pfnUART0_PutChar (0x2);

			pfnUART0_PutChar ('>');
			pfnUART0_PutChar ('}');
			pfnUART0_PutChar (']');
		}
		break;

		case '3':
			DBG_PRT ("\r\n Very High Dot Count Print");
			if (fNFCUserCardDetected == TRUE)
			{
				ulCostToBeDecremented = (stSystemData.ubVeryHighDotCountMSBCostPerPrint * 100) + stSystemData.ubVeryHighDotCountLSBCostPerPrint;
				fUpdateUserCounts = TRUE;
			}
		break;

		case '4':
			DBG_PRT ("\r\n High Dot Count Print");
			if (fNFCUserCardDetected == TRUE)
			{			
				ulCostToBeDecremented = (stSystemData.ubHighDotCountMSBCostPerPrint * 100) + stSystemData.ubHighDotCountLSBCostPerPrint;
				fUpdateUserCounts = TRUE;
			}
		break;

		case '5':
			DBG_PRT ("\r\n Medium Dot Count Print");
			if (fNFCUserCardDetected == TRUE)
			{
				ulCostToBeDecremented = (stSystemData.ubMediumDotCountMSBCostPerPrint * 100) + stSystemData.ubMediumDotCountLSBCostPerPrint;
				fUpdateUserCounts = TRUE;
			}
		break;

		case '6':
			DBG_PRT ("\r\n Low Dot Count Print");
			if (fNFCUserCardDetected == TRUE)
			{
				ulCostToBeDecremented = (stSystemData.ubLowDotCountMSBCostPerPrint * 100) + stSystemData.ubLowDotCountLSBCostPerPrint;
				fUpdateUserCounts = TRUE;
			}
		break;

		case '7':
			DBG_PRT ("\r\n Very Low Dot Count Print");
			if (fNFCUserCardDetected == TRUE)
			{
				ulCostToBeDecremented = (stSystemData.ubVeryLowDotCountMSBCostPerPrint * 100) + stSystemData.ubVeryLowDotCountLSBCostPerPrint;
				fUpdateUserCounts = TRUE;
			}
		break;

		case '8':
			DBG_PRT ("\r\n Get And Set User Data");
			App_GetAndSetSerialUserData ();
		break;

		case '9':
			DBG_PRT ("\r\n Get And Set Machine Data");
			App_GetAndSetSerialMachineData ();
		break;

		case 'A':
			DBG_PRT ("\r\n Send User Data");
			App_SendSerialUserData ();
		break;

		case 'B':
			DBG_PRT ("\r\n Send Machine Data");
			App_SendSerialMachineData ();
		break;	
	}

	if (fUpdateUserCounts)
	{
		ulCurrentUserBalance = (stUserData.ubMSBBalance * 100) + stUserData.ubLSBBalance;
		if(ulCurrentUserBalance < ulCostToBeDecremented)
		{
			ulNewUserBalance = 0;
		}
		else
		{
			ulNewUserBalance = ulCurrentUserBalance - ulCostToBeDecremented;
		}

		stUserData.ubMSBBalance = ulNewUserBalance / 100;
		stUserData.ubLSBBalance = ulNewUserBalance % 100;	
		DBG_PRT ("\r\n Current[%d] Dec[%d] New[%d][%d.%d]", ulCurrentUserBalance, ulCostToBeDecremented, ulNewUserBalance, stUserData.ubMSBBalance, stUserData.ubLSBBalance);
		App_WriteUserData (USER_COUNTS_ONLY);
		App_DisplayUserData (USER_COUNTS_ONLY);
	}
}

int main (void)
{
	U8 ubSerialCmd = 0xFF;
	unsigned int aulSerialHeadTail [4];

	GetBspInterfaceFunctionPtrs ();

	pfnU0_Printf ("\r\n Welcome To Print Assistant Application [%d] ... ", 0);
	pfnU1_Printf ("\r\n Welcome To Print Assistant Application [%d] ... ", 1);

	pfnLCD_DisplayStringInFirstLine ("     PRINT      ");
	pfnLCD_DisplayStringInSecondLine ("   ASSISTANT    ");

	pfn_EEPROM_I2C_Initialize ();
	pfn_PN532_NFC_Initialize ();

	//App_ManualUpdateUserAndSystemData ();
	App_ReadSystemCost ();

	while (1)
	{
		if (App_CheckIfNfcCardPresent())
		{
			DBG_PRT ("\r\n NFC Card Present");
			if (fNFCUserCardDetected == FALSE)
			{
				App_ReadUserData ();
				App_DisplayUserData (USER_NAME_AND_COUNTS);
				fNFCUserCardDetected = TRUE;
			}
		}
		else
		{
			fNFCUserCardDetected = FALSE;
			pfnLCD_DisplayStringInFirstLine ("     PRINT      ");
			pfnLCD_DisplayStringInSecondLine ("   ASSISTANT    ");
		}

		ubSerialCmd = App_GetSerialCmd ();
		App_ProcessSerialCmd (ubSerialCmd);

		pfnUART_GetHeadAndTail(&(aulSerialHeadTail[0]), &(aulSerialHeadTail[1]), &(aulSerialHeadTail[2]), &(aulSerialHeadTail[3]));
		DBG_PRT_ALWAYS ("\r\n Uart Head and Tail: U0H[%d] U0T[%d] U1H[%d] U1T[%d]", aulSerialHeadTail[0], aulSerialHeadTail[1], aulSerialHeadTail[2], aulSerialHeadTail[3]);

		App_Delay (1000);	
	}

	pfnLCD_DisplayStringInFirstLine ("   Application  ");
	pfnLCD_DisplayStringInSecondLine ("       End      ");
	while (1);
}


