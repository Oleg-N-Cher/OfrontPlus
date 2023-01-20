@ECHO OFF
IF NOT "%XDev%"=="" SET PATH=%XDev%\WinDev\Bin\MinGW\bin
CD "%~dp0"
CD ..\Obj
SET StripExe=-nostartfiles ..\..\..\Mod\Lib\crt1.c -Wl,-e_WinMain@16
SET CC=gcc.exe %StripExe% -I..\Lib\Obj -I..\..\..\Mod\Lib -m32 -s -Os -g0 -fvisibility=hidden -fomit-frame-pointer -finline-small-functions -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -Wl,--gc-sections -Wfatal-errors

%CC% -c OfrontErrors.c OfrontOPM.c OfrontOPS.c OfrontOPT.c OfrontOPB.c OfrontOPP.c OfrontOPC.c OfrontOPV.c
IF errorlevel 1 PAUSE

:: Ofront+

%CC% OfrontCmd.c OfrontErrors.o OfrontOPM.o OfrontOPS.o OfrontOPT.o OfrontOPB.o OfrontOPP.o OfrontOPC.o OfrontOPV.o ..\Lib\Ofront.a -o..\ofront+.exe
IF errorlevel 1 PAUSE

:: Browser

%CC% BrowserCmd.c OfrontErrors.o OfrontOPM.o OfrontOPS.o OfrontOPT.o OfrontOPC.o OfrontOPV.o ..\Lib\Ofront.a -o..\showdef.exe
IF errorlevel 1 PAUSE

:: ImportGraph

%CC% ImportGraph.c ..\Lib\Ofront.a -o..\graphviz.exe
IF errorlevel 1 PAUSE

:: OCat

%CC% OCatCmd.c ..\Lib\Ofront.a -o..\ocat.exe
IF errorlevel 1 PAUSE

:: OEF & OMake

%CC% OEF.c ..\Lib\Ofront.a -o..\oef.exe
IF errorlevel 1 PAUSE
%CC% OMake.c ..\Lib\Ofront.a -o..\omake.exe
IF errorlevel 1 PAUSE

:: OdcToText

%CC% OdcToText.c ..\Lib\Ofront.a -o..\odc2txt.exe
IF errorlevel 1 PAUSE

DEL *.o
