@IF /%XDev%==/ GOTO NoXDev

@IF %1==WinApi DEL WinApi.c

:MinGW

@SET Lib=..\Ofront.a
@IF EXIST %Lib% DEL %Lib%
@CALL ..\Bin\mingw %Lib% SYSTEM0
@CALL ..\Bin\mingw %Lib% Args
@CALL ..\Bin\mingw %Lib% Console
@CALL ..\Bin\mingw %Lib% Unix -noinit
@CALL ..\Bin\mingw %Lib% Kernel
@CALL ..\Bin\mingw %Lib% Files
@CALL ..\Bin\mingw %Lib% Reals -noinit
@CALL ..\Bin\mingw %Lib% Modules -noinit
@CALL ..\Bin\mingw %Lib% CmdlnTexts
@CALL ..\Bin\clear
@IF NOT EXIST .djgpp EXIT

:DJGPP

@SET Lib=..\OfrontDJGPP.a
@IF EXIST %Lib% DEL %Lib%
@CALL ..\Bin\djgpp-solid %Lib% SYSTEM0
@CALL ..\Bin\djgpp-solid %Lib% Args
@CALL ..\Bin\djgpp-smart %Lib% Console -noinit
@CALL ..\Bin\djgpp-solid %Lib% Unix -noinit
@CALL ..\Bin\djgpp-solid %Lib% Kernel
@CALL ..\Bin\djgpp-solid %Lib% Files
@CALL ..\Bin\djgpp-solid %Lib% Reals -noinit
@CALL ..\Bin\djgpp-solid %Lib% Modules -noinit
@CALL ..\Bin\djgpp-solid %Lib% CmdlnTexts

@..\Bin\clear
@EXIT

:NoXDev

@ECHO Please set system variable XDev=X:\Path\To\XDev
@PAUSE
