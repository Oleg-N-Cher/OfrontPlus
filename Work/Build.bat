@SET PATH=..\Bin;%PATH%
ofront -lm Test.Mod
ECHO %errorlevel%
@PAUSE
