@SET PATH=..\Bin;..\..\Bin64;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym64

@ofront+ -m ..\Test\TestASH2.Mod
@IF errorlevel 1 PAUSE
@CALL gcc64.bat TestASH2.c -o TestASH2.exe

@IF EXIST TestASH2.exe TestASH2.exe
@PAUSE
