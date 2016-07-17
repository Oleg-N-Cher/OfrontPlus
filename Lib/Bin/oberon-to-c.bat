@SET PATH=%CD%\..\..\Bin;%PATH%
@CD ..\Obj

ofront+ -liapxe ..\Mod\SYSTEM.Mod
@IF errorlevel 1 PAUSE
ofront+ -lse ..\Mod\Args.Mod
@IF errorlevel 1 PAUSE
ofront+ -lse ..\Mod\Console.Mod
@IF errorlevel 1 PAUSE
ofront+ -lse ..\Mod\Platform.Mod
@IF errorlevel 1 PAUSE
ofront+ -liapxe ..\Mod\Heap.Mod
@IF errorlevel 1 PAUSE
ofront+ -lse ..\Mod\Unix.msvcrt.Mod
@IF errorlevel 1 PAUSE
ofront+ -lse ..\Mod\Kernel.Mod
@IF errorlevel 1 PAUSE
ofront+ -lse ..\Mod\Files.Mod
@IF errorlevel 1 PAUSE
ofront+ -lse ..\Mod\Reals.Mod
@IF errorlevel 1 PAUSE
ofront+ -lse ..\Mod\Modules.Mod
@IF errorlevel 1 PAUSE
ofront+ -lse ..\Mod\CmdlnTexts.Mod
@IF errorlevel 1 PAUSE
