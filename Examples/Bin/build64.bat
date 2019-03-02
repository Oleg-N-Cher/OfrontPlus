@ECHO OFF
CD ..\Obj64
SET PATH=..\..\Bin;%PATH%
IF NOT "%XDev%"=="" SET PATH=%XDev%\WinDev\Bin\MinGW64\bin;%PATH%
SET OBERON=.;.\..\..\Lib\Sym;.\..\Mod

ofront+ -sC -88 MyDLL.Mod -d MyDLLuse.Mod -m
IF errorlevel 1 PAUSE
ofront+ -mC -88 BlitBmp.Mod BlitBmp2.Mod Book.Mod Cnezinka.Mod Elo4ka.Mod Gradient24.Mod HelloWorld.Mod Mandelbrot.Mod Mandelbrot2.Mod TestHttp.Mod
IF errorlevel 1 PAUSE

SET StripExe=-nostartfiles ..\..\Lib\Mod\crt1.c -Wl,-e_WinMain@16 -D_WINMAIN
SET StripDll=-nostartfiles ..\..\Lib\Mod\crt1dll.c
SET LIB=-I..\..\Lib\Obj64 -I..\..\Lib\Mod ..\..\Lib\Ofront64.a
SET CC=gcc.exe -m64 -s -Os -g0 -fvisibility=hidden -fomit-frame-pointer -finline-small-functions -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -Wl,--gc-sections -Wl,--file-alignment,512

%CC% %StripExe% HelloWorld.c -o..\x64\HelloWorld.exe %LIB%
IF errorlevel 1 PAUSE

:: ------------ Generate MyDLL.a (because of the circular dependency) -------------
%CC% %StripDll% MyDLL.c -fPIC -shared -o..\x64\MyDLL.dll -Wl,--out-implib,MyDLL.a %LIB%
IF errorlevel 1 PAUSE
:: ------------ Generate MyDLLuse.exe & MyDLLuse.a -------------
%CC% %StripExe% MyDLLuse.c -fPIC MyDLLuse.def -o..\x64\MyDLLuse.exe -Wl,--out-implib,MyDLLuse.a %LIB% MyDLL.a
IF errorlevel 1 PAUSE
:: ------------ Generate MyDLL.dll -------------
%CC% %StripDll% MyDLL.c -fPIC -shared MyDLLuse.a -o..\x64\MyDLL.dll -Wl,--out-implib,MyDLL.a -I..\..\Lib\Obj -I..\..\Lib\Mod
IF errorlevel 1 PAUSE
:: --------------------------------------------------------------------------------

%CC% %StripExe% TestHttp.c -o..\x64\TestHttp.exe %LIB% %XDev%\WinDev\Bin\MinGW64\lib\libcurldll.a
IF errorlevel 1 PAUSE
%CC% %StripExe% BlitBmp.c -o..\x64\BlitBmp.exe %LIB% %XDev%\WinDev\Bin\MinGW64\lib\libSDL2.dll.a %XDev%\WinDev\Bin\MinGW64\lib\libSDL2_image.dll.a
IF errorlevel 1 PAUSE
%CC% %StripExe% BlitBmp2.c -o..\x64\BlitBmp2.exe %LIB% %XDev%\WinDev\Bin\MinGW64\lib\libSDL2.dll.a %XDev%\WinDev\Bin\MinGW64\lib\libSDL2_image.dll.a
IF errorlevel 1 PAUSE
%CC% %StripExe% Book.c -o..\x64\Book.exe %LIB% %XDev%\WinDev\Bin\MinGW64\lib\libSDL2.dll.a %XDev%\WinDev\Bin\MinGW64\lib\libSDL2_image.dll.a
IF errorlevel 1 PAUSE
%CC% %StripExe% Cnezinka.c -o..\x64\Cnezinka.exe %LIB% %XDev%\WinDev\Bin\MinGW64\lib\libSDL2.dll.a %XDev%\WinDev\Bin\MinGW64\lib\libSDL2_image.dll.a
IF errorlevel 1 PAUSE
%CC% %StripExe% Elo4ka.c -o..\x64\Elo4ka.exe %LIB% %XDev%\WinDev\Bin\MinGW64\lib\libSDL2.dll.a %XDev%\WinDev\Bin\MinGW64\lib\libSDL2_image.dll.a
IF errorlevel 1 PAUSE
%CC% %StripExe% Gradient24.c -o..\x64\Gradient24.exe %LIB% %XDev%\WinDev\Bin\MinGW64\lib\libSDL2.dll.a %XDev%\WinDev\Bin\MinGW64\lib\libSDL2_image.dll.a
IF errorlevel 1 PAUSE
%CC% %StripExe% Mandelbrot.c -o..\x64\Mandelbrot.exe %LIB% %XDev%\WinDev\Bin\MinGW64\lib\libSDL2.dll.a %XDev%\WinDev\Bin\MinGW64\lib\libSDL2_image.dll.a
IF errorlevel 1 PAUSE
%CC% %StripExe% Mandelbrot2.c -o..\x64\Mandelbrot2.exe %LIB% %XDev%\WinDev\Bin\MinGW64\lib\libSDL2.dll.a %XDev%\WinDev\Bin\MinGW64\lib\libSDL2_image.dll.a
IF errorlevel 1 PAUSE
