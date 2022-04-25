//I2C_Main.c
#define I2C_MAIN_C

#include "LPC214x.H"
#include "Global_Enums.h"
#include "I2C_Main.h"
#include "Printf.h"

#define DBG_ERR U1_Printf
#define DBG_PRT U1_Printf

/*********************** Functions to control SCL and SDA pins ***********************************/
//LPC 2148
//P0.02 - SCL
//P0.03 - SDA
#define I2C_SDA_PIN (0x01<<3)
#define I2C_SCL_PIN (0x01<<2)
#define I2C_DEFAULT_DELAY 500
static unsigned int gul_I2C_Delay = I2C_DEFAULT_DELAY;

static void I2C_Make_SDA_SCL_GPIOPins (void)
{
	PINSEL0 &= 0xFFFFFF0F; // 4th, 5th, 6th and 7th bits to be made 0	
}

static void I2C_Make_SCL_OutPin (void)
{
	IODIR0 |= (I2C_SCL_PIN | I2C_SDA_PIN); //2nd and 3rd bits to be made 1
}

static void I2C_Make_SDA_OutPin (void)
{
	IODIR0 |= I2C_SDA_PIN;
}

static void I2C_Make_SDA_InPin (void)
{
	IODIR0 &= (~I2C_SDA_PIN);
}

static void I2C_Make_SDA_High (void)
{
	IOSET0 = I2C_SDA_PIN;
}

static void I2C_Make_SDA_Low (void)
{
	IOCLR0 = I2C_SDA_PIN;
}

static void I2C_Make_SCL_High (void)
{
	IOSET0 = I2C_SCL_PIN;
}

static void I2C_Make_SCL_Low (void)
{
	IOCLR0 = I2C_SCL_PIN;
}

static unsigned int I2C_Get_SDA_State (void)
{
	if ((IOPIN0) & I2C_SDA_PIN)
		return (1);
	else
		return (0);
}

/*********************** I2C protocol functions ***********************************/
void I2C_SetDelay (unsigned long int ulValue)
{
	gul_I2C_Delay = ulValue;
}

void I2C_Delay(unsigned long int ulValue)
{
    while(ulValue > 0) 
    {
        ulValue--;	// Loop Decrease Counter	
    }
}

void I2C_StartCondition (void)
{

	I2C_Make_SDA_High ();
	I2C_Make_SCL_High ();
	I2C_Delay (gul_I2C_Delay);

	I2C_Make_SDA_Low ();
	I2C_Delay (gul_I2C_Delay);
	I2C_Make_SCL_Low ();
	I2C_Delay (gul_I2C_Delay);
}

void I2C_StopCondition (void)
{
	I2C_Make_SDA_Low ();
	I2C_Delay (gul_I2C_Delay);
	I2C_Make_SCL_High();
	I2C_Delay (gul_I2C_Delay);
	I2C_Make_SDA_High ();
	I2C_Delay (gul_I2C_Delay);
}

/************************************************************* 
   Returns TRUE if 8 bit data was sent via I2C channel and ack was received sucuessfully
   Returns FALSE if 8 bit data was sent via I2C channel and ack was NOT received
*************************************************************/
unsigned int I2C_Send8BitData (unsigned char Data)
{
	unsigned int i;
	unsigned int u32_I2C_Ack;

	for (i=0; i<8; i++)
	{
		if (Data & (0x01 << (7-i)))
			I2C_Make_SDA_High();
		else
			I2C_Make_SDA_Low();

		I2C_Delay (gul_I2C_Delay);
		I2C_Make_SCL_High ();
		I2C_Delay (gul_I2C_Delay);
		I2C_Make_SCL_Low ();
		I2C_Delay (gul_I2C_Delay);
	}

	I2C_Make_SDA_Low();

	u32_I2C_Ack = I2C_WaitForAck();
	if (u32_I2C_Ack == FALSE)
	{
		DBG_ERR("\r\n I2C_Send8BitData Err: Ack not received");
	}

	return u32_I2C_Ack;
}

/************************************************************* 
   Returns TRUE if ack was received sucuessfully
   Returns FALSE if ack was NOT received
*************************************************************/
unsigned int I2C_WaitForAck (void)
{
	unsigned int Ack_Wait_Count = gul_I2C_Delay;
	unsigned int DataSendResult = TRUE;

	I2C_Make_SDA_InPin();
	I2C_Make_SCL_High ();
	I2C_Delay (gul_I2C_Delay);

	while (I2C_Get_SDA_State())
	{
		Ack_Wait_Count --;
		if (!Ack_Wait_Count)
		{
			DataSendResult = FALSE;
			break;
		}
	}

	I2C_Make_SCL_Low ();	
	I2C_Delay (gul_I2C_Delay);
	I2C_Make_SDA_OutPin();
	return (DataSendResult);
}

void I2C_SendAck (unsigned int ulAck)
{
	if (ulAck)
		I2C_Make_SDA_High ();
	else
		I2C_Make_SDA_Low ();

	I2C_Delay (gul_I2C_Delay);
	I2C_Make_SCL_High ();
	I2C_Delay (gul_I2C_Delay);
	I2C_Make_SCL_Low ();	
	I2C_Delay (gul_I2C_Delay);
}

unsigned char I2C_Read8BitData (void)
{
	unsigned int i;
	unsigned char ReturnValue = 0;

	I2C_Make_SDA_InPin();

	for (i=0; i<8; i++)
	{
		I2C_Make_SCL_High ();
		I2C_Delay (gul_I2C_Delay);

		if (I2C_Get_SDA_State())
			ReturnValue |= (0x01 << (7-i));
		
		I2C_Make_SCL_Low ();
		I2C_Delay (gul_I2C_Delay);
	}

	I2C_Make_SDA_OutPin();
	I2C_SendAck(0);
	
	return (ReturnValue);
}

unsigned char I2C_Read8BitDataWithAck (void)
{
	unsigned int i;
	unsigned char ReturnValue = 0;

	I2C_Make_SDA_InPin();

	for (i=0; i<8; i++)
	{
		I2C_Make_SCL_High ();
		I2C_Delay (gul_I2C_Delay);

		if (I2C_Get_SDA_State())
			ReturnValue |= (0x01 << (7-i));
		
		I2C_Make_SCL_Low ();
		I2C_Delay (gul_I2C_Delay);
	}

	I2C_Make_SDA_OutPin();
	I2C_SendAck(0);
	
	return (ReturnValue);
}

unsigned char I2C_Read8BitDataWithNack (void)
{
	unsigned int i;
	unsigned char ReturnValue = 0;

	I2C_Make_SDA_InPin();

	for (i=0; i<8; i++)
	{
		I2C_Make_SCL_High ();
		I2C_Delay (gul_I2C_Delay);

		if (I2C_Get_SDA_State())
			ReturnValue |= (0x01 << (7-i));
		
		I2C_Make_SCL_Low ();
		I2C_Delay (gul_I2C_Delay);
	}

	I2C_Make_SDA_OutPin();
	I2C_SendAck(1);
	
	return (ReturnValue);
}

void I2C_Initialize (void)
{
	static unsigned int ulI2CInitialized = 0;

	if (ulI2CInitialized != 1)
	{
		I2C_Make_SDA_SCL_GPIOPins ();
		I2C_Make_SDA_OutPin ();
		I2C_Make_SCL_OutPin ();

		I2C_Make_SCL_High ();
		I2C_Make_SDA_High ();
		I2C_Delay (gul_I2C_Delay);

		ulI2CInitialized = 1;
	}
	else
	{
		DBG_PRT("\r\n I2C already initialized");
	}
}

