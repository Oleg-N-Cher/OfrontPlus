@IF NOT "%XDev%"=="" GOTO XDev
@ECHO Please set system variable XDev=X:\Path\To\XDev
@PAUSE
@EXIT

:XDev
@SET WinDev=%XDev%\WinDev
@SET PATH=%WinDev%\Bin\MinGW64\bin
@SET lib=-I ..\Lib\Obj64 -I ..\Lib\Mod ..\Lib\Ofront64.a
@SET gcc=gcc.exe -s -Os -fno-exceptions -fno-asynchronous-unwind-tables -Wl,--gc-sections -Wl,--file-alignment,512 %lib%

:Browser
@IF NOT %1==BrowserCmd GOTO OCat
%gcc% OfrontErrors.c OfrontOPM.c OfrontOPS.c OfrontOPT.c OfrontOPC.c OfrontOPV.c BrowserCmd.c %lib% -o ..\Bin64\showdef.exe
@IF errorlevel 1 PAUSE
@EXIT

:OCat
@IF NOT %1==OCatCmd GOTO Ofront
%gcc% OCatCmd.c %lib% -o ..\Bin64\ocat.exe
@IF errorlevel 1 PAUSE
@EXIT

:Ofront
@IF NOT %1==OfrontCmd GOTO Watson
%gcc% OfrontErrors.c OfrontOPM.c OfrontOPS.c OfrontOPT.c OfrontOPB.c OfrontOPP.c OfrontOPC.c OfrontOPV.c OfrontCmd.c %lib% -o ..\Bin64\ofront+.exe
@IF errorlevel 1 PAUSE
@EXIT

:Watson
@IF NOT %1==WatsonCmd GOTO BuildAll
%gcc% OfrontErrors.c OfrontOPM.c OfrontOPS.c OfrontOPT.c OfrontOPC.c OfrontOPV.c Watson0.c WatsonCmd.c %lib% -o ..\Bin64\watson.exe
@IF errorlevel 1 PAUSE
@EXIT

:BuildAll
%gcc% OfrontErrors.c OfrontOPM.c OfrontOPS.c OfrontOPT.c OfrontOPC.c OfrontOPV.c BrowserCmd.c %lib% -o ..\Bin64\showdef.exe
@IF errorlevel 1 PAUSE
%gcc% OCatCmd.c %lib% -o ..\Bin64\ocat.exe
@IF errorlevel 1 PAUSE
%gcc% OfrontErrors.c OfrontOPM.c OfrontOPS.c OfrontOPT.c OfrontOPB.c OfrontOPP.c OfrontOPC.c OfrontOPV.c OfrontCmd.c %lib% -o ..\Bin64\ofront+.exe
@IF errorlevel 1 PAUSE
%gcc% OfrontErrors.c OfrontOPM.c OfrontOPS.c OfrontOPT.c OfrontOPC.c OfrontOPV.c Watson0.c WatsonCmd.c %lib% -o ..\Bin64\watson.exe
@IF errorlevel 1 PAUSE
