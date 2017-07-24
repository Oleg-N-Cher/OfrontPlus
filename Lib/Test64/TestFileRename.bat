@SET PATH=..\Bin;..\..\Bin64;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym64

@ofront+ -m ..\Test\TestFileRename.Mod
@IF errorlevel 1 PAUSE
@CALL gcc64.bat TestFileRename.c -o TestFileRename.exe

@PAUSE
