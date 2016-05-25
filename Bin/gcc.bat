@IF /%XDev%==/ GOTO NoXDev

@SET WinDev=%XDev%\WinDev
@SET PATH=%WinDev%\Bin\MinGW\bin;%PATH%
@SET lib=-I ..\Lib\Obj
@SET gcc=gcc.exe -s -Os -fno-exceptions -fno-asynchronous-unwind-tables -Wl,--file-alignment,512 %lib%

%gcc% %1 ..\Lib\Ofront.a -o %1.exe
@IF errorlevel 1 PAUSE
@EXIT

:NoXDev

@ECHO Please set system variable XDev=X:\Path\To\XDev
@PAUSE
