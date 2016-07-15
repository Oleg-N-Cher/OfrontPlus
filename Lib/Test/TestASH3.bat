@SET PATH=..\Bin;..\..\Bin;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym

@ofront+ -m TestASH3.Mod
@IF errorlevel 1 PAUSE
@CALL gcc.bat TestASH3.c -o TestASH3.exe

@IF EXIST TestASH3.exe TestASH3.exe
@PAUSE
