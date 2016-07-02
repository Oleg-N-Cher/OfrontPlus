@SET PATH=..\Bin;..\..\Bin;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym64

@ofront+ -m ..\Test\TestUnix.Mod
@IF errorlevel 1 PAUSE
@CALL gcc64.bat TestUnix.c -o TestUnix.exe

@IF EXIST TestUnix.exe TestUnix.exe
@PAUSE