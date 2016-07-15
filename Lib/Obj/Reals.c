/*  Ofront 1.2 -xtspkae */
#include "SYSTEM.h"




export void Reals_Convert (REAL x, INTEGER n, CHAR *d, LONGINT d__len);
export void Reals_ConvertH (REAL y, CHAR *d, LONGINT d__len);
export void Reals_ConvertHL (LONGREAL x, CHAR *d, LONGINT d__len);
export void Reals_ConvertL (LONGREAL x, INTEGER n, CHAR *d, LONGINT d__len);
export INTEGER Reals_Expo (REAL x);
export INTEGER Reals_ExpoL (LONGREAL x);
export void Reals_SetExpo (INTEGER e, REAL *x);
export void Reals_SetExpoL (INTEGER e, LONGREAL *x);
export REAL Reals_Ten (INTEGER e);
export LONGREAL Reals_TenL (INTEGER e);
static void Reals_Unpack (BYTE *b, LONGINT b__len, BYTE *d, LONGINT d__len);

#include <stdlib.h>
#define Reals_ecvt(x, ndigit, decpt, sign)	((LONGINT)ecvt (x, ndigit, (int*)(decpt), (int*)(sign)))

/*============================================================================*/

REAL Reals_Ten (INTEGER e)
{
	LONGREAL r, power;
	r = (LONGREAL)1;
	power = (LONGREAL)10;
	while (e > 0) {
		if (__ODD(e)) {
			r = r * power;
		}
		power = power * power;
		e = __ASHR(e, 1, INTEGER);
	}
	return r;
}

/*----------------------------------------------------------------------------*/
LONGREAL Reals_TenL (INTEGER e)
{
	LONGREAL r, power;
	r = (LONGREAL)1;
	power = (LONGREAL)10;
	for (;;) {
		if (__ODD(e)) {
			r = r * power;
		}
		e = __ASHR(e, 1, INTEGER);
		if (e <= 0) {
			return r;
		}
		power = power * power;
	}
	__RETCHK;
}

/*----------------------------------------------------------------------------*/
INTEGER Reals_Expo (REAL x)
{
	return (INTEGER)__MASK(__ASHR(__VAL(LONGINT, x), 23, LONGINT), -256);
}

/*----------------------------------------------------------------------------*/
INTEGER Reals_ExpoL (LONGREAL x)
{
	LONGINT h;
	__GET((INTEGER)&x + 4, h, LONGINT);
	return (INTEGER)__MASK(__ASHR(h, 20, LONGINT), -2048);
}

/*----------------------------------------------------------------------------*/
void Reals_SetExpo (INTEGER e, REAL *x)
{
	*x = (REAL)(__VAL(SET, *x) & ~0x01fe | (SET)__ASHL((LONGINT)e, 23, LONGINT));
}

/*----------------------------------------------------------------------------*/
void Reals_SetExpoL (INTEGER e, LONGREAL *x)
{
	SET h;
	__GET((INTEGER)x + 4, h, SET);
	h = h & ~0x0ffe | (SET)__ASHL((LONGINT)e, 20, LONGINT);
	__PUT((INTEGER)x + 4, h, SET);
}

/*----------------------------------------------------------------------------*/
void Reals_Convert (REAL x, INTEGER n, CHAR *d, LONGINT d__len)
{
	LONGINT i, k;
	i = __ENTIER(x);
	k = 0;
	while (k < (LONGINT)n) {
		d[__X(k, d__len)] = (CHAR)(__MOD(i, 10) + 48);
		i = __DIV(i, 10);
		k += 1;
	}
}

/*----------------------------------------------------------------------------*/
void Reals_ConvertL (LONGREAL x, INTEGER n, CHAR *d, LONGINT d__len)
{
	LONGINT decpt, sign, i, buf;
	buf = Reals_ecvt(x, n + 2, (INTEGER)&decpt, (INTEGER)&sign);
	i = 0;
	while (i < decpt) {
		__GET(buf + i, d[__X(((LONGINT)n - i) - 1, d__len)], CHAR);
		i += 1;
	}
	i = ((LONGINT)n - i) - 1;
	while (i >= 0) {
		d[__X(i, d__len)] = '0';
		i -= 1;
	}
}

/*----------------------------------------------------------------------------*/
static void Reals_Unpack (BYTE *b, LONGINT b__len, BYTE *d, LONGINT d__len)
{
	SHORTINT i, k;
	LONGINT len;
	i = 0;
	len = b__len;
	while ((LONGINT)i < len) {
		k = (INTEGER)__ASHR((INTEGER)(__VAL(CHAR, b[__X(i, b__len)])), 4, INTEGER);
		if (k > 9) {
			d[__X(__ASHL(i, 1, SHORTINT), d__len)] = k + 55;
		} else {
			d[__X(__ASHL(i, 1, SHORTINT), d__len)] = k + 48;
		}
		k = (INTEGER)__MASK((INTEGER)(__VAL(CHAR, b[__X(i, b__len)])), -16);
		if (k > 9) {
			d[__X(__ASHL(i, 1, SHORTINT) + 1, d__len)] = k + 55;
		} else {
			d[__X(__ASHL(i, 1, SHORTINT) + 1, d__len)] = k + 48;
		}
		i += 1;
	}
}

void Reals_ConvertH (REAL y, CHAR *d, LONGINT d__len)
{
	Reals_Unpack((void*)&y, 4, (void*)d, d__len * 1);
}

/*----------------------------------------------------------------------------*/
void Reals_ConvertHL (LONGREAL x, CHAR *d, LONGINT d__len)
{
	Reals_Unpack((void*)&x, 8, (void*)d, d__len * 1);
}

/*----------------------------------------------------------------------------*/

export void *Reals__init(void)
{
	__DEFMOD;
	__REGMOD("Reals", 0);
/* BEGIN */
	__ENDMOD;
}
