@IF NOT /%XDev%==/ GOTO XDev
@ECHO Please set system variable XDev=X:\Path\To\XDev
@PAUSE
@EXIT

:XDev
@SET PATH=%XDev%\WinDev\Bin\MinGW64\bin;%PATH%
@SET lib=-I ..\Obj64 -I ..\Mod ..\Ofront64.a
@SET CC=gcc.exe -I . -I ..\Mod -m64 -s -Os -g0 -fvisibility=hidden -fomit-frame-pointer -finline-small-functions -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-exceptions 

%CC% %1 %2 %3 %4 %5 %6 %7 %8 %9 %lib%
@IF errorlevel 1 PAUSE
