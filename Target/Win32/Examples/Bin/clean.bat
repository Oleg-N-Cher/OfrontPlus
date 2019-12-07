@CD ..\Obj
DEL /Q *.a
DEL /Q *.c
DEL /Q *.h
DEL /Q *.sym

@CD ..\Obj64
DEL /Q *.a
DEL /Q *.c
DEL /Q *.h
DEL /Q *.sym

@CD ..\x64
DEL /Q *.exe
DEL /Q MyDLL.dll

@CD ..
DEL /Q *.exe
DEL /Q MyDLL.dll
