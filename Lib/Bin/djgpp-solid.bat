@REM args:
@REM   LibName ModName

@SET RootBin=%XDev%\Bin
@SET WinDevBin=%XDev%\WinDev\Bin
@SET DJGPP=%XDev%\WinDev\Bin\DJGPP\djgpp.env
@SET PATH=%XDev%\WinDev\Bin\DJGPP\bin;%PATH%
@SET gcc=gcc.exe @..\Bin\djgpp.opt

%gcc% -c %2.c -I "."
@IF errorlevel 1 PAUSE

@ar -rc %1 %2.o
@DEL %2.o
