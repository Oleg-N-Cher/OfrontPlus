@SET PATH=..\Bin;..\..\Bin;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym

@ofront+ -m TestASH.Mod
@IF errorlevel 1 PAUSE
@CALL gcc.bat TestASH.c -o TestASH.exe
