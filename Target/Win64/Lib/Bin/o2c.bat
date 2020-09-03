@ECHO OFF
CD ..\Obj
SET PATH=..\..\..\Win32;%PATH%
SET OBERON=.;..\Sym;..\..\..\..\Mod\Lib

SET oef=
IF EXIST ..\..\oef.exe SET oef=^| oef

:: Ofront+

ofront+ -sC -88 Heap.cp -apx Platform.Windows.cp -atpx Console.cp Kernel.cp -atpx Args.cp Reals.cp Strings.cp Files.cp Modules.cp Texts.cp Oberon.cp %oef%
IF errorlevel 1 PAUSE

:: Eco

ofront+ -sC -88 ecoBigSets.cp ecoBuffer.cp ecoListen.cp ecoLists.cp ecoParser.cp ecoVector.cp %oef%
IF errorlevel 1 PAUSE

:: ETH Oberon

ofront+ -s2 -88 Zlib.ob2 ZlibBuffers.ob2 ZlibDeflate.ob2 ZlibInflate.ob2 ZlibReaders.ob2 ZlibWriters.ob2 Zip.ob2 %oef%
IF errorlevel 1 PAUSE

:: OOC2

ofront+ -sC -88 ooc2Ascii.cp ooc2ConvTypes.cp ooc2RandomNumbers.cp ooc2Strings.cp %oef%
IF errorlevel 1 PAUSE

:: POW

ofront+ -sC -88 powStrings.cp %oef%
IF errorlevel 1 PAUSE

:: Free Oberon

ofront+ -sC -88 SDL2.cp SDL2mixer.cp SQLite.Def Out.cp In.cp Graph.cp Math.cp MathL.cp Sound.cp Turtle.cp %oef%
IF errorlevel 1 PAUSE

:: libCurl
ofront+ -sC -88 libCurl.cp NetHttp.cp %oef%
IF errorlevel 1 PAUSE

:: Miscellaneous

ofront+ -s2 -88 Pattern.ob2 Configuration.ob2 %oef%
IF errorlevel 1 PAUSE

:: WinApi

ofront+ -sC -88 WinApi.Def %oef%
IF errorlevel 1 PAUSE

FOR %%i IN (*.sym) DO MOVE /Y %%i ..\Sym >NUL
