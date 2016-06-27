/*  Ofront 1.2 -xtspkae */
#include "SYSTEM.h"
#include "OfrontOPB.h"
#include "OfrontOPM.h"
#include "OfrontOPS.h"
#include "OfrontOPT.h"

struct OfrontOPP__1 {
	LONGINT low, high;
};

typedef
	struct OfrontOPP__1 OfrontOPP_CaseTable[128];


static SHORTINT OfrontOPP_sym, OfrontOPP_level;
static INTEGER OfrontOPP_LoopLevel;
static OfrontOPT_Node OfrontOPP_TDinit, OfrontOPP_lastTDinit;
static INTEGER OfrontOPP_nofFwdPtr;
static OfrontOPT_Struct OfrontOPP_FwdPtr[64];

export LONGINT *OfrontOPP__1__typ;

static void OfrontOPP_ActualParameters (OfrontOPT_Node *aparlist, OfrontOPT_Object fpar);
static void OfrontOPP_ArrayType (OfrontOPT_Struct *typ, OfrontOPT_Struct *banned);
static void OfrontOPP_Block (OfrontOPT_Node *procdec, OfrontOPT_Node *statseq);
static void OfrontOPP_CaseLabelList (OfrontOPT_Node *lab, INTEGER LabelForm, INTEGER *n, OfrontOPP_CaseTable tab);
static void OfrontOPP_CheckMark (SHORTINT *vis);
static void OfrontOPP_CheckSym (INTEGER s);
static void OfrontOPP_CheckSysFlag (INTEGER *sysflag, INTEGER default_);
static void OfrontOPP_ConstExpression (OfrontOPT_Node *x);
static void OfrontOPP_Element (OfrontOPT_Node *x);
static void OfrontOPP_Expression (OfrontOPT_Node *x);
static BOOLEAN OfrontOPP_Extends (OfrontOPT_Struct x, OfrontOPT_Struct b);
static void OfrontOPP_Factor (OfrontOPT_Node *x);
static void OfrontOPP_FormalParameters (OfrontOPT_Object *firstPar, OfrontOPT_Struct *resTyp);
export void OfrontOPP_Module (OfrontOPT_Node *prog, SET opt);
static void OfrontOPP_PointerType (OfrontOPT_Struct *typ);
static void OfrontOPP_ProcedureDeclaration (OfrontOPT_Node *x);
static void OfrontOPP_Receiver (SHORTINT *mode, OfrontOPS_Name name, OfrontOPT_Struct *typ, OfrontOPT_Struct *rec);
static void OfrontOPP_RecordType (OfrontOPT_Struct *typ, OfrontOPT_Struct *banned);
static void OfrontOPP_Sets (OfrontOPT_Node *x);
static void OfrontOPP_SimpleExpression (OfrontOPT_Node *x);
static void OfrontOPP_StandProcCall (OfrontOPT_Node *x);
static void OfrontOPP_StatSeq (OfrontOPT_Node *stat);
static void OfrontOPP_Term (OfrontOPT_Node *x);
static void OfrontOPP_Type (OfrontOPT_Struct *typ, OfrontOPT_Struct *banned);
static void OfrontOPP_TypeDecl (OfrontOPT_Struct *typ, OfrontOPT_Struct *banned);
static void OfrontOPP_err (INTEGER n);
static void OfrontOPP_qualident (OfrontOPT_Object *id);
static void OfrontOPP_selector (OfrontOPT_Node *x);


/*============================================================================*/

static void OfrontOPP_err (INTEGER n)
{
	OfrontOPM_err(n);
}

static void OfrontOPP_CheckSym (INTEGER s)
{
	if ((int)OfrontOPP_sym == s) {
		OfrontOPS_Get(&OfrontOPP_sym);
	} else {
		OfrontOPM_err(s);
	}
}

static void OfrontOPP_qualident (OfrontOPT_Object *id)
{
	OfrontOPT_Object obj = NIL;
	SHORTINT lev;
	OfrontOPT_Find(&obj);
	OfrontOPS_Get(&OfrontOPP_sym);
	if ((OfrontOPP_sym == 18 && obj != NIL) && obj->mode == 11) {
		OfrontOPS_Get(&OfrontOPP_sym);
		if (OfrontOPP_sym == 38) {
			OfrontOPT_FindImport(obj, &obj);
			OfrontOPS_Get(&OfrontOPP_sym);
		} else {
			OfrontOPP_err(38);
			obj = NIL;
		}
	}
	if (obj == NIL) {
		OfrontOPP_err(0);
		obj = OfrontOPT_NewObj();
		obj->mode = 1;
		obj->typ = OfrontOPT_undftyp;
		obj->adr = 0;
	} else {
		lev = obj->mnolev;
		if (__IN(obj->mode, 0x06) && lev != OfrontOPP_level) {
			obj->leaf = 0;
			if (lev > 0) {
				OfrontOPB_StaticLink(OfrontOPP_level - lev);
			}
		}
	}
	*id = obj;
}

static void OfrontOPP_ConstExpression (OfrontOPT_Node *x)
{
	OfrontOPP_Expression(&*x);
	if ((*x)->class != 7) {
		OfrontOPP_err(50);
		*x = OfrontOPB_NewIntConst(1);
	}
}

static void OfrontOPP_CheckMark (SHORTINT *vis)
{
	OfrontOPS_Get(&OfrontOPP_sym);
	if (OfrontOPP_sym == 1 || OfrontOPP_sym == 7) {
		if (OfrontOPP_level > 0) {
			OfrontOPP_err(47);
		}
		if (OfrontOPP_sym == 1) {
			*vis = 1;
		} else {
			*vis = 2;
		}
		OfrontOPS_Get(&OfrontOPP_sym);
	} else {
		*vis = 0;
	}
}

static void OfrontOPP_CheckSysFlag (INTEGER *sysflag, INTEGER default_)
{
	OfrontOPT_Node x = NIL;
	LONGINT sf;
	if (OfrontOPP_sym == 31) {
		OfrontOPS_Get(&OfrontOPP_sym);
		if (!OfrontOPT_SYSimported) {
			OfrontOPP_err(135);
		}
		OfrontOPP_ConstExpression(&x);
		if (__IN(x->typ->form, 0x70)) {
			sf = x->conval->intval;
			if (sf < 0 || sf > 1) {
				OfrontOPP_err(220);
				sf = 0;
			}
		} else {
			OfrontOPP_err(51);
			sf = 0;
		}
		*sysflag = (int)sf;
		OfrontOPP_CheckSym(23);
	} else {
		*sysflag = default_;
	}
}

static void OfrontOPP_RecordType (OfrontOPT_Struct *typ, OfrontOPT_Struct *banned)
{
	OfrontOPT_Object fld = NIL, first = NIL, last = NIL, base = NIL;
	OfrontOPT_Struct ftyp = NIL;
	INTEGER sysflag;
	*typ = OfrontOPT_NewStr(15, 4);
	(*typ)->BaseTyp = NIL;
	OfrontOPP_CheckSysFlag(&sysflag, -1);
	if (OfrontOPP_sym == 30) {
		OfrontOPS_Get(&OfrontOPP_sym);
		if (OfrontOPP_sym == 38) {
			OfrontOPP_qualident(&base);
			if (base->mode == 5 && base->typ->comp == 4) {
				if (base->typ == *banned) {
					OfrontOPP_err(58);
				} else {
					base->typ->pvused = 1;
					(*typ)->BaseTyp = base->typ;
					(*typ)->extlev = base->typ->extlev + 1;
					(*typ)->sysflag = base->typ->sysflag;
				}
			} else {
				OfrontOPP_err(52);
			}
		} else {
			OfrontOPP_err(38);
		}
		OfrontOPP_CheckSym(22);
	}
	if (sysflag >= 0) {
		(*typ)->sysflag = sysflag;
	}
	OfrontOPT_OpenScope(0, NIL);
	first = NIL;
	last = NIL;
	for (;;) {
		if (OfrontOPP_sym == 38) {
			for (;;) {
				if (OfrontOPP_sym == 38) {
					if ((*typ)->BaseTyp != NIL) {
						OfrontOPT_FindField(OfrontOPS_name, (*typ)->BaseTyp, &fld);
						if (fld != NIL) {
							OfrontOPP_err(1);
						}
					}
					OfrontOPT_Insert(OfrontOPS_name, &fld);
					OfrontOPP_CheckMark(&fld->vis);
					fld->mode = 4;
					fld->link = NIL;
					fld->typ = OfrontOPT_undftyp;
					if (first == NIL) {
						first = fld;
					}
					if (last == NIL) {
						(*typ)->link = fld;
					} else {
						last->link = fld;
					}
					last = fld;
				} else {
					OfrontOPP_err(38);
				}
				if (OfrontOPP_sym == 19) {
					OfrontOPS_Get(&OfrontOPP_sym);
				} else if (OfrontOPP_sym == 38) {
					OfrontOPP_err(19);
				} else {
					break;
				}
			}
			OfrontOPP_CheckSym(20);
			OfrontOPP_Type(&ftyp, &*banned);
			ftyp->pvused = 1;
			if (ftyp->comp == 3) {
				ftyp = OfrontOPT_undftyp;
				OfrontOPP_err(88);
			}
			while (first != NIL) {
				first->typ = ftyp;
				first = first->link;
			}
		}
		if (OfrontOPP_sym == 39) {
			OfrontOPS_Get(&OfrontOPP_sym);
		} else if (OfrontOPP_sym == 38) {
			OfrontOPP_err(39);
		} else {
			break;
		}
	}
	OfrontOPT_CloseScope();
}

static void OfrontOPP_ArrayType (OfrontOPT_Struct *typ, OfrontOPT_Struct *banned)
{
	OfrontOPT_Node x = NIL;
	LONGINT n;
	INTEGER sysflag;
	OfrontOPP_CheckSysFlag(&sysflag, 0);
	if (OfrontOPP_sym == 25) {
		*typ = OfrontOPT_NewStr(15, 3);
		(*typ)->mno = 0;
		(*typ)->sysflag = sysflag;
		OfrontOPS_Get(&OfrontOPP_sym);
		OfrontOPP_Type(&(*typ)->BaseTyp, &*banned);
		(*typ)->BaseTyp->pvused = 1;
		if ((*typ)->BaseTyp->comp == 3) {
			(*typ)->n = (*typ)->BaseTyp->n + 1;
		} else {
			(*typ)->n = 0;
		}
	} else {
		*typ = OfrontOPT_NewStr(15, 2);
		(*typ)->sysflag = sysflag;
		OfrontOPP_ConstExpression(&x);
		if (__IN(x->typ->form, 0x70)) {
			n = x->conval->intval;
			if (n <= 0 || n > OfrontOPM_MaxIndex) {
				OfrontOPP_err(63);
				n = 1;
			}
		} else {
			OfrontOPP_err(51);
			n = 1;
		}
		(*typ)->n = n;
		if (OfrontOPP_sym == 25) {
			OfrontOPS_Get(&OfrontOPP_sym);
			OfrontOPP_Type(&(*typ)->BaseTyp, &*banned);
			(*typ)->BaseTyp->pvused = 1;
		} else if (OfrontOPP_sym == 19) {
			OfrontOPS_Get(&OfrontOPP_sym);
			if (OfrontOPP_sym != 25) {
				OfrontOPP_ArrayType(&(*typ)->BaseTyp, &*banned);
			}
		} else {
			OfrontOPP_err(35);
		}
		if ((*typ)->BaseTyp->comp == 3) {
			(*typ)->BaseTyp = OfrontOPT_undftyp;
			OfrontOPP_err(88);
		}
	}
}

static void OfrontOPP_PointerType (OfrontOPT_Struct *typ)
{
	OfrontOPT_Object id = NIL;
	*typ = OfrontOPT_NewStr(13, 1);
	OfrontOPP_CheckSysFlag(&(*typ)->sysflag, 0);
	OfrontOPP_CheckSym(28);
	if (OfrontOPP_sym == 38) {
		OfrontOPT_Find(&id);
		if (id == NIL) {
			if (OfrontOPP_nofFwdPtr < 64) {
				OfrontOPP_FwdPtr[__X(OfrontOPP_nofFwdPtr, 64)] = *typ;
				OfrontOPP_nofFwdPtr += 1;
			} else {
				OfrontOPP_err(224);
			}
			(*typ)->link = OfrontOPT_NewObj();
			__COPY(OfrontOPS_name, (*typ)->link->name, 32);
			(*typ)->BaseTyp = OfrontOPT_undftyp;
			OfrontOPS_Get(&OfrontOPP_sym);
		} else {
			OfrontOPP_qualident(&id);
			if (id->mode == 5) {
				if (__IN(id->typ->comp, 0x1c)) {
					(*typ)->BaseTyp = id->typ;
				} else {
					(*typ)->BaseTyp = OfrontOPT_undftyp;
					OfrontOPP_err(57);
				}
			} else {
				(*typ)->BaseTyp = OfrontOPT_undftyp;
				OfrontOPP_err(52);
			}
		}
	} else {
		OfrontOPP_Type(&(*typ)->BaseTyp, &OfrontOPT_notyp);
		if (!__IN((*typ)->BaseTyp->comp, 0x1c)) {
			(*typ)->BaseTyp = OfrontOPT_undftyp;
			OfrontOPP_err(57);
		}
	}
}

static void OfrontOPP_FormalParameters (OfrontOPT_Object *firstPar, OfrontOPT_Struct *resTyp)
{
	SHORTINT mode;
	OfrontOPT_Object par = NIL, first = NIL, last = NIL, res = NIL;
	OfrontOPT_Struct typ = NIL;
	first = NIL;
	last = *firstPar;
	if (OfrontOPP_sym == 38 || OfrontOPP_sym == 60) {
		for (;;) {
			if (OfrontOPP_sym == 60) {
				OfrontOPS_Get(&OfrontOPP_sym);
				mode = 2;
			} else {
				mode = 1;
			}
			for (;;) {
				if (OfrontOPP_sym == 38) {
					OfrontOPT_Insert(OfrontOPS_name, &par);
					OfrontOPS_Get(&OfrontOPP_sym);
					par->mode = mode;
					par->link = NIL;
					if (first == NIL) {
						first = par;
					}
					if (*firstPar == NIL) {
						*firstPar = par;
					} else {
						last->link = par;
					}
					last = par;
				} else {
					OfrontOPP_err(38);
				}
				if (OfrontOPP_sym == 19) {
					OfrontOPS_Get(&OfrontOPP_sym);
				} else if (OfrontOPP_sym == 38) {
					OfrontOPP_err(19);
				} else if (OfrontOPP_sym == 60) {
					OfrontOPP_err(19);
					OfrontOPS_Get(&OfrontOPP_sym);
				} else {
					break;
				}
			}
			OfrontOPP_CheckSym(20);
			OfrontOPP_Type(&typ, &OfrontOPT_notyp);
			if (mode == 1) {
				typ->pvused = 1;
			}
			while (first != NIL) {
				first->typ = typ;
				first = first->link;
			}
			if (OfrontOPP_sym == 39) {
				OfrontOPS_Get(&OfrontOPP_sym);
			} else if (OfrontOPP_sym == 38) {
				OfrontOPP_err(39);
			} else {
				break;
			}
		}
	}
	OfrontOPP_CheckSym(22);
	if (OfrontOPP_sym == 20) {
		OfrontOPS_Get(&OfrontOPP_sym);
		*resTyp = OfrontOPT_undftyp;
		if (OfrontOPP_sym == 38) {
			OfrontOPP_qualident(&res);
			if (res->mode == 5) {
				if (res->typ->form < 15) {
					*resTyp = res->typ;
				} else {
					OfrontOPP_err(54);
				}
			} else {
				OfrontOPP_err(52);
			}
		} else {
			OfrontOPP_err(38);
		}
	} else {
		*resTyp = OfrontOPT_notyp;
	}
}

static void OfrontOPP_TypeDecl (OfrontOPT_Struct *typ, OfrontOPT_Struct *banned)
{
	OfrontOPT_Object id = NIL;
	*typ = OfrontOPT_undftyp;
	if (OfrontOPP_sym < 30) {
		OfrontOPP_err(12);
		do {
			OfrontOPS_Get(&OfrontOPP_sym);
		} while (!(OfrontOPP_sym >= 30));
	}
	if (OfrontOPP_sym == 38) {
		OfrontOPP_qualident(&id);
		if (id->mode == 5) {
			if (id->typ != *banned) {
				*typ = id->typ;
			} else {
				OfrontOPP_err(58);
			}
		} else {
			OfrontOPP_err(52);
		}
	} else if (OfrontOPP_sym == 54) {
		OfrontOPS_Get(&OfrontOPP_sym);
		OfrontOPP_ArrayType(&*typ, &*banned);
	} else if (OfrontOPP_sym == 55) {
		OfrontOPS_Get(&OfrontOPP_sym);
		OfrontOPP_RecordType(&*typ, &*banned);
		OfrontOPB_Inittd(&OfrontOPP_TDinit, &OfrontOPP_lastTDinit, *typ);
		OfrontOPP_CheckSym(41);
	} else if (OfrontOPP_sym == 56) {
		OfrontOPS_Get(&OfrontOPP_sym);
		OfrontOPP_PointerType(&*typ);
	} else if (OfrontOPP_sym == 61) {
		OfrontOPS_Get(&OfrontOPP_sym);
		*typ = OfrontOPT_NewStr(14, 1);
		OfrontOPP_CheckSysFlag(&(*typ)->sysflag, 0);
		if (OfrontOPP_sym == 30) {
			OfrontOPS_Get(&OfrontOPP_sym);
			OfrontOPT_OpenScope(OfrontOPP_level, NIL);
			OfrontOPP_FormalParameters(&(*typ)->link, &(*typ)->BaseTyp);
			OfrontOPT_CloseScope();
		} else {
			(*typ)->BaseTyp = OfrontOPT_notyp;
			(*typ)->link = NIL;
		}
	} else {
		OfrontOPP_err(12);
	}
	for (;;) {
		if ((OfrontOPP_sym >= 39 && OfrontOPP_sym <= 42 || OfrontOPP_sym == 22) || OfrontOPP_sym == 64) {
			break;
		}
		OfrontOPP_err(15);
		if (OfrontOPP_sym == 38) {
			break;
		}
		OfrontOPS_Get(&OfrontOPP_sym);
	}
}

static void OfrontOPP_Type (OfrontOPT_Struct *typ, OfrontOPT_Struct *banned)
{
	OfrontOPP_TypeDecl(&*typ, &*banned);
	if (((*typ)->form == 13 && (*typ)->BaseTyp == OfrontOPT_undftyp) && (*typ)->strobj == NIL) {
		OfrontOPP_err(0);
	}
}

static void OfrontOPP_selector (OfrontOPT_Node *x)
{
	OfrontOPT_Object obj = NIL, proc = NIL;
	OfrontOPT_Node y = NIL;
	OfrontOPT_Struct typ = NIL;
	OfrontOPS_Name name;
	for (;;) {
		if (OfrontOPP_sym == 31) {
			OfrontOPS_Get(&OfrontOPP_sym);
			for (;;) {
				if ((*x)->typ != NIL && (*x)->typ->form == 13) {
					OfrontOPB_DeRef(&*x);
				}
				OfrontOPP_Expression(&y);
				OfrontOPB_Index(&*x, y);
				if (OfrontOPP_sym == 19) {
					OfrontOPS_Get(&OfrontOPP_sym);
				} else {
					break;
				}
			}
			OfrontOPP_CheckSym(23);
		} else if (OfrontOPP_sym == 18) {
			OfrontOPS_Get(&OfrontOPP_sym);
			if (OfrontOPP_sym == 38) {
				__MOVE(OfrontOPS_name, name, 32);
				OfrontOPS_Get(&OfrontOPP_sym);
				if ((*x)->typ != NIL) {
					if ((*x)->typ->form == 13) {
						OfrontOPB_DeRef(&*x);
					}
					if ((*x)->typ->comp == 4) {
						OfrontOPT_FindField(name, (*x)->typ, &obj);
						OfrontOPB_Field(&*x, obj);
						if (obj != NIL && obj->mode == 13) {
							if (OfrontOPP_sym == 17) {
								OfrontOPS_Get(&OfrontOPP_sym);
								y = (*x)->left;
								if (y->class == 3) {
									y = y->left;
								}
								if (y->obj != NIL) {
									proc = OfrontOPT_topScope;
									while (proc->link != NIL && proc->link->mode != 13) {
										proc = proc->left;
									}
									if (proc->link == NIL || proc->link->link != y->obj) {
										OfrontOPP_err(75);
									}
									typ = y->obj->typ;
									if (typ->form == 13) {
										typ = typ->BaseTyp;
									}
									OfrontOPT_FindField((*x)->obj->name, typ->BaseTyp, &proc);
									if (proc != NIL) {
										(*x)->subcl = 1;
									} else {
										OfrontOPP_err(74);
									}
								} else {
									OfrontOPP_err(75);
								}
							}
							if (obj->typ != OfrontOPT_notyp && OfrontOPP_sym != 30) {
								OfrontOPP_err(30);
							}
						}
					} else {
						OfrontOPP_err(53);
					}
				} else {
					OfrontOPP_err(52);
				}
			} else {
				OfrontOPP_err(38);
			}
		} else if (OfrontOPP_sym == 17) {
			OfrontOPS_Get(&OfrontOPP_sym);
			OfrontOPB_DeRef(&*x);
		} else if (((OfrontOPP_sym == 30 && (*x)->class < 7) && (*x)->typ->form != 14) && ((*x)->obj == NIL || (*x)->obj->mode != 13)) {
			OfrontOPS_Get(&OfrontOPP_sym);
			if (OfrontOPP_sym == 38) {
				OfrontOPP_qualident(&obj);
				if (obj->mode == 5) {
					OfrontOPB_TypTest(&*x, obj, 1);
				} else {
					OfrontOPP_err(52);
				}
			} else {
				OfrontOPP_err(38);
			}
			OfrontOPP_CheckSym(22);
		} else {
			break;
		}
	}
}

static void OfrontOPP_ActualParameters (OfrontOPT_Node *aparlist, OfrontOPT_Object fpar)
{
	OfrontOPT_Node apar = NIL, last = NIL;
	*aparlist = NIL;
	last = NIL;
	if (OfrontOPP_sym != 22) {
		for (;;) {
			OfrontOPP_Expression(&apar);
			if (fpar != NIL) {
				OfrontOPB_Param(apar, fpar);
				OfrontOPB_Link(&*aparlist, &last, apar);
				fpar = fpar->link;
			} else {
				OfrontOPP_err(64);
			}
			if (OfrontOPP_sym == 19) {
				OfrontOPS_Get(&OfrontOPP_sym);
			} else if (30 <= OfrontOPP_sym && OfrontOPP_sym <= 38) {
				OfrontOPP_err(19);
			} else {
				break;
			}
		}
	}
	if (fpar != NIL) {
		OfrontOPP_err(65);
	}
}

static void OfrontOPP_StandProcCall (OfrontOPT_Node *x)
{
	OfrontOPT_Node y = NIL;
	SHORTINT m;
	INTEGER n;
	m = (int)(*x)->obj->adr;
	n = 0;
	if (OfrontOPP_sym == 30) {
		OfrontOPS_Get(&OfrontOPP_sym);
		if (OfrontOPP_sym != 22) {
			for (;;) {
				if (n == 0) {
					OfrontOPP_Expression(&*x);
					OfrontOPB_StPar0(&*x, m);
					n = 1;
				} else if (n == 1) {
					OfrontOPP_Expression(&y);
					OfrontOPB_StPar1(&*x, y, m);
					n = 2;
				} else {
					OfrontOPP_Expression(&y);
					OfrontOPB_StParN(&*x, y, m, n);
					n += 1;
				}
				if (OfrontOPP_sym == 19) {
					OfrontOPS_Get(&OfrontOPP_sym);
				} else if (30 <= OfrontOPP_sym && OfrontOPP_sym <= 38) {
					OfrontOPP_err(19);
				} else {
					break;
				}
			}
			OfrontOPP_CheckSym(22);
		} else {
			OfrontOPS_Get(&OfrontOPP_sym);
		}
		OfrontOPB_StFct(&*x, m, n);
	} else {
		OfrontOPP_err(30);
	}
	if (OfrontOPP_level > 0 && (m == 1 || m == 30)) {
		OfrontOPT_topScope->link->leaf = 0;
	}
}

static void OfrontOPP_Element (OfrontOPT_Node *x)
{
	OfrontOPT_Node y = NIL;
	OfrontOPP_Expression(&*x);
	if (OfrontOPP_sym == 21) {
		OfrontOPS_Get(&OfrontOPP_sym);
		OfrontOPP_Expression(&y);
		OfrontOPB_SetRange(&*x, y);
	} else {
		OfrontOPB_SetElem(&*x);
	}
}

static void OfrontOPP_Sets (OfrontOPT_Node *x)
{
	OfrontOPT_Node y = NIL;
	if (OfrontOPP_sym != 24) {
		OfrontOPP_Element(&*x);
		for (;;) {
			if (OfrontOPP_sym == 19) {
				OfrontOPS_Get(&OfrontOPP_sym);
			} else if (30 <= OfrontOPP_sym && OfrontOPP_sym <= 38) {
				OfrontOPP_err(19);
			} else {
				break;
			}
			OfrontOPP_Element(&y);
			OfrontOPB_Op(6, &*x, y);
		}
	} else {
		*x = OfrontOPB_EmptySet();
	}
	OfrontOPP_CheckSym(24);
}

static void OfrontOPP_Factor (OfrontOPT_Node *x)
{
	OfrontOPT_Object fpar = NIL, id = NIL;
	OfrontOPT_Node apar = NIL;
	if (OfrontOPP_sym < 30) {
		OfrontOPP_err(13);
		do {
			OfrontOPS_Get(&OfrontOPP_sym);
		} while (!(OfrontOPP_sym >= 30));
	}
	if (OfrontOPP_sym == 38) {
		OfrontOPP_qualident(&id);
		*x = OfrontOPB_NewLeaf(id);
		OfrontOPP_selector(&*x);
		if ((*x)->class == 9 && (*x)->obj->mode == 8) {
			OfrontOPP_StandProcCall(&*x);
		} else if (OfrontOPP_sym == 30) {
			OfrontOPS_Get(&OfrontOPP_sym);
			OfrontOPB_PrepCall(&*x, &fpar);
			OfrontOPP_ActualParameters(&apar, fpar);
			OfrontOPB_Call(&*x, apar, fpar);
			OfrontOPP_CheckSym(22);
			if (OfrontOPP_level > 0) {
				OfrontOPT_topScope->link->leaf = 0;
			}
		}
	} else if (OfrontOPP_sym == 35) {
		switch (OfrontOPS_numtyp) {
			case 1: 
				*x = OfrontOPB_NewIntConst(OfrontOPS_intval);
				(*x)->typ = OfrontOPT_chartyp;
				break;
			case 2: 
				*x = OfrontOPB_NewIntConst(OfrontOPS_intval);
				break;
			case 3: 
				*x = OfrontOPB_NewRealConst(OfrontOPS_realval, OfrontOPT_realtyp);
				break;
			case 4: 
				*x = OfrontOPB_NewRealConst(OfrontOPS_lrlval, OfrontOPT_lrltyp);
				break;
			default: __CASECHK;
		}
		OfrontOPS_Get(&OfrontOPP_sym);
	} else if (OfrontOPP_sym == 37) {
		*x = OfrontOPB_NewString(OfrontOPS_str, OfrontOPS_intval);
		OfrontOPS_Get(&OfrontOPP_sym);
	} else if (OfrontOPP_sym == 36) {
		*x = OfrontOPB_Nil();
		OfrontOPS_Get(&OfrontOPP_sym);
	} else if (OfrontOPP_sym == 30) {
		OfrontOPS_Get(&OfrontOPP_sym);
		OfrontOPP_Expression(&*x);
		OfrontOPP_CheckSym(22);
	} else if (OfrontOPP_sym == 31) {
		OfrontOPS_Get(&OfrontOPP_sym);
		OfrontOPP_err(30);
		OfrontOPP_Expression(&*x);
		OfrontOPP_CheckSym(22);
	} else if (OfrontOPP_sym == 32) {
		OfrontOPS_Get(&OfrontOPP_sym);
		OfrontOPP_Sets(&*x);
	} else if (OfrontOPP_sym == 33) {
		OfrontOPS_Get(&OfrontOPP_sym);
		OfrontOPP_Factor(&*x);
		OfrontOPB_MOp(33, &*x);
	} else {
		OfrontOPP_err(13);
		OfrontOPS_Get(&OfrontOPP_sym);
		*x = NIL;
	}
	if (*x == NIL) {
		*x = OfrontOPB_NewIntConst(1);
		(*x)->typ = OfrontOPT_undftyp;
	}
}

static void OfrontOPP_Term (OfrontOPT_Node *x)
{
	OfrontOPT_Node y = NIL;
	SHORTINT mulop;
	OfrontOPP_Factor(&*x);
	while (1 <= OfrontOPP_sym && OfrontOPP_sym <= 5) {
		mulop = OfrontOPP_sym;
		OfrontOPS_Get(&OfrontOPP_sym);
		OfrontOPP_Factor(&y);
		OfrontOPB_Op(mulop, &*x, y);
	}
}

static void OfrontOPP_SimpleExpression (OfrontOPT_Node *x)
{
	OfrontOPT_Node y = NIL;
	SHORTINT addop;
	if (OfrontOPP_sym == 7) {
		OfrontOPS_Get(&OfrontOPP_sym);
		OfrontOPP_Term(&*x);
		OfrontOPB_MOp(7, &*x);
	} else if (OfrontOPP_sym == 6) {
		OfrontOPS_Get(&OfrontOPP_sym);
		OfrontOPP_Term(&*x);
		OfrontOPB_MOp(6, &*x);
	} else {
		OfrontOPP_Term(&*x);
	}
	while (6 <= OfrontOPP_sym && OfrontOPP_sym <= 8) {
		addop = OfrontOPP_sym;
		OfrontOPS_Get(&OfrontOPP_sym);
		OfrontOPP_Term(&y);
		OfrontOPB_Op(addop, &*x, y);
	}
}

static void OfrontOPP_Expression (OfrontOPT_Node *x)
{
	OfrontOPT_Node y = NIL;
	OfrontOPT_Object obj = NIL;
	SHORTINT relation;
	OfrontOPP_SimpleExpression(&*x);
	if (9 <= OfrontOPP_sym && OfrontOPP_sym <= 14) {
		relation = OfrontOPP_sym;
		OfrontOPS_Get(&OfrontOPP_sym);
		OfrontOPP_SimpleExpression(&y);
		OfrontOPB_Op(relation, &*x, y);
	} else if (OfrontOPP_sym == 15) {
		OfrontOPS_Get(&OfrontOPP_sym);
		OfrontOPP_SimpleExpression(&y);
		OfrontOPB_In(&*x, y);
	} else if (OfrontOPP_sym == 16) {
		OfrontOPS_Get(&OfrontOPP_sym);
		if (OfrontOPP_sym == 38) {
			OfrontOPP_qualident(&obj);
			if (obj->mode == 5) {
				OfrontOPB_TypTest(&*x, obj, 0);
			} else {
				OfrontOPP_err(52);
			}
		} else {
			OfrontOPP_err(38);
		}
	}
}

static void OfrontOPP_Receiver (SHORTINT *mode, OfrontOPS_Name name, OfrontOPT_Struct *typ, OfrontOPT_Struct *rec)
{
	OfrontOPT_Object obj = NIL;
	*typ = OfrontOPT_undftyp;
	*rec = NIL;
	if (OfrontOPP_sym == 60) {
		OfrontOPS_Get(&OfrontOPP_sym);
		*mode = 2;
	} else {
		*mode = 1;
	}
	__MOVE(OfrontOPS_name, name, 32);
	OfrontOPP_CheckSym(38);
	OfrontOPP_CheckSym(20);
	if (OfrontOPP_sym == 38) {
		OfrontOPT_Find(&obj);
		OfrontOPS_Get(&OfrontOPP_sym);
		if (obj == NIL) {
			OfrontOPP_err(0);
		} else if (obj->mode != 5) {
			OfrontOPP_err(72);
		} else {
			*typ = obj->typ;
			*rec = *typ;
			if ((*rec)->form == 13) {
				*rec = (*rec)->BaseTyp;
			}
			if (!((*mode == 1 && (*typ)->form == 13) && (*rec)->comp == 4 || *mode == 2 && (*typ)->comp == 4)) {
				OfrontOPP_err(70);
				*rec = NIL;
			}
			if (*rec != NIL && (*rec)->mno != OfrontOPP_level) {
				OfrontOPP_err(72);
				*rec = NIL;
			}
		}
	} else {
		OfrontOPP_err(38);
	}
	OfrontOPP_CheckSym(22);
	if (*rec == NIL) {
		*rec = OfrontOPT_NewStr(15, 4);
		(*rec)->BaseTyp = NIL;
	}
}

static BOOLEAN OfrontOPP_Extends (OfrontOPT_Struct x, OfrontOPT_Struct b)
{
	if (b->form == 13 && x->form == 13) {
		b = b->BaseTyp;
		x = x->BaseTyp;
	}
	if (b->comp == 4 && x->comp == 4) {
		do {
			x = x->BaseTyp;
		} while (!(x == NIL || x == b));
	}
	return x == b;
}

static struct ProcedureDeclaration__16 {
	OfrontOPT_Node *x;
	OfrontOPT_Object *proc, *fwd;
	OfrontOPS_Name *name;
	SHORTINT *mode, *vis;
	BOOLEAN *forward;
	struct ProcedureDeclaration__16 *lnk;
} *ProcedureDeclaration__16_s;

static void Body__17 (void);
static void GetCode__19 (void);
static void GetParams__21 (void);
static void TProcDecl__23 (void);

static void GetCode__19 (void)
{
	OfrontOPT_ConstExt ext = NIL;
	INTEGER n;
	LONGINT c;
	ext = OfrontOPT_NewExt();
	(*ProcedureDeclaration__16_s->proc)->conval->ext = ext;
	n = 0;
	if (OfrontOPP_sym == 37) {
		while (OfrontOPS_str[__X(n, 256)] != 0x00) {
			(*ext)[__X(n + 1, 256)] = OfrontOPS_str[__X(n, 256)];
			n += 1;
		}
		(*ext)[0] = (CHAR)n;
		OfrontOPS_Get(&OfrontOPP_sym);
	} else {
		for (;;) {
			if (OfrontOPP_sym == 35) {
				c = OfrontOPS_intval;
				n += 1;
				if ((c < 0 || c > 255) || n == 256) {
					OfrontOPP_err(64);
					c = 1;
					n = 1;
				}
				OfrontOPS_Get(&OfrontOPP_sym);
				(*ext)[__X(n, 256)] = (CHAR)c;
			}
			if (OfrontOPP_sym == 19) {
				OfrontOPS_Get(&OfrontOPP_sym);
			} else if (OfrontOPP_sym == 35) {
				OfrontOPP_err(19);
			} else {
				(*ext)[0] = (CHAR)n;
				break;
			}
		}
	}
	(*ProcedureDeclaration__16_s->proc)->conval->setval |= __SETOF(1);
}

static void GetParams__21 (void)
{
	(*ProcedureDeclaration__16_s->proc)->vis = *ProcedureDeclaration__16_s->vis;
	(*ProcedureDeclaration__16_s->proc)->mode = *ProcedureDeclaration__16_s->mode;
	(*ProcedureDeclaration__16_s->proc)->typ = OfrontOPT_notyp;
	(*ProcedureDeclaration__16_s->proc)->conval = OfrontOPT_NewConst();
	(*ProcedureDeclaration__16_s->proc)->conval->setval = 0x0;
	if (OfrontOPP_sym == 30) {
		OfrontOPS_Get(&OfrontOPP_sym);
		OfrontOPP_FormalParameters(&(*ProcedureDeclaration__16_s->proc)->link, &(*ProcedureDeclaration__16_s->proc)->typ);
	}
	if (*ProcedureDeclaration__16_s->fwd != NIL) {
		OfrontOPB_CheckParameters((*ProcedureDeclaration__16_s->proc)->link, (*ProcedureDeclaration__16_s->fwd)->link, 1);
		if ((*ProcedureDeclaration__16_s->proc)->typ != (*ProcedureDeclaration__16_s->fwd)->typ) {
			OfrontOPP_err(117);
		}
		*ProcedureDeclaration__16_s->proc = *ProcedureDeclaration__16_s->fwd;
		OfrontOPT_topScope = (*ProcedureDeclaration__16_s->proc)->scope;
		if (*ProcedureDeclaration__16_s->mode == 10) {
			(*ProcedureDeclaration__16_s->proc)->mode = 10;
		}
	}
}

static void Body__17 (void)
{
	OfrontOPT_Node procdec = NIL, statseq = NIL;
	LONGINT c;
	c = OfrontOPM_errpos;
	(*ProcedureDeclaration__16_s->proc)->conval->setval |= __SETOF(1);
	OfrontOPP_CheckSym(39);
	OfrontOPP_Block(&procdec, &statseq);
	OfrontOPB_Enter(&procdec, statseq, *ProcedureDeclaration__16_s->proc);
	*ProcedureDeclaration__16_s->x = procdec;
	(*ProcedureDeclaration__16_s->x)->conval = OfrontOPT_NewConst();
	(*ProcedureDeclaration__16_s->x)->conval->intval = c;
	if (OfrontOPP_sym == 38) {
		if (__STRCMP(OfrontOPS_name, (*ProcedureDeclaration__16_s->proc)->name) != 0) {
			OfrontOPP_err(4);
		}
		OfrontOPS_Get(&OfrontOPP_sym);
	} else {
		OfrontOPP_err(38);
	}
}

static void TProcDecl__23 (void)
{
	OfrontOPT_Object baseProc = NIL;
	OfrontOPT_Struct objTyp = NIL, recTyp = NIL;
	SHORTINT objMode;
	OfrontOPS_Name objName;
	OfrontOPS_Get(&OfrontOPP_sym);
	*ProcedureDeclaration__16_s->mode = 13;
	if (OfrontOPP_level > 0) {
		OfrontOPP_err(73);
	}
	OfrontOPP_Receiver(&objMode, objName, &objTyp, &recTyp);
	if (OfrontOPP_sym == 38) {
		__MOVE(OfrontOPS_name, *ProcedureDeclaration__16_s->name, 32);
		OfrontOPP_CheckMark(&*ProcedureDeclaration__16_s->vis);
		OfrontOPT_FindField(*ProcedureDeclaration__16_s->name, recTyp, &*ProcedureDeclaration__16_s->fwd);
		OfrontOPT_FindField(*ProcedureDeclaration__16_s->name, recTyp->BaseTyp, &baseProc);
		if (baseProc != NIL && baseProc->mode != 13) {
			baseProc = NIL;
		}
		if (*ProcedureDeclaration__16_s->fwd == baseProc) {
			*ProcedureDeclaration__16_s->fwd = NIL;
		}
		if (*ProcedureDeclaration__16_s->fwd != NIL && (*ProcedureDeclaration__16_s->fwd)->mnolev != OfrontOPP_level) {
			*ProcedureDeclaration__16_s->fwd = NIL;
		}
		if ((*ProcedureDeclaration__16_s->fwd != NIL && (*ProcedureDeclaration__16_s->fwd)->mode == 13) && !__IN(1, (*ProcedureDeclaration__16_s->fwd)->conval->setval)) {
			*ProcedureDeclaration__16_s->proc = OfrontOPT_NewObj();
			(*ProcedureDeclaration__16_s->proc)->leaf = 1;
			if ((*ProcedureDeclaration__16_s->fwd)->vis != *ProcedureDeclaration__16_s->vis) {
				OfrontOPP_err(118);
			}
		} else {
			if (*ProcedureDeclaration__16_s->fwd != NIL) {
				OfrontOPP_err(1);
				*ProcedureDeclaration__16_s->fwd = NIL;
			}
			OfrontOPT_OpenScope(0, NIL);
			OfrontOPT_topScope->right = recTyp->link;
			OfrontOPT_Insert(*ProcedureDeclaration__16_s->name, &*ProcedureDeclaration__16_s->proc);
			recTyp->link = OfrontOPT_topScope->right;
			OfrontOPT_CloseScope();
		}
		OfrontOPP_level += 1;
		OfrontOPT_OpenScope(OfrontOPP_level, *ProcedureDeclaration__16_s->proc);
		OfrontOPT_Insert(objName, &(*ProcedureDeclaration__16_s->proc)->link);
		(*ProcedureDeclaration__16_s->proc)->link->mode = objMode;
		(*ProcedureDeclaration__16_s->proc)->link->typ = objTyp;
		GetParams__21();
		if (baseProc != NIL) {
			if (objMode != baseProc->link->mode || !OfrontOPP_Extends(objTyp, baseProc->link->typ)) {
				OfrontOPP_err(115);
			}
			OfrontOPB_CheckParameters((*ProcedureDeclaration__16_s->proc)->link->link, baseProc->link->link, 0);
			if ((*ProcedureDeclaration__16_s->proc)->typ != baseProc->typ) {
				OfrontOPP_err(117);
			}
			if (((baseProc->vis == 1 && (*ProcedureDeclaration__16_s->proc)->vis == 0) && recTyp->strobj != NIL) && recTyp->strobj->vis == 1) {
				OfrontOPP_err(109);
			}
			(*ProcedureDeclaration__16_s->proc)->conval->setval |= __SETOF(2);
		}
		if (!*ProcedureDeclaration__16_s->forward) {
			Body__17();
		}
		OfrontOPP_level -= 1;
		OfrontOPT_CloseScope();
	} else {
		OfrontOPP_err(38);
	}
}

static void OfrontOPP_ProcedureDeclaration (OfrontOPT_Node *x)
{
	OfrontOPT_Object proc = NIL, fwd = NIL;
	OfrontOPS_Name name;
	SHORTINT mode, vis;
	BOOLEAN forward;
	struct ProcedureDeclaration__16 _s;
	_s.x = x;
	_s.proc = &proc;
	_s.fwd = &fwd;
	_s.name = (void*)name;
	_s.mode = &mode;
	_s.vis = &vis;
	_s.forward = &forward;
	_s.lnk = ProcedureDeclaration__16_s;
	ProcedureDeclaration__16_s = &_s;
	proc = NIL;
	forward = 0;
	*x = NIL;
	mode = 6;
	if (OfrontOPP_sym != 38 && OfrontOPP_sym != 30) {
		if (OfrontOPP_sym == 1) {
		} else if (OfrontOPP_sym == 17) {
			forward = 1;
		} else if (OfrontOPP_sym == 6) {
			mode = 10;
		} else if (OfrontOPP_sym == 7) {
			mode = 9;
		} else {
			OfrontOPP_err(38);
		}
		if (__IN(mode, 0x0600) && !OfrontOPT_SYSimported) {
			OfrontOPP_err(135);
		}
		OfrontOPS_Get(&OfrontOPP_sym);
	}
	if (OfrontOPP_sym == 30) {
		TProcDecl__23();
	} else if (OfrontOPP_sym == 38) {
		OfrontOPT_Find(&fwd);
		__MOVE(OfrontOPS_name, name, 32);
		OfrontOPP_CheckMark(&vis);
		if (vis != 0 && mode == 6) {
			mode = 7;
		}
		if (fwd != NIL && (fwd->mnolev != OfrontOPP_level || fwd->mode == 8)) {
			fwd = NIL;
		}
		if ((fwd != NIL && __IN(fwd->mode, 0xc0)) && !__IN(1, fwd->conval->setval)) {
			proc = OfrontOPT_NewObj();
			proc->leaf = 1;
			if (fwd->vis != vis) {
				OfrontOPP_err(118);
			}
		} else {
			if (fwd != NIL) {
				OfrontOPP_err(1);
				fwd = NIL;
			}
			OfrontOPT_Insert(name, &proc);
		}
		if (mode != 6 && OfrontOPP_level > 0) {
			OfrontOPP_err(73);
		}
		OfrontOPP_level += 1;
		OfrontOPT_OpenScope(OfrontOPP_level, proc);
		proc->link = NIL;
		GetParams__21();
		if (mode == 9) {
			GetCode__19();
		} else if (!forward) {
			Body__17();
		}
		OfrontOPP_level -= 1;
		OfrontOPT_CloseScope();
	} else {
		OfrontOPP_err(38);
	}
	ProcedureDeclaration__16_s = _s.lnk;
}

static void OfrontOPP_CaseLabelList (OfrontOPT_Node *lab, INTEGER LabelForm, INTEGER *n, OfrontOPP_CaseTable tab)
{
	OfrontOPT_Node x = NIL, y = NIL, lastlab = NIL;
	INTEGER i, f;
	LONGINT xval, yval;
	*lab = NIL;
	lastlab = NIL;
	for (;;) {
		OfrontOPP_ConstExpression(&x);
		f = x->typ->form;
		if (__IN(f, 0x78)) {
			xval = x->conval->intval;
		} else {
			OfrontOPP_err(61);
			xval = 1;
		}
		if (__IN(f, 0x70)) {
			if (LabelForm < f) {
				OfrontOPP_err(60);
			}
		} else if (LabelForm != f) {
			OfrontOPP_err(60);
		}
		if (OfrontOPP_sym == 21) {
			OfrontOPS_Get(&OfrontOPP_sym);
			OfrontOPP_ConstExpression(&y);
			yval = y->conval->intval;
			if ((int)y->typ->form != f && !(__IN(f, 0x70) && __IN(y->typ->form, 0x70))) {
				OfrontOPP_err(60);
			}
			if (yval < xval) {
				OfrontOPP_err(63);
				yval = xval;
			}
		} else {
			yval = xval;
		}
		x->conval->intval2 = yval;
		i = *n;
		if (i < 128) {
			for (;;) {
				if (i == 0) {
					break;
				}
				if (tab[__X(i - 1, 128)].low <= yval) {
					if (tab[__X(i - 1, 128)].high >= xval) {
						OfrontOPP_err(62);
					}
					break;
				}
				tab[__X(i, 128)] = tab[__X(i - 1, 128)];
				i -= 1;
			}
			tab[__X(i, 128)].low = xval;
			tab[__X(i, 128)].high = yval;
			*n += 1;
		} else {
			OfrontOPP_err(213);
		}
		OfrontOPB_Link(&*lab, &lastlab, x);
		if (OfrontOPP_sym == 19) {
			OfrontOPS_Get(&OfrontOPP_sym);
		} else if (OfrontOPP_sym == 35 || OfrontOPP_sym == 38) {
			OfrontOPP_err(19);
		} else {
			break;
		}
	}
}

static struct StatSeq__30 {
	LONGINT *pos;
	struct StatSeq__30 *lnk;
} *StatSeq__30_s;

static void CasePart__31 (OfrontOPT_Node *x);
static void CheckBool__33 (OfrontOPT_Node *x);
static void SetPos__35 (OfrontOPT_Node x);

static void CasePart__31 (OfrontOPT_Node *x)
{
	INTEGER n;
	LONGINT low, high;
	BOOLEAN e;
	OfrontOPP_CaseTable tab;
	OfrontOPT_Node cases = NIL, lab = NIL, y = NIL, lastcase = NIL;
	OfrontOPP_Expression(&*x);
	*StatSeq__30_s->pos = OfrontOPM_errpos;
	if ((*x)->class == 8 || (*x)->class == 9) {
		OfrontOPP_err(126);
	} else if (!__IN((*x)->typ->form, 0x78)) {
		OfrontOPP_err(125);
	}
	OfrontOPP_CheckSym(25);
	cases = NIL;
	lastcase = NIL;
	n = 0;
	for (;;) {
		if (OfrontOPP_sym < 40) {
			OfrontOPP_CaseLabelList(&lab, (*x)->typ->form, &n, tab);
			OfrontOPP_CheckSym(20);
			OfrontOPP_StatSeq(&y);
			OfrontOPB_Construct(17, &lab, y);
			OfrontOPB_Link(&cases, &lastcase, lab);
		}
		if (OfrontOPP_sym == 40) {
			OfrontOPS_Get(&OfrontOPP_sym);
		} else {
			break;
		}
	}
	if (n > 0) {
		low = tab[0].low;
		high = tab[__X(n - 1, 128)].high;
		if (high - low > 512) {
			OfrontOPP_err(209);
		}
	} else {
		low = 1;
		high = 0;
	}
	e = OfrontOPP_sym == 42;
	if (e) {
		OfrontOPS_Get(&OfrontOPP_sym);
		OfrontOPP_StatSeq(&y);
	} else {
		y = NIL;
	}
	OfrontOPB_Construct(16, &cases, y);
	OfrontOPB_Construct(21, &*x, cases);
	cases->conval = OfrontOPT_NewConst();
	cases->conval->intval = low;
	cases->conval->intval2 = high;
	if (e) {
		cases->conval->setval = 0x02;
	} else {
		cases->conval->setval = 0x0;
	}
}

static void SetPos__35 (OfrontOPT_Node x)
{
	x->conval = OfrontOPT_NewConst();
	x->conval->intval = *StatSeq__30_s->pos;
}

static void CheckBool__33 (OfrontOPT_Node *x)
{
	if ((*x)->class == 8 || (*x)->class == 9) {
		OfrontOPP_err(126);
		*x = OfrontOPB_NewBoolConst(0);
	} else if ((*x)->typ->form != 2) {
		OfrontOPP_err(120);
		*x = OfrontOPB_NewBoolConst(0);
	}
	*StatSeq__30_s->pos = OfrontOPM_errpos;
}

static void OfrontOPP_StatSeq (OfrontOPT_Node *stat)
{
	OfrontOPT_Object fpar = NIL, id = NIL, t = NIL, obj = NIL;
	OfrontOPT_Struct idtyp = NIL;
	BOOLEAN e;
	OfrontOPT_Node s = NIL, x = NIL, y = NIL, z = NIL, apar = NIL, last = NIL, lastif = NIL;
	LONGINT pos;
	OfrontOPS_Name name;
	struct StatSeq__30 _s;
	_s.pos = &pos;
	_s.lnk = StatSeq__30_s;
	StatSeq__30_s = &_s;
	*stat = NIL;
	last = NIL;
	for (;;) {
		x = NIL;
		if (OfrontOPP_sym < 38) {
			OfrontOPP_err(14);
			do {
				OfrontOPS_Get(&OfrontOPP_sym);
			} while (!(OfrontOPP_sym >= 38));
		}
		if (OfrontOPP_sym == 38) {
			OfrontOPP_qualident(&id);
			x = OfrontOPB_NewLeaf(id);
			OfrontOPP_selector(&x);
			if (OfrontOPP_sym == 34) {
				OfrontOPS_Get(&OfrontOPP_sym);
				OfrontOPP_Expression(&y);
				OfrontOPB_Assign(&x, y);
			} else if (OfrontOPP_sym == 9) {
				OfrontOPP_err(34);
				OfrontOPS_Get(&OfrontOPP_sym);
				OfrontOPP_Expression(&y);
				OfrontOPB_Assign(&x, y);
			} else if (x->class == 9 && x->obj->mode == 8) {
				OfrontOPP_StandProcCall(&x);
				if (x != NIL && x->typ != OfrontOPT_notyp) {
					OfrontOPP_err(55);
				}
			} else {
				OfrontOPB_PrepCall(&x, &fpar);
				if (OfrontOPP_sym == 30) {
					OfrontOPS_Get(&OfrontOPP_sym);
					OfrontOPP_ActualParameters(&apar, fpar);
					OfrontOPP_CheckSym(22);
				} else {
					apar = NIL;
					if (fpar != NIL) {
						OfrontOPP_err(65);
					}
				}
				OfrontOPB_Call(&x, apar, fpar);
				if (x->typ != OfrontOPT_notyp) {
					OfrontOPP_err(55);
				}
				if (OfrontOPP_level > 0) {
					OfrontOPT_topScope->link->leaf = 0;
				}
			}
			pos = OfrontOPM_errpos;
		} else if (OfrontOPP_sym == 45) {
			OfrontOPS_Get(&OfrontOPP_sym);
			OfrontOPP_Expression(&x);
			CheckBool__33(&x);
			OfrontOPP_CheckSym(26);
			OfrontOPP_StatSeq(&y);
			OfrontOPB_Construct(15, &x, y);
			SetPos__35(x);
			lastif = x;
			while (OfrontOPP_sym == 43) {
				OfrontOPS_Get(&OfrontOPP_sym);
				OfrontOPP_Expression(&y);
				CheckBool__33(&y);
				OfrontOPP_CheckSym(26);
				OfrontOPP_StatSeq(&z);
				OfrontOPB_Construct(15, &y, z);
				SetPos__35(y);
				OfrontOPB_Link(&x, &lastif, y);
			}
			if (OfrontOPP_sym == 42) {
				OfrontOPS_Get(&OfrontOPP_sym);
				OfrontOPP_StatSeq(&y);
			} else {
				y = NIL;
			}
			OfrontOPB_Construct(20, &x, y);
			OfrontOPP_CheckSym(41);
			OfrontOPB_OptIf(&x);
			pos = OfrontOPM_errpos;
		} else if (OfrontOPP_sym == 46) {
			OfrontOPS_Get(&OfrontOPP_sym);
			CasePart__31(&x);
			OfrontOPP_CheckSym(41);
		} else if (OfrontOPP_sym == 47) {
			OfrontOPS_Get(&OfrontOPP_sym);
			OfrontOPP_Expression(&x);
			CheckBool__33(&x);
			OfrontOPP_CheckSym(27);
			OfrontOPP_StatSeq(&y);
			OfrontOPB_Construct(22, &x, y);
			OfrontOPP_CheckSym(41);
		} else if (OfrontOPP_sym == 48) {
			OfrontOPS_Get(&OfrontOPP_sym);
			OfrontOPP_StatSeq(&x);
			if (OfrontOPP_sym == 44) {
				OfrontOPS_Get(&OfrontOPP_sym);
				OfrontOPP_Expression(&y);
				CheckBool__33(&y);
			} else {
				OfrontOPP_err(44);
			}
			OfrontOPB_Construct(23, &x, y);
		} else if (OfrontOPP_sym == 49) {
			OfrontOPS_Get(&OfrontOPP_sym);
			if (OfrontOPP_sym == 38) {
				OfrontOPP_qualident(&id);
				if (!__IN(id->typ->form, 0x70)) {
					OfrontOPP_err(68);
				}
				OfrontOPP_CheckSym(34);
				OfrontOPP_Expression(&y);
				pos = OfrontOPM_errpos;
				x = OfrontOPB_NewLeaf(id);
				OfrontOPB_Assign(&x, y);
				SetPos__35(x);
				OfrontOPP_CheckSym(28);
				OfrontOPP_Expression(&y);
				pos = OfrontOPM_errpos;
				if (y->class != 7) {
					__MOVE("@@", name, 3);
					OfrontOPT_Insert(name, &t);
					__MOVE("@for", t->name, 5);
					t->mode = 1;
					t->typ = x->left->typ;
					obj = OfrontOPT_topScope->scope;
					if (obj == NIL) {
						OfrontOPT_topScope->scope = t;
					} else {
						while (obj->link != NIL) {
							obj = obj->link;
						}
						obj->link = t;
					}
					z = OfrontOPB_NewLeaf(t);
					OfrontOPB_Assign(&z, y);
					SetPos__35(z);
					OfrontOPB_Link(&*stat, &last, z);
					y = OfrontOPB_NewLeaf(t);
				} else if (y->typ->form < 4 || y->typ->form > x->left->typ->form) {
					OfrontOPP_err(113);
				}
				OfrontOPB_Link(&*stat, &last, x);
				if (OfrontOPP_sym == 29) {
					OfrontOPS_Get(&OfrontOPP_sym);
					OfrontOPP_ConstExpression(&z);
				} else {
					z = OfrontOPB_NewIntConst(1);
				}
				pos = OfrontOPM_errpos;
				x = OfrontOPB_NewLeaf(id);
				if (z->conval->intval > 0) {
					OfrontOPB_Op(12, &x, y);
				} else if (z->conval->intval < 0) {
					OfrontOPB_Op(14, &x, y);
				} else {
					OfrontOPP_err(63);
					OfrontOPB_Op(14, &x, y);
				}
				OfrontOPP_CheckSym(27);
				OfrontOPP_StatSeq(&s);
				y = OfrontOPB_NewLeaf(id);
				OfrontOPB_StPar1(&y, z, 13);
				SetPos__35(y);
				if (s == NIL) {
					s = y;
				} else {
					z = s;
					while (z->link != NIL) {
						z = z->link;
					}
					z->link = y;
				}
				OfrontOPP_CheckSym(41);
				OfrontOPB_Construct(22, &x, s);
			} else {
				OfrontOPP_err(38);
			}
		} else if (OfrontOPP_sym == 50) {
			OfrontOPS_Get(&OfrontOPP_sym);
			OfrontOPP_LoopLevel += 1;
			OfrontOPP_StatSeq(&x);
			OfrontOPP_LoopLevel -= 1;
			OfrontOPB_Construct(24, &x, NIL);
			OfrontOPP_CheckSym(41);
			pos = OfrontOPM_errpos;
		} else if (OfrontOPP_sym == 51) {
			OfrontOPS_Get(&OfrontOPP_sym);
			idtyp = NIL;
			x = NIL;
			for (;;) {
				if (OfrontOPP_sym == 38) {
					OfrontOPP_qualident(&id);
					y = OfrontOPB_NewLeaf(id);
					if ((id != NIL && id->typ->form == 13) && (id->mode == 2 || !id->leaf)) {
						OfrontOPP_err(245);
					}
					OfrontOPP_CheckSym(20);
					if (OfrontOPP_sym == 38) {
						OfrontOPP_qualident(&t);
						if (t->mode == 5) {
							if (id != NIL) {
								idtyp = id->typ;
								OfrontOPB_TypTest(&y, t, 0);
								id->typ = t->typ;
							} else {
								OfrontOPP_err(130);
							}
						} else {
							OfrontOPP_err(52);
						}
					} else {
						OfrontOPP_err(38);
					}
				} else {
					OfrontOPP_err(38);
				}
				pos = OfrontOPM_errpos;
				OfrontOPP_CheckSym(27);
				OfrontOPP_StatSeq(&s);
				OfrontOPB_Construct(15, &y, s);
				SetPos__35(y);
				if (idtyp != NIL) {
					id->typ = idtyp;
					idtyp = NIL;
				}
				if (x == NIL) {
					x = y;
					lastif = x;
				} else {
					OfrontOPB_Link(&x, &lastif, y);
				}
				if (OfrontOPP_sym == 40) {
					OfrontOPS_Get(&OfrontOPP_sym);
				} else {
					break;
				}
			}
			e = OfrontOPP_sym == 42;
			if (e) {
				OfrontOPS_Get(&OfrontOPP_sym);
				OfrontOPP_StatSeq(&s);
			} else {
				s = NIL;
			}
			OfrontOPB_Construct(27, &x, s);
			OfrontOPP_CheckSym(41);
			if (e) {
				x->subcl = 1;
			}
		} else if (OfrontOPP_sym == 52) {
			OfrontOPS_Get(&OfrontOPP_sym);
			if (OfrontOPP_LoopLevel == 0) {
				OfrontOPP_err(46);
			}
			OfrontOPB_Construct(25, &x, NIL);
			pos = OfrontOPM_errpos;
		} else if (OfrontOPP_sym == 53) {
			OfrontOPS_Get(&OfrontOPP_sym);
			if (OfrontOPP_sym < 39) {
				OfrontOPP_Expression(&x);
			}
			if (OfrontOPP_level > 0) {
				OfrontOPB_Return(&x, OfrontOPT_topScope->link);
			} else {
				OfrontOPB_Return(&x, NIL);
			}
			pos = OfrontOPM_errpos;
		}
		if (x != NIL) {
			SetPos__35(x);
			OfrontOPB_Link(&*stat, &last, x);
		}
		if (OfrontOPP_sym == 39) {
			OfrontOPS_Get(&OfrontOPP_sym);
		} else if (OfrontOPP_sym <= 38 || 45 <= OfrontOPP_sym && OfrontOPP_sym <= 53) {
			OfrontOPP_err(39);
		} else {
			break;
		}
	}
	StatSeq__30_s = _s.lnk;
}

static void OfrontOPP_Block (OfrontOPT_Node *procdec, OfrontOPT_Node *statseq)
{
	OfrontOPT_Struct typ = NIL;
	OfrontOPT_Object obj = NIL, first = NIL, last = NIL;
	OfrontOPT_Node x = NIL, lastdec = NIL;
	INTEGER i;
	first = NIL;
	last = NIL;
	OfrontOPP_nofFwdPtr = 0;
	for (;;) {
		if (OfrontOPP_sym == 58) {
			OfrontOPS_Get(&OfrontOPP_sym);
			while (OfrontOPP_sym == 38) {
				OfrontOPT_Insert(OfrontOPS_name, &obj);
				OfrontOPP_CheckMark(&obj->vis);
				obj->typ = OfrontOPT_sinttyp;
				obj->mode = 1;
				if (OfrontOPP_sym == 9) {
					OfrontOPS_Get(&OfrontOPP_sym);
					OfrontOPP_ConstExpression(&x);
				} else if (OfrontOPP_sym == 34) {
					OfrontOPP_err(9);
					OfrontOPS_Get(&OfrontOPP_sym);
					OfrontOPP_ConstExpression(&x);
				} else {
					OfrontOPP_err(9);
					x = OfrontOPB_NewIntConst(1);
				}
				obj->mode = 3;
				obj->typ = x->typ;
				obj->conval = x->conval;
				OfrontOPP_CheckSym(39);
			}
		}
		if (OfrontOPP_sym == 59) {
			OfrontOPS_Get(&OfrontOPP_sym);
			while (OfrontOPP_sym == 38) {
				OfrontOPT_Insert(OfrontOPS_name, &obj);
				obj->mode = 5;
				obj->typ = OfrontOPT_undftyp;
				OfrontOPP_CheckMark(&obj->vis);
				if (OfrontOPP_sym == 9) {
					OfrontOPS_Get(&OfrontOPP_sym);
					OfrontOPP_TypeDecl(&obj->typ, &obj->typ);
				} else if (OfrontOPP_sym == 34 || OfrontOPP_sym == 20) {
					OfrontOPP_err(9);
					OfrontOPS_Get(&OfrontOPP_sym);
					OfrontOPP_TypeDecl(&obj->typ, &obj->typ);
				} else {
					OfrontOPP_err(9);
				}
				if (obj->typ->strobj == NIL) {
					obj->typ->strobj = obj;
				}
				if (__IN(obj->typ->comp, 0x1c)) {
					i = 0;
					while (i < OfrontOPP_nofFwdPtr) {
						typ = OfrontOPP_FwdPtr[__X(i, 64)];
						i += 1;
						if (__STRCMP(typ->link->name, obj->name) == 0) {
							typ->BaseTyp = obj->typ;
							typ->link->name[0] = 0x00;
						}
					}
				}
				OfrontOPP_CheckSym(39);
			}
		}
		if (OfrontOPP_sym == 60) {
			OfrontOPS_Get(&OfrontOPP_sym);
			while (OfrontOPP_sym == 38) {
				for (;;) {
					if (OfrontOPP_sym == 38) {
						OfrontOPT_Insert(OfrontOPS_name, &obj);
						OfrontOPP_CheckMark(&obj->vis);
						obj->mode = 1;
						obj->link = NIL;
						obj->leaf = obj->vis == 0;
						obj->typ = OfrontOPT_undftyp;
						if (first == NIL) {
							first = obj;
						}
						if (last == NIL) {
							OfrontOPT_topScope->scope = obj;
						} else {
							last->link = obj;
						}
						last = obj;
					} else {
						OfrontOPP_err(38);
					}
					if (OfrontOPP_sym == 19) {
						OfrontOPS_Get(&OfrontOPP_sym);
					} else if (OfrontOPP_sym == 38) {
						OfrontOPP_err(19);
					} else {
						break;
					}
				}
				OfrontOPP_CheckSym(20);
				OfrontOPP_Type(&typ, &OfrontOPT_notyp);
				typ->pvused = 1;
				if (typ->comp == 3) {
					typ = OfrontOPT_undftyp;
					OfrontOPP_err(88);
				}
				while (first != NIL) {
					first->typ = typ;
					first = first->link;
				}
				OfrontOPP_CheckSym(39);
			}
		}
		if (OfrontOPP_sym < 58 || OfrontOPP_sym > 60) {
			break;
		}
	}
	i = 0;
	while (i < OfrontOPP_nofFwdPtr) {
		if (OfrontOPP_FwdPtr[__X(i, 64)]->link->name[0] != 0x00) {
			OfrontOPP_err(128);
		}
		OfrontOPP_FwdPtr[__X(i, 64)] = NIL;
		i += 1;
	}
	OfrontOPT_topScope->adr = OfrontOPM_errpos;
	*procdec = NIL;
	lastdec = NIL;
	while (OfrontOPP_sym == 61) {
		OfrontOPS_Get(&OfrontOPP_sym);
		OfrontOPP_ProcedureDeclaration(&x);
		if (x != NIL) {
			if (lastdec == NIL) {
				*procdec = x;
			} else {
				lastdec->link = x;
			}
			lastdec = x;
		}
		OfrontOPP_CheckSym(39);
	}
	if (OfrontOPP_sym == 57) {
		OfrontOPS_Get(&OfrontOPP_sym);
		OfrontOPP_StatSeq(&*statseq);
	} else {
		*statseq = NIL;
	}
	if (OfrontOPP_level == 0 && OfrontOPP_TDinit != NIL) {
		OfrontOPP_lastTDinit->link = *statseq;
		*statseq = OfrontOPP_TDinit;
	}
	OfrontOPP_CheckSym(41);
}

void OfrontOPP_Module (OfrontOPT_Node *prog, SET opt)
{
	OfrontOPS_Name impName, aliasName;
	OfrontOPT_Node procdec = NIL, statseq = NIL;
	LONGINT c;
	BOOLEAN done;
	OfrontOPS_Init();
	OfrontOPP_LoopLevel = 0;
	OfrontOPP_level = 0;
	OfrontOPS_Get(&OfrontOPP_sym);
	if (OfrontOPP_sym == 63) {
		OfrontOPS_Get(&OfrontOPP_sym);
	} else {
		OfrontOPP_err(16);
	}
	if (OfrontOPP_sym == 38) {
		OfrontOPM_LogW(' ');
		OfrontOPM_LogWStr(OfrontOPS_name, 32);
		OfrontOPT_Init(OfrontOPS_name, opt);
		OfrontOPS_Get(&OfrontOPP_sym);
		OfrontOPP_CheckSym(39);
		if (OfrontOPP_sym == 62) {
			OfrontOPS_Get(&OfrontOPP_sym);
			for (;;) {
				if (OfrontOPP_sym == 38) {
					__COPY(OfrontOPS_name, aliasName, 32);
					__COPY(aliasName, impName, 32);
					OfrontOPS_Get(&OfrontOPP_sym);
					if (OfrontOPP_sym == 34) {
						OfrontOPS_Get(&OfrontOPP_sym);
						if (OfrontOPP_sym == 38) {
							__COPY(OfrontOPS_name, impName, 32);
							OfrontOPS_Get(&OfrontOPP_sym);
						} else {
							OfrontOPP_err(38);
						}
					}
					OfrontOPT_Import(aliasName, impName, &done);
				} else {
					OfrontOPP_err(38);
				}
				if (OfrontOPP_sym == 19) {
					OfrontOPS_Get(&OfrontOPP_sym);
				} else if (OfrontOPP_sym == 38) {
					OfrontOPP_err(19);
				} else {
					break;
				}
			}
			OfrontOPP_CheckSym(39);
		}
		if (OfrontOPM_noerr) {
			OfrontOPP_TDinit = NIL;
			OfrontOPP_lastTDinit = NIL;
			c = OfrontOPM_errpos;
			OfrontOPP_Block(&procdec, &statseq);
			OfrontOPB_Enter(&procdec, statseq, NIL);
			*prog = procdec;
			(*prog)->conval = OfrontOPT_NewConst();
			(*prog)->conval->intval = c;
			if (OfrontOPP_sym == 38) {
				if (__STRCMP(OfrontOPS_name, OfrontOPT_SelfName) != 0) {
					OfrontOPP_err(4);
				}
				OfrontOPS_Get(&OfrontOPP_sym);
			} else {
				OfrontOPP_err(38);
			}
			if (OfrontOPP_sym != 18) {
				OfrontOPP_err(18);
			}
		}
	} else {
		OfrontOPP_err(38);
	}
	OfrontOPP_TDinit = NIL;
	OfrontOPP_lastTDinit = NIL;
}

/*----------------------------------------------------------------------------*/
static void EnumPtrs(void (*P)(void*))
{
	P(OfrontOPP_TDinit);
	P(OfrontOPP_lastTDinit);
	__ENUMP(OfrontOPP_FwdPtr, 64, P);
}

__TDESC(OfrontOPP__1__desc, 1, 0) = {__TDFLDS("", 16), {-8}};

export void *OfrontOPP__init(void)
{
	__DEFMOD;
	__IMPORT(OfrontOPB__init);
	__IMPORT(OfrontOPM__init);
	__IMPORT(OfrontOPS__init);
	__IMPORT(OfrontOPT__init);
	__REGMOD("OfrontOPP", EnumPtrs);
	__INITYP(OfrontOPP__1, OfrontOPP__1, 0);
/* BEGIN */
	__ENDMOD;
}
