@ECHO OFF
SET PATH=%CD%\..\..\..\Bin
CD ..\Obj
SET OBERON=%CD%;%CD%\..\Sym;%CD%\..\..\..\Lib\Mod

:: Ofront+
ofront+ -sC -44 Heap32.Mod -apx Types32.Mod Platform.Unix.Mod -atpx Console.Mod Kernel.Mod -atpx Args.Mod Strings.Mod Files.Mod Reals.Mod Modules.Mod Texts.Mod Oberon.Mod
IF errorlevel 1 PAUSE

:: OOC2
ofront+ -eC -44 ooc2Ascii.Mod ooc2ConvTypes.Mod ooc2RandomNumbers.Mod ooc2Strings.Mod
IF errorlevel 1 PAUSE

:: Free Oberon

ofront+ -sC -44 SDL2.Mod Out.Mod Graph.Mod Math.Mod Turtle.Mod
IF errorlevel 1 PAUSE

FOR %%i IN (*.sym) DO MOVE /Y %%i ..\Sym >NUL
