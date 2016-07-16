@SET PATH=..\Bin;..\..\Bin;%PATH%
@SET OBERON=%CD%;%CD%\..\Sym32L64

@ofront+ -m ..\Test\TestArgs.Mod
@IF errorlevel 1 PAUSE
@CALL gcc32L64.bat TestArgs.c -o TestArgs.exe

@IF EXIST TestArgs.exe TestArgs.exe arg1 "ARG 2" "__!@#$_arg3_&%*()-="
@PAUSE
