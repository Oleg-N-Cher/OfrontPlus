@SET PATH=..\Bin;..\..\Bin64;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym64

@ofront+ -mC -88 ..\Test\TestFileSize.Mod
@IF errorlevel 1 PAUSE
@CALL gcc64.bat TestFileSize.c -o TestFileSize.exe

@PAUSE
