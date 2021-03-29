@ECHO OFF
CD ..\Obj
SET PATH=..;%PATH%
SET OBERON=.;..\Lib\Sym;..\Sym;..\..\..\Mod

SET oef=
IF EXIST ..\oef.exe SET oef=^| oef

ofront+ -sC -48 ^
  OfrontErrors.cp OfrontOPM.cmdln.cp -G OfrontOPS.cp OfrontOPT.cp OfrontOPB.cp ^
  OfrontOPP.cp OfrontOPC.cp OfrontOPV.cp BrowserCmd.cp -m ImportGraph.cp -m ^
  OCatCmd.cp -m OfrontCmd.cp -m OdcToText.ob3 -3m OEF.ob2 -2m OMake.ob2 -2m ^
  %oef%
IF errorlevel 1 PAUSE

FOR %%i IN (*.sym) DO MOVE /Y %%i ..\Sym >NUL
