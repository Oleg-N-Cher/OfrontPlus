#!/bin/bash
export PATH="$PATH:../Bin"
export OBERON=.:../Lib/Sym
export CC="gcc -I . -I../../Lib/Mod -I../Lib/Obj -L../Lib -s -Os -fno-exceptions -fno-asynchronous-unwind-tables -Wl,--gc-sections"

ofront+ -m ../../Samples/BlitBmp.Mod
if [ $? != 0 ]; then exit 1; fi
$CC BlitBmp.c -o BlitBmp -lOfront -ldl -I/usr/include/SDL2 -L/usr/lib -lSDL2 -lSDL2_image
if [ $? != 0 ]; then exit 1; fi
