@REM args:
@REM   LibName ModName

@SET RootBin=..\..\Bin
@SET WinDevBin=..\..\WinDev\Bin
@SET tcc=%WinDevBin%\tcc\tcc.exe

%tcc% -c %2.c -I "."
@IF errorlevel 1 PAUSE

@%WinDevBin%\ar -rc %1 %2.o
@Bin\clear