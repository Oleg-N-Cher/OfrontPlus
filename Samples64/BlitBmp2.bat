@ECHO OFF
SET PATH=..\Bin;%PATH%
SET OBERON=%CD%;%CD%\..\Lib\Sym64

ofront+ -m BlitBmp2.Mod
IF errorlevel 1 PAUSE
CALL gcc64.bat -mwindows BlitBmp2.c -o BlitBmp2.exe
