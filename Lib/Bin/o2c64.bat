@ECHO OFF
CD ..\Obj64
SET PATH=.\..\..\Bin;%PATH%
SET OBERON=.;.\..\Sym64;.\..\Mod

:: Ofront+

ofront+ -sC -88 Heap.Mod -apx Platform.Windows.Mod -atpx Console.Mod Kernel.Mod -atpx Args.Mod Strings.Mod Files.Mod Reals.Mod Modules.Mod Texts.Mod Oberon.Mod
IF errorlevel 1 PAUSE

:: Eco

ofront+ -sC -88 ecoBigSets.Mod ecoBuffer.Mod ecoListen.Mod ecoLists.Mod ecoParser.Mod ecoVector.Mod
IF errorlevel 1 PAUSE

:: OOC2

ofront+ -sC -88 ooc2Ascii.Mod ooc2ConvTypes.Mod ooc2RandomNumbers.Mod ooc2Strings.Mod
IF errorlevel 1 PAUSE

:: POW

ofront+ -sC -88 powStrings.Mod
IF errorlevel 1 PAUSE

:: Free Oberon

ofront+ -sC -88 SDL2.Mod SDL2mixer.Mod SQLite.Mod In.Mod Out.Mod Graph.Mod Math.Mod MathL.Mod Sound.Mod Turtle.Mod
IF errorlevel 1 PAUSE

:: libCurl
ofront+ -sC -88 libCurl.Mod NetHttp.Mod
IF errorlevel 1 PAUSE

FOR %%i IN (*.sym) DO MOVE /Y %%i ..\Sym64 >NUL
