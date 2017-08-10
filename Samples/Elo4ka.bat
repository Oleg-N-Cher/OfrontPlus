@ECHO OFF
SET PATH=..\Bin;%PATH%
SET OBERON=%CD%;%CD%\..\Lib\Sym

ofront+ -m Elo4ka.Mod
IF errorlevel 1 PAUSE
gcc.bat -mwindows Elo4ka.c -o Elo4ka.exe
