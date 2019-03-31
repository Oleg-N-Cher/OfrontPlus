@ECHO OFF
SET PATH=..\Bin;..\..\Bin;%PATH%
SET OBERON=%CD%;%CD%\..\Sym

ofront+ -mC -48 TestTypesCP48.Mod
IF errorlevel 1 PAUSE
MOVE /Y TestTypesCP.c TestTypesCP48.c > NUL

ofront+ -mC -21 TestTypesCP21.Mod
IF errorlevel 1 PAUSE
MOVE /Y TestTypesCP.c TestTypesCP21.c > NUL
