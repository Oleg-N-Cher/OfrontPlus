@SET PATH=..\Bin;..\..\Bin64;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym64

@ofront+ -mC -88 ..\Test\TestBYTE.Mod
@IF errorlevel 1 PAUSE
@CALL gcc64.bat TestBYTE.c -o TestBYTE.exe

@IF EXIST TestBYTE.exe TestBYTE.exe
@PAUSE
