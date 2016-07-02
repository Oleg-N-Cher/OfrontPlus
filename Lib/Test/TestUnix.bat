@SET PATH=..\Bin;..\..\Bin;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym

@ofront+ -m TestUnix.Mod
@IF errorlevel 1 PAUSE
@CALL gcc.bat TestUnix.c -o TestUnix.exe

@IF EXIST TestUnix.exe TestUnix.exe
@PAUSE
