@SET PATH=%CD%\..\..\Bin;%PATH%
@CD ..\Obj

:: Ofront+
ofront+ -liapxe ..\Mod\Heap.Mod
@IF errorlevel 1 PAUSE
ofront+ -liapxe ..\Mod\Platform.Windows.Mod
@IF errorlevel 1 PAUSE
ofront+ -lse ..\Mod\Args.Mod
@IF errorlevel 1 PAUSE
ofront+ -lse ..\Mod\Console.Mod
@IF errorlevel 1 PAUSE
ofront+ -lse ..\Mod\Strings.Mod
@IF errorlevel 1 PAUSE
ofront+ -lse ..\Mod\Files.Mod
@IF errorlevel 1 PAUSE
ofront+ -lse ..\Mod\Reals.Mod
@IF errorlevel 1 PAUSE
ofront+ -lse ..\Mod\Modules.Mod
@IF errorlevel 1 PAUSE
ofront+ -lse ..\Mod\Texts.Mod
@IF errorlevel 1 PAUSE

:: OOC2
ofront+ -lse ..\Mod\ooc2Ascii.Mod
@IF errorlevel 1 PAUSE
ofront+ -lse ..\Mod\ooc2ConvTypes.Mod
@IF errorlevel 1 PAUSE
ofront+ -lse ..\Mod\ooc2RandomNumbers.Mod
@IF errorlevel 1 PAUSE
ofront+ -lse ..\Mod\ooc2Strings.Mod
@IF errorlevel 1 PAUSE
