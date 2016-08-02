/* Ofront+ 1.0 -xtspkae */

#ifndef OfrontOPS__h
#define OfrontOPS__h

#include "SYSTEM.h"

typedef
	CHAR OfrontOPS_Name[32];

typedef
	CHAR OfrontOPS_String[256];


import OfrontOPS_Name OfrontOPS_name;
import OfrontOPS_String OfrontOPS_str;
import INTEGER OfrontOPS_numtyp;
import LONGINT OfrontOPS_intval;
import REAL OfrontOPS_realval;
import LONGREAL OfrontOPS_lrlval;


import void OfrontOPS_Get (SHORTINT *sym);
import void OfrontOPS_Init (void);
import void *OfrontOPS__init(void);


#endif
