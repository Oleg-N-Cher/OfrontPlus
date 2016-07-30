@SET PATH=..\Bin;..\..\Bin64;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym64

@ofront+ -m ..\Test\TestBYTE.Mod
@IF errorlevel 1 PAUSE
@CALL gcc.bat TestBYTE.c -o TestBYTE.exe

@IF EXIST TestBYTE.exe TestBYTE.exe
@PAUSE
