@REM args:
@REM   LibName ModName

@SET WinDev=%XDev%\WinDev
@SET PATH=%WinDev%\Bin\MinGW\bin;%PATH%
@SET gcc=gcc.exe -s -Os -fno-exceptions -fno-asynchronous-unwind-tables

%gcc% -c %2.c -I "."
@IF errorlevel 1 PAUSE

@ar.exe -rc %1 %2.o
@DEL %2.o
