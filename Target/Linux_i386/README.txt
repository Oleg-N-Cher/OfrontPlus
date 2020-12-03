The directory structure of the project Ofront+ is as follows:

Linux_x86/
    /Bin      Script that builds Ofront+ and other tools for 32 bit Unix/Linux
    /Lib/Bin  Build scripts for libOfront
    /Lib/Obj  Working directory for library object files
    /Lib/Sym  Library symbol files
    /Examples of programs written in Oberon (to build, use /Bin/build)
    /Obj      Working directory for Ofront+ object files
    /Sym      Ofront+ symbol files

To build Ofront+ for 32 bits under 64-bit Linux, first run:

    sudo apt-get install g++-7-multilib

