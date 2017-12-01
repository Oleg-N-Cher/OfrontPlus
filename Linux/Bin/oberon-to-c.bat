@ECHO OFF
SET PATH=%CD%\..\..\Bin
CD ..\Obj
SET OBERON=%CD%;%CD%\..\Lib\Sym;%CD%\..\Sym;%CD%\..\..\Mod

ofront+ -e OfrontErrors.Mod OfrontOPM.cmdln.Mod OfrontOPS.Mod OfrontOPT.Mod OfrontOPB.Mod OfrontOPP.Mod OfrontOPC.Mod OfrontOPV.Mod BrowserCmd.Mod -m OCatCmd.Mod -m OfrontCmd.Mod -m
IF errorlevel 1 PAUSE

FOR %%i IN (*.sym) DO MOVE /Y %%i ..\Sym >NUL
