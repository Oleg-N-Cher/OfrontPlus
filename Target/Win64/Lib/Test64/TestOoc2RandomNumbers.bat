@SET PATH=..\Bin;..\..\Bin64;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym64

@ofront+ -mC -88 ..\Test\TestOoc2RandomNumbers.Mod
@IF errorlevel 1 PAUSE
@CALL gcc64.bat TestOoc2RandomNumbers.c -o TestOoc2RandomNumbers.exe

@IF EXIST TestOoc2RandomNumbers.exe TestOoc2RandomNumbers.exe
@PAUSE
