@SET PATH=%CD%\..\..\Bin;%PATH%
@CD ..\Obj
@SET OBERON=%CD%;%CD%\..\Sym

:: Ofront+
::ofront+ -apxe Heap0.Mod
::@IF errorlevel 1 PAUSE
ofront+ -seatpx ..\Mod\Platform.Windows.Mod
@IF errorlevel 1 PAUSE
ofront+ -se ..\Mod\Args.Mod
@IF errorlevel 1 PAUSE
ofront+ -se ..\Mod\Console.Mod
@IF errorlevel 1 PAUSE
ofront+ -se ..\Mod\Strings.Mod
@IF errorlevel 1 PAUSE
ofront+ -se ..\Mod\Files.Mod
@IF errorlevel 1 PAUSE
ofront+ -se ..\Mod\Reals.Mod
@IF errorlevel 1 PAUSE
ofront+ -se ..\Mod\Modules.Mod
@IF errorlevel 1 PAUSE
ofront+ -se ..\Mod\Texts.Mod
@IF errorlevel 1 PAUSE
ofront+ -se ..\Mod\Oberon.Mod
@IF errorlevel 1 PAUSE

:: OOC2
ofront+ -se ..\Mod\ooc2Ascii.Mod
@IF errorlevel 1 PAUSE
ofront+ -se ..\Mod\ooc2ConvTypes.Mod
@IF errorlevel 1 PAUSE
ofront+ -se ..\Mod\ooc2RandomNumbers.Mod
@IF errorlevel 1 PAUSE
ofront+ -se ..\Mod\ooc2Strings.Mod
@IF errorlevel 1 PAUSE
