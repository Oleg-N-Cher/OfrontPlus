@ECHO OFF
SET PATH=%CD%\..\..\Bin
CD ..\Obj64
SET OBERON=%CD%;%CD%\..\Lib\Sym64;%CD%\..\Sym64;%CD%\..\..\Mod

ofront+ -e OfrontErrors.Mod OfrontOPM.cmdln.Mod OfrontOPS.Mod OfrontOPT.Mod OfrontOPB.Mod OfrontOPP.Mod OfrontOPC.Mod OfrontOPV.Mod BrowserCmd.Mod -m OCatCmd.Mod -m OfrontCmd.Mod -m
IF errorlevel 1 PAUSE

FOR %%i IN (*.sym) DO MOVE /Y %%i ..\Sym64 >NUL
