@ECHO OFF
SET PATH=%CD%\..\..\Bin
CD ..\Obj
SET OBERON=%CD%;%CD%\..\Sym;%CD%\..\Mod

:: Ofront+
ofront+ -s Heap32.Mod -apx Platform.Windows.Mod -atpx Console.Mod Kernel.Mod -atpx Args.Mod Strings.Mod Files.Mod Reals.Mod Modules.Mod Texts.Mod Oberon.Mod
IF errorlevel 1 PAUSE

:: OOC2
ofront+ -e ooc2Ascii.Mod ooc2ConvTypes.Mod ooc2RandomNumbers.Mod ooc2Strings.Mod
IF errorlevel 1 PAUSE
