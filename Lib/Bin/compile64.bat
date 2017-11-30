@IF NOT "%XDev%"=="" GOTO XDev
@ECHO Please set system variable XDev=X:\Path\To\XDev
@PAUSE
@EXIT

:XDev
@SET WinDev=%XDev%\WinDev
@SET PATH=%WinDev%\Bin\MinGW64\bin
@SET CC=gcc.exe -I . -I ..\Mod -s -Os -fno-exceptions -fno-asynchronous-unwind-tables -c
@SET Mod=%1
@IF "%Mod%"=="Args2" SET Mod=Args
@IF "%Mod%"=="Platform.Windows" SET Mod=Platform
@IF "%Mod%"=="SYSTEM" SET Mod=..\C\SYSTEM
@IF "%Mod%"=="Types32" EXIT
@IF "%Mod%"=="Types64" SET Mod=Types
@IF NOT "%Mod%"=="WinApi" GOTO MinGW
@DEL WinApi.c
@EXIT

:MinGW
%CC% %Mod%.c
@IF errorlevel 1 PAUSE
PAUSE