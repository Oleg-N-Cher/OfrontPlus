@SET PATH=%CD%\..\..\Bin;%PATH%
@CD ..\Obj

ofront -se ..\Mod\SYSTEM.Mod
@IF errorlevel 1 PAUSE
ofront -se ..\Mod\Args.Mod
@IF errorlevel 1 PAUSE
ofront -se ..\Mod\Console.Mod
@IF errorlevel 1 PAUSE
ofront -se ..\Mod\Unix.msvcrt.Mod
@IF errorlevel 1 PAUSE
ofront -se ..\Mod\Kernel.Mod
@IF errorlevel 1 PAUSE
ofront -se ..\Mod\Files.Mod
@IF errorlevel 1 PAUSE
ofront -se ..\Mod\Reals.Mod
@IF errorlevel 1 PAUSE
ofront -se ..\Mod\Modules.Mod
@IF errorlevel 1 PAUSE
ofront -se ..\Mod\CmdlnTexts.Mod
@IF errorlevel 1 PAUSE
pause