@ECHO OFF
IF NOT "%XDev%"=="" SET PATH=%XDev%\WinDev\Bin\MinGW\bin;%PATH%
SET OBERON=%CD%;%CD%\..\Lib\Sym

..\ofront+.exe -m48 ..\..\..\Mod\Test\TestArgs.ob3
IF errorlevel 1 PAUSE

SET StripExe=-nostartfiles ..\..\..\Mod\Lib\crt1.c -Wl,-e_WinMain
SET LIB=-I..\Lib\Obj -I..\..\..\Mod\Lib ..\Lib\Ofront.a
SET CC=gcc.exe -m32 -s -Os -g0 -fvisibility=hidden -fomit-frame-pointer -finline-small-functions -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -Wl,--gc-sections
%CC% %StripExe% TestArgs.c -oTestArgs.exe %LIB%
IF errorlevel 1 PAUSE

IF EXIST TestArgs.exe TestArgs.exe arg1 "ARG 2" "__!@#$_arg3_&%*()-="
PAUSE
