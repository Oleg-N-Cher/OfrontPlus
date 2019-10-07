@ECHO OFF
IF "%1"=="" CALL o2c64.bat

IF NOT "%XDev%"=="" GOTO XDev
ECHO Please set system variable XDev=X:\Path\To\XDev
PAUSE
EXIT

:XDev
SET PATH=%XDev%\WinDev\Bin\MinGW64\bin
SET lib=-I..\Lib\Obj64 -I..\Lib\Mod ..\Lib\Ofront64.a
SET StripExe=-nostartfiles ..\Lib\Mod\crt1.c -Wl,-e_WinMain@16 -D_WINMAIN
SET CC=gcc.exe %StripExe% -m64 -s -Os -g0 -fvisibility=hidden -fomit-frame-pointer -finline-small-functions -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -Wl,--gc-sections

:Browser
IF NOT "%1"=="BrowserCmd" GOTO ImportGraph
%CC% BrowserCmd.c OfrontErrors.c OfrontOPM.c OfrontOPS.c OfrontOPT.c OfrontOPC.c OfrontOPV.c %lib% -o..\Bin64\showdef.exe -ffunction-sections
IF errorlevel 1 PAUSE
EXIT

:ImportGraph
IF NOT "%1"=="ImportGraph" GOTO OCat
%CC% ImportGraph.c %lib% -o..\Bin64\graphviz.exe -ffunction-sections
IF errorlevel 1 PAUSE
EXIT

:OCat
IF NOT "%1"=="OCatCmd" GOTO Ofront
%CC% OCatCmd.c %lib% -o..\Bin64\ocat.exe -ffunction-sections
IF errorlevel 1 PAUSE
EXIT

:Ofront
IF NOT "%1"=="OfrontCmd" GOTO Watson
%CC% OfrontCmd.c OfrontErrors.c OfrontOPM.c OfrontOPS.c OfrontOPT.c OfrontOPB.c OfrontOPP.c OfrontOPC.c OfrontOPV.c %lib% -o..\Bin64\ofront+.exe -ffunction-sections
IF errorlevel 1 PAUSE
EXIT

:Watson
IF NOT "%1"=="WatsonCmd" GOTO BuildAll
%CC% Watson0.c WatsonCmd.c OfrontErrors.c OfrontOPM.c OfrontOPS.c OfrontOPT.c OfrontOPC.c OfrontOPV.c %lib% -o..\Bin64\watson.exe -ffunction-sections
IF errorlevel 1 PAUSE
EXIT

:BuildAll
%CC% BrowserCmd.c OfrontErrors.c OfrontOPM.c OfrontOPS.c OfrontOPT.c OfrontOPC.c OfrontOPV.c %lib% -o..\Bin64\showdef.exe -ffunction-sections
IF errorlevel 1 PAUSE
%CC% ImportGraph.c %lib% -o..\Bin64\graphviz.exe -ffunction-sections
IF errorlevel 1 PAUSE
%CC% OCatCmd.c %lib% -o..\Bin64\ocat.exe -ffunction-sections
IF errorlevel 1 PAUSE
%CC% OfrontCmd.c OfrontErrors.c OfrontOPM.c OfrontOPS.c OfrontOPT.c OfrontOPB.c OfrontOPP.c OfrontOPC.c OfrontOPV.c %lib% -o..\Bin64\ofront+.exe -ffunction-sections
IF errorlevel 1 PAUSE
::%CC% Watson0.c WatsonCmd.c OfrontErrors.o OfrontOPM.o OfrontOPS.o OfrontOPT.o OfrontOPC.o OfrontOPV.o %lib% -o..\Bin64\watson.exe -ffunction-sections
::IF errorlevel 1 PAUSE
