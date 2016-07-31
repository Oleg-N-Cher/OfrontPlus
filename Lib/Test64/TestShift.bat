@SET PATH=..\Bin;..\..\Bin64;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym64

@ofront+ -m ..\Test\TestShift.Mod
@IF errorlevel 1 PAUSE
@CALL gcc64.bat TestShift.c -o TestShift.exe

TestShift.exe
ECHO %errorlevel%
@IF errorlevel 1 ECHO %errorlevel%
@PAUSE
