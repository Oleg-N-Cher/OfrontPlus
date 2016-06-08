@IF /%XDev%==/ GOTO NoXDev

@SET WinDev=%XDev%\WinDev
@SET PATH=%WinDev%\Bin\MinGW\bin;%PATH%
@SET gcc=gcc.exe -s -Os -fno-exceptions -fno-asynchronous-unwind-tables

@SET Mod=%1
@IF %Mod%==Unix.msvcrt SET Mod=Unix
@IF NOT %Mod%==WinApi GOTO MinGW
@DEL WinApi.c
@EXIT

MinGW:

%gcc% -c %Mod%.c -I "."
@IF errorlevel 1 PAUSE
