/* Ofront+ 0.9 -xtspkae */
#include "SYSTEM.h"
#include "Platform.h"


export INTEGER Args_argc;
export Platform_ArgVec Args_argv;


export void Args_Get (INTEGER n, CHAR *val, INTEGER val__len);
export void Args_GetEnv (CHAR *var, INTEGER var__len, CHAR *val, INTEGER val__len);
export void Args_GetInt (INTEGER n, INTEGER *val);
export INTEGER Args_Pos (CHAR *s, INTEGER s__len);


/*============================================================================*/

void Args_Get (INTEGER n, CHAR *val, INTEGER val__len)
{
	Platform_GetArg(n, (void*)val, val__len);
}

/*----------------------------------------------------------------------------*/
void Args_GetInt (INTEGER n, INTEGER *val)
{
	Platform_GetIntArg(n, &*val);
}

/*----------------------------------------------------------------------------*/
INTEGER Args_Pos (CHAR *s, INTEGER s__len)
{
	return Platform_ArgPos(s, s__len);
}

/*----------------------------------------------------------------------------*/
void Args_GetEnv (CHAR *var, INTEGER var__len, CHAR *val, INTEGER val__len)
{
	Platform_GetEnv(var, var__len, (void*)val, val__len);
}

/*----------------------------------------------------------------------------*/

export void *Args__init(void)
{
	__DEFMOD;
	__IMPORT(Platform__init);
	__REGMOD("Args", 0);
/* BEGIN */
	Args_argc = Platform_ArgCount;
	Args_argv = Platform_ArgVector;
	__ENDMOD;
}
