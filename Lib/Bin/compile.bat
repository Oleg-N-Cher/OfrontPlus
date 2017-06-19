@IF NOT "%XDev%"=="" GOTO XDev
@ECHO Please set system variable XDev=X:\Path\To\XDev
@PAUSE
@EXIT

:XDev
@SET WinDev=%XDev%\WinDev
@SET PATH=%WinDev%\Bin\MinGW\bin
@SET CC=gcc.exe -I . -I ..\Mod -s -Os -fno-exceptions -fno-asynchronous-unwind-tables -c
@SET Mod=%1
@IF "%Mod%"=="Args2" SET Mod=Args
@IF "%Mod%"=="Platform.Windows" SET Mod=Platform
@IF "%Mod%"=="SYSTEM" SET Mod=..\C\SYSTEM
@IF NOT "%Mod%"=="WinApi" GOTO MinGW
@DEL WinApi.c
@EXIT

:MinGW
%CC% %Mod%.c
@IF errorlevel 1 PAUSE
