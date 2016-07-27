@IF NOT "%XDev%"=="" GOTO XDev
@ECHO Please set system variable XDev=X:\Path\To\XDev
@PAUSE
@EXIT

:XDev
@SET PATH=%CD%\..\..\Bin;%PATH%
@CD ..\Obj32L64
@DEL *.sym
@DEL ..\Sym32L64\*.sym

@IF "%1"=="WinApi" DEL WinApi.c

@SET Lib=..\Ofront32L64.a
@IF EXIST %Lib% DEL %Lib%

:: Ofront+

ofront+.exe -iapxe Heap0.Mod
@IF errorlevel 1 PAUSE
ofront+ -iapxe ..\Mod\Platform.Windows.Mod
@IF errorlevel 1 PAUSE
ofront+ -se ..\Mod\Args.Mod
@IF errorlevel 1 PAUSE
ofront+ -se ..\Mod\Console.Mod
@IF errorlevel 1 PAUSE
ofront+ -se ..\Mod\Strings.Mod
@IF errorlevel 1 PAUSE
ofront+ -se ..\Mod\Files.Mod
@IF errorlevel 1 PAUSE
ofront+ -se ..\Mod\Reals.Mod
@IF errorlevel 1 PAUSE
ofront+ -se ..\Mod\Modules.Mod
@IF errorlevel 1 PAUSE
ofront+ -se ..\Mod\Texts.Mod
@IF errorlevel 1 PAUSE

@CALL ..\Bin\mingw %Lib% SYSTEM -D_LONGINT64
@CALL ..\Bin\mingw %Lib% Heap -D_LONGINT64
@CALL ..\Bin\mingw %Lib% Platform -D_LONGINT64
@CALL ..\Bin\mingw %Lib% Args -D_LONGINT64
@CALL ..\Bin\mingw %Lib% Console -D_LONGINT64
@CALL ..\Bin\mingw %Lib% Strings -D_LONGINT64
@CALL ..\Bin\mingw %Lib% Files -D_LONGINT64
@CALL ..\Bin\mingw %Lib% Reals -D_LONGINT64
@CALL ..\Bin\mingw %Lib% Modules -D_LONGINT64
@CALL ..\Bin\mingw %Lib% Texts -D_LONGINT64

:: OOC2

ofront+ -se ..\Mod\ooc2Ascii.Mod
@IF errorlevel 1 PAUSE
ofront+ -se ..\Mod\ooc2ConvTypes.Mod
@IF errorlevel 1 PAUSE
ofront+ -se ..\Mod\ooc2RandomNumbers.Mod
@IF errorlevel 1 PAUSE
ofront+ -se ..\Mod\ooc2Strings.Mod
@IF errorlevel 1 PAUSE

@CALL ..\Bin\mingw %Lib% ooc2Ascii -D_LONGINT64
@CALL ..\Bin\mingw %Lib% ooc2ConvTypes -D_LONGINT64
@CALL ..\Bin\mingw %Lib% ooc2RandomNumbers -D_LONGINT64
@CALL ..\Bin\mingw %Lib% ooc2Strings -D_LONGINT64

@MOVE *.sym ..\Sym32L64
@..\Bin\clear
