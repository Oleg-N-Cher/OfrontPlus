@ECHO OFF
SET PATH=%CD%\..\..\Win32
CD ..\Obj
SET OBERON=%CD%;%CD%\..\Lib\Sym;%CD%\..\Sym;%CD%\..\..\..\Mod

ofront+ -s88 ^
  OfrontErrors.cp OfrontOPM.cmdln.cp -G OfrontOPS.cp OfrontOPT.cp OfrontOPB.cp ^
  OfrontOPP.cp OfrontOPC.cp OfrontOPV.cp BrowserCmd.cp -m ImportGraph.cp -m ^
  OCatCmd.cp -m OfrontCmd.cp -m OdcToText.ob3 -m OEF.ob2 -m OMake.ob2 -m
IF errorlevel 1 PAUSE

FOR %%i IN (*.sym) DO MOVE /Y %%i ..\Sym >NUL
