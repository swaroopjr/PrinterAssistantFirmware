#ifdef EEPROM_I2C_C
	#define EXTERN 
#else
	#define EXTERN extern
#endif	

#ifndef EEPROM_I2C_H
#define EEPROM_I2C_H	

EXTERN void EEPROM_I2C_Initialize (void);
EXTERN void EEPROM_I2C_WriteData (unsigned int ulSlaveAddress, unsigned int Address, unsigned char * BufferPtr, unsigned int SizeInBytes);
EXTERN void EEPROM_I2C_ReadData (unsigned int ulSlaveAddress, unsigned int Address, unsigned char * BufferPtr, unsigned int SizeInBytes);
EXTERN void EEPROM_I2C_Test (void);

#undef EXTERN
#endif
