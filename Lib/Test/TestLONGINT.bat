@SET PATH=..\Bin;..\..\Bin;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym

@ofront+ -m TestLONGINT.Mod
@IF errorlevel 1 PAUSE
@CALL gcc.bat TestLONGINT.c -o TestLONGINT.exe

@IF EXIST TestLONGINT.exe TestLONGINT.exe
@PAUSE
