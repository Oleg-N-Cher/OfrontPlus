/*  Ofront 1.2 -xtspkae */
#include "SYSTEM.h"
#include "OfrontOPC.h"
#include "OfrontOPM.h"
#include "OfrontOPS.h"
#include "OfrontOPT.h"

typedef
	struct OfrontOPV_ExitInfo {
		INTEGER level, label;
	} OfrontOPV_ExitInfo;


static BOOLEAN OfrontOPV_assert, OfrontOPV_inxchk, OfrontOPV_mainprog, OfrontOPV_ansi;
static INTEGER OfrontOPV_stamp;
static LONGINT OfrontOPV_recno;
static OfrontOPV_ExitInfo OfrontOPV_exit;
static INTEGER OfrontOPV_nofExitLabels;
static BOOLEAN OfrontOPV_naturalAlignment;

export LONGINT *OfrontOPV_ExitInfo__typ;

static void OfrontOPV_ActualPar (OfrontOPT_Node n, OfrontOPT_Object fp);
export void OfrontOPV_AdrAndSize (OfrontOPT_Object topScope);
static void OfrontOPV_CaseStat (OfrontOPT_Node n, OfrontOPT_Object outerProc);
static void OfrontOPV_Convert (OfrontOPT_Node n, INTEGER form, INTEGER prec);
static void OfrontOPV_DefineTDescs (OfrontOPT_Node n);
static void OfrontOPV_Entier (OfrontOPT_Node n, INTEGER prec);
static void OfrontOPV_GetTProcNum (OfrontOPT_Object obj);
static void OfrontOPV_IfStat (OfrontOPT_Node n, BOOLEAN withtrap, OfrontOPT_Object outerProc);
static BOOLEAN OfrontOPV_ImplicitReturn (OfrontOPT_Node n);
static void OfrontOPV_Index (OfrontOPT_Node n, OfrontOPT_Node d, INTEGER prec, INTEGER dim);
export void OfrontOPV_Init (void);
static void OfrontOPV_InitTDescs (OfrontOPT_Node n);
static void OfrontOPV_Len (OfrontOPT_Node n, LONGINT dim);
export void OfrontOPV_Module (OfrontOPT_Node prog);
static LONGINT OfrontOPV_NaturalAlignment (LONGINT size, LONGINT max);
static void OfrontOPV_NewArr (OfrontOPT_Node d, OfrontOPT_Node x);
static INTEGER OfrontOPV_Precedence (INTEGER class, INTEGER subclass, INTEGER form, INTEGER comp);
static BOOLEAN OfrontOPV_SideEffects (OfrontOPT_Node n);
static void OfrontOPV_Stamp (OfrontOPS_Name s);
static OfrontOPT_Object OfrontOPV_SuperProc (OfrontOPT_Node n);
static void OfrontOPV_Traverse (OfrontOPT_Object obj, OfrontOPT_Object outerScope, BOOLEAN exported);
static void OfrontOPV_TraverseRecord (OfrontOPT_Struct typ);
export void OfrontOPV_TypSize (OfrontOPT_Struct typ);
static void OfrontOPV_TypeOf (OfrontOPT_Node n);
static void OfrontOPV_design (OfrontOPT_Node n, INTEGER prec);
static void OfrontOPV_expr (OfrontOPT_Node n, INTEGER prec);
static void OfrontOPV_stat (OfrontOPT_Node n, OfrontOPT_Object outerProc);


/*============================================================================*/

static LONGINT OfrontOPV_NaturalAlignment (LONGINT size, LONGINT max)
{
	LONGINT i;
	if (size >= max) {
		return max;
	} else {
		i = 1;
		while (i < size) {
			i += i;
		}
		return i;
	}
	__RETCHK;
}

void OfrontOPV_TypSize (OfrontOPT_Struct typ)
{
	INTEGER f, c;
	LONGINT offset, size, base, fbase, off0;
	OfrontOPT_Object fld = NIL;
	OfrontOPT_Struct btyp = NIL;
	if (typ == OfrontOPT_undftyp) {
		OfrontOPM_err(58);
	} else if (typ->size == -1) {
		f = typ->form;
		c = typ->comp;
		if (c == 4) {
			btyp = typ->BaseTyp;
			if (btyp == NIL) {
				offset = 0;
				base = OfrontOPM_RecAlign;
			} else {
				OfrontOPV_TypSize(btyp);
				offset = btyp->size - (LONGINT)__ASHR(btyp->sysflag, 8, INTEGER);
				base = btyp->align;
			}
			fld = typ->link;
			while (fld != NIL && fld->mode == 4) {
				btyp = fld->typ;
				OfrontOPV_TypSize(btyp);
				size = btyp->size;
				fbase = OfrontOPC_Base(btyp);
				OfrontOPC_Align(&offset, fbase);
				fld->adr = offset;
				offset += size;
				if (fbase > base) {
					base = fbase;
				}
				fld = fld->link;
			}
			off0 = offset;
			if (offset == 0) {
				offset = 1;
			}
			if (OfrontOPM_RecSize == 0) {
				base = OfrontOPV_NaturalAlignment(offset, OfrontOPM_RecAlign);
			}
			OfrontOPC_Align(&offset, base);
			if (typ->strobj == NIL && __MASK(typ->align, -65536) == 0) {
				OfrontOPV_recno += 1;
				base += __ASHL(OfrontOPV_recno, 16, LONGINT);
			}
			typ->size = offset;
			typ->align = base;
			typ->sysflag = __MASK(typ->sysflag, -256) + (INTEGER)__ASHL(offset - off0, 8, LONGINT);
		} else if (c == 2) {
			OfrontOPV_TypSize(typ->BaseTyp);
			typ->size = typ->n * typ->BaseTyp->size;
		} else if (f == 13) {
			typ->size = OfrontOPM_PointerSize;
			if (typ->BaseTyp == OfrontOPT_undftyp) {
				OfrontOPM_Mark(128, typ->n);
			} else {
				OfrontOPV_TypSize(typ->BaseTyp);
			}
		} else if (f == 14) {
			typ->size = OfrontOPM_ProcSize;
		} else if (c == 3) {
			btyp = typ->BaseTyp;
			OfrontOPV_TypSize(btyp);
			if (btyp->comp == 3) {
				typ->size = btyp->size + 4;
			} else {
				typ->size = 8;
			}
		}
	}
}

/*----------------------------------------------------------------------------*/
void OfrontOPV_Init (void)
{
	OfrontOPV_stamp = 0;
	OfrontOPV_recno = 0;
	OfrontOPV_nofExitLabels = 0;
	OfrontOPV_assert = __IN(7, OfrontOPM_opt);
	OfrontOPV_inxchk = __IN(0, OfrontOPM_opt);
	OfrontOPV_mainprog = __IN(10, OfrontOPM_opt);
	OfrontOPV_ansi = __IN(6, OfrontOPM_opt);
}

/*----------------------------------------------------------------------------*/
static void OfrontOPV_GetTProcNum (OfrontOPT_Object obj)
{
	LONGINT oldPos;
	OfrontOPT_Struct typ = NIL;
	OfrontOPT_Object redef = NIL;
	oldPos = OfrontOPM_errpos;
	OfrontOPM_errpos = obj->scope->adr;
	typ = obj->link->typ;
	if (typ->form == 13) {
		typ = typ->BaseTyp;
	}
	OfrontOPT_FindField(obj->name, typ->BaseTyp, &redef);
	if (redef != NIL) {
		obj->adr = __ASHL(__ASHR(redef->adr, 16, LONGINT), 16, LONGINT);
		if (!__IN(2, obj->conval->setval)) {
			OfrontOPM_err(119);
		}
	} else {
		obj->adr += __ASHL(typ->n, 16, LONGINT);
		typ->n += 1;
	}
	OfrontOPM_errpos = oldPos;
}

static void OfrontOPV_TraverseRecord (OfrontOPT_Struct typ)
{
	if (!typ->allocated) {
		if (typ->BaseTyp != NIL) {
			OfrontOPV_TraverseRecord(typ->BaseTyp);
			typ->n = typ->BaseTyp->n;
		}
		typ->allocated = 1;
		OfrontOPV_Traverse(typ->link, typ->strobj, 0);
	}
}

static void OfrontOPV_Stamp (OfrontOPS_Name s)
{
	INTEGER i, j, k;
	CHAR n[10];
	OfrontOPV_stamp += 1;
	i = 0;
	j = OfrontOPV_stamp;
	while (s[__X(i, 32)] != 0x00) {
		i += 1;
	}
	if (i > 25) {
		i = 25;
	}
	s[__X(i, 32)] = '_';
	s[__X(i + 1, 32)] = '_';
	i += 2;
	k = 0;
	do {
		n[__X(k, 10)] = (CHAR)((INTEGER)__MOD(j, 10) + 48);
		j = __DIV(j, 10);
		k += 1;
	} while (!(j == 0));
	do {
		k -= 1;
		s[__X(i, 32)] = n[__X(k, 10)];
		i += 1;
	} while (!(k == 0));
	s[__X(i, 32)] = 0x00;
}

static void OfrontOPV_Traverse (OfrontOPT_Object obj, OfrontOPT_Object outerScope, BOOLEAN exported)
{
	INTEGER mode;
	OfrontOPT_Object scope = NIL;
	OfrontOPT_Struct typ = NIL;
	if (obj != NIL) {
		OfrontOPV_Traverse(obj->left, outerScope, exported);
		if (obj->name[0] == '@') {
			obj->name[0] = '_';
			OfrontOPV_Stamp(obj->name);
		}
		obj->linkadr = 0;
		mode = obj->mode;
		if (mode == 5 && (obj->vis != 0) == exported) {
			typ = obj->typ;
			OfrontOPV_TypSize(obj->typ);
			if (typ->form == 13) {
				typ = typ->BaseTyp;
			}
			if (typ->comp == 4) {
				OfrontOPV_TraverseRecord(typ);
			}
		} else if (mode == 13) {
			OfrontOPV_GetTProcNum(obj);
		} else if (mode == 1) {
			OfrontOPV_TypSize(obj->typ);
		}
		if (!exported) {
			if (__IN(mode, 0x60) && obj->mnolev > 0) {
				OfrontOPV_Stamp(obj->name);
			}
			if (__IN(mode, 0x26)) {
				obj->scope = outerScope;
			} else if (__IN(mode, 0x26c0)) {
				if (obj->conval->setval == 0x0) {
					OfrontOPM_err(129);
				}
				scope = obj->scope;
				scope->leaf = 1;
				__MOVE(obj->name, scope->name, 32);
				OfrontOPV_Stamp(scope->name);
				if (mode == 9) {
					obj->adr = 1;
				}
				if (scope->mnolev > 1) {
					outerScope->leaf = 0;
				}
				OfrontOPV_Traverse(obj->scope->right, obj->scope, 0);
			}
		}
		OfrontOPV_Traverse(obj->right, outerScope, exported);
	}
}

void OfrontOPV_AdrAndSize (OfrontOPT_Object topScope)
{
	OfrontOPM_errpos = topScope->adr;
	topScope->leaf = 1;
	OfrontOPV_Traverse(topScope->right, topScope, 1);
	OfrontOPV_Traverse(topScope->right, topScope, 0);
	OfrontOPT_chartyp->strobj->linkadr = 2;
	OfrontOPT_settyp->strobj->linkadr = 2;
	OfrontOPT_realtyp->strobj->linkadr = 2;
	OfrontOPT_inttyp->strobj->linkadr = 2;
	OfrontOPT_linttyp->strobj->linkadr = 2;
	OfrontOPT_lrltyp->strobj->linkadr = 2;
	OfrontOPT_sinttyp->strobj->linkadr = 2;
	OfrontOPT_booltyp->strobj->linkadr = 2;
	OfrontOPT_bytetyp->strobj->linkadr = 2;
	OfrontOPT_sysptrtyp->strobj->linkadr = 2;
}

/*----------------------------------------------------------------------------*/
static INTEGER OfrontOPV_Precedence (INTEGER class, INTEGER subclass, INTEGER form, INTEGER comp)
{
	switch (class) {
		case 7: case 0: case 2: case 4: case 9: 
		case 13: 
			return 10;
			break;
		case 5: 
			if (__IN(3, OfrontOPM_opt)) {
				return 10;
			} else {
				return 9;
			}
			break;
		case 1: 
			if (__IN(comp, 0x0c)) {
				return 10;
			} else {
				return 9;
			}
			break;
		case 3: 
			return 9;
			break;
		case 11: 
			switch (subclass) {
				case 33: case 7: case 24: case 29: case 20: 
					return 9;
					break;
				case 16: case 21: case 22: case 23: case 25: 
					return 10;
					break;
				default: __CASECHK;
			}
			break;
		case 12: 
			switch (subclass) {
				case 1: 
					if (form == 9) {
						return 4;
					} else {
						return 8;
					}
					break;
				case 2: 
					if (form == 9) {
						return 3;
					} else {
						return 8;
					}
					break;
				case 3: case 4: 
					return 10;
					break;
				case 6: 
					if (form == 9) {
						return 2;
					} else {
						return 7;
					}
					break;
				case 7: 
					if (form == 9) {
						return 4;
					} else {
						return 7;
					}
					break;
				case 11: case 12: case 13: case 14: 
					return 6;
					break;
				case 9: case 10: 
					return 5;
					break;
				case 5: 
					return 1;
					break;
				case 8: 
					return 0;
					break;
				case 19: case 15: case 17: case 18: case 26: 
				case 27: case 28: 
					return 10;
					break;
				default: __CASECHK;
			}
			break;
		case 10: 
			return 10;
			break;
		case 8: case 6: 
			return 12;
			break;
		default: __CASECHK;
	}
	__RETCHK;
}

static void OfrontOPV_Len (OfrontOPT_Node n, LONGINT dim)
{
	while (n->class == 4 && n->typ->comp == 3) {
		dim += 1;
		n = n->left;
	}
	if (n->class == 3 && n->typ->comp == 3) {
		OfrontOPV_design(n->left, 10);
		OfrontOPM_WriteString((CHAR*)"->len[", (LONGINT)7);
		OfrontOPM_WriteInt(dim);
		OfrontOPM_Write(']');
	} else {
		OfrontOPC_Len(n->obj, n->typ, dim);
	}
}

static BOOLEAN OfrontOPV_SideEffects (OfrontOPT_Node n)
{
	if (n != NIL) {
		return (n->class == 13 || OfrontOPV_SideEffects(n->left)) || OfrontOPV_SideEffects(n->right);
	} else {
		return 0;
	}
	__RETCHK;
}

static void OfrontOPV_Entier (OfrontOPT_Node n, INTEGER prec)
{
	if (__IN(n->typ->form, 0x0180)) {
		OfrontOPM_WriteString((CHAR*)"__ENTIER(", (LONGINT)10);
		OfrontOPV_expr(n, -1);
		OfrontOPM_Write(')');
	} else {
		OfrontOPV_expr(n, prec);
	}
}

static void OfrontOPV_Convert (OfrontOPT_Node n, INTEGER form, INTEGER prec)
{
	INTEGER from;
	from = n->typ->form;
	if (form == 9) {
		OfrontOPM_WriteString((CHAR*)"__SETOF(", (LONGINT)9);
		OfrontOPV_Entier(n, -1);
		OfrontOPM_Write(')');
	} else if (form == 6) {
		if (from < 6) {
			OfrontOPM_WriteString((CHAR*)"(LONGINT)", (LONGINT)10);
		}
		OfrontOPV_Entier(n, 9);
	} else if (form == 5) {
		if (from < 5) {
			OfrontOPM_WriteString((CHAR*)"(INTEGER)", (LONGINT)10);
			OfrontOPV_expr(n, 9);
		} else {
			if (__IN(2, OfrontOPM_opt)) {
				OfrontOPM_WriteString((CHAR*)"__SHORT", (LONGINT)8);
				if (OfrontOPV_SideEffects(n)) {
					OfrontOPM_Write('F');
				}
				OfrontOPM_Write('(');
				OfrontOPV_Entier(n, -1);
				OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
				OfrontOPM_WriteInt(OfrontOPM_MaxInt + 1);
				OfrontOPM_Write(')');
			} else {
				OfrontOPM_WriteString((CHAR*)"(INTEGER)", (LONGINT)10);
				OfrontOPV_Entier(n, 9);
			}
		}
	} else if (form == 4) {
		if (__IN(2, OfrontOPM_opt)) {
			OfrontOPM_WriteString((CHAR*)"__SHORT", (LONGINT)8);
			if (OfrontOPV_SideEffects(n)) {
				OfrontOPM_Write('F');
			}
			OfrontOPM_Write('(');
			OfrontOPV_Entier(n, -1);
			OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
			OfrontOPM_WriteInt(OfrontOPM_MaxSInt + 1);
			OfrontOPM_Write(')');
		} else {
			OfrontOPM_WriteString((CHAR*)"(INTEGER)", (LONGINT)10);
			OfrontOPV_Entier(n, 9);
		}
	} else if (form == 3) {
		if (__IN(2, OfrontOPM_opt)) {
			OfrontOPM_WriteString((CHAR*)"__CHR", (LONGINT)6);
			if (OfrontOPV_SideEffects(n)) {
				OfrontOPM_Write('F');
			}
			OfrontOPM_Write('(');
			OfrontOPV_Entier(n, -1);
			OfrontOPM_Write(')');
		} else {
			OfrontOPM_WriteString((CHAR*)"(CHAR)", (LONGINT)7);
			OfrontOPV_Entier(n, 9);
		}
	} else {
		OfrontOPV_expr(n, prec);
	}
}

static void OfrontOPV_TypeOf (OfrontOPT_Node n)
{
	if (n->typ->form == 13) {
		OfrontOPM_WriteString((CHAR*)"__TYPEOF(", (LONGINT)10);
		OfrontOPV_expr(n, -1);
		OfrontOPM_Write(')');
	} else if (__IN(n->class, 0x15)) {
		OfrontOPC_Andent(n->typ);
		OfrontOPM_WriteString((CHAR*)"__typ", (LONGINT)6);
	} else if (n->class == 3) {
		OfrontOPM_WriteString((CHAR*)"__TYPEOF(", (LONGINT)10);
		OfrontOPV_expr(n->left, -1);
		OfrontOPM_Write(')');
	} else if (n->class == 5) {
		OfrontOPV_TypeOf(n->left);
	} else if (n->class == 11 && n->subcl == 29) {
		OfrontOPC_TypeOf(n->left->obj);
	} else {
		OfrontOPC_TypeOf(n->obj);
	}
}

static void OfrontOPV_Index (OfrontOPT_Node n, OfrontOPT_Node d, INTEGER prec, INTEGER dim)
{
	if (!OfrontOPV_inxchk || n->right->class == 7 && (n->right->conval->intval == 0 || n->left->typ->comp != 3)) {
		OfrontOPV_expr(n->right, prec);
	} else {
		if (OfrontOPV_SideEffects(n->right)) {
			OfrontOPM_WriteString((CHAR*)"__XF(", (LONGINT)6);
		} else {
			OfrontOPM_WriteString((CHAR*)"__X(", (LONGINT)5);
		}
		OfrontOPV_expr(n->right, -1);
		OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
		OfrontOPV_Len(d, dim);
		OfrontOPM_Write(')');
	}
}

static void OfrontOPV_design (OfrontOPT_Node n, INTEGER prec)
{
	OfrontOPT_Object obj = NIL;
	OfrontOPT_Struct typ = NIL;
	INTEGER class, designPrec, comp;
	OfrontOPT_Node d = NIL, x = NIL;
	INTEGER dims, i, _for__26;
	comp = n->typ->comp;
	obj = n->obj;
	class = n->class;
	designPrec = OfrontOPV_Precedence(class, n->subcl, n->typ->form, comp);
	if (((class == 0 && obj->mnolev > 0) && (INTEGER)obj->mnolev != OfrontOPM_level) && prec == 10) {
		designPrec = 9;
	}
	if (prec > designPrec) {
		OfrontOPM_Write('(');
	}
	if (prec == 11) {
		OfrontOPM_Write('*');
	}
	switch (class) {
		case 9: 
			OfrontOPC_Ident(n->obj);
			break;
		case 0: 
			OfrontOPC_CompleteIdent(n->obj);
			break;
		case 1: 
			if (!__IN(comp, 0x0c)) {
				OfrontOPM_Write('*');
			}
			OfrontOPC_CompleteIdent(n->obj);
			break;
		case 2: 
			if (n->left->class == 3) {
				OfrontOPV_design(n->left->left, designPrec);
				OfrontOPM_WriteString((CHAR*)"->", (LONGINT)3);
			} else {
				OfrontOPV_design(n->left, designPrec);
				OfrontOPM_Write('.');
			}
			OfrontOPC_Ident(n->obj);
			break;
		case 3: 
			if (n->typ->comp == 3) {
				OfrontOPV_design(n->left, 10);
				OfrontOPM_WriteString((CHAR*)"->data", (LONGINT)7);
			} else {
				OfrontOPM_Write('*');
				OfrontOPV_design(n->left, designPrec);
			}
			break;
		case 4: 
			d = n->left;
			if (d->typ->comp == 3) {
				dims = 0;
				while (d->class == 4) {
					d = d->left;
					dims += 1;
				}
				if (n->typ->comp == 3) {
					OfrontOPM_Write('&');
				}
				OfrontOPV_design(d, designPrec);
				OfrontOPM_Write('[');
				if (n->typ->comp == 3) {
					OfrontOPM_Write('(');
				}
				i = dims;
				x = n;
				while (x != d) {
					if (x->left != d) {
						OfrontOPV_Index(x, d, 7, i);
						OfrontOPM_WriteString((CHAR*)" + ", (LONGINT)4);
						OfrontOPV_Len(d, i);
						OfrontOPM_WriteString((CHAR*)" * (", (LONGINT)5);
						i -= 1;
					} else {
						OfrontOPV_Index(x, d, -1, i);
					}
					x = x->left;
				}
				_for__26 = dims;
				i = 1;
				while (i <= _for__26) {
					OfrontOPM_Write(')');
					i += 1;
				}
				if (n->typ->comp == 3) {
					OfrontOPM_Write(')');
					while ((LONGINT)i < __ASHR(d->typ->size - 4, 2, LONGINT)) {
						OfrontOPM_WriteString((CHAR*)" * ", (LONGINT)4);
						OfrontOPV_Len(d, i);
						i += 1;
					}
				}
				OfrontOPM_Write(']');
			} else {
				OfrontOPV_design(n->left, designPrec);
				OfrontOPM_Write('[');
				OfrontOPV_Index(n, n->left, -1, 0);
				OfrontOPM_Write(']');
			}
			break;
		case 5: 
			typ = n->typ;
			obj = n->left->obj;
			if (__IN(3, OfrontOPM_opt)) {
				if (typ->comp == 4) {
					OfrontOPM_WriteString((CHAR*)"__GUARDR(", (LONGINT)10);
					if ((INTEGER)obj->mnolev != OfrontOPM_level) {
						OfrontOPM_WriteStringVar((void*)obj->scope->name, 32);
						OfrontOPM_WriteString((CHAR*)"__curr->", (LONGINT)9);
						OfrontOPC_Ident(obj);
					} else {
						OfrontOPC_Ident(obj);
					}
				} else {
					if (typ->BaseTyp->strobj == NIL) {
						OfrontOPM_WriteString((CHAR*)"__GUARDA(", (LONGINT)10);
					} else {
						OfrontOPM_WriteString((CHAR*)"__GUARDP(", (LONGINT)10);
					}
					OfrontOPV_expr(n->left, -1);
					typ = typ->BaseTyp;
				}
				OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
				OfrontOPC_Andent(typ);
				OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
				OfrontOPM_WriteInt(typ->extlev);
				OfrontOPM_Write(')');
			} else {
				if (typ->comp == 4) {
					OfrontOPM_WriteString((CHAR*)"*(", (LONGINT)3);
					OfrontOPC_Ident(typ->strobj);
					OfrontOPM_WriteString((CHAR*)"*)", (LONGINT)3);
					OfrontOPC_CompleteIdent(obj);
				} else {
					OfrontOPM_Write('(');
					OfrontOPC_Ident(typ->strobj);
					OfrontOPM_Write(')');
					OfrontOPV_expr(n->left, designPrec);
				}
			}
			break;
		case 6: 
			if (__IN(3, OfrontOPM_opt)) {
				if (n->left->class == 1) {
					OfrontOPM_WriteString((CHAR*)"__GUARDEQR(", (LONGINT)12);
					OfrontOPC_CompleteIdent(n->left->obj);
					OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
					OfrontOPV_TypeOf(n->left);
				} else {
					OfrontOPM_WriteString((CHAR*)"__GUARDEQP(", (LONGINT)12);
					OfrontOPV_expr(n->left->left, -1);
				}
				OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
				OfrontOPC_Ident(n->left->typ->strobj);
				OfrontOPM_Write(')');
			} else {
				OfrontOPV_expr(n->left, -1);
			}
			break;
		case 11: 
			if (n->subcl == 29) {
				OfrontOPV_design(n->left, prec);
			}
			break;
		default: __CASECHK;
	}
	if (prec > designPrec) {
		OfrontOPM_Write(')');
	}
}

static void OfrontOPV_ActualPar (OfrontOPT_Node n, OfrontOPT_Object fp)
{
	OfrontOPT_Struct typ = NIL, aptyp = NIL;
	INTEGER comp, form, mode, prec, dim;
	OfrontOPM_Write('(');
	while (n != NIL) {
		typ = fp->typ;
		comp = typ->comp;
		form = typ->form;
		mode = fp->mode;
		prec = -1;
		if ((mode == 2 && n->class == 11) && n->subcl == 29) {
			OfrontOPM_Write('(');
			OfrontOPC_Ident(n->typ->strobj);
			OfrontOPM_WriteString((CHAR*)"*)", (LONGINT)3);
			prec = 10;
		}
		if (!__IN(n->typ->comp, 0x0c)) {
			if (mode == 2) {
				if (OfrontOPV_ansi && typ != n->typ) {
					OfrontOPM_WriteString((CHAR*)"(void*)", (LONGINT)8);
				}
				OfrontOPM_Write('&');
				prec = 9;
			} else if (OfrontOPV_ansi) {
				if (__IN(comp, 0x0c) && n->class == 7) {
					OfrontOPM_WriteString((CHAR*)"(CHAR*)", (LONGINT)8);
				} else if ((form == 13 && typ != n->typ) && n->typ != OfrontOPT_niltyp) {
					OfrontOPM_WriteString((CHAR*)"(void*)", (LONGINT)8);
				}
			} else {
				if (__IN(form, 0x0180) && __IN(n->typ->form, 0x70)) {
					OfrontOPM_WriteString((CHAR*)"(double)", (LONGINT)9);
					prec = 9;
				} else if (form == 6 && n->typ->form < 6) {
					OfrontOPM_WriteString((CHAR*)"(LONGINT)", (LONGINT)10);
					prec = 9;
				}
			}
		} else if (OfrontOPV_ansi) {
			if ((mode == 2 && typ != n->typ) && prec == -1) {
				OfrontOPM_WriteString((CHAR*)"(void*)", (LONGINT)8);
			}
		}
		if ((mode == 2 && n->class == 11) && n->subcl == 29) {
			OfrontOPV_expr(n->left, prec);
		} else {
			OfrontOPV_expr(n, prec);
		}
		if (((form == 6 && n->class == 7) && n->conval->intval <= (LONGINT)OfrontOPM_MaxInt) && n->conval->intval >= (LONGINT)OfrontOPM_MinInt) {
			OfrontOPM_PromoteIntConstToLInt();
		} else if (comp == 4 && mode == 2) {
			OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
			OfrontOPV_TypeOf(n);
		} else if (comp == 3) {
			if (n->class == 7) {
				OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
				OfrontOPM_WriteString((CHAR*)"(LONGINT)", (LONGINT)10);
				OfrontOPM_WriteInt(n->conval->intval2);
			} else {
				aptyp = n->typ;
				dim = 0;
				while (typ->comp == 3 && typ->BaseTyp->form != 1) {
					OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
					OfrontOPV_Len(n, dim);
					typ = typ->BaseTyp;
					aptyp = aptyp->BaseTyp;
					dim += 1;
				}
				if (typ->comp == 3 && typ->BaseTyp->form == 1) {
					OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
					while (aptyp->comp == 3) {
						OfrontOPV_Len(n, dim);
						OfrontOPM_WriteString((CHAR*)" * ", (LONGINT)4);
						dim += 1;
						aptyp = aptyp->BaseTyp;
					}
					OfrontOPM_WriteInt(aptyp->size);
					OfrontOPM_PromoteIntConstToLInt();
				}
			}
		}
		n = n->link;
		fp = fp->link;
		if (n != NIL) {
			OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
		}
	}
	OfrontOPM_Write(')');
}

static OfrontOPT_Object OfrontOPV_SuperProc (OfrontOPT_Node n)
{
	OfrontOPT_Object obj = NIL;
	OfrontOPT_Struct typ = NIL;
	typ = n->right->typ;
	if (typ->form == 13) {
		typ = typ->BaseTyp;
	}
	OfrontOPT_FindField(n->left->obj->name, typ->BaseTyp, &obj);
	return obj;
}

static void OfrontOPV_expr (OfrontOPT_Node n, INTEGER prec)
{
	INTEGER class, subclass, form, exprPrec;
	OfrontOPT_Struct typ = NIL;
	OfrontOPT_Node l = NIL, r = NIL;
	OfrontOPT_Object proc = NIL;
	class = n->class;
	subclass = n->subcl;
	form = n->typ->form;
	l = n->left;
	r = n->right;
	exprPrec = OfrontOPV_Precedence(class, subclass, form, n->typ->comp);
	if (exprPrec <= prec && __IN(class, 0x3ce0)) {
		OfrontOPM_Write('(');
	}
	switch (class) {
		case 7: 
			OfrontOPC_Constant(n->conval, form);
			break;
		case 10: 
			OfrontOPM_WriteString((CHAR*)"__SETRNG(", (LONGINT)10);
			OfrontOPV_expr(l, -1);
			OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
			OfrontOPV_expr(r, -1);
			OfrontOPM_Write(')');
			break;
		case 11: 
			switch (subclass) {
				case 33: 
					OfrontOPM_Write('!');
					OfrontOPV_expr(l, exprPrec);
					break;
				case 7: 
					if (form == 9) {
						OfrontOPM_Write('~');
					} else {
						OfrontOPM_Write('-');
					}
					OfrontOPV_expr(l, exprPrec);
					break;
				case 16: 
					typ = n->obj->typ;
					if (l->typ->comp == 4) {
						OfrontOPM_WriteString((CHAR*)"__IS(", (LONGINT)6);
						OfrontOPC_TypeOf(l->obj);
					} else {
						OfrontOPM_WriteString((CHAR*)"__ISP(", (LONGINT)7);
						OfrontOPV_expr(l, -1);
						typ = typ->BaseTyp;
					}
					OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
					OfrontOPC_Andent(typ);
					OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
					OfrontOPM_WriteInt(typ->extlev);
					OfrontOPM_Write(')');
					break;
				case 20: 
					OfrontOPV_Convert(l, form, exprPrec);
					break;
				case 21: 
					if (OfrontOPV_SideEffects(l)) {
						if (l->typ->form < 7) {
							if (l->typ->form < 6) {
								OfrontOPM_WriteString((CHAR*)"(INTEGER)", (LONGINT)10);
							}
							OfrontOPM_WriteString((CHAR*)"__ABSF(", (LONGINT)8);
						} else {
							OfrontOPM_WriteString((CHAR*)"__ABSFD(", (LONGINT)9);
						}
					} else {
						OfrontOPM_WriteString((CHAR*)"__ABS(", (LONGINT)7);
					}
					OfrontOPV_expr(l, -1);
					OfrontOPM_Write(')');
					break;
				case 22: 
					OfrontOPM_WriteString((CHAR*)"__CAP(", (LONGINT)7);
					OfrontOPV_expr(l, -1);
					OfrontOPM_Write(')');
					break;
				case 23: 
					OfrontOPM_WriteString((CHAR*)"__ODD(", (LONGINT)7);
					OfrontOPV_expr(l, -1);
					OfrontOPM_Write(')');
					break;
				case 24: 
					if (OfrontOPM_PointerSize == OfrontOPM_IntSize) {
						OfrontOPM_WriteString((CHAR*)"(INTEGER)", (LONGINT)10);
					} else {
						OfrontOPM_WriteString((CHAR*)"(LONGINT)", (LONGINT)10);
					}
					if (l->class == 1) {
						OfrontOPC_CompleteIdent(l->obj);
					} else {
						if (l->typ->form != 10 && !__IN(l->typ->comp, 0x0c)) {
							OfrontOPM_Write('&');
						}
						OfrontOPV_expr(l, exprPrec);
					}
					break;
				case 29: 
					if ((__IN(n->typ->form, 0x6240) && __IN(l->typ->form, 0x6240)) && n->typ->size == l->typ->size || !__IN(l->class, 0x17)) {
						OfrontOPM_Write('(');
						OfrontOPC_Ident(n->typ->strobj);
						OfrontOPM_Write(')');
						OfrontOPV_expr(l, exprPrec);
					} else {
						OfrontOPM_WriteString((CHAR*)"__VAL(", (LONGINT)7);
						OfrontOPC_Ident(n->typ->strobj);
						OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
						OfrontOPV_expr(l, -1);
						OfrontOPM_Write(')');
					}
					break;
				default: 
					OfrontOPM_err(200);
					break;
			}
			break;
		case 12: 
			switch (subclass) {
				case 19: 
					OfrontOPV_Len(l, r->conval->intval);
					break;
				case 15: case 17: case 18: case 26: case 27: 
				case 28: case 3: case 4: 
					switch (subclass) {
						case 15: 
							OfrontOPM_WriteString((CHAR*)"__IN(", (LONGINT)6);
							break;
						case 17: 
							if (r->class == 7) {
								if (r->conval->intval >= 0) {
									OfrontOPM_WriteString((CHAR*)"__ASHL(", (LONGINT)8);
								} else {
									OfrontOPM_WriteString((CHAR*)"__ASHR(", (LONGINT)8);
								}
							} else if (!OfrontOPV_SideEffects(r)) {
								OfrontOPM_WriteString((CHAR*)"__ASH(", (LONGINT)7);
							} else if (form == 6) {
								OfrontOPM_WriteString((CHAR*)"__ASHFL(", (LONGINT)9);
							} else {
								OfrontOPM_WriteString((CHAR*)"__ASHF(", (LONGINT)8);
							}
							break;
						case 18: 
							OfrontOPM_WriteString((CHAR*)"__MASK(", (LONGINT)8);
							break;
						case 26: 
							OfrontOPM_WriteString((CHAR*)"__BIT(", (LONGINT)7);
							break;
						case 27: 
							if (r->class == 7) {
								if (r->conval->intval >= 0) {
									OfrontOPM_WriteString((CHAR*)"__LSHL(", (LONGINT)8);
								} else {
									OfrontOPM_WriteString((CHAR*)"__LSHR(", (LONGINT)8);
								}
							} else {
								OfrontOPM_WriteString((CHAR*)"__LSH(", (LONGINT)7);
							}
							break;
						case 28: 
							if (r->class == 7) {
								if (r->conval->intval >= 0) {
									OfrontOPM_WriteString((CHAR*)"__ROTL(", (LONGINT)8);
								} else {
									OfrontOPM_WriteString((CHAR*)"__ROTR(", (LONGINT)8);
								}
							} else {
								OfrontOPM_WriteString((CHAR*)"__ROT(", (LONGINT)7);
							}
							break;
						case 3: 
							if (OfrontOPV_SideEffects(n)) {
								if (form < 6) {
									OfrontOPM_WriteString((CHAR*)"(INTEGER)", (LONGINT)10);
								}
								OfrontOPM_WriteString((CHAR*)"__DIVF(", (LONGINT)8);
							} else {
								OfrontOPM_WriteString((CHAR*)"__DIV(", (LONGINT)7);
							}
							break;
						case 4: 
							if (form < 6) {
								OfrontOPM_WriteString((CHAR*)"(INTEGER)", (LONGINT)10);
							}
							if (OfrontOPV_SideEffects(n)) {
								OfrontOPM_WriteString((CHAR*)"__MODF(", (LONGINT)8);
							} else {
								OfrontOPM_WriteString((CHAR*)"__MOD(", (LONGINT)7);
							}
							break;
						default: __CASECHK;
					}
					OfrontOPV_expr(l, -1);
					OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
					if ((__IN(subclass, 0x18020000) && r->class == 7) && r->conval->intval < 0) {
						OfrontOPM_WriteInt(-r->conval->intval);
					} else {
						OfrontOPV_expr(r, -1);
					}
					if (__IN(subclass, 0x18020000)) {
						OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
						OfrontOPC_Ident(l->typ->strobj);
					}
					OfrontOPM_Write(')');
					break;
				case 9: case 10: case 11: case 12: case 13: 
				case 14: 
					if (__IN(l->typ->form, 0x8400)) {
						OfrontOPM_WriteString((CHAR*)"__STRCMP(", (LONGINT)10);
						OfrontOPV_expr(l, -1);
						OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
						OfrontOPV_expr(r, -1);
						OfrontOPM_Write(')');
						OfrontOPC_Cmp(subclass);
						OfrontOPM_Write('0');
					} else {
						OfrontOPV_expr(l, exprPrec);
						OfrontOPC_Cmp(subclass);
						typ = l->typ;
						if (((typ->form == 13 && r->typ->form != 11) && r->typ != typ) && r->typ != OfrontOPT_sysptrtyp) {
							OfrontOPM_WriteString((CHAR*)"(void *) ", (LONGINT)10);
						}
						OfrontOPV_expr(r, exprPrec);
					}
					break;
				default: 
					OfrontOPV_expr(l, exprPrec);
					switch (subclass) {
						case 1: 
							if (form == 9) {
								OfrontOPM_WriteString((CHAR*)" & ", (LONGINT)4);
							} else {
								OfrontOPM_WriteString((CHAR*)" * ", (LONGINT)4);
							}
							break;
						case 2: 
							if (form == 9) {
								OfrontOPM_WriteString((CHAR*)" ^ ", (LONGINT)4);
							} else {
								OfrontOPM_WriteString((CHAR*)" / ", (LONGINT)4);
								if (r->obj == NIL || __IN(r->obj->typ->form, 0x70)) {
									OfrontOPM_Write('(');
									OfrontOPC_Ident(n->typ->strobj);
									OfrontOPM_Write(')');
								}
							}
							break;
						case 5: 
							OfrontOPM_WriteString((CHAR*)" && ", (LONGINT)5);
							break;
						case 6: 
							if (form == 9) {
								OfrontOPM_WriteString((CHAR*)" | ", (LONGINT)4);
							} else {
								OfrontOPM_WriteString((CHAR*)" + ", (LONGINT)4);
							}
							break;
						case 7: 
							if (form == 9) {
								OfrontOPM_WriteString((CHAR*)" & ~", (LONGINT)5);
							} else {
								OfrontOPM_WriteString((CHAR*)" - ", (LONGINT)4);
							}
							break;
						case 8: 
							OfrontOPM_WriteString((CHAR*)" || ", (LONGINT)5);
							break;
						default: __CASECHK;
					}
					OfrontOPV_expr(r, exprPrec);
					break;
			}
			break;
		case 13: 
			if (l->obj != NIL && l->obj->mode == 13) {
				if (l->subcl == 1) {
					proc = OfrontOPV_SuperProc(n);
				} else {
					OfrontOPM_WriteString((CHAR*)"__", (LONGINT)3);
					proc = OfrontOPC_BaseTProc(l->obj);
				}
				OfrontOPC_Ident(proc);
				n->obj = proc->link;
			} else if (l->class == 9) {
				OfrontOPV_design(l, 10);
			} else {
				OfrontOPV_design(l, 11);
			}
			OfrontOPV_ActualPar(r, n->obj);
			break;
		default: 
			OfrontOPV_design(n, prec);
			break;
	}
	if (exprPrec <= prec && __IN(class, 0x3ca0)) {
		OfrontOPM_Write(')');
	}
}

static void OfrontOPV_IfStat (OfrontOPT_Node n, BOOLEAN withtrap, OfrontOPT_Object outerProc)
{
	OfrontOPT_Node if_ = NIL;
	OfrontOPT_Object obj = NIL;
	OfrontOPT_Struct typ = NIL;
	LONGINT adr;
	if_ = n->left;
	while (if_ != NIL) {
		OfrontOPM_WriteString((CHAR*)"if ", (LONGINT)4);
		OfrontOPV_expr(if_->left, 12);
		OfrontOPM_Write(' ');
		OfrontOPC_BegBlk();
		if (n->class == 27 && if_->left->left != NIL) {
			obj = if_->left->left->obj;
			typ = obj->typ;
			adr = obj->adr;
			if (typ->comp == 4) {
				OfrontOPC_BegStat();
				OfrontOPC_Ident(if_->left->obj);
				OfrontOPM_WriteString((CHAR*)" *", (LONGINT)3);
				OfrontOPM_WriteString(obj->name, 32);
				OfrontOPM_WriteString((CHAR*)"__ = (void*)", (LONGINT)13);
				obj->adr = 0;
				OfrontOPC_CompleteIdent(obj);
				OfrontOPC_EndStat();
			}
			obj->adr = 1;
			obj->typ = if_->left->obj->typ;
			OfrontOPV_stat(if_->right, outerProc);
			obj->typ = typ;
			obj->adr = adr;
		} else {
			OfrontOPV_stat(if_->right, outerProc);
		}
		if_ = if_->link;
		if ((if_ != NIL || n->right != NIL) || withtrap) {
			OfrontOPC_EndBlk0();
			OfrontOPM_WriteString((CHAR*)" else ", (LONGINT)7);
		} else {
			OfrontOPC_EndBlk();
		}
	}
	if (withtrap) {
		OfrontOPM_WriteString((CHAR*)"__WITHCHK", (LONGINT)10);
		OfrontOPC_EndStat();
	} else if (n->right != NIL) {
		OfrontOPC_BegBlk();
		OfrontOPV_stat(n->right, outerProc);
		OfrontOPC_EndBlk();
	}
}

static void OfrontOPV_CaseStat (OfrontOPT_Node n, OfrontOPT_Object outerProc)
{
	OfrontOPT_Node switchCase = NIL, label = NIL;
	LONGINT low, high;
	INTEGER form, i;
	OfrontOPM_WriteString((CHAR*)"switch ", (LONGINT)8);
	OfrontOPV_expr(n->left, 12);
	OfrontOPM_Write(' ');
	OfrontOPC_BegBlk();
	form = n->left->typ->form;
	switchCase = n->right->left;
	while (switchCase != NIL) {
		label = switchCase->left;
		i = 0;
		while (label != NIL) {
			low = label->conval->intval;
			high = label->conval->intval2;
			while (low <= high) {
				if (i == 0) {
					OfrontOPC_BegStat();
				}
				OfrontOPC_Case(low, form);
				low += 1;
				i += 1;
				if (i == 5) {
					OfrontOPM_WriteLn();
					i = 0;
				}
			}
			label = label->link;
		}
		if (i > 0) {
			OfrontOPM_WriteLn();
		}
		OfrontOPC_Indent(1);
		OfrontOPV_stat(switchCase->right, outerProc);
		OfrontOPC_BegStat();
		OfrontOPM_WriteString((CHAR*)"break", (LONGINT)6);
		OfrontOPC_EndStat();
		OfrontOPC_Indent(-1);
		switchCase = switchCase->link;
	}
	OfrontOPC_BegStat();
	OfrontOPM_WriteString((CHAR*)"default: ", (LONGINT)10);
	if (n->right->conval->setval != 0x0) {
		OfrontOPC_Indent(1);
		OfrontOPM_WriteLn();
		OfrontOPV_stat(n->right->right, outerProc);
		OfrontOPC_BegStat();
		OfrontOPM_WriteString((CHAR*)"break", (LONGINT)6);
		OfrontOPC_Indent(-1);
	} else {
		OfrontOPM_WriteString((CHAR*)"__CASECHK", (LONGINT)10);
	}
	OfrontOPC_EndStat();
	OfrontOPC_EndBlk();
}

static BOOLEAN OfrontOPV_ImplicitReturn (OfrontOPT_Node n)
{
	while (n != NIL && n->class != 26) {
		n = n->link;
	}
	return n == NIL;
}

static void OfrontOPV_NewArr (OfrontOPT_Node d, OfrontOPT_Node x)
{
	OfrontOPT_Struct typ = NIL, base = NIL;
	INTEGER nofdim, nofdyn;
	typ = d->typ->BaseTyp;
	base = typ;
	nofdim = 0;
	nofdyn = 0;
	while (base->comp == 3) {
		nofdim += 1;
		nofdyn += 1;
		base = base->BaseTyp;
	}
	OfrontOPV_design(d, -1);
	OfrontOPM_WriteString((CHAR*)" = __NEWARR(", (LONGINT)13);
	while (base->comp == 2) {
		nofdim += 1;
		base = base->BaseTyp;
	}
	if (base->comp == 4 && OfrontOPC_NofPtrs(base) != 0) {
		OfrontOPC_Ident(base->strobj);
		OfrontOPM_WriteString((CHAR*)"__typ", (LONGINT)6);
	} else if (base->form == 13) {
		OfrontOPM_WriteString((CHAR*)"POINTER__typ", (LONGINT)13);
	} else {
		OfrontOPM_WriteString((CHAR*)"NIL", (LONGINT)4);
	}
	OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
	OfrontOPM_WriteInt(base->size);
	OfrontOPM_PromoteIntConstToLInt();
	OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
	OfrontOPM_WriteInt(OfrontOPC_Base(base));
	OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
	OfrontOPM_WriteInt(nofdim);
	OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
	OfrontOPM_WriteInt(nofdyn);
	while (typ != base) {
		OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
		if (typ->comp == 3) {
			if (x->class == 7) {
				OfrontOPV_expr(x, -1);
				OfrontOPM_PromoteIntConstToLInt();
			} else {
				OfrontOPM_WriteString((CHAR*)"(LONGINT)", (LONGINT)10);
				OfrontOPV_expr(x, 10);
			}
			x = x->link;
		} else {
			OfrontOPM_WriteInt(typ->n);
			OfrontOPM_PromoteIntConstToLInt();
		}
		typ = typ->BaseTyp;
	}
	OfrontOPM_Write(')');
}

static void OfrontOPV_DefineTDescs (OfrontOPT_Node n)
{
	while (n != NIL && n->class == 14) {
		OfrontOPC_TDescDecl(n->typ);
		n = n->link;
	}
}

static void OfrontOPV_InitTDescs (OfrontOPT_Node n)
{
	while (n != NIL && n->class == 14) {
		OfrontOPC_InitTDesc(n->typ);
		n = n->link;
	}
}

static void OfrontOPV_stat (OfrontOPT_Node n, OfrontOPT_Object outerProc)
{
	OfrontOPT_Object proc = NIL;
	OfrontOPV_ExitInfo saved;
	OfrontOPT_Node l = NIL, r = NIL;
	while (n != NIL && OfrontOPM_noerr) {
		OfrontOPM_errpos = n->conval->intval;
		if (n->class != 14) {
			OfrontOPC_BegStat();
		}
		switch (n->class) {
			case 18: 
				if (n->obj == NIL) {
					OfrontOPM_level += 1;
					OfrontOPV_stat(n->left, outerProc);
					OfrontOPM_level -= 1;
					OfrontOPC_GenEnumPtrs(OfrontOPT_topScope->scope);
					OfrontOPV_DefineTDescs(n->right);
					OfrontOPC_EnterBody();
					OfrontOPV_InitTDescs(n->right);
					OfrontOPM_WriteString((CHAR*)"/* BEGIN */", (LONGINT)12);
					OfrontOPM_WriteLn();
					OfrontOPV_stat(n->right, outerProc);
					OfrontOPC_ExitBody();
				} else {
					proc = n->obj;
					OfrontOPC_TypeDefs(proc->scope->right, 0);
					if (!proc->scope->leaf) {
						OfrontOPC_DefineInter(proc);
					}
					OfrontOPM_level += 1;
					OfrontOPV_stat(n->left, proc);
					OfrontOPM_level -= 1;
					OfrontOPC_EnterProc(proc);
					OfrontOPV_stat(n->right, proc);
					OfrontOPC_ExitProc(proc, 1, OfrontOPV_ImplicitReturn(n->right));
				}
				break;
			case 14: 
				break;
			case 19: 
				switch (n->subcl) {
					case 0: 
						l = n->left;
						r = n->right;
						if (l->typ->comp == 2) {
							OfrontOPM_WriteString((CHAR*)"__MOVE(", (LONGINT)8);
							OfrontOPV_expr(r, -1);
							OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
							OfrontOPV_expr(l, -1);
							OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
							if (r->typ == OfrontOPT_stringtyp) {
								OfrontOPM_WriteInt(r->conval->intval2);
							} else {
								OfrontOPM_WriteInt(r->typ->size);
							}
							OfrontOPM_Write(')');
						} else {
							if (((l->typ->form == 13 && l->obj != NIL) && l->obj->adr == 1) && l->obj->mode == 1) {
								l->obj->adr = 0;
								OfrontOPV_design(l, -1);
								l->obj->adr = 1;
								if (r->typ->form != 11) {
									OfrontOPM_WriteString((CHAR*)" = (void*)", (LONGINT)11);
								} else {
									OfrontOPM_WriteString((CHAR*)" = ", (LONGINT)4);
								}
							} else {
								OfrontOPV_design(l, -1);
								OfrontOPM_WriteString((CHAR*)" = ", (LONGINT)4);
							}
							if (l->typ == r->typ) {
								OfrontOPV_expr(r, -1);
							} else if ((l->typ->form == 13 && r->typ->form != 11) && l->typ->strobj != NIL) {
								OfrontOPM_Write('(');
								OfrontOPC_Ident(l->typ->strobj);
								OfrontOPM_Write(')');
								OfrontOPV_expr(r, -1);
							} else if (l->typ->comp == 4) {
								OfrontOPM_WriteString((CHAR*)"*(", (LONGINT)3);
								OfrontOPC_Andent(l->typ);
								OfrontOPM_WriteString((CHAR*)"*)&", (LONGINT)4);
								OfrontOPV_expr(r, 9);
							} else {
								OfrontOPV_expr(r, -1);
							}
						}
						break;
					case 1: 
						if (n->left->typ->BaseTyp->comp == 4) {
							OfrontOPM_WriteString((CHAR*)"__NEW(", (LONGINT)7);
							OfrontOPV_design(n->left, -1);
							OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
							OfrontOPC_Andent(n->left->typ->BaseTyp);
							OfrontOPM_WriteString((CHAR*)")", (LONGINT)2);
						} else if (__IN(n->left->typ->BaseTyp->comp, 0x0c)) {
							OfrontOPV_NewArr(n->left, n->right);
						}
						break;
					case 13: case 14: 
						OfrontOPV_expr(n->left, -1);
						OfrontOPC_Increment(n->subcl == 14);
						OfrontOPV_expr(n->right, -1);
						break;
					case 15: case 16: 
						OfrontOPV_expr(n->left, -1);
						OfrontOPC_SetInclude(n->subcl == 16);
						OfrontOPM_WriteString((CHAR*)"__SETOF(", (LONGINT)9);
						OfrontOPV_expr(n->right, -1);
						OfrontOPM_Write(')');
						break;
					case 18: 
						OfrontOPM_WriteString((CHAR*)"__COPY(", (LONGINT)8);
						OfrontOPV_expr(n->right, -1);
						OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
						OfrontOPV_expr(n->left, -1);
						OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
						OfrontOPV_Len(n->left, 0);
						OfrontOPM_Write(')');
						break;
					case 31: 
						OfrontOPM_WriteString((CHAR*)"__MOVE(", (LONGINT)8);
						OfrontOPV_expr(n->right, -1);
						OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
						OfrontOPV_expr(n->left, -1);
						OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
						OfrontOPV_expr(n->right->link, -1);
						OfrontOPM_Write(')');
						break;
					case 24: 
						OfrontOPM_WriteString((CHAR*)"__GET(", (LONGINT)7);
						OfrontOPV_expr(n->right, -1);
						OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
						OfrontOPV_expr(n->left, -1);
						OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
						OfrontOPC_Ident(n->left->typ->strobj);
						OfrontOPM_Write(')');
						break;
					case 25: 
						OfrontOPM_WriteString((CHAR*)"__PUT(", (LONGINT)7);
						OfrontOPV_expr(n->left, -1);
						OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
						OfrontOPV_expr(n->right, -1);
						OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
						OfrontOPC_Ident(n->right->typ->strobj);
						OfrontOPM_Write(')');
						break;
					case 26: case 27: 
						OfrontOPM_err(200);
						break;
					case 30: 
						OfrontOPM_WriteString((CHAR*)"__SYSNEW(", (LONGINT)10);
						OfrontOPV_design(n->left, -1);
						OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
						OfrontOPV_expr(n->right, -1);
						OfrontOPM_Write(')');
						break;
					default: __CASECHK;
				}
				break;
			case 13: 
				if (n->left->obj != NIL && n->left->obj->mode == 13) {
					if (n->left->subcl == 1) {
						proc = OfrontOPV_SuperProc(n);
					} else {
						OfrontOPM_WriteString((CHAR*)"__", (LONGINT)3);
						proc = OfrontOPC_BaseTProc(n->left->obj);
					}
					OfrontOPC_Ident(proc);
					n->obj = proc->link;
				} else if (n->left->class == 9) {
					OfrontOPV_design(n->left, 10);
				} else {
					OfrontOPV_design(n->left, 11);
				}
				OfrontOPV_ActualPar(n->right, n->obj);
				break;
			case 20: 
				if (n->subcl != 32) {
					OfrontOPV_IfStat(n, 0, outerProc);
				} else if (OfrontOPV_assert) {
					OfrontOPM_WriteString((CHAR*)"__ASSERT(", (LONGINT)10);
					OfrontOPV_expr(n->left->left->left, -1);
					OfrontOPM_WriteString((CHAR*)", ", (LONGINT)3);
					OfrontOPM_WriteInt(n->left->right->right->conval->intval);
					OfrontOPM_Write(')');
					OfrontOPC_EndStat();
				}
				break;
			case 21: 
				OfrontOPV_exit.level += 1;
				OfrontOPV_CaseStat(n, outerProc);
				OfrontOPV_exit.level -= 1;
				break;
			case 22: 
				OfrontOPV_exit.level += 1;
				OfrontOPM_WriteString((CHAR*)"while ", (LONGINT)7);
				OfrontOPV_expr(n->left, 12);
				OfrontOPM_Write(' ');
				OfrontOPC_BegBlk();
				OfrontOPV_stat(n->right, outerProc);
				OfrontOPC_EndBlk();
				OfrontOPV_exit.level -= 1;
				break;
			case 23: 
				OfrontOPV_exit.level += 1;
				OfrontOPM_WriteString((CHAR*)"do ", (LONGINT)4);
				OfrontOPC_BegBlk();
				OfrontOPV_stat(n->left, outerProc);
				OfrontOPC_EndBlk0();
				OfrontOPM_WriteString((CHAR*)" while (!", (LONGINT)10);
				OfrontOPV_expr(n->right, 9);
				OfrontOPM_Write(')');
				OfrontOPV_exit.level -= 1;
				break;
			case 24: 
				saved = OfrontOPV_exit;
				OfrontOPV_exit.level = 0;
				OfrontOPV_exit.label = -1;
				OfrontOPM_WriteString((CHAR*)"for (;;) ", (LONGINT)10);
				OfrontOPC_BegBlk();
				OfrontOPV_stat(n->left, outerProc);
				OfrontOPC_EndBlk();
				if (OfrontOPV_exit.label != -1) {
					OfrontOPC_BegStat();
					OfrontOPM_WriteString((CHAR*)"exit__", (LONGINT)7);
					OfrontOPM_WriteInt(OfrontOPV_exit.label);
					OfrontOPM_Write(':');
					OfrontOPC_EndStat();
				}
				OfrontOPV_exit = saved;
				break;
			case 25: 
				if (OfrontOPV_exit.level == 0) {
					OfrontOPM_WriteString((CHAR*)"break", (LONGINT)6);
				} else {
					if (OfrontOPV_exit.label == -1) {
						OfrontOPV_exit.label = OfrontOPV_nofExitLabels;
						OfrontOPV_nofExitLabels += 1;
					}
					OfrontOPM_WriteString((CHAR*)"goto exit__", (LONGINT)12);
					OfrontOPM_WriteInt(OfrontOPV_exit.label);
				}
				break;
			case 26: 
				if (OfrontOPM_level == 0) {
					if (OfrontOPV_mainprog) {
						OfrontOPM_WriteString((CHAR*)"__FINI", (LONGINT)7);
					} else {
						OfrontOPM_WriteString((CHAR*)"__ENDMOD", (LONGINT)9);
					}
				} else {
					OfrontOPC_ExitProc(outerProc, 0, 0);
					OfrontOPM_WriteString((CHAR*)"return", (LONGINT)7);
					if (n->left != NIL) {
						OfrontOPM_Write(' ');
						if (n->left->typ->form == 13 && n->obj->typ != n->left->typ) {
							OfrontOPM_WriteString((CHAR*)"(void*)", (LONGINT)8);
							OfrontOPV_expr(n->left, 10);
						} else {
							OfrontOPV_expr(n->left, -1);
						}
					}
				}
				break;
			case 27: 
				OfrontOPV_IfStat(n, n->subcl == 0, outerProc);
				break;
			case 28: 
				OfrontOPC_Halt(n->right->conval->intval);
				break;
			default: __CASECHK;
		}
		if (!__IN(n->class, 0x09744000)) {
			OfrontOPC_EndStat();
		}
		n = n->link;
	}
}

void OfrontOPV_Module (OfrontOPT_Node prog)
{
	if (!OfrontOPV_mainprog) {
		OfrontOPC_GenHdr(prog->right);
		OfrontOPC_GenHdrIncludes();
	}
	OfrontOPC_GenBdy(prog->right);
	OfrontOPV_stat(prog, NIL);
}

/*----------------------------------------------------------------------------*/
__TDESC(OfrontOPV_ExitInfo__desc, 1, 0) = {__TDFLDS("ExitInfo", 8), {-4}};

export void *OfrontOPV__init(void)
{
	__DEFMOD;
	__IMPORT(OfrontOPC__init);
	__IMPORT(OfrontOPM__init);
	__IMPORT(OfrontOPS__init);
	__IMPORT(OfrontOPT__init);
	__REGMOD("OfrontOPV", 0);
	__REGCMD("Init", OfrontOPV_Init);
	__INITYP(OfrontOPV_ExitInfo, OfrontOPV_ExitInfo, 0);
/* BEGIN */
	__ENDMOD;
}
