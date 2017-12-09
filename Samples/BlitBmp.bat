@ECHO OFF
SET PATH=..\Bin;%PATH%
SET OBERON=%CD%;%CD%\..\Lib\Sym

ofront+ -mC -48 BlitBmp.Mod
IF errorlevel 1 PAUSE
CALL gcc.bat -mwindows BlitBmp.c -o BlitBmp.exe
