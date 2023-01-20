@ECHO OFF
IF NOT "%XDev%"=="" GOTO XDev
ECHO Please set system variable XDev=X:\Path\To\XDev
PAUSE
EXIT

:XDev

CD "%~dp0"
SET txt=..\Win32\odc2txt.exe
%txt% Mod\Browser.odc Mod\Text\Browser.cp
IF errorlevel 1 PAUSE
%txt% Mod\Cmd.odc Mod\Text\Cmd.cp
IF errorlevel 1 PAUSE
%txt% Mod\OPB.odc ..\..\Mod\OfrontOPB.cp
IF errorlevel 1 PAUSE
%txt% Mod\OPC.odc ..\..\Mod\OfrontOPC.cp
IF errorlevel 1 PAUSE
%txt% Mod\OPM.odc Mod\Text\OPM.cp
IF errorlevel 1 PAUSE
%txt% Mod\OPP.odc ..\..\Mod\OfrontOPP.cp
IF errorlevel 1 PAUSE
%txt% Mod\OPS.odc ..\..\Mod\OfrontOPS.cp
IF errorlevel 1 PAUSE
%txt% Mod\OPT.odc ..\..\Mod\OfrontOPT.cp
IF errorlevel 1 PAUSE
%txt% Mod\OPV.odc ..\..\Mod\OfrontOPV.cp
IF errorlevel 1 PAUSE
%txt% Rsrc\Strings.odc Rsrc\Text\Strings.txt
IF errorlevel 1 PAUSE
