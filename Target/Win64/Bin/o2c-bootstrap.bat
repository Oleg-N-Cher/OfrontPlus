@ECHO OFF
SET PATH=%CD%\..\Target\Win64
SET OBERON=%CD%;%CD%\..\Mod;%CD%\..\Mod\Lib

:: Ofront+

ofront+ -s %1 Heap.cp -apx Platform.%2.cp -atpx Console.cp Kernel.cp -atpx ^
  CmdArgs.Unix.cp -px Reals.cp Strings.cp Files.cp Modules.cp Texts.cp Oberon.cp
IF errorlevel 1 EXIT

:: Eco

ofront+ -s %1 ecoBigSets.cp ecoBuffer.cp ecoListen.cp ecoLists.cp ecoParser.cp ecoVector.cp
IF errorlevel 1 EXIT

:: ETH Oberon

ofront+ -s %1 Zlib.ob2 ZlibBuffers.ob2 ZlibDeflate.ob2 ZlibInflate.ob2 ZlibReaders.ob2 ZlibWriters.ob2 Zip.ob2
IF errorlevel 1 EXIT

:: OO2C

ofront+ -s %1 ^
  oo2cAscii.cp oo2cConvTypes.cp oo2cLongStrings.cp oo2cRandomNumbers.cp ^
  oo2cStrings.cp %oef%
IF errorlevel 1 EXIT

:: POW

ofront+ -s %1 powStrings.cp
IF errorlevel 1 EXIT

:: Free Oberon

ofront+ -s %1 SDL2.cp SDL2mixer.cp SQLite.cp Out.cp In.cp Graph.cp Math.cp MathL.cp Sound.cp Turtle.cp
IF errorlevel 1 EXIT

:: libCurl

ofront+ -s %1 libCurl.cp NetHttp.cp
IF errorlevel 1 EXIT

:: Aria - Reusable Oberon-2 modules by Stewart Greenhill
:: https://github.com/sgreenhill/aria

ofront+ -sO %1 ^
  arArgs.ob2 arC.ob2 arCFormat.ob2 arChar.ob2 arCString.ob2 arDir.ob2 ^
  arMath.ob2 arSize.ob2 arStrings.ob2 arStringList.ob2 arStringAssoc.ob2 ^
  arText.ob2 arStream.ob2 arFile.ob2 arOut.ob2 arPath.ob2 arPattern.ob2 ^
  arErr.ob2 arConfiguration.ob2 arValue.ob2 arFormat.ob2 arJSON.ob2
IF errorlevel 1 EXIT

:: BlackBox

ofront+ -sw %1 bbLinLibc.cp bbLinLibW.cp bbKernel.Unix.cp bbMath.cp bbStrings.cp
IF errorlevel 1 EXIT

:: Ofront+ and tools

ofront+ -s %1 ^
  OfrontErrors.cp OfrontOPM.cmdln.cp -G OfrontOPS.cp OfrontOPT.cp OfrontOPB.cp ^
  OfrontOPP.cp OfrontOPC.cp OfrontOPV.cp BrowserCmd.cp -m ImportGraph.cp -m ^
  OCatCmd.cp -m OfrontCmd.cp -m OdcToText.ob3 -3m OEF.ob2 -2m OMake.ob2 -2m
IF errorlevel 1 EXIT

DEL /Q *.sym
