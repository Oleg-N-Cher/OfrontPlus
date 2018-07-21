@ECHO OFF
IF NOT "%XDev%"=="" GOTO XDev
ECHO Please set system variable XDev=X:\Path\To\XDev
PAUSE
EXIT

::SET Aliasing=-fno-strict-aliasing
SET Aliasing=-Wstrict-aliasing

:XDev

CD ..\Obj
SET PATH=%XDev%\WinDev\Bin\MinGW\bin
SET CC=gcc.exe -I. -I..\Mod -m32 -s -Os -g0 -fvisibility=hidden -fomit-frame-pointer -finline-small-functions -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -ffunction-sections %Aliasing% -c
SET AR=ar.exe -rc ..\Ofront.a
IF EXIST ..\Ofront.a DEL ..\Ofront.a

:: Ofront+

%CC% ..\Mod\SYSTEM.c Heap.c Types.c Platform.c Console.c Kernel.c Args.c Strings.c Files.c Reals.c Modules.c Texts.c Oberon.c
IF errorlevel 1 PAUSE
%AR% SYSTEM.o Heap.o Types.o Platform.o Console.o Kernel.o Args.o Strings.o Files.o Reals.o Modules.o Texts.o Oberon.o

:: OOC2

%CC% ooc2Ascii.c ooc2ConvTypes.c ooc2RandomNumbers.c ooc2Strings.c
IF errorlevel 1 PAUSE
%AR% ooc2Ascii.o ooc2ConvTypes.o ooc2RandomNumbers.o ooc2Strings.o

:: POW

%CC% powStrings.c
IF errorlevel 1 PAUSE
%AR% powStrings.o

:: Free Oberon

%CC% SDL2.c SDL2mixer.c Out.c Graph.c Math.c MathL.c Sound.c Turtle.c -w
IF errorlevel 1 PAUSE
%AR% SDL2.o SDL2mixer.o Out.o Graph.o Math.o MathL.c Sound.o Turtle.o

:: libCurl

%CC% libCurl.c NetHttp.c
IF errorlevel 1 PAUSE
%AR% libCurl.o NetHttp.o
