Ofront(tm) 1.2.3 for Windows
============================
running as a subsystem under BlackBox Component Builder 1.4
(c) Copyright Software Templ, 1994-2002


Ofront is a tool that translates Oberon-2 programs into 
semantically equivalent C programs. Ofront for Windows is 
an implementation of Ofront for PCs running under BlackBox 
Component Builder for Microsoft Windows 95, 98, NT, 2000 or XP. 
For more detailed information on how Ofront works please 
refere to the "Ofront - Oberon-2 to C Translator, User Guide" 
and to the documentation included in this version.


Differences to previous version
===============================
+ Version 1.2.3: path separator changed from blank to semicolon
+ bug fix for various kinds of symbol file changes
+ Version 1.2.2: uses BlackBox 1.4 as a host system
+ Version 1.2.1: uses BlackBox 1.3.2 as a host system
+ Version 1.2: supports Ofront 1.2 with multidimensional open arrays and ANSII-style prototypes
+ uses BlackBox 1.4 rather than Oberon/F as a host system
+ portable symbol file format allows for cross translation to different 
  target platform, e.g. Unix
+ easier to install due to automatic menu bar extensions in BlackBox 1.4
+ several fixes and updates
+ navigation tool included for convenient source code browsing
- command OpenDocuments has been removed since it is available in Dev->Open File List
- standard BlackBox error markers are used instead of specific Ofront error markers


Installation
============
Ofront for windows runs as a subsystem of the BlackBox Component Builder.
It is assumed that BlackBox component builder 1.4 is installed and operational 
on your machine. A free evaluation version can be downloaded from 
http://www.oberon.ch.
Installation of Ofront then means to create the Ofront subsystem. 
For that, simply copy the distribution directory (rooted in a directory 
named Ofront) into the BlackBox installation directory. Ofront must be 
available at the same directory level as other subsystems such as Text, 
Dev or Host. In order to extend the menu bar with the Ofront menu simply 
restart BlackBox or activate the command Info->Update All Menus from 
within the running BlackBox process. In order to make the Navigator 
tool available (see below) you also have to install the Nav subsystem. 
(Acknowledgement: The Navigator tool has been based on a similar tool 
provided by Guy Laden.)


Contact
=======
If you encounter any problems with Ofront for Windows please contact

Dr. Josef Templ
Software Templ OEG
Voeststr. 33
4060 Leonding, AUSTRIA

fon [0 [043] 732] 676081
fax [0 [043] 732] 676082
GSM: 0664 3841348
e-mail: Josef.Templ@aon.at

http://members.aon.at/software-templ