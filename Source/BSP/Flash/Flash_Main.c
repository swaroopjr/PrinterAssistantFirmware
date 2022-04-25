//Flash Code

#define FLASH_MAIN_C

#include "Global_Enums.h"
#include "Flash_Main.h"
#include "Flash_Drv.h"
#include "Printf.h"

void FLASH_GetSectorNumber (unsigned int ulFlashAddress, unsigned int * pulSectorNumber, unsigned int * pulSectorSize)
{
	switch (ulFlashAddress)
	{
	
		case 0x00000000:
			*pulSectorNumber = 0xFFFFFFFF; //Block Vectortable
			*pulSectorSize = 4 * 1024; //4KB
			break;
		case 0x00001000:
			*pulSectorNumber = 1;
			*pulSectorSize = 4 * 1024; //4KB
			break;
		case 0x00002000:
			*pulSectorNumber = 2;
			*pulSectorSize = 4 * 1024; //4KB
			break;
		case 0x00003000:
			*pulSectorNumber = 3;
			*pulSectorSize = 4 * 1024; //4KB
			break;
		case 0x00004000:
			*pulSectorNumber = 4;
			*pulSectorSize = 4 * 1024; //4KB
			break;
		case 0x00005000:
			*pulSectorNumber = 5;
			*pulSectorSize = 4 * 1024; //4KB
			break;
		case 0x00006000:
			*pulSectorNumber = 6;
			*pulSectorSize = 4 * 1024; //4KB
			break;
		case 0x00007000:
			*pulSectorNumber = 7;
			*pulSectorSize = 4 * 1024; //4KB
			break;
		case 0x00008000:
			*pulSectorNumber = 8;
			*pulSectorSize = 32 * 1024; //32KB
			break;
		case 0x00010000:
			*pulSectorNumber = 0xFFFFFFFF; //Block boot code
			*pulSectorSize = 32 * 1024; //32KB
			break;
		case 0x00018000:
			*pulSectorNumber = 10;
			*pulSectorSize = 32 * 1024; //32KB
			break;
		case 0x00020000:
			*pulSectorNumber = 11;
			*pulSectorSize = 32 * 1024; //32KB
				break;
		case 0x00028000:
			*pulSectorNumber = 12;
			*pulSectorSize = 32 * 1024; //32KB
			break;
		case 0x00030000:
			*pulSectorNumber = 13;
			*pulSectorSize = 32 * 1024; //32KB
			break;
		case 0x00038000:
			*pulSectorNumber = 14;
			*pulSectorSize = 32 * 1024; //32KB
			break;
		case 0x00040000:
			*pulSectorNumber = 15;
			*pulSectorSize = 32 * 1024; //32KB
			break;
		case 0x00048000:
			*pulSectorNumber = 16;
			*pulSectorSize = 32 * 1024; //32KB
			break;
		case 0x00050000:
			*pulSectorNumber = 17;
			*pulSectorSize = 32 * 1024; //32KB
			break;
		case 0x00058000:
			*pulSectorNumber = 18;
			*pulSectorSize = 32 * 1024; //32KB
			break;
		case 0x00060000:
			*pulSectorNumber = 19;
			*pulSectorSize = 32 * 1024; //32KB
			break;
		case 0x00068000:
			*pulSectorNumber = 20;
			*pulSectorSize = 32 * 1024; //32KB
			break;
		case 0x00070000:
			*pulSectorNumber = 21;
			*pulSectorSize = 32 * 1024; //32KB
			break;
		case 0x00078000:
			*pulSectorNumber = 22;
			*pulSectorSize = 4 * 1024; //4KB
			break;
		case 0x00079000:
			*pulSectorNumber = 23;
			*pulSectorSize = 4 * 1024; //4KB
			break;
		case 0x0007A000:
			*pulSectorNumber = 24;
			*pulSectorSize = 4 * 1024; //4KB
			break;
		case 0x0007B000:
			*pulSectorNumber = 25;
			*pulSectorSize = 4 * 1024; //4KB
			break;
		case 0x0007C000:
			*pulSectorNumber = 26;
			*pulSectorSize = 4 * 1024; //4KB
			break;
		default:
			*pulSectorNumber = 0xFFFFFFFF;
			*pulSectorSize = 0; //0KB
			break;
	}
}


unsigned int FLASH_WriteFirmware (unsigned int ulSourceRamAddress, unsigned int ulFlashDestAddress, unsigned int ulFirmwareSize)
{
	unsigned int ulSectorNumber = 0xFFFFFFFF, ulSectorSize = 0xFFFFFFFF;
	FLASH_GetSectorNumber (ulFlashDestAddress, &ulSectorNumber, &ulSectorSize);
	U0_Printf ("\r\n [FLASH_WriteFirmware] Source Addr[%x], Dest Addr[%x], Sector[%d], Sector size [%d], Firmware Size[%d]", 
		                                   ulSourceRamAddress, ulFlashDestAddress, ulSectorNumber, ulSectorSize, ulFirmwareSize);
	U1_Printf ("\r\n [FLASH_WriteFirmware] Source Addr[%x], Dest Addr[%x], Sector[%d], Sector size [%d], Firmware Size[%d]", 
		                                   ulSourceRamAddress, ulFlashDestAddress, ulSectorNumber, ulSectorSize, ulFirmwareSize);

	if (ulSectorNumber == 0xFFFFFFFF)
	{
		U0_Printf ("\r\n [FLASH_WriteFirmware] Sector Number Error");
		U1_Printf ("\r\n [FLASH_WriteFirmware] Sector Number Error");
		return FALSE;
	}

	if (ulSectorSize < ulFirmwareSize)
	{
		U0_Printf ("\r\n [FLASH_WriteFirmware] Firmware size is greater than sector size");
		U1_Printf ("\r\n [FLASH_WriteFirmware] Firmware size is greater than sector size");
		return FALSE;
	}

	if (FLASH_WriteSector (ulSectorNumber, ulSourceRamAddress, ulFirmwareSize))
	{
		U0_Printf ("\r\n [FLASH_WriteFirmware] Firmware write done");
		U1_Printf ("\r\n [FLASH_WriteFirmware] Firmware write done");
		return TRUE;
	}

	U0_Printf ("\r\n [FLASH_WriteFirmware] Flash Sector %d Write error", ulSectorNumber);
	U1_Printf ("\r\n [FLASH_WriteFirmware] Flash Sector %d Write error", ulSectorNumber);
	return FALSE;
}

void FLASH_WriteTest (void)
{
    unsigned int i;
	unsigned int * RamAddr = 0;

	for (i=0;i<512; i++)
	{
		RamAddr = (unsigned int *)(0x40005000 + i);
		* RamAddr = 0x12345678;
	}
	
	FLASH_WriteSector (5, 0x40005000, 512);
}

void FLASH_Printf (char *pubString, unsigned int ulData1, unsigned int ulData2)
{
	U0_Printf ("\r\n [FLASH DRV] %s [%x][%x]", pubString, ulData1, ulData2);
	U1_Printf ("\r\n [FLASH DRV] %s [%x][%x]", pubString, ulData1, ulData2);
}


