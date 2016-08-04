/* Ofront+ 1.0 -xtspkae */
#include "SYSTEM.h"
#include "OfrontOPM.h"
#include "OfrontOPS.h"

typedef
	struct OfrontOPT_ConstDesc *OfrontOPT_Const;

typedef
	OfrontOPS_String *OfrontOPT_ConstExt;

typedef
	struct OfrontOPT_ConstDesc {
		OfrontOPT_ConstExt ext;
		LONGINT intval, intval2;
		SET setval;
		LONGREAL realval;
	} OfrontOPT_ConstDesc;

typedef
	struct OfrontOPT_ExpCtxt {
		LONGINT reffp;
		INTEGER ref;
		SHORTINT nofm;
		SHORTINT locmno[64];
	} OfrontOPT_ExpCtxt;

typedef
	struct OfrontOPT_StrDesc *OfrontOPT_Struct;

typedef
	struct OfrontOPT_ObjDesc *OfrontOPT_Object;

typedef
	struct OfrontOPT_ImpCtxt {
		LONGINT nextTag, reffp;
		INTEGER nofr, minr, nofm;
		BOOLEAN self;
		OfrontOPT_Struct ref[255];
		OfrontOPT_Object old[255];
		LONGINT pvfp[255];
		SHORTINT glbmno[64];
	} OfrontOPT_ImpCtxt;

typedef
	struct OfrontOPT_NodeDesc *OfrontOPT_Node;

typedef
	struct OfrontOPT_NodeDesc {
		OfrontOPT_Node left, right, link;
		SHORTINT class, subcl;
		BOOLEAN readonly;
		OfrontOPT_Struct typ;
		OfrontOPT_Object obj;
		OfrontOPT_Const conval;
	} OfrontOPT_NodeDesc;

typedef
	struct OfrontOPT_ObjDesc {
		OfrontOPT_Object left, right, link, scope;
		OfrontOPS_Name name;
		BOOLEAN leaf;
		SHORTINT mode, mnolev, vis, history;
		BOOLEAN used, fpdone;
		LONGINT fprint;
		OfrontOPT_Struct typ;
		OfrontOPT_Const conval;
		LONGINT adr, linkadr;
		INTEGER x;
	} OfrontOPT_ObjDesc;

typedef
	struct OfrontOPT_StrDesc {
		SHORTINT form, comp, mno, extlev;
		INTEGER ref, sysflag;
		LONGINT n, size, align, txtpos;
		BOOLEAN allocated, pbused, pvused, fpdone, idfpdone;
		LONGINT idfp, pbfp, pvfp;
		OfrontOPT_Struct BaseTyp;
		OfrontOPT_Object link, strobj;
	} OfrontOPT_StrDesc;


export void (*OfrontOPT_typSize)(OfrontOPT_Struct);
export OfrontOPT_Object OfrontOPT_topScope;
export OfrontOPT_Struct OfrontOPT_undftyp, OfrontOPT_bytetyp, OfrontOPT_booltyp, OfrontOPT_chartyp, OfrontOPT_sinttyp, OfrontOPT_inttyp, OfrontOPT_linttyp, OfrontOPT_realtyp, OfrontOPT_lrltyp, OfrontOPT_settyp, OfrontOPT_stringtyp, OfrontOPT_niltyp, OfrontOPT_notyp, OfrontOPT_sysptrtyp;
export SHORTINT OfrontOPT_nofGmod;
export OfrontOPT_Object OfrontOPT_GlbMod[64];
export OfrontOPS_Name OfrontOPT_SelfName;
export BOOLEAN OfrontOPT_SYSimported;
static OfrontOPT_Object OfrontOPT_universe, OfrontOPT_syslink;
static OfrontOPT_ImpCtxt OfrontOPT_impCtxt;
static OfrontOPT_ExpCtxt OfrontOPT_expCtxt;
static LONGINT OfrontOPT_nofhdfld;
static BOOLEAN OfrontOPT_newsf, OfrontOPT_findpc, OfrontOPT_extsf, OfrontOPT_sfpresent, OfrontOPT_symExtended, OfrontOPT_symNew;

export LONGINT *OfrontOPT_ConstDesc__typ;
export LONGINT *OfrontOPT_ObjDesc__typ;
export LONGINT *OfrontOPT_StrDesc__typ;
export LONGINT *OfrontOPT_NodeDesc__typ;
export LONGINT *OfrontOPT_ImpCtxt__typ;
export LONGINT *OfrontOPT_ExpCtxt__typ;

export void OfrontOPT_Close (void);
export void OfrontOPT_CloseScope (void);
static void OfrontOPT_EnterBoolConst (OfrontOPS_Name name, LONGINT value);
static void OfrontOPT_EnterProc (OfrontOPS_Name name, INTEGER num);
static void OfrontOPT_EnterTyp (OfrontOPS_Name name, SHORTINT form, INTEGER size, OfrontOPT_Struct *res);
export void OfrontOPT_Export (BOOLEAN *ext, BOOLEAN *new);
export void OfrontOPT_FPrintErr (OfrontOPT_Object obj, INTEGER errno);
static void OfrontOPT_FPrintName (LONGINT *fp, CHAR *name, INTEGER name__len);
export void OfrontOPT_FPrintObj (OfrontOPT_Object obj);
static void OfrontOPT_FPrintSign (LONGINT *fp, OfrontOPT_Struct result, OfrontOPT_Object par);
export void OfrontOPT_FPrintStr (OfrontOPT_Struct typ);
export void OfrontOPT_Find (OfrontOPT_Object *res);
export void OfrontOPT_FindField (OfrontOPS_Name name, OfrontOPT_Struct typ, OfrontOPT_Object *res);
export void OfrontOPT_FindImport (OfrontOPT_Object mod, OfrontOPT_Object *res);
export void OfrontOPT_IdFPrint (OfrontOPT_Struct typ);
export void OfrontOPT_Import (OfrontOPS_Name aliasName, OfrontOPS_Name name, BOOLEAN *done);
static void OfrontOPT_InConstant (LONGINT f, OfrontOPT_Const conval);
static OfrontOPT_Object OfrontOPT_InFld (void);
static void OfrontOPT_InMod (SHORTINT *mno);
static void OfrontOPT_InName (CHAR *name, INTEGER name__len);
static OfrontOPT_Object OfrontOPT_InObj (SHORTINT mno);
static void OfrontOPT_InSign (SHORTINT mno, OfrontOPT_Struct *res, OfrontOPT_Object *par);
static void OfrontOPT_InStruct (OfrontOPT_Struct *typ);
static OfrontOPT_Object OfrontOPT_InTProc (SHORTINT mno);
export void OfrontOPT_Init (OfrontOPS_Name name, SET opt);
static void OfrontOPT_InitStruct (OfrontOPT_Struct *typ, SHORTINT form);
export void OfrontOPT_Insert (OfrontOPS_Name name, OfrontOPT_Object *obj);
export void OfrontOPT_InsertImport (OfrontOPT_Object obj, OfrontOPT_Object *root, OfrontOPT_Object *old);
export OfrontOPT_Const OfrontOPT_NewConst (void);
export OfrontOPT_ConstExt OfrontOPT_NewExt (void);
export OfrontOPT_Node OfrontOPT_NewNode (SHORTINT class);
export OfrontOPT_Object OfrontOPT_NewObj (void);
export OfrontOPT_Struct OfrontOPT_NewStr (SHORTINT form, SHORTINT comp);
export void OfrontOPT_OpenScope (SHORTINT level, OfrontOPT_Object owner);
static void OfrontOPT_OutConstant (OfrontOPT_Object obj);
static void OfrontOPT_OutFlds (OfrontOPT_Object fld, LONGINT adr, BOOLEAN visible);
static void OfrontOPT_OutHdFld (OfrontOPT_Struct typ, OfrontOPT_Object fld, LONGINT adr);
static void OfrontOPT_OutMod (INTEGER mno);
static void OfrontOPT_OutName (CHAR *name, INTEGER name__len);
static void OfrontOPT_OutObj (OfrontOPT_Object obj);
static void OfrontOPT_OutSign (OfrontOPT_Struct result, OfrontOPT_Object par);
static void OfrontOPT_OutStr (OfrontOPT_Struct typ);
static void OfrontOPT_OutTProcs (OfrontOPT_Struct typ, OfrontOPT_Object obj);
static void OfrontOPT_err (INTEGER n);


/*============================================================================*/

static void OfrontOPT_err (INTEGER n)
{
	OfrontOPM_err(n);
}

OfrontOPT_Const OfrontOPT_NewConst (void)
{
	OfrontOPT_Const const_ = NIL;
	__NEW(const_, OfrontOPT_ConstDesc);
	return const_;
}

/*----------------------------------------------------------------------------*/
OfrontOPT_Object OfrontOPT_NewObj (void)
{
	OfrontOPT_Object obj = NIL;
	__NEW(obj, OfrontOPT_ObjDesc);
	return obj;
}

/*----------------------------------------------------------------------------*/
OfrontOPT_Struct OfrontOPT_NewStr (SHORTINT form, SHORTINT comp)
{
	OfrontOPT_Struct typ = NIL;
	__NEW(typ, OfrontOPT_StrDesc);
	typ->form = form;
	typ->comp = comp;
	typ->ref = 255;
	if (form != 0) {
		typ->txtpos = OfrontOPM_errpos;
	}
	typ->size = -1;
	typ->BaseTyp = OfrontOPT_undftyp;
	return typ;
}

/*----------------------------------------------------------------------------*/
OfrontOPT_Node OfrontOPT_NewNode (SHORTINT class)
{
	OfrontOPT_Node node = NIL;
	__NEW(node, OfrontOPT_NodeDesc);
	node->class = class;
	return node;
}

/*----------------------------------------------------------------------------*/
OfrontOPT_ConstExt OfrontOPT_NewExt (void)
{
	OfrontOPT_ConstExt ext = NIL;
	ext = __NEWARR(NIL, 1, 1, 1, 0, 256);
	return ext;
}

/*----------------------------------------------------------------------------*/
void OfrontOPT_OpenScope (SHORTINT level, OfrontOPT_Object owner)
{
	OfrontOPT_Object head = NIL;
	head = OfrontOPT_NewObj();
	head->mode = 12;
	head->mnolev = level;
	head->link = owner;
	if (owner != NIL) {
		owner->scope = head;
	}
	head->left = OfrontOPT_topScope;
	head->right = NIL;
	head->scope = NIL;
	OfrontOPT_topScope = head;
}

/*----------------------------------------------------------------------------*/
void OfrontOPT_CloseScope (void)
{
	OfrontOPT_topScope = OfrontOPT_topScope->left;
}

/*----------------------------------------------------------------------------*/
void OfrontOPT_Init (OfrontOPS_Name name, SET opt)
{
	OfrontOPT_topScope = OfrontOPT_universe;
	OfrontOPT_OpenScope(0, NIL);
	OfrontOPT_SYSimported = 0;
	__MOVE(name, OfrontOPT_SelfName, 32);
	__MOVE(name, OfrontOPT_topScope->name, 32);
	OfrontOPT_GlbMod[0] = OfrontOPT_topScope;
	OfrontOPT_nofGmod = 1;
	OfrontOPT_newsf = __IN(4, opt);
	OfrontOPT_findpc = __IN(8, opt);
	OfrontOPT_extsf = OfrontOPT_newsf || __IN(9, opt);
	OfrontOPT_sfpresent = 1;
}

/*----------------------------------------------------------------------------*/
void OfrontOPT_Close (void)
{
	INTEGER i;
	OfrontOPT_CloseScope();
	i = 0;
	while (i < 64) {
		OfrontOPT_GlbMod[__X(i, 64)] = NIL;
		i += 1;
	}
	i = 16;
	while (i < 255) {
		OfrontOPT_impCtxt.ref[__X(i, 255)] = NIL;
		OfrontOPT_impCtxt.old[__X(i, 255)] = NIL;
		i += 1;
	}
}

/*----------------------------------------------------------------------------*/
void OfrontOPT_FindImport (OfrontOPT_Object mod, OfrontOPT_Object *res)
{
	OfrontOPT_Object obj = NIL;
	obj = mod->scope;
	for (;;) {
		if (obj == NIL) {
			break;
		}
		if (__STRCMP(OfrontOPS_name, obj->name) < 0) {
			obj = obj->left;
		} else if (__STRCMP(OfrontOPS_name, obj->name) > 0) {
			obj = obj->right;
		} else {
			if (obj->mode == 5 && obj->vis == 0) {
				obj = NIL;
			} else {
				obj->used = 1;
			}
			break;
		}
	}
	*res = obj;
}

/*----------------------------------------------------------------------------*/
void OfrontOPT_Find (OfrontOPT_Object *res)
{
	OfrontOPT_Object obj = NIL, head = NIL;
	head = OfrontOPT_topScope;
	for (;;) {
		obj = head->right;
		for (;;) {
			if (obj == NIL) {
				break;
			}
			if (__STRCMP(OfrontOPS_name, obj->name) < 0) {
				obj = obj->left;
			} else if (__STRCMP(OfrontOPS_name, obj->name) > 0) {
				obj = obj->right;
			} else {
				break;
			}
		}
		if (obj != NIL) {
			break;
		}
		head = head->left;
		if (head == NIL) {
			break;
		}
	}
	*res = obj;
}

/*----------------------------------------------------------------------------*/
void OfrontOPT_FindField (OfrontOPS_Name name, OfrontOPT_Struct typ, OfrontOPT_Object *res)
{
	OfrontOPT_Object obj = NIL;
	while (typ != NIL) {
		obj = typ->link;
		while (obj != NIL) {
			if (__STRCMP(name, obj->name) < 0) {
				obj = obj->left;
			} else if (__STRCMP(name, obj->name) > 0) {
				obj = obj->right;
			} else {
				*res = obj;
				return;
			}
		}
		typ = typ->BaseTyp;
	}
	*res = NIL;
}

/*----------------------------------------------------------------------------*/
void OfrontOPT_Insert (OfrontOPS_Name name, OfrontOPT_Object *obj)
{
	OfrontOPT_Object ob0 = NIL, ob1 = NIL;
	BOOLEAN left;
	SHORTINT mnolev;
	ob0 = OfrontOPT_topScope;
	ob1 = ob0->right;
	left = 0;
	for (;;) {
		if (ob1 != NIL) {
			if (__STRCMP(name, ob1->name) < 0) {
				ob0 = ob1;
				ob1 = ob0->left;
				left = 1;
			} else if (__STRCMP(name, ob1->name) > 0) {
				ob0 = ob1;
				ob1 = ob0->right;
				left = 0;
			} else {
				OfrontOPT_err(1);
				ob0 = ob1;
				ob1 = ob0->right;
			}
		} else {
			ob1 = OfrontOPT_NewObj();
			ob1->leaf = 1;
			if (left) {
				ob0->left = ob1;
			} else {
				ob0->right = ob1;
			}
			ob1->left = NIL;
			ob1->right = NIL;
			__COPY(name, ob1->name, 32);
			mnolev = OfrontOPT_topScope->mnolev;
			ob1->mnolev = mnolev;
			break;
		}
	}
	*obj = ob1;
}

/*----------------------------------------------------------------------------*/
static void OfrontOPT_FPrintName (LONGINT *fp, CHAR *name, INTEGER name__len)
{
	INTEGER i;
	CHAR ch;
	i = 0;
	do {
		ch = name[__X(i, name__len)];
		OfrontOPM_FPrint(&*fp, (SHORTINT)ch);
		i += 1;
	} while (!(ch == 0x00));
}

static void OfrontOPT_FPrintSign (LONGINT *fp, OfrontOPT_Struct result, OfrontOPT_Object par)
{
	OfrontOPT_IdFPrint(result);
	OfrontOPM_FPrint(&*fp, result->idfp);
	while (par != NIL) {
		OfrontOPM_FPrint(&*fp, par->mode);
		OfrontOPT_IdFPrint(par->typ);
		OfrontOPM_FPrint(&*fp, par->typ->idfp);
		par = par->link;
	}
}

void OfrontOPT_IdFPrint (OfrontOPT_Struct typ)
{
	OfrontOPT_Struct btyp = NIL;
	OfrontOPT_Object strobj = NIL;
	LONGINT idfp;
	INTEGER f, c;
	if (!typ->idfpdone) {
		typ->idfpdone = 1;
		idfp = 0;
		f = typ->form;
		c = typ->comp;
		OfrontOPM_FPrint(&idfp, f);
		OfrontOPM_FPrint(&idfp, c);
		btyp = typ->BaseTyp;
		strobj = typ->strobj;
		if (strobj != NIL && strobj->name[0] != 0x00) {
			OfrontOPT_FPrintName(&idfp, (void*)OfrontOPT_GlbMod[__X(typ->mno, 64)]->name, 32);
			OfrontOPT_FPrintName(&idfp, (void*)strobj->name, 32);
		}
		if ((f == 13 || c == 4 && btyp != NIL) || c == 3) {
			OfrontOPT_IdFPrint(btyp);
			OfrontOPM_FPrint(&idfp, btyp->idfp);
		} else if (c == 2) {
			OfrontOPT_IdFPrint(btyp);
			OfrontOPM_FPrint(&idfp, btyp->idfp);
			OfrontOPM_FPrint(&idfp, typ->n);
		} else if (f == 14) {
			OfrontOPT_FPrintSign(&idfp, btyp, typ->link);
		}
		typ->idfp = idfp;
	}
}

/*----------------------------------------------------------------------------*/
static struct FPrintStr__11 {
	LONGINT *pbfp, *pvfp;
	struct FPrintStr__11 *lnk;
} *FPrintStr__11_s;

static void FPrintFlds__12 (OfrontOPT_Object fld, LONGINT adr, BOOLEAN visible);
static void FPrintHdFld__14 (OfrontOPT_Struct typ, OfrontOPT_Object fld, LONGINT adr);
static void FPrintTProcs__16 (OfrontOPT_Object obj);

static void FPrintHdFld__14 (OfrontOPT_Struct typ, OfrontOPT_Object fld, LONGINT adr)
{
	LONGINT i, j, n;
	OfrontOPT_Struct btyp = NIL;
	if (typ->comp == 4) {
		FPrintFlds__12(typ->link, adr, 0);
	} else if (typ->comp == 2) {
		btyp = typ->BaseTyp;
		n = typ->n;
		while (btyp->comp == 2) {
			n = btyp->n * n;
			btyp = btyp->BaseTyp;
		}
		if (btyp->form == 13 || btyp->comp == 4) {
			j = OfrontOPT_nofhdfld;
			FPrintHdFld__14(btyp, fld, adr);
			if (j != OfrontOPT_nofhdfld) {
				i = 1;
				while (i < n && OfrontOPT_nofhdfld <= 512) {
					adr += btyp->size;
					FPrintHdFld__14(btyp, fld, adr);
					i += 1;
				}
			}
		}
	} else if (typ->form == 13 || __STRCMP(fld->name, "@ptr") == 0) {
		OfrontOPM_FPrint(&*FPrintStr__11_s->pvfp, 13);
		OfrontOPM_FPrint(&*FPrintStr__11_s->pvfp, adr);
		OfrontOPT_nofhdfld += 1;
	}
}

static void FPrintFlds__12 (OfrontOPT_Object fld, LONGINT adr, BOOLEAN visible)
{
	while (fld != NIL && fld->mode == 4) {
		if (fld->vis != 0 && visible) {
			OfrontOPM_FPrint(&*FPrintStr__11_s->pbfp, fld->vis);
			OfrontOPT_FPrintName(&*FPrintStr__11_s->pbfp, (void*)fld->name, 32);
			OfrontOPM_FPrint(&*FPrintStr__11_s->pbfp, fld->adr);
			OfrontOPT_FPrintStr(fld->typ);
			OfrontOPM_FPrint(&*FPrintStr__11_s->pbfp, fld->typ->pbfp);
			OfrontOPM_FPrint(&*FPrintStr__11_s->pvfp, fld->typ->pvfp);
		} else {
			FPrintHdFld__14(fld->typ, fld, fld->adr + adr);
		}
		fld = fld->link;
	}
}

static void FPrintTProcs__16 (OfrontOPT_Object obj)
{
	if (obj != NIL) {
		FPrintTProcs__16(obj->left);
		if (obj->mode == 13) {
			if (obj->vis != 0) {
				OfrontOPM_FPrint(&*FPrintStr__11_s->pbfp, 13);
				OfrontOPM_FPrint(&*FPrintStr__11_s->pbfp, __ASHR(obj->adr, 16, LONGINT));
				OfrontOPT_FPrintSign(&*FPrintStr__11_s->pbfp, obj->typ, obj->link);
				OfrontOPT_FPrintName(&*FPrintStr__11_s->pbfp, (void*)obj->name, 32);
			}
		}
		FPrintTProcs__16(obj->right);
	}
}

void OfrontOPT_FPrintStr (OfrontOPT_Struct typ)
{
	INTEGER f, c;
	OfrontOPT_Struct btyp = NIL;
	OfrontOPT_Object strobj = NIL, bstrobj = NIL;
	LONGINT pbfp, pvfp;
	struct FPrintStr__11 _s;
	_s.pbfp = &pbfp;
	_s.pvfp = &pvfp;
	_s.lnk = FPrintStr__11_s;
	FPrintStr__11_s = &_s;
	if (!typ->fpdone) {
		OfrontOPT_IdFPrint(typ);
		pbfp = typ->idfp;
		if (typ->sysflag != 0) {
			OfrontOPM_FPrint(&pbfp, typ->sysflag);
		}
		pvfp = pbfp;
		typ->pbfp = pbfp;
		typ->pvfp = pvfp;
		typ->fpdone = 1;
		f = typ->form;
		c = typ->comp;
		btyp = typ->BaseTyp;
		if (f == 13) {
			strobj = typ->strobj;
			bstrobj = btyp->strobj;
			if (((strobj == NIL || strobj->name[0] == 0x00) || bstrobj == NIL) || bstrobj->name[0] == 0x00) {
				OfrontOPT_FPrintStr(btyp);
				OfrontOPM_FPrint(&pbfp, btyp->pbfp);
				pvfp = pbfp;
			}
		} else if (f == 14) {
		} else if (__IN(c, 0x0c)) {
			OfrontOPT_FPrintStr(btyp);
			OfrontOPM_FPrint(&pbfp, btyp->pvfp);
			pvfp = pbfp;
		} else {
			if (btyp != NIL) {
				OfrontOPT_FPrintStr(btyp);
				OfrontOPM_FPrint(&pbfp, btyp->pbfp);
				OfrontOPM_FPrint(&pvfp, btyp->pvfp);
			}
			OfrontOPM_FPrint(&pvfp, typ->size);
			OfrontOPM_FPrint(&pvfp, typ->align);
			OfrontOPM_FPrint(&pvfp, typ->n);
			OfrontOPT_nofhdfld = 0;
			FPrintFlds__12(typ->link, 0, 1);
			if (OfrontOPT_nofhdfld > 512) {
				OfrontOPM_Mark(225, typ->txtpos);
			}
			FPrintTProcs__16(typ->link);
			OfrontOPM_FPrint(&pvfp, pbfp);
			strobj = typ->strobj;
			if (strobj == NIL || strobj->name[0] == 0x00) {
				pbfp = pvfp;
			}
		}
		typ->pbfp = pbfp;
		typ->pvfp = pvfp;
	}
	FPrintStr__11_s = _s.lnk;
}

/*----------------------------------------------------------------------------*/
void OfrontOPT_FPrintObj (OfrontOPT_Object obj)
{
	LONGINT fprint;
	INTEGER f, m;
	REAL rval;
	OfrontOPT_ConstExt ext = NIL;
	if (!obj->fpdone) {
		fprint = 0;
		obj->fpdone = 1;
		OfrontOPM_FPrint(&fprint, obj->mode);
		if (obj->mode == 3) {
			f = obj->typ->form;
			OfrontOPM_FPrint(&fprint, f);
			switch (f) {
				case 1: case 2: case 3: case 4: case 5: 
				case 6: 
					OfrontOPM_FPrint(&fprint, obj->conval->intval);
					break;
				case 9: 
					OfrontOPM_FPrintSet(&fprint, obj->conval->setval);
					break;
				case 7: 
					rval = obj->conval->realval;
					OfrontOPM_FPrintReal(&fprint, rval);
					break;
				case 8: 
					OfrontOPM_FPrintLReal(&fprint, obj->conval->realval);
					break;
				case 10: 
					OfrontOPT_FPrintName(&fprint, (void*)*obj->conval->ext, 256);
					break;
				case 11: 
					break;
				default: 
					OfrontOPT_err(127);
					break;
			}
		} else if (obj->mode == 1) {
			OfrontOPM_FPrint(&fprint, obj->vis);
			OfrontOPT_FPrintStr(obj->typ);
			OfrontOPM_FPrint(&fprint, obj->typ->pbfp);
		} else if (__IN(obj->mode, 0x0480)) {
			OfrontOPT_FPrintSign(&fprint, obj->typ, obj->link);
		} else if (obj->mode == 9) {
			OfrontOPT_FPrintSign(&fprint, obj->typ, obj->link);
			ext = obj->conval->ext;
			m = (SHORTINT)(*ext)[0];
			f = 1;
			OfrontOPM_FPrint(&fprint, m);
			while (f <= m) {
				OfrontOPM_FPrint(&fprint, (SHORTINT)(*ext)[__X(f, 256)]);
				f += 1;
			}
		} else if (obj->mode == 5) {
			OfrontOPT_FPrintStr(obj->typ);
			OfrontOPM_FPrint(&fprint, obj->typ->pbfp);
		}
		obj->fprint = fprint;
	}
}

/*----------------------------------------------------------------------------*/
void OfrontOPT_FPrintErr (OfrontOPT_Object obj, INTEGER errno)
{
	INTEGER i, j;
	CHAR ch;
	if (obj->mnolev != 0) {
		__COPY(OfrontOPT_GlbMod[__X(-obj->mnolev, 64)]->name, OfrontOPM_objname, 64);
		i = 0;
		while (OfrontOPM_objname[__X(i, 64)] != 0x00) {
			i += 1;
		}
		OfrontOPM_objname[__X(i, 64)] = '.';
		j = 0;
		i += 1;
		do {
			ch = obj->name[__X(j, 32)];
			OfrontOPM_objname[__X(i, 64)] = ch;
			j += 1;
			i += 1;
		} while (!(ch == 0x00));
	} else {
		__COPY(obj->name, OfrontOPM_objname, 64);
	}
	if (errno == 249) {
		if (OfrontOPM_noerr) {
			OfrontOPT_err(errno);
		}
	} else if (errno == 253) {
		if ((!OfrontOPT_symNew && !OfrontOPT_symExtended) && !OfrontOPT_extsf) {
			OfrontOPT_err(errno);
		}
		OfrontOPT_symExtended = 1;
	} else {
		if (!OfrontOPT_symNew && !OfrontOPT_newsf) {
			OfrontOPT_err(errno);
		}
		OfrontOPT_symNew = 1;
	}
}

/*----------------------------------------------------------------------------*/
void OfrontOPT_InsertImport (OfrontOPT_Object obj, OfrontOPT_Object *root, OfrontOPT_Object *old)
{
	OfrontOPT_Object ob0 = NIL, ob1 = NIL;
	BOOLEAN left;
	if (*root == NIL) {
		*root = obj;
		*old = NIL;
	} else {
		ob0 = *root;
		ob1 = ob0->right;
		left = 0;
		if (__STRCMP(obj->name, ob0->name) < 0) {
			ob1 = ob0->left;
			left = 1;
		} else if (__STRCMP(obj->name, ob0->name) > 0) {
			ob1 = ob0->right;
			left = 0;
		} else {
			*old = ob0;
			return;
		}
		for (;;) {
			if (ob1 != NIL) {
				if (__STRCMP(obj->name, ob1->name) < 0) {
					ob0 = ob1;
					ob1 = ob1->left;
					left = 1;
				} else if (__STRCMP(obj->name, ob1->name) > 0) {
					ob0 = ob1;
					ob1 = ob1->right;
					left = 0;
				} else {
					*old = ob1;
					break;
				}
			} else {
				ob1 = obj;
				if (left) {
					ob0->left = ob1;
				} else {
					ob0->right = ob1;
				}
				ob1->left = NIL;
				ob1->right = NIL;
				*old = NIL;
				break;
			}
		}
	}
}

/*----------------------------------------------------------------------------*/
static void OfrontOPT_InName (CHAR *name, INTEGER name__len)
{
	INTEGER i;
	CHAR ch;
	i = 0;
	do {
		OfrontOPM_SymRCh(&ch);
		name[__X(i, name__len)] = ch;
		i += 1;
	} while (!(ch == 0x00));
}

static void OfrontOPT_InMod (SHORTINT *mno)
{
	OfrontOPT_Object head = NIL;
	OfrontOPS_Name name;
	LONGINT mn;
	SHORTINT i;
	mn = OfrontOPM_SymRInt();
	if (mn == 0) {
		*mno = OfrontOPT_impCtxt.glbmno[0];
	} else {
		if (mn == 16) {
			OfrontOPT_InName((void*)name, 32);
			if (__STRCMP(name, OfrontOPT_SelfName) == 0 && !OfrontOPT_impCtxt.self) {
				OfrontOPT_err(154);
			}
			i = 0;
			while (i < OfrontOPT_nofGmod && __STRCMP(name, OfrontOPT_GlbMod[__X(i, 64)]->name) != 0) {
				i += 1;
			}
			if (i < OfrontOPT_nofGmod) {
				*mno = i;
			} else {
				head = OfrontOPT_NewObj();
				head->mode = 12;
				__COPY(name, head->name, 32);
				*mno = OfrontOPT_nofGmod;
				head->mnolev = -*mno;
				if (OfrontOPT_nofGmod < 64) {
					OfrontOPT_GlbMod[__X(*mno, 64)] = head;
					OfrontOPT_nofGmod += 1;
				} else {
					OfrontOPT_err(227);
				}
			}
			OfrontOPT_impCtxt.glbmno[__X(OfrontOPT_impCtxt.nofm, 64)] = *mno;
			OfrontOPT_impCtxt.nofm += 1;
		} else {
			*mno = OfrontOPT_impCtxt.glbmno[__X(-mn, 64)];
		}
	}
}

static void OfrontOPT_InConstant (LONGINT f, OfrontOPT_Const conval)
{
	CHAR ch;
	INTEGER i;
	OfrontOPT_ConstExt ext = NIL;
	REAL rval;
	switch (f) {
		case 3: case 2: 
			OfrontOPM_SymRCh(&ch);
			conval->intval = (SHORTINT)ch;
			break;
		case 1: case 4: case 5: case 6: 
			conval->intval = OfrontOPM_SymRInt();
			break;
		case 9: 
			OfrontOPM_SymRSet(&conval->setval);
			break;
		case 7: 
			OfrontOPM_SymRReal(&rval);
			conval->realval = rval;
			conval->intval = -1;
			break;
		case 8: 
			OfrontOPM_SymRLReal(&conval->realval);
			conval->intval = -1;
			break;
		case 10: 
			ext = OfrontOPT_NewExt();
			conval->ext = ext;
			i = 0;
			do {
				OfrontOPM_SymRCh(&ch);
				(*ext)[__X(i, 256)] = ch;
				i += 1;
			} while (!(ch == 0x00));
			conval->intval2 = i;
			conval->intval = -1;
			break;
		case 11: 
			conval->intval = 0;
			break;
		default: __CASECHK;
	}
}

static void OfrontOPT_InSign (SHORTINT mno, OfrontOPT_Struct *res, OfrontOPT_Object *par)
{
	OfrontOPT_Object last = NIL, new = NIL;
	LONGINT tag;
	OfrontOPT_InStruct(&*res);
	tag = OfrontOPM_SymRInt();
	last = NIL;
	while (tag != 18) {
		new = OfrontOPT_NewObj();
		new->mnolev = -mno;
		if (last == NIL) {
			*par = new;
		} else {
			last->link = new;
		}
		if (tag == 23) {
			new->mode = 1;
		} else {
			new->mode = 2;
		}
		OfrontOPT_InStruct(&new->typ);
		new->adr = OfrontOPM_SymRInt();
		OfrontOPT_InName((void*)new->name, 32);
		last = new;
		tag = OfrontOPM_SymRInt();
	}
}

static OfrontOPT_Object OfrontOPT_InFld (void)
{
	LONGINT tag;
	OfrontOPT_Object obj = NIL;
	tag = OfrontOPT_impCtxt.nextTag;
	obj = OfrontOPT_NewObj();
	if (tag <= 26) {
		obj->mode = 4;
		if (tag == 26) {
			obj->vis = 2;
		} else {
			obj->vis = 1;
		}
		OfrontOPT_InStruct(&obj->typ);
		OfrontOPT_InName((void*)obj->name, 32);
		obj->adr = OfrontOPM_SymRInt();
	} else {
		obj->mode = 4;
		if (tag == 27) {
			__MOVE("@ptr", obj->name, 5);
		} else {
			__MOVE("@proc", obj->name, 6);
		}
		obj->typ = OfrontOPT_undftyp;
		obj->vis = 0;
		obj->adr = OfrontOPM_SymRInt();
	}
	return obj;
}

static OfrontOPT_Object OfrontOPT_InTProc (SHORTINT mno)
{
	LONGINT tag;
	OfrontOPT_Object obj = NIL;
	tag = OfrontOPT_impCtxt.nextTag;
	obj = OfrontOPT_NewObj();
	obj->mnolev = -mno;
	if (tag == 29) {
		obj->mode = 13;
		obj->conval = OfrontOPT_NewConst();
		obj->conval->intval = -1;
		OfrontOPT_InSign(mno, &obj->typ, &obj->link);
		obj->vis = 1;
		OfrontOPT_InName((void*)obj->name, 32);
		obj->adr = __ASHL(OfrontOPM_SymRInt(), 16, LONGINT);
	} else {
		obj->mode = 13;
		__MOVE("@tproc", obj->name, 7);
		obj->link = OfrontOPT_NewObj();
		obj->typ = OfrontOPT_undftyp;
		obj->vis = 0;
		obj->adr = __ASHL(OfrontOPM_SymRInt(), 16, LONGINT);
	}
	return obj;
}

static void OfrontOPT_InStruct (OfrontOPT_Struct *typ)
{
	SHORTINT mno;
	INTEGER ref;
	LONGINT tag;
	OfrontOPS_Name name;
	OfrontOPT_Struct t = NIL;
	OfrontOPT_Object obj = NIL, last = NIL, fld = NIL, old = NIL, dummy = NIL;
	tag = OfrontOPM_SymRInt();
	if (tag != 34) {
		*typ = OfrontOPT_impCtxt.ref[__X(-tag, 255)];
	} else {
		ref = OfrontOPT_impCtxt.nofr;
		OfrontOPT_impCtxt.nofr += 1;
		if (ref < OfrontOPT_impCtxt.minr) {
			OfrontOPT_impCtxt.minr = ref;
		}
		OfrontOPT_InMod(&mno);
		OfrontOPT_InName((void*)name, 32);
		obj = OfrontOPT_NewObj();
		if (name[0] == 0x00) {
			if (OfrontOPT_impCtxt.self) {
				old = NIL;
			} else {
				__MOVE("@", obj->name, 2);
				OfrontOPT_InsertImport(obj, &OfrontOPT_GlbMod[__X(mno, 64)]->right, &old);
				obj->name[0] = 0x00;
			}
			*typ = OfrontOPT_NewStr(0, 1);
		} else {
			__MOVE(name, obj->name, 32);
			OfrontOPT_InsertImport(obj, &OfrontOPT_GlbMod[__X(mno, 64)]->right, &old);
			if (old != NIL) {
				OfrontOPT_FPrintObj(old);
				OfrontOPT_impCtxt.pvfp[__X(ref, 255)] = old->typ->pvfp;
				if (OfrontOPT_impCtxt.self) {
					*typ = OfrontOPT_NewStr(0, 1);
				} else {
					*typ = old->typ;
					(*typ)->link = NIL;
					(*typ)->sysflag = 0;
					(*typ)->fpdone = 0;
					(*typ)->idfpdone = 0;
				}
			} else {
				*typ = OfrontOPT_NewStr(0, 1);
			}
		}
		OfrontOPT_impCtxt.ref[__X(ref, 255)] = *typ;
		OfrontOPT_impCtxt.old[__X(ref, 255)] = old;
		(*typ)->ref = ref + 255;
		(*typ)->mno = mno;
		(*typ)->allocated = 1;
		(*typ)->strobj = obj;
		obj->mode = 5;
		obj->typ = *typ;
		obj->mnolev = -mno;
		obj->vis = 0;
		tag = OfrontOPM_SymRInt();
		if (tag == 35) {
			(*typ)->sysflag = (INTEGER)OfrontOPM_SymRInt();
			tag = OfrontOPM_SymRInt();
		}
		switch (tag) {
			case 36: 
				(*typ)->form = 13;
				(*typ)->size = OfrontOPM_PointerSize;
				(*typ)->n = 0;
				OfrontOPT_InStruct(&(*typ)->BaseTyp);
				break;
			case 37: 
				(*typ)->form = 15;
				(*typ)->comp = 2;
				OfrontOPT_InStruct(&(*typ)->BaseTyp);
				(*typ)->n = OfrontOPM_SymRInt();
				(*OfrontOPT_typSize)(*typ);
				break;
			case 38: 
				(*typ)->form = 15;
				(*typ)->comp = 3;
				OfrontOPT_InStruct(&(*typ)->BaseTyp);
				if ((*typ)->BaseTyp->comp == 3) {
					(*typ)->n = (*typ)->BaseTyp->n + 1;
				} else {
					(*typ)->n = 0;
				}
				(*OfrontOPT_typSize)(*typ);
				break;
			case 39: 
				(*typ)->form = 15;
				(*typ)->comp = 4;
				OfrontOPT_InStruct(&(*typ)->BaseTyp);
				if ((*typ)->BaseTyp == OfrontOPT_notyp) {
					(*typ)->BaseTyp = NIL;
				}
				(*typ)->extlev = 0;
				t = (*typ)->BaseTyp;
				while (t != NIL) {
					(*typ)->extlev += 1;
					t = t->BaseTyp;
				}
				(*typ)->size = OfrontOPM_SymRInt();
				(*typ)->align = OfrontOPM_SymRInt();
				(*typ)->n = OfrontOPM_SymRInt();
				OfrontOPT_impCtxt.nextTag = OfrontOPM_SymRInt();
				last = NIL;
				while (OfrontOPT_impCtxt.nextTag >= 25 && OfrontOPT_impCtxt.nextTag <= 28) {
					fld = OfrontOPT_InFld();
					fld->mnolev = -mno;
					if (last != NIL) {
						last->link = fld;
					}
					last = fld;
					OfrontOPT_InsertImport(fld, &(*typ)->link, &dummy);
					OfrontOPT_impCtxt.nextTag = OfrontOPM_SymRInt();
				}
				while (OfrontOPT_impCtxt.nextTag != 18) {
					fld = OfrontOPT_InTProc(mno);
					OfrontOPT_InsertImport(fld, &(*typ)->link, &dummy);
					OfrontOPT_impCtxt.nextTag = OfrontOPM_SymRInt();
				}
				break;
			case 40: 
				(*typ)->form = 14;
				(*typ)->size = OfrontOPM_ProcSize;
				OfrontOPT_InSign(mno, &(*typ)->BaseTyp, &(*typ)->link);
				break;
			default: __CASECHK;
		}
		if (ref == OfrontOPT_impCtxt.minr) {
			while (ref < OfrontOPT_impCtxt.nofr) {
				t = OfrontOPT_impCtxt.ref[__X(ref, 255)];
				OfrontOPT_FPrintStr(t);
				obj = t->strobj;
				if (obj->name[0] != 0x00) {
					OfrontOPT_FPrintObj(obj);
				}
				old = OfrontOPT_impCtxt.old[__X(ref, 255)];
				if (old != NIL) {
					t->strobj = old;
					if (OfrontOPT_impCtxt.self) {
						if (old->mnolev < 0) {
							if (old->history != 5) {
								if (old->fprint != obj->fprint) {
									old->history = 2;
								} else if (OfrontOPT_impCtxt.pvfp[__X(ref, 255)] != t->pvfp) {
									old->history = 3;
								}
							}
						} else if (old->fprint != obj->fprint) {
							old->history = 2;
						} else if (OfrontOPT_impCtxt.pvfp[__X(ref, 255)] != t->pvfp) {
							old->history = 3;
						} else if (old->vis == 0) {
							old->history = 1;
						} else {
							old->history = 0;
						}
					} else {
						if (OfrontOPT_impCtxt.pvfp[__X(ref, 255)] != t->pvfp) {
							old->history = 5;
						}
						if (old->fprint != obj->fprint) {
							OfrontOPT_FPrintErr(old, 249);
						}
					}
				} else if (OfrontOPT_impCtxt.self) {
					obj->history = 4;
				} else {
					obj->history = 1;
				}
				ref += 1;
			}
			OfrontOPT_impCtxt.minr = 255;
		}
	}
}

static OfrontOPT_Object OfrontOPT_InObj (SHORTINT mno)
{
	INTEGER i, s;
	CHAR ch;
	OfrontOPT_Object obj = NIL, old = NIL;
	OfrontOPT_Struct typ = NIL;
	LONGINT tag;
	OfrontOPT_ConstExt ext = NIL;
	tag = OfrontOPT_impCtxt.nextTag;
	if (tag == 19) {
		OfrontOPT_InStruct(&typ);
		obj = typ->strobj;
		if (!OfrontOPT_impCtxt.self) {
			obj->vis = 1;
		}
	} else {
		obj = OfrontOPT_NewObj();
		obj->mnolev = -mno;
		obj->vis = 1;
		if (tag <= 13) {
			obj->mode = 3;
			obj->typ = OfrontOPT_impCtxt.ref[__X(tag, 255)];
			obj->conval = OfrontOPT_NewConst();
			OfrontOPT_InConstant(tag, obj->conval);
		} else if (tag >= 31) {
			obj->conval = OfrontOPT_NewConst();
			obj->conval->intval = -1;
			OfrontOPT_InSign(mno, &obj->typ, &obj->link);
			switch (tag) {
				case 31: 
					obj->mode = 7;
					break;
				case 32: 
					obj->mode = 10;
					break;
				case 33: 
					obj->mode = 9;
					ext = OfrontOPT_NewExt();
					obj->conval->ext = ext;
					s = (INTEGER)OfrontOPM_SymRInt();
					(*ext)[0] = (CHAR)s;
					i = 1;
					while (i <= s) {
						OfrontOPM_SymRCh(&(*ext)[__X(i, 256)]);
						i += 1;
					}
					break;
				default: __CASECHK;
			}
		} else if (tag == 20) {
			obj->mode = 5;
			OfrontOPT_InStruct(&obj->typ);
		} else {
			obj->mode = 1;
			if (tag == 22) {
				obj->vis = 2;
			}
			OfrontOPT_InStruct(&obj->typ);
		}
		OfrontOPT_InName((void*)obj->name, 32);
	}
	OfrontOPT_FPrintObj(obj);
	if (obj->mode == 1 && (obj->typ->strobj == NIL || obj->typ->strobj->name[0] == 0x00)) {
		OfrontOPM_FPrint(&OfrontOPT_impCtxt.reffp, obj->typ->ref - 255);
	}
	if (tag != 19) {
		OfrontOPT_InsertImport(obj, &OfrontOPT_GlbMod[__X(mno, 64)]->right, &old);
		if (OfrontOPT_impCtxt.self) {
			if (old != NIL) {
				if (old->vis == 0) {
					old->history = 4;
				} else {
					OfrontOPT_FPrintObj(old);
					if (obj->fprint != old->fprint) {
						old->history = 2;
					} else if (obj->typ->pvfp != old->typ->pvfp) {
						old->history = 3;
					} else {
						old->history = 1;
					}
				}
			} else {
				obj->history = 4;
			}
		}
	} else {
		if (OfrontOPT_impCtxt.self) {
			if (obj->vis == 0) {
				obj->history = 4;
			} else if (obj->history == 0) {
				obj->history = 1;
			}
		}
	}
	return obj;
}

void OfrontOPT_Import (OfrontOPS_Name aliasName, OfrontOPS_Name name, BOOLEAN *done)
{
	OfrontOPT_Object obj = NIL;
	SHORTINT mno;
	OfrontOPS_Name aliasName__copy;
	__DUPARR(aliasName);
	if (__STRCMP(name, "SYSTEM") == 0) {
		OfrontOPT_SYSimported = 1;
		OfrontOPT_Insert(aliasName, &obj);
		obj->mode = 11;
		obj->mnolev = 0;
		obj->scope = OfrontOPT_syslink;
		obj->typ = OfrontOPT_notyp;
	} else {
		OfrontOPT_impCtxt.nofr = 16;
		OfrontOPT_impCtxt.minr = 255;
		OfrontOPT_impCtxt.nofm = 0;
		OfrontOPT_impCtxt.self = __STRCMP(aliasName, "@self") == 0;
		OfrontOPT_impCtxt.reffp = 0;
		OfrontOPM_OldSym((void*)name, 32, &*done);
		if (*done) {
			OfrontOPT_InMod(&mno);
			OfrontOPT_impCtxt.nextTag = OfrontOPM_SymRInt();
			while (!OfrontOPM_eofSF()) {
				obj = OfrontOPT_InObj(mno);
				OfrontOPT_impCtxt.nextTag = OfrontOPM_SymRInt();
			}
			OfrontOPT_Insert(aliasName, &obj);
			obj->mode = 11;
			obj->scope = OfrontOPT_GlbMod[__X(mno, 64)]->right;
			OfrontOPT_GlbMod[__X(mno, 64)]->link = obj;
			obj->mnolev = -mno;
			obj->typ = OfrontOPT_notyp;
			OfrontOPM_CloseOldSym();
		} else if (OfrontOPT_impCtxt.self) {
			OfrontOPT_newsf = 1;
			OfrontOPT_extsf = 1;
			OfrontOPT_sfpresent = 0;
		} else {
			OfrontOPT_err(152);
		}
	}
}

/*----------------------------------------------------------------------------*/
static void OfrontOPT_OutName (CHAR *name, INTEGER name__len)
{
	INTEGER i;
	CHAR ch;
	i = 0;
	do {
		ch = name[__X(i, name__len)];
		OfrontOPM_SymWCh(ch);
		i += 1;
	} while (!(ch == 0x00));
}

static void OfrontOPT_OutMod (INTEGER mno)
{
	if (OfrontOPT_expCtxt.locmno[__X(mno, 64)] < 0) {
		OfrontOPM_SymWInt(16);
		OfrontOPT_expCtxt.locmno[__X(mno, 64)] = OfrontOPT_expCtxt.nofm;
		OfrontOPT_expCtxt.nofm += 1;
		OfrontOPT_OutName((void*)OfrontOPT_GlbMod[__X(mno, 64)]->name, 32);
	} else {
		OfrontOPM_SymWInt(-OfrontOPT_expCtxt.locmno[__X(mno, 64)]);
	}
}

static void OfrontOPT_OutHdFld (OfrontOPT_Struct typ, OfrontOPT_Object fld, LONGINT adr)
{
	LONGINT i, j, n;
	OfrontOPT_Struct btyp = NIL;
	if (typ->comp == 4) {
		OfrontOPT_OutFlds(typ->link, adr, 0);
	} else if (typ->comp == 2) {
		btyp = typ->BaseTyp;
		n = typ->n;
		while (btyp->comp == 2) {
			n = btyp->n * n;
			btyp = btyp->BaseTyp;
		}
		if (btyp->form == 13 || btyp->comp == 4) {
			j = OfrontOPT_nofhdfld;
			OfrontOPT_OutHdFld(btyp, fld, adr);
			if (j != OfrontOPT_nofhdfld) {
				i = 1;
				while (i < n && OfrontOPT_nofhdfld <= 512) {
					adr += btyp->size;
					OfrontOPT_OutHdFld(btyp, fld, adr);
					i += 1;
				}
			}
		}
	} else if (typ->form == 13 || __STRCMP(fld->name, "@ptr") == 0) {
		OfrontOPM_SymWInt(27);
		OfrontOPM_SymWInt(adr);
		OfrontOPT_nofhdfld += 1;
	}
}

static void OfrontOPT_OutFlds (OfrontOPT_Object fld, LONGINT adr, BOOLEAN visible)
{
	while (fld != NIL && fld->mode == 4) {
		if (fld->vis != 0 && visible) {
			if (fld->vis == 2) {
				OfrontOPM_SymWInt(26);
			} else {
				OfrontOPM_SymWInt(25);
			}
			OfrontOPT_OutStr(fld->typ);
			OfrontOPT_OutName((void*)fld->name, 32);
			OfrontOPM_SymWInt(fld->adr);
		} else {
			OfrontOPT_OutHdFld(fld->typ, fld, fld->adr + adr);
		}
		fld = fld->link;
	}
}

static void OfrontOPT_OutSign (OfrontOPT_Struct result, OfrontOPT_Object par)
{
	OfrontOPT_OutStr(result);
	while (par != NIL) {
		if (par->mode == 1) {
			OfrontOPM_SymWInt(23);
		} else {
			OfrontOPM_SymWInt(24);
		}
		OfrontOPT_OutStr(par->typ);
		OfrontOPM_SymWInt(par->adr);
		OfrontOPT_OutName((void*)par->name, 32);
		par = par->link;
	}
	OfrontOPM_SymWInt(18);
}

static void OfrontOPT_OutTProcs (OfrontOPT_Struct typ, OfrontOPT_Object obj)
{
	if (obj != NIL) {
		OfrontOPT_OutTProcs(typ, obj->left);
		if (obj->mode == 13) {
			if ((typ->BaseTyp != NIL && __ASHR(obj->adr, 16, LONGINT) < typ->BaseTyp->n) && obj->vis == 0) {
				OfrontOPM_Mark(109, typ->txtpos);
			}
			if (obj->vis != 0) {
				if (obj->vis != 0) {
					OfrontOPM_SymWInt(29);
					OfrontOPT_OutSign(obj->typ, obj->link);
					OfrontOPT_OutName((void*)obj->name, 32);
					OfrontOPM_SymWInt(__ASHR(obj->adr, 16, LONGINT));
				} else {
					OfrontOPM_SymWInt(30);
					OfrontOPM_SymWInt(__ASHR(obj->adr, 16, LONGINT));
				}
			}
		}
		OfrontOPT_OutTProcs(typ, obj->right);
	}
}

static void OfrontOPT_OutStr (OfrontOPT_Struct typ)
{
	OfrontOPT_Object strobj = NIL;
	if (typ->ref < OfrontOPT_expCtxt.ref) {
		OfrontOPM_SymWInt(-typ->ref);
	} else {
		OfrontOPM_SymWInt(34);
		typ->ref = OfrontOPT_expCtxt.ref;
		OfrontOPT_expCtxt.ref += 1;
		if (OfrontOPT_expCtxt.ref >= 255) {
			OfrontOPT_err(228);
		}
		OfrontOPT_OutMod(typ->mno);
		strobj = typ->strobj;
		if (strobj != NIL && strobj->name[0] != 0x00) {
			OfrontOPT_OutName((void*)strobj->name, 32);
			switch (strobj->history) {
				case 2: 
					OfrontOPT_FPrintErr(strobj, 252);
					break;
				case 3: 
					OfrontOPT_FPrintErr(strobj, 251);
					break;
				case 5: 
					OfrontOPT_FPrintErr(strobj, 249);
					break;
				default: 
					break;
			}
		} else {
			OfrontOPM_SymWCh(0x00);
		}
		if (typ->sysflag != 0) {
			OfrontOPM_SymWInt(35);
			OfrontOPM_SymWInt(typ->sysflag);
		}
		switch (typ->form) {
			case 13: 
				OfrontOPM_SymWInt(36);
				OfrontOPT_OutStr(typ->BaseTyp);
				break;
			case 14: 
				OfrontOPM_SymWInt(40);
				OfrontOPT_OutSign(typ->BaseTyp, typ->link);
				break;
			case 15: 
				switch (typ->comp) {
					case 2: 
						OfrontOPM_SymWInt(37);
						OfrontOPT_OutStr(typ->BaseTyp);
						OfrontOPM_SymWInt(typ->n);
						break;
					case 3: 
						OfrontOPM_SymWInt(38);
						OfrontOPT_OutStr(typ->BaseTyp);
						break;
					case 4: 
						OfrontOPM_SymWInt(39);
						if (typ->BaseTyp == NIL) {
							OfrontOPT_OutStr(OfrontOPT_notyp);
						} else {
							OfrontOPT_OutStr(typ->BaseTyp);
						}
						OfrontOPM_SymWInt(typ->size);
						OfrontOPM_SymWInt(typ->align);
						OfrontOPM_SymWInt(typ->n);
						OfrontOPT_nofhdfld = 0;
						OfrontOPT_OutFlds(typ->link, 0, 1);
						if (OfrontOPT_nofhdfld > 512) {
							OfrontOPM_Mark(223, typ->txtpos);
						}
						OfrontOPT_OutTProcs(typ, typ->link);
						OfrontOPM_SymWInt(18);
						break;
					default: __CASECHK;
				}
				break;
			default: __CASECHK;
		}
	}
}

static void OfrontOPT_OutConstant (OfrontOPT_Object obj)
{
	INTEGER f;
	REAL rval;
	f = obj->typ->form;
	OfrontOPM_SymWInt(f);
	switch (f) {
		case 2: case 3: 
			OfrontOPM_SymWCh((CHAR)obj->conval->intval);
			break;
		case 1: case 4: case 5: case 6: 
			OfrontOPM_SymWInt(obj->conval->intval);
			break;
		case 9: 
			OfrontOPM_SymWSet(obj->conval->setval);
			break;
		case 7: 
			rval = obj->conval->realval;
			OfrontOPM_SymWReal(rval);
			break;
		case 8: 
			OfrontOPM_SymWLReal(obj->conval->realval);
			break;
		case 10: 
			OfrontOPT_OutName((void*)*obj->conval->ext, 256);
			break;
		case 11: 
			break;
		default: 
			OfrontOPT_err(127);
			break;
	}
}

static void OfrontOPT_OutObj (OfrontOPT_Object obj)
{
	INTEGER i, j;
	OfrontOPT_ConstExt ext = NIL;
	if (obj != NIL) {
		OfrontOPT_OutObj(obj->left);
		if (__IN(obj->mode, 0x06ea)) {
			if (obj->history == 4) {
				OfrontOPT_FPrintErr(obj, 250);
			} else if (obj->vis != 0) {
				switch (obj->history) {
					case 0: 
						OfrontOPT_FPrintErr(obj, 253);
						break;
					case 1: 
						break;
					case 2: 
						OfrontOPT_FPrintErr(obj, 252);
						break;
					case 3: 
						OfrontOPT_FPrintErr(obj, 251);
						break;
					default: __CASECHK;
				}
				switch (obj->mode) {
					case 3: 
						OfrontOPT_OutConstant(obj);
						OfrontOPT_OutName((void*)obj->name, 32);
						break;
					case 5: 
						if (obj->typ->strobj == obj) {
							OfrontOPM_SymWInt(19);
							OfrontOPT_OutStr(obj->typ);
						} else {
							OfrontOPM_SymWInt(20);
							OfrontOPT_OutStr(obj->typ);
							OfrontOPT_OutName((void*)obj->name, 32);
						}
						break;
					case 1: 
						if (obj->vis == 2) {
							OfrontOPM_SymWInt(22);
						} else {
							OfrontOPM_SymWInt(21);
						}
						OfrontOPT_OutStr(obj->typ);
						OfrontOPT_OutName((void*)obj->name, 32);
						if (obj->typ->strobj == NIL || obj->typ->strobj->name[0] == 0x00) {
							OfrontOPM_FPrint(&OfrontOPT_expCtxt.reffp, obj->typ->ref);
						}
						break;
					case 7: 
						OfrontOPM_SymWInt(31);
						OfrontOPT_OutSign(obj->typ, obj->link);
						OfrontOPT_OutName((void*)obj->name, 32);
						break;
					case 10: 
						OfrontOPM_SymWInt(32);
						OfrontOPT_OutSign(obj->typ, obj->link);
						OfrontOPT_OutName((void*)obj->name, 32);
						break;
					case 9: 
						OfrontOPM_SymWInt(33);
						OfrontOPT_OutSign(obj->typ, obj->link);
						ext = obj->conval->ext;
						j = (SHORTINT)(*ext)[0];
						i = 1;
						OfrontOPM_SymWInt(j);
						while (i <= j) {
							OfrontOPM_SymWCh((*ext)[__X(i, 256)]);
							i += 1;
						}
						OfrontOPT_OutName((void*)obj->name, 32);
						break;
					default: __CASECHK;
				}
			}
		}
		OfrontOPT_OutObj(obj->right);
	}
}

void OfrontOPT_Export (BOOLEAN *ext, BOOLEAN *new)
{
	INTEGER i;
	SHORTINT nofmod;
	BOOLEAN done;
	OfrontOPT_symExtended = 0;
	OfrontOPT_symNew = 0;
	nofmod = OfrontOPT_nofGmod;
	OfrontOPT_Import((CHAR*)"@self", OfrontOPT_SelfName, &done);
	OfrontOPT_nofGmod = nofmod;
	if (OfrontOPM_noerr) {
		OfrontOPM_NewSym((void*)OfrontOPT_SelfName, 32);
		if (OfrontOPM_noerr) {
			OfrontOPM_SymWInt(16);
			OfrontOPT_OutName((void*)OfrontOPT_SelfName, 32);
			OfrontOPT_expCtxt.reffp = 0;
			OfrontOPT_expCtxt.ref = 16;
			OfrontOPT_expCtxt.nofm = 1;
			OfrontOPT_expCtxt.locmno[0] = 0;
			i = 1;
			while (i < 64) {
				OfrontOPT_expCtxt.locmno[__X(i, 64)] = -1;
				i += 1;
			}
			OfrontOPT_OutObj(OfrontOPT_topScope->right);
			*ext = OfrontOPT_sfpresent && OfrontOPT_symExtended;
			*new = !OfrontOPT_sfpresent || OfrontOPT_symNew;
			if ((OfrontOPM_noerr && OfrontOPT_sfpresent) && OfrontOPT_impCtxt.reffp != OfrontOPT_expCtxt.reffp) {
				*new = 1;
				if (!OfrontOPT_extsf) {
					OfrontOPT_err(155);
				}
			}
			OfrontOPT_newsf = 0;
			OfrontOPT_symNew = 0;
			if (!OfrontOPM_noerr || OfrontOPT_findpc) {
				OfrontOPM_DeleteNewSym();
			}
		}
	}
}

/*----------------------------------------------------------------------------*/
static void OfrontOPT_InitStruct (OfrontOPT_Struct *typ, SHORTINT form)
{
	*typ = OfrontOPT_NewStr(form, 1);
	(*typ)->ref = form;
	(*typ)->size = OfrontOPM_ByteSize;
	(*typ)->allocated = 1;
	(*typ)->strobj = OfrontOPT_NewObj();
	(*typ)->pbfp = form;
	(*typ)->pvfp = form;
	(*typ)->fpdone = 1;
	(*typ)->idfp = form;
	(*typ)->idfpdone = 1;
}

static void OfrontOPT_EnterBoolConst (OfrontOPS_Name name, LONGINT value)
{
	OfrontOPT_Object obj = NIL;
	OfrontOPS_Name name__copy;
	__DUPARR(name);
	OfrontOPT_Insert(name, &obj);
	obj->conval = OfrontOPT_NewConst();
	obj->mode = 3;
	obj->typ = OfrontOPT_booltyp;
	obj->conval->intval = value;
}

static void OfrontOPT_EnterTyp (OfrontOPS_Name name, SHORTINT form, INTEGER size, OfrontOPT_Struct *res)
{
	OfrontOPT_Object obj = NIL;
	OfrontOPT_Struct typ = NIL;
	OfrontOPS_Name name__copy;
	__DUPARR(name);
	OfrontOPT_Insert(name, &obj);
	typ = OfrontOPT_NewStr(form, 1);
	obj->mode = 5;
	obj->typ = typ;
	obj->vis = 1;
	typ->strobj = obj;
	typ->size = size;
	typ->ref = form;
	typ->allocated = 1;
	typ->pbfp = form;
	typ->pvfp = form;
	typ->fpdone = 1;
	typ->idfp = form;
	typ->idfpdone = 1;
	*res = typ;
}

static void OfrontOPT_EnterProc (OfrontOPS_Name name, INTEGER num)
{
	OfrontOPT_Object obj = NIL;
	OfrontOPS_Name name__copy;
	__DUPARR(name);
	OfrontOPT_Insert(name, &obj);
	obj->mode = 8;
	obj->typ = OfrontOPT_notyp;
	obj->adr = num;
}

static void EnumPtrs(void (*P)(void*))
{
	P(OfrontOPT_topScope);
	P(OfrontOPT_undftyp);
	P(OfrontOPT_bytetyp);
	P(OfrontOPT_booltyp);
	P(OfrontOPT_chartyp);
	P(OfrontOPT_sinttyp);
	P(OfrontOPT_inttyp);
	P(OfrontOPT_linttyp);
	P(OfrontOPT_realtyp);
	P(OfrontOPT_lrltyp);
	P(OfrontOPT_settyp);
	P(OfrontOPT_stringtyp);
	P(OfrontOPT_niltyp);
	P(OfrontOPT_notyp);
	P(OfrontOPT_sysptrtyp);
	__ENUMP(OfrontOPT_GlbMod, 64, P);
	P(OfrontOPT_universe);
	P(OfrontOPT_syslink);
	__ENUMR(&OfrontOPT_impCtxt, OfrontOPT_ImpCtxt__typ, 3212, 1, P);
}

__TDESC(OfrontOPT_ConstDesc__desc, 1, 1) = {__TDFLDS("ConstDesc", 24), {0, -8}};
__TDESC(OfrontOPT_ObjDesc__desc, 1, 6) = {__TDFLDS("ObjDesc", 84), {0, 4, 8, 12, 64, 68, -28}};
__TDESC(OfrontOPT_StrDesc__desc, 1, 3) = {__TDFLDS("StrDesc", 64), {52, 56, 60, -16}};
__TDESC(OfrontOPT_NodeDesc__desc, 1, 6) = {__TDFLDS("NodeDesc", 32), {0, 4, 8, 20, 24, 28, -28}};
__TDESC(OfrontOPT_ImpCtxt__desc, 1, 510) = {__TDFLDS("ImpCtxt", 3212), {24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 
	88, 92, 96, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 144, 148, 
	152, 156, 160, 164, 168, 172, 176, 180, 184, 188, 192, 196, 200, 204, 208, 212, 
	216, 220, 224, 228, 232, 236, 240, 244, 248, 252, 256, 260, 264, 268, 272, 276, 
	280, 284, 288, 292, 296, 300, 304, 308, 312, 316, 320, 324, 328, 332, 336, 340, 
	344, 348, 352, 356, 360, 364, 368, 372, 376, 380, 384, 388, 392, 396, 400, 404, 
	408, 412, 416, 420, 424, 428, 432, 436, 440, 444, 448, 452, 456, 460, 464, 468, 
	472, 476, 480, 484, 488, 492, 496, 500, 504, 508, 512, 516, 520, 524, 528, 532, 
	536, 540, 544, 548, 552, 556, 560, 564, 568, 572, 576, 580, 584, 588, 592, 596, 
	600, 604, 608, 612, 616, 620, 624, 628, 632, 636, 640, 644, 648, 652, 656, 660, 
	664, 668, 672, 676, 680, 684, 688, 692, 696, 700, 704, 708, 712, 716, 720, 724, 
	728, 732, 736, 740, 744, 748, 752, 756, 760, 764, 768, 772, 776, 780, 784, 788, 
	792, 796, 800, 804, 808, 812, 816, 820, 824, 828, 832, 836, 840, 844, 848, 852, 
	856, 860, 864, 868, 872, 876, 880, 884, 888, 892, 896, 900, 904, 908, 912, 916, 
	920, 924, 928, 932, 936, 940, 944, 948, 952, 956, 960, 964, 968, 972, 976, 980, 
	984, 988, 992, 996, 1000, 1004, 1008, 1012, 1016, 1020, 1024, 1028, 1032, 1036, 1040, 1044, 
	1048, 1052, 1056, 1060, 1064, 1068, 1072, 1076, 1080, 1084, 1088, 1092, 1096, 1100, 1104, 1108, 
	1112, 1116, 1120, 1124, 1128, 1132, 1136, 1140, 1144, 1148, 1152, 1156, 1160, 1164, 1168, 1172, 
	1176, 1180, 1184, 1188, 1192, 1196, 1200, 1204, 1208, 1212, 1216, 1220, 1224, 1228, 1232, 1236, 
	1240, 1244, 1248, 1252, 1256, 1260, 1264, 1268, 1272, 1276, 1280, 1284, 1288, 1292, 1296, 1300, 
	1304, 1308, 1312, 1316, 1320, 1324, 1328, 1332, 1336, 1340, 1344, 1348, 1352, 1356, 1360, 1364, 
	1368, 1372, 1376, 1380, 1384, 1388, 1392, 1396, 1400, 1404, 1408, 1412, 1416, 1420, 1424, 1428, 
	1432, 1436, 1440, 1444, 1448, 1452, 1456, 1460, 1464, 1468, 1472, 1476, 1480, 1484, 1488, 1492, 
	1496, 1500, 1504, 1508, 1512, 1516, 1520, 1524, 1528, 1532, 1536, 1540, 1544, 1548, 1552, 1556, 
	1560, 1564, 1568, 1572, 1576, 1580, 1584, 1588, 1592, 1596, 1600, 1604, 1608, 1612, 1616, 1620, 
	1624, 1628, 1632, 1636, 1640, 1644, 1648, 1652, 1656, 1660, 1664, 1668, 1672, 1676, 1680, 1684, 
	1688, 1692, 1696, 1700, 1704, 1708, 1712, 1716, 1720, 1724, 1728, 1732, 1736, 1740, 1744, 1748, 
	1752, 1756, 1760, 1764, 1768, 1772, 1776, 1780, 1784, 1788, 1792, 1796, 1800, 1804, 1808, 1812, 
	1816, 1820, 1824, 1828, 1832, 1836, 1840, 1844, 1848, 1852, 1856, 1860, 1864, 1868, 1872, 1876, 
	1880, 1884, 1888, 1892, 1896, 1900, 1904, 1908, 1912, 1916, 1920, 1924, 1928, 1932, 1936, 1940, 
	1944, 1948, 1952, 1956, 1960, 1964, 1968, 1972, 1976, 1980, 1984, 1988, 1992, 1996, 2000, 2004, 
	2008, 2012, 2016, 2020, 2024, 2028, 2032, 2036, 2040, 2044, 2048, 2052, 2056, 2060, -2044}};
__TDESC(OfrontOPT_ExpCtxt__desc, 1, 0) = {__TDFLDS("ExpCtxt", 140), {-4}};

export void *OfrontOPT__init(void)
{
	__DEFMOD;
	__IMPORT(OfrontOPM__init);
	__IMPORT(OfrontOPS__init);
	__REGMOD("OfrontOPT", EnumPtrs);
	__REGCMD("Close", OfrontOPT_Close);
	__REGCMD("CloseScope", OfrontOPT_CloseScope);
	__INITYP(OfrontOPT_ConstDesc, OfrontOPT_ConstDesc, 0);
	__INITYP(OfrontOPT_ObjDesc, OfrontOPT_ObjDesc, 0);
	__INITYP(OfrontOPT_StrDesc, OfrontOPT_StrDesc, 0);
	__INITYP(OfrontOPT_NodeDesc, OfrontOPT_NodeDesc, 0);
	__INITYP(OfrontOPT_ImpCtxt, OfrontOPT_ImpCtxt, 0);
	__INITYP(OfrontOPT_ExpCtxt, OfrontOPT_ExpCtxt, 0);
/* BEGIN */
	OfrontOPT_topScope = NIL;
	OfrontOPT_OpenScope(0, NIL);
	OfrontOPM_errpos = 0;
	OfrontOPT_InitStruct(&OfrontOPT_undftyp, 0);
	OfrontOPT_InitStruct(&OfrontOPT_notyp, 12);
	OfrontOPT_InitStruct(&OfrontOPT_stringtyp, 10);
	OfrontOPT_InitStruct(&OfrontOPT_niltyp, 11);
	OfrontOPT_undftyp->BaseTyp = OfrontOPT_undftyp;
	OfrontOPT_EnterTyp((CHAR*)"PTR", 13, OfrontOPM_PointerSize, &OfrontOPT_sysptrtyp);
	OfrontOPT_EnterProc((CHAR*)"ADR", 20);
	OfrontOPT_EnterProc((CHAR*)"CC", 21);
	OfrontOPT_EnterProc((CHAR*)"LSH", 22);
	OfrontOPT_EnterProc((CHAR*)"ROT", 23);
	OfrontOPT_EnterProc((CHAR*)"GET", 24);
	OfrontOPT_EnterProc((CHAR*)"PUT", 25);
	OfrontOPT_EnterProc((CHAR*)"GETREG", 26);
	OfrontOPT_EnterProc((CHAR*)"PUTREG", 27);
	OfrontOPT_EnterProc((CHAR*)"BIT", 28);
	OfrontOPT_EnterProc((CHAR*)"VAL", 29);
	OfrontOPT_EnterProc((CHAR*)"NEW", 30);
	OfrontOPT_EnterProc((CHAR*)"MOVE", 31);
	OfrontOPT_syslink = OfrontOPT_topScope->right;
	OfrontOPT_universe = OfrontOPT_topScope;
	OfrontOPT_topScope->right = NIL;
	OfrontOPT_EnterTyp((CHAR*)"BYTE", 1, OfrontOPM_ByteSize, &OfrontOPT_bytetyp);
	OfrontOPT_EnterTyp((CHAR*)"CHAR", 3, OfrontOPM_CharSize, &OfrontOPT_chartyp);
	OfrontOPT_EnterTyp((CHAR*)"SET", 9, OfrontOPM_SetSize, &OfrontOPT_settyp);
	OfrontOPT_EnterTyp((CHAR*)"REAL", 7, OfrontOPM_RealSize, &OfrontOPT_realtyp);
	OfrontOPT_EnterTyp((CHAR*)"INTEGER", 5, OfrontOPM_IntSize, &OfrontOPT_inttyp);
	OfrontOPT_EnterTyp((CHAR*)"LONGINT", 6, OfrontOPM_LIntSize, &OfrontOPT_linttyp);
	OfrontOPT_EnterTyp((CHAR*)"LONGREAL", 8, OfrontOPM_LRealSize, &OfrontOPT_lrltyp);
	OfrontOPT_EnterTyp((CHAR*)"SHORTINT", 4, OfrontOPM_SIntSize, &OfrontOPT_sinttyp);
	OfrontOPT_EnterTyp((CHAR*)"BOOLEAN", 2, OfrontOPM_BoolSize, &OfrontOPT_booltyp);
	OfrontOPT_EnterBoolConst((CHAR*)"FALSE", 0);
	OfrontOPT_EnterBoolConst((CHAR*)"TRUE", 1);
	OfrontOPT_EnterProc((CHAR*)"HALT", 0);
	OfrontOPT_EnterProc((CHAR*)"NEW", 1);
	OfrontOPT_EnterProc((CHAR*)"ABS", 2);
	OfrontOPT_EnterProc((CHAR*)"CAP", 3);
	OfrontOPT_EnterProc((CHAR*)"ORD", 4);
	OfrontOPT_EnterProc((CHAR*)"ENTIER", 5);
	OfrontOPT_EnterProc((CHAR*)"ODD", 6);
	OfrontOPT_EnterProc((CHAR*)"MIN", 7);
	OfrontOPT_EnterProc((CHAR*)"MAX", 8);
	OfrontOPT_EnterProc((CHAR*)"CHR", 9);
	OfrontOPT_EnterProc((CHAR*)"SHORT", 10);
	OfrontOPT_EnterProc((CHAR*)"LONG", 11);
	OfrontOPT_EnterProc((CHAR*)"SIZE", 12);
	OfrontOPT_EnterProc((CHAR*)"INC", 13);
	OfrontOPT_EnterProc((CHAR*)"DEC", 14);
	OfrontOPT_EnterProc((CHAR*)"INCL", 15);
	OfrontOPT_EnterProc((CHAR*)"EXCL", 16);
	OfrontOPT_EnterProc((CHAR*)"LEN", 17);
	OfrontOPT_EnterProc((CHAR*)"COPY", 18);
	OfrontOPT_EnterProc((CHAR*)"ASH", 19);
	OfrontOPT_EnterProc((CHAR*)"ASSERT", 32);
	OfrontOPT_impCtxt.ref[0] = OfrontOPT_undftyp;
	OfrontOPT_impCtxt.ref[1] = OfrontOPT_bytetyp;
	OfrontOPT_impCtxt.ref[2] = OfrontOPT_booltyp;
	OfrontOPT_impCtxt.ref[3] = OfrontOPT_chartyp;
	OfrontOPT_impCtxt.ref[4] = OfrontOPT_sinttyp;
	OfrontOPT_impCtxt.ref[5] = OfrontOPT_inttyp;
	OfrontOPT_impCtxt.ref[6] = OfrontOPT_linttyp;
	OfrontOPT_impCtxt.ref[7] = OfrontOPT_realtyp;
	OfrontOPT_impCtxt.ref[8] = OfrontOPT_lrltyp;
	OfrontOPT_impCtxt.ref[9] = OfrontOPT_settyp;
	OfrontOPT_impCtxt.ref[10] = OfrontOPT_stringtyp;
	OfrontOPT_impCtxt.ref[11] = OfrontOPT_niltyp;
	OfrontOPT_impCtxt.ref[12] = OfrontOPT_notyp;
	OfrontOPT_impCtxt.ref[13] = OfrontOPT_sysptrtyp;
	__ENDMOD;
}
