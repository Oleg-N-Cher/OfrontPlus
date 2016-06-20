@IF NOT "%XDev%"=="" GOTO XDev
@ECHO Please set system variable XDev=X:\Path\To\XDev
@PAUSE
@EXIT

:XDev
@SET WinDev=%XDev%\WinDev
@SET PATH=%WinDev%\Bin\MinGW\bin;%PATH%
@SET lib=-I ..\Lib\Obj
@SET gcc=gcc.exe -Os -fno-exceptions -fno-asynchronous-unwind-tables %lib%

@SET Mod=%1
@IF %Mod%==OfrontOPM.cmdln SET Mod=OfrontOPM

%gcc% -c %Mod%.c
@IF errorlevel 1 PAUSE
