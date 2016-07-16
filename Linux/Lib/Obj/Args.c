/* Ofront 1.2 -xtspkael */
#include "SYSTEM.h"

typedef
	CHAR (*Args_ArgPtr)[1024];

typedef
	Args_ArgPtr (*Args_ArgVec)[1024];


export INTEGER Args_argc;
export Args_ArgVec Args_argv;


export void Args_Get (INTEGER n, CHAR *val, LONGINT val__len);
export void Args_GetEnv (CHAR *var, LONGINT var__len, CHAR *val, LONGINT val__len);
export void Args_GetInt (INTEGER n, INTEGER *val);
export void Args_GetLongInt (INTEGER n, LONGINT *val);
export INTEGER Args_Pos (CHAR *s, LONGINT s__len);

#include <stdlib.h>
#define Args_Argc()	SYSTEM_argc
#define Args_Argv()	(Args_ArgVec)SYSTEM_argv
#define Args_getenv(var, var__len)	(Args_ArgPtr)getenv((char*)var)

void Args_Get (INTEGER n, CHAR *val, LONGINT val__len)
{
	if (n < Args_argc) {
		__COPY(*(*Args_argv)[__X(n, 1024)], val, val__len);
	}
}

void Args_GetInt (INTEGER n, INTEGER *val)
{
	CHAR s[64];
	INTEGER k, d, i;
	s[0] = 0x00;
	Args_Get(n, (void*)s, 64);
	i = 0;
	if (s[0] == '-') {
		i = 1;
	}
	k = 0;
	d = (INTEGER)s[__X(i, 64)] - 48;
	while (d >= 0 && d <= 9) {
		k = k * 10 + d;
		i += 1;
		d = (INTEGER)s[__X(i, 64)] - 48;
	}
	if (s[0] == '-') {
		d = -d;
		i -= 1;
	}
	if (i > 0) {
		*val = k;
	}
}

void Args_GetLongInt (INTEGER n, LONGINT *val)
{
	CHAR s[64];
	LONGINT k;
	INTEGER d, i;
	s[0] = 0x00;
	Args_Get(n, (void*)s, 64);
	i = 0;
	if (s[0] == '-') {
		i = 1;
	}
	k = 0;
	d = (INTEGER)s[__X(i, 64)] - 48;
	while (d >= 0 && d <= 9) {
		k = k * 10 + (LONGINT)d;
		i += 1;
		d = (INTEGER)s[__X(i, 64)] - 48;
	}
	if (s[0] == '-') {
		d = -d;
		i -= 1;
	}
	if (i > 0) {
		*val = k;
	}
}

INTEGER Args_Pos (CHAR *s, LONGINT s__len)
{
	INTEGER i;
	CHAR arg[256];
	i = 0;
	Args_Get(i, (void*)arg, 256);
	while (i < Args_argc && __STRCMP(s, arg) != 0) {
		i += 1;
		Args_Get(i, (void*)arg, 256);
	}
	return i;
}

void Args_GetEnv (CHAR *var, LONGINT var__len, CHAR *val, LONGINT val__len)
{
	Args_ArgPtr p = NIL;
	p = Args_getenv(var, var__len);
	if (p != NIL) {
		__COPY(*p, val, val__len);
	}
}


export void *Args__init(void)
{
	__DEFMOD;
	__REGMOD("Args", 0);
/* BEGIN */
	Args_argc = Args_Argc();
	Args_argv = Args_Argv();
	__ENDMOD;
}
