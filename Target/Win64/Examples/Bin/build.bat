@ECHO OFF
CD ..\Obj
SET PATH=..\..\..\Win32;%PATH%
IF NOT "%XDev%"=="" SET PATH=%XDev%\WinDev\Bin\MinGW64\bin;%PATH%
SET OBERON=.;..\..\Lib\Sym;..\..\..\..\Examples

ofront+ -sC -88 MyDLL.cp -d MyDLLuse.cp -m
IF errorlevel 1 PAUSE
ofront+ -mC -88 BlitBmp.cp BlitBmp2.cp Book.cp Cnezinka.cp Elo4ka.cp Gradient24.cp HelloWorld.cp Mandelbrot.cp Mandelbrot2.cp TestHttp.cp
IF errorlevel 1 PAUSE

SET StripExe=-nostartfiles ..\..\..\..\Mod\Lib\crt1.c -Wl,-eWinMain
SET StripDll=-nostartfiles ..\..\..\..\Mod\Lib\crt1dll.c
SET LIB=-I..\..\Lib\Obj -I..\..\..\..\Mod\Lib ..\..\Lib\Ofront.a
SET CC=gcc.exe -m64 -s -Os -g0 -fvisibility=hidden -fomit-frame-pointer -finline-small-functions -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -Wl,--gc-sections

%CC% %StripExe% HelloWorld.c -o..\HelloWorld.exe %LIB%
IF errorlevel 1 PAUSE

:: ------------ Generate MyDLL.a (because of the circular dependency) -------------
%CC% %StripDll% MyDLL.c -fPIC -shared -o..\MyDLL.dll -Wl,--out-implib,MyDLL.a %LIB%
IF errorlevel 1 PAUSE
:: ------------ Generate MyDLLuse.exe & MyDLLuse.a -------------
%CC% %StripExe% MyDLLuse.c -fPIC MyDLLuse.def -o..\MyDLLuse.exe -Wl,--out-implib,MyDLLuse.a %LIB% MyDLL.a
IF errorlevel 1 PAUSE
:: ------------ Generate MyDLL.dll -------------
%CC% %StripDll% MyDLL.c -fPIC -shared MyDLLuse.a -o..\MyDLL.dll -Wl,--out-implib,MyDLL.a -I..\..\Lib\Obj -I..\..\..\..\Mod\Lib
IF errorlevel 1 PAUSE
:: --------------------------------------------------------------------------------

%CC% %StripExe% TestHttp.c -o..\TestHttp.exe %LIB% %XDev%\WinDev\Bin\MinGW64\lib\libcurldll.a
IF errorlevel 1 PAUSE
%CC% %StripExe% BlitBmp.c -o..\BlitBmp.exe -mwindows %LIB% %XDev%\WinDev\Bin\MinGW64\lib\libSDL2.dll.a %XDev%\WinDev\Bin\MinGW64\lib\libSDL2_image.dll.a
IF errorlevel 1 PAUSE
%CC% %StripExe% BlitBmp2.c -o..\BlitBmp2.exe -mwindows %LIB% %XDev%\WinDev\Bin\MinGW64\lib\libSDL2.dll.a %XDev%\WinDev\Bin\MinGW64\lib\libSDL2_image.dll.a
IF errorlevel 1 PAUSE
%CC% %StripExe% Book.c -o..\Book.exe -mwindows %LIB% %XDev%\WinDev\Bin\MinGW64\lib\libSDL2.dll.a %XDev%\WinDev\Bin\MinGW64\lib\libSDL2_image.dll.a
IF errorlevel 1 PAUSE
%CC% %StripExe% Cnezinka.c -o..\Cnezinka.exe -mwindows %LIB% %XDev%\WinDev\Bin\MinGW64\lib\libSDL2.dll.a %XDev%\WinDev\Bin\MinGW64\lib\libSDL2_image.dll.a
IF errorlevel 1 PAUSE
%CC% %StripExe% Elo4ka.c -o..\Elo4ka.exe -mwindows %LIB% %XDev%\WinDev\Bin\MinGW64\lib\libSDL2.dll.a %XDev%\WinDev\Bin\MinGW64\lib\libSDL2_image.dll.a
IF errorlevel 1 PAUSE
%CC% %StripExe% Gradient24.c -o..\Gradient24.exe -mwindows %LIB% %XDev%\WinDev\Bin\MinGW64\lib\libSDL2.dll.a %XDev%\WinDev\Bin\MinGW64\lib\libSDL2_image.dll.a
IF errorlevel 1 PAUSE
%CC% %StripExe% Mandelbrot.c -o..\Mandelbrot.exe -mwindows %LIB% %XDev%\WinDev\Bin\MinGW64\lib\libSDL2.dll.a %XDev%\WinDev\Bin\MinGW64\lib\libSDL2_image.dll.a
IF errorlevel 1 PAUSE
%CC% %StripExe% Mandelbrot2.c -o..\Mandelbrot2.exe -mwindows %LIB% %XDev%\WinDev\Bin\MinGW64\lib\libSDL2.dll.a %XDev%\WinDev\Bin\MinGW64\lib\libSDL2_image.dll.a
IF errorlevel 1 PAUSE
