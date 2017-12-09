@ECHO OFF
SET PATH=..\Bin;%PATH%
SET OBERON=%CD%;%CD%\..\Lib\Sym

ofront+ -mC -48 Mandelbrot2.Mod
IF errorlevel 1 PAUSE
CALL gcc.bat -mwindows Mandelbrot2.c -o Mandelbrot2.exe
