@ECHO OFF
SET PATH=..\Bin;%PATH%
SET OBERON=%CD%;%CD%\..\Lib\Sym64

ofront+ -mC -88 Elo4ka.Mod
IF errorlevel 1 PAUSE
gcc64.bat -mwindows Elo4ka.c -o Elo4ka.exe
