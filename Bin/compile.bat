@IF NOT "%XDev%"=="" GOTO XDev
@ECHO Please set system variable XDev=X:\Path\To\XDev
@PAUSE
@EXIT

:XDev
@SET WinDev=%XDev%\WinDev
@SET PATH=%WinDev%\Bin\MinGW\bin
@SET lib=-I..\Lib\Obj -I..\Lib\Mod
@SET CC=gcc.exe -m32 -Os -g0 -fvisibility=hidden -fomit-frame-pointer -finline-small-functions -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables %lib%

@SET Mod=%1
@IF %Mod%==OfrontOPM.cmdln SET Mod=OfrontOPM

%CC% -c %Mod%.c
@IF errorlevel 1 PAUSE
