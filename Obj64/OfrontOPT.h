/* Ofront+ 0.9 -xtspkae */

#ifndef OfrontOPT__h
#define OfrontOPT__h

#include "SYSTEM.h"
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
	struct OfrontOPT_NodeDesc *OfrontOPT_Node;

typedef
	struct OfrontOPT_StrDesc *OfrontOPT_Struct;

typedef
	struct OfrontOPT_ObjDesc *OfrontOPT_Object;

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
		BOOLEAN allocated, pbused, pvused;
		char _prvt0[8];
		LONGINT pbfp, pvfp;
		OfrontOPT_Struct BaseTyp;
		OfrontOPT_Object link, strobj;
	} OfrontOPT_StrDesc;


import void (*OfrontOPT_typSize)(OfrontOPT_Struct);
import OfrontOPT_Object OfrontOPT_topScope;
import OfrontOPT_Struct OfrontOPT_undftyp, OfrontOPT_bytetyp, OfrontOPT_booltyp, OfrontOPT_chartyp, OfrontOPT_sinttyp, OfrontOPT_inttyp, OfrontOPT_linttyp, OfrontOPT_realtyp, OfrontOPT_lrltyp, OfrontOPT_settyp, OfrontOPT_stringtyp, OfrontOPT_niltyp, OfrontOPT_notyp, OfrontOPT_sysptrtyp;
import SHORTINT OfrontOPT_nofGmod;
import OfrontOPT_Object OfrontOPT_GlbMod[64];
import OfrontOPS_Name OfrontOPT_SelfName;
import BOOLEAN OfrontOPT_SYSimported;

import LONGINT *OfrontOPT_ConstDesc__typ;
import LONGINT *OfrontOPT_ObjDesc__typ;
import LONGINT *OfrontOPT_StrDesc__typ;
import LONGINT *OfrontOPT_NodeDesc__typ;

import void OfrontOPT_Close (void);
import void OfrontOPT_CloseScope (void);
import void OfrontOPT_Export (BOOLEAN *ext, BOOLEAN *new);
import void OfrontOPT_FPrintErr (OfrontOPT_Object obj, INTEGER errno);
import void OfrontOPT_FPrintObj (OfrontOPT_Object obj);
import void OfrontOPT_FPrintStr (OfrontOPT_Struct typ);
import void OfrontOPT_Find (OfrontOPT_Object *res);
import void OfrontOPT_FindField (OfrontOPS_Name name, OfrontOPT_Struct typ, OfrontOPT_Object *res);
import void OfrontOPT_FindImport (OfrontOPT_Object mod, OfrontOPT_Object *res);
import void OfrontOPT_IdFPrint (OfrontOPT_Struct typ);
import void OfrontOPT_Import (OfrontOPS_Name aliasName, OfrontOPS_Name name, BOOLEAN *done);
import void OfrontOPT_Init (OfrontOPS_Name name, SET opt);
import void OfrontOPT_Insert (OfrontOPS_Name name, OfrontOPT_Object *obj);
import void OfrontOPT_InsertImport (OfrontOPT_Object obj, OfrontOPT_Object *root, OfrontOPT_Object *old);
import OfrontOPT_Const OfrontOPT_NewConst (void);
import OfrontOPT_ConstExt OfrontOPT_NewExt (void);
import OfrontOPT_Node OfrontOPT_NewNode (SHORTINT class);
import OfrontOPT_Object OfrontOPT_NewObj (void);
import OfrontOPT_Struct OfrontOPT_NewStr (SHORTINT form, SHORTINT comp);
import void OfrontOPT_OpenScope (SHORTINT level, OfrontOPT_Object owner);
import void *OfrontOPT__init(void);


#endif
