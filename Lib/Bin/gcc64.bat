@IF NOT /%XDev%==/ GOTO XDev
@ECHO Please set system variable XDev=X:\Path\To\XDev
@PAUSE
@EXIT

:XDev
@SET PATH=%XDev%\WinDev\Bin\MinGW64\bin;%PATH%
@SET lib=-I ..\Obj64 -I ..\C ..\Ofront64.a
@SET gcc=gcc.exe -s -Os -fno-exceptions -fno-asynchronous-unwind-tables -Wl,--gc-sections -Wl,--relax -Wl,--file-alignment,512 -Wl,--strip-all

%gcc% %1 %2 %3 %4 %5 %6 %7 %8 %9 %lib%
@IF errorlevel 1 PAUSE
