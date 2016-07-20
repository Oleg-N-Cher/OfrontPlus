@IF NOT "%XDev%"=="" GOTO XDev
@ECHO Please set system variable XDev=X:\Path\To\XDev
@PAUSE
@EXIT

:XDev
@IF "%1"=="WinApi" DEL WinApi.c

@SET Lib=..\Ofront64.a
@IF EXIST %Lib% DEL %Lib%

:: Ofront+
@CALL ..\Bin\mingw64 %Lib% SYSTEM
@CALL ..\Bin\mingw64 %Lib% Heap
@CALL ..\Bin\mingw64 %Lib% Platform
@CALL ..\Bin\mingw64 %Lib% Args
@CALL ..\Bin\mingw64 %Lib% Console
@CALL ..\Bin\mingw64 %Lib% Strings
@CALL ..\Bin\mingw64 %Lib% Files
@CALL ..\Bin\mingw64 %Lib% Reals
@CALL ..\Bin\mingw64 %Lib% Modules
@CALL ..\Bin\mingw64 %Lib% CmdlnTexts

:: OOC2
@CALL ..\Bin\mingw64 %Lib% ooc2Ascii
@CALL ..\Bin\mingw64 %Lib% ooc2ConvTypes
@CALL ..\Bin\mingw64 %Lib% ooc2RandomNumbers
@CALL ..\Bin\mingw64 %Lib% ooc2Strings

@..\Bin\clear
