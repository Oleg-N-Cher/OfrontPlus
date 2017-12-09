@ECHO OFF
SET PATH=..\Bin;%PATH%
SET OBERON=%CD%;%CD%\..\Lib\Sym

ofront+ -mC -48 Gradient24.Mod
IF errorlevel 1 PAUSE
CALL gcc.bat -mwindows Gradient24.c -o Gradient24.exe
