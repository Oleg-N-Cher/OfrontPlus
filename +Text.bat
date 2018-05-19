@ECHO OFF
SET txt=..\Bin\odc2txt.exe
%txt% Mod\Browser.odc Mod\Text\Browser.txt
IF errorlevel 1 PAUSE
%txt% Mod\Cmd.odc Mod\Text\Cmd.txt
IF errorlevel 1 PAUSE
%txt% Mod\OPB.odc Mod\Text\OPB.txt
IF errorlevel 1 PAUSE
%txt% Mod\OPC.odc Mod\Text\OPC.txt
IF errorlevel 1 PAUSE
%txt% Mod\OPM.odc Mod\Text\OPM.txt
IF errorlevel 1 PAUSE
%txt% Mod\OPP.odc Mod\Text\OPP.txt
IF errorlevel 1 PAUSE
%txt% Mod\OPS.odc Mod\Text\OPS.txt
IF errorlevel 1 PAUSE
%txt% Mod\OPT.odc Mod\Text\OPT.txt
IF errorlevel 1 PAUSE
%txt% Mod\OPV.odc Mod\Text\OPV.txt
IF errorlevel 1 PAUSE
%txt% Rsrc\Strings.odc Rsrc\Text\Strings.txt
IF errorlevel 1 PAUSE
