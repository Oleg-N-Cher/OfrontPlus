@ECHO OFF
IF EXIST ..\Lib\Ofront.a GOTO Lib
CD ..\Lib\Bin
CALL build.bat
CD ..\..\Obj

:Lib

CALL ..\Bin\o2c.bat
CALL ..\Bin\cc.bat
