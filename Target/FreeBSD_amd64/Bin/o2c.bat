@ECHO OFF
SET PATH=%CD%\..\..\Win32
CD ..\Obj
SET OBERON=%CD%;%CD%\..\Lib\Sym;%CD%\..\Sym;%CD%\..\..\..\Mod

ofront+ -sC -88 OfrontErrors.Mod OfrontOPM.cmdln.Mod OfrontOPS.Mod OfrontOPT.Mod OfrontOPB.Mod OfrontOPP.Mod OfrontOPC.Mod OfrontOPV.Mod BrowserCmd.Mod -m ImportGraph.Mod -m OCatCmd.Mod -m OfrontCmd.Mod -m OdcToText.Mod -3m
IF errorlevel 1 PAUSE

FOR %%i IN (*.sym) DO MOVE /Y %%i ..\Sym >NUL
