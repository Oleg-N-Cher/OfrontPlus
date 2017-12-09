@ECHO OFF
SET PATH=..\Bin;%PATH%
SET OBERON=%CD%;%CD%\..\Lib\Sym

ofront+ -mC -48 Mandelbrot.Mod
IF errorlevel 1 PAUSE
CALL gcc.bat -mwindows Mandelbrot.c -o Mandelbrot.exe
