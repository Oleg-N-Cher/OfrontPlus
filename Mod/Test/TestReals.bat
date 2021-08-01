@SET PATH=..\Bin;..\..;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym

@ofront+ -sC -48 TestReals.Mod
@ECHO %errorlevel%
@IF errorlevel 1 PAUSE
@showdef TestReals

@ofront+ -mC -48 TestReals.Mod
@CALL gcc.bat TestReals.c -o TestReals.exe

@TestReals.exe
@PAUSE
