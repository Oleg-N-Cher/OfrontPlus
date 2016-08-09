/* Ofront+ 0.9 -xtspkae */
#include "SYSTEM.h"

typedef
	struct ooc2ConvTypes_ScanDesc *ooc2ConvTypes_ScanState;

typedef
	struct ooc2ConvTypes_ScanDesc {
		void (*p)(CHAR, SHORTINT*, ooc2ConvTypes_ScanState*);
	} ooc2ConvTypes_ScanDesc;



export LONGINT *ooc2ConvTypes_ScanDesc__typ;



/*============================================================================*/

__TDESC(ooc2ConvTypes_ScanDesc__desc, 1, 0) = {__TDFLDS("ScanDesc", 4), {-4}};

export void *ooc2ConvTypes__init(void)
{
	__DEFMOD;
	__REGMOD("ooc2ConvTypes", 0);
	__INITYP(ooc2ConvTypes_ScanDesc, ooc2ConvTypes_ScanDesc, 0);
/* BEGIN */
	__ENDMOD;
}
