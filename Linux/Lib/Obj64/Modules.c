/* Ofront 1.2 -xtspkael */
#include "SYSTEM.h"
#include "Console.h"
#include "Kernel.h"
#include "Unix.h"

typedef
	struct Modules_CmdDesc *Modules_Cmd;

typedef
	void (*Modules_Command)(void);

typedef
	struct Modules_CmdDesc {
		Modules_Cmd next;
		CHAR name[24];
		Modules_Command cmd;
	} Modules_CmdDesc;

typedef
	struct Modules_ModuleDesc *Modules_Module;

typedef
	CHAR Modules_ModuleName[20];

typedef
	struct Modules_ModuleDesc {
		Modules_Module next;
		Modules_ModuleName name;
		LONGINT refcnt;
		Modules_Cmd cmds;
		LONGINT types;
		void (*enumPtrs)(void(*)(LONGINT));
		LONGINT reserved1, reserved2;
	} Modules_ModuleDesc;


export INTEGER Modules_res;
export CHAR Modules_resMsg[256];
export Modules_ModuleName Modules_imported, Modules_importing;
export BOOLEAN Modules_trace;

export LONGINT *Modules_ModuleDesc__typ;
export LONGINT *Modules_CmdDesc__typ;

static void Modules_Append (CHAR *a, LONGINT a__len, CHAR *b, LONGINT b__len);
export void Modules_Free (CHAR *name, LONGINT name__len, BOOLEAN all);
static BOOLEAN Modules_FullName (CHAR *n, LONGINT n__len);
static void Modules_GetSubsys1 (CHAR *n, LONGINT n__len, CHAR *s, LONGINT s__len);
static void Modules_GetSubsys2 (CHAR *n, LONGINT n__len, CHAR *s, LONGINT s__len);
static void Modules_Load (CHAR *name, LONGINT name__len, CHAR *bodyname, LONGINT bodyname__len, LONGINT *lib, Modules_Command *body);
export Modules_Command Modules_ThisCommand (Modules_Module mod, CHAR *name, LONGINT name__len);
export Modules_Module Modules_ThisMod (CHAR *name, LONGINT name__len);
static void Modules_err (void);

#define Modules_dlclose(handle)	dlclose((void*)handle)
#define Modules_dlerror(s, s__len)	__COPY(dlerror(), s, s__len)
#define Modules_dlopen(path, path__len)	(long)dlopen((const char*)path, RTLD_LAZY)
#define Modules_dlsym(handle, name, name__len)	(Modules_Command)dlsym((void*)handle, name)
#include <dlfcn.h>
#define Modules_modules()	(Modules_Module)SYSTEM_modules
#define Modules_next()	(LONGINT)RTLD_NEXT
#define Modules_setmodules(m)	SYSTEM_modules = m

static void Modules_Append (CHAR *a, LONGINT a__len, CHAR *b, LONGINT b__len)
{
	INTEGER i, j;
	__DUP(b, b__len, CHAR);
	i = 0;
	while (a[__X(i, a__len)] != 0x00) {
		i += 1;
	}
	j = 0;
	while (b[__X(j, b__len)] != 0x00) {
		a[__X(i, a__len)] = b[__X(j, b__len)];
		i += 1;
		j += 1;
	}
	a[__X(i, a__len)] = 0x00;
	__DEL(b);
}

static void Modules_GetSubsys1 (CHAR *n, LONGINT n__len, CHAR *s, LONGINT s__len)
{
	INTEGER i;
	CHAR ch;
	__DUP(n, n__len, CHAR);
	ch = n[0];
	i = 0;
	while (ch != 0x00 && (ch < 'a' || ch > 'z')) {
		s[__X(i, s__len)] = ch;
		i += 1;
		ch = n[__X(i, n__len)];
	}
	while (ch >= 'a' && ch <= 'z') {
		s[__X(i, s__len)] = ch;
		i += 1;
		ch = n[__X(i, n__len)];
	}
	if (ch == 0x00) {
		s[0] = 0x00;
	} else {
		s[__X(i, s__len)] = 0x00;
	}
	__DEL(n);
}

static void Modules_GetSubsys2 (CHAR *n, LONGINT n__len, CHAR *s, LONGINT s__len)
{
	INTEGER i;
	CHAR ch;
	__DUP(n, n__len, CHAR);
	ch = n[0];
	i = 0;
	while (ch >= 'A' && ch <= 'Z') {
		s[__X(i, s__len)] = ch;
		i += 1;
		ch = n[__X(i, n__len)];
	}
	while (ch != 0x00 && (ch < 'A' || ch > 'Z')) {
		s[__X(i, s__len)] = ch;
		i += 1;
		ch = n[__X(i, n__len)];
	}
	if (ch == 0x00) {
		s[0] = 0x00;
	} else {
		s[__X(i, s__len)] = 0x00;
	}
	__DEL(n);
}

static BOOLEAN Modules_FullName (CHAR *n, LONGINT n__len)
{
	INTEGER i;
	CHAR ch;
	i = 0;
	ch = n[0];
	while (ch != 0x00) {
		if (ch == '.') {
			return 1;
		}
		i += 1;
		ch = n[__X(i, n__len)];
	}
	return 0;
}

static void Modules_err (void)
{
	INTEGER i;
	CHAR s[1024];
	i = 0;
	Modules_dlerror((void*)s, 1024);
	if (Modules_trace) {
		Console_String(s, 1024);
		Console_Ln();
	}
}

static void Modules_Load (CHAR *name, LONGINT name__len, CHAR *bodyname, LONGINT bodyname__len, LONGINT *lib, Modules_Command *body)
{
	CHAR libname[256];
	__DUP(name, name__len, CHAR);
	__DUP(bodyname, bodyname__len, CHAR);
	__MOVE("lib", libname, 4);
	Modules_Append((void*)libname, 256, name, name__len);
	Modules_Append((void*)libname, 256, (CHAR*)".so", (LONGINT)4);
	if (Modules_trace) {
		Console_String((CHAR*)"Modules.Load(libname=", (LONGINT)22);
		Console_String(libname, 256);
		Console_String((CHAR*)", entry=", (LONGINT)9);
		Console_String(bodyname, bodyname__len);
		Console_String((CHAR*)")", (LONGINT)2);
		Console_Ln();
	}
	*lib = Modules_dlopen(libname, 256);
	if (*lib != 0) {
		*body = Modules_dlsym(*lib, bodyname, bodyname__len);
		if (*body == NIL) {
			Modules_err();
		}
	} else {
		Modules_err();
	}
	__DEL(name);
	__DEL(bodyname);
}

Modules_Module Modules_ThisMod (CHAR *name, LONGINT name__len)
{
	Modules_Module m = NIL;
	CHAR bodyname[64], libname1[64], libname2[64];
	Modules_Command body;
	LONGINT lib, handle;
	__DUP(name, name__len, CHAR);
	m = Modules_modules();
	while (m != NIL && __STRCMP(m->name, name) != 0) {
		m = m->next;
	}
	if (m == NIL) {
		if (Modules_trace) {
			Console_String((CHAR*)"Modules.ThisMod(name=", (LONGINT)22);
			Console_String(name, name__len);
			Console_String((CHAR*)")", (LONGINT)2);
			Console_Ln();
		}
		handle = 0;
		body = NIL;
		__COPY(name, bodyname, 64);
		Modules_Append((void*)bodyname, 64, (CHAR*)"__init", (LONGINT)7);
		Modules_Load(name, name__len, bodyname, 64, &lib, &body);
		if (body != NIL) {
			handle = lib;
		} else {
			Modules_GetSubsys1(name, name__len, (void*)libname1, 64);
			if (libname1[0] != 0x00) {
				Modules_Load(libname1, 64, bodyname, 64, &lib, &body);
			}
		}
		if (body == NIL) {
			Modules_GetSubsys2(name, name__len, (void*)libname2, 64);
			if (libname2[0] != 0x00) {
				if (__STRCMP(libname2, libname1) != 0) {
					Modules_Load(libname2, 64, bodyname, 64, &lib, &body);
				}
			}
		}
		if (body == NIL) {
			if (Modules_FullName((void*)Kernel_LIB, 256)) {
				lib = Modules_dlopen(Kernel_LIB, 256);
				if (lib != 0) {
					body = Modules_dlsym(lib, bodyname, 64);
				} else {
					Modules_err();
				}
			} else {
				Modules_Load(Kernel_LIB, 256, bodyname, 64, &lib, &body);
			}
		}
		if (body != NIL) {
			(*body)();
			m = Modules_modules();
			while (m != NIL && __STRCMP(m->name, name) != 0) {
				m = m->next;
			}
			if (m != NIL) {
				m->reserved1 = handle;
			}
		}
	}
	if (m != NIL) {
		Modules_res = 0;
		Modules_resMsg[0] = 0x00;
	} else {
		Modules_res = 1;
		__COPY(name, Modules_importing, 20);
		__MOVE(" module \"", Modules_resMsg, 10);
		Modules_Append((void*)Modules_resMsg, 256, name, name__len);
		Modules_Append((void*)Modules_resMsg, 256, (CHAR*)"\" not found", (LONGINT)12);
	}
	__DEL(name);
	return m;
}

Modules_Command Modules_ThisCommand (Modules_Module mod, CHAR *name, LONGINT name__len)
{
	Modules_Cmd c = NIL;
	__DUP(name, name__len, CHAR);
	c = mod->cmds;
	while (c != NIL && __STRCMP(c->name, name) != 0) {
		c = c->next;
	}
	if (c != NIL) {
		Modules_res = 0;
		Modules_resMsg[0] = 0x00;
		__DEL(name);
		return c->cmd;
	} else {
		Modules_res = 2;
		__MOVE(" command \"", Modules_resMsg, 11);
		__COPY(name, Modules_importing, 20);
		Modules_Append((void*)Modules_resMsg, 256, mod->name, 20);
		Modules_Append((void*)Modules_resMsg, 256, (CHAR*)".", (LONGINT)2);
		Modules_Append((void*)Modules_resMsg, 256, name, name__len);
		Modules_Append((void*)Modules_resMsg, 256, (CHAR*)"\" not found", (LONGINT)12);
		__DEL(name);
		return NIL;
	}
	__RETCHK;
}

void Modules_Free (CHAR *name, LONGINT name__len, BOOLEAN all)
{
	Modules_Module m = NIL, p = NIL;
	__DUP(name, name__len, CHAR);
	m = Modules_modules();
	if (all) {
		Modules_res = 1;
		__MOVE("unloading \"all\" not yet supported", Modules_resMsg, 34);
	} else {
		while (m != NIL && __STRCMP(m->name, name) != 0) {
			p = m;
			m = m->next;
		}
		if (m != NIL && m->refcnt == 0) {
			if (m->reserved1 != 0) {
				if (Modules_dlclose(m->reserved1) != 0) {
					Modules_res = 1;
					Modules_dlerror((void*)Modules_resMsg, 256);
				} else {
					Modules_res = 0;
					if (m == Modules_modules()) {
						Modules_setmodules(m->next);
					} else {
						p->next = m->next;
					}
				}
			} else {
				Modules_res = 1;
				__MOVE("module not loaded in separate library", Modules_resMsg, 38);
			}
		} else {
			Modules_res = 1;
			if (m == NIL) {
				__MOVE("module not found", Modules_resMsg, 17);
			} else {
				__MOVE("clients of this module exist", Modules_resMsg, 29);
			}
		}
	}
	__DEL(name);
}

__TDESC(Modules_ModuleDesc__desc, 1, 2) = {__TDFLDS("ModuleDesc", 80), {0, 40, -24}};
__TDESC(Modules_CmdDesc__desc, 1, 1) = {__TDFLDS("CmdDesc", 40), {0, -16}};

export void *Modules__init(void)
{
	__DEFMOD;
	__IMPORT(Console__init);
	__IMPORT(Kernel__init);
	__IMPORT(Unix__init);
	__REGMOD("Modules", 0);
	__INITYP(Modules_ModuleDesc, Modules_ModuleDesc, 0);
	__INITYP(Modules_CmdDesc, Modules_CmdDesc, 0);
/* BEGIN */
	__ENDMOD;
}
