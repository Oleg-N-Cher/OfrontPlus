@SET PATH=..\Bin;..\..\Bin64;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym64

@ofront+ -mC -88 ..\Test\TestLONGINT.Mod
@IF errorlevel 1 PAUSE
@CALL gcc64.bat TestLONGINT.c -o TestLONGINT.exe

@IF EXIST TestLONGINT.exe TestLONGINT.exe
@PAUSE
