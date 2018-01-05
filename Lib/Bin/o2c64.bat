@ECHO OFF
CD ..\Obj64
SET PATH=.\..\..\Bin;%PATH%
SET OBERON=.;.\..\Sym64;.\..\Mod

:: Ofront+

ofront+ -sC -88 Heap64.Mod -apx Types64.Mod Platform.Windows.Mod -atpx Console.Mod Kernel.Mod -atpx Args.Mod Strings.Mod Files.Mod Reals.Mod Modules.Mod Texts.Mod Oberon.Mod
IF errorlevel 1 PAUSE

:: OOC2

ofront+ -eC -88 ooc2Ascii.Mod ooc2ConvTypes.Mod ooc2RandomNumbers.Mod ooc2Strings.Mod
IF errorlevel 1 PAUSE

:: POW

ofront+ -eC -88 powStrings.Mod
IF errorlevel 1 PAUSE

:: Free Oberon

ofront+ -sC -88 SDL2.Mod SDL2mixer.Mod Out.Mod Graph.Mod Math.Mod Sound.Mod Turtle.Mod
IF errorlevel 1 PAUSE

FOR %%i IN (*.sym) DO MOVE /Y %%i ..\Sym64 >NUL
