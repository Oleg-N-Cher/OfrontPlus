@SET PATH=..\Bin;..\..\Bin;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym32L64

@ofront+ -m ..\Test\TestOoc2RandomNumbers.Mod
@IF errorlevel 1 PAUSE
@CALL gcc32L64.bat TestOoc2RandomNumbers.c -o TestOoc2RandomNumbers.exe

@IF EXIST TestOoc2RandomNumbers.exe TestOoc2RandomNumbers.exe
@PAUSE
