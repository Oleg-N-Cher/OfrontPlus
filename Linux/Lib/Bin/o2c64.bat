@ECHO OFF
SET PATH=%CD%\..\..\..\Bin64
CD ..\Obj64
SET OBERON=%CD%;%CD%\..\Sym64;%CD%\..\..\..\Lib\Mod

:: Ofront+
ofront+ -s Heap64.Mod -apx Types64.Mod Platform.Unix.Mod -atpx Console.Mod Kernel.Mod -atpx Args.Mod Strings.Mod Files.Mod Reals.Mod Modules.Mod Texts.Mod Oberon.Mod
IF errorlevel 1 PAUSE

:: OOC2
ofront+ -e ooc2Ascii.Mod ooc2ConvTypes.Mod ooc2RandomNumbers.Mod ooc2Strings.Mod
IF errorlevel 1 PAUSE

FOR %%i IN (*.sym) DO MOVE /Y %%i ..\Sym64 >NUL
