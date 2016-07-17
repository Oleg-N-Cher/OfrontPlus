@IF NOT "%XDev%"=="" GOTO XDev
@ECHO Please set system variable XDev=X:\Path\To\XDev
@PAUSE
@EXIT

:XDev
@SET WinDev=%XDev%\WinDev
@SET PATH=%WinDev%\Bin\MinGW64\bin;%PATH%
@SET gcc=gcc.exe -s -Os -fno-exceptions -fno-asynchronous-unwind-tables

@SET Mod=%1
@IF "%Mod%"=="Args2" SET Mod=Args
@IF "%Mod%"=="Files.WinApi" SET Mod=Files
@IF "%Mod%"=="Kernel.WinApi" SET Mod=Kernel
@IF "%Mod%"=="Platform.Windows" SET Mod=Platform
@IF "%Mod%"=="SYSTEM" SET Mod=..\C\SYSTEM
@IF NOT "%Mod%"=="WinApi" GOTO MinGW
@DEL WinApi.c
@EXIT

:MinGW
%gcc% -c %Mod%.c -I "." -I ..\C
@IF errorlevel 1 PAUSE
PAUSE