@REM args:
@REM   LibName ModName [PartName] [-noinit] [-nocut]

@SET RootBin=%XDev%\Bin
@SET WinDevBin=%XDev%\WinDev\Bin
@SET DJGPP=%XDev%\WinDev\Bin\DJGPP\djgpp.env
@SET PATH=%XDev%\WinDev\Bin\DJGPP\bin;%PATH%
@SET gcc=gcc.exe @..\Bin\djgpp.opt

%RootBin%\smartlib %2.c %3 %4 %5

@FOR %%i IN (%2_???.c) DO (
  %gcc% -c %%i -I "." -I Obj
  @IF errorlevel 1 PAUSE
)
@FOR %%i IN (%2_???.o) DO ar -rc %1 %%i

@..\Bin\clear
