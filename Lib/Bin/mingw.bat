@REM args:
@REM   LibName ModName

@SET WinDev=%XDev%\WinDev
@SET PATH=%WinDev%\Bin\MinGW\bin;%PATH%
@SET gcc=gcc.exe -s -Os -fno-exceptions -fno-asynchronous-unwind-tables

@SET Mod=%2
@IF %2==SYSTEM0 SET Mod=..\C\SYSTEM0

%gcc% -c %Mod%.c -I "." -I ..\C
@IF errorlevel 1 PAUSE

@ar.exe -rc %1 %2.o
@DEL %2.o
