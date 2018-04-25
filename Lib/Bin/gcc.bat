@IF NOT /%XDev%==/ GOTO XDev
@ECHO Please set system variable XDev=X:\Path\To\XDev
@PAUSE
@EXIT

:XDev
@SET PATH=%XDev%\WinDev\Bin\MinGW\bin;%PATH%
@SET lib=-I ..\Obj -I ..\Mod ..\Ofront.a
@SET CC=gcc.exe -I . -I ..\Mod -m32 -s -Os -g0 -fvisibility=hidden -finline-small-functions -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-exceptions

%CC% %1 %2 %3 %4 %5 %6 %7 %8 %9 %lib%
@IF errorlevel 1 PAUSE
