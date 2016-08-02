/* Ofront+ 1.0 -xtspkaem */
#include "SYSTEM.h"
#include "Args.h"
#include "Console.h"
#include "Files.h"
#include "Texts.h"




export void OCatCmd_Cat (void);


/*============================================================================*/

static struct Cat__1 {
	CHAR (*buf)[1024];
	INTEGER *bufpos;
	struct Cat__1 *lnk;
} *Cat__1_s;

static void ConsoleChar__2 (CHAR ch);

static void ConsoleChar__2 (CHAR ch)
{
	(*Cat__1_s->buf)[__X(*Cat__1_s->bufpos, 1024)] = ch;
	*Cat__1_s->bufpos += 1;
	if (*Cat__1_s->bufpos == 1023) {
		(*Cat__1_s->buf)[__X(*Cat__1_s->bufpos, 1024)] = 0x00;
		Console_String(*Cat__1_s->buf, 1024);
		*Cat__1_s->bufpos = 0;
	}
}

void OCatCmd_Cat (void)
{
	CHAR path[128];
	INTEGER i;
	Texts_Text T = NIL;
	Texts_Reader R;
	CHAR ch;
	BOOLEAN tab;
	CHAR buf[1024];
	INTEGER bufpos;
	struct Cat__1 _s;
	_s.buf = (void*)buf;
	_s.bufpos = &bufpos;
	_s.lnk = Cat__1_s;
	Cat__1_s = &_s;
	path[0] = 0x00;
	__NEW(T, Texts_TextDesc);
	Args_Get(1, (void*)path, 128);
	if (__STRCMP(path, "-t") == 0) {
		tab = 1;
		i = 2;
		Args_Get(2, (void*)path, 128);
	} else {
		tab = 0;
		i = 1;
	}
	while (path[0] != 0x00) {
		if (Files_Old(path, 128) != NIL) {
			Texts_Open(T, path, 128);
			Texts_OpenReader(&R, Texts_Reader__typ, T, 0);
			Texts_Read(&R, Texts_Reader__typ, &ch);
			bufpos = 0;
			while (!R.eot) {
				if (ch >= ' ') {
					ConsoleChar__2(ch);
				} else if (ch == 0x09) {
					if (tab) {
						ConsoleChar__2(ch);
					} else {
						ConsoleChar__2(' ');
						ConsoleChar__2(' ');
					}
				} else if (ch == 0x0d) {
					ConsoleChar__2(0x0a);
				}
				Texts_Read(&R, Texts_Reader__typ, &ch);
			}
			buf[__X(bufpos, 1024)] = 0x00;
			Console_String(buf, 1024);
		} else {
			Console_String((CHAR*)"OCat: cannot open ", 19);
			Console_String(path, 128);
			Console_Ln();
		}
		i += 1;
		path[0] = 0x00;
		Args_Get(i, (void*)path, 128);
	}
	Cat__1_s = _s.lnk;
}

/*----------------------------------------------------------------------------*/

int main(int argc, char **argv)
{
	__INIT(argc, argv);
	__IMPORT(Args__init);
	__IMPORT(Console__init);
	__IMPORT(Files__init);
	__IMPORT(Texts__init);
	__REGMAIN("OCatCmd", 0);
	__REGCMD("Cat", OCatCmd_Cat);
/* BEGIN */
	OCatCmd_Cat();
	__FINI;
}
