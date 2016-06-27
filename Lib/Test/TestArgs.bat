@SET PATH=..\Bin;..\..\Bin;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym

@ofront+ -m TestArgs.Mod
@IF errorlevel 1 PAUSE
@CALL gcc.bat TestArgs.c -o TestArgs.exe

@IF EXIST TestArgs.exe TestArgs.exe arg1 "ARG 2" "__!@#$_arg3_&%*()-="
@PAUSE
