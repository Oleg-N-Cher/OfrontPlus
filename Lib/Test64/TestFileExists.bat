@SET PATH=..\Bin;..\..\Bin64;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym64

@ofront+ -m ..\Test\TestFileExists.Mod
@IF errorlevel 1 PAUSE
@CALL gcc64.bat TestFileExists.c -o TestFileExists.exe

@PAUSE
