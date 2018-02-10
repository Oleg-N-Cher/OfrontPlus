@ECHO OFF
SET PATH=..\Bin;%PATH%
SET OBERON=%CD%;%CD%\..\Lib\Sym
SET Lib=%XDev%\Ofront\Lib\Ofront.a %XDev%\WinDev\Bin\MinGW\lib\libcurldll.a

ofront+ -mC -48 TestHttp.Mod
IF errorlevel 1 PAUSE
CALL gcc.bat TestHttp.c -oTestHttp.exe %Lib%
