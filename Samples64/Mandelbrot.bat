@ECHO OFF
SET PATH=..\Bin;%PATH%
SET OBERON=%CD%;%CD%\..\Lib\Sym64

ofront+ -mC -88 Mandelbrot.Mod
IF errorlevel 1 PAUSE
CALL gcc64.bat -mwindows Mandelbrot.c -o Mandelbrot.exe
