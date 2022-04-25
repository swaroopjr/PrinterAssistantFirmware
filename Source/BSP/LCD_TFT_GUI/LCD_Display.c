//LCD_Display.c

#define LCD_TFT_DISPLAY_C

#include "LPC214x.H"
#include "Global_Enums.h"

// Pin connections to LPC2148
// LCD_D0  -- P0.16 - PIN_46
// LCD_D1  -- P0.17 - PIN_47
// LCD_D2  -- P0.18 - PIN_53
// LCD_D3  -- P0.19 - PIN_54
// LCD_D4  -- P0.20 - PIN_55
// LCD_D5  -- P0.21 - PIN_01
// LCD_D6  -- P0.22 - PIN_02
// LCD_D7  -- P0.23 - PIN_58
// LCD_RST -- P0.31 - //This has to be connected to GPIO. We cannot make in high always as reset pulse is required at bootup without which, LCD does not work sometimes.
// LCD_CS  -- P0.13 - PIN_39
// LCD_RS  -- P0.10 - PIN_35
// LCD_WR  -- P0.12 - PIN_38
// LCD_RD  -- Connect to HIGH always as we do not use read functionality

#define LCD_DATA_PINS 0x00FF0000
#define LCD_RS_PIN (0x01<<10) //D/CX Pin
#define LCD_RST_PIN (0x01<<31)
#define LCD_WR_PIN (0x01<<12)
#define LCD_CS_PIN (0x01<<13)

#define	LCD24_COLOR_BLACK   0x0000
#define	LCD24_COLOR_RED     0x001F
#define	LCD24_COLOR_BLUE    0xF800
#define	LCD24_COLOR_GREEN   0x07E0
#define LCD24_COLOR_YELLOW  0x07FF
#define LCD24_COLOR_MAGENTA 0xF81F
#define LCD24_COLOR_CYAN    0xFFE0
#define LCD24_COLOR_WHITE   0xFFFF

#define LCDTFT_CMD_NO_OPERATION               0x00
#define LCDTFT_CMD_SOFTWARE_RESET             0x01
#define LCDTFT_CMD_PUMP_RATION_CONTROL        0xF7
#define LCDTFT_CMD_PIXEL_FORMAT_SET           0x3A
#define LCDTFT_CMD_MEMORY_ACCESS_CONTROL      0x36
#define LCDTFT_CMD_ENTER_SLEEP_MODE           0x10
#define LCDTFT_CMD_SLEEP_OUT                  0x11
#define LCDTFT_CMD_DISPLAY_OFF                0x28
#define LCDTFT_CMD_DISPLAY_ON                 0x29
#define LCDTFT_CMD_COLUMN_ADDRESS_SET         0x2A
#define LCDTFT_CMD_ROW_ADDRESS_SET            0x2B
#define LCDTFT_CMD_MEMORY_WRITE               0x2C

static const unsigned char CHARECTER_ASCII_TABLE [][5] =
  {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 20
    {0x00, 0x00, 0x5f, 0x00, 0x00}, // 21 !	
    {0x00, 0x07, 0x00, 0x07, 0x00}, // 22 "
    {0x14, 0x7f, 0x14, 0x7f, 0x14}, // 23 # 
    {0x24, 0x2a, 0x7f, 0x2a, 0x12} ,// 24 $
    {0x23, 0x13, 0x08, 0x64, 0x62}, // 25 %
    {0x36, 0x49, 0x55, 0x22, 0x50}, // 26 &
    {0x00, 0x00, 0x07, 0x05, 0x07}, // 27 ' 
    {0x00, 0x1c, 0x22, 0x41, 0x00}, // 28 (
    {0x00, 0x41, 0x22, 0x1c, 0x00}, // 29 )
    {0x14, 0x08, 0x3e, 0x08, 0x14}, // 2a *
    {0x08, 0x08, 0x3e, 0x08, 0x08}, // 2b +
    {0x00, 0x50, 0x30, 0x00, 0x00}, // 2c ,
    {0x08, 0x08, 0x08, 0x08, 0x08}, // 2d -
    {0x00, 0x60, 0x60, 0x00, 0x00}, // 2e .
    {0x20, 0x10, 0x08, 0x04, 0x02}, // 2f /
    {0x3e, 0x51, 0x49, 0x45, 0x3e}, // 30 0
    {0x00, 0x42, 0x7f, 0x40, 0x00}, // 31 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 32 2
    {0x21, 0x41, 0x45, 0x4b, 0x31}, // 33 3
    {0x18, 0x14, 0x12, 0x7f, 0x10}, // 34 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 35 5
    {0x3c, 0x4a, 0x49, 0x49, 0x30}, // 36 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 37 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 38 8
    {0x06, 0x49, 0x49, 0x29, 0x1e}, // 39 9
    {0x00, 0x36, 0x36, 0x00, 0x00}, // 3a :
    {0x00, 0x56, 0x36, 0x00, 0x00}, // 3b ;
    {0x08, 0x14, 0x22, 0x41, 0x00}, // 3c <
    {0x14, 0x14, 0x14, 0x14, 0x14}, // 3d =
    {0x00, 0x41, 0x22, 0x14, 0x08}, // 3e >
    {0x02, 0x01, 0x51, 0x09, 0x06}, // 3f ?
    {0x32, 0x49, 0x79, 0x41, 0x3e}, // 40 @
    {0x7e, 0x11, 0x11, 0x11, 0x7e}, // 41 A
    {0x7f, 0x49, 0x49, 0x49, 0x36}, // 42 B
    {0x3e, 0x41, 0x41, 0x41, 0x22}, // 43 C
    {0x7f, 0x41, 0x41, 0x22, 0x1c}, // 44 D
    {0x7f, 0x49, 0x49, 0x49, 0x41}, // 45 E
    {0x7f, 0x09, 0x09, 0x09, 0x01}, // 46 F
    {0x3e, 0x41, 0x49, 0x49, 0x7a}, // 47 G
    {0x7f, 0x08, 0x08, 0x08, 0x7f}, // 48 H
    {0x00, 0x41, 0x7f, 0x41, 0x00}, // 49 I
    {0x20, 0x40, 0x41, 0x3f, 0x01}, // 4a J
    {0x7f, 0x08, 0x14, 0x22, 0x41}, // 4b K
    {0x7f, 0x40, 0x40, 0x40, 0x40}, // 4c L
    {0x7f, 0x02, 0x0c, 0x02, 0x7f}, // 4d M
    {0x7f, 0x04, 0x08, 0x10, 0x7f}, // 4e N
    {0x3e, 0x41, 0x41, 0x41, 0x3e}, // 4f O
    {0x7f, 0x09, 0x09, 0x09, 0x06}, // 50 P
    {0x3e, 0x41, 0x51, 0x21, 0x5e}, // 51 Q
    {0x7f, 0x09, 0x19, 0x29, 0x46}, // 52 R
    {0x46, 0x49, 0x49, 0x49, 0x31}, // 53 S
    {0x01, 0x01, 0x7f, 0x01, 0x01}, // 54 T
    {0x3f, 0x40, 0x40, 0x40, 0x3f}, // 55 U
    {0x1f, 0x20, 0x40, 0x20, 0x1f}, // 56 V
    {0x3f, 0x40, 0x38, 0x40, 0x3f}, // 57 W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // 58 X
    {0x07, 0x08, 0x70, 0x08, 0x07}, // 59 Y
    {0x61, 0x51, 0x49, 0x45, 0x43}, // 5a Z
    {0x00, 0x7f, 0x41, 0x41, 0x00}, // 5b [
    {0x02, 0x04, 0x08, 0x10, 0x20}, // 5c Y
    {0x00, 0x41, 0x41, 0x7f, 0x00}, // 5d ]
    {0x04, 0x02, 0x01, 0x02, 0x04}, // 5e ^
    {0x40, 0x40, 0x40, 0x40, 0x40}, // 5f _
    {0x00, 0x01, 0x02, 0x04, 0x00}, // 60 `
    {0x20, 0x54, 0x54, 0x54, 0x78}, // 61 a
    {0x7f, 0x48, 0x44, 0x44, 0x38}, // 62 b
    {0x38, 0x44, 0x44, 0x44, 0x20}, // 63 c
    {0x38, 0x44, 0x44, 0x48, 0x7f}, // 64 d
    {0x38, 0x54, 0x54, 0x54, 0x18}, // 65 e
    {0x08, 0x7e, 0x09, 0x01, 0x02}, // 66 f
    {0x0c, 0x52, 0x52, 0x52, 0x3e}, // 67 g
    {0x7f, 0x08, 0x04, 0x04, 0x78}, // 68 h
    {0x00, 0x44, 0x7d, 0x40, 0x00}, // 69 i
    {0x20, 0x40, 0x44, 0x3d, 0x00}, // 6a j
    {0x7f, 0x10, 0x28, 0x44, 0x00}, // 6b k
    {0x00, 0x41, 0x7f, 0x40, 0x00}, // 6c l
    {0x7c, 0x04, 0x18, 0x04, 0x78}, // 6d m
    {0x7c, 0x08, 0x04, 0x04, 0x78}, // 6e n
    {0x38, 0x44, 0x44, 0x44, 0x38}, // 6f o
    {0x7c, 0x14, 0x14, 0x14, 0x08}, // 70 p
    {0x08, 0x14, 0x14, 0x18, 0x7c}, // 71 q
    {0x7c, 0x08, 0x04, 0x04, 0x08}, // 72 r
    {0x48, 0x54, 0x54, 0x54, 0x20}, // 73 s
    {0x04, 0x3f, 0x44, 0x40, 0x20}, // 74 t
    {0x3c, 0x40, 0x40, 0x20, 0x7c}, // 75 u
    {0x1c, 0x20, 0x40, 0x20, 0x1c}, // 76 v
    {0x3c, 0x40, 0x30, 0x40, 0x3c}, // 77 w
    {0x44, 0x28, 0x10, 0x28, 0x44}, // 78 x
    {0x0c, 0x50, 0x50, 0x50, 0x3c}, // 79 y
    {0x44, 0x64, 0x54, 0x4c, 0x44}, // 7a z
    {0x00, 0x08, 0x36, 0x41, 0x00}, // 7b {
    {0x00, 0x00, 0x7f, 0x00, 0x00}, // 7c |
    {0x00, 0x41, 0x36, 0x08, 0x00}, // 7d }
    {0x10, 0x08, 0x08, 0x10, 0x08}, // 7e ?
    {0x00, 0x06, 0x09, 0x09, 0x06}  // 7f ?
  }; 


static void LCD_Delay(unsigned long int ulValue)
{
    while(ulValue > 0) 
    {
        ulValue--;	// Loop Decrease Counter	
    }
}
 
static void LCD_Setup_GPIO_Pins (void)
{
	//========== Make LCD pins as GPIO pins =============
	// Make LCD D0 to D7 as GPIO Pins
	PINSEL1 &= 0xFFFF0000; //P0.16 - P0.23 ->0 to 15 bits to be made 0

	// Make LCD RS (D/CX) pin as GPIO Pin
	PINSEL0 &= 0xFFCFFFFF; //P0.10 -> 20th and 21st bits to be made 0
	
	// Make LCD RST pin as GPIO Pin
	PINSEL1 &= 0x3FFFFFFF; //P0.31 -> 30th and 31st bits to be made 0
	
	// Make  LCD WR pin as GPIO Pin
	PINSEL0 &= 0xFCFFFFFF; //P0.12 -> 24th and 25th bits to be made 0
	
	// Make  LCD CS pin as GPIO Pin
	PINSEL0 &= 0xF3FFFFFF; //P0.13 -> 26th and 27th bits to be made 0
}

static void LCD_Make_Output_Pins (void)
{
	//========== Make LCD pins as output pins =============
	// Make LCD D0 to D7 as output Pins
	IODIR0 |= LCD_DATA_PINS; //P0.16 - P0.23 -> 16 to 23rd bits to be made 1

	// Make LCD RS (D/CX) pin as output Pin
	IODIR0 |= LCD_RS_PIN; //P0.31 -> 31st bit to be made 1

	// Make LCD RST pin as output Pin
	IODIR0 |= LCD_RST_PIN; //P0.11 -> 11th bit to be made 1

	// Make  LCD WR pin as output Pin
	IODIR0 |= LCD_WR_PIN; //P0.12 -> 12th bit to be made 1

	// Make  LCD CS pin as output Pin
	IODIR0 |= LCD_CS_PIN; //P0.13 -> 13th bit to be made 1
}

static void LCD_Ctrl_RS_Pin (unsigned int ON_OFF)
{
    if(ON_OFF)
        IOSET0 = LCD_RS_PIN;
    else
        IOCLR0 = LCD_RS_PIN;    
}

static void LCD_Ctrl_RST_Pin (unsigned int ON_OFF)
{
    if(ON_OFF)
        IOSET0 = LCD_RST_PIN;
    else
        IOCLR0 = LCD_RST_PIN;    
}

static void LCD_Ctrl_WR_Pin (unsigned int ON_OFF)
{
    if(ON_OFF)
        IOSET0 = LCD_WR_PIN;
    else
        IOCLR0 = LCD_WR_PIN;   
}

static void LCD_Ctrl_CS_Pin (unsigned int ON_OFF) //D/CX(Data/command) pin
{
    if(ON_OFF)
        IOSET0 = LCD_CS_PIN;
    else
        IOCLR0 = LCD_CS_PIN;   
}

static void LCD_Send8BitDataBits (unsigned char ubData)
{
	LCD_Ctrl_WR_Pin (0);

    IOCLR0 = LCD_DATA_PINS;
    IOSET0 = ubData << 16;	

	LCD_Ctrl_WR_Pin (1);	
}

static void LCD_Test_GPIO_Pins (void)
{
	unsigned int ulIdx;

	LCD_Setup_GPIO_Pins ();
	LCD_Make_Output_Pins ();	

	LCD_Ctrl_RS_Pin (0);
	LCD_Ctrl_RS_Pin (1);
	LCD_Ctrl_RS_Pin (0);
	LCD_Ctrl_RS_Pin (1);

	LCD_Ctrl_WR_Pin (0);
	LCD_Ctrl_WR_Pin (1);
	LCD_Ctrl_WR_Pin (0);
	LCD_Ctrl_WR_Pin (1);	

	LCD_Ctrl_CS_Pin (0);
	LCD_Ctrl_CS_Pin (1);
	LCD_Ctrl_CS_Pin (0);
	LCD_Ctrl_CS_Pin (1);	

	for (ulIdx=0; ulIdx<16; ulIdx++)
	{
		LCD_Send8BitDataBits (0x00);
		LCD_Send8BitDataBits (0xFF);
	}
}

static void LCD_WriteCommand (unsigned char ubData)
{
    LCD_Ctrl_RS_Pin (0);
    LCD_Send8BitDataBits (ubData);
}

static void LCD_WriteData (unsigned char ubData)
{
    LCD_Ctrl_RS_Pin (1);
    LCD_Send8BitDataBits (ubData);
}

void LCD_Init (void)
{
    LCD_Setup_GPIO_Pins ();
    LCD_Make_Output_Pins ();
    //LCD_Test_GPIO_Pins (); // Only for tested. This line has to be commented after initial GPIO test
    
    //Initial GPIO values
    LCD_Ctrl_RST_Pin (0);
    LCD_Delay(50000);
    LCD_Ctrl_RST_Pin (1);
    LCD_Delay(50000);
    
    LCD_Ctrl_CS_Pin (1);
    LCD_Ctrl_WR_Pin (1);
    LCD_Ctrl_RS_Pin (1);
    LCD_Ctrl_CS_Pin (0);	
    
    LCD_WriteCommand (LCDTFT_CMD_PUMP_RATION_CONTROL); //Pump Ratio Control
    LCD_WriteData (0x20);
    
    LCD_WriteCommand (LCDTFT_CMD_PIXEL_FORMAT_SET); //COLMOD: Pixel Formal Set
    LCD_WriteData (0x55);
    
    LCD_WriteCommand (LCDTFT_CMD_MEMORY_ACCESS_CONTROL); // Memory Access Control 
    // MY  - Row Address Order (bit7)
    // MX  - Column Address Order
    // MV  - Row / Column Exchange
    // ML  - Vertical Refresh Order
    // BGR - RGB-BGR Order
    // MH  - Horizontal Refresh ORDER(bit2)
    LCD_WriteData (0x08); //(B00001000); 	
    
    LCD_WriteCommand (LCDTFT_CMD_SLEEP_OUT); // Sleep OUT
    LCD_WriteCommand (LCDTFT_CMD_DISPLAY_ON); // Display ON
    
    LCD_Delay (50);	
}

#if 1
static void LCD_Display_Clear (unsigned char uwColor) // This is for fast clear
{
  /* 
  Clear screen faster sacrifing color depth. Instead of writing
  to data bits high and low byte of the color for each pixel, which takes more 
  than 300ms to fill the screen, set once data bits to 0's for black or 
  to 1's for white and start changing control bit WR from LOW to HIGH to 
  write the whole area. It takes cca 70 ms. In this way the colors of screen are
  limited to those with the same high and low byte. For example setting color
  to 0x0C fills the screen with color 0x0C0C.
  Writing two pixels in one cycle lowering cycle count from 76800 (240x320) to 
  38400 clears screen in less then 30ms.
  */
    unsigned int x, y;
    
    LCD_WriteCommand(LCDTFT_CMD_COLUMN_ADDRESS_SET); 
    LCD_WriteData(0);
    LCD_WriteData(0);
    LCD_WriteData(0);
    LCD_WriteData(0xEC);
    
    LCD_WriteCommand(LCDTFT_CMD_ROW_ADDRESS_SET); 
    LCD_WriteData(0); 
    LCD_WriteData(0);
    LCD_WriteData(1);
    LCD_WriteData(0x3F);
    
    LCD_WriteCommand(LCDTFT_CMD_MEMORY_WRITE);
    
    LCD_Ctrl_RS_Pin (1);
    
    IOCLR0 = LCD_DATA_PINS;
    IOSET0 = uwColor << 16;
    
    
    x=38400 * 4; // 240*320/2
    for(y=0;y<x;y++)
    {
        LCD_Ctrl_WR_Pin (0);
        LCD_Ctrl_WR_Pin (1);
    }

}
#endif

static void LCD_Display_Rectangle (unsigned short uwCol, unsigned short uwRow, unsigned short uwWidth, unsigned short uwHeight, unsigned short uwColor) 
{
  unsigned char ubColor_MSB;
  unsigned char ubColor_LSB;
  unsigned int i, j;

 
  LCD_WriteCommand(LCDTFT_CMD_COLUMN_ADDRESS_SET); // Column Address Set
  LCD_WriteData(uwRow>>8);
  LCD_WriteData(uwRow);
  LCD_WriteData((uwRow+uwHeight-1)>>8);
  LCD_WriteData(uwRow+uwHeight-1);

  LCD_WriteCommand(LCDTFT_CMD_ROW_ADDRESS_SET); // Page Address Set
  LCD_WriteData(uwCol>>8); 
  LCD_WriteData(uwCol);
  LCD_WriteData((uwCol+uwWidth-1)>>8);
  LCD_WriteData(uwCol+uwWidth-1);

  LCD_WriteCommand(LCDTFT_CMD_MEMORY_WRITE); // Memory Write
  ubColor_MSB = uwColor >> 8;
  ubColor_LSB = uwColor;
  for(i=0;i<uwWidth;i++)
  {
    for(j=0;j<uwHeight;j++)
    {
      LCD_WriteData(ubColor_MSB);
      LCD_WriteData(ubColor_LSB);
    }
  }
}

void LCD_Display_Charecter (unsigned short uwCol, unsigned short uwRow, unsigned short uwFontColor, unsigned short uwBackColor, unsigned char ubFontSize, unsigned char ubData)
{
    LCD_WriteCommand(LCDTFT_CMD_COLUMN_ADDRESS_SET); // ROWS
    LCD_WriteData(uwRow>>8);
    LCD_WriteData(uwRow);
    LCD_WriteData(((uwRow+ubFontSize*8)-1)>>8);
    LCD_WriteData((uwRow+ubFontSize*8)-1);
    
    LCD_WriteCommand(LCDTFT_CMD_ROW_ADDRESS_SET); // COLUMNS
    LCD_WriteData(uwCol>>8); 
    LCD_WriteData(uwCol);
    LCD_WriteData((uwCol+(ubFontSize*6))>>8);
    LCD_WriteData(uwCol+(ubFontSize*6));
    
    LCD_WriteCommand(LCDTFT_CMD_MEMORY_WRITE);
    {
        unsigned int ulIndex, ulNbit;
        unsigned int i, j;
        unsigned char ubColor_LSB, ubColor_MSB;
        
        for (ulIndex = 0; ulIndex < 5; ulIndex++) 
        {    
            unsigned char ubColData = CHARECTER_ASCII_TABLE[ubData - 0x20][ulIndex];
            for ( i=0; i<ubFontSize; i++)
            {
                unsigned char ubMask=0x01;
                for (ulNbit = 0; ulNbit < 8; ulNbit++) 
                {
                    if (ubColData & ubMask) 
                    {
                        for (j=0; j<ubFontSize; j++)
                        {
                            ubColor_MSB = uwFontColor >> 8;
                            ubColor_LSB = uwFontColor;
                            LCD_WriteData(ubColor_MSB);
                            LCD_WriteData(ubColor_LSB);
                        }
                    }
                    else 
                    {
                        for (j=0; j<ubFontSize; j++)
                        {
                            ubColor_MSB = uwBackColor >> 8;
                            ubColor_LSB = uwBackColor;
                            LCD_WriteData(ubColor_MSB);
                            LCD_WriteData(ubColor_LSB);
                        }
                    }
                    
                    ubMask = ubMask << 1;
                }
            }
        }
    }
}

static void LCD_Display_String (   unsigned short uwCol, 
	                                   unsigned short uwRow, 
	                                   unsigned short uwFontColor, 
	                                   unsigned short uwBackColor, 
	                                   unsigned char ubFontSize, 
	                                   unsigned char * pubString, 
	                                   unsigned int ulStringLength)
{
    unsigned int ulIndex;
    
    for (ulIndex=0; ulIndex<ulStringLength; ulIndex++)
    {
        LCD_Display_Charecter (uwCol, uwRow, uwFontColor, uwBackColor, ubFontSize, pubString[ulIndex]);
        uwCol = uwCol + (6*ubFontSize) + 2;		

        if( (uwCol+(ubFontSize*6)) > 319) 
        {
            uwCol=0;
            uwRow += (ubFontSize*6) + 2;
        }		
    }
}


static void LCD_Test_Rectange (void)
{
    unsigned int ulIndex;
    for (ulIndex=0; ulIndex<10; ulIndex++)
    {
        LCD_Display_Rectangle(0,0,320,240, LCD24_COLOR_WHITE); // slow
        //LCD_Delay(5000);
        
        LCD_Display_Rectangle(0,0,320,240, LCD24_COLOR_BLACK); // slow	
        //LCD_Delay(5000);
        
        LCD_Display_Rectangle(0,0,320,240, LCD24_COLOR_BLUE); // slow	
        //LCD_Delay(5000);
        
        LCD_Display_Rectangle(0,0,320,240, LCD24_COLOR_RED); // slow	
        //LCD_Delay(5000);
        
        LCD_Display_Rectangle(0,0,320,240, LCD24_COLOR_GREEN); // slow	
        //LCD_Delay(5000);
        
        LCD_Display_Rectangle(0,0,320,240, LCD24_COLOR_CYAN); // slow	
        //LCD_Delay(5000);
        
        LCD_Display_Rectangle(0,0,320,240, LCD24_COLOR_MAGENTA); // slow	
        //LCD_Delay(5000);
        
        LCD_Display_Rectangle(0,0,320,240, LCD24_COLOR_YELLOW); // slow 
        //LCD_Delay(5000);
        
        LCD_Display_Rectangle(0,0,320,240, LCD24_COLOR_BLACK); // slow	
        //LCD_Delay(5000);
    }
}

static void LCD_Test_Charecters (void)
{
    unsigned int ulIndex;
    
    LCD_Display_Rectangle(0,0,320,240, LCD24_COLOR_BLACK); // slow
    
    for (ulIndex=0x30; ulIndex<0x30+10; ulIndex++) //Numbers
    {
        LCD_Display_Charecter (0, 0, LCD24_COLOR_BLUE, LCD24_COLOR_RED, 3, ulIndex);
    }
    
    for (ulIndex=0x41; ulIndex<0x41+26; ulIndex++) //Capital Letters
    {
        LCD_Display_Charecter (0, 0, LCD24_COLOR_BLUE, LCD24_COLOR_RED, 3, ulIndex);
    }
    
    for (ulIndex=0x61; ulIndex<0x61+26; ulIndex++) //Small Letters
    {
        LCD_Display_Charecter (0, 0, LCD24_COLOR_BLUE, LCD24_COLOR_RED, 3, ulIndex);
    }
}

static void LCD_Test_String (void)
{
    unsigned int ulRow = 20;
    //LCD_Display_Rectangle(0,0,320,240, LCD24_COLOR_WHITE); // slow
    LCD_Display_Clear (LCD24_COLOR_WHITE);
    //LCD_Display_Clear (LCD24_COLOR_BLACK);
    
    LCD_Display_String (20, ulRow, LCD24_COLOR_BLUE, LCD24_COLOR_WHITE, 3, (unsigned char *)("Dear Poornima,"), 14);
    ulRow = ulRow + 30;	
    LCD_Display_String (20, ulRow, LCD24_COLOR_BLUE, LCD24_COLOR_WHITE, 3, (unsigned char *)("              "), 14);    	
    ulRow = ulRow + 30;		
    LCD_Display_String (20, ulRow, LCD24_COLOR_BLUE, LCD24_COLOR_WHITE, 3, (unsigned char *)("I Love You !!!"), 14);    
    ulRow = ulRow + 30;		
    LCD_Display_String (20, ulRow, LCD24_COLOR_BLUE, LCD24_COLOR_WHITE, 3, (unsigned char *)("              "), 14);    	
    ulRow = ulRow + 30;		
    LCD_Display_String (20, ulRow, LCD24_COLOR_BLUE, LCD24_COLOR_WHITE, 3, (unsigned char *)("     - Swaroop"), 14);    
    ulRow = ulRow + 30;		
    LCD_Display_String (20, ulRow, LCD24_COLOR_BLUE, LCD24_COLOR_WHITE, 3, (unsigned char *)("    9886233312"), 14);    	
}


void LCD_Main (void)
{
    LCD_Init ();
    LCD_Test_String ();
}

/*******************************************************************************/
/*******************************************************************************/
/*******************************************************************************/
/*******************************************************************************/
void LCD_DisplayStringInFirstLine (char *DisplayString) //to avoid compilation error
{

}

void LCD_DisplayStringInSecondLine (char *DisplayString) //to avoid compilation error
{

}

