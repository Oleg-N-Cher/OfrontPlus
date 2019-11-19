The directory structure of the project Ofront+ is as follows:

Linux_x86/
    /Bin      Scripts for build Ofront+ tools for Unix/Linux 32 bit on x86
    /Lib/Bin  Scripts for build libOfront
    /Lib/Obj  The working directory for library object files
    /Lib/Sym  Library symbol files
    /Examples of programs written in Oberon (to build, use /Bin/build)
    /Obj      The working directory for Ofront+ object files
    /Sym      Ofront+ symbol files

To build Ofront+ for 32 bits under 64-bit Linux, use:

    sudo apt-get install g++-7-multilib

