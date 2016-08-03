/* Ofront+ 1.0 -xtspkae */
#include "SYSTEM.h"
#include "Args.h"
#include "Console.h"
#include "Platform.h"
#include "Texts.h"

typedef
	struct Oberon_ParRec *Oberon_ParList;

typedef
	struct Oberon_ParRec {
		Texts_Text text;
		INTEGER pos;
	} Oberon_ParRec;


export Texts_Text Oberon_Log;
export Oberon_ParList Oberon_Par;
static Texts_Reader Oberon_R;
static Texts_Writer Oberon_W;
export CHAR Oberon_OptionChar;

export LONGINT *Oberon_ParRec__typ;

export void Oberon_GetClock (INTEGER *t, INTEGER *d);
export void Oberon_GetSelection (Texts_Text *text, INTEGER *beg, INTEGER *end, INTEGER *time);
static void Oberon_LogNotifier (Texts_Text Log, SHORTINT op, INTEGER beg, INTEGER end);
static void Oberon_PopulateParams (void);
export INTEGER Oberon_Time (void);


/*============================================================================*/

void Oberon_GetClock (INTEGER *t, INTEGER *d)
{
	Platform_GetClock(&*t, &*d);
}

/*----------------------------------------------------------------------------*/
INTEGER Oberon_Time (void)
{
	return Platform_Time();
}

/*----------------------------------------------------------------------------*/
static void Oberon_PopulateParams (void)
{
	Texts_Writer W;
	SHORTINT i;
	CHAR str[32];
	i = 1;
	Texts_OpenWriter(&W, Texts_Writer__typ);
	do {
		if ((INTEGER)i < Args_argc) {
			Args_Get(i, (void*)str, 32);
			Texts_WriteString(&W, Texts_Writer__typ, str, 32);
			Texts_WriteString(&W, Texts_Writer__typ, (CHAR*)" ", 2);
		}
		i += 1;
	} while (!((INTEGER)i >= Args_argc));
	Texts_Append(Oberon_Par->text, W.buf);
}

void Oberon_GetSelection (Texts_Text *text, INTEGER *beg, INTEGER *end, INTEGER *time)
{
	*text = NIL;
	*beg = 0;
	*end = 0;
	*time = 0;
}

/*----------------------------------------------------------------------------*/
static void Oberon_LogNotifier (Texts_Text Log, SHORTINT op, INTEGER beg, INTEGER end)
{
	CHAR ch;
	Texts_OpenReader(&Oberon_R, Texts_Reader__typ, Log, beg);
	while (!Oberon_R.eot && beg < end) {
		Texts_Read(&Oberon_R, Texts_Reader__typ, &ch);
		if (ch == 0x0d) {
			Console_Ln();
		} else {
			Console_Char(ch);
		}
		beg += 1;
	}
}

static void EnumPtrs(void (*P)(void*))
{
	P(Oberon_Log);
	P(Oberon_Par);
	__ENUMR(&Oberon_R, Texts_Reader__typ, 72, 1, P);
	__ENUMR(&Oberon_W, Texts_Writer__typ, 56, 1, P);
}

__TDESC(Oberon_ParRec__desc, 1, 1) = {__TDFLDS("ParRec", 16), {0, -16}};

export void *Oberon__init(void)
{
	__DEFMOD;
	__IMPORT(Args__init);
	__IMPORT(Console__init);
	__IMPORT(Platform__init);
	__IMPORT(Texts__init);
	__REGMOD("Oberon", EnumPtrs);
	__INITYP(Oberon_ParRec, Oberon_ParRec, 0);
/* BEGIN */
	__NEW(Oberon_Par, Oberon_ParRec);
	__NEW(Oberon_Par->text, Texts_TextDesc);
	Oberon_Par->pos = 0;
	Oberon_OptionChar = '-';
	Texts_Open(Oberon_Par->text, (CHAR*)"", 1);
	Oberon_PopulateParams();
	__NEW(Oberon_Log, Texts_TextDesc);
	Texts_Open(Oberon_Log, (CHAR*)"", 1);
	Oberon_Log->notify = Oberon_LogNotifier;
	__ENDMOD;
}
