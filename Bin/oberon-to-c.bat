@SET PATH=%CD%;%PATH%
@CD ..\Lib\Obj
ofront SYSTEM.Mod
@IF errorlevel 1 PAUSE
ofront Args.Mod
@IF errorlevel 1 PAUSE
ofront Console.Mod
@IF errorlevel 1 PAUSE
ofront Unix.msvcrt.Mod
@IF errorlevel 1 PAUSE
ofront Kernel.Mod
@IF errorlevel 1 PAUSE
ofront Files.Mod
@IF errorlevel 1 PAUSE
ofront Reals.Mod
@IF errorlevel 1 PAUSE
ofront Modules.Mod
@IF errorlevel 1 PAUSE
ofront CmdlnTexts.Mod
@IF errorlevel 1 PAUSE
pause