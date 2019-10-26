@SET PATH=..\Bin;..\..\Bin;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym

@ofront+ -m3 -48 TestASH.Mod
@IF errorlevel 1 PAUSE
@CALL gcc.bat TestASH.c -o TestASH.exe
