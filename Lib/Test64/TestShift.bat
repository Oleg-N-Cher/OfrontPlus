@SET PATH=..\Bin;..\..\Bin64;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym64

@ofront+ -m ..\Test\TestShift.Mod
@IF errorlevel 1 PAUSE
@CALL gcc.bat TestShift.c -o TestShift.exe

TestShift.exe
ECHO %errorlevel%
@IF errorlevel 1 ECHO %errorlevel%
@PAUSE
