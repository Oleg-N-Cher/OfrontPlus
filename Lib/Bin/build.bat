@IF NOT "%XDev%"=="" GOTO XDev
@ECHO Please set system variable XDev=X:\Path\To\XDev
@PAUSE
@EXIT

:XDev
@IF "%1"=="WinApi" DEL WinApi.c

@SET Lib=..\Ofront.a
@IF EXIST %Lib% DEL %Lib%

:: Ofront+
@CALL ..\Bin\mingw %Lib% SYSTEM
@CALL ..\Bin\mingw %Lib% Heap
@CALL ..\Bin\mingw %Lib% Platform
@CALL ..\Bin\mingw %Lib% Args
@CALL ..\Bin\mingw %Lib% Console
@CALL ..\Bin\mingw %Lib% Strings
@CALL ..\Bin\mingw %Lib% Files
@CALL ..\Bin\mingw %Lib% Reals
@CALL ..\Bin\mingw %Lib% Modules
@CALL ..\Bin\mingw %Lib% CmdlnTexts

:: OOC2
@CALL ..\Bin\mingw %Lib% ooc2RandomNumbers
@CALL ..\Bin\mingw %Lib% ooc2Strings

@..\Bin\clear
