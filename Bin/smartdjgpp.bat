@REM args:
@REM   LibName ModName [PartName] [-noinit] [-nocut]

@SET RootBin=..\..\Bin
@SET Bin=..\Bin
@SET DJGPP=d:\Archive\Projects\XDev\WinDev\Bin\djgpp\djgpp.env
@SET PATH=d:\Archive\Projects\XDev\WinDev\Bin\djgpp\bin;%PATH%
@SET gcc=gcc.exe @Bin\djgpp.opt

@IF EXIST %2.c GOTO clib

:olib
@COPY Obj\%2.h
%RootBin%\smartlib Obj\%2.c %3 %4 %5
@GOTO compile

:clib
%RootBin%\smartlib %2.c %3 %4 %5

:compile
@FOR %%i IN (%2_???.c) DO (
  %gcc% -c %%i -I "." -I Obj
  @IF errorlevel 1 PAUSE
)
@FOR %%i IN (%2_???.o) DO %Bin%\ar -rc %1 %%i
@Bin\clear