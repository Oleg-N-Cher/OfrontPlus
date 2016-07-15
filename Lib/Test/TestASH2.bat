@SET PATH=..\Bin;..\..\Bin;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym

@ofront+ -m TestASH2.Mod
@IF errorlevel 1 PAUSE
@CALL gcc.bat TestASH2.c -o TestASH2.exe

@IF EXIST TestASH2.exe TestASH2.exe
@PAUSE
