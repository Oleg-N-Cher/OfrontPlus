@SET PATH=..\Bin;..\..\Bin;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym64

@ofront+ -m ..\Test\TestConsole.Mod
@IF errorlevel 1 PAUSE
@CALL gcc64.bat TestConsole.c -o TestConsole.exe

@IF EXIST TestConsole.exe TestConsole.exe
@PAUSE
