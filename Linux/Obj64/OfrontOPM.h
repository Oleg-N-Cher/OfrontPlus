/* Ofront 1.2 -xtspkael */

#ifndef OfrontOPM__h
#define OfrontOPM__h

#include "SYSTEM.h"


import INTEGER OfrontOPM_ByteSize, OfrontOPM_CharSize, OfrontOPM_BoolSize, OfrontOPM_SIntSize, OfrontOPM_IntSize, OfrontOPM_LIntSize, OfrontOPM_SetSize, OfrontOPM_RealSize, OfrontOPM_LRealSize, OfrontOPM_PointerSize, OfrontOPM_ProcSize, OfrontOPM_RecSize, OfrontOPM_CharAlign, OfrontOPM_BoolAlign, OfrontOPM_SIntAlign, OfrontOPM_IntAlign, OfrontOPM_LIntAlign, OfrontOPM_SetAlign, OfrontOPM_RealAlign, OfrontOPM_LRealAlign, OfrontOPM_PointerAlign, OfrontOPM_ProcAlign, OfrontOPM_RecAlign, OfrontOPM_ByteOrder, OfrontOPM_BitOrder, OfrontOPM_MaxSet;
import LONGINT OfrontOPM_MinSInt, OfrontOPM_MinInt, OfrontOPM_MinLInt, OfrontOPM_MaxSInt, OfrontOPM_MaxInt, OfrontOPM_MaxLInt, OfrontOPM_MaxIndex;
import LONGREAL OfrontOPM_MinReal, OfrontOPM_MaxReal, OfrontOPM_MinLReal, OfrontOPM_MaxLReal;
import BOOLEAN OfrontOPM_noerr;
import LONGINT OfrontOPM_curpos, OfrontOPM_errpos, OfrontOPM_breakpc;
import INTEGER OfrontOPM_currFile, OfrontOPM_level, OfrontOPM_pc, OfrontOPM_entno;
import CHAR OfrontOPM_modName[32];
import CHAR OfrontOPM_objname[64];
import SET OfrontOPM_opt, OfrontOPM_glbopt;


import void OfrontOPM_CloseFiles (void);
import void OfrontOPM_CloseOldSym (void);
import void OfrontOPM_DeleteNewSym (void);
import void OfrontOPM_FPrint (LONGINT *fp, LONGINT val);
import void OfrontOPM_FPrintLReal (LONGINT *fp, LONGREAL lr);
import void OfrontOPM_FPrintReal (LONGINT *fp, REAL real);
import void OfrontOPM_FPrintSet (LONGINT *fp, SET set);
import void OfrontOPM_Get (CHAR *ch);
import void OfrontOPM_Init (BOOLEAN *done);
import void OfrontOPM_InitOptions (void);
import void OfrontOPM_LogW (CHAR ch);
import void OfrontOPM_LogWLn (void);
import void OfrontOPM_LogWNum (LONGINT i, LONGINT len);
import void OfrontOPM_LogWStr (CHAR *s, LONGINT s__len);
import void OfrontOPM_Mark (INTEGER n, LONGINT pos);
import void OfrontOPM_NewSym (CHAR *modName, LONGINT modName__len);
import void OfrontOPM_OldSym (CHAR *modName, LONGINT modName__len, BOOLEAN *done);
import void OfrontOPM_OpenFiles (CHAR *moduleName, LONGINT moduleName__len);
import void OfrontOPM_OpenPar (void);
import void OfrontOPM_PromoteIntConstToLInt (void);
import void OfrontOPM_RegisterNewSym (void);
import void OfrontOPM_SymRCh (CHAR *ch);
import LONGINT OfrontOPM_SymRInt (void);
import void OfrontOPM_SymRLReal (LONGREAL *lr);
import void OfrontOPM_SymRReal (REAL *r);
import void OfrontOPM_SymRSet (SET *s);
import void OfrontOPM_SymWCh (CHAR ch);
import void OfrontOPM_SymWInt (LONGINT i);
import void OfrontOPM_SymWLReal (LONGREAL lr);
import void OfrontOPM_SymWReal (REAL r);
import void OfrontOPM_SymWSet (SET s);
import void OfrontOPM_Write (CHAR ch);
import void OfrontOPM_WriteHex (LONGINT i);
import void OfrontOPM_WriteInt (LONGINT i);
import void OfrontOPM_WriteLn (void);
import void OfrontOPM_WriteReal (LONGREAL r, CHAR suffx);
import void OfrontOPM_WriteString (CHAR *s, LONGINT s__len);
import void OfrontOPM_WriteStringVar (CHAR *s, LONGINT s__len);
import BOOLEAN OfrontOPM_eofSF (void);
import void OfrontOPM_err (INTEGER n);
import void *OfrontOPM__init(void);


#endif
