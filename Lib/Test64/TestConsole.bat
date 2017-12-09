@SET PATH=..\Bin;..\..\Bin64;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym64

@ofront+ -mC -88 ..\Test\TestConsole.Mod
@IF errorlevel 1 PAUSE
@CALL gcc64.bat TestConsole.c -o TestConsole.exe

@IF EXIST TestConsole.exe TestConsole.exe
@PAUSE
