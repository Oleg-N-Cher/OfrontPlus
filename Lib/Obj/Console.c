/* Ofront+ 0.9 -xtspkae */
#include "SYSTEM.h"
#include "Platform.h"


static CHAR Console_line[128];
static INTEGER Console_pos;


export void Console_Bool (BOOLEAN b);
export void Console_Char (CHAR ch);
export void Console_Flush (void);
export void Console_Hex (INTEGER i);
export void Console_Int (INTEGER i, INTEGER n);
export void Console_Ln (void);
export void Console_LongHex (LONGINT i);
export void Console_LongInt (LONGINT i, INTEGER n);
export void Console_Read (CHAR *ch);
export void Console_ReadLine (CHAR *line, INTEGER line__len);
export void Console_String (CHAR *s, INTEGER s__len);


/*============================================================================*/

void Console_Flush (void)
{
	INTEGER error;
	error = Platform_Write(Platform_StdOut, (Platform_MemAdr)((INTEGER)Console_line), Console_pos);
	Console_pos = 0;
}

/*----------------------------------------------------------------------------*/
void Console_Char (CHAR ch)
{
	if (Console_pos == 128) {
		Console_Flush();
	}
	Console_line[__X(Console_pos, 128)] = ch;
	Console_pos += 1;
	if (ch == 0x0a) {
		Console_Flush();
	}
}

/*----------------------------------------------------------------------------*/
void Console_String (CHAR *s, INTEGER s__len)
{
	INTEGER i;
	__DUP(s, s__len);
	i = 0;
	while (s[__X(i, s__len)] != 0x00) {
		Console_Char(s[__X(i, s__len)]);
		i += 1;
	}
	__DEL(s);
}

/*----------------------------------------------------------------------------*/
void Console_Int (INTEGER i, INTEGER n)
{
	CHAR s[16];
	INTEGER i1, k;
	if (i == __LSHL((INTEGER)1, 31, INTEGER)) {
		__MOVE("8463847412", s, 11);
		k = 10;
	} else {
		i1 = __ABS(i);
		s[0] = (CHAR)((INTEGER)__MOD(i1, 10) + 48);
		i1 = __DIV(i1, 10);
		k = 1;
		while (i1 > 0) {
			s[__X(k, 16)] = (CHAR)((INTEGER)__MOD(i1, 10) + 48);
			i1 = __DIV(i1, 10);
			k += 1;
		}
	}
	if (i < 0) {
		s[__X(k, 16)] = '-';
		k += 1;
	}
	while (n > k) {
		Console_Char(' ');
		n -= 1;
	}
	while (k > 0) {
		k -= 1;
		Console_Char(s[__X(k, 16)]);
	}
}

/*----------------------------------------------------------------------------*/
void Console_LongInt (LONGINT i, INTEGER n)
{
	CHAR s[24];
	LONGINT i1;
	INTEGER k;
	if (i == __LSHL((LONGINT)1, 31, LONGINT)) {
		__MOVE("8463847412", s, 11);
		k = 10;
	} else {
		i1 = __ABS(i);
		s[0] = (CHAR)(__MOD(i1, 10) + 48);
		i1 = __DIV(i1, 10);
		k = 1;
		while (i1 > 0) {
			s[__X(k, 24)] = (CHAR)(__MOD(i1, 10) + 48);
			i1 = __DIV(i1, 10);
			k += 1;
		}
	}
	if (i < 0) {
		s[__X(k, 24)] = '-';
		k += 1;
	}
	while (n > k) {
		Console_Char(' ');
		n -= 1;
	}
	while (k > 0) {
		k -= 1;
		Console_Char(s[__X(k, 24)]);
	}
}

/*----------------------------------------------------------------------------*/
void Console_Ln (void)
{
	Console_String(Platform_newLine, 3);
}

/*----------------------------------------------------------------------------*/
void Console_Bool (BOOLEAN b)
{
	if (b) {
		Console_String((CHAR*)"TRUE", 5);
	} else {
		Console_String((CHAR*)"FALSE", 6);
	}
}

/*----------------------------------------------------------------------------*/
void Console_Hex (INTEGER i)
{
	INTEGER k, n;
	k = -28;
	while (k <= 0) {
		n = __MASK(__ASH(i, k, INTEGER), -16);
		if (n <= 9) {
			Console_Char((CHAR)(48 + n));
		} else {
			Console_Char((CHAR)(55 + n));
		}
		k += 4;
	}
}

/*----------------------------------------------------------------------------*/
void Console_LongHex (LONGINT i)
{
	INTEGER k, n;
	k = -28;
	while (k <= 0) {
		n = __MASK((INTEGER)__ASH(i, k, LONGINT), -16);
		if (n <= 9) {
			Console_Char((CHAR)(48 + n));
		} else {
			Console_Char((CHAR)(55 + n));
		}
		k += 4;
	}
}

/*----------------------------------------------------------------------------*/
void Console_Read (CHAR *ch)
{
	INTEGER n, error;
	Console_Flush();
	error = Platform_ReadBuf(Platform_StdIn, (BYTE*)(void*)&*ch, 1, &n);
	if (n != 1) {
		*ch = 0x00;
	}
}

/*----------------------------------------------------------------------------*/
void Console_ReadLine (CHAR *line, INTEGER line__len)
{
	INTEGER i;
	CHAR ch;
	Console_Flush();
	i = 0;
	Console_Read(&ch);
	while ((i < line__len - 1 && ch != 0x0a) && ch != 0x00) {
		line[__X(i, line__len)] = ch;
		i += 1;
		Console_Read(&ch);
	}
	line[__X(i, line__len)] = 0x00;
}

/*----------------------------------------------------------------------------*/

export void *Console__init(void)
{
	__DEFMOD;
	__IMPORT(Platform__init);
	__REGMOD("Console", 0);
	__REGCMD("Flush", Console_Flush);
	__REGCMD("Ln", Console_Ln);
/* BEGIN */
	Console_pos = 0;
	__ENDMOD;
}
