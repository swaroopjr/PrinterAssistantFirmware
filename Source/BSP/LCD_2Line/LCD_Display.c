//LCD_Display.c

#include "LPC214x.H"
#include "Global_Enums.h"

#define LCD_DATA_PINS 0x00FF0000
#define LCD_RS_PIN (0x01<<10)
#define LCD_RST_PIN (0x01<<11)
#define LCD_RW_PIN (0x01<<12)
#define LCD_E_PIN (0x01<<13)

#define LCD_BUSY_CHECK_TIME 100
static unsigned int LCD_Charcounter;

static void LCD_Delay(unsigned long int ulValue)
{
    while(ulValue > 0) 
    {
        ulValue--;	// Loop Decrease Counter	
    }
}
 
static void LCD_Setup_GPIO_Pins ()
{
	//========== Make LCD pins as GPIO pins =============
	// Make LCD D0 to D7 as GPIO Pins
	PINSEL1 &= 0xFFFF0000; //P0.16 - P0.23 ->0 to 15 bits to be made 0

	// Make LCD RS pin as GPIO Pin
	PINSEL0 &= 0xFFCFFFFF; //P0.10 -> 20th and 21st bits to be made 0

	// Make LCD REST pin as GPIO Pin
	PINSEL0 &= 0xFF3FFFFF; //P0.11 -> 22nd and 23rd bits to be made 0
	
	// Make  LCD R/W pin as GPIO Pin
	PINSEL0 &= 0xFCFFFFFF; //P0.12 -> 24th and 25th bits to be made 0
	
	// Make  LCD Enable pin as GPIO Pin
	PINSEL0 &= 0xF3FFFFFF; //P0.13 -> 26th and 27th bits to be made 0
}

static void LCD_Make_Output_Pins ()
{
	//========== Make LCD pins as output pins =============
	// Make LCD D0 to D7 as output Pins
	IODIR0 |= LCD_DATA_PINS; //P0.16 - P0.23 -> 16 to 23rd bits to be made 1

	// Make LCD RS pin as output Pin
	IODIR0 |= LCD_RS_PIN; //P0.10 -> 10th bit to be made 1

	// Make LCD RS pin as output Pin
	IODIR0 |= LCD_RST_PIN; //P0.11 -> 11th bit to be made 1

	// Make  LCD R/W pin as output Pin
	IODIR0 |= LCD_RW_PIN; //P0.12 -> 12th bit to be made 1

	// Make  LCD Enable pin as output Pin
	IODIR0 |= LCD_E_PIN; //P0.13 -> 13th bit to be made 1
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

static void LCD_Ctrl_RW_Pin (unsigned int ON_OFF)
{
    if(ON_OFF)
        IOSET0 = LCD_RW_PIN;
    else
        IOCLR0 = LCD_RW_PIN;   
}

static void LCD_Ctrl_E_Pin (unsigned int ON_OFF)
{
    if(ON_OFF)
        IOSET0 = LCD_E_PIN;
    else
        IOCLR0 = LCD_E_PIN;   
}

static void LCD_SendDataBits (unsigned char Data)
{
	unsigned int uiLCD_Cmd_Data=0;
    IOCLR0 = LCD_DATA_PINS;
	uiLCD_Cmd_Data = Data << 16;
    IOSET0 = uiLCD_Cmd_Data;
}

#if 1
static unsigned char LCD_ReadDataBits (void)
{
    unsigned char Data;
	Data = IOPIN0 >> 16;
       return (Data);
}
#endif

#if 1
static unsigned int LCD_checkbusy(void)
{
#if 1
    unsigned char lcdstate;
    LCD_Ctrl_RS_Pin (0);
    LCD_Ctrl_RW_Pin (1);
    LCD_Ctrl_E_Pin (1); 
  	IODIR0 &= (~LCD_DATA_PINS); 
    LCD_Delay (10);
    
    lcdstate = LCD_ReadDataBits();
    LCD_Ctrl_E_Pin (0);
    return((unsigned int)(lcdstate & 0x80)); //D7 Bit
#else
    LCD_Delay (10000);
    return 0;

#endif
}
#endif

static void LCD_WriteCmd(unsigned char lcdcmd)
{
	unsigned int ulTemp = 0;

	ulTemp = 0;	
	while(LCD_checkbusy())
	{
		ulTemp++;
		if (ulTemp >= LCD_BUSY_CHECK_TIME)
			break;
	}
	
	LCD_Make_Output_Pins (); // Make all LCD pins as output pins
    LCD_SendDataBits(lcdcmd);

    LCD_Ctrl_RS_Pin (0);
    LCD_Ctrl_RW_Pin (0);
    LCD_Ctrl_E_Pin (1);
    LCD_Delay (10); //soft_nop();
    LCD_Ctrl_E_Pin (0);
}

static void LCD_SwitchLine(unsigned int LineNo)
{
    unsigned char cmd = 0x80;
	unsigned char Address;
	unsigned int ulTemp = 0;

    if (LineNo == 1)
        Address = 0x00;
    else if (LineNo == 2)
        Address = 0x2C;
	else
        return;

	ulTemp = 0;
    while(LCD_checkbusy())
	{
		ulTemp++;
		if (ulTemp >= LCD_BUSY_CHECK_TIME)
			break;
	}

    cmd |= Address;
    LCD_WriteCmd(cmd); 
}

static void LCD_WriteDisplayChar(char LCD_Char, unsigned int ContinousMode)
{
	unsigned int ulTemp = 0;

	ulTemp = 0;
    while(LCD_checkbusy())
	{
		ulTemp++;
		if (ulTemp >= LCD_BUSY_CHECK_TIME)
			break;
	}		

	if (ContinousMode)
	{
        if(LCD_Charcounter == 16)
        {
            LCD_SwitchLine(2);

			ulTemp = 0;
            while(LCD_checkbusy())
			{
				ulTemp++;
				if (ulTemp >= LCD_BUSY_CHECK_TIME)
					break;
			}
        }
        else if(LCD_Charcounter >= 32)
            return;

		LCD_Charcounter++;
	}

	LCD_Make_Output_Pins (); // Make all LCD pins as output pins
    LCD_SendDataBits(LCD_Char);
    
    LCD_Ctrl_RS_Pin (1);
    LCD_Ctrl_RW_Pin (0);
    LCD_Ctrl_E_Pin (1);
    LCD_Delay (10); //soft_nop();
    LCD_Ctrl_E_Pin (0);
}

void LCD_Init (void)
{ 
	 LCD_Setup_GPIO_Pins ();
	 LCD_Make_Output_Pins ();

     //Function Set: 0 0 1 DL N F X X
     LCD_WriteCmd(0x38); // [Function Set]: DL(8-Bit mode), N(Two Line Display), F(5X7 dots font)

	 //Display ON/OFF: 0 0 0 0 1 D C B
     LCD_WriteCmd(0x0c); //[Display ON/OFF]: D(Display ON), C(Cursor OFF), B(Blink OFF)
     //LCD_WriteCmd(0x0F); //[Display ON/OFF]: D(Display ON), C(Cursor ON), B(Blink ON)

	 //Entry Mode Set: 0 0 0 0 0 1 I/D S
     LCD_WriteCmd(0x06); // I/D: Increments DD Address, S: Entire Display Shift OFF

	 //Clear Display: 0 0 0 0 0 0 0 1
     LCD_WriteCmd(0x01); // Clears Display and returns to Home DD address

     LCD_Charcounter=0;
}

static void LCD_DisplayStringContinous (char *DisplayString)
{
    unsigned int i=0;
    while(DisplayString[i])
	{
        LCD_WriteDisplayChar(DisplayString[i], TRUE);
        i++;
    }
}

void LCD_DisplayStringInFirstLine (char *DisplayString)
{
    unsigned int i=0;
	LCD_SwitchLine (1);
    while(DisplayString[i])
	{
		if (i >= 16)
			break;
	
        LCD_WriteDisplayChar(DisplayString[i], FALSE);
        i++;
    }
}

void LCD_DisplayStringInSecondLine (char *DisplayString)
{
    unsigned int i=0;
	LCD_SwitchLine (2);
    while(DisplayString[i])
	{
		if (i >= 16)
			break;

        LCD_WriteDisplayChar(DisplayString[i], FALSE);
        i++;
    }
}


void LCD_Main (void)
{
    LCD_Init();
	LCD_DisplayStringInFirstLine ("     System     ");
	LCD_DisplayStringInSecondLine ("  Initializing  ");
    LCD_Delay (20000);
    //LCD_Delay (20000);
}

void LCD_Test_GPIO_Pins (void)
{
	unsigned int ulIdx;

	LCD_Setup_GPIO_Pins ();
	LCD_Make_Output_Pins ();	

	LCD_Ctrl_RS_Pin (0);
	LCD_Ctrl_RS_Pin (1);
	LCD_Ctrl_RS_Pin (0);
	LCD_Ctrl_RS_Pin (1);

	LCD_Ctrl_RW_Pin (0);
	LCD_Ctrl_RW_Pin (1);
	LCD_Ctrl_RW_Pin (0);
	LCD_Ctrl_RW_Pin (1);	

	LCD_Ctrl_E_Pin (0);
	LCD_Ctrl_E_Pin (1);
	LCD_Ctrl_E_Pin (0);
	LCD_Ctrl_E_Pin (1);	

	for (ulIdx=0; ulIdx<16; ulIdx++)
	{
		LCD_SendDataBits (0x00);
		LCD_SendDataBits (0xFF);
	}
}

