@ECHO OFF
IF NOT "%XDev%"=="" SET PATH=%XDev%\WinDev\Bin\MinGW64\bin
CD "%~dp0"
CD ..\Obj

SET CC=gcc.exe -m64 -fPIC -Os -g0 -I..\..\..\..\Mod\Lib -fomit-frame-pointer -finline-small-functions -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -ffunction-sections -Wfatal-errors -c
SET AR=ar.exe -rc ..\Ofront.a
IF EXIST ..\Ofront.a DEL ..\Ofront.a

:: Ofront+

%CC% ..\..\..\..\Mod\Lib\SYSTEM.c Heap.c Platform.c Console.c Kernel.c CmdArgs.c Strings.c Files.c Math.c MathL.c Reals.c Modules.c Texts.c Oberon.c
IF errorlevel 1 PAUSE
%AR% SYSTEM.o Heap.o Platform.o Console.o Kernel.o CmdArgs.o Strings.o Files.o Math.o MathL.o Reals.o Modules.o Texts.o Oberon.o

:: Eco

%CC% ecoBigSets.c ecoBuffer.c ecoListen.c ecoLists.c ecoParser.c ecoVector.c
IF errorlevel 1 PAUSE
%AR% ecoBigSets.o ecoBuffer.o ecoListen.o ecoLists.o ecoParser.o ecoVector.o

:: ETH Oberon

%CC% Zlib.c ZlibBuffers.c ZlibDeflate.c ZlibInflate.c ZlibReaders.c ZlibWriters.c Zip.c
IF errorlevel 1 PAUSE
%AR% Zlib.o ZlibBuffers.o ZlibDeflate.o ZlibInflate.o ZlibReaders.o ZlibWriters.o Zip.o

:: OO2C

%CC% oo2cAscii.c oo2cConvTypes.c oo2cLongStrings.c oo2cRandomNumbers.c ^
  oo2cStrings.c
IF errorlevel 1 PAUSE
%AR% oo2cAscii.o oo2cConvTypes.o oo2cLongStrings.o oo2cRandomNumbers.o ^
  oo2cStrings.o

:: POW

%CC% powStrings.c
IF errorlevel 1 PAUSE
%AR% powStrings.o

:: Free Oberon

%CC% In.c Out.c Graph.c Sound.c Turtle.c -w
IF errorlevel 1 PAUSE
%AR% In.o Out.o Graph.o Sound.o Turtle.o

:: libCurl

%CC% NetHttp.c
IF errorlevel 1 PAUSE
%AR% NetHttp.o

:: Aria - Reusable Oberon-2 modules by Stewart Greenhill
:: https://github.com/sgreenhill/aria

%CC% arArgs.c arChar.c arCString.c arDir.c arSize.c arStrings.c arStringList.c ^
  arStringAssoc.c arText.c arStream.c arFile.c arOut.c arPath.c arPattern.c ^
  arErr.c arConfiguration.c arValue.c arFormat.c arJSON.c
IF errorlevel 1 PAUSE
%AR% arArgs.o arChar.o arCString.o arDir.o arSize.o arStrings.o arStringList.o ^
  arStringAssoc.o arText.o arStream.o arFile.o arOut.o arPath.o arPattern.o ^
  arErr.o arConfiguration.o arValue.o arFormat.o arJSON.o

:: BlackBox

%CC% bbKernel.c bbMath.c bbStrings.c
IF errorlevel 1 PAUSE
%AR% bbKernel.o bbMath.o bbStrings.o

DEL /Q *.o
