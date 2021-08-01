@SET PATH=..\Bin;..\..\Bin;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym

@ofront+ -mC -48 TestFileExists.Mod
@IF errorlevel 1 PAUSE
@CALL gcc.bat TestFileExists.c -o TestFileExists.exe

@PAUSE
