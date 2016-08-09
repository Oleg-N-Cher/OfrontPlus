/* Ofront+ 0.9 -xtspkae */

#ifndef Strings__h
#define Strings__h

#include "SYSTEM.h"




import void Strings_Append (CHAR *extra, INTEGER extra__len, CHAR *dest, INTEGER dest__len);
import void Strings_Cap (CHAR *s, INTEGER s__len);
import void Strings_Delete (CHAR *s, INTEGER s__len, INTEGER pos, INTEGER n);
import void Strings_Extract (CHAR *source, INTEGER source__len, INTEGER pos, INTEGER n, CHAR *dest, INTEGER dest__len);
import void Strings_Insert (CHAR *source, INTEGER source__len, INTEGER pos, CHAR *dest, INTEGER dest__len);
import INTEGER Strings_Length (CHAR *s, INTEGER s__len);
import BOOLEAN Strings_Match (CHAR *string, INTEGER string__len, CHAR *pattern, INTEGER pattern__len);
import INTEGER Strings_Pos (CHAR *pattern, INTEGER pattern__len, CHAR *s, INTEGER s__len, INTEGER pos);
import void Strings_Replace (CHAR *source, INTEGER source__len, INTEGER pos, CHAR *dest, INTEGER dest__len);
import void *Strings__init(void);


#endif
