@SET PATH=..\Bin;%PATH%
@SET OBERON=%CD%;%CD%\..\Lib\Sym

@ofront+ -m Hello.Mod
@IF errorlevel 1 PAUSE
@CALL gcc.bat Hello.c

@ofront+ -m SameFile.Mod
@IF errorlevel 1 PAUSE
@CALL gcc.bat SameFile.c
