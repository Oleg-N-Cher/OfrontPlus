/* Ofront 1.2 -xtspkae */

#ifndef OfrontOPC__h
#define OfrontOPC__h

#include "SYSTEM.h"
#include "OfrontOPT.h"




import void OfrontOPC_Align (LONGINT *adr, LONGINT base);
import void OfrontOPC_Andent (OfrontOPT_Struct typ);
import LONGINT OfrontOPC_Base (OfrontOPT_Struct typ);
import OfrontOPT_Object OfrontOPC_BaseTProc (OfrontOPT_Object obj);
import void OfrontOPC_BegBlk (void);
import void OfrontOPC_BegStat (void);
import void OfrontOPC_Case (LONGINT caseVal, INTEGER form);
import void OfrontOPC_Cmp (INTEGER rel);
import void OfrontOPC_CompleteIdent (OfrontOPT_Object obj);
import void OfrontOPC_Constant (OfrontOPT_Const con, INTEGER form);
import void OfrontOPC_DefineInter (OfrontOPT_Object proc);
import void OfrontOPC_EndBlk (void);
import void OfrontOPC_EndBlk0 (void);
import void OfrontOPC_EndStat (void);
import void OfrontOPC_EnterBody (void);
import void OfrontOPC_EnterProc (OfrontOPT_Object proc);
import void OfrontOPC_ExitBody (void);
import void OfrontOPC_ExitProc (OfrontOPT_Object proc, BOOLEAN eoBlock, BOOLEAN implicitRet);
import void OfrontOPC_GenBdy (OfrontOPT_Node n);
import void OfrontOPC_GenEnumPtrs (OfrontOPT_Object var);
import void OfrontOPC_GenHdr (OfrontOPT_Node n);
import void OfrontOPC_GenHdrIncludes (void);
import void OfrontOPC_Halt (LONGINT n);
import void OfrontOPC_Ident (OfrontOPT_Object obj);
import void OfrontOPC_Increment (BOOLEAN decrement);
import void OfrontOPC_Indent (INTEGER count);
import void OfrontOPC_Init (void);
import void OfrontOPC_InitTDesc (OfrontOPT_Struct typ);
import void OfrontOPC_Len (OfrontOPT_Object obj, OfrontOPT_Struct array, LONGINT dim);
import LONGINT OfrontOPC_NofPtrs (OfrontOPT_Struct typ);
import void OfrontOPC_SetInclude (BOOLEAN exclude);
import void OfrontOPC_TDescDecl (OfrontOPT_Struct typ);
import void OfrontOPC_TypeDefs (OfrontOPT_Object obj, INTEGER vis);
import void OfrontOPC_TypeOf (OfrontOPT_Object ap);
import void *OfrontOPC__init(void);


#endif
