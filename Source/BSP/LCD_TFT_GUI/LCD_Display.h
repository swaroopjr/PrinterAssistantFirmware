#undef EXTERN
#ifdef LCD_TFT_DISPLAY_C
#define EXTERN 
#else
#define EXTERN  extern
#endif

EXTERN void LCD_Init (void);
EXTERN void LCD_Main (void);

EXTERN void LCD_DisplayStringInFirstLine (char *DisplayString);
EXTERN void LCD_DisplayStringInSecondLine (char *DisplayString) ;