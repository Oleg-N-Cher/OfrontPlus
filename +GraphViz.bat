@ECHO OFF
SET GraphViz=%XDev%\Bin\graphviz

Bin\graphviz.exe -r Platform.Windows OfrontOPM.cmdln OfrontCmd > OfrontPlus.dot
"%GraphViz%\bin\dot" -Tpng OfrontPlus.dot > OfrontPlus.png

::"%GraphViz%\bin\fdp" -Tpng OfrontPlus.dot > OfrontPlus.png
::"%GraphViz%\bin\neato" -Tpng OfrontPlus.dot > OfrontPlus.png
::"%GraphViz%\bin\circo" -Tpng OfrontPlus.dot > OfrontPlus.png
::"%GraphViz%\bin\twopi" -Tpng OfrontPlus.dot > OfrontPlus.png

START OfrontPlus.png
