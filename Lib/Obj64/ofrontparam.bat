@IF NOT "%XDev%"=="" GOTO XDev
@ECHO Please set system variable XDev=X:\Path\To\XDev
@PAUSE
@EXIT

:XDev
@SET WinDev=%XDev%\WinDev
@SET PATH=%WinDev%\Bin\MinGW64\bin
@SET gcc=gcc.exe -s -Os -I ..\C -fno-exceptions -fno-asynchronous-unwind-tables

%gcc% ofrontparam.c -o ofrontparam.exe ..\Ofront64.a
@IF errorlevel 1 PAUSE
