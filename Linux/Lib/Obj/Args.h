/* Ofront 1.2 -xtspkael */

#ifndef Args__h
#define Args__h

#include "SYSTEM.h"

typedef
	SYSTEM_PTR (*Args_ANYPTR)[1];


import INTEGER Args_argc;
import Args_ANYPTR Args_argv;


import void Args_Get (INTEGER n, CHAR *val, LONGINT val__len);
import void Args_GetEnv (CHAR *var, LONGINT var__len, CHAR *val, LONGINT val__len);
import void Args_GetInt (INTEGER n, INTEGER *val);
import void Args_GetLInt (INTEGER n, LONGINT *val);
import INTEGER Args_Pos (CHAR *s, LONGINT s__len);
import void *Args__init(void);


#endif
