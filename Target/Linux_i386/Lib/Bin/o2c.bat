@ECHO OFF
SET PATH=%CD%\..\..\..\Win32
CD ..\Obj
SET OBERON=%CD%;%CD%\..\Sym;%CD%\..\..\..\..\Mod\Lib

:: Ofront+

ofront+ -s44 Heap.cp -apx Platform.Unix.cp -atpx Console.cp Kernel.cp -atpx CmdArgs.Unix.cp -px Reals.cp Strings.cp Files.cp Modules.cp Texts.cp Oberon.cp
IF errorlevel 1 PAUSE

:: Eco

ofront+ -s44 ecoBigSets.cp ecoBuffer.cp ecoListen.cp ecoLists.cp ecoParser.cp ecoVector.cp
IF errorlevel 1 PAUSE

:: ETH Oberon

ofront+ -s44 Zlib.ob2 ZlibBuffers.ob2 ZlibDeflate.ob2 ZlibInflate.ob2 ZlibReaders.ob2 ZlibWriters.ob2 Zip.ob2
IF errorlevel 1 PAUSE

:: OO2C

ofront+ -s44 ^
  oo2cAscii.cp oo2cConvTypes.cp oo2cLongStrings.cp oo2cRandomNumbers.cp ^
  oo2cStrings.cp
IF errorlevel 1 PAUSE

:: POW

ofront+ -s44 powStrings.cp
IF errorlevel 1 PAUSE

:: Free Oberon

ofront+ -s44 SDL2.cp SDL2mixer.cp SQLite.cp Out.cp In.cp Graph.cp Math.cp MathL.cp Sound.cp Turtle.cp
IF errorlevel 1 PAUSE

:: libCurl

ofront+ -s44 libCurl.cp NetHttp.cp
IF errorlevel 1 PAUSE

:: Aria - Reusable Oberon-2 modules by Stewart Greenhill
:: https://github.com/sgreenhill/aria

ofront+ -sO44 ^
  arArgs.ob2 arC.ob2 arCFormat.ob2 arChar.ob2 arCString.ob2 arDir.ob2 ^
  arMath.ob2 arSize.ob2 arStrings.ob2 arStringList.ob2 arStringAssoc.ob2 ^
  arText.ob2 arStream.ob2 arFile.ob2 arOut.ob2 arPath.ob2 arPattern.ob2 ^
  arErr.ob2 arConfiguration.ob2 arValue.ob2 arFormat.ob2 arJSON.ob2
IF errorlevel 1 PAUSE

:: BlackBox

ofront+ -sw44 bbLinLibc.cp bbLinLibW.cp bbKernel.Unix.cp bbMath.cp bbStrings.cp
IF errorlevel 1 PAUSE

FOR %%i IN (*.sym) DO MOVE /Y %%i ..\Sym >NUL
