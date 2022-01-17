@ECHO OFF
CD ..\Obj
SET PATH=..\..\Win32;%PATH%
SET OBERON=.;..\Lib\Sym;..\Sym;..\..\..\Mod

SET oef=
::IF EXIST ..\oef.exe SET oef=^| oef

ofront+ -sC -88 ^
  OfrontErrors.cp OfrontOPM.cmdln.cp -G OfrontOPS.cp OfrontOPT.cp OfrontOPB.cp ^
  OfrontOPP.cp OfrontOPC.cp OfrontOPV.cp BrowserCmd.cp -m ImportGraph.cp -m ^
  OCatCmd.cp -m OfrontCmd.cp -m OdcToText.ob3 -m OEF.ob2 -m OMake.ob2 -m ^
  %oef%
IF errorlevel 1 PAUSE

FOR %%i IN (*.sym) DO MOVE /Y %%i ..\Sym >NUL
