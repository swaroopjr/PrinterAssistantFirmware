//Flash Code

#include "Global_Enums.h"
#include "Flash_Drv.h"

#define RAM_TO_FLASH_COPY_SIZE 512

void Flash_Memset (unsigned char * pubOutputBuff, unsigned char ubData, unsigned int ulSize)
{
	while (ulSize --)
	{
		*pubOutputBuff ++ = ubData;
	}
}

/************************************************************************************************
Command: Prepare sector(s) for write operation

Input:
Command code: 50
Param0: Start Sector Number
Param1: End Sector Number: Should be greater than or equal to start sector number.

Status Code:
CMD_SUCCESS |
BUSY |
INVALID_SECTOR

Result: None

Description:
This command must be executed before executing "Copy RAM to Flash" or "Erase Sector(s)"
command. Successful execution of the "Copy RAM to Flash" or "Erase Sector(s)" command causes
relevant sectors to be protected again. The boot sector can not be prepared by this command. To
prepare a single sector use the same "Start" and "End" sector numbers.
************************************************************************************************/
unsigned int FLASH_PrepareSectorsWrite (unsigned int StartSector, unsigned int EndSector)
{
    unsigned int Temp;

	IAP_Result[IAP_CMD_STATUS_CODE] = 0xFF;
	IAP_Result[IAP_CMD_RESULT_ONE] = 0xFF;
	
	IAP_Command[IAP_CMD_CODE] = 50;
	IAP_Command[IAP_CMD_PARAMETER_ONE] = StartSector; //Start Sector Number
	IAP_Command[IAP_CMD_PARAMETER_TWO] = EndSector; //End Sector Number

	IAP_Entry = (IAP) IAP_LOCATION;
	IAP_Entry(IAP_Command, IAP_Result);

    while (1)
    {
       Temp = IAP_Result[IAP_CMD_STATUS_CODE];

       if (Temp == IAP_CMD_SUCCESS)
       {
          Temp = TRUE;
		  break;
       }
       else if (Temp == IAP_BUSY)
       {
          continue;
       }
	   else if (Temp == IAP_INVALID_SECTOR)
	   {
	       FLASH_Printf("FLASH_PrepareSectorsWrite Fail", Temp, Temp);
		   Temp = FALSE;
		   break;
	   }
    }	
	return (Temp);
}



unsigned int FLASH_EraseSector (unsigned int StartSector, unsigned int EndSector)
{
    unsigned int Temp;
	
	IAP_Result[IAP_CMD_STATUS_CODE] = 0xFF;
	IAP_Result[IAP_CMD_RESULT_ONE] = 0xFF;

	IAP_Command[IAP_CMD_CODE] = 52;
	IAP_Command[IAP_CMD_PARAMETER_ONE] = StartSector; //Start Sector Number.
	IAP_Command[IAP_CMD_PARAMETER_TWO] = EndSector; //End Sector Number (should be greater than or equal to start sector number)
	IAP_Command[IAP_CMD_PARAMETER_THREE]= IAP_CCLK / 4; //System Clock Frequency (CCLK) in KHz.

	IAP_Entry = (IAP) IAP_LOCATION;
	IAP_Entry (IAP_Command, IAP_Result);

    while (1)
    {
       Temp = IAP_Result[IAP_CMD_STATUS_CODE];

       if (Temp == IAP_CMD_SUCCESS)
       {
          Temp = TRUE;
		  break;
       }
       else if (Temp == IAP_BUSY)
       {
          continue;
       }
	   else
	   {
	       FLASH_Printf("FLASH_EraseSector Fail", Temp, Temp);
		   Temp = FALSE;
		   break;
	   }
    }	
    return (Temp);
}


/************************************************************************************************
Command: Copy RAM to Flash

Input:
Command code: 51
Param0(DST): Destination Flash address where data bytes are to be written. The destination
address should be a 512 byte boundary.
Param1(SRC): Source RAM address from which data bytes are to be read. This address should be
on word boundary.
Param2: Number of bytes to be written. Should be 512 | 1024 | 4096 | 8192.
Param3: System Clock Frequency (CCLK) in KHz.

Status Code:
CMD_SUCCESS |
SRC_ADDR_ERROR (Address not on word boundary) |
DST_ADDR_ERROR (Address not on correct boundary) |
SRC_ADDR_NOT_MAPPED |
DST_ADDR_NOT_MAPPED |
COUNT_ERROR (Byte count is not 512 | 1024 | 4096 | 8192) |
SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION |
BUSY

Result: None

Description:
This command is used to program the flash memory. The affected sectors should be prepared first
by calling "Prepare Sector for Write Operation" command. The affected sectors are automatically
protected again once the copy command is successfully executed. The boot sector can not be
written by this command.

************************************************************************************************/
unsigned int FLASH_CopyRamToFlash (unsigned int FlashDestAddress, unsigned int RAMSourceAddress, unsigned int Size)
{
    unsigned int Temp;

	IAP_Result[IAP_CMD_STATUS_CODE] = 0xFF;
	IAP_Result[IAP_CMD_RESULT_ONE] = 0xFF;
	
	IAP_Command[IAP_CMD_CODE] = 51;
	IAP_Command[IAP_CMD_PARAMETER_ONE] = FlashDestAddress; //Destination Flash address where data bytes are to be written.
	IAP_Command[IAP_CMD_PARAMETER_TWO] = RAMSourceAddress; //Source RAM address from which data bytes are to be read.
	IAP_Command[IAP_CMD_PARAMETER_THREE] = Size; //Number of bytes to be written. Should be 512 | 1024 | 4096 | 8192.
	IAP_Command[IAP_CMD_PARAMETER_FOUR] = IAP_CCLK / 4; //System Clock Frequency (CCLK) in KHz.

	IAP_Entry = (IAP) IAP_LOCATION;
	IAP_Entry (IAP_Command, IAP_Result);

    while (1)
    {
       Temp = IAP_Result[IAP_CMD_STATUS_CODE];

	   if (Temp == IAP_CMD_SUCCESS)
	   {
          Temp = TRUE;
		  break;
	   }
       else if (Temp == IAP_BUSY)
       {
          continue;
       }
	   else
	   {
	       FLASH_Printf("FLASH_CopyRamToFlash Fail", Temp, Temp);
		   Temp = FALSE;
		   break;
	   }
    }	
    return (Temp);
}

void FLASH_GetSectorAddress (unsigned int ulSectorNumber, unsigned int * ulSectorAddress, unsigned int * ulSectorSize)
{
	switch (ulSectorNumber)
	{
		case 0:
			*ulSectorAddress = 0x00000000;
			*ulSectorSize = 4 * 1024; //4KB
			break;	
		case 1:
			*ulSectorAddress = 0x00001000;
			*ulSectorSize = 4 * 1024; //4KB
			break;	
		case 2:
			*ulSectorAddress = 0x00002000;
			*ulSectorSize = 4 * 1024; //4KB
			break;	
		case 3:
			*ulSectorAddress = 0x00003000;
			*ulSectorSize = 4 * 1024; //4KB
			break;	
		case 4:
			*ulSectorAddress = 0x00004000;
			*ulSectorSize = 4 * 1024; //4KB
			break;	
		case 5:
			*ulSectorAddress = 0x00005000;
			*ulSectorSize = 4 * 1024; //4KB
			break;	
		case 6:
			*ulSectorAddress = 0x00006000;
			*ulSectorSize = 4 * 1024; //4KB
			break;	
		case 7:
			*ulSectorAddress = 0x00007000;
			*ulSectorSize = 4 * 1024; //4KB
			break;	
		case 8:
			*ulSectorAddress = 0x00008000;
			*ulSectorSize = 32 * 1024; //32kb
			break;	
		case 9:
			*ulSectorAddress = 0x00010000;
			*ulSectorSize = 32 * 1024; //32KB
			break;	
		case 10:
			*ulSectorAddress = 0x00018000;
			*ulSectorSize = 32 * 1024; //32KB
			break;	
		case 11:
			*ulSectorAddress = 0x00020000;
			*ulSectorSize = 32 * 1024; //32KB
			break;	
		case 12:
			*ulSectorAddress = 0x00028000;
			*ulSectorSize = 32 * 1024; //32KB
			break;	
		case 13:
			*ulSectorAddress = 0x00030000;
			*ulSectorSize = 32 * 1024; //32KB
			break;	
		case 14:
			*ulSectorAddress = 0x000380000;
			*ulSectorSize = 32 * 1024; //32KB
			break;	
		case 15:
			*ulSectorAddress = 0x00040000;
			*ulSectorSize = 32 * 1024; //32KB
			break;	
		case 16:
			*ulSectorAddress = 0x00048000;
			*ulSectorSize = 32 * 1024; //32KB
			break;	
		case 17:
			*ulSectorAddress = 0x00050000;
			*ulSectorSize = 32 * 1024; //32KB
			break;	
		case 18:
			*ulSectorAddress = 0x00058000;
			*ulSectorSize = 32 * 1024; //32KB
			break;	
		case 19:
			*ulSectorAddress = 0x00060000;
			*ulSectorSize = 32 * 1024; //32KB
			break;	
		case 20:
			*ulSectorAddress = 0x00068000;
			*ulSectorSize = 32 * 1024; //32KB
			break;	
		case 21:
			*ulSectorAddress = 0x00070000;
			*ulSectorSize = 32 * 1024; //32KB
			break;	
		case 22:
			*ulSectorAddress = 0x00078000;
			*ulSectorSize = 4 * 1024; //4KB
			break;	
		case 23:
			*ulSectorAddress = 0x00079000;
			*ulSectorSize = 4 * 1024; //4KB
			break;	
		case 24:
			*ulSectorAddress = 0x0007A000;
			*ulSectorSize = 4 * 1024; //4KB
			break;	
		case 25:
			*ulSectorAddress = 0x0007B000;
			*ulSectorSize = 4 * 1024; //4KB
			break;	
		case 26:
			*ulSectorAddress = 0x0007C000;
			*ulSectorSize = 4 * 1024; //4KB
			break;	
		default:
			*ulSectorAddress = 0xFFFFFFFF;
			*ulSectorSize = 0xFFFFFFFF;
			break;	
	}
}

unsigned int FLASH_WriteSector (unsigned int ulSectorNumber, unsigned int ulRamDataAddress, unsigned int ulSize)
{
    unsigned int ulResult, ulSectorAddress, ulSectorSize, ulIndex;

	FLASH_GetSectorAddress (ulSectorNumber, &ulSectorAddress, &ulSectorSize);
	if ((ulSectorAddress == 0xFFFFFFFF) || (ulSectorSize == 0xFFFFFFFF))
	{
		FLASH_Printf("Invalid Sector Address or Size", ulSectorAddress, ulSectorSize); 
		return FALSE;
	}
	if (ulSize > ulSectorSize)
	{
		FLASH_Printf("Required size is greater than Sector Size", ulSectorAddress, ulSectorSize); 
		return FALSE;
	}

	ulResult = FLASH_PrepareSectorsWrite(ulSectorNumber, ulSectorNumber);
	if (!ulResult)
	{
		FLASH_Printf("FLASH_PrepareSectorsWrite Fail", ulResult, ulSectorNumber); 
		return FALSE;
	}
	FLASH_Printf("FLASH_PrepareSectorsWrite OK", ulResult, ulSectorNumber);
	
	ulResult = FLASH_EraseSector(ulSectorNumber, ulSectorNumber);
	if (!ulResult)
	{
		FLASH_Printf("FLASH_EraseSector Fail", ulResult, ulSectorNumber); 
		return FALSE;
	}
	FLASH_Printf("FLASH_EraseSector OK", ulResult, ulSectorNumber);

	while (ulSize)
	{
		ulResult = FLASH_PrepareSectorsWrite(ulSectorNumber, ulSectorNumber);
		if (!ulResult)
		{
			FLASH_Printf("FLASH_PrepareSectorsWrite Fail", ulResult, ulSectorNumber);
			return FALSE;
		}
		FLASH_Printf("FLASH_PrepareSectorsWrite OK", ulSectorNumber, RAM_TO_FLASH_COPY_SIZE);

		#if 1
		if (ulSize < RAM_TO_FLASH_COPY_SIZE)
		{
			Flash_Memset ((unsigned char *)(ulRamDataAddress + ulSize), (unsigned char)(0xA5), (unsigned int)(RAM_TO_FLASH_COPY_SIZE - ulSize));
			ulSize = RAM_TO_FLASH_COPY_SIZE;
		}
		#endif

		FLASH_Printf("FLASH_CopyRamToFlash Start > ", ulSectorAddress, ulRamDataAddress);					
		ulResult = FLASH_CopyRamToFlash(ulSectorAddress, ulRamDataAddress, RAM_TO_FLASH_COPY_SIZE);
		//ulResult = TRUE;
		FLASH_Printf("FLASH_CopyRamToFlash End > ", ulResult, ulRamDataAddress);		
		if (!ulResult)
		{
			FLASH_Printf("FLASH_CopyRamToFlash Fail", ulResult, ulSectorNumber);
			return FALSE;
		}
		ulRamDataAddress += RAM_TO_FLASH_COPY_SIZE;
		ulSectorAddress += RAM_TO_FLASH_COPY_SIZE;
		ulSize -= RAM_TO_FLASH_COPY_SIZE;
	}

	FLASH_Printf("FLASH_CopyRamToFlash OK", ulResult, ulSectorNumber);
	return TRUE;	
}



