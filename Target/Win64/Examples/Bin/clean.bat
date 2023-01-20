@CD "%~dp0"
CD ..\Obj
DEL /Q *.a
DEL /Q *.c
DEL /Q *.h
DEL /Q *.sym

@CD ..
DEL /Q *.exe
DEL /Q MyDLL.dll
