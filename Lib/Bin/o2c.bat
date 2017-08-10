@ECHO OFF
CD ..\Obj
SET PATH=.\..\..\Bin;%PATH%
SET OBERON=.;.\..\Sym;.\..\Mod

:: Ofront+

ofront+.exe -s Heap32.Mod -apx Platform.Windows.Mod -atpx Console.Mod Kernel.Mod -atpx Args.Mod Strings.Mod Files.Mod Reals.Mod Modules.Mod Texts.Mod Oberon.Mod
IF errorlevel 1 PAUSE

:: OOC2

ofront+.exe -e ooc2Ascii.Mod ooc2ConvTypes.Mod ooc2RandomNumbers.Mod ooc2Strings.Mod
IF errorlevel 1 PAUSE

:: Free Oberon

ofront+.exe -s Types32.Mod SDL2.Mod Out.Mod Graph.Mod Math.Mod
IF errorlevel 1 PAUSE

FOR %%i IN (*.sym) DO MOVE /Y %%i ..\Sym >NUL
