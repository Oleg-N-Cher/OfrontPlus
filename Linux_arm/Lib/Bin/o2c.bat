@ECHO OFF
SET PATH=%CD%\..\..\..\Bin
CD ..\Obj
SET OBERON=%CD%;%CD%\..\Sym;%CD%\..\..\..\Lib\Mod

:: Ofront+

ofront+ -sC -48 Heap.Mod -apx Types.Mod Platform.Unix.Mod -atpx Console.Mod Kernel.Mod -atpx Args.Mod Strings.Mod Files.Mod Reals.Mod Modules.Mod Texts.Mod Oberon.Mod
IF errorlevel 1 PAUSE

:: Eco

ofront+ -sC -48 ecoBigSets.Mod ecoBuffer.Mod ecoListen.Mod ecoLists.Mod ecoPars$
IF errorlevel 1 PAUSE

:: OOC2

ofront+ -eC -48 ooc2Ascii.Mod ooc2ConvTypes.Mod ooc2RandomNumbers.Mod ooc2Strings.Mod
IF errorlevel 1 PAUSE

:: POW

ofront+ -eC -48 powStrings.Mod
IF errorlevel 1 PAUSE

:: Free Oberon

ofront+ -sC -48 SDL2.Mod SDL2mixer.Mod Out.Mod Graph.Mod Math.Mod MathL.Mod Sound.Mod Turtle.Mod
IF errorlevel 1 PAUSE

:: libCurl

ofront+ -sC -48 libCurl.Mod NetHttp.Mod
IF errorlevel 1 PAUSE

FOR %%i IN (*.sym) DO MOVE /Y %%i ..\Sym >NUL
