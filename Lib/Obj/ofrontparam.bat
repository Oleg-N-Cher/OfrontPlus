:tcc
@SET WinDevBin=..\..\..\WinDev\Bin
%WinDevBin%\tcc\tcc ofrontparam.c -I ..\Lib\Obj -o ofrontparamt.exe
@IF errorlevel 1 PAUSE

:mingw
@SET gccBin=d:\WIN32APP\Dev\Dev-Cpp\bin
@SET gcc=%GccBin%\gcc.exe -Os
%gcc% ofrontparam.c -I ..\Lib\Obj -o ofrontparam.exe
@IF errorlevel 1 PAUSE
