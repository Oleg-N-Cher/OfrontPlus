@ECHO OFF
SET PATH=..\Bin;%PATH%
SET OBERON=%CD%;%CD%\..\Lib\Sym

ofront+ -m Book.Mod
IF errorlevel 1 PAUSE
CALL gcc.bat -mwindows Book.c -o Book.exe
