@SET PATH=..\Bin;..\..\Bin64;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym64

@ofront+ -m TestFiles.Mod
@IF errorlevel 1 PAUSE
@CALL gcc64.bat TestFiles.c -o TestFiles.exe

@IF EXIST TestFiles.exe TestFiles.exe
@PAUSE
