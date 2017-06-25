@SET PATH=..\Bin;..\..\Bin;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym

@ofront+ -m TestFileSize.Mod
@IF errorlevel 1 PAUSE
@CALL gcc.bat TestFileSize.c -o TestFileSize.exe

@PAUSE
