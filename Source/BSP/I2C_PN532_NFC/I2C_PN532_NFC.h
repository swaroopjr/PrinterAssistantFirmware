#ifdef I2C_PN532_NFC_C
	#define EXTERN 
#else
	#define EXTERN extern
#endif	

#ifndef I2C_PN532_NFC_H
#define I2C_PN532_NFC_H

#define PN532_I2C_SLAVE_ADDRESS       (0x48)

#define PN532_PREAMBLE                (0x00)
#define PN532_STARTCODE1              (0x00)
#define PN532_STARTCODE2              (0xFF)
#define PN532_POSTAMBLE               (0x00)
#define PN532_ACKCODE1                (0x00)
#define PN532_ACKCODE2                (0xFF)

#define PN532_HOSTTOPN532             (0xD4)
#define PN532_PN532TOHOST             (0xD5)

#define PN532_ACK_WAIT_TIME           (10)  // ms, timeout of waiting for ACK

#define PN532_OK                      (0)
#define PN532_INVALID_ACK             (-1)
#define PN532_TIMEOUT                 (-2)
#define PN532_INVALID_FRAME           (-3)
#define PN532_NO_MEMORY               (-4)
#define PN532_INVALID_CHECKSUM        (-5)
#define PN532_COMMUNICATION_ERROR     (-6)

// PN532 Commands
#define PN532_COMMAND_DIAGNOSE              (0x00)
#define PN532_COMMAND_GETFIRMWAREVERSION    (0x02)
#define PN532_COMMAND_GETGENERALSTATUS      (0x04)
#define PN532_COMMAND_READREGISTER          (0x06)
#define PN532_COMMAND_WRITEREGISTER         (0x08)
#define PN532_COMMAND_READGPIO              (0x0C)
#define PN532_COMMAND_WRITEGPIO             (0x0E)
#define PN532_COMMAND_SETSERIALBAUDRATE     (0x10)
#define PN532_COMMAND_SETPARAMETERS         (0x12)
#define PN532_COMMAND_SAMCONFIGURATION      (0x14)
#define PN532_COMMAND_POWERDOWN             (0x16)
#define PN532_COMMAND_RFCONFIGURATION       (0x32)
#define PN532_COMMAND_RFREGULATIONTEST      (0x58)
#define PN532_COMMAND_INJUMPFORDEP          (0x56)
#define PN532_COMMAND_INJUMPFORPSL          (0x46)
#define PN532_COMMAND_INLISTPASSIVETARGET   (0x4A)
#define PN532_COMMAND_INATR                 (0x50)
#define PN532_COMMAND_INPSL                 (0x4E)
#define PN532_COMMAND_INDATAEXCHANGE        (0x40)
#define PN532_COMMAND_INCOMMUNICATETHRU     (0x42)
#define PN532_COMMAND_INDESELECT            (0x44)
#define PN532_COMMAND_INRELEASE             (0x52)
#define PN532_COMMAND_INSELECT              (0x54)
#define PN532_COMMAND_INAUTOPOLL            (0x60)
#define PN532_COMMAND_TGINITASTARGET        (0x8C)
#define PN532_COMMAND_TGSETGENERALBYTES     (0x92)
#define PN532_COMMAND_TGGETDATA             (0x86)
#define PN532_COMMAND_TGSETDATA             (0x8E)
#define PN532_COMMAND_TGSETMETADATA         (0x94)
#define PN532_COMMAND_TGGETINITIATORCOMMAND (0x88)
#define PN532_COMMAND_TGRESPONSETOINITIATOR (0x90)
#define PN532_COMMAND_TGGETTARGETSTATUS     (0x8A)
#define PN532_RESPONSE_INDATAEXCHANGE       (0x41)
#define PN532_RESPONSE_INLISTPASSIVETARGET  (0x4B)

// Mifare Commands
#define MIFARE_CMD_AUTH_A                   (0x60)
#define MIFARE_CMD_AUTH_B                   (0x61)
#define MIFARE_CMD_READ                     (0x30)
#define MIFARE_CMD_WRITE                    (0xA0)
#define MIFARE_CMD_WRITE_ULTRALIGHT         (0xA2)
#define MIFARE_CMD_TRANSFER                 (0xB0)
#define MIFARE_CMD_DECREMENT                (0xC0)
#define MIFARE_CMD_INCREMENT                (0xC1)
#define MIFARE_CMD_STORE                    (0xC2)

EXTERN void PN532_NFC_Initialize (void);
EXTERN unsigned int PN532_NFC_CheckI2cConnection (void);
EXTERN signed int PN532_NFC_SendCommandFrame (unsigned char *pubCommandPtr, unsigned int ulLength);
EXTERN signed int PN532_NFC_ReceiveResposeFrame(unsigned char *pubReceiveBufPtr, unsigned int ulReceiveBufSize, unsigned int * pulReceiveActualSize);
EXTERN unsigned int PN532_NFC_ReadAckFrame (void);
EXTERN void PN532_NFC_GetFirmwareVersion (void);
EXTERN void PN532_NFC_SAMConfig (void);
EXTERN unsigned int PN532_NFC_ReadPassiveTargetID (void);
EXTERN unsigned int PN532_NFC_AuthenticateBlock (unsigned int ulBlockNumber);
EXTERN unsigned int PN532_NFC_ReadDataBlock (unsigned int ulBlockNumber, unsigned char *pubData);
EXTERN unsigned int PN532_NFC_WriteDataBlock (unsigned int ulBlockNumber, unsigned char *pubData);
EXTERN void PN532_NFC_Test (void);

#undef EXTERN
#endif
