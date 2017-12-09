@SET PATH=..\Bin;..\..\Bin;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym

@ofront+ -mC -48 TestASH.Mod
@IF errorlevel 1 PAUSE
@CALL gcc.bat TestASH.c -o TestASH.exe
