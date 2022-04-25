#ifdef I2C_MAIN_C
	#define EXTERN 
#else
	#define EXTERN extern
#endif	

#ifndef I2C_MAIN_H
#define I2C_MAIN_H	

EXTERN void I2C_SetDelay (unsigned long int ulValue);
EXTERN void I2C_Delay(unsigned long int ulValue);
EXTERN void I2C_StartCondition (void);
EXTERN void I2C_StopCondition (void);
EXTERN unsigned int I2C_Send8BitData (unsigned char Data);
EXTERN unsigned int I2C_WaitForAck (void);
EXTERN void I2C_SendAck (unsigned int ulAck);
EXTERN unsigned char I2C_Read8BitData (void);
EXTERN unsigned char I2C_Read8BitDataWithAck (void);
EXTERN unsigned char I2C_Read8BitDataWithNack (void);
EXTERN void I2C_Initialize (void);

#undef EXTERN
#endif
