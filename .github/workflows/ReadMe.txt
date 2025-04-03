To rebuild Ofront+ on a new architecture using C bootstrap sources

Download and unpack the latest "master" branch archive from

  https://github.com/Oleg-N-Cher/OfrontPlus

Mix with the content of this repository

  https://github.com/Oleg-N-Cher/OfrontPlus-bootstrap

For a 64-bit OS we are interested in the unix-88 folder.
Everything there is in one pile, but for simplicity you copy all these files from

  unix-88 (for 64-bit OS)

to the local Obj and THE SAME FILES to Lib/Obj in the folder of your archiecture:

  for example, Linux_amd64

Then run Lib/Bin/cc and Bin/cc (inside Target/Linux_amd64 folder)

After that you can clean the Obj folders of unnecessary files.
Then rebuild Ofront+ with Bin/build and libraries with Lib/Bin/build
