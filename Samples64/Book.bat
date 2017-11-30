@ECHO OFF
SET PATH=..\Bin;%PATH%
SET OBERON=%CD%;%CD%\..\Lib\Sym64

ofront+ -m Book.Mod
IF errorlevel 1 PAUSE
CALL gcc64.bat -mwindows Book.c -o Book.exe
