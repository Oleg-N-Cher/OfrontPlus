@SET PATH=..\Bin;..\..\Bin;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym

@ofront+ -mC -48 TestReals.Mod
@IF errorlevel 1 PAUSE
@CALL gcc.bat TestReals.c -o TestReals.exe

@TestReals.exe
@PAUSE
