@SET PATH=..\Bin;..\..\Bin64;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym64

@ofront+ -mC -88 ..\Test\TestASH3.Mod
@IF errorlevel 1 PAUSE
@CALL gcc64.bat TestASH3.c -o TestASH3.exe

@IF EXIST TestASH3.exe TestASH3.exe
@PAUSE
