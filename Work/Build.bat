@SET PATH=..\Bin;%PATH%
@SET OBERON=%CD%;%CD%\..\Lib\Sym

@ofront -m Hello.Mod
@IF errorlevel 1 PAUSE
@CALL gcc Hello.c
