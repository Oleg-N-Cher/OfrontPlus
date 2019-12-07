@SET PATH=..\Bin;..\..\Bin;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym

@ofront+ -mC -48 TestShift.Mod
@IF errorlevel 1 PAUSE
@CALL gcc.bat TestShift.c -o TestShift.exe

TestShift.exe
ECHO %errorlevel%
@IF errorlevel 1 ECHO %errorlevel%
@PAUSE
