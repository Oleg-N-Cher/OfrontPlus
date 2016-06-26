/* Ofront 1.2 -xtspkael */
#include "SYSTEM.h"


static INTEGER ooc2RandomNumbers_z;


export void ooc2RandomNumbers_GetSeed (INTEGER *seed);
static void ooc2RandomNumbers_NextRND (void);
export void ooc2RandomNumbers_PutSeed (INTEGER seed);
export INTEGER ooc2RandomNumbers_RND (INTEGER range);
export REAL ooc2RandomNumbers_Random (void);


void ooc2RandomNumbers_GetSeed (INTEGER *seed)
{
	*seed = ooc2RandomNumbers_z;
}

void ooc2RandomNumbers_PutSeed (INTEGER seed)
{
	seed = (int)__MOD(seed, 2147483647);
	if (seed == 0) {
		ooc2RandomNumbers_z = 1;
	} else {
		ooc2RandomNumbers_z = seed;
	}
}

static void ooc2RandomNumbers_NextRND (void)
{
	INTEGER lo, hi, test;
	hi = __DIV(ooc2RandomNumbers_z, 127773);
	lo = (int)__MOD(ooc2RandomNumbers_z, 127773);
	test = 16807 * lo - 2836 * hi;
	if (test > 0) {
		ooc2RandomNumbers_z = test;
	} else {
		ooc2RandomNumbers_z = test + 2147483647;
	}
}

INTEGER ooc2RandomNumbers_RND (INTEGER range)
{
	ooc2RandomNumbers_NextRND();
	return (int)__MOD(ooc2RandomNumbers_z, range);
}

REAL ooc2RandomNumbers_Random (void)
{
	ooc2RandomNumbers_NextRND();
	return (ooc2RandomNumbers_z - 1) *   4.6566129e-010;
}


export void *ooc2RandomNumbers__init(void)
{
	__DEFMOD;
	__REGMOD("ooc2RandomNumbers", 0);
/* BEGIN */
	ooc2RandomNumbers_z = 1;
	__ENDMOD;
}
