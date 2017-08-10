@ECHO OFF
SET PATH=..\Bin;%PATH%
SET OBERON=%CD%;%CD%\..\Lib\Sym

ofront+ -m BlitBmp.Mod
IF errorlevel 1 PAUSE
CALL gcc.bat -mwindows BlitBmp.c -o BlitBmp.exe
