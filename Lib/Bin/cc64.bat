@ECHO OFF
IF NOT "%XDev%"=="" GOTO XDev
ECHO Please set system variable XDev=X:\Path\To\XDev
PAUSE
EXIT

:XDev

CD ..\Obj64
SET PATH=%XDev%\WinDev\Bin\MinGW64\bin
SET CC=gcc.exe -I. -I..\Mod -m64 -s -Os -g0 -fvisibility=hidden -finline-small-functions -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -ffunction-sections -c
SET AR=ar.exe -rc ..\Ofront64.a
IF EXIST ..\Ofront64.a DEL ..\Ofront64.a

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

%CC% SDL2.c SDL2mixer.c Out.c Graph.c Math.c Sound.c Turtle.c -w
IF errorlevel 1 PAUSE
%AR% SDL2.o SDL2mixer.o Out.o Graph.o Math.o Sound.o Turtle.o
