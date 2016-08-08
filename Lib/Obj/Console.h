/* Ofront+ 0.9 -xtspkae */

#ifndef Console__h
#define Console__h

#include "SYSTEM.h"




import void Console_Bool (BOOLEAN b);
import void Console_Char (CHAR ch);
import void Console_Flush (void);
import void Console_Hex (INTEGER i);
import void Console_Int (INTEGER i, INTEGER n);
import void Console_Ln (void);
import void Console_LongHex (LONGINT i);
import void Console_LongInt (LONGINT i, INTEGER n);
import void Console_Read (CHAR *ch);
import void Console_ReadLine (CHAR *line, INTEGER line__len);
import void Console_String (CHAR *s, INTEGER s__len);
import void *Console__init(void);


#endif
