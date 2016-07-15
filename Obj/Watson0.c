/*  Ofront 1.2 -xtspkae */
#include "SYSTEM.h"
#include "Files.h"
#include "Modules.h"
#include "OfrontOPM.h"
#include "OfrontOPS.h"
#include "OfrontOPT.h"
#include "OfrontOPV.h"
#include "Console.h"
#include "CmdlnTexts.h"


static CmdlnTexts_Writer Watson0_W;
export CHAR Watson0_options[12];
static BOOLEAN Watson0_expandTypes, Watson0_showAdr;
static CHAR Watson0_Hex[17];
static OfrontOPT_Object Watson0_curObj;
static LONGINT Watson0_syspos;
static BOOLEAN Watson0_sysImported, Watson0_first, Watson0_showObj;


static void Watson0_Append (CHAR *d, LONGINT d__len, CHAR *s, LONGINT s__len);
static void Watson0_AppendCh (CHAR *d, LONGINT d__len, CHAR ch);
static void Watson0_Ch (CHAR ch);
static void Watson0_Commands (Files_File F, CHAR *modName, LONGINT modName__len);
static void Watson0_Control (CHAR *name, LONGINT name__len, CHAR *cmd, LONGINT cmd__len, CHAR *par, LONGINT par__len);
static void Watson0_Find (OfrontOPT_Object root, CHAR *name, LONGINT name__len, OfrontOPT_Object *obj);
static void Watson0_FindPointer (OfrontOPT_Object root, CHAR *name, LONGINT name__len, OfrontOPT_Object *obj);
static void Watson0_GroupTypes (OfrontOPT_Object root, OfrontOPT_Object obj);
static void Watson0_Hexa (LONGINT i, BOOLEAN short_);
static void Watson0_Identifier (CHAR *name, LONGINT name__len);
static void Watson0_Indent (INTEGER i);
static void Watson0_Ln (void);
static void Watson0_MakeText (CmdlnTexts_Text T);
static void Watson0_Num (LONGINT i);
static OfrontOPT_Object Watson0_ReadSym (CHAR *mod, LONGINT mod__len, CHAR *fileName, LONGINT fileName__len);
static void Watson0_SetOptions (void);
export void Watson0_ShowCmd (CHAR *file, LONGINT file__len, CmdlnTexts_Text T);
export void Watson0_ShowDef (CHAR *mod, LONGINT mod__len, CmdlnTexts_Text T);
export void Watson0_ShowObj (CHAR *mod, LONGINT mod__len, CHAR *objName, LONGINT objName__len, CmdlnTexts_Text T);
static void Watson0_Str (CHAR *s, LONGINT s__len);
static void Watson0_WriteBase (OfrontOPT_Struct typ);
static void Watson0_WriteConstValue (OfrontOPT_Object obj);
static void Watson0_WriteFieldList (OfrontOPT_Object obj, INTEGER i);
static void Watson0_WriteFields (OfrontOPT_Object *obj, INTEGER i, BOOLEAN recField);
static void Watson0_WriteForm (OfrontOPT_Struct typ, INTEGER i);
static void Watson0_WriteImports (void);
static void Watson0_WriteInstVars (OfrontOPT_Struct typ, INTEGER i, LONGINT extlev);
static void Watson0_WriteModule (CHAR *mod, LONGINT mod__len, OfrontOPT_Object root);
static void Watson0_WriteObject (OfrontOPT_Object obj, LONGINT mode);
static void Watson0_WriteParams (OfrontOPT_Object param, OfrontOPT_Struct resTyp);
static void Watson0_WriteProc (OfrontOPT_Object obj);
static void Watson0_WriteScope (OfrontOPT_Object obj, INTEGER mode);
static void Watson0_WriteTProcs (OfrontOPT_Object obj, INTEGER i);
static void Watson0_WriteType (OfrontOPT_Struct typ, INTEGER i);


/*============================================================================*/

static void Watson0_Append (CHAR *d, LONGINT d__len, CHAR *s, LONGINT s__len)
{
	INTEGER i, j;
	CHAR ch;
	__DUP(s, s__len, CHAR);
	i = 0;
	while (d[__X(i, d__len)] != 0x00) {
		i += 1;
	}
	j = 0;
	do {
		ch = s[__X(j, s__len)];
		d[__X(i, d__len)] = ch;
		i += 1;
		j += 1;
	} while (!(ch == 0x00));
	__DEL(s);
}

static void Watson0_AppendCh (CHAR *d, LONGINT d__len, CHAR ch)
{
	INTEGER i;
	i = 0;
	while (d[__X(i, d__len)] != 0x00) {
		i += 1;
	}
	d[__X(i, d__len)] = ch;
	d[__X(i + 1, d__len)] = 0x00;
}

static void Watson0_Str (CHAR *s, LONGINT s__len)
{
	__DUP(s, s__len, CHAR);
	CmdlnTexts_WriteString(&Watson0_W, CmdlnTexts_Writer__typ, s, s__len);
	__DEL(s);
}

static void Watson0_Ch (CHAR ch)
{
	CmdlnTexts_Write(&Watson0_W, CmdlnTexts_Writer__typ, ch);
}

static void Watson0_Num (LONGINT i)
{
	CmdlnTexts_WriteInt(&Watson0_W, CmdlnTexts_Writer__typ, i, 0);
}

static void Watson0_Hexa (LONGINT i, BOOLEAN short_)
{
	if (!short_) {
		Watson0_Ch(Watson0_Hex[__X(__ASHR(i, 28, LONGINT), 17)]);
		i = __MASK(i, -268435456);
		Watson0_Ch(Watson0_Hex[__X(__ASHR(i, 24, LONGINT), 17)]);
		i = __MASK(i, -16777216);
		Watson0_Ch(Watson0_Hex[__X(__ASHR(i, 20, LONGINT), 17)]);
		i = __MASK(i, -1048576);
		Watson0_Ch(Watson0_Hex[__X(__ASHR(i, 16, LONGINT), 17)]);
		i = __MASK(i, -65536);
	}
	Watson0_Ch(Watson0_Hex[__X(__ASHR(i, 12, LONGINT), 17)]);
	i = __MASK(i, -4096);
	Watson0_Ch(Watson0_Hex[__X(__ASHR(i, 8, LONGINT), 17)]);
	i = __MASK(i, -256);
	Watson0_Ch(Watson0_Hex[__X(__ASHR(i, 4, LONGINT), 17)]);
	Watson0_Ch(Watson0_Hex[__X(__MASK(i, -16), 17)]);
}

static void Watson0_Ln (void)
{
	CmdlnTexts_WriteLn(&Watson0_W, CmdlnTexts_Writer__typ);
}

static void Watson0_Indent (INTEGER i)
{
	while (i > 0) {
		Watson0_Ch(0x09);
		i -= 1;
	}
}

static void Watson0_Control (CHAR *name, LONGINT name__len, CHAR *cmd, LONGINT cmd__len, CHAR *par, LONGINT par__len)
{
	__DUP(name, name__len, CHAR);
	__DUP(cmd, cmd__len, CHAR);
	__DUP(par, par__len, CHAR);
	__DEL(name);
	__DEL(cmd);
	__DEL(par);
}

static void Watson0_Identifier (CHAR *name, LONGINT name__len)
{
	INTEGER i, j;
	__DUP(name, name__len, CHAR);
	Watson0_Str(name, name__len);
	if (Watson0_showObj) {
		i = 0;
		do {
			i += 1;
		} while (!(name[__X(i, name__len)] == '.' || name[__X(i, name__len)] == 0x00));
		if (name[__X(i, name__len)] != 0x00) {
			i += 1;
			j = 0;
			do {
				name[__X(j, name__len)] = name[__X(i, name__len)];
				i += 1;
				j += 1;
			} while (!(name[__X(j, name__len)] == 0x00));
		}
	}
	Watson0_Control(name, name__len, (CHAR*)"", (LONGINT)1, (CHAR*)"", (LONGINT)1);
	__DEL(name);
}

static void Watson0_WriteConstValue (OfrontOPT_Object obj)
{
	INTEGER i;
	LONGINT y;
	SET s;
	y = obj->conval->intval;
	switch (obj->typ->form) {
		case 4: case 5: case 6: 
			CmdlnTexts_WriteInt(&Watson0_W, CmdlnTexts_Writer__typ, y, 0);
			break;
		case 7: 
			CmdlnTexts_WriteReal(&Watson0_W, CmdlnTexts_Writer__typ, obj->conval->realval, 15);
			break;
		case 8: 
			CmdlnTexts_WriteLongReal(&Watson0_W, CmdlnTexts_Writer__typ, obj->conval->realval, 20);
			break;
		case 2: 
			if (y == 0) {
				Watson0_Str((CHAR*)"FALSE", (LONGINT)6);
			} else {
				Watson0_Str((CHAR*)"TRUE", (LONGINT)5);
			}
			break;
		case 3: 
			if (y >= 32 && y <= 126) {
				Watson0_Ch('\"');
				Watson0_Ch((CHAR)y);
				Watson0_Ch('\"');
			} else {
				i = (INTEGER)__ASHR(y, 4, LONGINT);
				if (i > 0) {
					if (i > 9) {
						Watson0_Ch('0');
					}
					Watson0_Ch(Watson0_Hex[__X(i, 17)]);
				}
				Watson0_Ch(Watson0_Hex[__X(__MASK(y, -16), 17)]);
				Watson0_Ch('X');
			}
			break;
		case 9: 
			Watson0_Ch('{');
			s = obj->conval->setval;
			y = 0;
			while (y <= 31) {
				if (__IN(y, s)) {
					CmdlnTexts_WriteInt(&Watson0_W, CmdlnTexts_Writer__typ, y, 0);
					s &= ~__SETOF(y);
					if (s != 0x0) {
						Watson0_Str((CHAR*)", ", (LONGINT)3);
					}
				}
				y += 1;
			}
			Watson0_Ch('}');
			break;
		case 11: 
			Watson0_Str((CHAR*)"NIL", (LONGINT)4);
			break;
		case 10: 
			Watson0_Ch('\"');
			Watson0_Str(*obj->conval->ext, 256);
			Watson0_Ch('\"');
			break;
		default: __CASECHK;
	}
}

static void Watson0_WriteBase (OfrontOPT_Struct typ)
{
	OfrontOPT_Struct base = NIL;
	base = typ->BaseTyp;
	if (base != NIL) {
		Watson0_Str((CHAR*)" ( ", (LONGINT)4);
		Watson0_WriteType(base, 0);
		if (Watson0_expandTypes) {
			Watson0_WriteBase(base);
		}
		Watson0_Str((CHAR*)" )", (LONGINT)3);
	}
}

static void Watson0_WriteFields (OfrontOPT_Object *obj, INTEGER i, BOOLEAN recField)
{
	OfrontOPT_Struct typ = NIL;
	LONGINT mode;
	typ = (*obj)->typ;
	mode = (*obj)->mode;
	for (;;) {
		if (recField && Watson0_showAdr) {
			Watson0_Ch('[');
			Watson0_Num((*obj)->adr);
			Watson0_Str((CHAR*)"] ", (LONGINT)3);
		}
		Watson0_Str((*obj)->name, 32);
		if ((*obj)->vis == 2) {
			Watson0_Ch('-');
		}
		do {
			*obj = (*obj)->link;
		} while (!(*obj == NIL || (*obj)->name[0] != 0x00));
		if ((*obj == NIL || (LONGINT)(*obj)->mode != mode) || (*obj)->typ != typ) {
			break;
		}
		Watson0_Str((CHAR*)", ", (LONGINT)3);
	}
	Watson0_Str((CHAR*)": ", (LONGINT)3);
	Watson0_WriteType(typ, i + 1);
}

static void Watson0_WriteParams (OfrontOPT_Object param, OfrontOPT_Struct resTyp)
{
	if (param != NIL || resTyp != OfrontOPT_notyp) {
		Watson0_Str((CHAR*)" (", (LONGINT)3);
		while (param != NIL) {
			if (param->mode == 2) {
				Watson0_Str((CHAR*)"VAR ", (LONGINT)5);
			}
			if (param->name[0] == 0x00) {
				Watson0_WriteType(param->typ, 0);
				param = param->link;
				if (param != NIL) {
					Watson0_Str((CHAR*)", ", (LONGINT)3);
				}
			} else {
				Watson0_WriteFields(&param, 0, 0);
				if (param != NIL) {
					Watson0_Str((CHAR*)"; ", (LONGINT)3);
				}
			}
		}
		Watson0_Ch(')');
	}
	if (resTyp != OfrontOPT_notyp) {
		Watson0_Str((CHAR*)": ", (LONGINT)3);
		Watson0_WriteType(resTyp, 0);
	}
}

static void Watson0_WriteProc (OfrontOPT_Object obj)
{
	if (Watson0_showAdr && obj->mode != 9) {
		Watson0_Ch('[');
		Watson0_Num(obj->adr);
		Watson0_Str((CHAR*)"] ", (LONGINT)3);
	}
	Watson0_Str((CHAR*)"PROCEDURE ", (LONGINT)11);
	Watson0_Identifier(obj->name, 32);
	Watson0_WriteParams(obj->link, obj->typ);
	Watson0_Ch(';');
}

static void Watson0_WriteTProcs (OfrontOPT_Object obj, INTEGER i)
{
	OfrontOPT_Object firstpar = NIL;
	if (obj != NIL) {
		Watson0_WriteTProcs(obj->left, i);
		if (obj->mode == 13) {
			Watson0_Indent(i);
			if (Watson0_showAdr) {
				Watson0_Ch('[');
				Watson0_Num(__MASK(obj->adr, -65536));
				Watson0_Str((CHAR*)", ", (LONGINT)3);
				Watson0_Num(__ASHR(obj->adr, 16, LONGINT));
				Watson0_Str((CHAR*)"] ", (LONGINT)3);
			}
			Watson0_Str((CHAR*)"PROCEDURE ", (LONGINT)11);
			firstpar = obj->link;
			Watson0_Str(obj->name, 32);
			Watson0_WriteParams(firstpar->link, obj->typ);
			Watson0_Ch(';');
			Watson0_Ln();
		}
		Watson0_WriteTProcs(obj->right, i);
	}
}

static void Watson0_WriteFieldList (OfrontOPT_Object obj, INTEGER i)
{
	while (obj != NIL && obj->mode == 4) {
		if (obj->vis != 0) {
			Watson0_Indent(i);
			Watson0_WriteFields(&obj, i, 1);
			Watson0_Ch(';');
			Watson0_Ln();
		} else {
			obj = obj->link;
		}
	}
}

static void Watson0_WriteInstVars (OfrontOPT_Struct typ, INTEGER i, LONGINT extlev)
{
	if (typ != NIL) {
		if (Watson0_expandTypes && extlev > 0) {
			Watson0_WriteInstVars(typ->BaseTyp, i, extlev);
			Watson0_Indent(i);
			Watson0_Str((CHAR*)"(* ", (LONGINT)4);
			Watson0_Ch('.');
			Watson0_Str(typ->strobj->name, 32);
			Watson0_Str((CHAR*)" *)", (LONGINT)4);
			Watson0_Ln();
		}
		Watson0_WriteFieldList(typ->link, i);
		Watson0_WriteTProcs(typ->link, i);
	}
}

static void Watson0_WriteForm (OfrontOPT_Struct typ, INTEGER i)
{
	if (typ->form == 15 && typ->comp == 6) {
		Watson0_Str((CHAR*)"RECORD", (LONGINT)7);
		Watson0_WriteBase(typ);
		if (Watson0_showAdr) {
			Watson0_Str((CHAR*)" [size = ", (LONGINT)10);
			Watson0_Num(typ->size);
			Watson0_Ch(']');
		}
		if (typ->link != NIL) {
			Watson0_Ln();
			Watson0_WriteInstVars(typ, i, typ->extlev);
			Watson0_Indent(i - 1);
		} else {
			Watson0_Ch(' ');
		}
		Watson0_Str((CHAR*)"END", (LONGINT)4);
	} else if (typ->form == 15 && typ->comp == 2) {
		Watson0_Str((CHAR*)"ARRAY ", (LONGINT)7);
		Watson0_Num(typ->n);
		Watson0_Str((CHAR*)" OF ", (LONGINT)5);
		Watson0_WriteType(typ->BaseTyp, i);
	} else if (typ->form == 15 && typ->comp == 5) {
		Watson0_Str((CHAR*)"ARRAY OF ", (LONGINT)10);
		Watson0_WriteType(typ->BaseTyp, i);
	} else if (typ->form == 15 && typ->comp == 4) {
		Watson0_Str((CHAR*)"ARRAY * OF ", (LONGINT)12);
		Watson0_WriteType(typ->BaseTyp, i);
	} else if (typ->form == 13) {
		Watson0_Str((CHAR*)"POINTER TO ", (LONGINT)12);
	} else if (typ->form == 14) {
		Watson0_Str((CHAR*)"PROCEDURE", (LONGINT)10);
		Watson0_WriteParams(typ->link, typ->BaseTyp);
	}
}

static void Watson0_WriteType (OfrontOPT_Struct typ, INTEGER i)
{
	CHAR cmd[32], par[32];
	BOOLEAN makeLink;
	CHAR ch;
	if (typ->strobj != NIL && typ->strobj->name[0] != 0x00) {
		makeLink = 0;
		if (typ == OfrontOPT_bytetyp) {
			Watson0_Str((CHAR*)"SYSTEM.", (LONGINT)8);
			Watson0_sysImported = 1;
		} else if (typ->form >= 13 && (typ->mno >= 1 || __ABS(typ->strobj->vis) > 0)) {
			makeLink = 1;
			CmdlnTexts_SetColor(&Watson0_W, CmdlnTexts_Writer__typ, 3);
			if (typ->mno >= 1 || Watson0_showObj) {
			}
		}
		Watson0_Str(typ->strobj->name, 32);
		if (makeLink) {
			if (typ->mno >= 1) {
				__MOVE("Watson.ShowObj ", cmd, 16);
			} else {
				if ((!Watson0_showObj || Watson0_curObj->link == typ->strobj) || Watson0_curObj == typ->strobj) {
					__MOVE("Watson.Goto ", cmd, 13);
					par[0] = 0x00;
				} else {
					__MOVE("Watson.ShowObj ", cmd, 16);
				}
				Watson0_Append((void*)par, 32, typ->strobj->name, 32);
			}
			Watson0_Control((CHAR*)"", (LONGINT)1, cmd, 32, par, 32);
			CmdlnTexts_SetColor(&Watson0_W, CmdlnTexts_Writer__typ, 15);
		}
	} else {
		Watson0_WriteForm(typ, i);
	}
}

static void Watson0_WriteObject (OfrontOPT_Object obj, LONGINT mode)
{
	OfrontOPT_Object h = NIL;
	CHAR name[64];
	Watson0_curObj = obj;
	if (mode == 3) {
		if (Watson0_first) {
			Watson0_Indent(1);
			Watson0_Str((CHAR*)"CONST", (LONGINT)6);
			Watson0_Ln();
			Watson0_first = 0;
		}
		Watson0_Indent(2);
		Watson0_Identifier(obj->name, 32);
		Watson0_Str((CHAR*)" = ", (LONGINT)4);
		Watson0_WriteConstValue(obj);
		Watson0_Ch(';');
		Watson0_Ln();
	} else if (mode == 1) {
		if (Watson0_first) {
			Watson0_Indent(1);
			Watson0_Str((CHAR*)"VAR", (LONGINT)4);
			Watson0_Ln();
			Watson0_first = 0;
		}
		Watson0_Indent(2);
		if (Watson0_showAdr) {
			Watson0_Str((CHAR*)" [", (LONGINT)3);
			Watson0_Num(obj->adr);
			Watson0_Str((CHAR*)"] ", (LONGINT)3);
		}
		Watson0_Identifier(obj->name, 32);
		if (obj->vis == 2) {
			Watson0_Ch('-');
		}
		Watson0_Str((CHAR*)": ", (LONGINT)3);
		Watson0_WriteType(obj->typ, 3);
		Watson0_Ch(';');
		Watson0_Ln();
	} else if (mode == 5 && obj->vis > 0) {
		if (Watson0_first) {
			Watson0_Indent(1);
			Watson0_Str((CHAR*)"TYPE", (LONGINT)5);
			Watson0_Ln();
			Watson0_first = 0;
		}
		Watson0_Indent(2);
		Watson0_Append((void*)name, 64, obj->name, 32);
		Watson0_Identifier(name, 64);
		Watson0_Str((CHAR*)" = ", (LONGINT)4);
		if (obj->typ->strobj != obj) {
			Watson0_WriteType(obj->typ, 0);
		} else {
			Watson0_WriteForm(obj->typ, 3);
		}
		Watson0_Ch(';');
		Watson0_Ln();
		if (obj->link != NIL) {
			h = obj->link;
			if (__ABS(h->vis) > 0) {
				Watson0_Indent(2);
				Watson0_Append((void*)name, 64, h->name, 32);
				Watson0_Identifier(name, 64);
				Watson0_Str((CHAR*)" = ", (LONGINT)4);
				Watson0_WriteForm(h->typ, 3);
				Watson0_Ch(';');
				Watson0_Ln();
			}
		}
	} else if (__IN(mode, 0x0280)) {
		Watson0_first = 0;
		Watson0_Indent(1);
		Watson0_WriteProc(obj);
		Watson0_Ln();
	}
}

static void Watson0_WriteImports (void)
{
	INTEGER i;
	i = 1;
	Watson0_first = 1;
	if (!Watson0_first) {
		Watson0_Ch(';');
		Watson0_Ln();
		Watson0_Ln();
	}
}

static struct WriteScope__34 {
	struct WriteScope__34 *lnk;
} *WriteScope__34_s;

static void Scope__35 (OfrontOPT_Object obj, INTEGER mode);

static void Scope__35 (OfrontOPT_Object obj, INTEGER mode)
{
	if (obj != NIL) {
		Scope__35(obj->left, mode);
		if ((INTEGER)obj->mode == mode) {
			Watson0_WriteObject(obj, mode);
		}
		Scope__35(obj->right, mode);
	}
}

static void Watson0_WriteScope (OfrontOPT_Object obj, INTEGER mode)
{
	struct WriteScope__34 _s;
	_s.lnk = WriteScope__34_s;
	WriteScope__34_s = &_s;
	Watson0_first = 1;
	Scope__35(obj, mode);
	if (!Watson0_first) {
		Watson0_Ln();
	}
	WriteScope__34_s = _s.lnk;
}

static void Watson0_WriteModule (CHAR *mod, LONGINT mod__len, OfrontOPT_Object root)
{
	__DUP(mod, mod__len, CHAR);
	Watson0_sysImported = 0;
	Watson0_Str((CHAR*)"DEFINITION ", (LONGINT)12);
	Watson0_Str(mod, mod__len);
	if (Watson0_showAdr) {
		Watson0_Str((CHAR*)" [", (LONGINT)3);
		CmdlnTexts_WriteHex(&Watson0_W, CmdlnTexts_Writer__typ, root->adr);
		Watson0_Ch(']');
	}
	Watson0_Ch(';');
	Watson0_Ln();
	Watson0_Ln();
	Watson0_syspos = Watson0_W.buf->len;
	Watson0_WriteImports();
	Watson0_WriteScope(root, 3);
	Watson0_WriteScope(root, 5);
	Watson0_WriteScope(root, 1);
	Watson0_WriteScope(root, 7);
	Watson0_Str((CHAR*)"END ", (LONGINT)5);
	Watson0_Str(mod, mod__len);
	Watson0_Ch('.');
	Watson0_Ln();
	__DEL(mod);
}

static void Watson0_MakeText (CmdlnTexts_Text T)
{
	CmdlnTexts_Append(T, Watson0_W.buf);
	if (Watson0_sysImported && !Watson0_showObj) {
		CmdlnTexts_Insert(T, Watson0_syspos, Watson0_W.buf);
	}
}

static void Watson0_Find (OfrontOPT_Object root, CHAR *name, LONGINT name__len, OfrontOPT_Object *obj)
{
	__DUP(name, name__len, CHAR);
	*obj = root;
	for (;;) {
		if (*obj == NIL) {
			__DEL(name);
			return;
		} else if (__STRCMP(name, (*obj)->name) < 0) {
			*obj = (*obj)->left;
		} else if (__STRCMP(name, (*obj)->name) > 0) {
			*obj = (*obj)->right;
		} else {
			__DEL(name);
			return;
		}
	}
	__DEL(name);
}

static struct FindPointer__7 {
	struct FindPointer__7 *lnk;
} *FindPointer__7_s;

static void FindP__8 (CHAR *name, LONGINT name__len, OfrontOPT_Object root, OfrontOPT_Object *obj);

static void FindP__8 (CHAR *name, LONGINT name__len, OfrontOPT_Object root, OfrontOPT_Object *obj)
{
	OfrontOPT_Struct typ = NIL;
	__DUP(name, name__len, CHAR);
	*obj = root;
	if (root == NIL) {
		__DEL(name);
		return;
	}
	if (root->mode == 5 && root->typ->form == 13) {
		typ = root->typ->BaseTyp;
		if (typ->strobj != NIL && __STRCMP(typ->strobj->name, name) == 0) {
			__DEL(name);
			return;
		}
	} else {
		FindP__8(name, name__len, root->left, &*obj);
		if (*obj == NIL) {
			FindP__8(name, name__len, root->right, &*obj);
		}
	}
	__DEL(name);
}

static void Watson0_FindPointer (OfrontOPT_Object root, CHAR *name, LONGINT name__len, OfrontOPT_Object *obj)
{
	struct FindPointer__7 _s;
	__DUP(name, name__len, CHAR);
	_s.lnk = FindPointer__7_s;
	FindPointer__7_s = &_s;
	FindP__8(name, name__len, root, &*obj);
	FindPointer__7_s = _s.lnk;
	__DEL(name);
}

static void Watson0_GroupTypes (OfrontOPT_Object root, OfrontOPT_Object obj)
{
	OfrontOPT_Struct typ = NIL;
	OfrontOPT_Object h = NIL;
	if (obj != NIL) {
		Watson0_GroupTypes(root, obj->left);
		if (obj->mode == 5) {
			typ = obj->typ;
			if (typ->form == 13) {
				if (typ->BaseTyp->strobj != NIL) {
					Watson0_Find(root, typ->BaseTyp->strobj->name, 32, &h);
					if (h != NIL && h->typ->mno == 1) {
						obj->link = h;
						h->vis = -h->vis;
					}
				}
			}
		}
		Watson0_GroupTypes(root, obj->right);
	}
}

static OfrontOPT_Object Watson0_ReadSym (CHAR *mod, LONGINT mod__len, CHAR *fileName, LONGINT fileName__len)
{
	CmdlnTexts_Text T = NIL;
	CmdlnTexts_Reader RR;
	Files_File f = NIL;
	Files_Rider R;
	OfrontOPS_Name selfname, file;
	CHAR ch;
	OfrontOPT_Object root = NIL;
	f = Files_Old(fileName, fileName__len);
	if (f != NIL || __STRCMP(mod, "SYSTEM") == 0) {
		if (__STRCMP(mod, "SYSTEM") == 0) {
			Console_String((CHAR*)"definition of SYSTEM not available", (LONGINT)35);
			Watson0_Ln();
			return NIL;
		} else {
			Files_Set(&R, Files_Rider__typ, f, 0);
			Files_Read(&R, Files_Rider__typ, (void*)&ch);
		}
		if (ch == 0xbb) {
			__NEW(T, CmdlnTexts_TextDesc);
			CmdlnTexts_Open(T, (CHAR*)"", (LONGINT)1);
			CmdlnTexts_OpenReader(&RR, CmdlnTexts_Reader__typ, T, 0);
			__COPY(mod, selfname, 32);
			__MOVE("$$", file, 3);
			OfrontOPT_OpenScope(0, NIL);
			Watson0_GroupTypes(root, root->scope);
			return root;
		} else {
			Console_String((CHAR*)"-- not a symbol file", (LONGINT)21);
			Console_Ln();
		}
	} else {
		Console_String((CHAR*)"--  symbol file not found", (LONGINT)26);
		Console_Ln();
	}
	return NIL;
}

static void Watson0_Commands (Files_File F, CHAR *modName, LONGINT modName__len)
{
	Files_Rider R;
	CHAR name[32];
	LONGINT ve, key, ref, symSize;
	INTEGER i, c, e, nofentry, nofcmds;
	CHAR ch;
	Files_Set(&R, Files_Rider__typ, F, 0);
	Files_Read(&R, Files_Rider__typ, (void*)&ch);
	if (ch != 0xbb) {
		Console_String((CHAR*)"not a object file", (LONGINT)18);
		return;
	}
	Files_Read(&R, Files_Rider__typ, (void*)&ch);
	Files_ReadNum(&R, Files_Rider__typ, &symSize);
	Files_Set(&R, Files_Rider__typ, F, Files_Pos(&R, Files_Rider__typ) + symSize);
	Files_ReadBytes(&R, Files_Rider__typ, (void*)&ref, 4, 4);
	Files_ReadBytes(&R, Files_Rider__typ, (void*)&nofentry, 4, 2);
	Files_ReadBytes(&R, Files_Rider__typ, (void*)&nofcmds, 4, 2);
	Files_Set(&R, Files_Rider__typ, F, Files_Pos(&R, Files_Rider__typ) + 18);
	do {
		Files_Read(&R, Files_Rider__typ, (void*)&ch);
	} while (!(ch == 0x00));
	Files_Read(&R, Files_Rider__typ, (void*)&ch);
	if (ch != 0x82) {
		Console_String((CHAR*)"corrunpted object file", (LONGINT)23);
		return;
	}
	while (nofentry > 0) {
		Files_ReadBytes(&R, Files_Rider__typ, (void*)&e, 4, 2);
		nofentry -= 1;
	}
	Files_Read(&R, Files_Rider__typ, (void*)&ch);
	if (ch != 0x83) {
		Console_String((CHAR*)"corrunpted object file", (LONGINT)23);
		return;
	}
	Watson0_Str((CHAR*)"DEFINITION ", (LONGINT)12);
	Watson0_Str(modName, modName__len);
	if (Watson0_showAdr) {
		Watson0_Str((CHAR*)" [", (LONGINT)3);
		Watson0_Hexa(key, 0);
		Watson0_Ch(']');
	}
	Watson0_Ch(';');
	Watson0_Ln();
	while (nofcmds > 0) {
		i = 0;
		do {
			Files_Read(&R, Files_Rider__typ, (void*)&ch);
			name[__X(i, 32)] = ch;
			i += 1;
		} while (!(ch == 0x00));
		Files_ReadBytes(&R, Files_Rider__typ, (void*)&c, 4, 2);
		Watson0_Ch(0x09);
		if (Watson0_showAdr) {
			Watson0_Ch('[');
			Watson0_Hexa(c, 1);
			Watson0_Str((CHAR*)"] ", (LONGINT)3);
		}
		Watson0_Str(modName, modName__len);
		Watson0_Ch('.');
		Watson0_Str(name, 32);
		Watson0_Control(name, 32, (CHAR*)"", (LONGINT)1, (CHAR*)"", (LONGINT)1);
		Watson0_Ln();
		nofcmds -= 1;
	}
	Watson0_Str((CHAR*)"END ", (LONGINT)5);
	Watson0_Str(modName, modName__len);
	Watson0_Ch('.');
}

static void Watson0_SetOptions (void)
{
	INTEGER i;
	Watson0_expandTypes = 0;
	Watson0_showAdr = 0;
	i = 0;
	while (Watson0_options[__X(i, 12)] != 0x00) {
		if (Watson0_options[__X(i, 12)] == 'x') {
			Watson0_expandTypes = 1;
		} else if (Watson0_options[__X(i, 12)] == 'd') {
			Watson0_showAdr = 1;
		}
		i += 1;
	}
}

void Watson0_ShowDef (CHAR *mod, LONGINT mod__len, CmdlnTexts_Text T)
{
	CHAR file[64];
	OfrontOPT_Object root = NIL;
	Watson0_SetOptions();
	__COPY(mod, file, 64);
	Console_String((CHAR*)"reading ", (LONGINT)9);
	Console_String(file, 64);
	Console_Ln();
	root = Watson0_ReadSym((void*)mod, mod__len, (void*)file, 64);
	if (root != NIL) {
		__COPY(mod, file, 64);
		Watson0_showObj = 0;
		Watson0_WriteModule(mod, mod__len, root);
		Watson0_MakeText(T);
	}
}

/*----------------------------------------------------------------------------*/
void Watson0_ShowObj (CHAR *mod, LONGINT mod__len, CHAR *objName, LONGINT objName__len, CmdlnTexts_Text T)
{
	OfrontOPT_Object root = NIL, obj = NIL, pObj = NIL;
	CHAR file[64];
	Watson0_SetOptions();
	__COPY(mod, file, 64);
	Console_String((CHAR*)"reading ", (LONGINT)9);
	Console_String(file, 64);
	Console_Ln();
	root = Watson0_ReadSym((void*)mod, mod__len, (void*)file, 64);
	if (root != NIL) {
		Watson0_Find(root, objName, objName__len, &obj);
		if (obj != NIL) {
			__COPY(mod, file, 64);
			if (obj->vis < 0) {
				Watson0_FindPointer(root, obj->name, 32, &pObj);
				if (pObj != NIL) {
					obj = pObj;
					__COPY(obj->name, objName, objName__len);
				}
			}
			Watson0_showObj = 1;
			Watson0_first = 1;
			Watson0_WriteObject(obj, obj->mode);
			Watson0_MakeText(T);
		} else {
			Console_String((CHAR*)"-- \'", (LONGINT)5);
			Console_String(objName, objName__len);
			Console_String((CHAR*)"\' not found", (LONGINT)12);
			Console_Ln();
		}
	}
}

/*----------------------------------------------------------------------------*/
void Watson0_ShowCmd (CHAR *file, LONGINT file__len, CmdlnTexts_Text T)
{
	Files_File F = NIL;
	CHAR fileName[64];
	Watson0_SetOptions();
	__COPY(file, fileName, 64);
	F = Files_Old(fileName, 64);
	if (F != NIL) {
		Console_String((CHAR*)"reading ", (LONGINT)9);
		Console_String(fileName, 64);
		Console_Ln();
		Watson0_Commands(F, (void*)file, file__len);
		CmdlnTexts_Append(T, Watson0_W.buf);
	}
}

/*----------------------------------------------------------------------------*/
static void EnumPtrs(void (*P)(void*))
{
	__ENUMR(&Watson0_W, CmdlnTexts_Writer__typ, 36, 1, P);
	P(Watson0_curObj);
}


export void *Watson0__init(void)
{
	__DEFMOD;
	__IMPORT(Files__init);
	__IMPORT(Modules__init);
	__IMPORT(OfrontOPM__init);
	__IMPORT(OfrontOPS__init);
	__IMPORT(OfrontOPT__init);
	__IMPORT(OfrontOPV__init);
	__IMPORT(Console__init);
	__IMPORT(CmdlnTexts__init);
	__REGMOD("Watson0", EnumPtrs);
/* BEGIN */
	__MOVE("0123456789ABCDEF", Watson0_Hex, 17);
	CmdlnTexts_OpenWriter(&Watson0_W, CmdlnTexts_Writer__typ);
	__ENDMOD;
}
