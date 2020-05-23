@ECHO OFF
IF NOT "%XDev%"=="" GOTO XDev
ECHO Please set system variable XDev=X:\Path\To\XDev
PAUSE
EXIT

:XDev
SET txt=%XDev%\Bin\odc2txt.exe
%txt% Mod\Browser.odc Mod\Text\Browser.txt
IF errorlevel 1 PAUSE
%txt% Mod\Cmd.odc Mod\Text\Cmd.txt
IF errorlevel 1 PAUSE
%txt% Mod\OPB.odc ..\..\Mod\OfrontOPB.Mod
IF errorlevel 1 PAUSE
%txt% Mod\OPC.odc ..\..\Mod\OfrontOPC.Mod
IF errorlevel 1 PAUSE
%txt% Mod\OPM.odc Mod\Text\OPM.txt
IF errorlevel 1 PAUSE
%txt% Mod\OPP.odc ..\..\Mod\OfrontOPP.Mod
IF errorlevel 1 PAUSE
%txt% Mod\OPS.odc ..\..\Mod\OfrontOPS.Mod
IF errorlevel 1 PAUSE
%txt% Mod\OPT.odc ..\..\Mod\OfrontOPT.Mod
IF errorlevel 1 PAUSE
%txt% Mod\OPV.odc ..\..\Mod\OfrontOPV.Mod
IF errorlevel 1 PAUSE
%txt% Rsrc\Strings.odc Rsrc\Text\Strings.txt
IF errorlevel 1 PAUSE
