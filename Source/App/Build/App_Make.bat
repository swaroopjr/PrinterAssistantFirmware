@echo off

rem ================================ Start Compilation =====================================================
echo Compiling

echo Compiling Global.c
armcc.exe -g -c -errors comp.err -I ..\..\Common ..\..\Common\Global.c -o Obj\Global.o
if not exist Obj\Global.o goto COMPILE_ERR
rem pause

echo Compiling AppMain.c
armcc.exe -g -c -errors comp.err -I ..\..\Common -I ..\AppMain ..\AppMain\AppMain.c -o Obj\AppMain.o
if not exist Obj\AppMain.o goto COMPILE_ERR
rem pause


rem ================================ Link obj files =====================================================
echo Linking ...
armlink.exe -symbols AppMain.sym -errors link.err -scatter App_Scat.txt Obj\Global.o Obj\AppMain.o -o image.axf
if not exist image.axf goto LINK_ERR

rem ================================ Link obj files =====================================================
echo Create Binary ...
fromelf image.axf -bin Binary_Files
if not exist Binary_Files\ROOT goto BIN_ERR
rename Binary_Files\ROOT APP@20000.bin
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

