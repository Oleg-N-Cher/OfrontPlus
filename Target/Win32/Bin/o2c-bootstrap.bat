@ECHO OFF
SET PATH=%CD%\..\Target\Win32
SET OBERON=%CD%;%CD%\..\Mod;%CD%\..\Mod\Lib

:: Ofront+

ofront+ -sC %1 Heap.cp -apx Platform.Unix.cp -atpx Console.cp Kernel.cp -atpx Args.cp Reals.cp Strings.cp Files.cp Modules.cp Texts.cp Oberon.cp
IF errorlevel 1 EXIT

:: Eco

ofront+ -sC %1 ecoBigSets.cp ecoBuffer.cp ecoListen.cp ecoLists.cp ecoParser.cp ecoVector.cp
IF errorlevel 1 EXIT

:: ETH Oberon

ofront+ -s2 %1 Zlib.ob2 ZlibBuffers.ob2 ZlibDeflate.ob2 ZlibInflate.ob2 ZlibReaders.ob2 ZlibWriters.ob2 Zip.ob2
IF errorlevel 1 EXIT

:: OOC2

ofront+ -sC %1 ooc2Ascii.cp ooc2ConvTypes.cp ooc2RandomNumbers.cp ooc2Strings.cp
IF errorlevel 1 EXIT

:: POW

ofront+ -sC %1 powStrings.cp
IF errorlevel 1 EXIT

:: Free Oberon

ofront+ -sC %1 SDL2.cp SDL2mixer.cp SQLite.cp Out.cp In.cp Graph.cp Math.cp MathL.cp Sound.cp Turtle.cp
IF errorlevel 1 EXIT

:: libCurl

ofront+ -sC %1 libCurl.cp NetHttp.cp
IF errorlevel 1 EXIT

:: Aria - Reusable Oberon-2 modules by Stewart Greenhill
:: https://github.com/sgreenhill/aria

ofront+ -sO2 %1 ^
  arC.ob2 arCFormat.ob2 arChar.ob2 arCString.ob2 arDir.ob2 arMath.ob2 ^
  arSize.ob2 arStrings.ob2 arStringList.ob2 arStringAssoc.ob2 arText.ob2 ^
  arStream.ob2 arFile.ob2 arOut.ob2 arPath.ob2 arPattern.ob2 arErr.ob2 ^
  arConfiguration.ob2 arValue.ob2 arFormat.ob2 arJSON.ob2
IF errorlevel 1 EXIT

:: BlackBox

ofront+ -sC %1 bbMath.cp
IF errorlevel 1 EXIT

:: Ofront+ and tools

ofront+ -sC %1 ^
  OfrontErrors.cp OfrontOPM.cmdln.cp -G OfrontOPS.cp OfrontOPT.cp OfrontOPB.cp ^
  OfrontOPP.cp OfrontOPC.cp OfrontOPV.cp BrowserCmd.cp -m ImportGraph.cp -m ^
  OCatCmd.cp -m OfrontCmd.cp -m OdcToText.ob3 -3m OEF.ob2 -2m OMake.ob2 -2m
IF errorlevel 1 EXIT

DEL /Q *.sym
