@ECHO OFF
SET PATH=..\Bin;%PATH%
SET OBERON=%CD%;%CD%\..\Lib\Sym64
SET Lib=%XDev%\Ofront\Lib\Ofront64.a %XDev%\WinDev\Bin\MinGW64\lib\libcurldll.a

ofront+ -mC -88 ..\Samples\TestHttp.Mod
IF errorlevel 1 PAUSE
CALL gcc64.bat TestHttp.c -o TestHttp.exe %Lib%
