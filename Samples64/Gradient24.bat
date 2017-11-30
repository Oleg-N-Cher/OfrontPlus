@ECHO OFF
SET PATH=..\Bin;%PATH%
SET OBERON=%CD%;%CD%\..\Lib\Sym64

ofront+ -m Gradient24.Mod
IF errorlevel 1 PAUSE
CALL gcc64.bat -mwindows Gradient24.c -o Gradient24.exe
