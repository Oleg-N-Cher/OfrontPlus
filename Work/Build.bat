@SET PATH=..\Bin;%PATH%
@SET OBERON=%CD%;%CD%\..\Lib\Sym

@ofront+ -mC -48 Hello.Mod
PAUSE
@IF errorlevel 1 PAUSE
@CALL gcc.bat Hello.c

@ofront+ -mC -48 SameFile.Mod
@IF errorlevel 1 PAUSE
@CALL gcc.bat SameFile.c

@ofront+ -mC -48 Test.Mod
@IF errorlevel 1 PAUSE
@CALL gcc.bat Test.c
