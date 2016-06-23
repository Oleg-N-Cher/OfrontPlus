/*  Ofront 1.2 -xtspkae */

#ifndef Watson0__h
#define Watson0__h

#include "SYSTEM.h"
#include "CmdlnTexts.h"


import CHAR Watson0_options[12];


import void Watson0_ShowCmd (CHAR *file, LONGINT file__len, CmdlnTexts_Text T);
import void Watson0_ShowDef (CHAR *mod, LONGINT mod__len, CmdlnTexts_Text T);
import void Watson0_ShowObj (CHAR *mod, LONGINT mod__len, CHAR *objName, LONGINT objName__len, CmdlnTexts_Text T);
import void *Watson0__init(void);


#endif
