@SET PATH=%CD%;%PATH%
@CD ..\Obj
@SET OBERON=%CD%;%CD%\..\Lib\Sym

ofront+ -se ..\Mod\OfrontOPM.cmdln.Mod
@IF errorlevel 1 PAUSE
ofront+ -se ..\Mod\OfrontOPS.Mod
@IF errorlevel 1 PAUSE
ofront+ -se..\Mod\OfrontOPT.Mod
@IF errorlevel 1 PAUSE
ofront+ -se ..\Mod\OfrontOPB.Mod
@IF errorlevel 1 PAUSE
ofront+ -se ..\Mod\OfrontOPP.Mod
@IF errorlevel 1 PAUSE
ofront+ -se ..\Mod\OfrontOPC.Mod
@IF errorlevel 1 PAUSE
ofront+ -se ..\Mod\OfrontOPV.Mod
@IF errorlevel 1 PAUSE
ofront+ -mse ..\Mod\OCatCmd.Mod
@IF errorlevel 1 PAUSE
ofront+ -mse ..\Mod\OfrontCmd.Mod
@IF errorlevel 1 PAUSE
ofront+ -mse ..\Mod\BrowserCmd.Mod
@IF errorlevel 1 PAUSE
@PAUSE
