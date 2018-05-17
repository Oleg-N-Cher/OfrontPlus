@ECHO OFF
SET txt=..\Bin\odc2txt.exe
%txt% ..\Ofront\Mod\Browser.odc ..\Ofront\Mod\Text\Browser.txt
IF errorlevel 1 PAUSE
%txt% ..\Ofront\Mod\Cmd.odc ..\Ofront\Mod\Text\Cmd.txt
IF errorlevel 1 PAUSE
%txt% ..\Ofront\Mod\OPB.odc ..\Ofront\Mod\Text\OPB.txt
IF errorlevel 1 PAUSE
%txt% ..\Ofront\Mod\OPC.odc ..\Ofront\Mod\Text\OPC.txt
IF errorlevel 1 PAUSE
%txt% ..\Ofront\Mod\OPM.odc ..\Ofront\Mod\Text\OPM.txt
IF errorlevel 1 PAUSE
%txt% ..\Ofront\Mod\OPP.odc ..\Ofront\Mod\Text\OPP.txt
IF errorlevel 1 PAUSE
%txt% ..\Ofront\Mod\OPS.odc ..\Ofront\Mod\Text\OPS.txt
IF errorlevel 1 PAUSE
%txt% ..\Ofront\Mod\OPT.odc ..\Ofront\Mod\Text\OPT.txt
IF errorlevel 1 PAUSE
%txt% ..\Ofront\Mod\OPV.odc ..\Ofront\Mod\Text\OPV.txt
IF errorlevel 1 PAUSE
%txt% ..\Ofront\Rsrc\Strings.odc ..\Ofront\Rsrc\Text\Strings.txt
IF errorlevel 1 PAUSE
