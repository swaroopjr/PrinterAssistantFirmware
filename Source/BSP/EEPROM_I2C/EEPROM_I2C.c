//EEPROM_I2C.c

#define EEPROM_I2C_C

#include "LPC214x.H"
#include "Global_Enums.h"
#include "EEPROM_I2C.h"
#include "I2C_Main.h"
#include "Printf.h"

#define DBG_ERR U1_Printf
#define DBG_PRT U1_Printf
#define EEPROM_I2C_DELAY 200

void EEPROM_I2C_Initialize (void)
{
	I2C_Initialize ();
}

void EEPROM_I2C_WriteData (unsigned int ulSlaveAddress, unsigned int Address, unsigned char * BufferPtr, unsigned int SizeInBytes)
{
	unsigned int i;

	DBG_PRT("\r\n EEP Write : Slave[%d] Address[0x%x] Data", ulSlaveAddress, Address);
	for (i=0; i<SizeInBytes; i++)
	{
		DBG_PRT("[0x%x]", *(BufferPtr + i));
	}

	I2C_SetDelay (EEPROM_I2C_DELAY);

	I2C_StartCondition();

	I2C_Send8BitData ((0xA0) | ((ulSlaveAddress & 0x07) << 1) | (0x00)); //1010-A2-A1-A0-0
	//I2C_WaitForAck();

	I2C_Send8BitData ((Address>>8) & 0xFF);
	//I2C_WaitForAck();

	I2C_Send8BitData (Address & 0xFF);
	//I2C_WaitForAck();	

	for (i=0; i<SizeInBytes; i++)
	{
		I2C_Send8BitData (*(BufferPtr + i));
		//I2C_WaitForAck();
	}

	I2C_StopCondition();
	DBG_PRT(" Done");
}

void EEPROM_I2C_ReadData (unsigned int ulSlaveAddress, unsigned int Address, unsigned char * BufferPtr, unsigned int SizeInBytes)
{
	unsigned int i;

	if (SizeInBytes == 0)
		return;

	I2C_SetDelay (EEPROM_I2C_DELAY);

	I2C_StartCondition();

	I2C_Send8BitData ((0xA0) | ((ulSlaveAddress & 0x07) << 1) | (0x00)); //1010-A2-A1-A0-0
	//I2C_WaitForAck();

	I2C_Send8BitData ((Address>>8) & 0xFF);
	//I2C_WaitForAck();

	I2C_Send8BitData (Address & 0xFF);
	//I2C_WaitForAck();

	//I2C_StopCondition();
	I2C_StartCondition();

	I2C_Send8BitData ((0xA0) | ((ulSlaveAddress & 0x07) << 1) | (0x01)); //1010-A2-A1-A0-1
	//I2C_WaitForAck();

	for (i=0; i<SizeInBytes-1; i++)
	{
		*(BufferPtr + i) = I2C_Read8BitData ();
		//I2C_SendAck(0);
	}

	*(BufferPtr + i) = I2C_Read8BitData ();
	//I2C_SendAck(1);	

	I2C_StopCondition();

	DBG_PRT("\r\n EEP Read : Slave[%d] Address[0x%x] Data", ulSlaveAddress, Address);
	for (i=0; i<SizeInBytes; i++)
	{
		DBG_PRT("[0x%x]", *(BufferPtr + i));
	}
	DBG_PRT(" Done");
}

void EEPROM_I2C_Test (void)
{
	unsigned char DataBufWr1[4] = {0x37, 0x38, 0x39, 0x3a};
	unsigned char DataBufWr2[4] = {0xab, 0xcd, 0xef, 0xaf};
	unsigned char DataBufRd[4] = {0, 0, 0, 0};
	unsigned char DataBufRd1[8] = {0, 0, 0, 0, 0, 0, 0, 0};

	EEPROM_I2C_Initialize();

	EEPROM_I2C_WriteData (1, 0x03, DataBufWr1, 4);
	EEPROM_I2C_WriteData (1, 0x07, DataBufWr2, 4);

	EEPROM_I2C_ReadData (1, 0x07, DataBufRd, 4);
	EEPROM_I2C_ReadData (1, 0x03, DataBufRd, 4);
	EEPROM_I2C_ReadData (1, 0x05, DataBufRd, 4);
	EEPROM_I2C_ReadData (1, 0x03, DataBufRd1, 8);
}

