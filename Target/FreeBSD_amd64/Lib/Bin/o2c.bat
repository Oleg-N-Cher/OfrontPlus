@ECHO OFF
SET PATH=%CD%\..\..\..\Win32
CD ..\Obj
SET OBERON=%CD%;%CD%\..\Sym;%CD%\..\..\..\..\Mod\Lib

:: Ofront+

ofront+ -s88 Heap.cp -apx Platform.Unix.cp -atpx Console.cp Kernel.cp -atpx CmdArgs.Unix.cp -px Reals.cp Strings.cp Files.cp Modules.cp Texts.cp Oberon.cp
IF errorlevel 1 PAUSE

:: Eco

ofront+ -s88 ecoBigSets.cp ecoBuffer.cp ecoListen.cp ecoLists.cp ecoParser.cp ecoVector.cp
IF errorlevel 1 PAUSE

:: ETH Oberon

ofront+ -s88 Zlib.ob2 ZlibBuffers.ob2 ZlibDeflate.ob2 ZlibInflate.ob2 ZlibReaders.ob2 ZlibWriters.ob2 Zip.ob2
IF errorlevel 1 PAUSE

:: OOC2

ofront+ -s88 ooc2Ascii.cp ooc2ConvTypes.cp ooc2RandomNumbers.cp ooc2Strings.cp
IF errorlevel 1 PAUSE

:: POW

ofront+ -s88 powStrings.cp
IF errorlevel 1 PAUSE

:: Free Oberon

ofront+ -s88 SDL2.cp SDL2mixer.cp SQLite.cp Out.cp In.cp Graph.cp Math.cp MathL.cp Sound.cp Turtle.cp
IF errorlevel 1 PAUSE

:: libCurl

ofront+ -s88 libCurl.cp NetHttp.cp
IF errorlevel 1 PAUSE

:: Aria - Reusable Oberon-2 modules by Stewart Greenhill
:: https://github.com/sgreenhill/aria

ofront+ -sO2 -88 ^
  arArgs.ob2 arC.ob2 arCFormat.ob2 arChar.ob2 arCString.ob2 arDir.ob2 ^
  arMath.ob2 arSize.ob2 arStrings.ob2 arStringList.ob2 arStringAssoc.ob2 ^
  arText.ob2 arStream.ob2 arFile.ob2 arOut.ob2 arPath.ob2 arPattern.ob2 ^
  arErr.ob2 arConfiguration.ob2 arValue.ob2 arFormat.ob2 arJSON.ob2
IF errorlevel 1 PAUSE

:: BlackBox

ofront+ -sw88 bbLinLibc.cp bbLinLibW.cp bbKernel.Unix.cp bbMath.cp bbStrings.cp
IF errorlevel 1 PAUSE

FOR %%i IN (*.sym) DO MOVE /Y %%i ..\Sym >NUL
