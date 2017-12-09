@ECHO OFF
SET PATH=..\Bin;%PATH%
SET OBERON=%CD%;%CD%\..\Lib\Sym64

ofront+ -mC -88 BlitBmp2.Mod
IF errorlevel 1 PAUSE
CALL gcc64.bat -mwindows BlitBmp2.c -o BlitBmp2.exe
