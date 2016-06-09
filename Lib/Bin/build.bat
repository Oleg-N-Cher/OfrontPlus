@IF /%XDev%==/ GOTO NoXDev

@IF %1==WinApi DEL WinApi.c

:MinGW

@SET Lib=..\Ofront.a
@IF EXIST %Lib% DEL %Lib%
@CALL ..\Bin\mingw %Lib% SYSTEM0
@CALL ..\Bin\mingw %Lib% Args
@CALL ..\Bin\mingw %Lib% Console
@CALL ..\Bin\mingw %Lib% Unix
@CALL ..\Bin\mingw %Lib% Kernel
@CALL ..\Bin\mingw %Lib% Files
@CALL ..\Bin\mingw %Lib% Reals
@CALL ..\Bin\mingw %Lib% Modules
@CALL ..\Bin\mingw %Lib% CmdlnTexts
@..\Bin\clear

:NoXDev

@ECHO Please set system variable XDev=X:\Path\To\XDev
@PAUSE
