/*  Ofront 1.2 -xtspkae */
#include "SYSTEM.h"
#include "OfrontOPM.h"
#include "OfrontOPS.h"
#include "OfrontOPT.h"


export void (*OfrontOPB_typSize)(OfrontOPT_Struct);
static INTEGER OfrontOPB_exp;


export void OfrontOPB_Assign (OfrontOPT_Node *x, OfrontOPT_Node y);
static void OfrontOPB_BindNodes (SHORTINT class, OfrontOPT_Struct typ, OfrontOPT_Node *x, OfrontOPT_Node y);
static LONGINT OfrontOPB_BoolToInt (BOOLEAN b);
export void OfrontOPB_Call (OfrontOPT_Node *x, OfrontOPT_Node apar, OfrontOPT_Object fp);
static void OfrontOPB_CharToString (OfrontOPT_Node n);
static void OfrontOPB_CheckAssign (OfrontOPT_Struct x, OfrontOPT_Node ynode);
static void OfrontOPB_CheckLeaf (OfrontOPT_Node x, BOOLEAN dynArrToo);
export void OfrontOPB_CheckParameters (OfrontOPT_Object fp, OfrontOPT_Object ap, BOOLEAN checkNames);
static void OfrontOPB_CheckProc (OfrontOPT_Struct x, OfrontOPT_Object y);
static void OfrontOPB_CheckPtr (OfrontOPT_Node x, OfrontOPT_Node y);
static void OfrontOPB_CheckRealType (INTEGER f, INTEGER nr, OfrontOPT_Const x);
static void OfrontOPB_CheckReceiver (OfrontOPT_Node *x, OfrontOPT_Object fp);
static void OfrontOPB_ConstOp (INTEGER op, OfrontOPT_Node x, OfrontOPT_Node y);
export void OfrontOPB_Construct (SHORTINT class, OfrontOPT_Node *x, OfrontOPT_Node y);
static void OfrontOPB_Convert (OfrontOPT_Node *x, OfrontOPT_Struct typ);
export void OfrontOPB_DeRef (OfrontOPT_Node *x);
static void OfrontOPB_DynArrParCheck (OfrontOPT_Struct ftyp, OfrontOPT_Struct atyp, BOOLEAN fvarpar);
export OfrontOPT_Node OfrontOPB_EmptySet (void);
export void OfrontOPB_Enter (OfrontOPT_Node *procdec, OfrontOPT_Node stat, OfrontOPT_Object proc);
export void OfrontOPB_Field (OfrontOPT_Node *x, OfrontOPT_Object y);
export void OfrontOPB_In (OfrontOPT_Node *x, OfrontOPT_Node y);
export void OfrontOPB_Index (OfrontOPT_Node *x, OfrontOPT_Node y);
export void OfrontOPB_Inittd (OfrontOPT_Node *inittd, OfrontOPT_Node *last, OfrontOPT_Struct typ);
static BOOLEAN OfrontOPB_IntToBool (LONGINT i);
export void OfrontOPB_Link (OfrontOPT_Node *x, OfrontOPT_Node *last, OfrontOPT_Node y);
export void OfrontOPB_MOp (SHORTINT op, OfrontOPT_Node *x);
export OfrontOPT_Node OfrontOPB_NewBoolConst (BOOLEAN boolval);
export OfrontOPT_Node OfrontOPB_NewIntConst (LONGINT intval);
export OfrontOPT_Node OfrontOPB_NewLeaf (OfrontOPT_Object obj);
export OfrontOPT_Node OfrontOPB_NewRealConst (LONGREAL realval, OfrontOPT_Struct typ);
export OfrontOPT_Node OfrontOPB_NewString (OfrontOPS_String str, LONGINT len);
export OfrontOPT_Node OfrontOPB_Nil (void);
static BOOLEAN OfrontOPB_NotVar (OfrontOPT_Node x);
export void OfrontOPB_Op (SHORTINT op, OfrontOPT_Node *x, OfrontOPT_Node y);
export void OfrontOPB_OptIf (OfrontOPT_Node *x);
export void OfrontOPB_Param (OfrontOPT_Node ap, OfrontOPT_Object fp);
export void OfrontOPB_PrepCall (OfrontOPT_Node *x, OfrontOPT_Object *fpar);
export void OfrontOPB_Return (OfrontOPT_Node *x, OfrontOPT_Object proc);
export void OfrontOPB_SetElem (OfrontOPT_Node *x);
static void OfrontOPB_SetIntType (OfrontOPT_Node node);
export void OfrontOPB_SetRange (OfrontOPT_Node *x, OfrontOPT_Node y);
export void OfrontOPB_StFct (OfrontOPT_Node *par0, SHORTINT fctno, INTEGER parno);
export void OfrontOPB_StPar0 (OfrontOPT_Node *par0, INTEGER fctno);
export void OfrontOPB_StPar1 (OfrontOPT_Node *par0, OfrontOPT_Node x, SHORTINT fctno);
export void OfrontOPB_StParN (OfrontOPT_Node *par0, OfrontOPT_Node x, INTEGER fctno, INTEGER n);
export void OfrontOPB_StaticLink (SHORTINT dlev);
export void OfrontOPB_TypTest (OfrontOPT_Node *x, OfrontOPT_Object obj, BOOLEAN guard);
static void OfrontOPB_err (INTEGER n);
static LONGINT OfrontOPB_log (LONGINT x);


/*============================================================================*/

static void OfrontOPB_err (INTEGER n)
{
	OfrontOPM_err(n);
}

OfrontOPT_Node OfrontOPB_NewLeaf (OfrontOPT_Object obj)
{
	OfrontOPT_Node node = NIL;
	switch (obj->mode) {
		case 1: 
			node = OfrontOPT_NewNode(0);
			node->readonly = obj->vis == 2 && obj->mnolev < 0;
			break;
		case 2: 
			node = OfrontOPT_NewNode(1);
			break;
		case 3: 
			node = OfrontOPT_NewNode(7);
			node->conval = OfrontOPT_NewConst();
			__GUARDEQP(node->conval, OfrontOPT_ConstDesc) = *obj->conval;
			break;
		case 5: 
			node = OfrontOPT_NewNode(8);
			break;
		case 6: case 7: case 8: case 9: case 10: 
			node = OfrontOPT_NewNode(9);
			break;
		default: 
			OfrontOPB_err(127);
			node = OfrontOPT_NewNode(0);
			break;
	}
	node->obj = obj;
	node->typ = obj->typ;
	return node;
}

/*----------------------------------------------------------------------------*/
void OfrontOPB_Construct (SHORTINT class, OfrontOPT_Node *x, OfrontOPT_Node y)
{
	OfrontOPT_Node node = NIL;
	node = OfrontOPT_NewNode(class);
	node->typ = OfrontOPT_notyp;
	node->left = *x;
	node->right = y;
	*x = node;
}

/*----------------------------------------------------------------------------*/
void OfrontOPB_Link (OfrontOPT_Node *x, OfrontOPT_Node *last, OfrontOPT_Node y)
{
	if (*x == NIL) {
		*x = y;
	} else {
		(*last)->link = y;
	}
	while (y->link != NIL) {
		y = y->link;
	}
	*last = y;
}

/*----------------------------------------------------------------------------*/
static LONGINT OfrontOPB_BoolToInt (BOOLEAN b)
{
	if (b) {
		return 1;
	} else {
		return 0;
	}
	__RETCHK;
}

static BOOLEAN OfrontOPB_IntToBool (LONGINT i)
{
	if (i == 0) {
		return 0;
	} else {
		return 1;
	}
	__RETCHK;
}

OfrontOPT_Node OfrontOPB_NewBoolConst (BOOLEAN boolval)
{
	OfrontOPT_Node x = NIL;
	x = OfrontOPT_NewNode(7);
	x->typ = OfrontOPT_booltyp;
	x->conval = OfrontOPT_NewConst();
	x->conval->intval = OfrontOPB_BoolToInt(boolval);
	return x;
}

/*----------------------------------------------------------------------------*/
void OfrontOPB_OptIf (OfrontOPT_Node *x)
{
	OfrontOPT_Node if_ = NIL, pred = NIL;
	if_ = (*x)->left;
	while (if_->left->class == 7) {
		if (OfrontOPB_IntToBool(if_->left->conval->intval)) {
			*x = if_->right;
			return;
		} else if (if_->link == NIL) {
			*x = (*x)->right;
			return;
		} else {
			if_ = if_->link;
			(*x)->left = if_;
		}
	}
	pred = if_;
	if_ = if_->link;
	while (if_ != NIL) {
		if (if_->left->class == 7) {
			if (OfrontOPB_IntToBool(if_->left->conval->intval)) {
				pred->link = NIL;
				(*x)->right = if_->right;
				return;
			} else {
				if_ = if_->link;
				pred->link = if_;
			}
		} else {
			pred = if_;
			if_ = if_->link;
		}
	}
}

/*----------------------------------------------------------------------------*/
OfrontOPT_Node OfrontOPB_Nil (void)
{
	OfrontOPT_Node x = NIL;
	x = OfrontOPT_NewNode(7);
	x->typ = OfrontOPT_niltyp;
	x->conval = OfrontOPT_NewConst();
	x->conval->intval = 0;
	return x;
}

/*----------------------------------------------------------------------------*/
OfrontOPT_Node OfrontOPB_EmptySet (void)
{
	OfrontOPT_Node x = NIL;
	x = OfrontOPT_NewNode(7);
	x->typ = OfrontOPT_settyp;
	x->conval = OfrontOPT_NewConst();
	x->conval->setval = 0x0;
	return x;
}

/*----------------------------------------------------------------------------*/
static void OfrontOPB_SetIntType (OfrontOPT_Node node)
{
	LONGINT v;
	v = node->conval->intval;
	if ((LONGINT)OfrontOPM_MinSInt <= v && v <= (LONGINT)OfrontOPM_MaxSInt) {
		node->typ = OfrontOPT_sinttyp;
	} else if ((LONGINT)OfrontOPM_MinInt <= v && v <= (LONGINT)OfrontOPM_MaxInt) {
		node->typ = OfrontOPT_inttyp;
	} else if (OfrontOPM_MinLInt <= v && v <= OfrontOPM_MaxLInt) {
		node->typ = OfrontOPT_linttyp;
	} else {
		OfrontOPB_err(203);
		node->typ = OfrontOPT_sinttyp;
		node->conval->intval = 1;
	}
}

OfrontOPT_Node OfrontOPB_NewIntConst (LONGINT intval)
{
	OfrontOPT_Node x = NIL;
	x = OfrontOPT_NewNode(7);
	x->conval = OfrontOPT_NewConst();
	x->conval->intval = intval;
	OfrontOPB_SetIntType(x);
	return x;
}

/*----------------------------------------------------------------------------*/
OfrontOPT_Node OfrontOPB_NewRealConst (LONGREAL realval, OfrontOPT_Struct typ)
{
	OfrontOPT_Node x = NIL;
	x = OfrontOPT_NewNode(7);
	x->conval = OfrontOPT_NewConst();
	x->conval->realval = realval;
	x->typ = typ;
	x->conval->intval = -1;
	return x;
}

/*----------------------------------------------------------------------------*/
OfrontOPT_Node OfrontOPB_NewString (OfrontOPS_String str, LONGINT len)
{
	OfrontOPT_Node x = NIL;
	x = OfrontOPT_NewNode(7);
	x->conval = OfrontOPT_NewConst();
	x->typ = OfrontOPT_stringtyp;
	x->conval->intval = -1;
	x->conval->intval2 = len;
	x->conval->ext = OfrontOPT_NewExt();
	__MOVE(str, *x->conval->ext, 256);
	return x;
}

/*----------------------------------------------------------------------------*/
static void OfrontOPB_CharToString (OfrontOPT_Node n)
{
	CHAR ch;
	n->typ = OfrontOPT_stringtyp;
	ch = (CHAR)n->conval->intval;
	n->conval->ext = OfrontOPT_NewExt();
	if (ch == 0x00) {
		n->conval->intval2 = 1;
	} else {
		n->conval->intval2 = 2;
		(*n->conval->ext)[1] = 0x00;
	}
	(*n->conval->ext)[0] = ch;
	n->conval->intval = -1;
	n->obj = NIL;
}

static void OfrontOPB_BindNodes (SHORTINT class, OfrontOPT_Struct typ, OfrontOPT_Node *x, OfrontOPT_Node y)
{
	OfrontOPT_Node node = NIL;
	node = OfrontOPT_NewNode(class);
	node->typ = typ;
	node->left = *x;
	node->right = y;
	*x = node;
}

static BOOLEAN OfrontOPB_NotVar (OfrontOPT_Node x)
{
	return x->class >= 7 && ((x->class != 11 || x->subcl != 29) || x->left->class >= 7);
}

void OfrontOPB_DeRef (OfrontOPT_Node *x)
{
	OfrontOPT_Object strobj = NIL, bstrobj = NIL;
	OfrontOPT_Struct typ = NIL, btyp = NIL;
	typ = (*x)->typ;
	if ((*x)->class >= 7) {
		OfrontOPB_err(78);
	} else if (typ->form == 13) {
		if (typ == OfrontOPT_sysptrtyp) {
			OfrontOPB_err(57);
		}
		btyp = typ->BaseTyp;
		strobj = typ->strobj;
		bstrobj = btyp->strobj;
		if (((strobj != NIL && strobj->name[0] != 0x00) && bstrobj != NIL) && bstrobj->name[0] != 0x00) {
			btyp->pbused = 1;
		}
		OfrontOPB_BindNodes(3, btyp, &*x, NIL);
	} else {
		OfrontOPB_err(84);
	}
}

/*----------------------------------------------------------------------------*/
void OfrontOPB_Index (OfrontOPT_Node *x, OfrontOPT_Node y)
{
	INTEGER f;
	OfrontOPT_Struct typ = NIL;
	f = y->typ->form;
	if ((*x)->class >= 7) {
		OfrontOPB_err(79);
	} else if (!__IN(f, 0x70) || __IN(y->class, 0x0300)) {
		OfrontOPB_err(80);
		y->typ = OfrontOPT_inttyp;
	}
	if ((*x)->typ->comp == 2) {
		typ = (*x)->typ->BaseTyp;
		if (y->class == 7 && (y->conval->intval < 0 || y->conval->intval >= (*x)->typ->n)) {
			OfrontOPB_err(81);
		}
	} else if ((*x)->typ->comp == 3) {
		typ = (*x)->typ->BaseTyp;
		if (y->class == 7 && y->conval->intval < 0) {
			OfrontOPB_err(81);
		}
	} else {
		OfrontOPB_err(82);
		typ = OfrontOPT_undftyp;
	}
	OfrontOPB_BindNodes(4, typ, &*x, y);
	(*x)->readonly = (*x)->left->readonly;
}

/*----------------------------------------------------------------------------*/
void OfrontOPB_Field (OfrontOPT_Node *x, OfrontOPT_Object y)
{
	if ((*x)->class >= 7) {
		OfrontOPB_err(77);
	}
	if (y != NIL && __IN(y->mode, 0x2010)) {
		OfrontOPB_BindNodes(2, y->typ, &*x, NIL);
		(*x)->obj = y;
		(*x)->readonly = (*x)->left->readonly || y->vis == 2 && y->mnolev < 0;
	} else {
		OfrontOPB_err(83);
		(*x)->typ = OfrontOPT_undftyp;
	}
}

/*----------------------------------------------------------------------------*/
static struct TypTest__57 {
	OfrontOPT_Node *x;
	OfrontOPT_Object *obj;
	BOOLEAN *guard;
	struct TypTest__57 *lnk;
} *TypTest__57_s;

static void GTT__58 (OfrontOPT_Struct t0, OfrontOPT_Struct t1);

static void GTT__58 (OfrontOPT_Struct t0, OfrontOPT_Struct t1)
{
	OfrontOPT_Node node = NIL;
	OfrontOPT_Struct t = NIL;
	t = t0;
	while ((t != NIL && t != t1) && t != OfrontOPT_undftyp) {
		t = t->BaseTyp;
	}
	if (t != t1) {
		while ((t1 != NIL && t1 != t0) && t1 != OfrontOPT_undftyp) {
			t1 = t1->BaseTyp;
		}
		if (t1 == t0 || t0->form == 0) {
			if (*TypTest__57_s->guard) {
				OfrontOPB_BindNodes(5, NIL, &*TypTest__57_s->x, NIL);
				(*TypTest__57_s->x)->readonly = (*TypTest__57_s->x)->left->readonly;
			} else {
				node = OfrontOPT_NewNode(11);
				node->subcl = 16;
				node->left = *TypTest__57_s->x;
				node->obj = *TypTest__57_s->obj;
				*TypTest__57_s->x = node;
			}
		} else {
			OfrontOPB_err(85);
		}
	} else if (t0 != t1) {
		OfrontOPB_err(85);
	} else if (!*TypTest__57_s->guard) {
		if ((*TypTest__57_s->x)->class == 5) {
			node = OfrontOPT_NewNode(11);
			node->subcl = 16;
			node->left = *TypTest__57_s->x;
			node->obj = *TypTest__57_s->obj;
			*TypTest__57_s->x = node;
		} else {
			*TypTest__57_s->x = OfrontOPB_NewBoolConst(1);
		}
	}
}

void OfrontOPB_TypTest (OfrontOPT_Node *x, OfrontOPT_Object obj, BOOLEAN guard)
{
	struct TypTest__57 _s;
	_s.x = x;
	_s.obj = &obj;
	_s.guard = &guard;
	_s.lnk = TypTest__57_s;
	TypTest__57_s = &_s;
	if (OfrontOPB_NotVar(*x)) {
		OfrontOPB_err(112);
	} else if ((*x)->typ->form == 13) {
		if ((*x)->typ->BaseTyp->comp != 4 && (*x)->typ != OfrontOPT_sysptrtyp) {
			OfrontOPB_err(85);
		} else if (obj->typ->form == 13) {
			GTT__58((*x)->typ->BaseTyp, obj->typ->BaseTyp);
		} else {
			OfrontOPB_err(86);
		}
	} else if (((*x)->typ->comp == 4 && (*x)->class == 1) && obj->typ->comp == 4) {
		GTT__58((*x)->typ, obj->typ);
	} else {
		OfrontOPB_err(87);
	}
	if (guard) {
		(*x)->typ = obj->typ;
	} else {
		(*x)->typ = OfrontOPT_booltyp;
	}
	TypTest__57_s = _s.lnk;
}

/*----------------------------------------------------------------------------*/
void OfrontOPB_In (OfrontOPT_Node *x, OfrontOPT_Node y)
{
	INTEGER f;
	LONGINT k;
	f = (*x)->typ->form;
	if ((((*x)->class == 8 || (*x)->class == 9) || y->class == 8) || y->class == 9) {
		OfrontOPB_err(126);
	} else if (__IN(f, 0x70) && y->typ->form == 9) {
		if ((*x)->class == 7) {
			k = (*x)->conval->intval;
			if (k < 0 || k > (LONGINT)OfrontOPM_MaxSet) {
				OfrontOPB_err(202);
			} else if (y->class == 7) {
				(*x)->conval->intval = OfrontOPB_BoolToInt(__IN(k, y->conval->setval));
				(*x)->obj = NIL;
			} else {
				OfrontOPB_BindNodes(12, OfrontOPT_booltyp, &*x, y);
				(*x)->subcl = 15;
			}
		} else {
			OfrontOPB_BindNodes(12, OfrontOPT_booltyp, &*x, y);
			(*x)->subcl = 15;
		}
	} else {
		OfrontOPB_err(92);
	}
	(*x)->typ = OfrontOPT_booltyp;
}

/*----------------------------------------------------------------------------*/
static LONGINT OfrontOPB_log (LONGINT x)
{
	OfrontOPB_exp = 0;
	if (x > 0) {
		while (!__ODD(x)) {
			x = __ASHR(x, 1, LONGINT);
			OfrontOPB_exp += 1;
		}
	}
	return x;
}

static void OfrontOPB_CheckRealType (INTEGER f, INTEGER nr, OfrontOPT_Const x)
{
	LONGREAL min, max, r;
	if (f == 7) {
		min = OfrontOPM_MinReal;
		max = OfrontOPM_MaxReal;
	} else {
		min = OfrontOPM_MinLReal;
		max = OfrontOPM_MaxLReal;
	}
	r = __ABS(x->realval);
	if (r > max || r < min) {
		OfrontOPB_err(nr);
		x->realval = (LONGREAL)1;
	} else if (f == 7) {
		x->realval = x->realval;
	}
	x->intval = -1;
}

static struct MOp__28 {
	struct MOp__28 *lnk;
} *MOp__28_s;

static OfrontOPT_Node NewOp__29 (SHORTINT op, OfrontOPT_Struct typ, OfrontOPT_Node z);

static OfrontOPT_Node NewOp__29 (SHORTINT op, OfrontOPT_Struct typ, OfrontOPT_Node z)
{
	OfrontOPT_Node node = NIL;
	node = OfrontOPT_NewNode(11);
	node->subcl = op;
	node->typ = typ;
	node->left = z;
	return node;
}

void OfrontOPB_MOp (SHORTINT op, OfrontOPT_Node *x)
{
	INTEGER f;
	OfrontOPT_Struct typ = NIL;
	OfrontOPT_Node z = NIL;
	struct MOp__28 _s;
	_s.lnk = MOp__28_s;
	MOp__28_s = &_s;
	z = *x;
	if (z->class == 8 || z->class == 9) {
		OfrontOPB_err(126);
	} else {
		typ = z->typ;
		f = typ->form;
		switch (op) {
			case 33: 
				if (f == 2) {
					if (z->class == 7) {
						z->conval->intval = OfrontOPB_BoolToInt(!OfrontOPB_IntToBool(z->conval->intval));
						z->obj = NIL;
					} else {
						z = NewOp__29(op, typ, z);
					}
				} else {
					OfrontOPB_err(98);
				}
				break;
			case 6: 
				if (!__IN(f, 0x01f0)) {
					OfrontOPB_err(96);
				}
				break;
			case 7: 
				if (__IN(f, 0x03f0)) {
					if (z->class == 7) {
						if (__IN(f, 0x70)) {
							if (z->conval->intval == (-2147483647-1)) {
								OfrontOPB_err(203);
							} else {
								z->conval->intval = -z->conval->intval;
								OfrontOPB_SetIntType(z);
							}
						} else if (__IN(f, 0x0180)) {
							z->conval->realval = -z->conval->realval;
						} else {
							z->conval->setval = ~z->conval->setval;
						}
						z->obj = NIL;
					} else {
						z = NewOp__29(op, typ, z);
					}
				} else {
					OfrontOPB_err(97);
				}
				break;
			case 21: 
				if (__IN(f, 0x01f0)) {
					if (z->class == 7) {
						if (__IN(f, 0x70)) {
							if (z->conval->intval == (-2147483647-1)) {
								OfrontOPB_err(203);
							} else {
								z->conval->intval = __ABS(z->conval->intval);
								OfrontOPB_SetIntType(z);
							}
						} else {
							z->conval->realval = __ABS(z->conval->realval);
						}
						z->obj = NIL;
					} else {
						z = NewOp__29(op, typ, z);
					}
				} else {
					OfrontOPB_err(111);
				}
				break;
			case 22: 
				if (f == 3) {
					if (z->class == 7) {
						z->conval->intval = (INTEGER)__CAP((CHAR)z->conval->intval);
						z->obj = NIL;
					} else {
						z = NewOp__29(op, typ, z);
					}
				} else {
					OfrontOPB_err(111);
					z->typ = OfrontOPT_chartyp;
				}
				break;
			case 23: 
				if (__IN(f, 0x70)) {
					if (z->class == 7) {
						z->conval->intval = OfrontOPB_BoolToInt(__ODD(z->conval->intval));
						z->obj = NIL;
					} else {
						z = NewOp__29(op, typ, z);
					}
				} else {
					OfrontOPB_err(111);
				}
				z->typ = OfrontOPT_booltyp;
				break;
			case 24: 
				if ((z->class == 7 && f == 3) && z->conval->intval >= 32) {
					OfrontOPB_CharToString(z);
					f = 10;
				}
				if (z->class < 7 || f == 10) {
					z = NewOp__29(op, typ, z);
				} else {
					OfrontOPB_err(127);
				}
				if (OfrontOPM_PointerSize == OfrontOPM_IntSize) {
					z->typ = OfrontOPT_inttyp;
				} else {
					z->typ = OfrontOPT_linttyp;
				}
				break;
			case 25: 
				if (__IN(f, 0x70) && z->class == 7) {
					if (0 <= z->conval->intval && z->conval->intval <= -1) {
						z = NewOp__29(op, typ, z);
					} else {
						OfrontOPB_err(219);
					}
				} else {
					OfrontOPB_err(69);
				}
				z->typ = OfrontOPT_booltyp;
				break;
			default: __CASECHK;
		}
	}
	*x = z;
	MOp__28_s = _s.lnk;
}

/*----------------------------------------------------------------------------*/
static void OfrontOPB_CheckPtr (OfrontOPT_Node x, OfrontOPT_Node y)
{
	INTEGER g;
	OfrontOPT_Struct p = NIL, q = NIL, t = NIL;
	g = y->typ->form;
	if (g == 13) {
		p = x->typ->BaseTyp;
		q = y->typ->BaseTyp;
		if (p->comp == 4 && q->comp == 4) {
			if (p->extlev < q->extlev) {
				t = p;
				p = q;
				q = t;
			}
			while ((p != q && p != NIL) && p != OfrontOPT_undftyp) {
				p = p->BaseTyp;
			}
			if (p == NIL) {
				OfrontOPB_err(100);
			}
		} else {
			OfrontOPB_err(100);
		}
	} else if (g != 11) {
		OfrontOPB_err(100);
	}
}

void OfrontOPB_CheckParameters (OfrontOPT_Object fp, OfrontOPT_Object ap, BOOLEAN checkNames)
{
	OfrontOPT_Struct ft = NIL, at = NIL;
	while (fp != NIL) {
		if (ap != NIL) {
			ft = fp->typ;
			at = ap->typ;
			while (ft->comp == 3 && at->comp == 3) {
				ft = ft->BaseTyp;
				at = at->BaseTyp;
			}
			if (ft != at) {
				if (ft->form == 14 && at->form == 14) {
					if (ft->BaseTyp == at->BaseTyp) {
						OfrontOPB_CheckParameters(ft->link, at->link, 0);
					} else {
						OfrontOPB_err(117);
					}
				} else {
					OfrontOPB_err(115);
				}
			}
			if (fp->mode != ap->mode || checkNames && __STRCMP(fp->name, ap->name) != 0) {
				OfrontOPB_err(115);
			}
			ap = ap->link;
		} else {
			OfrontOPB_err(116);
		}
		fp = fp->link;
	}
	if (ap != NIL) {
		OfrontOPB_err(116);
	}
}

/*----------------------------------------------------------------------------*/
static void OfrontOPB_CheckProc (OfrontOPT_Struct x, OfrontOPT_Object y)
{
	if (__IN(y->mode, 0x04c0)) {
		if (y->mode == 6) {
			if (y->mnolev == 0) {
				y->mode = 7;
			} else {
				OfrontOPB_err(73);
			}
		}
		if (x->BaseTyp == y->typ) {
			OfrontOPB_CheckParameters(x->link, y->link, 0);
		} else {
			OfrontOPB_err(117);
		}
	} else {
		OfrontOPB_err(113);
	}
}

static struct ConstOp__13 {
	OfrontOPT_Node *x;
	INTEGER *f;
	OfrontOPT_Const *xval, *yval;
	struct ConstOp__13 *lnk;
} *ConstOp__13_s;

static INTEGER ConstCmp__14 (void);

static INTEGER ConstCmp__14 (void)
{
	INTEGER res;
	switch (*ConstOp__13_s->f) {
		case 0: 
			res = 9;
			break;
		case 1: case 3: case 4: case 5: case 6: 
			if ((*ConstOp__13_s->xval)->intval < (*ConstOp__13_s->yval)->intval) {
				res = 11;
			} else if ((*ConstOp__13_s->xval)->intval > (*ConstOp__13_s->yval)->intval) {
				res = 13;
			} else {
				res = 9;
			}
			break;
		case 7: case 8: 
			if ((*ConstOp__13_s->xval)->realval < (*ConstOp__13_s->yval)->realval) {
				res = 11;
			} else if ((*ConstOp__13_s->xval)->realval > (*ConstOp__13_s->yval)->realval) {
				res = 13;
			} else {
				res = 9;
			}
			break;
		case 2: 
			if ((*ConstOp__13_s->xval)->intval != (*ConstOp__13_s->yval)->intval) {
				res = 10;
			} else {
				res = 9;
			}
			break;
		case 9: 
			if ((*ConstOp__13_s->xval)->setval != (*ConstOp__13_s->yval)->setval) {
				res = 10;
			} else {
				res = 9;
			}
			break;
		case 10: 
			if (__STRCMP(*(*ConstOp__13_s->xval)->ext, *(*ConstOp__13_s->yval)->ext) < 0) {
				res = 11;
			} else if (__STRCMP(*(*ConstOp__13_s->xval)->ext, *(*ConstOp__13_s->yval)->ext) > 0) {
				res = 13;
			} else {
				res = 9;
			}
			break;
		case 11: case 13: case 14: 
			if ((*ConstOp__13_s->xval)->intval != (*ConstOp__13_s->yval)->intval) {
				res = 10;
			} else {
				res = 9;
			}
			break;
		default: __CASECHK;
	}
	(*ConstOp__13_s->x)->typ = OfrontOPT_booltyp;
	return res;
}

static void OfrontOPB_ConstOp (INTEGER op, OfrontOPT_Node x, OfrontOPT_Node y)
{
	INTEGER f, g;
	OfrontOPT_Const xval = NIL, yval = NIL;
	LONGINT xv, yv;
	BOOLEAN temp;
	struct ConstOp__13 _s;
	_s.x = &x;
	_s.f = &f;
	_s.xval = &xval;
	_s.yval = &yval;
	_s.lnk = ConstOp__13_s;
	ConstOp__13_s = &_s;
	f = x->typ->form;
	g = y->typ->form;
	xval = x->conval;
	yval = y->conval;
	if (f != g) {
		switch (f) {
			case 3: 
				if (g == 10) {
					OfrontOPB_CharToString(x);
				} else {
					OfrontOPB_err(100);
					y->typ = x->typ;
					__GUARDEQP(yval, OfrontOPT_ConstDesc) = *xval;
				}
				break;
			case 4: 
				if (__IN(g, 0x70)) {
					x->typ = y->typ;
				} else if (g == 7) {
					x->typ = OfrontOPT_realtyp;
					xval->realval = xval->intval;
				} else if (g == 8) {
					x->typ = OfrontOPT_lrltyp;
					xval->realval = xval->intval;
				} else {
					OfrontOPB_err(100);
					y->typ = x->typ;
					__GUARDEQP(yval, OfrontOPT_ConstDesc) = *xval;
				}
				break;
			case 5: 
				if (g == 4) {
					y->typ = OfrontOPT_inttyp;
				} else if (__IN(g, 0x70)) {
					x->typ = y->typ;
				} else if (g == 7) {
					x->typ = OfrontOPT_realtyp;
					xval->realval = xval->intval;
				} else if (g == 8) {
					x->typ = OfrontOPT_lrltyp;
					xval->realval = xval->intval;
				} else {
					OfrontOPB_err(100);
					y->typ = x->typ;
					__GUARDEQP(yval, OfrontOPT_ConstDesc) = *xval;
				}
				break;
			case 6: 
				if (__IN(g, 0x70)) {
					y->typ = OfrontOPT_linttyp;
				} else if (g == 7) {
					x->typ = OfrontOPT_realtyp;
					xval->realval = xval->intval;
				} else if (g == 8) {
					x->typ = OfrontOPT_lrltyp;
					xval->realval = xval->intval;
				} else {
					OfrontOPB_err(100);
					y->typ = x->typ;
					__GUARDEQP(yval, OfrontOPT_ConstDesc) = *xval;
				}
				break;
			case 7: 
				if (__IN(g, 0x70)) {
					y->typ = x->typ;
					yval->realval = yval->intval;
				} else if (g == 8) {
					x->typ = OfrontOPT_lrltyp;
				} else {
					OfrontOPB_err(100);
					y->typ = x->typ;
					__GUARDEQP(yval, OfrontOPT_ConstDesc) = *xval;
				}
				break;
			case 8: 
				if (__IN(g, 0x70)) {
					y->typ = x->typ;
					yval->realval = yval->intval;
				} else if (g == 7) {
					y->typ = OfrontOPT_lrltyp;
				} else {
					OfrontOPB_err(100);
					y->typ = x->typ;
					__GUARDEQP(yval, OfrontOPT_ConstDesc) = *xval;
				}
				break;
			case 10: 
				if (g == 3) {
					OfrontOPB_CharToString(y);
					g = 10;
				} else {
					OfrontOPB_err(100);
					y->typ = x->typ;
					__GUARDEQP(yval, OfrontOPT_ConstDesc) = *xval;
				}
				break;
			case 11: 
				if (!__IN(g, 0x6000)) {
					OfrontOPB_err(100);
				}
				break;
			case 13: 
				OfrontOPB_CheckPtr(x, y);
				break;
			case 14: 
				if (g != 11) {
					OfrontOPB_err(100);
				}
				break;
			default: 
				OfrontOPB_err(100);
				y->typ = x->typ;
				__GUARDEQP(yval, OfrontOPT_ConstDesc) = *xval;
				break;
		}
		f = x->typ->form;
	}
	switch (op) {
		case 1: 
			if (__IN(f, 0x70)) {
				xv = xval->intval;
				yv = yval->intval;
				if (((((xv == 0 || yv == 0) || (xv > 0 && yv > 0) && yv <= __DIV(2147483647, xv)) || (xv > 0 && yv < 0) && yv >= __DIV((-2147483647-1), xv)) || (xv < 0 && yv > 0) && xv >= __DIV((-2147483647-1), yv)) || (((xv < 0 && yv < 0) && xv != (-2147483647-1)) && yv != (-2147483647-1)) && -xv <= __DIV(2147483647, -yv)) {
					xval->intval = xv * yv;
					OfrontOPB_SetIntType(x);
				} else {
					OfrontOPB_err(204);
				}
			} else if (__IN(f, 0x0180)) {
				temp = __ABS(yval->realval) <= (LONGREAL)1;
				if (temp || __ABS(xval->realval) <= OfrontOPM_MaxLReal / (LONGREAL)__ABS(yval->realval)) {
					xval->realval = xval->realval * yval->realval;
					OfrontOPB_CheckRealType(f, 204, xval);
				} else {
					OfrontOPB_err(204);
				}
			} else if (f == 9) {
				xval->setval = xval->setval & yval->setval;
			} else if (f != 0) {
				OfrontOPB_err(101);
			}
			break;
		case 2: 
			if (__IN(f, 0x70)) {
				if (yval->intval != 0) {
					xval->realval = xval->intval / (REAL)yval->intval;
					OfrontOPB_CheckRealType(7, 205, xval);
				} else {
					OfrontOPB_err(205);
					xval->realval = (LONGREAL)1;
				}
				x->typ = OfrontOPT_realtyp;
			} else if (__IN(f, 0x0180)) {
				temp = __ABS(yval->realval) >= (LONGREAL)1;
				if (temp || __ABS(xval->realval) <= OfrontOPM_MaxLReal * __ABS(yval->realval)) {
					xval->realval = xval->realval / yval->realval;
					OfrontOPB_CheckRealType(f, 205, xval);
				} else {
					OfrontOPB_err(205);
				}
			} else if (f == 9) {
				xval->setval = xval->setval ^ yval->setval;
			} else if (f != 0) {
				OfrontOPB_err(102);
			}
			break;
		case 3: 
			if (__IN(f, 0x70)) {
				if (yval->intval != 0) {
					xval->intval = __DIV(xval->intval, yval->intval);
					OfrontOPB_SetIntType(x);
				} else {
					OfrontOPB_err(205);
				}
			} else if (f != 0) {
				OfrontOPB_err(103);
			}
			break;
		case 4: 
			if (__IN(f, 0x70)) {
				if (yval->intval != 0) {
					xval->intval = __MOD(xval->intval, yval->intval);
					OfrontOPB_SetIntType(x);
				} else {
					OfrontOPB_err(205);
				}
			} else if (f != 0) {
				OfrontOPB_err(104);
			}
			break;
		case 5: 
			if (f == 2) {
				xval->intval = OfrontOPB_BoolToInt(OfrontOPB_IntToBool(xval->intval) && OfrontOPB_IntToBool(yval->intval));
			} else {
				OfrontOPB_err(94);
			}
			break;
		case 6: 
			if (__IN(f, 0x70)) {
				temp = yval->intval >= 0 && xval->intval <= 2147483647 - yval->intval;
				if (temp || yval->intval < 0 && xval->intval >= (-2147483647-1) - yval->intval) {
					xval->intval += yval->intval;
					OfrontOPB_SetIntType(x);
				} else {
					OfrontOPB_err(206);
				}
			} else if (__IN(f, 0x0180)) {
				temp = yval->realval >= (LONGREAL)0 && xval->realval <= OfrontOPM_MaxLReal - yval->realval;
				if (temp || yval->realval < (LONGREAL)0 && xval->realval >= -OfrontOPM_MaxLReal - yval->realval) {
					xval->realval = xval->realval + yval->realval;
					OfrontOPB_CheckRealType(f, 206, xval);
				} else {
					OfrontOPB_err(206);
				}
			} else if (f == 9) {
				xval->setval = xval->setval | yval->setval;
			} else if (f != 0) {
				OfrontOPB_err(105);
			}
			break;
		case 7: 
			if (__IN(f, 0x70)) {
				if (yval->intval >= 0 && xval->intval >= (-2147483647-1) + yval->intval || yval->intval < 0 && xval->intval <= 2147483647 + yval->intval) {
					xval->intval -= yval->intval;
					OfrontOPB_SetIntType(x);
				} else {
					OfrontOPB_err(207);
				}
			} else if (__IN(f, 0x0180)) {
				temp = yval->realval >= (LONGREAL)0 && xval->realval >= -OfrontOPM_MaxLReal + yval->realval;
				if (temp || yval->realval < (LONGREAL)0 && xval->realval <= OfrontOPM_MaxLReal + yval->realval) {
					xval->realval = xval->realval - yval->realval;
					OfrontOPB_CheckRealType(f, 207, xval);
				} else {
					OfrontOPB_err(207);
				}
			} else if (f == 9) {
				xval->setval = xval->setval & ~yval->setval;
			} else if (f != 0) {
				OfrontOPB_err(106);
			}
			break;
		case 8: 
			if (f == 2) {
				xval->intval = OfrontOPB_BoolToInt(OfrontOPB_IntToBool(xval->intval) || OfrontOPB_IntToBool(yval->intval));
			} else {
				OfrontOPB_err(95);
			}
			break;
		case 9: 
			xval->intval = OfrontOPB_BoolToInt(ConstCmp__14() == 9);
			break;
		case 10: 
			xval->intval = OfrontOPB_BoolToInt(ConstCmp__14() != 9);
			break;
		case 11: 
			if (__IN(f, 0x2a04)) {
				OfrontOPB_err(108);
			} else {
				xval->intval = OfrontOPB_BoolToInt(ConstCmp__14() == 11);
			}
			break;
		case 12: 
			if (__IN(f, 0x2a04)) {
				OfrontOPB_err(108);
			} else {
				xval->intval = OfrontOPB_BoolToInt(ConstCmp__14() != 13);
			}
			break;
		case 13: 
			if (__IN(f, 0x2a04)) {
				OfrontOPB_err(108);
			} else {
				xval->intval = OfrontOPB_BoolToInt(ConstCmp__14() == 13);
			}
			break;
		case 14: 
			if (__IN(f, 0x2a04)) {
				OfrontOPB_err(108);
			} else {
				xval->intval = OfrontOPB_BoolToInt(ConstCmp__14() != 11);
			}
			break;
		default: __CASECHK;
	}
	ConstOp__13_s = _s.lnk;
}

static void OfrontOPB_Convert (OfrontOPT_Node *x, OfrontOPT_Struct typ)
{
	OfrontOPT_Node node = NIL;
	INTEGER f, g;
	LONGINT k;
	LONGREAL r;
	f = (*x)->typ->form;
	g = typ->form;
	if ((*x)->class == 7) {
		if (__IN(f, 0x70)) {
			if (__IN(g, 0x70)) {
				if (f > g) {
					OfrontOPB_SetIntType(*x);
					if ((INTEGER)(*x)->typ->form > g) {
						OfrontOPB_err(203);
						(*x)->conval->intval = 1;
					}
				}
			} else if (__IN(g, 0x0180)) {
				(*x)->conval->realval = (*x)->conval->intval;
				(*x)->conval->intval = -1;
			} else {
				k = (*x)->conval->intval;
				if (0 > k || k > 255) {
					OfrontOPB_err(220);
				}
			}
		} else if (__IN(f, 0x0180)) {
			if (__IN(g, 0x0180)) {
				OfrontOPB_CheckRealType(g, 203, (*x)->conval);
			} else {
				r = (*x)->conval->realval;
				if (r < -2147483648.0 || r > 2147483647.0) {
					OfrontOPB_err(203);
					r = (LONGREAL)1;
				}
				(*x)->conval->intval = __ENTIER(r);
				OfrontOPB_SetIntType(*x);
			}
		}
		(*x)->obj = NIL;
	} else if (((*x)->class == 11 && (*x)->subcl == 20) && ((INTEGER)(*x)->left->typ->form < f || f > g)) {
		if ((*x)->left->typ == typ) {
			*x = (*x)->left;
		}
	} else {
		node = OfrontOPT_NewNode(11);
		node->subcl = 20;
		node->left = *x;
		*x = node;
	}
	(*x)->typ = typ;
}

static struct Op__38 {
	INTEGER *f, *g;
	struct Op__38 *lnk;
} *Op__38_s;

static void NewOp__39 (SHORTINT op, OfrontOPT_Struct typ, OfrontOPT_Node *x, OfrontOPT_Node y);
static BOOLEAN strings__41 (OfrontOPT_Node *x, OfrontOPT_Node *y);

static void NewOp__39 (SHORTINT op, OfrontOPT_Struct typ, OfrontOPT_Node *x, OfrontOPT_Node y)
{
	OfrontOPT_Node node = NIL;
	node = OfrontOPT_NewNode(12);
	node->subcl = op;
	node->typ = typ;
	node->left = *x;
	node->right = y;
	*x = node;
}

static BOOLEAN strings__41 (OfrontOPT_Node *x, OfrontOPT_Node *y)
{
	BOOLEAN ok, xCharArr, yCharArr;
	xCharArr = __IN((*x)->typ->comp, 0x0c) && (*x)->typ->BaseTyp->form == 3 || *Op__38_s->f == 10;
	yCharArr = __IN((*y)->typ->comp, 0x0c) && (*y)->typ->BaseTyp->form == 3 || *Op__38_s->g == 10;
	if ((xCharArr && *Op__38_s->g == 3) && (*y)->class == 7) {
		OfrontOPB_CharToString(*y);
		*Op__38_s->g = 10;
		yCharArr = 1;
	}
	if ((yCharArr && *Op__38_s->f == 3) && (*x)->class == 7) {
		OfrontOPB_CharToString(*x);
		*Op__38_s->f = 10;
		xCharArr = 1;
	}
	ok = xCharArr && yCharArr;
	if (ok) {
		if (*Op__38_s->f == 10 && (*x)->conval->intval2 == 1) {
			(*x)->typ = OfrontOPT_chartyp;
			(*x)->conval->intval = 0;
			OfrontOPB_Index(&*y, OfrontOPB_NewIntConst(0));
		} else if (*Op__38_s->g == 10 && (*y)->conval->intval2 == 1) {
			(*y)->typ = OfrontOPT_chartyp;
			(*y)->conval->intval = 0;
			OfrontOPB_Index(&*x, OfrontOPB_NewIntConst(0));
		}
	}
	return ok;
}

void OfrontOPB_Op (SHORTINT op, OfrontOPT_Node *x, OfrontOPT_Node y)
{
	INTEGER f, g;
	OfrontOPT_Node t = NIL, z = NIL;
	OfrontOPT_Struct typ = NIL;
	BOOLEAN do_;
	LONGINT val;
	struct Op__38 _s;
	_s.f = &f;
	_s.g = &g;
	_s.lnk = Op__38_s;
	Op__38_s = &_s;
	z = *x;
	if (((z->class == 8 || z->class == 9) || y->class == 8) || y->class == 9) {
		OfrontOPB_err(126);
	} else if (z->class == 7 && y->class == 7) {
		OfrontOPB_ConstOp(op, z, y);
		z->obj = NIL;
	} else {
		if (z->typ != y->typ) {
			g = y->typ->form;
			switch (z->typ->form) {
				case 3: 
					if (z->class == 7) {
						OfrontOPB_CharToString(z);
					} else {
						OfrontOPB_err(100);
					}
					break;
				case 4: 
					if (__IN(g, 0x01f0)) {
						OfrontOPB_Convert(&z, y->typ);
					} else {
						OfrontOPB_err(100);
					}
					break;
				case 5: 
					if (g == 4) {
						OfrontOPB_Convert(&y, z->typ);
					} else if (__IN(g, 0x01f0)) {
						OfrontOPB_Convert(&z, y->typ);
					} else {
						OfrontOPB_err(100);
					}
					break;
				case 6: 
					if (__IN(g, 0x70)) {
						OfrontOPB_Convert(&y, z->typ);
					} else if (__IN(g, 0x0180)) {
						OfrontOPB_Convert(&z, y->typ);
					} else {
						OfrontOPB_err(100);
					}
					break;
				case 7: 
					if (__IN(g, 0x70)) {
						OfrontOPB_Convert(&y, z->typ);
					} else if (__IN(g, 0x0180)) {
						OfrontOPB_Convert(&z, y->typ);
					} else {
						OfrontOPB_err(100);
					}
					break;
				case 8: 
					if (__IN(g, 0x01f0)) {
						OfrontOPB_Convert(&y, z->typ);
					} else if (__IN(g, 0x0180)) {
						OfrontOPB_Convert(&y, z->typ);
					} else {
						OfrontOPB_err(100);
					}
					break;
				case 11: 
					if (!__IN(g, 0x6000)) {
						OfrontOPB_err(100);
					}
					break;
				case 13: 
					OfrontOPB_CheckPtr(z, y);
					break;
				case 14: 
					if (g != 11) {
						OfrontOPB_err(100);
					}
					break;
				case 10: 
					break;
				case 15: 
					if (z->typ->comp == 4) {
						OfrontOPB_err(100);
					}
					break;
				default: 
					OfrontOPB_err(100);
					break;
			}
		}
		typ = z->typ;
		f = typ->form;
		g = y->typ->form;
		switch (op) {
			case 1: 
				do_ = 1;
				if (__IN(f, 0x70)) {
					if (z->class == 7) {
						val = z->conval->intval;
						if (val == 1) {
							do_ = 0;
							z = y;
						} else if (val == 0) {
							do_ = 0;
						} else if (OfrontOPB_log(val) == 1) {
							t = y;
							y = z;
							z = t;
							op = 17;
							y->typ = OfrontOPT_sinttyp;
							y->conval->intval = OfrontOPB_exp;
							y->obj = NIL;
						}
					} else if (y->class == 7) {
						val = y->conval->intval;
						if (val == 1) {
							do_ = 0;
						} else if (val == 0) {
							do_ = 0;
							z = y;
						} else if (OfrontOPB_log(val) == 1) {
							op = 17;
							y->typ = OfrontOPT_sinttyp;
							y->conval->intval = OfrontOPB_exp;
							y->obj = NIL;
						}
					}
				} else if (!__IN(f, 0x0381)) {
					OfrontOPB_err(105);
					typ = OfrontOPT_undftyp;
				}
				if (do_) {
					NewOp__39(op, typ, &z, y);
				}
				break;
			case 2: 
				if (__IN(f, 0x70)) {
					if (y->class == 7 && y->conval->intval == 0) {
						OfrontOPB_err(205);
					}
					OfrontOPB_Convert(&z, OfrontOPT_realtyp);
					OfrontOPB_Convert(&y, OfrontOPT_realtyp);
					typ = OfrontOPT_realtyp;
				} else if (__IN(f, 0x0180)) {
					if (y->class == 7 && y->conval->realval == (LONGREAL)0) {
						OfrontOPB_err(205);
					}
				} else if (f != 9 && f != 0) {
					OfrontOPB_err(102);
					typ = OfrontOPT_undftyp;
				}
				NewOp__39(op, typ, &z, y);
				break;
			case 3: 
				do_ = 1;
				if (__IN(f, 0x70)) {
					if (y->class == 7) {
						val = y->conval->intval;
						if (val == 0) {
							OfrontOPB_err(205);
						} else if (val == 1) {
							do_ = 0;
						} else if (OfrontOPB_log(val) == 1) {
							op = 17;
							y->typ = OfrontOPT_sinttyp;
							y->conval->intval = -OfrontOPB_exp;
							y->obj = NIL;
						}
					}
				} else if (f != 0) {
					OfrontOPB_err(103);
					typ = OfrontOPT_undftyp;
				}
				if (do_) {
					NewOp__39(op, typ, &z, y);
				}
				break;
			case 4: 
				if (__IN(f, 0x70)) {
					if (y->class == 7) {
						if (y->conval->intval == 0) {
							OfrontOPB_err(205);
						} else if (OfrontOPB_log(y->conval->intval) == 1) {
							op = 18;
							y->conval->intval = __ASH(-1, OfrontOPB_exp, LONGINT);
							y->obj = NIL;
						}
					}
				} else if (f != 0) {
					OfrontOPB_err(104);
					typ = OfrontOPT_undftyp;
				}
				NewOp__39(op, typ, &z, y);
				break;
			case 5: 
				if (f == 2) {
					if (z->class == 7) {
						if (OfrontOPB_IntToBool(z->conval->intval)) {
							z = y;
						}
					} else if (y->class == 7 && OfrontOPB_IntToBool(y->conval->intval)) {
					} else {
						NewOp__39(op, typ, &z, y);
					}
				} else if (f != 0) {
					OfrontOPB_err(94);
					z->typ = OfrontOPT_undftyp;
				}
				break;
			case 6: 
				if (!__IN(f, 0x03f1)) {
					OfrontOPB_err(105);
					typ = OfrontOPT_undftyp;
				}
				do_ = 1;
				if (__IN(f, 0x70)) {
					if (z->class == 7 && z->conval->intval == 0) {
						do_ = 0;
						z = y;
					}
					if (y->class == 7 && y->conval->intval == 0) {
						do_ = 0;
					}
				}
				if (do_) {
					NewOp__39(op, typ, &z, y);
				}
				break;
			case 7: 
				if (!__IN(f, 0x03f1)) {
					OfrontOPB_err(106);
					typ = OfrontOPT_undftyp;
				}
				if ((!__IN(f, 0x70) || y->class != 7) || y->conval->intval != 0) {
					NewOp__39(op, typ, &z, y);
				}
				break;
			case 8: 
				if (f == 2) {
					if (z->class == 7) {
						if (!OfrontOPB_IntToBool(z->conval->intval)) {
							z = y;
						}
					} else if (y->class == 7 && !OfrontOPB_IntToBool(y->conval->intval)) {
					} else {
						NewOp__39(op, typ, &z, y);
					}
				} else if (f != 0) {
					OfrontOPB_err(95);
					z->typ = OfrontOPT_undftyp;
				}
				break;
			case 9: case 10: 
				if (__IN(f, 0x6bff) || strings__41(&z, &y)) {
					typ = OfrontOPT_booltyp;
				} else {
					OfrontOPB_err(107);
					typ = OfrontOPT_undftyp;
				}
				NewOp__39(op, typ, &z, y);
				break;
			case 11: case 12: case 13: case 14: 
				if (__IN(f, 0x01f9) || strings__41(&z, &y)) {
					typ = OfrontOPT_booltyp;
				} else {
					OfrontOPB_err(108);
					typ = OfrontOPT_undftyp;
				}
				NewOp__39(op, typ, &z, y);
				break;
			default: __CASECHK;
		}
	}
	*x = z;
	Op__38_s = _s.lnk;
}

/*----------------------------------------------------------------------------*/
void OfrontOPB_SetRange (OfrontOPT_Node *x, OfrontOPT_Node y)
{
	LONGINT k, l;
	if ((((*x)->class == 8 || (*x)->class == 9) || y->class == 8) || y->class == 9) {
		OfrontOPB_err(126);
	} else if (__IN((*x)->typ->form, 0x70) && __IN(y->typ->form, 0x70)) {
		if ((*x)->class == 7) {
			k = (*x)->conval->intval;
			if (0 > k || k > (LONGINT)OfrontOPM_MaxSet) {
				OfrontOPB_err(202);
			}
		}
		if (y->class == 7) {
			l = y->conval->intval;
			if (0 > l || l > (LONGINT)OfrontOPM_MaxSet) {
				OfrontOPB_err(202);
			}
		}
		if ((*x)->class == 7 && y->class == 7) {
			if (k <= l) {
				(*x)->conval->setval = __SETRNG(k, l);
			} else {
				OfrontOPB_err(201);
				(*x)->conval->setval = __SETRNG(l, k);
			}
			(*x)->obj = NIL;
		} else {
			OfrontOPB_BindNodes(10, OfrontOPT_settyp, &*x, y);
		}
	} else {
		OfrontOPB_err(93);
	}
	(*x)->typ = OfrontOPT_settyp;
}

/*----------------------------------------------------------------------------*/
void OfrontOPB_SetElem (OfrontOPT_Node *x)
{
	LONGINT k;
	if ((*x)->class == 8 || (*x)->class == 9) {
		OfrontOPB_err(126);
	} else if (!__IN((*x)->typ->form, 0x70)) {
		OfrontOPB_err(93);
	} else if ((*x)->class == 7) {
		k = (*x)->conval->intval;
		if (0 <= k && k <= (LONGINT)OfrontOPM_MaxSet) {
			(*x)->conval->setval = __SETOF(k);
		} else {
			OfrontOPB_err(202);
		}
		(*x)->obj = NIL;
	} else {
		OfrontOPB_Convert(&*x, OfrontOPT_settyp);
	}
	(*x)->typ = OfrontOPT_settyp;
}

/*----------------------------------------------------------------------------*/
static void OfrontOPB_CheckAssign (OfrontOPT_Struct x, OfrontOPT_Node ynode)
{
	INTEGER f, g;
	OfrontOPT_Struct y = NIL, p = NIL, q = NIL;
	y = ynode->typ;
	f = x->form;
	g = y->form;
	if (ynode->class == 8 || ynode->class == 9 && f != 14) {
		OfrontOPB_err(126);
	}
	switch (f) {
		case 0: case 10: 
			break;
		case 1: 
			if (!__IN(g, 0x1a)) {
				OfrontOPB_err(113);
			}
			break;
		case 2: case 3: case 4: case 9: 
			if (g != f) {
				OfrontOPB_err(113);
			}
			break;
		case 5: 
			if (!__IN(g, 0x30)) {
				OfrontOPB_err(113);
			}
			break;
		case 6: 
			if (!__IN(g, 0x70)) {
				OfrontOPB_err(113);
			}
			break;
		case 7: 
			if (!__IN(g, 0xf0)) {
				OfrontOPB_err(113);
			}
			break;
		case 8: 
			if (!__IN(g, 0x01f0)) {
				OfrontOPB_err(113);
			}
			break;
		case 13: 
			if ((x == y || g == 11) || x == OfrontOPT_sysptrtyp && g == 13) {
			} else if (g == 13) {
				p = x->BaseTyp;
				q = y->BaseTyp;
				if (p->comp == 4 && q->comp == 4) {
					while ((q != p && q != NIL) && q != OfrontOPT_undftyp) {
						q = q->BaseTyp;
					}
					if (q == NIL) {
						OfrontOPB_err(113);
					}
				} else {
					OfrontOPB_err(113);
				}
			} else {
				OfrontOPB_err(113);
			}
			break;
		case 14: 
			if (ynode->class == 9) {
				OfrontOPB_CheckProc(x, ynode->obj);
			} else if (x == y || g == 11) {
			} else {
				OfrontOPB_err(113);
			}
			break;
		case 12: case 11: 
			OfrontOPB_err(113);
			break;
		case 15: 
			x->pvused = 1;
			if (x->comp == 2) {
				if (ynode->class == 7 && g == 3) {
					OfrontOPB_CharToString(ynode);
					y = ynode->typ;
					g = 10;
				}
				if (x == y) {
				} else if (g == 10 && x->BaseTyp == OfrontOPT_chartyp) {
					if (ynode->conval->intval2 > x->n) {
						OfrontOPB_err(114);
					}
				} else {
					OfrontOPB_err(113);
				}
			} else if (x->comp == 4) {
				if (x == y) {
				} else if (y->comp == 4) {
					q = y->BaseTyp;
					while ((q != NIL && q != x) && q != OfrontOPT_undftyp) {
						q = q->BaseTyp;
					}
					if (q == NIL) {
						OfrontOPB_err(113);
					}
				} else {
					OfrontOPB_err(113);
				}
			} else {
				OfrontOPB_err(113);
			}
			break;
		default: __CASECHK;
	}
	if (((ynode->class == 7 && g < f) && __IN(g, 0xf0)) && __IN(f, 0x01e0)) {
		OfrontOPB_Convert(&ynode, x);
	}
}

static void OfrontOPB_CheckLeaf (OfrontOPT_Node x, BOOLEAN dynArrToo)
{
}

void OfrontOPB_StPar0 (OfrontOPT_Node *par0, INTEGER fctno)
{
	INTEGER f;
	OfrontOPT_Struct typ = NIL;
	OfrontOPT_Node x = NIL;
	x = *par0;
	f = x->typ->form;
	switch (fctno) {
		case 0: 
			if (__IN(f, 0x70) && x->class == 7) {
				if (0 <= x->conval->intval && x->conval->intval <= 255) {
					OfrontOPB_BindNodes(28, OfrontOPT_notyp, &x, x);
				} else {
					OfrontOPB_err(218);
				}
			} else {
				OfrontOPB_err(69);
			}
			x->typ = OfrontOPT_notyp;
			break;
		case 1: 
			typ = OfrontOPT_notyp;
			if (OfrontOPB_NotVar(x)) {
				OfrontOPB_err(112);
			} else if (f == 13) {
				if (x->readonly) {
					OfrontOPB_err(76);
				}
				f = x->typ->BaseTyp->comp;
				if (__IN(f, 0x1c)) {
					if (f == 3) {
						typ = x->typ->BaseTyp;
					}
					OfrontOPB_BindNodes(19, OfrontOPT_notyp, &x, NIL);
					x->subcl = 1;
				} else {
					OfrontOPB_err(111);
				}
			} else {
				OfrontOPB_err(111);
			}
			x->typ = typ;
			break;
		case 2: 
			OfrontOPB_MOp(21, &x);
			break;
		case 3: 
			OfrontOPB_MOp(22, &x);
			break;
		case 4: 
			if (x->class == 8 || x->class == 9) {
				OfrontOPB_err(126);
			} else if (f == 3) {
				if (OfrontOPM_SIntSize == 2) {
					OfrontOPB_Convert(&x, OfrontOPT_sinttyp);
				} else {
					OfrontOPB_Convert(&x, OfrontOPT_inttyp);
				}
			} else if (f == 9) {
				if (OfrontOPM_IntSize == 4) {
					OfrontOPB_Convert(&x, OfrontOPT_inttyp);
				} else {
					OfrontOPB_Convert(&x, OfrontOPT_linttyp);
				}
			} else {
				OfrontOPB_err(111);
			}
			break;
		case 5: 
			if (x->class == 8 || x->class == 9) {
				OfrontOPB_err(126);
			} else if (__IN(f, 0x0180)) {
				OfrontOPB_Convert(&x, OfrontOPT_linttyp);
			} else {
				OfrontOPB_err(111);
			}
			x->typ = OfrontOPT_linttyp;
			break;
		case 6: 
			OfrontOPB_MOp(23, &x);
			break;
		case 7: 
			if (x->class == 8) {
				switch (f) {
					case 2: 
						x = OfrontOPB_NewBoolConst(0);
						break;
					case 3: 
						x = OfrontOPB_NewIntConst(0);
						x->typ = OfrontOPT_chartyp;
						break;
					case 4: 
						x = OfrontOPB_NewIntConst(OfrontOPM_MinSInt);
						break;
					case 5: 
						x = OfrontOPB_NewIntConst(OfrontOPM_MinInt);
						break;
					case 6: 
						x = OfrontOPB_NewIntConst(OfrontOPM_MinLInt);
						break;
					case 9: 
						x = OfrontOPB_NewIntConst(0);
						x->typ = OfrontOPT_inttyp;
						break;
					case 7: 
						x = OfrontOPB_NewRealConst(OfrontOPM_MinReal, OfrontOPT_realtyp);
						break;
					case 8: 
						x = OfrontOPB_NewRealConst(OfrontOPM_MinLReal, OfrontOPT_lrltyp);
						break;
					default: 
						OfrontOPB_err(111);
						break;
				}
			} else {
				OfrontOPB_err(110);
			}
			break;
		case 8: 
			if (x->class == 8) {
				switch (f) {
					case 2: 
						x = OfrontOPB_NewBoolConst(1);
						break;
					case 3: 
						x = OfrontOPB_NewIntConst(255);
						x->typ = OfrontOPT_chartyp;
						break;
					case 4: 
						x = OfrontOPB_NewIntConst(OfrontOPM_MaxSInt);
						break;
					case 5: 
						x = OfrontOPB_NewIntConst(OfrontOPM_MaxInt);
						break;
					case 6: 
						x = OfrontOPB_NewIntConst(OfrontOPM_MaxLInt);
						break;
					case 9: 
						x = OfrontOPB_NewIntConst(OfrontOPM_MaxSet);
						x->typ = OfrontOPT_inttyp;
						break;
					case 7: 
						x = OfrontOPB_NewRealConst(OfrontOPM_MaxReal, OfrontOPT_realtyp);
						break;
					case 8: 
						x = OfrontOPB_NewRealConst(OfrontOPM_MaxLReal, OfrontOPT_lrltyp);
						break;
					default: 
						OfrontOPB_err(111);
						break;
				}
			} else {
				OfrontOPB_err(110);
			}
			break;
		case 9: 
			if (x->class == 8 || x->class == 9) {
				OfrontOPB_err(126);
			} else if (__IN(f, 0x71)) {
				OfrontOPB_Convert(&x, OfrontOPT_chartyp);
			} else {
				OfrontOPB_err(111);
				x->typ = OfrontOPT_chartyp;
			}
			break;
		case 10: 
			if (x->class == 8 || x->class == 9) {
				OfrontOPB_err(126);
			} else if (f == 5) {
				OfrontOPB_Convert(&x, OfrontOPT_sinttyp);
			} else if (f == 6) {
				OfrontOPB_Convert(&x, OfrontOPT_inttyp);
			} else if (f == 8) {
				OfrontOPB_Convert(&x, OfrontOPT_realtyp);
			} else {
				OfrontOPB_err(111);
			}
			break;
		case 11: 
			if (x->class == 8 || x->class == 9) {
				OfrontOPB_err(126);
			} else if (f == 4) {
				OfrontOPB_Convert(&x, OfrontOPT_inttyp);
			} else if (f == 5) {
				OfrontOPB_Convert(&x, OfrontOPT_linttyp);
			} else if (f == 7) {
				OfrontOPB_Convert(&x, OfrontOPT_lrltyp);
			} else if (f == 3) {
				OfrontOPB_Convert(&x, OfrontOPT_linttyp);
			} else {
				OfrontOPB_err(111);
			}
			break;
		case 13: case 14: 
			if (OfrontOPB_NotVar(x)) {
				OfrontOPB_err(112);
			} else if (!__IN(f, 0x70)) {
				OfrontOPB_err(111);
			} else if (x->readonly) {
				OfrontOPB_err(76);
			}
			break;
		case 15: case 16: 
			if (OfrontOPB_NotVar(x)) {
				OfrontOPB_err(112);
			} else if (x->typ != OfrontOPT_settyp) {
				OfrontOPB_err(111);
				x->typ = OfrontOPT_settyp;
			} else if (x->readonly) {
				OfrontOPB_err(76);
			}
			break;
		case 17: 
			if (!__IN(x->typ->comp, 0x0c)) {
				OfrontOPB_err(131);
			}
			break;
		case 18: 
			if (x->class == 7 && f == 3) {
				OfrontOPB_CharToString(x);
				f = 10;
			}
			if (x->class == 8 || x->class == 9) {
				OfrontOPB_err(126);
			} else if ((!__IN(x->typ->comp, 0x0c) || x->typ->BaseTyp->form != 3) && f != 10) {
				OfrontOPB_err(111);
			}
			break;
		case 19: 
			if (x->class == 8 || x->class == 9) {
				OfrontOPB_err(126);
			} else if (__IN(f, 0x70)) {
				if (f < 5) {
					OfrontOPB_Convert(&x, OfrontOPT_inttyp);
				}
			} else {
				OfrontOPB_err(111);
				x->typ = OfrontOPT_inttyp;
			}
			break;
		case 20: 
			OfrontOPB_CheckLeaf(x, 0);
			OfrontOPB_MOp(24, &x);
			break;
		case 12: 
			if (x->class != 8) {
				OfrontOPB_err(110);
				x = OfrontOPB_NewIntConst(1);
			} else if (__IN(f, 0x63fe) || __IN(x->typ->comp, 0x14)) {
				(*OfrontOPB_typSize)(x->typ);
				x->typ->pvused = 1;
				x = OfrontOPB_NewIntConst(x->typ->size);
			} else {
				OfrontOPB_err(111);
				x = OfrontOPB_NewIntConst(1);
			}
			break;
		case 21: 
			OfrontOPB_MOp(25, &x);
			break;
		case 22: case 23: 
			if (x->class == 8 || x->class == 9) {
				OfrontOPB_err(126);
			} else if (!__IN(f, 0x027a)) {
				OfrontOPB_err(111);
			}
			break;
		case 24: case 25: case 28: case 31: 
			if (x->class == 8 || x->class == 9) {
				OfrontOPB_err(126);
			} else if (OfrontOPM_PointerSize == OfrontOPM_IntSize) {
				if (x->class == 7 && __IN(f, 0x12)) {
					OfrontOPB_Convert(&x, OfrontOPT_inttyp);
				} else if (!__IN(f, 0x2020)) {
					OfrontOPB_err(111);
					x->typ = OfrontOPT_inttyp;
				}
			} else {
				if (x->class == 7 && __IN(f, 0x32)) {
					OfrontOPB_Convert(&x, OfrontOPT_linttyp);
				} else if (!__IN(f, 0x2040)) {
					OfrontOPB_err(111);
					x->typ = OfrontOPT_linttyp;
				}
			}
			break;
		case 26: case 27: 
			if (__IN(f, 0x70) && x->class == 7) {
				if (x->conval->intval < 0 || x->conval->intval > -1) {
					OfrontOPB_err(220);
				}
			} else {
				OfrontOPB_err(69);
			}
			break;
		case 29: 
			if (x->class != 8) {
				OfrontOPB_err(110);
			} else if (__IN(f, 0x1401) || x->typ->comp == 3) {
				OfrontOPB_err(111);
			}
			break;
		case 30: 
			if (OfrontOPB_NotVar(x)) {
				OfrontOPB_err(112);
			} else if (f == 13) {
			} else {
				OfrontOPB_err(111);
			}
			break;
		case 32: 
			if (x->class == 8 || x->class == 9) {
				OfrontOPB_err(126);
				x = OfrontOPB_NewBoolConst(0);
			} else if (f != 2) {
				OfrontOPB_err(120);
				x = OfrontOPB_NewBoolConst(0);
			} else {
				OfrontOPB_MOp(33, &x);
			}
			break;
		default: __CASECHK;
	}
	*par0 = x;
}

/*----------------------------------------------------------------------------*/
static struct StPar1__52 {
	struct StPar1__52 *lnk;
} *StPar1__52_s;

static OfrontOPT_Node NewOp__53 (SHORTINT class, SHORTINT subcl, OfrontOPT_Node left, OfrontOPT_Node right);

static OfrontOPT_Node NewOp__53 (SHORTINT class, SHORTINT subcl, OfrontOPT_Node left, OfrontOPT_Node right)
{
	OfrontOPT_Node node = NIL;
	node = OfrontOPT_NewNode(class);
	node->subcl = subcl;
	node->left = left;
	node->right = right;
	return node;
}

void OfrontOPB_StPar1 (OfrontOPT_Node *par0, OfrontOPT_Node x, SHORTINT fctno)
{
	INTEGER f, L;
	OfrontOPT_Struct typ = NIL;
	OfrontOPT_Node p = NIL, t = NIL;
	LONGINT maxInt;
	INTEGER maxExp;
	struct StPar1__52 _s;
	_s.lnk = StPar1__52_s;
	StPar1__52_s = &_s;
	p = *par0;
	f = x->typ->form;
	switch (fctno) {
		case 13: case 14: 
			if (x->class == 8 || x->class == 9) {
				OfrontOPB_err(126);
				p->typ = OfrontOPT_notyp;
			} else {
				if (x->typ != p->typ) {
					if (x->class == 7 && __IN(f, 0x70)) {
						OfrontOPB_Convert(&x, p->typ);
					} else {
						OfrontOPB_err(111);
					}
				}
				p = NewOp__53(19, fctno, p, x);
				p->typ = OfrontOPT_notyp;
			}
			break;
		case 15: case 16: 
			if (x->class == 8 || x->class == 9) {
				OfrontOPB_err(126);
			} else if (__IN(f, 0x70)) {
				if (x->class == 7 && (0 > x->conval->intval || x->conval->intval > (LONGINT)OfrontOPM_MaxSet)) {
					OfrontOPB_err(202);
				}
				p = NewOp__53(19, fctno, p, x);
			} else {
				OfrontOPB_err(111);
			}
			p->typ = OfrontOPT_notyp;
			break;
		case 17: 
			if (!__IN(f, 0x70) || x->class != 7) {
				OfrontOPB_err(69);
			} else if (f == 4) {
				L = (INTEGER)x->conval->intval;
				typ = p->typ;
				while (L > 0 && __IN(typ->comp, 0x0c)) {
					typ = typ->BaseTyp;
					L -= 1;
				}
				if (L != 0 || !__IN(typ->comp, 0x0c)) {
					OfrontOPB_err(132);
				} else {
					x->obj = NIL;
					if (typ->comp == 3) {
						while (p->class == 4) {
							p = p->left;
							x->conval->intval += 1;
						}
						p = NewOp__53(12, 19, p, x);
						p->typ = OfrontOPT_linttyp;
					} else {
						p = x;
						p->conval->intval = typ->n;
						OfrontOPB_SetIntType(p);
					}
				}
			} else {
				OfrontOPB_err(132);
			}
			break;
		case 18: 
			if (OfrontOPB_NotVar(x)) {
				OfrontOPB_err(112);
			} else if (__IN(x->typ->comp, 0x0c) && x->typ->BaseTyp->form == 3) {
				if (x->readonly) {
					OfrontOPB_err(76);
				}
				t = x;
				x = p;
				p = t;
				p = NewOp__53(19, 18, p, x);
			} else {
				OfrontOPB_err(111);
			}
			p->typ = OfrontOPT_notyp;
			break;
		case 19: 
			if (x->class == 8 || x->class == 9) {
				OfrontOPB_err(126);
			} else if (__IN(f, 0x70)) {
				if (p->typ == OfrontOPT_linttyp) {
					maxInt = OfrontOPM_MaxLInt;
				} else {
					maxInt = OfrontOPM_MaxInt;
				}
				maxExp = (INTEGER)OfrontOPB_log(__ASHR(maxInt, 1, LONGINT) + 1);
				maxExp = OfrontOPB_exp;
				if (x->class == 7 && ((LONGINT)(-maxExp) > x->conval->intval || x->conval->intval > (LONGINT)maxExp)) {
					OfrontOPB_err(208);
				} else if (p->class == 7 && x->class == 7) {
					if (x->conval->intval >= 0) {
						if (__ABS(p->conval->intval) <= __DIV(maxInt, __ASH(1, (INTEGER)x->conval->intval, LONGINT))) {
							p->conval->intval = p->conval->intval * __ASH(1, (INTEGER)x->conval->intval, LONGINT);
						} else {
							OfrontOPB_err(208);
							p->conval->intval = 1;
						}
					} else {
						p->conval->intval = __ASH(p->conval->intval, (INTEGER)x->conval->intval, LONGINT);
					}
					p->obj = NIL;
				} else {
					if (f == 6) {
						OfrontOPB_err(113);
						OfrontOPB_Convert(&x, OfrontOPT_inttyp);
					}
					typ = p->typ;
					p = NewOp__53(12, 17, p, x);
					p->typ = typ;
				}
			} else {
				OfrontOPB_err(111);
			}
			break;
		case 1: 
			if (x->class == 8 || x->class == 9) {
				OfrontOPB_err(126);
			} else if (p->typ->comp == 3) {
				if (__IN(f, 0x70)) {
					if (x->class == 7 && (x->conval->intval <= 0 || x->conval->intval > (LONGINT)OfrontOPM_MaxIndex)) {
						OfrontOPB_err(63);
					}
				} else {
					OfrontOPB_err(111);
				}
				p->right = x;
				p->typ = p->typ->BaseTyp;
			} else {
				OfrontOPB_err(64);
			}
			break;
		case 22: case 23: 
			if (x->class == 8 || x->class == 9) {
				OfrontOPB_err(126);
			} else if (!__IN(f, 0x70)) {
				OfrontOPB_err(111);
			} else {
				if (fctno == 22) {
					p = NewOp__53(12, 27, p, x);
				} else {
					p = NewOp__53(12, 28, p, x);
				}
				p->typ = p->left->typ;
			}
			break;
		case 24: case 25: case 26: case 27: 
			if (x->class == 8 || x->class == 9) {
				OfrontOPB_err(126);
			} else if (__IN(f, 0x63ff)) {
				if (fctno == 24 || fctno == 26) {
					if (OfrontOPB_NotVar(x)) {
						OfrontOPB_err(112);
					}
					t = x;
					x = p;
					p = t;
				}
				p = NewOp__53(19, fctno, p, x);
			} else {
				OfrontOPB_err(111);
			}
			p->typ = OfrontOPT_notyp;
			break;
		case 28: 
			if (x->class == 8 || x->class == 9) {
				OfrontOPB_err(126);
			} else if (__IN(f, 0x70)) {
				p = NewOp__53(12, 26, p, x);
			} else {
				OfrontOPB_err(111);
			}
			p->typ = OfrontOPT_booltyp;
			break;
		case 29: 
			if (((x->class == 8 || x->class == 9) || __IN(f, 0x1401)) || x->typ->comp == 3) {
				OfrontOPB_err(126);
			}
			t = OfrontOPT_NewNode(11);
			t->subcl = 29;
			t->left = x;
			x = t;
			x->typ = p->typ;
			p = x;
			break;
		case 30: 
			if (x->class == 8 || x->class == 9) {
				OfrontOPB_err(126);
			} else if (__IN(f, 0x70)) {
				p = NewOp__53(19, 30, p, x);
			} else {
				OfrontOPB_err(111);
			}
			p->typ = OfrontOPT_notyp;
			break;
		case 31: 
			if (x->class == 8 || x->class == 9) {
				OfrontOPB_err(126);
			} else if (OfrontOPM_PointerSize == OfrontOPM_IntSize) {
				if (x->class == 7 && __IN(f, 0x12)) {
					OfrontOPB_Convert(&x, OfrontOPT_inttyp);
				} else if (!__IN(f, 0x2020)) {
					OfrontOPB_err(111);
					x->typ = OfrontOPT_inttyp;
				}
			} else {
				if (x->class == 7 && __IN(f, 0x32)) {
					OfrontOPB_Convert(&x, OfrontOPT_linttyp);
				} else if (!__IN(f, 0x2040)) {
					OfrontOPB_err(111);
					x->typ = OfrontOPT_linttyp;
				}
			}
			p->link = x;
			break;
		case 32: 
			if (__IN(f, 0x70) && x->class == 7) {
				if (0 <= x->conval->intval && x->conval->intval <= 255) {
					OfrontOPB_BindNodes(28, OfrontOPT_notyp, &x, x);
					x->conval = OfrontOPT_NewConst();
					x->conval->intval = OfrontOPM_errpos;
					OfrontOPB_Construct(15, &p, x);
					p->conval = OfrontOPT_NewConst();
					p->conval->intval = OfrontOPM_errpos;
					OfrontOPB_Construct(20, &p, NIL);
					OfrontOPB_OptIf(&p);
					if (p == NIL) {
					} else if (p->class == 28) {
						OfrontOPB_err(99);
					} else {
						p->subcl = 32;
					}
				} else {
					OfrontOPB_err(218);
				}
			} else {
				OfrontOPB_err(69);
			}
			break;
		default: 
			OfrontOPB_err(64);
			break;
	}
	*par0 = p;
	StPar1__52_s = _s.lnk;
}

/*----------------------------------------------------------------------------*/
void OfrontOPB_StParN (OfrontOPT_Node *par0, OfrontOPT_Node x, INTEGER fctno, INTEGER n)
{
	OfrontOPT_Node node = NIL;
	INTEGER f;
	OfrontOPT_Node p = NIL;
	p = *par0;
	f = x->typ->form;
	if (fctno == 1) {
		if (x->class == 8 || x->class == 9) {
			OfrontOPB_err(126);
		} else if (p->typ->comp != 3) {
			OfrontOPB_err(64);
		} else if (__IN(f, 0x70)) {
			if (x->class == 7 && (x->conval->intval <= 0 || x->conval->intval > (LONGINT)OfrontOPM_MaxIndex)) {
				OfrontOPB_err(63);
			}
			node = p->right;
			while (node->link != NIL) {
				node = node->link;
			}
			node->link = x;
			p->typ = p->typ->BaseTyp;
		} else {
			OfrontOPB_err(111);
		}
	} else if (fctno == 31 && n == 2) {
		if (x->class == 8 || x->class == 9) {
			OfrontOPB_err(126);
		} else if (__IN(f, 0x70)) {
			node = OfrontOPT_NewNode(19);
			node->subcl = 31;
			node->right = p;
			node->left = p->link;
			p->link = x;
			p = node;
		} else {
			OfrontOPB_err(111);
		}
		p->typ = OfrontOPT_notyp;
	} else {
		OfrontOPB_err(64);
	}
	*par0 = p;
}

/*----------------------------------------------------------------------------*/
void OfrontOPB_StFct (OfrontOPT_Node *par0, SHORTINT fctno, INTEGER parno)
{
	INTEGER dim;
	OfrontOPT_Node x = NIL, p = NIL;
	p = *par0;
	if (fctno <= 19) {
		if (fctno == 1 && p->typ != OfrontOPT_notyp) {
			if (p->typ->comp == 3) {
				OfrontOPB_err(65);
			}
			p->typ = OfrontOPT_notyp;
		} else if (fctno <= 12) {
			if (parno < 1) {
				OfrontOPB_err(65);
			}
		} else {
			if ((fctno == 13 || fctno == 14) && parno == 1) {
				OfrontOPB_BindNodes(19, OfrontOPT_notyp, &p, OfrontOPB_NewIntConst(1));
				p->subcl = fctno;
				p->right->typ = p->left->typ;
			} else if (fctno == 17 && parno == 1) {
				if (p->typ->comp == 3) {
					dim = 0;
					while (p->class == 4) {
						p = p->left;
						dim += 1;
					}
					OfrontOPB_BindNodes(12, OfrontOPT_linttyp, &p, OfrontOPB_NewIntConst(dim));
					p->subcl = 19;
				} else {
					p = OfrontOPB_NewIntConst(p->typ->n);
				}
			} else if (parno < 2) {
				OfrontOPB_err(65);
			}
		}
	} else if (fctno == 32) {
		if (parno == 1) {
			x = NIL;
			OfrontOPB_BindNodes(28, OfrontOPT_notyp, &x, OfrontOPB_NewIntConst(0));
			x->conval = OfrontOPT_NewConst();
			x->conval->intval = OfrontOPM_errpos;
			OfrontOPB_Construct(15, &p, x);
			p->conval = OfrontOPT_NewConst();
			p->conval->intval = OfrontOPM_errpos;
			OfrontOPB_Construct(20, &p, NIL);
			OfrontOPB_OptIf(&p);
			if (p == NIL) {
			} else if (p->class == 28) {
				OfrontOPB_err(99);
			} else {
				p->subcl = 32;
			}
		} else if (parno < 1) {
			OfrontOPB_err(65);
		}
	} else {
		if ((parno < 1 || fctno > 21 && parno < 2) || fctno == 31 && parno < 3) {
			OfrontOPB_err(65);
		}
	}
	*par0 = p;
}

/*----------------------------------------------------------------------------*/
static void OfrontOPB_DynArrParCheck (OfrontOPT_Struct ftyp, OfrontOPT_Struct atyp, BOOLEAN fvarpar)
{
	INTEGER f;
	f = atyp->comp;
	ftyp = ftyp->BaseTyp;
	atyp = atyp->BaseTyp;
	if (fvarpar && ftyp == OfrontOPT_bytetyp) {
		if (!__IN(f, 0x0c) || !__IN(atyp->form, 0x1e)) {
			OfrontOPB_err(-301);
		}
	} else if (__IN(f, 0x0c)) {
		if (ftyp->comp == 3) {
			OfrontOPB_DynArrParCheck(ftyp, atyp, fvarpar);
		} else if (ftyp != atyp) {
			if ((!fvarpar && ftyp->form == 13) && atyp->form == 13) {
				ftyp = ftyp->BaseTyp;
				atyp = atyp->BaseTyp;
				if (ftyp->comp == 4 && atyp->comp == 4) {
					while ((ftyp != atyp && atyp != NIL) && atyp != OfrontOPT_undftyp) {
						atyp = atyp->BaseTyp;
					}
					if (atyp == NIL) {
						OfrontOPB_err(113);
					}
				} else {
					OfrontOPB_err(66);
				}
			} else {
				OfrontOPB_err(66);
			}
		}
	} else {
		OfrontOPB_err(67);
	}
}

static void OfrontOPB_CheckReceiver (OfrontOPT_Node *x, OfrontOPT_Object fp)
{
	if (fp->typ->form == 13) {
		if ((*x)->class == 3) {
			*x = (*x)->left;
		} else {
			OfrontOPB_err(71);
		}
	}
}

void OfrontOPB_PrepCall (OfrontOPT_Node *x, OfrontOPT_Object *fpar)
{
	if ((*x)->obj != NIL && __IN((*x)->obj->mode, 0x22c0)) {
		*fpar = (*x)->obj->link;
		if ((*x)->obj->mode == 13) {
			OfrontOPB_CheckReceiver(&(*x)->left, *fpar);
			*fpar = (*fpar)->link;
		}
	} else if (((*x)->class != 8 && (*x)->typ != NIL) && (*x)->typ->form == 14) {
		*fpar = (*x)->typ->link;
	} else {
		OfrontOPB_err(121);
		*fpar = NIL;
		(*x)->typ = OfrontOPT_undftyp;
	}
}

/*----------------------------------------------------------------------------*/
void OfrontOPB_Param (OfrontOPT_Node ap, OfrontOPT_Object fp)
{
	OfrontOPT_Struct q = NIL;
	if (fp->typ->form != 0) {
		if (fp->mode == 2) {
			if (OfrontOPB_NotVar(ap)) {
				OfrontOPB_err(122);
			} else {
				OfrontOPB_CheckLeaf(ap, 0);
			}
			if (ap->readonly) {
				OfrontOPB_err(76);
			}
			if (fp->typ->comp == 3) {
				OfrontOPB_DynArrParCheck(fp->typ, ap->typ, 1);
			} else if (fp->typ->comp == 4 && ap->typ->comp == 4) {
				q = ap->typ;
				while ((q != fp->typ && q != NIL) && q != OfrontOPT_undftyp) {
					q = q->BaseTyp;
				}
				if (q == NIL) {
					OfrontOPB_err(111);
				}
			} else if (fp->typ == OfrontOPT_sysptrtyp && ap->typ->form == 13) {
			} else if (ap->typ != fp->typ && !(fp->typ->form == 1 && __IN(ap->typ->form, 0x18))) {
				OfrontOPB_err(123);
			} else if (fp->typ->form == 13 && ap->class == 5) {
				OfrontOPB_err(123);
			}
		} else if (fp->typ->comp == 3) {
			if (ap->class == 7 && ap->typ->form == 3) {
				OfrontOPB_CharToString(ap);
			}
			if (ap->typ->form == 10 && fp->typ->BaseTyp->form == 3) {
			} else if (ap->class >= 7) {
				OfrontOPB_err(59);
			} else {
				OfrontOPB_DynArrParCheck(fp->typ, ap->typ, 0);
			}
		} else {
			OfrontOPB_CheckAssign(fp->typ, ap);
		}
	}
}

/*----------------------------------------------------------------------------*/
void OfrontOPB_StaticLink (SHORTINT dlev)
{
	OfrontOPT_Object scope = NIL;
	scope = OfrontOPT_topScope;
	while (dlev > 0) {
		dlev -= 1;
		scope->link->conval->setval |= __SETOF(3);
		scope = scope->left;
	}
}

/*----------------------------------------------------------------------------*/
void OfrontOPB_Call (OfrontOPT_Node *x, OfrontOPT_Node apar, OfrontOPT_Object fp)
{
	OfrontOPT_Struct typ = NIL;
	OfrontOPT_Node p = NIL;
	SHORTINT lev;
	if ((*x)->class == 9) {
		typ = (*x)->typ;
		lev = (*x)->obj->mnolev;
		if (lev > 0) {
			OfrontOPB_StaticLink(OfrontOPT_topScope->mnolev - lev);
		}
		if ((*x)->obj->mode == 10) {
			OfrontOPB_err(121);
		}
	} else if ((*x)->class == 2 && (*x)->obj->mode == 13) {
		typ = (*x)->typ;
		(*x)->class = 9;
		p = (*x)->left;
		(*x)->left = NIL;
		p->link = apar;
		apar = p;
		fp = (*x)->obj->link;
	} else {
		typ = (*x)->typ->BaseTyp;
	}
	OfrontOPB_BindNodes(13, typ, &*x, apar);
	(*x)->obj = fp;
}

/*----------------------------------------------------------------------------*/
void OfrontOPB_Enter (OfrontOPT_Node *procdec, OfrontOPT_Node stat, OfrontOPT_Object proc)
{
	OfrontOPT_Node x = NIL;
	x = OfrontOPT_NewNode(18);
	x->typ = OfrontOPT_notyp;
	x->obj = proc;
	x->left = *procdec;
	x->right = stat;
	*procdec = x;
}

/*----------------------------------------------------------------------------*/
void OfrontOPB_Return (OfrontOPT_Node *x, OfrontOPT_Object proc)
{
	OfrontOPT_Node node = NIL;
	if (proc == NIL) {
		if (*x != NIL) {
			OfrontOPB_err(124);
		}
	} else {
		if (*x != NIL) {
			OfrontOPB_CheckAssign(proc->typ, *x);
		} else if (proc->typ != OfrontOPT_notyp) {
			OfrontOPB_err(124);
		}
	}
	node = OfrontOPT_NewNode(26);
	node->typ = OfrontOPT_notyp;
	node->obj = proc;
	node->left = *x;
	*x = node;
}

/*----------------------------------------------------------------------------*/
void OfrontOPB_Assign (OfrontOPT_Node *x, OfrontOPT_Node y)
{
	OfrontOPT_Node z = NIL;
	if ((*x)->class >= 7) {
		OfrontOPB_err(56);
	}
	OfrontOPB_CheckAssign((*x)->typ, y);
	if ((*x)->readonly) {
		OfrontOPB_err(76);
	}
	if ((*x)->typ->comp == 4) {
		if ((*x)->class == 5) {
			z = (*x)->left;
		} else {
			z = *x;
		}
		if (z->class == 3 && z->left->class == 5) {
			z->left = z->left->left;
		}
		if ((*x)->typ->strobj != NIL && (z->class == 3 || z->class == 1)) {
			OfrontOPB_BindNodes(6, (*x)->typ, &z, NIL);
			*x = z;
		}
	} else if ((((*x)->typ->comp == 2 && (*x)->typ->BaseTyp == OfrontOPT_chartyp) && y->typ->form == 10) && y->conval->intval2 == 1) {
		y->typ = OfrontOPT_chartyp;
		y->conval->intval = 0;
		OfrontOPB_Index(&*x, OfrontOPB_NewIntConst(0));
	}
	OfrontOPB_BindNodes(19, OfrontOPT_notyp, &*x, y);
	(*x)->subcl = 0;
}

/*----------------------------------------------------------------------------*/
void OfrontOPB_Inittd (OfrontOPT_Node *inittd, OfrontOPT_Node *last, OfrontOPT_Struct typ)
{
	OfrontOPT_Node node = NIL;
	node = OfrontOPT_NewNode(14);
	node->typ = typ;
	node->conval = OfrontOPT_NewConst();
	node->conval->intval = typ->txtpos;
	if (*inittd == NIL) {
		*inittd = node;
	} else {
		(*last)->link = node;
	}
	*last = node;
}

/*----------------------------------------------------------------------------*/

export void *OfrontOPB__init(void)
{
	__DEFMOD;
	__IMPORT(OfrontOPM__init);
	__IMPORT(OfrontOPS__init);
	__IMPORT(OfrontOPT__init);
	__REGMOD("OfrontOPB", 0);
/* BEGIN */
	__ENDMOD;
}
