/*  Ofront+ 1.0 -xtspkae */

#ifndef Reals__h
#define Reals__h

#include "SYSTEM.h"




import void Reals_Convert (REAL x, SHORTINT n, CHAR *d, INTEGER d__len);
import void Reals_ConvertH (REAL y, CHAR *d, INTEGER d__len);
import void Reals_ConvertHL (LONGREAL x, CHAR *d, INTEGER d__len);
import void Reals_ConvertL (LONGREAL x, SHORTINT n, CHAR *d, INTEGER d__len);
import SHORTINT Reals_Expo (REAL x);
import SHORTINT Reals_ExpoL (LONGREAL x);
import void Reals_SetExpo (SHORTINT e, REAL *x);
import void Reals_SetExpoL (SHORTINT e, LONGREAL *x);
import REAL Reals_Ten (SHORTINT e);
import LONGREAL Reals_TenL (SHORTINT e);
import void *Reals__init(void);


#endif
