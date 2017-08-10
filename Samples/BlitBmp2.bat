@ECHO OFF
SET PATH=..\Bin;%PATH%
SET OBERON=%CD%;%CD%\..\Lib\Sym

ofront+ -m BlitBmp2.Mod
IF errorlevel 1 PAUSE
CALL gcc.bat -mwindows BlitBmp2.c -o BlitBmp2.exe
