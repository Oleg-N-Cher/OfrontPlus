@IF NOT /%XDev%==/ GOTO XDev
@ECHO Please set system variable XDev=X:\Path\To\XDev
@PAUSE
@EXIT

:XDev
@SET PATH=%XDev%\WinDev\Bin\MinGW\bin;%PATH%
@SET lib=-I ..\Lib\Obj -I ..\Lib\Mod ..\Lib\Ofront.a
@SET gcc=gcc.exe -s -Os -fno-exceptions -fno-asynchronous-unwind-tables -Wl,--gc-sections -Wl,--file-alignment,512 %lib%

%gcc% %1 %2 %3 %4 %5 %6 %7 %8 %9
@IF errorlevel 1 PAUSE
