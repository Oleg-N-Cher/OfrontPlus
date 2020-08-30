@ECHO OFF
IF NOT "%XDev%"=="" SET PATH=%XDev%\WinDev\Bin\MinGW64\bin
CD ..\Obj

SET CC=gcc.exe -m64 -fPIC -Os -g0 -I..\..\..\..\Mod\Lib -fomit-frame-pointer -finline-small-functions -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -ffunction-sections -c
SET AR=ar.exe -rc ..\Ofront.a
IF EXIST ..\Ofront.a DEL ..\Ofront.a

:: Ofront+

%CC% ..\..\..\..\Mod\Lib\SYSTEM.c Heap.c Platform.c Console.c Kernel.c Args.c Strings.c Files.c Reals.c Modules.c Texts.c Oberon.c
IF errorlevel 1 PAUSE
%AR% SYSTEM.o Heap.o Platform.o Console.o Kernel.o Args.o Strings.o Files.o Reals.o Modules.o Texts.o Oberon.o

:: Eco

%CC% ecoBigSets.c ecoBuffer.c ecoListen.c ecoLists.c ecoParser.c ecoVector.c
IF errorlevel 1 PAUSE
%AR% ecoBigSets.o ecoBuffer.o ecoListen.o ecoLists.o ecoParser.o ecoVector.o

:: ETH Oberon

%CC% Zlib.c ZlibBuffers.c ZlibDeflate.c ZlibInflate.c ZlibReaders.c ZlibWriters.c Zip.c
IF errorlevel 1 PAUSE
%AR% Zlib.o ZlibBuffers.o ZlibDeflate.o ZlibInflate.o ZlibReaders.o ZlibWriters.o Zip.o

:: OOC2

%CC% ooc2Ascii.c ooc2ConvTypes.c ooc2RandomNumbers.c ooc2Strings.c
IF errorlevel 1 PAUSE
%AR% ooc2Ascii.o ooc2ConvTypes.o ooc2RandomNumbers.o ooc2Strings.o

:: POW

%CC% powStrings.c
IF errorlevel 1 PAUSE
%AR% powStrings.o

:: Free Oberon

%CC% SDL2.c SDL2mixer.c In.c Out.c Graph.c Math.c MathL.c Sound.c Turtle.c -w
IF errorlevel 1 PAUSE
%AR% SDL2.o SDL2mixer.o In.o Out.o Graph.o Math.o MathL.o Sound.o Turtle.o

:: libCurl

%CC% NetHttp.c
IF errorlevel 1 PAUSE
%AR% NetHttp.o

:: Miscellaneous

%CC% Pattern.c Configuration.c
IF errorlevel 1 PAUSE
%AR% Pattern.o Configuration.o

DEL /Q *.o
