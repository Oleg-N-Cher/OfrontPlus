@ECHO OFF
SET PATH=..\..\Bin;%PATH%
SET OBERON=%CD%;%CD%\..\..\Lib\Sym

FOR %%i IN (*.Mod) DO (
  ofront+ -m %%i
  IF errorlevel 1 PAUSE
)

FOR %%i IN (*.c) DO CALL +gcc.bat -mwindows %%i -o ..\%%i.exe
DEL /Q *.c
