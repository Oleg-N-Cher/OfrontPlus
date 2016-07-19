/*  Ofront 1.2 -xtspkae */

#ifndef ooc2Strings__h
#define ooc2Strings__h

#include "SYSTEM.h"




import void ooc2Strings_Append (CHAR *source, LONGINT source__len, CHAR *destination, LONGINT destination__len);
import void ooc2Strings_Assign (CHAR *source, LONGINT source__len, CHAR *destination, LONGINT destination__len);
import BOOLEAN ooc2Strings_CanAppendAll (INTEGER sourceLength, CHAR *destination, LONGINT destination__len);
import BOOLEAN ooc2Strings_CanAssignAll (INTEGER sourceLength, CHAR *destination, LONGINT destination__len);
import BOOLEAN ooc2Strings_CanConcatAll (INTEGER source1Length, INTEGER source2Length, CHAR *destination, LONGINT destination__len);
import BOOLEAN ooc2Strings_CanDeleteAll (INTEGER stringLength, INTEGER startPos, INTEGER numberToDelete);
import BOOLEAN ooc2Strings_CanExtractAll (INTEGER sourceLength, INTEGER startPos, INTEGER numberToExtract, CHAR *destination, LONGINT destination__len);
import BOOLEAN ooc2Strings_CanInsertAll (INTEGER sourceLength, INTEGER startPos, CHAR *destination, LONGINT destination__len);
import BOOLEAN ooc2Strings_CanReplaceAll (INTEGER sourceLength, INTEGER startPos, CHAR *destination, LONGINT destination__len);
import void ooc2Strings_Capitalize (CHAR *stringVar, LONGINT stringVar__len);
import SHORTINT ooc2Strings_Compare (CHAR *stringVal1, LONGINT stringVal1__len, CHAR *stringVal2, LONGINT stringVal2__len);
import void ooc2Strings_Concat (CHAR *source1, LONGINT source1__len, CHAR *source2, LONGINT source2__len, CHAR *destination, LONGINT destination__len);
import void ooc2Strings_Delete (CHAR *stringVar, LONGINT stringVar__len, INTEGER startPos, INTEGER numberToDelete);
import BOOLEAN ooc2Strings_Equal (CHAR *stringVal1, LONGINT stringVal1__len, CHAR *stringVal2, LONGINT stringVal2__len);
import void ooc2Strings_Extract (CHAR *source, LONGINT source__len, INTEGER startPos, INTEGER numberToExtract, CHAR *destination, LONGINT destination__len);
import void ooc2Strings_FindDiff (CHAR *stringVal1, LONGINT stringVal1__len, CHAR *stringVal2, LONGINT stringVal2__len, BOOLEAN *differenceFound, INTEGER *posOfDifference);
import void ooc2Strings_FindNext (CHAR *pattern, LONGINT pattern__len, CHAR *stringToSearch, LONGINT stringToSearch__len, INTEGER startPos, BOOLEAN *patternFound, INTEGER *posOfPattern);
import void ooc2Strings_FindPrev (CHAR *pattern, LONGINT pattern__len, CHAR *stringToSearch, LONGINT stringToSearch__len, INTEGER startPos, BOOLEAN *patternFound, INTEGER *posOfPattern);
import void ooc2Strings_Insert (CHAR *source, LONGINT source__len, INTEGER startPos, CHAR *destination, LONGINT destination__len);
import INTEGER ooc2Strings_Length (CHAR *stringVal, LONGINT stringVal__len);
import void ooc2Strings_Replace (CHAR *source, LONGINT source__len, INTEGER startPos, CHAR *destination, LONGINT destination__len);
import void *ooc2Strings__init(void);


#endif
