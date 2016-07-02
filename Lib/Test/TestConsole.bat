@SET PATH=..\Bin;..\..\Bin;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym

@ofront+ -m TestConsole.Mod
@IF errorlevel 1 PAUSE
@CALL gcc.bat TestConsole.c -o TestConsole.exe

@IF EXIST TestConsole.exe TestConsole.exe
@PAUSE
