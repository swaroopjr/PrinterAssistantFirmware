//I2C_PN532_NFC.c

#define I2C_PN532_NFC_C

#include "LPC214x.H"
#include "Global_Enums.h"
#include "I2C_Main.h"
#include "I2C_PN532_NFC.h"
#include "Printf.h"
#include "Global.h"

#define DBG_ERR U1_Printf
#define DBG_PRT_ALWAYS U1_Printf
//#define DBG_PRT U1_Printf
#define DBG_PRT Global_NULL_Printf

#define PN532_NFC_I2C_DELAY 200
#define PN532_NFC_MAX_RETRY 3

static unsigned char aubPassiveTargetID[8];
static unsigned int ulPassiveTargetIDLength;
static unsigned int ulTagNumber;
static unsigned int aulAuthenticationDone[8];

void PN532_NFC_Initialize (void)
{
	//unsigned int i;
	unsigned int ulRetry = PN532_NFC_MAX_RETRY;

	DBG_PRT("\r\n PN532_NFC_Initialize");

	I2C_Initialize ();

	while (ulRetry)
	{
		if (TRUE == PN532_NFC_CheckI2cConnection())
		{
			break;
		}
		else
		{
			ulRetry --;
			DBG_PRT("\r\n PN532 NFC connection retry %d", PN532_NFC_MAX_RETRY - ulRetry);
		}
	}

	if (ulRetry)
	{
		DBG_PRT_ALWAYS("\r\n PN532 NFC Connected");
	}
	else
	{
		DBG_ERR("\r\n Check PN532 NFC Connection");
		return;
	}

	Global_Memset ((unsigned char *)aulAuthenticationDone, 0x0, sizeof (aulAuthenticationDone));

	PN532_NFC_GetFirmwareVersion ();
	PN532_NFC_SAMConfig ();
	//PN532_NFC_Test ();
}

/************************************************************* 
   Returns TRUE if PN532 NFC device is connected
   Returns FALSE if PN532 NFC device is NOT connected  
*************************************************************/
unsigned int PN532_NFC_CheckI2cConnection (void)
{
	unsigned int ul_I2C_Ack = FALSE;

	I2C_SetDelay (PN532_NFC_I2C_DELAY);
    I2C_StartCondition ();
	ul_I2C_Ack = I2C_Send8BitData (PN532_I2C_SLAVE_ADDRESS | 0x00); //0010010-0    
    I2C_StopCondition();

    return ul_I2C_Ack;
}

signed int PN532_NFC_SendCommandFrame (unsigned char *pubCommandPtr, unsigned int ulLength)
{
	unsigned char u8_length, u8_sum, u8_checksum;
	unsigned int i;
	unsigned int ulRetry = PN532_NFC_MAX_RETRY;

	if (ulLength > 32)
	{
		DBG_ERR("\r\n [PN532_NFC_SendCommandFrame] Error: Frame length is too large");
		return PN532_INVALID_FRAME; // I2C max packet: 32 bytes
	}

	I2C_SetDelay (PN532_NFC_I2C_DELAY);

	while (ulRetry)
	{
		if (PN532_NFC_MAX_RETRY != ulRetry)
		{
			DBG_ERR("\r\n PN532 NFC : PN532_NFC_SendCommandFrame : Retry %d", PN532_NFC_MAX_RETRY - ulRetry);
		}
		ulRetry --;

	    I2C_StartCondition ();

		if (FALSE == I2C_Send8BitData (PN532_I2C_SLAVE_ADDRESS | 0x00))		//0010010-0
		{
			DBG_ERR("\r\n PN532 NFC : PN532_NFC_SendCommandFrame : Unable to send slave address write");
			I2C_StopCondition();
			continue; //return PN532_COMMUNICATION_ERROR;
		}

	    if (FALSE == I2C_Send8BitData (PN532_PREAMBLE))
		{
			DBG_ERR("\r\n PN532 NFC : PN532_NFC_SendCommandFrame : Unable to send PREAMBLE");
			I2C_StopCondition();
			continue; //return PN532_COMMUNICATION_ERROR;
		}
		
	    if (FALSE == I2C_Send8BitData (PN532_STARTCODE1))
		{
			DBG_ERR("\r\n PN532 NFC : PN532_NFC_SendCommandFrame : Unable to send STARTCODE1");
			I2C_StopCondition();
			continue; //return PN532_COMMUNICATION_ERROR;
		}
		
	    if (FALSE == I2C_Send8BitData (PN532_STARTCODE2))
		{
			DBG_ERR("\r\n PN532 NFC : PN532_NFC_SendCommandFrame : Unable to send STARTCODE2");
			I2C_StopCondition();
			continue; //return PN532_COMMUNICATION_ERROR;
		}

	    u8_length = ulLength + 1;   // length of data field: TFI + DATA
	    if (FALSE == I2C_Send8BitData(u8_length))
		{
			DBG_ERR("\r\n PN532 NFC : PN532_NFC_SendCommandFrame : Unable to send length");
			I2C_StopCondition();
			return PN532_COMMUNICATION_ERROR;
		}

	    if (FALSE == I2C_Send8BitData(~u8_length + 1)) // checksum of length
		{
			DBG_ERR("\r\n PN532 NFC : PN532_NFC_SendCommandFrame : Unable to send length checksum");
			I2C_StopCondition();
			continue; //return PN532_COMMUNICATION_ERROR;
		}    
	    
	    if (FALSE == I2C_Send8BitData(PN532_HOSTTOPN532))
		{
			DBG_ERR("\r\n PN532 NFC : PN532_NFC_SendCommandFrame : Unable to send HOSTTOPN532");
			I2C_StopCondition();
			continue; //return PN532_COMMUNICATION_ERROR;
		}  
	    u8_sum = PN532_HOSTTOPN532;    // sum of TFI + DATA
	    
	    for (i = 0; i < ulLength; i++) 
		{
			if (FALSE == I2C_Send8BitData(pubCommandPtr[i]))
			{
				DBG_ERR("\r\n PN532 NFC : PN532_NFC_SendCommandFrame : Unable to send data at %d", i);
				I2C_StopCondition();
				return PN532_COMMUNICATION_ERROR;
			} 		
	        u8_sum += pubCommandPtr[i];
	    }

		u8_checksum = ~u8_sum + 1;            // checksum of TFI + DATA
	    if (FALSE == I2C_Send8BitData(u8_checksum))
		{
			DBG_ERR("\r\n PN532 NFC : PN532_NFC_SendCommandFrame : Unable to send packet checksum");
			I2C_StopCondition();
			continue; //return PN532_COMMUNICATION_ERROR;
		} 

	    if (FALSE == I2C_Send8BitData(PN532_POSTAMBLE))
		{
			DBG_ERR("\r\n PN532 NFC : PN532_NFC_SendCommandFrame : Unable to send POSTAMBLE");
			I2C_StopCondition();
			continue; //return PN532_COMMUNICATION_ERROR;
		}

	    I2C_StopCondition();
		break;
	}

	if (ulRetry == 0)
	{
		DBG_ERR("\r\n PN532 NFC : PN532_NFC_SendCommandFrame : Unable to send Command");
		return PN532_COMMUNICATION_ERROR;
	}	

    return PN532_NFC_ReadAckFrame();
}

unsigned int PN532_NFC_ReadAckFrame (void)
{
	unsigned int i;
    unsigned char aubExpectedAck[] = {0, 0, 0xFF, 0, 0xFF, 0};
    unsigned char aubReceivedAck[sizeof(aubExpectedAck)];
	unsigned int ulReadyTime = 150;
	unsigned char ubTemp;

	I2C_SetDelay (PN532_NFC_I2C_DELAY);

	I2C_StartCondition();
	if (FALSE == I2C_Send8BitData (PN532_I2C_SLAVE_ADDRESS | 0x01)) //0010010-1
	{
		DBG_ERR("\r\n PN532 NFC : PN532_NFC_ReadAckFrame : Unable to send slave address read");
		I2C_StopCondition();
		return PN532_COMMUNICATION_ERROR;
	}

	DBG_PRT("\r\n PN532 NFC : PN532_NFC_ReadAckFrame : Wait For Ready");
	while (ulReadyTime)
	{
		ubTemp = I2C_Read8BitData ();
		DBG_PRT("[0x%x]", ubTemp);
		if (0x1 == ubTemp)
			break;

		ulReadyTime --;
		I2C_StopCondition();
		I2C_StartCondition();
		if (FALSE == I2C_Send8BitData (PN532_I2C_SLAVE_ADDRESS | 0x01)) //0010010-1
		{
			DBG_ERR("\r\n PN532 NFC : PN532_NFC_ReadAckFrame : Unable to send slave address");
			I2C_StopCondition();
			return PN532_COMMUNICATION_ERROR;
		}		
	}

	if (ulReadyTime == 0)
	{
		I2C_StopCondition();
		DBG_ERR("\r\n PN532 NFC : PN532_NFC_ReadAckFrame : Timeout");
		I2C_StopCondition();
		return PN532_TIMEOUT;
	}

    for (i = 0; i < sizeof(aubReceivedAck)-1; i++) 
	{
        aubReceivedAck[i] = I2C_Read8BitData ();
    }
	//aubReceivedAck[i] = I2C_Read8BitData();
	aubReceivedAck[i] = I2C_Read8BitDataWithNack();

	I2C_StopCondition();
    
    if (FALSE == Global_Memcmp(aubReceivedAck, aubExpectedAck, sizeof(aubExpectedAck))) 
	{
        DBG_ERR("\r\n PN532 NFC : PN532_NFC_ReadAckFrame : Invalid ACK");
        DBG_ERR("\r\n PN532 NFC : PN532_NFC_ReadAckFrame : aubExpectedAck[0x%x][0x%x][0x%x][0x%x][0x%x][0x%x]", aubExpectedAck[0], aubExpectedAck[1], aubExpectedAck[2], aubExpectedAck[3], aubExpectedAck[4], aubExpectedAck[5]);
        DBG_ERR("\r\n PN532 NFC : PN532_NFC_ReadAckFrame : aubReceivedAck[0x%x][0x%x][0x%x][0x%x][0x%x][0x%x]", aubReceivedAck[0], aubReceivedAck[1], aubReceivedAck[2], aubReceivedAck[3], aubReceivedAck[4], aubReceivedAck[5]);
        return PN532_INVALID_ACK;
    }
    
    return PN532_OK;
}

signed int PN532_NFC_ReceiveResposeFrame (unsigned char *pubReceiveBufPtr, unsigned int ulReceiveBufSize, unsigned int * pulReceiveActualSize)
{
	unsigned int i;
	unsigned int ulReadyTime = 8;
	unsigned char ubResponseLength, ubLengthChecksumReceived, ubLengthChecksumCalculated, ubPacketDataChecksumReceived, ubPacketDataChecksumCalculated, ubPacketDataSum;
	unsigned char ubTemp;

	*pulReceiveActualSize = 0;

	I2C_SetDelay (PN532_NFC_I2C_DELAY);
	I2C_StartCondition();

	if (FALSE == I2C_Send8BitData (PN532_I2C_SLAVE_ADDRESS | 0x01)) //0010010-1
	{
		DBG_ERR("\r\n PN532 NFC : PN532_NFC_ReceiveResposeFrame : Unable to send slave address read");
		I2C_StopCondition();
		return PN532_COMMUNICATION_ERROR;
	}	

	DBG_PRT("\r\n PN532 NFC : PN532_NFC_ReceiveResposeFrame : Wait For Ready");
	while (ulReadyTime)
	{
		ubTemp = I2C_Read8BitData ();
		DBG_PRT("[0x%x]", ubTemp);
		if (0x1 == ubTemp)
			break;

		ulReadyTime --;
		I2C_StopCondition();
		I2C_StartCondition();
		if (FALSE == I2C_Send8BitData (PN532_I2C_SLAVE_ADDRESS | 0x01)) //0010010-1
		{
			DBG_ERR("\r\n PN532 NFC : PN532_NFC_ReceiveResposeFrame : Unable to send slave address read");
			I2C_StopCondition();
			return PN532_COMMUNICATION_ERROR;
		}
	}

	if (ulReadyTime == 0)
	{
		I2C_StopCondition();
		DBG_ERR("\r\n PN532 NFC ERR : PN532_NFC_ReceiveResposeFrame : Ready Timeout");
		return PN532_TIMEOUT;
	}

	ubTemp = I2C_Read8BitData ();
	if (PN532_PREAMBLE != ubTemp)
	{
		I2C_StopCondition();
		DBG_ERR("\r\n PN532 NFC ERR : PN532_NFC_ReceiveResposeFrame : PREAMBLE not received [0x%x]", ubTemp);
		return PN532_INVALID_FRAME;
	}

	ubTemp = I2C_Read8BitData ();	
	if (PN532_STARTCODE1 != ubTemp)
	{
		I2C_StopCondition();
		DBG_ERR("\r\n PN532 NFC ERR : PN532_NFC_ReceiveResposeFrame : STARTCODE1 not received [0x%x]", ubTemp);
		return PN532_INVALID_FRAME;
	}

	ubTemp = I2C_Read8BitData ();	
	if (PN532_STARTCODE2 != ubTemp)
	{
		I2C_StopCondition();
		DBG_ERR("\r\n PN532 NFC ERR : PN532_NFC_ReceiveResposeFrame : STARTCODE2 not received [0x%x]", ubTemp);
		return PN532_INVALID_FRAME;
	}

	ubResponseLength = I2C_Read8BitData ();
	if (ubResponseLength > ulReceiveBufSize)
	{
		I2C_StopCondition();
		DBG_ERR("\r\n PN532 NFC ERR : PN532_NFC_ReceiveResposeFrame : No memory [0x%x]", ubResponseLength);
		return PN532_NO_MEMORY;
	}
	
	ubLengthChecksumReceived = I2C_Read8BitData ();
	ubLengthChecksumCalculated = ~ubResponseLength + 1;
	if (ubLengthChecksumReceived != ubLengthChecksumCalculated)
	{
		I2C_StopCondition();
		DBG_ERR("\r\n PN532 NFC ERR : PN532_NFC_ReceiveResposeFrame : Length checksum error : Length[0x%x] Received[0x%x], Calculated[0x%x]", ubResponseLength, ubLengthChecksumReceived, ubLengthChecksumCalculated);
		return PN532_INVALID_CHECKSUM;
	}

	ubTemp = I2C_Read8BitData ();
	if (PN532_PN532TOHOST != ubTemp)
	{
		I2C_StopCondition();
		DBG_ERR("\r\n PN532 NFC ERR : PN532_NFC_ReceiveResposeFrame : PN532TOHOST not received [0x%x]", ubTemp);
		return PN532_INVALID_FRAME;
	}
	ubPacketDataSum = PN532_PN532TOHOST;

	DBG_PRT("\r\n PN532 NFC : PN532_NFC_ReceiveResposeFrame : Data :- ");
    for (i = 0; i < ubResponseLength-1; i++) //PN532_PN532TOHOST is also included in this length. So -1.
	{
        pubReceiveBufPtr[i] = I2C_Read8BitData ();
		ubPacketDataSum += pubReceiveBufPtr[i];
		DBG_PRT("[0x%x]", pubReceiveBufPtr[i]);
    }

	ubPacketDataChecksumCalculated = ~ubPacketDataSum + 1;
	ubPacketDataChecksumReceived = I2C_Read8BitData ();
	if (ubPacketDataChecksumReceived != ubPacketDataChecksumCalculated)
	{
		I2C_StopCondition();
		DBG_ERR("\r\n PN532 NFC ERR : PN532_NFC_ReceiveResposeFrame : Packet checksum error : Sum[0x%x] Received[0x%x], Calculated[0x%x]", ubPacketDataSum, ubPacketDataChecksumReceived, ubPacketDataChecksumCalculated);
		return PN532_INVALID_CHECKSUM;
	}	

	ubTemp = I2C_Read8BitDataWithNack ();
	if (PN532_POSTAMBLE != ubTemp)
	{
		I2C_StopCondition();
		DBG_ERR("\r\n PN532 NFC ERR : PN532_NFC_ReceiveResposeFrame : PN532_POSTAMBLE not received [0x%x]", ubTemp);		
		return PN532_INVALID_FRAME;
	}

    I2C_StopCondition();
	*pulReceiveActualSize = (unsigned int)(ubResponseLength-1);

    return PN532_OK;
}

signed int PN532_NFC_SendAckFrame (void)
{
	unsigned int ulRetry = PN532_NFC_MAX_RETRY;

	I2C_SetDelay (PN532_NFC_I2C_DELAY);

	while (ulRetry)
	{
		if (ulRetry != PN532_NFC_MAX_RETRY)
		{
			DBG_ERR("\r\n PN532 NFC : PN532_NFC_SendAckFrame : Try %d", PN532_NFC_MAX_RETRY - ulRetry);
		}
		ulRetry --;

	    I2C_StartCondition ();

		if (FALSE == I2C_Send8BitData (PN532_I2C_SLAVE_ADDRESS | 0x00))		//0010010-0
		{
			DBG_ERR("\r\n PN532 NFC : PN532_NFC_SendAckFrame : Unable to send slave address write");
			I2C_StopCondition();
			continue; //return PN532_COMMUNICATION_ERROR;
		}

	    if (FALSE == I2C_Send8BitData (PN532_PREAMBLE))
		{
			DBG_ERR("\r\n PN532 NFC : PN532_NFC_SendAckFrame : Unable to send PREAMBLE");
			I2C_StopCondition();
			continue; //return PN532_COMMUNICATION_ERROR;
		}
		
	    if (FALSE == I2C_Send8BitData (PN532_STARTCODE1))
		{
			DBG_ERR("\r\n PN532 NFC : PN532_NFC_SendAckFrame : Unable to send STARTCODE1");
			I2C_StopCondition();
			continue; //return PN532_COMMUNICATION_ERROR;
		}

	    if (FALSE == I2C_Send8BitData (PN532_STARTCODE2))
		{
			DBG_ERR("\r\n PN532 NFC : PN532_NFC_SendAckFrame : Unable to send STARTCODE2");
			I2C_StopCondition();
			continue; //return PN532_COMMUNICATION_ERROR;
		}

	    if (FALSE == I2C_Send8BitData (PN532_ACKCODE1))
		{
			DBG_ERR("\r\n PN532 NFC : PN532_NFC_SendAckFrame : Unable to send PN532_ACKCODE1");
			I2C_StopCondition();
			continue; //return PN532_COMMUNICATION_ERROR;
		}
		
	    if (FALSE == I2C_Send8BitData (PN532_ACKCODE2))
		{
			DBG_ERR("\r\n PN532 NFC : PN532_NFC_SendAckFrame : Unable to send PN532_ACKCODE2");
			I2C_StopCondition();
			continue; //return PN532_COMMUNICATION_ERROR;
		}

	    if (FALSE == I2C_Send8BitData(PN532_POSTAMBLE))
		{
			DBG_ERR("\r\n PN532 NFC : PN532_NFC_SendAckFrame : Unable to send POSTAMBLE");
			I2C_StopCondition();
			continue; //return PN532_COMMUNICATION_ERROR;
		}

	    I2C_StopCondition();
		break;
	}

	if (ulRetry == 0)
	{
		DBG_ERR("\r\n PN532 NFC : PN532_NFC_SendCommandFrame : Unable to ACK frame");
		return PN532_COMMUNICATION_ERROR;
	}	
	return PN532_OK;
}


void PN532_NFC_GetFirmwareVersion (void)
{
	unsigned char aubSendPacketBuffer[4];
	unsigned char aubReceivePacketBuffer[6];
	unsigned int ulReceivedDataLength;
	unsigned int i;

	DBG_PRT("\r\n [PN532_NFC_GetFirmwareVersion]");
 
    aubSendPacketBuffer[0] = PN532_COMMAND_GETFIRMWAREVERSION;
	if (PN532_NFC_SendCommandFrame (aubSendPacketBuffer, 1))
	{
		DBG_ERR("\r\n [PN532_NFC_GetFirmwareVersion] : Unable to send command frame");
		return;
	}

	if (PN532_NFC_ReceiveResposeFrame (aubReceivePacketBuffer, sizeof(aubReceivePacketBuffer), &ulReceivedDataLength))
	{
		DBG_ERR("\r\n [PN532_NFC_GetFirmwareVersion] : Unable to receive response frame");
		return;
	}

	PN532_NFC_SendAckFrame (); //Yet to handle NACK packet is validation fails

	DBG_PRT_ALWAYS("\r\n PN532 NFC firmware version: ");
	for (i=0; i<ulReceivedDataLength; i++)
	{
		DBG_PRT_ALWAYS("[0x%x]", aubReceivePacketBuffer[i]);
	}
}

/**************************************************************************
    @brief  Configures the SAM (Secure Access Module)
**************************************************************************/
void PN532_NFC_SAMConfig (void)
{
	unsigned char aubSendPacketBuffer[4];
	unsigned char aubReceivePacketBuffer[6];
	unsigned int ulReceivedDataLength;
	unsigned int i;

	DBG_PRT("\r\n [PN532_NFC_SAMConfig]");

    aubSendPacketBuffer[0] = PN532_COMMAND_SAMCONFIGURATION;
    aubSendPacketBuffer[1] = 0x01; // normal mode;
    aubSendPacketBuffer[2] = 0x14; // timeout 50ms * 20 = 1 second
    //aubSendPacketBuffer[2] = 0xC8; // timeout 50ms * 200 = 10 seconds
    aubSendPacketBuffer[3] = 0x01; // use IRQ pin!

	if (PN532_NFC_SendCommandFrame (aubSendPacketBuffer, 4))
	{
		DBG_ERR("\r\n [PN532_NFC_SAMConfig] : Unable to send command frame");
		return;
	}

	if (PN532_NFC_ReceiveResposeFrame (aubReceivePacketBuffer, sizeof(aubReceivePacketBuffer), &ulReceivedDataLength))
	{
		DBG_ERR("\r\n [PN532_NFC_SAMConfig] : Unable to receive response frame");
		return;
	}

	// Validate response data packet
	if ((ulReceivedDataLength != 1) && (aubReceivePacketBuffer[0] != 0x15)) // From adafruit code
	{
		DBG_ERR("\r\n [PN532_NFC_SAMConfig] : incorrect response length or respose code");
		return;
	}

	PN532_NFC_SendAckFrame (); //Yet to handle NACK packet is validation fails

	DBG_PRT("\r\n PN532 NFC SAM Config: ");
	for (i=0; i<ulReceivedDataLength; i++)
	{
		DBG_PRT("[0x%x]", aubReceivePacketBuffer[i]);
	}
}

unsigned int PN532_NFC_ReadPassiveTargetID (void)
{
	unsigned char aubSendPacketBuffer[4];
	unsigned char aubReceivePacketBuffer[32];
	unsigned int ulReceivedDataLength;
	unsigned int i;
	unsigned int ulIdLength;

	DBG_PRT("\r\n [PN532_NFC_ReadPassiveTargetID]");

    aubSendPacketBuffer[0] = PN532_COMMAND_INLISTPASSIVETARGET;
    aubSendPacketBuffer[1] = 1;  // max 1 cards at once (we can set this to 2 later)
    aubSendPacketBuffer[2] = 0;  //cardbaudrate;

	if (PN532_NFC_SendCommandFrame (aubSendPacketBuffer, 3))
	{
		DBG_ERR("\r\n [PN532_NFC_ReadPassiveTargetID] : Unable to send command frame");
		return FALSE;
	}

    // read data packet
	if (PN532_NFC_ReceiveResposeFrame (aubReceivePacketBuffer, sizeof(aubReceivePacketBuffer), &ulReceivedDataLength))
	{
		DBG_ERR("\r\n [PN532_NFC_ReadPassiveTargetID] : Unable to receive response frame");
		return FALSE;
	}

	DBG_PRT("\r\n PN532 Read Passive Target ID: ");
	for (i=0; i<ulReceivedDataLength; i++)
	{
		DBG_PRT("[0x%x]", aubReceivePacketBuffer[i]);
	}

	// Validate response data packet
    /* ISO14443A card response should be in the following format: Application notes Page-34

      byte            Description
      -------------   ------------------------------------------
      b0              Response code
      b1              Tags Found
      b2              Tag Number (only one used in this example)
      b3,4            SENS_RES
      b5              SEL_RES
      b6              NFCID Length
      b7..NFCIDLen    NFCID
    */
    if (aubReceivePacketBuffer[0] != PN532_RESPONSE_INLISTPASSIVETARGET)
	{
		DBG_ERR("\r\n [PN532_NFC_ReadPassiveTargetID][%d] Response code incorrect", aubReceivePacketBuffer[0]);
		return FALSE;
	}
	if (aubReceivePacketBuffer[1] != 1)
    {
    	DBG_ERR("\r\n [PN532_NFC_ReadPassiveTargetID][%d] Only one target supported as of now. Need to add more buffers for more targets", aubReceivePacketBuffer[1]);
        return FALSE;
    }
	ulTagNumber = aubReceivePacketBuffer[2];

	PN532_NFC_SendAckFrame (); //Yet to handle NACK packet is validation fails

    /* Card appears to be Mifare Classic */
    ulIdLength = aubReceivePacketBuffer[6];
	DBG_PRT_ALWAYS("\r\n Target ID: ");
    for (i = 0; i < ulIdLength; i++) 
	{
        DBG_PRT_ALWAYS("[0x%x]", aubReceivePacketBuffer[7+i]);
    }

	ulPassiveTargetIDLength = ulIdLength;
	Global_Memcpy(aubPassiveTargetID, aubReceivePacketBuffer+7, ulIdLength);

    return TRUE;
}

/**************************************************************************
    Tries to authenticate a block of memory on a MIFARE card using the
    INDATAEXCHANGE command.  See section 7.3.8 of the PN532 User Manual
    for more information on sending MIFARE and other commands.

    @param  uid           Pointer to a byte array containing the card UID
    @param  uidLen        The length (in bytes) of the card's UID (Should
                          be 4 for MIFARE Classic)
    @param  blockNumber   The block number to authenticate.  (0..63 for
                          1KB cards, and 0..255 for 4KB cards).
    @param  keyNumber     Which key type to use during authentication
                          (0 = MIFARE_CMD_AUTH_A, 1 = MIFARE_CMD_AUTH_B)
    @param  keyData       Pointer to a byte array containing the 6 byte
                          key value
    
    @returns 1 if everything executed properly, 0 for an error

**************************************************************************/
unsigned int PN532_NFC_AuthenticateBlock (unsigned int ulBlockNumber)
{
	unsigned char aubSendPacketBuffer[16];
	unsigned char aubReceivePacketBuffer[32];
	unsigned int ulReceivedDataLength;
	//unsigned int i;
	unsigned char aubKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

	DBG_PRT("\r\n [PN532_NFC_AuthenticateBlock][%d]", ulBlockNumber);

	/*if (aulAuthenticationDone[ulBlockNumber/sizeof(aulAuthenticationDone[0])] & (0x01 << (ulBlockNumber % sizeof(aulAuthenticationDone[0]))))
	{
		DBG_PRT_ALWAYS ("\r\n [PN532_NFC_AuthenticateBlock] %d block is already authenticated", ulBlockNumber);
		return TRUE;
	}*/

	// Prepare the authentication command //
	aubSendPacketBuffer[0] = PN532_COMMAND_INDATAEXCHANGE;   /* Command Code */
	aubSendPacketBuffer[1] = ulTagNumber;      /* Target number / index */
	aubSendPacketBuffer[2] = MIFARE_CMD_AUTH_A;              /* Mifarecmd code */
	aubSendPacketBuffer[3] = ulBlockNumber;                  /* Block Number (1K = 0..63, 4K = 0..255 */
	Global_Memcpy (aubSendPacketBuffer+4, aubKey, 6);        /* Authentication Key */
	Global_Memcpy (aubSendPacketBuffer+10, aubPassiveTargetID, ulPassiveTargetIDLength);  /* Passive Target ID */

	/* Send the command */
	if (PN532_NFC_SendCommandFrame (aubSendPacketBuffer, 10+ulPassiveTargetIDLength))
	{
		DBG_ERR("\r\n [PN532_NFC_AuthenticateBlock] : Unable to send command frame");
		return FALSE;
	}

	// Read data packet
	if (PN532_NFC_ReceiveResposeFrame (aubReceivePacketBuffer, sizeof(aubReceivePacketBuffer), &ulReceivedDataLength))
	{
		DBG_ERR("\r\n [PN532_NFC_AuthenticateBlock] : Unable to receive response frame");
		return FALSE;
	}

	// Validate response data packet
	if ((aubReceivePacketBuffer[0] == PN532_RESPONSE_INDATAEXCHANGE) && (aubReceivePacketBuffer[1] == 0x00)) //Application notes page 34
	{
		DBG_PRT("\r\n PN532 NFC Authentification OK");
		PN532_NFC_SendAckFrame (); //Yet to handle NACK packet is validation fails
		aulAuthenticationDone[ulBlockNumber/sizeof(aulAuthenticationDone[0])] |= (0x01 << (ulBlockNumber % sizeof(aulAuthenticationDone[0])));
		return TRUE;
	}
	else if ((aubReceivePacketBuffer[0] == PN532_RESPONSE_INDATAEXCHANGE) && (aubReceivePacketBuffer[1] == 0x14)) //Application notes page 27 & 34
	{
		DBG_ERR("\r\n PN532 NFC Mifare Authentification Error");
		return FALSE;
	}
	else
	{
		DBG_ERR("\r\n PN532 NFC Authentification Error [%d][%d]", aubReceivePacketBuffer[0], aubReceivePacketBuffer[1]);
		return FALSE;
	}
}


/**************************************************************************
    Tries to read an entire 16-byte data block at the specified block
    address.

    @param  blockNumber   The block number to authenticate.  (0..63 for
                          1KB cards, and 0..255 for 4KB cards).
    
    @returns 1 if everything executed properly, 0 for an error
**************************************************************************/
unsigned int PN532_NFC_ReadDataBlock (unsigned int ulBlockNumber, unsigned char *pubData)
{
	unsigned char aubSendPacketBuffer[4];
	unsigned char aubReceivePacketBuffer[32];
	unsigned int ulReceivedDataLength;
	//unsigned int i;

	DBG_PRT("\r\n [PN532_NFC_ReadDataBlock][%d]", ulBlockNumber);

	/* Prepare the command */
	aubSendPacketBuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
	aubSendPacketBuffer[1] = ulTagNumber;              /* Card number */
	aubSendPacketBuffer[2] = MIFARE_CMD_READ;          /* Mifare Read command = 0x30 */
	aubSendPacketBuffer[3] = ulBlockNumber;            /* Block Number (0..63 for 1K, 0..255 for 4K) */

	/* Send the command */
	if (PN532_NFC_SendCommandFrame (aubSendPacketBuffer, 4))
	{
		DBG_ERR("\r\n [PN532_NFC_ReadDataBlock] : Unable to send command frame");
		return FALSE;
	}

	Global_Delay (100); //Give some time to read data

	// read data packet
	if (PN532_NFC_ReceiveResposeFrame (aubReceivePacketBuffer, sizeof(aubReceivePacketBuffer), &ulReceivedDataLength))
	{
		DBG_ERR("\r\n [PN532_NFC_ReadDataBlock] : Unable to receive response frame");
		return FALSE;
	}

	// Validate response data packet
	if (ulReceivedDataLength != 18) // Response code (1 byte) + response result (1 byte) + 16 data bytes 
	{
		DBG_ERR("\r\n PN532 NFC Mifare Read Error: Received length is not 18 [%d]", ulReceivedDataLength);
		return FALSE;
	}
	else if ((aubReceivePacketBuffer[0] == PN532_RESPONSE_INDATAEXCHANGE) && (aubReceivePacketBuffer[1] == 0x00)) //Application notes page 34
	{
		DBG_PRT("\r\n PN532 NFC Read Block OK");
		PN532_NFC_SendAckFrame (); //Yet to handle NACK packet is validation fails
		Global_Memcpy (pubData, aubReceivePacketBuffer+2, 16); //Do not copy Response code (1 byte) + response result (1 byte)

		return TRUE;
	}
	else if ((aubReceivePacketBuffer[0] == PN532_RESPONSE_INDATAEXCHANGE) && (aubReceivePacketBuffer[1] == 0x01)) //Application notes page 27 & 34
	{
		DBG_ERR("\r\n PN532 NFC Mifare Read Error: Time Out, the target has not answered");
		return FALSE;
	}
	else
	{
		DBG_ERR("\r\n PN532 NFC Read Error [%d][%d]", aubReceivePacketBuffer[0], aubReceivePacketBuffer[1]);
		return FALSE;
	} 	
}

/**************************************************************************
    Write entire 16-byte data block at the specified block address.

    @param  blockNumber   The block number to authenticate.  (0..63 for
                          1KB cards, and 0..255 for 4KB cards).
    @param  data          The byte array that contains the data to write.
    
    @returns 1 if everything executed properly, 0 for an error
**************************************************************************/
unsigned int PN532_NFC_WriteDataBlock (unsigned int ulBlockNumber, unsigned char *pubData)
{
	unsigned char aubSendPacketBuffer[20];
	unsigned char aubReceivePacketBuffer[32];
	unsigned int ulReceivedDataLength;
	//unsigned int i;

	DBG_PRT("\r\n [PN532_NFC_ReadDataBlock][%d]", ulBlockNumber);

	// Validate input parameters
	if (pubData == 0x0)
	{
		DBG_ERR("\r\n [PN532_NFC_WriteDataBlock] : Data is NULL");
		return FALSE;
	}

	if (ulBlockNumber == 0)
	{
		DBG_ERR("\r\n [PN532_NFC_WriteDataBlock] : Dont touch block 0");
		return FALSE;
	}

	if (((ulBlockNumber+1) % 4) == 0) //Dont update sector tailer blocks 3, 7, .. , 63
	{
		DBG_ERR("\r\n [PN532_NFC_WriteDataBlock] : Dont touch sector tailer blocks [%d] as of now", ulBlockNumber);
		return FALSE;
	}

	/* Prepare the command */  
	aubSendPacketBuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
	aubSendPacketBuffer[1] = ulTagNumber;                  /* Card number */
	aubSendPacketBuffer[2] = MIFARE_CMD_WRITE;             /* Mifare Write command = 0xA0 */
	aubSendPacketBuffer[3] = ulBlockNumber;                /* Block Number (0..63 for 1K, 0..255 for 4K) */
	Global_Memcpy (aubSendPacketBuffer+4, pubData, 16);     /* Data Payload */

	/* Send the command */
	if (PN532_NFC_SendCommandFrame (aubSendPacketBuffer, 20))
	{
		DBG_ERR("\r\n [PN532_NFC_WriteDataBlock] : Unable to send command frame");
		return FALSE;
	}

	Global_Delay (100); // Give some time to write

	// read data packet
	if (PN532_NFC_ReceiveResposeFrame (aubReceivePacketBuffer, sizeof(aubReceivePacketBuffer), &ulReceivedDataLength))
	{
		DBG_ERR("\r\n [PN532_NFC_WriteDataBlock] : Unable to receive response frame");
		return FALSE;
	} 

	// Validate response data packet
	if ((aubReceivePacketBuffer[0] == PN532_RESPONSE_INDATAEXCHANGE) && (aubReceivePacketBuffer[1] == 0x00)) //Application notes page 34
	{
		DBG_PRT("\r\n PN532 NFC Write Block OK");
		return TRUE;
	}
	else if ((aubReceivePacketBuffer[0] == PN532_RESPONSE_INDATAEXCHANGE) && (aubReceivePacketBuffer[1] == 0x01)) //Application notes page 27 & 34
	{
		DBG_ERR("\r\n PN532 NFC Mifare write Error: Time Out, the target has not answered");
		return FALSE;
	}
	else
	{
		DBG_ERR("\r\n PN532 NFC write Error [%d][%d]", aubReceivePacketBuffer[0], aubReceivePacketBuffer[1]);
		return FALSE;
	}
}

void PN532_NFC_Test (void)
{
	unsigned char aubReadData[16];
	unsigned char aubWriteData[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x65, 0x66, 0x67, 0x68, 0x69, 0x70};
	unsigned int i;

	if (PN532_NFC_ReadPassiveTargetID())
	{
		if(PN532_NFC_AuthenticateBlock (1))
		{
			PN532_NFC_ReadDataBlock (1, aubReadData);
			DBG_PRT_ALWAYS("\r\n [PN532_NFC_Test] Read1");
			for (i=0; i<16; i++) // First two bytes are response command code and read result
			{
				DBG_PRT_ALWAYS("[0x%x]", aubReadData[i]);
			}

			PN532_NFC_WriteDataBlock (1, aubWriteData);
			PN532_NFC_ReadDataBlock (1, aubReadData);
			DBG_PRT_ALWAYS("\r\n [PN532_NFC_Test] Read2");
			for (i=0; i<16; i++) // First two bytes are response command code and read result
			{
				DBG_PRT_ALWAYS("[0x%x]", aubReadData[i]);
			}			
		}
	}
}

