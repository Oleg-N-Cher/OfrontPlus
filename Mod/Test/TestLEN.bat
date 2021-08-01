@SET PATH=..\Bin;..\..\Bin;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym

@ofront+ -mC -48 TestLEN.Mod
@IF errorlevel 1 PAUSE
@CALL gcc.bat TestLEN.c -o TestLEN.exe

TestLEN.exe
@PAUSE
