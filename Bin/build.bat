@IF /%XDev%==/ GOTO NoXDev

@SET WinDev=%XDev%\WinDev
@SET PATH=%WinDev%\Bin\MinGW\bin;%PATH%
@SET lib=-I ..\Lib\Obj ..\Lib\Ofront.a
@SET gcc=gcc.exe -s -Os -fno-exceptions -fno-asynchronous-unwind-tables -Wl,--gc-sections -Wl,--file-alignment,512 %lib%

%gcc% OfrontOPM.c OfrontOPS.c OfrontOPT.c OfrontOPB.c OfrontOPP.c OfrontOPC.c OfrontOPV.c OfrontCmd.c %lib% -o ..\Bin\ofront.exe
@IF errorlevel 1 PAUSE
%gcc% OfrontOPM.c OfrontOPS.c OfrontOPT.c OfrontOPC.c OfrontOPV.c BrowserCmd.c %lib% -o ..\Bin\showdef.exe
@IF errorlevel 1 PAUSE
%gcc% OCatCmd.c %lib% -o ..\Bin\ocat.exe
@IF errorlevel 1 PAUSE
::..\Bin\clear
@EXIT

:NoXDev

@ECHO Please set system variable XDev=X:\Path\To\XDev
@PAUSE
