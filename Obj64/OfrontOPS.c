/*  Ofront 1.2 -xtspkae */
#include "SYSTEM.h"
#include "OfrontOPM.h"

typedef
	CHAR OfrontOPS_Name[32];

typedef
	CHAR OfrontOPS_String[256];


export OfrontOPS_Name OfrontOPS_name;
export OfrontOPS_String OfrontOPS_str;
export INTEGER OfrontOPS_numtyp;
export LONGINT OfrontOPS_intval;
export REAL OfrontOPS_realval;
export LONGREAL OfrontOPS_lrlval;
static CHAR OfrontOPS_ch;


export void OfrontOPS_Get (SHORTINT *sym);
static void OfrontOPS_Identifier (SHORTINT *sym);
export void OfrontOPS_Init (void);
static void OfrontOPS_Number (void);
static void OfrontOPS_Str (SHORTINT *sym);
static void OfrontOPS_err (INTEGER n);


/*============================================================================*/

static void OfrontOPS_err (INTEGER n)
{
	OfrontOPM_err(n);
}

static void OfrontOPS_Str (SHORTINT *sym)
{
	INTEGER i;
	CHAR och;
	i = 0;
	och = OfrontOPS_ch;
	for (;;) {
		OfrontOPM_Get(&OfrontOPS_ch);
		if (OfrontOPS_ch == och) {
			break;
		}
		if (OfrontOPS_ch < ' ') {
			OfrontOPS_err(3);
			break;
		}
		if (i == 255) {
			OfrontOPS_err(241);
			break;
		}
		OfrontOPS_str[__X(i, 256)] = OfrontOPS_ch;
		i += 1;
	}
	OfrontOPM_Get(&OfrontOPS_ch);
	OfrontOPS_str[__X(i, 256)] = 0x00;
	OfrontOPS_intval = i + 1;
	if (OfrontOPS_intval == 2) {
		*sym = 35;
		OfrontOPS_numtyp = 1;
		OfrontOPS_intval = (INTEGER)OfrontOPS_str[0];
	} else {
		*sym = 37;
	}
}

static void OfrontOPS_Identifier (SHORTINT *sym)
{
	INTEGER i;
	i = 0;
	do {
		OfrontOPS_name[__X(i, 32)] = OfrontOPS_ch;
		i += 1;
		OfrontOPM_Get(&OfrontOPS_ch);
	} while (!(((OfrontOPS_ch < '0' || '9' < OfrontOPS_ch && __CAP(OfrontOPS_ch) < 'A') || 'Z' < __CAP(OfrontOPS_ch) && OfrontOPS_ch != '_') || i == 32));
	if (i == 32) {
		OfrontOPS_err(240);
		i -= 1;
	}
	OfrontOPS_name[__X(i, 32)] = 0x00;
	*sym = 38;
}

static struct Number__6 {
	struct Number__6 *lnk;
} *Number__6_s;

static INTEGER Ord__7 (CHAR ch, BOOLEAN hex);
static LONGREAL Ten__9 (INTEGER e);

static LONGREAL Ten__9 (INTEGER e)
{
	LONGREAL x, p;
	x = (LONGREAL)1;
	p = (LONGREAL)10;
	while (e > 0) {
		if (__ODD(e)) {
			x = x * p;
		}
		e = __ASHR(e, 1);
		if (e > 0) {
			p = p * p;
		}
	}
	return x;
}

static INTEGER Ord__7 (CHAR ch, BOOLEAN hex)
{
	if (ch <= '9') {
		return (INTEGER)ch - 48;
	} else if (hex) {
		return ((INTEGER)ch - 65) + 10;
	} else {
		OfrontOPS_err(2);
		return 0;
	}
	__RETCHK;
}

static void OfrontOPS_Number (void)
{
	INTEGER i, m, n, d, e;
	CHAR dig[24];
	LONGREAL f;
	CHAR expCh;
	BOOLEAN neg;
	struct Number__6 _s;
	_s.lnk = Number__6_s;
	Number__6_s = &_s;
	i = 0;
	m = 0;
	n = 0;
	d = 0;
	for (;;) {
		if ('0' <= OfrontOPS_ch && OfrontOPS_ch <= '9' || (d == 0 && 'A' <= OfrontOPS_ch) && OfrontOPS_ch <= 'F') {
			if (m > 0 || OfrontOPS_ch != '0') {
				if (n < 24) {
					dig[__X(n, 24)] = OfrontOPS_ch;
					n += 1;
				}
				m += 1;
			}
			OfrontOPM_Get(&OfrontOPS_ch);
			i += 1;
		} else if (OfrontOPS_ch == '.') {
			OfrontOPM_Get(&OfrontOPS_ch);
			if (OfrontOPS_ch == '.') {
				OfrontOPS_ch = 0x7f;
				break;
			} else if (d == 0) {
				d = i;
			} else {
				OfrontOPS_err(2);
			}
		} else {
			break;
		}
	}
	if (d == 0) {
		if (n == m) {
			OfrontOPS_intval = 0;
			i = 0;
			if (OfrontOPS_ch == 'X') {
				OfrontOPM_Get(&OfrontOPS_ch);
				OfrontOPS_numtyp = 1;
				if (n <= 2) {
					while (i < n) {
						OfrontOPS_intval = __ASHL(OfrontOPS_intval, 4) + (LONGINT)Ord__7(dig[__X(i, 24)], 1);
						i += 1;
					}
				} else {
					OfrontOPS_err(203);
				}
			} else if (OfrontOPS_ch == 'H') {
				OfrontOPM_Get(&OfrontOPS_ch);
				OfrontOPS_numtyp = 2;
				if (n <= 16) {
					if (n == 16 && dig[0] > '7') {
						OfrontOPS_intval = -1;
					}
					while (i < n) {
						OfrontOPS_intval = __ASHL(OfrontOPS_intval, 4) + (LONGINT)Ord__7(dig[__X(i, 24)], 1);
						i += 1;
					}
				} else {
					OfrontOPS_err(203);
				}
			} else {
				OfrontOPS_numtyp = 2;
				while (i < n) {
					d = Ord__7(dig[__X(i, 24)], 0);
					i += 1;
					if (OfrontOPS_intval <= __DIV(9223372036854775807 - (LONGINT)d, 10)) {
						OfrontOPS_intval = OfrontOPS_intval * 10 + (LONGINT)d;
					} else {
						OfrontOPS_err(203);
					}
				}
			}
		} else {
			OfrontOPS_err(203);
		}
	} else {
		f = (LONGREAL)0;
		e = 0;
		expCh = 'E';
		while (n > 0) {
			n -= 1;
			f = (Ord__7(dig[__X(n, 24)], 0) + f) / (LONGREAL)(LONGREAL)10;
		}
		if (OfrontOPS_ch == 'E' || OfrontOPS_ch == 'D') {
			expCh = OfrontOPS_ch;
			OfrontOPM_Get(&OfrontOPS_ch);
			neg = 0;
			if (OfrontOPS_ch == '-') {
				neg = 1;
				OfrontOPM_Get(&OfrontOPS_ch);
			} else if (OfrontOPS_ch == '+') {
				OfrontOPM_Get(&OfrontOPS_ch);
			}
			if ('0' <= OfrontOPS_ch && OfrontOPS_ch <= '9') {
				do {
					n = Ord__7(OfrontOPS_ch, 0);
					OfrontOPM_Get(&OfrontOPS_ch);
					if (e <= __DIV(2147483647 - n, 10)) {
						e = e * 10 + n;
					} else {
						OfrontOPS_err(203);
					}
				} while (!(OfrontOPS_ch < '0' || '9' < OfrontOPS_ch));
				if (neg) {
					e = -e;
				}
			} else {
				OfrontOPS_err(2);
			}
		}
		e -= (i - d) - m;
		if (expCh == 'E') {
			OfrontOPS_numtyp = 3;
			if (-37 < e && e <= 38) {
				if (e < 0) {
					OfrontOPS_realval = (f / (LONGREAL)Ten__9(-e));
				} else {
					OfrontOPS_realval = (f * Ten__9(e));
				}
			} else {
				OfrontOPS_err(203);
			}
		} else {
			OfrontOPS_numtyp = 4;
			if (-307 < e && e <= 308) {
				if (e < 0) {
					OfrontOPS_lrlval = f / (LONGREAL)Ten__9(-e);
				} else {
					OfrontOPS_lrlval = f * Ten__9(e);
				}
			} else {
				OfrontOPS_err(203);
			}
		}
	}
	Number__6_s = _s.lnk;
}

static struct Get__1 {
	struct Get__1 *lnk;
} *Get__1_s;

static void Comment__2 (void);

static void Comment__2 (void)
{
	OfrontOPM_Get(&OfrontOPS_ch);
	for (;;) {
		for (;;) {
			while (OfrontOPS_ch == '(') {
				OfrontOPM_Get(&OfrontOPS_ch);
				if (OfrontOPS_ch == '*') {
					Comment__2();
				}
			}
			if (OfrontOPS_ch == '*') {
				OfrontOPM_Get(&OfrontOPS_ch);
				break;
			}
			if (OfrontOPS_ch == 0x00) {
				break;
			}
			OfrontOPM_Get(&OfrontOPS_ch);
		}
		if (OfrontOPS_ch == ')') {
			OfrontOPM_Get(&OfrontOPS_ch);
			break;
		}
		if (OfrontOPS_ch == 0x00) {
			OfrontOPS_err(5);
			break;
		}
	}
}

void OfrontOPS_Get (SHORTINT *sym)
{
	SHORTINT s;
	struct Get__1 _s;
	_s.lnk = Get__1_s;
	Get__1_s = &_s;
	OfrontOPM_errpos = OfrontOPM_curpos - 1;
	while (OfrontOPS_ch <= ' ') {
		if (OfrontOPS_ch == 0x00) {
			*sym = 64;
			Get__1_s = _s.lnk;
			return;
		} else {
			OfrontOPM_Get(&OfrontOPS_ch);
		}
	}
	switch (OfrontOPS_ch) {
		case '\"': case '\'': 
			OfrontOPS_Str(&s);
			break;
		case '#': 
			s = 10;
			OfrontOPM_Get(&OfrontOPS_ch);
			break;
		case '&': 
			s = 5;
			OfrontOPM_Get(&OfrontOPS_ch);
			break;
		case '(': 
			OfrontOPM_Get(&OfrontOPS_ch);
			if (OfrontOPS_ch == '*') {
				Comment__2();
				OfrontOPS_Get(&s);
			} else {
				s = 30;
			}
			break;
		case ')': 
			s = 22;
			OfrontOPM_Get(&OfrontOPS_ch);
			break;
		case '*': 
			s = 1;
			OfrontOPM_Get(&OfrontOPS_ch);
			break;
		case '+': 
			s = 6;
			OfrontOPM_Get(&OfrontOPS_ch);
			break;
		case ',': 
			s = 19;
			OfrontOPM_Get(&OfrontOPS_ch);
			break;
		case '-': 
			s = 7;
			OfrontOPM_Get(&OfrontOPS_ch);
			break;
		case '.': 
			OfrontOPM_Get(&OfrontOPS_ch);
			if (OfrontOPS_ch == '.') {
				OfrontOPM_Get(&OfrontOPS_ch);
				s = 21;
			} else {
				s = 18;
			}
			break;
		case '/': 
			s = 2;
			OfrontOPM_Get(&OfrontOPS_ch);
			break;
		case '0': case '1': case '2': case '3': case '4': 
		case '5': case '6': case '7': case '8': case '9': 
			OfrontOPS_Number();
			s = 35;
			break;
		case ':': 
			OfrontOPM_Get(&OfrontOPS_ch);
			if (OfrontOPS_ch == '=') {
				OfrontOPM_Get(&OfrontOPS_ch);
				s = 34;
			} else {
				s = 20;
			}
			break;
		case ';': 
			s = 39;
			OfrontOPM_Get(&OfrontOPS_ch);
			break;
		case '<': 
			OfrontOPM_Get(&OfrontOPS_ch);
			if (OfrontOPS_ch == '=') {
				OfrontOPM_Get(&OfrontOPS_ch);
				s = 12;
			} else {
				s = 11;
			}
			break;
		case '=': 
			s = 9;
			OfrontOPM_Get(&OfrontOPS_ch);
			break;
		case '>': 
			OfrontOPM_Get(&OfrontOPS_ch);
			if (OfrontOPS_ch == '=') {
				OfrontOPM_Get(&OfrontOPS_ch);
				s = 14;
			} else {
				s = 13;
			}
			break;
		case 'A': 
			OfrontOPS_Identifier(&s);
			if (__STRCMP(OfrontOPS_name, "ARRAY") == 0) {
				s = 54;
			}
			break;
		case 'B': 
			OfrontOPS_Identifier(&s);
			if (__STRCMP(OfrontOPS_name, "BEGIN") == 0) {
				s = 57;
			} else if (__STRCMP(OfrontOPS_name, "BY") == 0) {
				s = 29;
			}
			break;
		case 'C': 
			OfrontOPS_Identifier(&s);
			if (__STRCMP(OfrontOPS_name, "CASE") == 0) {
				s = 46;
			} else if (__STRCMP(OfrontOPS_name, "CONST") == 0) {
				s = 58;
			}
			break;
		case 'D': 
			OfrontOPS_Identifier(&s);
			if (__STRCMP(OfrontOPS_name, "DO") == 0) {
				s = 27;
			} else if (__STRCMP(OfrontOPS_name, "DIV") == 0) {
				s = 3;
			}
			break;
		case 'E': 
			OfrontOPS_Identifier(&s);
			if (__STRCMP(OfrontOPS_name, "END") == 0) {
				s = 41;
			} else if (__STRCMP(OfrontOPS_name, "ELSE") == 0) {
				s = 42;
			} else if (__STRCMP(OfrontOPS_name, "ELSIF") == 0) {
				s = 43;
			} else if (__STRCMP(OfrontOPS_name, "EXIT") == 0) {
				s = 52;
			}
			break;
		case 'F': 
			OfrontOPS_Identifier(&s);
			if (__STRCMP(OfrontOPS_name, "FOR") == 0) {
				s = 49;
			}
			break;
		case 'I': 
			OfrontOPS_Identifier(&s);
			if (__STRCMP(OfrontOPS_name, "IF") == 0) {
				s = 45;
			} else if (__STRCMP(OfrontOPS_name, "IN") == 0) {
				s = 15;
			} else if (__STRCMP(OfrontOPS_name, "IS") == 0) {
				s = 16;
			} else if (__STRCMP(OfrontOPS_name, "IMPORT") == 0) {
				s = 62;
			}
			break;
		case 'L': 
			OfrontOPS_Identifier(&s);
			if (__STRCMP(OfrontOPS_name, "LOOP") == 0) {
				s = 50;
			}
			break;
		case 'M': 
			OfrontOPS_Identifier(&s);
			if (__STRCMP(OfrontOPS_name, "MOD") == 0) {
				s = 4;
			} else if (__STRCMP(OfrontOPS_name, "MODULE") == 0) {
				s = 63;
			}
			break;
		case 'N': 
			OfrontOPS_Identifier(&s);
			if (__STRCMP(OfrontOPS_name, "NIL") == 0) {
				s = 36;
			}
			break;
		case 'O': 
			OfrontOPS_Identifier(&s);
			if (__STRCMP(OfrontOPS_name, "OR") == 0) {
				s = 8;
			} else if (__STRCMP(OfrontOPS_name, "OF") == 0) {
				s = 25;
			}
			break;
		case 'P': 
			OfrontOPS_Identifier(&s);
			if (__STRCMP(OfrontOPS_name, "PROCEDURE") == 0) {
				s = 61;
			} else if (__STRCMP(OfrontOPS_name, "POINTER") == 0) {
				s = 56;
			}
			break;
		case 'R': 
			OfrontOPS_Identifier(&s);
			if (__STRCMP(OfrontOPS_name, "RECORD") == 0) {
				s = 55;
			} else if (__STRCMP(OfrontOPS_name, "REPEAT") == 0) {
				s = 48;
			} else if (__STRCMP(OfrontOPS_name, "RETURN") == 0) {
				s = 53;
			}
			break;
		case 'T': 
			OfrontOPS_Identifier(&s);
			if (__STRCMP(OfrontOPS_name, "THEN") == 0) {
				s = 26;
			} else if (__STRCMP(OfrontOPS_name, "TO") == 0) {
				s = 28;
			} else if (__STRCMP(OfrontOPS_name, "TYPE") == 0) {
				s = 59;
			}
			break;
		case 'U': 
			OfrontOPS_Identifier(&s);
			if (__STRCMP(OfrontOPS_name, "UNTIL") == 0) {
				s = 44;
			}
			break;
		case 'V': 
			OfrontOPS_Identifier(&s);
			if (__STRCMP(OfrontOPS_name, "VAR") == 0) {
				s = 60;
			}
			break;
		case 'W': 
			OfrontOPS_Identifier(&s);
			if (__STRCMP(OfrontOPS_name, "WHILE") == 0) {
				s = 47;
			} else if (__STRCMP(OfrontOPS_name, "WITH") == 0) {
				s = 51;
			}
			break;
		case 'G': case 'H': case 'J': case 'K': case 'Q': 
		case 'S': case 'X': case 'Y': case 'Z': case '_': 
			OfrontOPS_Identifier(&s);
			break;
		case '[': 
			s = 31;
			OfrontOPM_Get(&OfrontOPS_ch);
			break;
		case ']': 
			s = 23;
			OfrontOPM_Get(&OfrontOPS_ch);
			break;
		case '^': 
			s = 17;
			OfrontOPM_Get(&OfrontOPS_ch);
			break;
		case 'a': case 'b': case 'c': case 'd': case 'e': 
		case 'f': case 'g': case 'h': case 'i': case 'j': 
		case 'k': case 'l': case 'm': case 'n': case 'o': 
		case 'p': case 'q': case 'r': case 's': case 't': 
		case 'u': case 'v': case 'w': case 'x': case 'y': 
		case 'z': 
			OfrontOPS_Identifier(&s);
			break;
		case '{': 
			s = 32;
			OfrontOPM_Get(&OfrontOPS_ch);
			break;
		case '|': 
			s = 40;
			OfrontOPM_Get(&OfrontOPS_ch);
			break;
		case '}': 
			s = 24;
			OfrontOPM_Get(&OfrontOPS_ch);
			break;
		case '~': 
			s = 33;
			OfrontOPM_Get(&OfrontOPS_ch);
			break;
		case 0x7f: 
			s = 21;
			OfrontOPM_Get(&OfrontOPS_ch);
			break;
		default: 
			s = 0;
			OfrontOPM_Get(&OfrontOPS_ch);
			break;
	}
	*sym = s;
	Get__1_s = _s.lnk;
}

/*----------------------------------------------------------------------------*/
void OfrontOPS_Init (void)
{
	OfrontOPS_ch = ' ';
}

/*----------------------------------------------------------------------------*/

export void *OfrontOPS__init(void)
{
	__DEFMOD;
	__IMPORT(OfrontOPM__init);
	__REGMOD("OfrontOPS", 0);
	__REGCMD("Init", OfrontOPS_Init);
/* BEGIN */
	__ENDMOD;
}
