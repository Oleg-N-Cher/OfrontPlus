/* Ofront+ 1.0 -xtspkae */

#ifndef ooc2ConvTypes__h
#define ooc2ConvTypes__h

#include "SYSTEM.h"

typedef
	struct ooc2ConvTypes_ScanDesc *ooc2ConvTypes_ScanState;

typedef
	struct ooc2ConvTypes_ScanDesc {
		void (*p)(CHAR, SHORTINT*, ooc2ConvTypes_ScanState*);
	} ooc2ConvTypes_ScanDesc;



import LONGINT *ooc2ConvTypes_ScanDesc__typ;

import void *ooc2ConvTypes__init(void);


#endif
