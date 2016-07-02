/* Ofront 1.2 -xtspkael */

#ifndef OfrontOPB__h
#define OfrontOPB__h

#include "SYSTEM.h"
#include "OfrontOPS.h"
#include "OfrontOPT.h"


import void (*OfrontOPB_typSize)(OfrontOPT_Struct);


import void OfrontOPB_Assign (OfrontOPT_Node *x, OfrontOPT_Node y);
import void OfrontOPB_Call (OfrontOPT_Node *x, OfrontOPT_Node apar, OfrontOPT_Object fp);
import void OfrontOPB_CheckParameters (OfrontOPT_Object fp, OfrontOPT_Object ap, BOOLEAN checkNames);
import void OfrontOPB_Construct (SHORTINT class, OfrontOPT_Node *x, OfrontOPT_Node y);
import void OfrontOPB_DeRef (OfrontOPT_Node *x);
import OfrontOPT_Node OfrontOPB_EmptySet (void);
import void OfrontOPB_Enter (OfrontOPT_Node *procdec, OfrontOPT_Node stat, OfrontOPT_Object proc);
import void OfrontOPB_Field (OfrontOPT_Node *x, OfrontOPT_Object y);
import void OfrontOPB_In (OfrontOPT_Node *x, OfrontOPT_Node y);
import void OfrontOPB_Index (OfrontOPT_Node *x, OfrontOPT_Node y);
import void OfrontOPB_Inittd (OfrontOPT_Node *inittd, OfrontOPT_Node *last, OfrontOPT_Struct typ);
import void OfrontOPB_Link (OfrontOPT_Node *x, OfrontOPT_Node *last, OfrontOPT_Node y);
import void OfrontOPB_MOp (SHORTINT op, OfrontOPT_Node *x);
import OfrontOPT_Node OfrontOPB_NewBoolConst (BOOLEAN boolval);
import OfrontOPT_Node OfrontOPB_NewIntConst (LONGINT intval);
import OfrontOPT_Node OfrontOPB_NewLeaf (OfrontOPT_Object obj);
import OfrontOPT_Node OfrontOPB_NewRealConst (LONGREAL realval, OfrontOPT_Struct typ);
import OfrontOPT_Node OfrontOPB_NewString (OfrontOPS_String str, LONGINT len);
import OfrontOPT_Node OfrontOPB_Nil (void);
import void OfrontOPB_Op (SHORTINT op, OfrontOPT_Node *x, OfrontOPT_Node y);
import void OfrontOPB_OptIf (OfrontOPT_Node *x);
import void OfrontOPB_Param (OfrontOPT_Node ap, OfrontOPT_Object fp);
import void OfrontOPB_PrepCall (OfrontOPT_Node *x, OfrontOPT_Object *fpar);
import void OfrontOPB_Return (OfrontOPT_Node *x, OfrontOPT_Object proc);
import void OfrontOPB_SetElem (OfrontOPT_Node *x);
import void OfrontOPB_SetRange (OfrontOPT_Node *x, OfrontOPT_Node y);
import void OfrontOPB_StFct (OfrontOPT_Node *par0, SHORTINT fctno, INTEGER parno);
import void OfrontOPB_StPar0 (OfrontOPT_Node *par0, INTEGER fctno);
import void OfrontOPB_StPar1 (OfrontOPT_Node *par0, OfrontOPT_Node x, SHORTINT fctno);
import void OfrontOPB_StParN (OfrontOPT_Node *par0, OfrontOPT_Node x, INTEGER fctno, INTEGER n);
import void OfrontOPB_StaticLink (SHORTINT dlev);
import void OfrontOPB_TypTest (OfrontOPT_Node *x, OfrontOPT_Object obj, BOOLEAN guard);
import void *OfrontOPB__init(void);


#endif
