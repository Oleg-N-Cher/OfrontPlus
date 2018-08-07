@ECHO OFF
CD ..\Obj
SET PATH=.\..\..\Bin;%PATH%
SET OBERON=.;.\..\Sym;.\..\Mod

:: Ofront+

ofront+ -sC -48 Heap32.Mod -apx Types32.Mod Platform.Windows.Mod -atpx Console.Mod Kernel.Mod -atpx Args.Mod Strings.Mod Files.Mod Reals.Mod Modules.Mod Texts.Mod Oberon.Mod
IF errorlevel 1 PAUSE

:: OOC2

ofront+ -sC -48 ooc2Ascii.Mod ooc2ConvTypes.Mod ooc2RandomNumbers.Mod ooc2Strings.Mod
IF errorlevel 1 PAUSE

:: POW

ofront+ -sC -48 powStrings.Mod
IF errorlevel 1 PAUSE

:: Free Oberon

ofront+ -sC -48 SDL2.Mod SDL2mixer.Mod Out.Mod Graph.Mod Math.Mod MathL.Mod Sound.Mod Turtle.Mod
IF errorlevel 1 PAUSE

:: libCurl
ofront+ -sC -48 libCurl.Mod NetHttp.Mod
IF errorlevel 1 PAUSE

FOR %%i IN (*.sym) DO MOVE /Y %%i ..\Sym >NUL
