#ifdef FLASH_MAIN_C
#define EXTERN 
#else
#define EXTERN extern
#endif


#ifndef FLASH_MAIN_H
#define FLASH_MAIN_H

EXTERN unsigned int FLASH_WriteFirmware (unsigned int ulSourceRamAddress, unsigned int ulFlashDestAddress, unsigned int ulFirmwareSize);
EXTERN void FLASH_WriteTest (void);
EXTERN void FLASH_Printf (char *pubString, unsigned int ulData1, unsigned int ulData2);

#undef EXTERN
#endif