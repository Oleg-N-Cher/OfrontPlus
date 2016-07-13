@IF NOT "%XDev%"=="" GOTO XDev
@ECHO Please set system variable XDev=X:\Path\To\XDev
@PAUSE
@EXIT

:XDev
@IF %1==WinApi DEL WinApi.c

@SET Lib=..\Ofront64.a
@IF EXIST %Lib% DEL %Lib%

:: Ofront+
@CALL ..\Bin\mingw64 %Lib% SYSTEM0
pause
@CALL ..\Bin\mingw64 %Lib% Args
pause
@CALL ..\Bin\mingw64 %Lib% Console
@CALL ..\Bin\mingw %Lib% Platform
@CALL ..\Bin\mingw64 %Lib% Unix
@CALL ..\Bin\mingw64 %Lib% Kernel
@CALL ..\Bin\mingw64 %Lib% Files
@CALL ..\Bin\mingw64 %Lib% Reals
@CALL ..\Bin\mingw64 %Lib% Modules
@CALL ..\Bin\mingw64 %Lib% CmdlnTexts

:: OOC2
@CALL ..\Bin\mingw64 %Lib% ooc2RandomNumbers

@..\Bin\clear
