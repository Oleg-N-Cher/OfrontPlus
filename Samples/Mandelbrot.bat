@ECHO OFF
SET PATH=..\Bin;%PATH%
SET OBERON=%CD%;%CD%\..\Lib\Sym

ofront+ -m Mandelbrot.Mod
IF errorlevel 1 PAUSE
CALL gcc.bat -mwindows Mandelbrot.c -o Mandelbrot.exe
