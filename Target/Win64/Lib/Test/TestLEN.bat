@SET PATH=..\Bin;..\..\Bin64;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym64

@ofront+ -mC -88 ..\Test\TestLEN.Mod
@IF errorlevel 1 PAUSE
@CALL gcc64.bat TestLEN.c -o TestLEN.exe

TestLEN.exe
@PAUSE
