@ECHO OFF
SET PATH=%CD%\..\..\Bin
CD ..\Obj
SET OBERON=%CD%;%CD%\..\Sym;%CD%\..\Mod

:: Ofront+
::ofront+ -apx Heap0.Mod
::IF errorlevel 1 PAUSE
ofront+ Platform.Windows.Mod -eatpx Console.Mod Kernel.Mod -eatpx Args.Mod Strings.Mod Files.Mod Reals.Mod Modules.Mod Texts.Mod Oberon.Mod
IF errorlevel 1 PAUSE

:: OOC2
ofront+ ooc2Ascii.Mod ooc2ConvTypes.Mod ooc2RandomNumbers.Mod ooc2Strings.Mod
IF errorlevel 1 PAUSE
