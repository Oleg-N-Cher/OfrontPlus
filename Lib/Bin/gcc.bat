@IF NOT /%XDev%==/ GOTO XDev
@ECHO Please set system variable XDev=X:\Path\To\XDev
@PAUSE
@EXIT

:XDev
@SET PATH=%XDev%\WinDev\Bin\MinGW\bin;%PATH%
@SET lib=-I ..\Obj -I ..\C ..\Ofront.a
@SET gcc=gcc.exe -s -Os -fno-exceptions -fno-asynchronous-unwind-tables -Wl,--gc-sections -Wl,--file-alignment,512

%gcc% %1 %2 %3 %4 %5 %6 %7 %8 %9 %lib% 2>&1|more
@IF errorlevel 1 PAUSE
