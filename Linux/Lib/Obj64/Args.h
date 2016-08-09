/* Ofront+ 0.9 -xtspkae */

#ifndef Args__h
#define Args__h

#include "SYSTEM.h"
#include "Platform.h"


import INTEGER Args_argc;
import Platform_ArgVec Args_argv;


import void Args_Get (INTEGER n, CHAR *val, INTEGER val__len);
import void Args_GetEnv (CHAR *var, INTEGER var__len, CHAR *val, INTEGER val__len);
import void Args_GetInt (INTEGER n, INTEGER *val);
import INTEGER Args_Pos (CHAR *s, INTEGER s__len);
import void *Args__init(void);


#endif
