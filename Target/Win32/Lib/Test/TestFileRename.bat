@SET PATH=..\Bin;..\..\Bin;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym

@ofront+ -mC -48 TestFileRename.Mod
@IF errorlevel 1 PAUSE
@CALL gcc.bat TestFileRename.c -o TestFileRename.exe

@PAUSE
