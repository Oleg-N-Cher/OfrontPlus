@ECHO OFF
CD ..\Obj
SET PATH=..\..\Bin;%PATH%
IF NOT "%XDev%"=="" SET PATH=%XDev%\WinDev\Bin\MinGW\bin;%PATH%
SET OBERON=.;.\..\..\Lib\Sym;.\..\Mod

ofront+ -sC -48 MyDLL.Mod -d MyDLLuse.Mod -m
IF errorlevel 1 PAUSE
ofront+ -mC -48 BlitBmp.Mod BlitBmp2.Mod Book.Mod Cnezinka.Mod Elo4ka.Mod Gradient24.Mod HelloWorld.Mod Mandelbrot.Mod Mandelbrot2.Mod TestHttp.Mod
IF errorlevel 1 PAUSE

SET StripExe=-nostartfiles ..\..\Lib\Mod\crt1.c -Wl,-e__WinMain -D_WINMAIN
SET StripDll=-nostartfiles ..\..\Lib\Mod\crt1dll.c -static-libgcc
SET LIB=-I..\..\Lib\Obj -I..\..\Lib\Mod ..\..\Lib\Ofront.a
SET CC=gcc.exe -m32 -s -Os -g0 -fvisibility=hidden -fomit-frame-pointer -finline-small-functions -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -Wl,--gc-sections

%CC% %StripExe% HelloWorld.c -o..\HelloWorld.exe %LIB%
IF errorlevel 1 PAUSE

:: ------------ Generate MyDLL.a (because of the circular dependency) -------------
%CC% %StripDll% MyDLL.c -fPIC -shared -o..\MyDLL.dll -Wl,--out-implib,MyDLL.a %LIB%
IF errorlevel 1 PAUSE
:: ------------ Generate MyDLLuse.exe & MyDLLuse.a -------------
%CC% %StripExe% MyDLLuse.c -fPIC MyDLLuse.def -o..\MyDLLuse.exe -Wl,--out-implib,MyDLLuse.a %LIB% MyDLL.a
IF errorlevel 1 PAUSE
:: ------------ Generate MyDLL.dll -------------
%CC% %StripDll% MyDLL.c -fPIC -shared MyDLLuse.a -o..\MyDLL.dll -Wl,--out-implib,MyDLL.a -I..\..\Lib\Obj -I..\..\Lib\Mod
IF errorlevel 1 PAUSE
:: --------------------------------------------------------------------------------

%CC% %StripExe% TestHttp.c -o..\TestHttp.exe %LIB% %XDev%\WinDev\Bin\MinGW\lib\libcurldll.a
IF errorlevel 1 PAUSE
%CC% %StripExe% BlitBmp.c -o..\BlitBmp.exe -mwindows %LIB% %XDev%\WinDev\Bin\MinGW\lib\libSDL2.dll.a %XDev%\WinDev\Bin\MinGW\lib\libSDL2_image.dll.a
IF errorlevel 1 PAUSE
%CC% %StripExe% BlitBmp2.c -o..\BlitBmp2.exe -mwindows %LIB% %XDev%\WinDev\Bin\MinGW\lib\libSDL2.dll.a %XDev%\WinDev\Bin\MinGW\lib\libSDL2_image.dll.a
IF errorlevel 1 PAUSE
%CC% %StripExe% Book.c -o..\Book.exe -mwindows %LIB% %XDev%\WinDev\Bin\MinGW\lib\libSDL2.dll.a %XDev%\WinDev\Bin\MinGW\lib\libSDL2_image.dll.a
IF errorlevel 1 PAUSE
%CC% %StripExe% Cnezinka.c -o..\Cnezinka.exe -mwindows %LIB% %XDev%\WinDev\Bin\MinGW\lib\libSDL2.dll.a %XDev%\WinDev\Bin\MinGW\lib\libSDL2_image.dll.a
IF errorlevel 1 PAUSE
%CC% %StripExe% Elo4ka.c -o..\Elo4ka.exe -mwindows %LIB% %XDev%\WinDev\Bin\MinGW\lib\libSDL2.dll.a %XDev%\WinDev\Bin\MinGW\lib\libSDL2_image.dll.a
IF errorlevel 1 PAUSE
%CC% %StripExe% Gradient24.c -o..\Gradient24.exe -mwindows %LIB% %XDev%\WinDev\Bin\MinGW\lib\libSDL2.dll.a %XDev%\WinDev\Bin\MinGW\lib\libSDL2_image.dll.a
IF errorlevel 1 PAUSE
%CC% %StripExe% Mandelbrot.c -o..\Mandelbrot.exe -mwindows %LIB% %XDev%\WinDev\Bin\MinGW\lib\libSDL2.dll.a %XDev%\WinDev\Bin\MinGW\lib\libSDL2_image.dll.a
IF errorlevel 1 PAUSE
%CC% %StripExe% Mandelbrot2.c -o..\Mandelbrot2.exe -mwindows %LIB% %XDev%\WinDev\Bin\MinGW\lib\libSDL2.dll.a %XDev%\WinDev\Bin\MinGW\lib\libSDL2_image.dll.a
IF errorlevel 1 PAUSE
