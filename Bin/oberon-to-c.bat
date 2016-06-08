@SET PATH=%CD%;%PATH%
@CD ..\Obj
@SET OBERON=%CD%;%CD%\..\Lib\Sym

ofront+ -lse ..\Mod\OfrontOPM.cmdln.Mod
@IF errorlevel 1 PAUSE
ofront+ -lse ..\Mod\OfrontOPS.Mod
@IF errorlevel 1 PAUSE
ofront+ -lse..\Mod\OfrontOPT.Mod
@IF errorlevel 1 PAUSE
ofront+ -lse ..\Mod\OfrontOPB.Mod
@IF errorlevel 1 PAUSE
ofront+ -lse ..\Mod\OfrontOPP.Mod
@IF errorlevel 1 PAUSE
ofront+ -lse ..\Mod\OfrontOPC.Mod
@IF errorlevel 1 PAUSE
ofront+ -lse ..\Mod\OfrontOPV.Mod
@IF errorlevel 1 PAUSE
ofront+ -lmse ..\Mod\OCatCmd.Mod
@IF errorlevel 1 PAUSE
ofront+ -lmse ..\Mod\OfrontCmd.Mod
@IF errorlevel 1 PAUSE
ofront+ -lmse ..\Mod\BrowserCmd.Mod
@IF errorlevel 1 PAUSE
