@ECHO OFF
IF NOT "%XDev%"=="" GOTO XDev
ECHO Please set system variable XDev=X:\Path\To\XDev
PAUSE
EXIT

:XDev
SET PATH=%XDev%\WinDev\Bin\MinGW\bin
SET CC=gcc.exe -I. -I..\Mod -m32 -s -Os -g0 -fvisibility=hidden -finline-small-functions -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-exceptions -c
SET Mod=%1
IF "%Mod%"=="Args2" SET Mod=Args
IF "%Mod%"=="Platform.Windows" SET Mod=Platform
IF "%Mod%"=="SYSTEM" SET Mod=..\C\SYSTEM
IF "%Mod%"=="Types32" SET Mod=Types
IF "%Mod%"=="Types64" EXIT
IF NOT "%Mod%"=="WinApi" GOTO MinGW
DEL WinApi.c
EXIT

:MinGW
%CC% %Mod%.c
IF errorlevel 1 PAUSE
