@ECHO OFF
SET PATH=%CD%\..\..\..\Bin
CD ..\Obj
SET OBERON=%CD%;%CD%\..\Sym;%CD%\..\..\..\Lib\Mod

:: Ofront+

ofront+ -sC -88 Heap.Mod -apx Platform.Unix.Mod -atpx Console.Mod Kernel.Mod -atpx Args.Mod Reals.Mod Strings.Mod Files.Mod Modules.Mod Texts.Mod Oberon.Mod
IF errorlevel 1 PAUSE

:: Eco

ofront+ -sC -88 ecoBigSets.Mod ecoBuffer.Mod ecoListen.Mod ecoLists.Mod ecoParser.Mod
IF errorlevel 1 PAUSE

:: OOC2

ofront+ -eC -88 ooc2Ascii.Mod ooc2ConvTypes.Mod ooc2RandomNumbers.Mod ooc2Strings.Mod
IF errorlevel 1 PAUSE

:: POW

ofront+ -eC -88 powStrings.Mod
IF errorlevel 1 PAUSE

:: Free Oberon

ofront+ -sC -88 SDL2.Mod SDL2mixer.Mod SQLite.Mod Out.Mod In.Mod Graph.Mod Math.Mod MathL.Mod Sound.Mod Turtle.Mod
IF errorlevel 1 PAUSE

:: libCurl

ofront+ -sC -88 libCurl.Mod NetHttp.Mod
IF errorlevel 1 PAUSE

FOR %%i IN (*.sym) DO MOVE /Y %%i ..\Sym >NUL
