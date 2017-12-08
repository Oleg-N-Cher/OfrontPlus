@ECHO OFF
SET PATH=..\Bin;%PATH%
SET OBERON=%CD%;%CD%\..\Lib\Sym64

ofront+ -m Mandelbrot2.Mod
IF errorlevel 1 PAUSE
CALL gcc64.bat -mwindows Mandelbrot2.c -o Mandelbrot2.exe
