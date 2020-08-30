@ECHO OFF
CD ..\Obj
SET PATH=..\..\Win32;%PATH%
SET OBERON=.;..\Lib\Sym;..\Sym;..\..\..\Mod

SET oef=
IF EXIST ..\oef.exe SET oef=^| oef

ofront+ -sC -88 OfrontErrors.Mod OfrontOPM.cmdln.Mod OfrontOPS.Mod OfrontOPT.Mod OfrontOPB.Mod OfrontOPP.Mod OfrontOPC.Mod OfrontOPV.Mod BrowserCmd.Mod -m ImportGraph.Mod -m OCatCmd.Mod -m OfrontCmd.Mod -m OdcToText.Ob3 -3m OEF.Ob2 -2m %oef%
IF errorlevel 1 PAUSE

FOR %%i IN (*.sym) DO MOVE /Y %%i ..\Sym >NUL
