/* Ofront 1.2 -xtspkael */

#ifndef CmdlnTexts__h
#define CmdlnTexts__h

#include "SYSTEM.h"
#include "Files.h"

typedef
	struct CmdlnTexts_BufDesc {
		LONGINT len;
		char _prvt0[8];
	} CmdlnTexts_BufDesc;

typedef
	CmdlnTexts_BufDesc *CmdlnTexts_Buffer;

typedef
	struct CmdlnTexts_ElemMsg {
		char _prvt0[1];
	} CmdlnTexts_ElemMsg;

typedef
	struct CmdlnTexts_ElemDesc *CmdlnTexts_Elem;

typedef
	struct CmdlnTexts_CopyMsg { /* CmdlnTexts_ElemMsg */
		CmdlnTexts_Elem e;
	} CmdlnTexts_CopyMsg;

typedef
	struct CmdlnTexts_RunDesc {
		LONGINT _prvt0;
		char _prvt1[29];
	} CmdlnTexts_RunDesc;

typedef
	void (*CmdlnTexts_Handler)(CmdlnTexts_Elem, CmdlnTexts_ElemMsg*, LONGINT *);

typedef
	struct CmdlnTexts_ElemDesc {
		char _prvt0[40];
		LONGINT W, H;
		CmdlnTexts_Handler handle;
		char _prvt1[8];
	} CmdlnTexts_ElemDesc;

typedef
	struct CmdlnTexts_FileMsg { /* CmdlnTexts_ElemMsg */
		INTEGER id;
		LONGINT pos;
		Files_Rider r;
	} CmdlnTexts_FileMsg;

typedef
	struct CmdlnTexts_FontDesc {
		char _prvt0[32];
	} CmdlnTexts_FontDesc;

typedef
	CmdlnTexts_FontDesc *CmdlnTexts_FontsFont;

typedef
	struct CmdlnTexts_IdentifyMsg { /* CmdlnTexts_ElemMsg */
		CHAR mod[32], proc[32];
	} CmdlnTexts_IdentifyMsg;

typedef
	struct CmdlnTexts_Reader {
		BOOLEAN eot;
		CmdlnTexts_FontsFont fnt;
		SHORTINT col, voff;
		CmdlnTexts_Elem elem;
		char _prvt0[64];
	} CmdlnTexts_Reader;

typedef
	struct CmdlnTexts_Scanner { /* CmdlnTexts_Reader */
		BOOLEAN eot;
		CmdlnTexts_FontsFont fnt;
		SHORTINT col, voff;
		CmdlnTexts_Elem elem;
		char _prvt0[64];
		CHAR nextCh;
		INTEGER line, class;
		LONGINT i;
		REAL x;
		LONGREAL y;
		CHAR c;
		SHORTINT len;
		CHAR s[64];
	} CmdlnTexts_Scanner;

typedef
	struct CmdlnTexts_TextDesc *CmdlnTexts_Text;

typedef
	struct CmdlnTexts_TextDesc {
		LONGINT len;
		char _prvt0[24];
	} CmdlnTexts_TextDesc;

typedef
	struct CmdlnTexts_Writer {
		CmdlnTexts_Buffer buf;
		CmdlnTexts_FontsFont fnt;
		SHORTINT col, voff;
		char _prvt0[52];
	} CmdlnTexts_Writer;


import CmdlnTexts_Elem CmdlnTexts_new;

import LONGINT *CmdlnTexts_FontDesc__typ;
import LONGINT *CmdlnTexts_RunDesc__typ;
import LONGINT *CmdlnTexts_ElemMsg__typ;
import LONGINT *CmdlnTexts_ElemDesc__typ;
import LONGINT *CmdlnTexts_FileMsg__typ;
import LONGINT *CmdlnTexts_CopyMsg__typ;
import LONGINT *CmdlnTexts_IdentifyMsg__typ;
import LONGINT *CmdlnTexts_BufDesc__typ;
import LONGINT *CmdlnTexts_TextDesc__typ;
import LONGINT *CmdlnTexts_Reader__typ;
import LONGINT *CmdlnTexts_Scanner__typ;
import LONGINT *CmdlnTexts_Writer__typ;

import void CmdlnTexts_Append (CmdlnTexts_Text T, CmdlnTexts_Buffer B);
import void CmdlnTexts_ChangeLooks (CmdlnTexts_Text T, LONGINT beg, LONGINT end, SET sel, CmdlnTexts_FontsFont fnt, SHORTINT col, SHORTINT voff);
import void CmdlnTexts_Close (CmdlnTexts_Text T, CHAR *name, LONGINT name__len);
import void CmdlnTexts_Copy (CmdlnTexts_Buffer SB, CmdlnTexts_Buffer DB);
import void CmdlnTexts_CopyElem (CmdlnTexts_Elem SE, CmdlnTexts_Elem DE);
import void CmdlnTexts_Delete (CmdlnTexts_Text T, LONGINT beg, LONGINT end);
import CmdlnTexts_Text CmdlnTexts_ElemBase (CmdlnTexts_Elem E);
import LONGINT CmdlnTexts_ElemPos (CmdlnTexts_Elem E);
import void CmdlnTexts_Insert (CmdlnTexts_Text T, LONGINT pos, CmdlnTexts_Buffer B);
import void CmdlnTexts_Load (Files_Rider *r, LONGINT *r__typ, CmdlnTexts_Text T);
import void CmdlnTexts_Open (CmdlnTexts_Text T, CHAR *name, LONGINT name__len);
import void CmdlnTexts_OpenBuf (CmdlnTexts_Buffer B);
import void CmdlnTexts_OpenReader (CmdlnTexts_Reader *R, LONGINT *R__typ, CmdlnTexts_Text T, LONGINT pos);
import void CmdlnTexts_OpenScanner (CmdlnTexts_Scanner *S, LONGINT *S__typ, CmdlnTexts_Text T, LONGINT pos);
import void CmdlnTexts_OpenWriter (CmdlnTexts_Writer *W, LONGINT *W__typ);
import LONGINT CmdlnTexts_Pos (CmdlnTexts_Reader *R, LONGINT *R__typ);
import void CmdlnTexts_Read (CmdlnTexts_Reader *R, LONGINT *R__typ, CHAR *ch);
import void CmdlnTexts_ReadElem (CmdlnTexts_Reader *R, LONGINT *R__typ);
import void CmdlnTexts_ReadPrevElem (CmdlnTexts_Reader *R, LONGINT *R__typ);
import void CmdlnTexts_Recall (CmdlnTexts_Buffer *B);
import void CmdlnTexts_Save (CmdlnTexts_Text T, LONGINT beg, LONGINT end, CmdlnTexts_Buffer B);
import void CmdlnTexts_Scan (CmdlnTexts_Scanner *S, LONGINT *S__typ);
import void CmdlnTexts_SetColor (CmdlnTexts_Writer *W, LONGINT *W__typ, SHORTINT col);
import void CmdlnTexts_SetFont (CmdlnTexts_Writer *W, LONGINT *W__typ, CmdlnTexts_FontsFont fnt);
import void CmdlnTexts_SetOffset (CmdlnTexts_Writer *W, LONGINT *W__typ, SHORTINT voff);
import void CmdlnTexts_Store (Files_Rider *r, LONGINT *r__typ, CmdlnTexts_Text T);
import void CmdlnTexts_Write (CmdlnTexts_Writer *W, LONGINT *W__typ, CHAR ch);
import void CmdlnTexts_WriteDate (CmdlnTexts_Writer *W, LONGINT *W__typ, LONGINT t, LONGINT d);
import void CmdlnTexts_WriteElem (CmdlnTexts_Writer *W, LONGINT *W__typ, CmdlnTexts_Elem e);
import void CmdlnTexts_WriteHex (CmdlnTexts_Writer *W, LONGINT *W__typ, LONGINT x);
import void CmdlnTexts_WriteInt (CmdlnTexts_Writer *W, LONGINT *W__typ, LONGINT x, LONGINT n);
import void CmdlnTexts_WriteLn (CmdlnTexts_Writer *W, LONGINT *W__typ);
import void CmdlnTexts_WriteLongReal (CmdlnTexts_Writer *W, LONGINT *W__typ, LONGREAL x, INTEGER n);
import void CmdlnTexts_WriteLongRealHex (CmdlnTexts_Writer *W, LONGINT *W__typ, LONGREAL x);
import void CmdlnTexts_WriteReal (CmdlnTexts_Writer *W, LONGINT *W__typ, REAL x, INTEGER n);
import void CmdlnTexts_WriteRealFix (CmdlnTexts_Writer *W, LONGINT *W__typ, REAL x, INTEGER n, INTEGER k);
import void CmdlnTexts_WriteRealHex (CmdlnTexts_Writer *W, LONGINT *W__typ, REAL x);
import void CmdlnTexts_WriteString (CmdlnTexts_Writer *W, LONGINT *W__typ, CHAR *s, LONGINT s__len);
import void *CmdlnTexts__init(void);


#endif
