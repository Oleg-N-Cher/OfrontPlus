@ECHO OFF
CD ..\Obj64
SET PATH=%CD%\..\..\Bin64
SET OBERON=%CD%;%CD%\..\Sym64;%CD%\..\Mod

:: Ofront+

ofront+.exe -s Heap64.Mod -apx Platform.Windows.Mod -atpx Console.Mod Kernel.Mod -atpx Args.Mod Strings.Mod Files.Mod Reals.Mod Modules.Mod Texts.Mod Oberon.Mod
IF errorlevel 1 PAUSE

:: OOC2

ofront+.exe -e ooc2Ascii.Mod ooc2ConvTypes.Mod ooc2RandomNumbers.Mod ooc2Strings.Mod
IF errorlevel 1 PAUSE

FOR %%i IN (*.sym) DO MOVE /Y %%i ..\Sym64 >NUL
