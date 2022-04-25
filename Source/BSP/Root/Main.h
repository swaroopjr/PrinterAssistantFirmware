
/************************************************* 
Heap incleases from 0x40003000 to 0x40003300
Stacl decreases from 0x40003FF0 to 0x40003300

     ------------ Heap Base
     |                  |
     |                  |      |
     |                  |      | 
     |                  |      V
     |                  |
     ------------ Heap Limit
     ------------ Stack Limit     
     |                  |
     |                  |      ^
     |                  |      | 
     |                  |      |
     |                  |
     ------------ Stack Base

************************************************ */
unsigned int __heap_base   = 0x40002000;
unsigned int __heap_limit  = 0x40003000;
unsigned int __stack_base  = 0x40004000 - 4; //0x40003300;
unsigned int __stack_limit = 0x40003000; //0x40003FF0; 

extern void UART_Main (void);
extern void IOPins_Main (void);
extern void FLASH_WriteTest (void);
extern void Test_Heap (void);
extern void Timer_Main (void);
extern void LCD_Main (void);


