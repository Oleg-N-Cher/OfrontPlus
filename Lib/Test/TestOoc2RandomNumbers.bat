@SET PATH=..\Bin;..\..\Bin;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym

@ofront+ -m TestOoc2RandomNumbers.Mod
@IF errorlevel 1 PAUSE
@CALL gcc.bat TestOoc2RandomNumbers.c -o TestOoc2RandomNumbers.exe

@IF EXIST TestOoc2RandomNumbers.exe TestOoc2RandomNumbers.exe
@PAUSE
