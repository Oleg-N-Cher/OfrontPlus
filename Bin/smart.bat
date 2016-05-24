@REM args:
@REM   LibName ModName [PartName] [-noinit] [-nocut]

@SET RootBin=..\..\Bin
@SET WinDevBin=..\..\WinDev\Bin
@SET tcc=%WinDevBin%\tcc\tcc.exe

%RootBin%\smartlib %2.c %3 %4 %5

@FOR %%i IN (%2_???.c) DO (
  %tcc% -c %%i -I "."
  @IF errorlevel 1 PAUSE
)
@FOR %%i IN (%2_???.o) DO %WinDevBin%\ar -rc %1 %%i
@Bin\clear