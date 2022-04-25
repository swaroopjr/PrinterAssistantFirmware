@echo off

SET LCD_Display=LCD_2Line
rem SET LCD_Display=LCD_TFT_GUI

rem ================================ Start Compilation =====================================================
echo Compiling

echo Compiling Global.c
armcc.exe -g -c -errors comp.err -I ..\..\Common ..\..\Common\Global.c -o Obj\Global.o
if not exist Obj\Global.o goto COMPILE_ERR
rem pause

echo Compiling Flash_Drv.c
tcc.exe -g -c -errors comp.err -apcs /interwork -I ..\..\Common -I ..\General -I ..\Flash ..\Flash\Flash_Drv.c -o Obj\Flash_Drv.o
if not exist Obj\Flash_Drv.o goto COMPILE_ERR
rem pause

echo Compiling Flash_Main.c
armcc.exe -g -c -errors comp.err -apcs /interwork -I ..\..\Common -I ..\General -I ..\Flash ..\Flash\Flash_Main.c -o Obj\Flash_Main.o
if not exist Obj\Flash_Main.o goto COMPILE_ERR
rem pause

echo Compiling ExceptionHandler.c
armcc.exe -g -c -errors comp.err -I ..\..\Common -I ..\General -I ..\%LCD_Display% ..\General\ExceptionHandler.c -o Obj\ExceptionHandler.o
if not exist Obj\ExceptionHandler.o goto COMPILE_ERR
rem pause

echo Compiling Printf.c
armcc.exe -g -c -errors comp.err -I ..\..\Common -I ..\General -I ..\UART ..\General\Printf.c -o Obj\Printf.o
if not exist Obj\Printf.o goto COMPILE_ERR
rem pause

echo Compiling IOPins.c
armcc.exe -g -c -errors comp.err -I ..\..\Common -I ..\General ..\General\IOPins.c -o Obj\IOPins.o
if not exist Obj\IOPins.o goto COMPILE_ERR
rem pause

echo Compiling LCD_Display.c
armcc.exe -g -c -errors comp.err -I ..\..\Common -I ..\General -I ..\%LCD_Display% ..\%LCD_Display%\LCD_Display.c -o Obj\LCD_Display.o
if not exist Obj\LCD_Display.o goto COMPILE_ERR
rem pause

echo Compiling Main.c
armcc.exe -g -c -errors comp.err -I ..\..\Common -I ..\General -I ..\UART -I ..\Timer -I ..\%LCD_Display% -I ..\Root -I ..\Flash -I ..\EEPROM_I2C -I ..\I2C_PN532_NFC -I ..\VIC -I ..\ExInt ..\Root\Main.c -o Obj\Main.o
if not exist Obj\Main.o goto COMPILE_ERR
rem pause

echo Compiling Timer.c
armcc.exe -g -c -errors comp.err -I ..\..\Common -I ..\General -I ..\Timer -I ..\UART ..\Timer\Timer.c -o Obj\Timer.o
if not exist Obj\Timer.o goto COMPILE_ERR
rem pause

echo Compiling UART_Drv.c
armcc.exe -g -c -errors comp.err -I ..\..\Common -I ..\General -I ..\UART -I ..\Monitor ..\UART\UART_Drv.c -o Obj\UART_Drv.o
if not exist Obj\UART_Drv.o goto COMPILE_ERR
rem pause

echo Compiling Monitor.c
armcc.exe -g -c -errors comp.err -I ..\..\Common -I ..\General -I ..\Monitor -I ..\UART -I ..\Flash -I ..\%LCD_Display% -I ..\EEPROM_I2C ..\Monitor\Monitor.c -o Obj\Monitor.o
if not exist Obj\Monitor.o goto COMPILE_ERR
rem pause

echo Compiling I2C_Main.c
armcc.exe -g -c -errors comp.err -I ..\..\Common -I ..\General -I ..\I2C_Main_Driver ..\I2C_Main_Driver\I2C_Main.c -o Obj\I2C_Main.o
if not exist Obj\I2C_Main.o goto COMPILE_ERR
rem pause

echo Compiling EEPROM_I2C.c
armcc.exe -g -c -errors comp.err -I ..\..\Common -I ..\General -I ..\I2C_Main_Driver -I ..\EEPROM_I2C ..\EEPROM_I2C\EEPROM_I2C.c -o Obj\EEPROM_I2C.o
if not exist Obj\EEPROM_I2C.o goto COMPILE_ERR
rem pause

echo Compiling I2C_PN532_NFC.c
armcc.exe -g -c -errors comp.err -I ..\..\Common -I ..\General -I ..\I2C_Main_Driver -I ..\I2C_PN532_NFC ..\I2C_PN532_NFC\I2C_PN532_NFC.c -o Obj\I2C_PN532_NFC.o
if not exist Obj\I2C_PN532_NFC.o goto COMPILE_ERR
rem pause

echo Compiling VIC_Main.c
armcc.exe -g -c -errors comp.err -I ..\..\Common -I ..\General -I ..\UART -I ..\Timer -I ..\VIC -I ..\ExInt ..\VIC\VIC_Main.c -o Obj\VIC_Main.o
if not exist Obj\VIC_Main.o goto COMPILE_ERR
rem pause

echo Compiling ExInt_Main.c
armcc.exe -g -c -errors comp.err -I ..\..\Common -I ..\General -I ..\ExInt ..\ExInt\ExInt_Main.c -o Obj\ExInt_Main.o
if not exist Obj\ExInt_Main.o goto COMPILE_ERR
rem pause

echo Compiling VectorTable.asm
armasm -g -errors asmcomp.err ..\VectorTable\VectorTable.asm -o Obj\VectorTable.o
if not exist Obj\VectorTable.o goto COMPILE_ERR
rem pause

rem ================================ Link obj files =====================================================
echo Linking ...
armlink.exe -symbols main.sym -errors link.err -scatter scat.txt Obj\VectorTable.o Obj\Global.o Obj\main.o Obj\Flash_Drv.o Obj\Flash_Main.o Obj\IOPins.o Obj\LCD_Display.o Obj\UART_Drv.o Obj\Monitor.o Obj\ExceptionHandler.o Obj\Printf.o Obj\Timer.o Obj\I2C_Main.o Obj\EEPROM_I2C.o Obj\I2C_PN532_NFC.o Obj\VIC_Main.o Obj\ExInt_Main.o -o image.axf
if not exist image.axf goto LINK_ERR

rem ================================ Link obj files =====================================================
echo Create Binary ...
fromelf image.axf -bin Binary_Files
if not exist Binary_Files\ROOT goto BIN_ERR
if not exist Binary_Files\VECTORTABLE_FILE_@0.bin goto BIN_ERR
rename Binary_Files\ROOT BSP@10000.bin
goto END
 
:COMPILE_ERR
echo There are compilation errors !!
goto END

:LINK_ERR
echo There are linker errors !!
goto END

:BIN_ERR
echo Failed to create binaries !!
goto END

:END
pause
