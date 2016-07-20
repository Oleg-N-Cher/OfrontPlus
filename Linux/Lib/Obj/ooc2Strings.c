/* Ofront 1.2 -xtspkael */
#include "SYSTEM.h"




export void ooc2Strings_Append (CHAR *source, LONGINT source__len, CHAR *destination, LONGINT destination__len);
export void ooc2Strings_Assign (CHAR *source, LONGINT source__len, CHAR *destination, LONGINT destination__len);
export BOOLEAN ooc2Strings_CanAppendAll (INTEGER sourceLength, CHAR *destination, LONGINT destination__len);
export BOOLEAN ooc2Strings_CanAssignAll (INTEGER sourceLength, CHAR *destination, LONGINT destination__len);
export BOOLEAN ooc2Strings_CanConcatAll (INTEGER source1Length, INTEGER source2Length, CHAR *destination, LONGINT destination__len);
export BOOLEAN ooc2Strings_CanDeleteAll (INTEGER stringLength, INTEGER startPos, INTEGER numberToDelete);
export BOOLEAN ooc2Strings_CanExtractAll (INTEGER sourceLength, INTEGER startPos, INTEGER numberToExtract, CHAR *destination, LONGINT destination__len);
export BOOLEAN ooc2Strings_CanInsertAll (INTEGER sourceLength, INTEGER startPos, CHAR *destination, LONGINT destination__len);
export BOOLEAN ooc2Strings_CanReplaceAll (INTEGER sourceLength, INTEGER startPos, CHAR *destination, LONGINT destination__len);
export void ooc2Strings_Capitalize (CHAR *stringVar, LONGINT stringVar__len);
export SHORTINT ooc2Strings_Compare (CHAR *stringVal1, LONGINT stringVal1__len, CHAR *stringVal2, LONGINT stringVal2__len);
export void ooc2Strings_Concat (CHAR *source1, LONGINT source1__len, CHAR *source2, LONGINT source2__len, CHAR *destination, LONGINT destination__len);
export void ooc2Strings_Delete (CHAR *stringVar, LONGINT stringVar__len, INTEGER startPos, INTEGER numberToDelete);
export BOOLEAN ooc2Strings_Equal (CHAR *stringVal1, LONGINT stringVal1__len, CHAR *stringVal2, LONGINT stringVal2__len);
export void ooc2Strings_Extract (CHAR *source, LONGINT source__len, INTEGER startPos, INTEGER numberToExtract, CHAR *destination, LONGINT destination__len);
export void ooc2Strings_FindDiff (CHAR *stringVal1, LONGINT stringVal1__len, CHAR *stringVal2, LONGINT stringVal2__len, BOOLEAN *differenceFound, INTEGER *posOfDifference);
export void ooc2Strings_FindNext (CHAR *pattern, LONGINT pattern__len, CHAR *stringToSearch, LONGINT stringToSearch__len, INTEGER startPos, BOOLEAN *patternFound, INTEGER *posOfPattern);
export void ooc2Strings_FindPrev (CHAR *pattern, LONGINT pattern__len, CHAR *stringToSearch, LONGINT stringToSearch__len, INTEGER startPos, BOOLEAN *patternFound, INTEGER *posOfPattern);
export void ooc2Strings_Insert (CHAR *source, LONGINT source__len, INTEGER startPos, CHAR *destination, LONGINT destination__len);
export INTEGER ooc2Strings_Length (CHAR *stringVal, LONGINT stringVal__len);
export void ooc2Strings_Replace (CHAR *source, LONGINT source__len, INTEGER startPos, CHAR *destination, LONGINT destination__len);


INTEGER ooc2Strings_Length (CHAR *stringVal, LONGINT stringVal__len)
{
	INTEGER i;
	__DUP(stringVal, stringVal__len, CHAR);
	i = 0;
	while (stringVal[__X(i, stringVal__len)] != 0x00) {
		i += 1;
	}
	__DEL(stringVal);
	return i;
}

void ooc2Strings_Assign (CHAR *source, LONGINT source__len, CHAR *destination, LONGINT destination__len)
{
	INTEGER i;
	__DUP(source, source__len, CHAR);
	i = -1;
	do {
		i += 1;
		destination[__X(i, destination__len)] = source[__X(i, source__len)];
	} while (!(destination[__X(i, destination__len)] == 0x00 || (LONGINT)i == destination__len - 1));
	destination[__X(i, destination__len)] = 0x00;
	__DEL(source);
}

void ooc2Strings_Extract (CHAR *source, LONGINT source__len, INTEGER startPos, INTEGER numberToExtract, CHAR *destination, LONGINT destination__len)
{
	INTEGER sourceLength, i;
	__DUP(source, source__len, CHAR);
	sourceLength = ooc2Strings_Length(source, source__len);
	if (startPos > sourceLength) {
		startPos = sourceLength;
	}
	if ((LONGINT)numberToExtract >= destination__len) {
		numberToExtract = (INTEGER)destination__len - 1;
	}
	i = 0;
	while (i < numberToExtract && source[__X(startPos + i, source__len)] != 0x00) {
		destination[__X(i, destination__len)] = source[__X(startPos + i, source__len)];
		i += 1;
	}
	destination[__X(i, destination__len)] = 0x00;
	__DEL(source);
}

void ooc2Strings_Delete (CHAR *stringVar, LONGINT stringVar__len, INTEGER startPos, INTEGER numberToDelete)
{
	INTEGER stringLength, i, _for__14;
	stringLength = ooc2Strings_Length(stringVar, stringVar__len);
	if (startPos + numberToDelete < stringLength) {
		_for__14 = stringLength - numberToDelete;
		i = startPos;
		while (i <= _for__14) {
			stringVar[__X(i, stringVar__len)] = stringVar[__X(i + numberToDelete, stringVar__len)];
			i += 1;
		}
	} else if (startPos < stringLength) {
		stringVar[__X(startPos, stringVar__len)] = 0x00;
	}
}

void ooc2Strings_Insert (CHAR *source, LONGINT source__len, INTEGER startPos, CHAR *destination, LONGINT destination__len)
{
	INTEGER sourceLength, destLength, destMax, i, _for__22, _for__21;
	__DUP(source, source__len, CHAR);
	destLength = ooc2Strings_Length(destination, destination__len);
	sourceLength = ooc2Strings_Length(source, source__len);
	destMax = (INTEGER)destination__len - 1;
	if (startPos + sourceLength < destMax) {
		if (destLength + sourceLength > destMax) {
			destLength = destMax - sourceLength;
			destination[__X(destLength, destination__len)] = 0x00;
		}
		_for__22 = startPos;
		i = destLength;
		while (i >= _for__22) {
			destination[__X(i + sourceLength, destination__len)] = destination[__X(i, destination__len)];
			i += -1;
		}
	} else if (startPos <= destLength) {
		destination[__X(destMax, destination__len)] = 0x00;
		sourceLength = destMax - startPos;
	} else {
		sourceLength = 0;
	}
	_for__21 = sourceLength - 1;
	i = 0;
	while (i <= _for__21) {
		destination[__X(startPos + i, destination__len)] = source[__X(i, source__len)];
		i += 1;
	}
	__DEL(source);
}

void ooc2Strings_Replace (CHAR *source, LONGINT source__len, INTEGER startPos, CHAR *destination, LONGINT destination__len)
{
	INTEGER destLength, i;
	__DUP(source, source__len, CHAR);
	destLength = ooc2Strings_Length(destination, destination__len);
	if (startPos < destLength) {
		i = 0;
		while (startPos != destLength && source[__X(i, source__len)] != 0x00) {
			destination[__X(startPos, destination__len)] = source[__X(i, source__len)];
			startPos += 1;
			i += 1;
		}
	}
	__DEL(source);
}

void ooc2Strings_Append (CHAR *source, LONGINT source__len, CHAR *destination, LONGINT destination__len)
{
	INTEGER destLength, i;
	__DUP(source, source__len, CHAR);
	destLength = ooc2Strings_Length(destination, destination__len);
	i = 0;
	while ((LONGINT)destLength < destination__len - 1 && source[__X(i, source__len)] != 0x00) {
		destination[__X(destLength, destination__len)] = source[__X(i, source__len)];
		destLength += 1;
		i += 1;
	}
	destination[__X(destLength, destination__len)] = 0x00;
	__DEL(source);
}

void ooc2Strings_Concat (CHAR *source1, LONGINT source1__len, CHAR *source2, LONGINT source2__len, CHAR *destination, LONGINT destination__len)
{
	INTEGER i, j;
	__DUP(source1, source1__len, CHAR);
	__DUP(source2, source2__len, CHAR);
	i = 0;
	while (source1[__X(i, source1__len)] != 0x00 && (LONGINT)i < destination__len - 1) {
		destination[__X(i, destination__len)] = source1[__X(i, source1__len)];
		i += 1;
	}
	j = 0;
	while (source2[__X(j, source2__len)] != 0x00 && (LONGINT)i < destination__len - 1) {
		destination[__X(i, destination__len)] = source2[__X(j, source2__len)];
		j += 1;
		i += 1;
	}
	destination[__X(i, destination__len)] = 0x00;
	__DEL(source1);
	__DEL(source2);
}

BOOLEAN ooc2Strings_CanAssignAll (INTEGER sourceLength, CHAR *destination, LONGINT destination__len)
{
	return (LONGINT)sourceLength < destination__len;
}

BOOLEAN ooc2Strings_CanExtractAll (INTEGER sourceLength, INTEGER startPos, INTEGER numberToExtract, CHAR *destination, LONGINT destination__len)
{
	return startPos + numberToExtract <= sourceLength && (LONGINT)numberToExtract < destination__len;
}

BOOLEAN ooc2Strings_CanDeleteAll (INTEGER stringLength, INTEGER startPos, INTEGER numberToDelete)
{
	return startPos + numberToDelete <= stringLength;
}

BOOLEAN ooc2Strings_CanInsertAll (INTEGER sourceLength, INTEGER startPos, CHAR *destination, LONGINT destination__len)
{
	INTEGER lenDestination;
	lenDestination = ooc2Strings_Length(destination, destination__len);
	return startPos <= lenDestination && (LONGINT)(sourceLength + lenDestination) < destination__len;
}

BOOLEAN ooc2Strings_CanReplaceAll (INTEGER sourceLength, INTEGER startPos, CHAR *destination, LONGINT destination__len)
{
	return sourceLength + startPos <= ooc2Strings_Length(destination, destination__len);
}

BOOLEAN ooc2Strings_CanAppendAll (INTEGER sourceLength, CHAR *destination, LONGINT destination__len)
{
	return (LONGINT)(ooc2Strings_Length(destination, destination__len) + sourceLength) < destination__len;
}

BOOLEAN ooc2Strings_CanConcatAll (INTEGER source1Length, INTEGER source2Length, CHAR *destination, LONGINT destination__len)
{
	return (LONGINT)(source1Length + source2Length) < destination__len;
}

SHORTINT ooc2Strings_Compare (CHAR *stringVal1, LONGINT stringVal1__len, CHAR *stringVal2, LONGINT stringVal2__len)
{
	INTEGER i;
	__DUP(stringVal1, stringVal1__len, CHAR);
	__DUP(stringVal2, stringVal2__len, CHAR);
	i = 0;
	while (stringVal1[__X(i, stringVal1__len)] != 0x00 && stringVal1[__X(i, stringVal1__len)] == stringVal2[__X(i, stringVal2__len)]) {
		i += 1;
	}
	if (stringVal1[__X(i, stringVal1__len)] < stringVal2[__X(i, stringVal2__len)]) {
		__DEL(stringVal1);
		__DEL(stringVal2);
		return -1;
	} else if (stringVal1[__X(i, stringVal1__len)] > stringVal2[__X(i, stringVal2__len)]) {
		__DEL(stringVal1);
		__DEL(stringVal2);
		return 1;
	} else {
		__DEL(stringVal1);
		__DEL(stringVal2);
		return 0;
	}
	__RETCHK;
}

BOOLEAN ooc2Strings_Equal (CHAR *stringVal1, LONGINT stringVal1__len, CHAR *stringVal2, LONGINT stringVal2__len)
{
	INTEGER i;
	__DUP(stringVal1, stringVal1__len, CHAR);
	__DUP(stringVal2, stringVal2__len, CHAR);
	i = 0;
	while (stringVal1[__X(i, stringVal1__len)] != 0x00 && stringVal1[__X(i, stringVal1__len)] == stringVal2[__X(i, stringVal2__len)]) {
		i += 1;
	}
	__DEL(stringVal1);
	__DEL(stringVal2);
	return stringVal1[__X(i, stringVal1__len)] == 0x00 && stringVal2[__X(i, stringVal2__len)] == 0x00;
}

void ooc2Strings_FindNext (CHAR *pattern, LONGINT pattern__len, CHAR *stringToSearch, LONGINT stringToSearch__len, INTEGER startPos, BOOLEAN *patternFound, INTEGER *posOfPattern)
{
	INTEGER patternPos;
	__DUP(pattern, pattern__len, CHAR);
	__DUP(stringToSearch, stringToSearch__len, CHAR);
	if (startPos < ooc2Strings_Length(stringToSearch, stringToSearch__len)) {
		patternPos = 0;
		for (;;) {
			if (pattern[__X(patternPos, pattern__len)] == 0x00) {
				*patternFound = 1;
				*posOfPattern = startPos - patternPos;
				break;
			} else if (stringToSearch[__X(startPos, stringToSearch__len)] == 0x00) {
				*patternFound = 0;
				break;
			} else if (stringToSearch[__X(startPos, stringToSearch__len)] == pattern[__X(patternPos, pattern__len)]) {
				startPos += 1;
				patternPos += 1;
			} else {
				startPos = (startPos - patternPos) + 1;
				patternPos = 0;
			}
		}
	} else {
		*patternFound = 0;
	}
	__DEL(pattern);
	__DEL(stringToSearch);
}

void ooc2Strings_FindPrev (CHAR *pattern, LONGINT pattern__len, CHAR *stringToSearch, LONGINT stringToSearch__len, INTEGER startPos, BOOLEAN *patternFound, INTEGER *posOfPattern)
{
	INTEGER patternPos, stringLength, patternLength;
	__DUP(pattern, pattern__len, CHAR);
	__DUP(stringToSearch, stringToSearch__len, CHAR);
	stringLength = ooc2Strings_Length(stringToSearch, stringToSearch__len);
	patternLength = ooc2Strings_Length(pattern, pattern__len);
	if (startPos > stringLength - patternLength) {
		startPos = stringLength - patternLength;
	}
	if (startPos >= 0) {
		patternPos = 0;
		for (;;) {
			if (pattern[__X(patternPos, pattern__len)] == 0x00) {
				*patternFound = 1;
				*posOfPattern = startPos - patternPos;
				break;
			} else if (stringToSearch[__X(startPos, stringToSearch__len)] != pattern[__X(patternPos, pattern__len)]) {
				if (startPos > patternPos) {
					startPos = (startPos - patternPos) - 1;
					patternPos = 0;
				} else {
					*patternFound = 0;
					break;
				}
			} else {
				startPos += 1;
				patternPos += 1;
			}
		}
	} else {
		*patternFound = 0;
	}
	__DEL(pattern);
	__DEL(stringToSearch);
}

void ooc2Strings_FindDiff (CHAR *stringVal1, LONGINT stringVal1__len, CHAR *stringVal2, LONGINT stringVal2__len, BOOLEAN *differenceFound, INTEGER *posOfDifference)
{
	INTEGER i;
	__DUP(stringVal1, stringVal1__len, CHAR);
	__DUP(stringVal2, stringVal2__len, CHAR);
	i = 0;
	while (stringVal1[__X(i, stringVal1__len)] != 0x00 && stringVal1[__X(i, stringVal1__len)] == stringVal2[__X(i, stringVal2__len)]) {
		i += 1;
	}
	*differenceFound = stringVal1[__X(i, stringVal1__len)] != 0x00 || stringVal2[__X(i, stringVal2__len)] != 0x00;
	if (*differenceFound) {
		*posOfDifference = i;
	}
	__DEL(stringVal1);
	__DEL(stringVal2);
}

void ooc2Strings_Capitalize (CHAR *stringVar, LONGINT stringVar__len)
{
	INTEGER i;
	i = 0;
	while (stringVar[__X(i, stringVar__len)] != 0x00) {
		stringVar[__X(i, stringVar__len)] = __CAP(stringVar[__X(i, stringVar__len)]);
		i += 1;
	}
}


export void *ooc2Strings__init(void)
{
	__DEFMOD;
	__REGMOD("ooc2Strings", 0);
/* BEGIN */
	__ENDMOD;
}
