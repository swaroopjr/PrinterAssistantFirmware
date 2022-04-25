
#define IAP_LOCATION 0x7ffffff1
#define IAP_CCLK 12000

unsigned int IAP_Command[5];
unsigned int IAP_Result[2];

typedef void (*IAP)(unsigned int [], unsigned int []);
IAP IAP_Entry;

typedef enum
{
    IAP_CMD_SUCCESS,
    IAP_INVALID_COMMAND,
    IAP_SRC_ADDR_ERROR,
    IAP_DST_ADDR_ERROR,
    IAP_SRC_ADDR_NOT_MAPPED,
    IAP_DST_ADDR_NOT_MAPPED,
    IAP_COUNT_ERROR,
    IAP_INVALID_SECTOR,
    IAP_SECTOR_NOT_BLANK,
    IAP_SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION,
    IAP_COMPARE_ERROR,
    IAP_BUSY
}IAP_STATUS_CODES;

typedef enum
{
     IAP_CMD_CODE,
     IAP_CMD_PARAMETER_ONE,
     IAP_CMD_PARAMETER_TWO,
     IAP_CMD_PARAMETER_THREE,
     IAP_CMD_PARAMETER_FOUR
}IAP_COMMAND_PARAMETER;

typedef enum
{
	IAP_CMD_STATUS_CODE,
	IAP_CMD_RESULT_ONE
}IAP_COMMAND_RESULT;

unsigned int FLASH_WriteSector (unsigned int ulSectorNumber, unsigned int ulRamDataAddress, unsigned int ulSize);

