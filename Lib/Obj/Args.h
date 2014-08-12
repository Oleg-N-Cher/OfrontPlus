/*  Ofront 1.2 -xtspkae */

#ifndef Args__h
#define Args__h

#include "SYSTEM.h"


import LONGINT Args_argc, Args_argv;


import void Args_Get (INTEGER n, CHAR *val, LONGINT val__len);
import void Args_GetEnv (CHAR *var, LONGINT var__len, CHAR *val, LONGINT val__len);
import void Args_GetInt (INTEGER n, LONGINT *val);
import INTEGER Args_Pos (CHAR *s, LONGINT s__len);
import void *Args__init(void);


#endif
