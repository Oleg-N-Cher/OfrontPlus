@ECHO OFF
SET PATH=%CD%\..\..\..\Win32
CD ..\Obj
SET OBERON=%CD%;%CD%\..\Sym;%CD%\..\..\..\..\Mod\Lib

:: Ofront+

ofront+ -sC -48 Heap.Mod -apx Platform.Unix.Mod -atpx Console.Mod Kernel.Mod -atpx Args.Mod Reals.Mod Strings.Mod Files.Mod Modules.Mod Texts.Mod Oberon.Mod
IF errorlevel 1 PAUSE

:: Eco

ofront+ -sC -48 ecoBigSets.Mod ecoBuffer.Mod ecoListen.Mod ecoLists.Mod ecoParser.CP ecoVector.Mod
IF errorlevel 1 PAUSE

:: ETH Oberon

ofront+ -s2 -48 Zlib.Ob2 ZlibBuffers.Ob2 ZlibDeflate.Ob2 ZlibInflate.Ob2 ZlibReaders.Ob2 ZlibWriters.Ob2 Zip.Ob2
IF errorlevel 1 PAUSE

:: OOC2

ofront+ -sC -48 ooc2Ascii.Mod ooc2ConvTypes.Mod ooc2RandomNumbers.Mod ooc2Strings.Mod
IF errorlevel 1 PAUSE

:: POW

ofront+ -sC -48 powStrings.Mod
IF errorlevel 1 PAUSE

:: Free Oberon

ofront+ -sC -48 SDL2.Mod SDL2mixer.Mod SQLite.Def Out.Mod In.Mod Graph.Mod Math.Mod MathL.Mod Sound.Mod Turtle.Mod
IF errorlevel 1 PAUSE

:: libCurl

ofront+ -sC -48 libCurl.Mod NetHttp.Mod
IF errorlevel 1 PAUSE

:: Miscellaneous

ofront+ -s2 -48 Pattern.Ob2 Configuration.Ob2
IF errorlevel 1 PAUSE

FOR %%i IN (*.sym) DO MOVE /Y %%i ..\Sym >NUL
