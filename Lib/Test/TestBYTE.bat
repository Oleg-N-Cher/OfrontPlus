@SET PATH=..\Bin;..\..\Bin;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym

@ofront+ -mC -48 TestBYTE.Mod
@IF errorlevel 1 PAUSE
@CALL gcc.bat TestBYTE.c -o TestBYTE.exe

@IF EXIST TestBYTE.exe TestBYTE.exe
@PAUSE
