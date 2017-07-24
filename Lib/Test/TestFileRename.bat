@SET PATH=..\Bin;..\..\Bin;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym

@ofront+ -m TestFileRename.Mod
@IF errorlevel 1 PAUSE
@CALL gcc.bat TestFileRename.c -o TestFileRename.exe

@PAUSE
