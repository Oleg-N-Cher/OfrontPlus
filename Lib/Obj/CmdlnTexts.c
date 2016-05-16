/*  Ofront 1.2 -xtspkae */
#include "SYSTEM.h"
#include "Files.h"
#include "Modules.h"
#include "Reals.h"

typedef
	struct CmdlnTexts_RunDesc *CmdlnTexts_Run;

typedef
	struct CmdlnTexts_FontDesc *CmdlnTexts_FontsFont;

typedef
	struct CmdlnTexts_RunDesc {
		CmdlnTexts_Run prev, next;
		LONGINT len;
		CmdlnTexts_FontsFont fnt;
		SHORTINT col, voff;
		BOOLEAN ascii;
	} CmdlnTexts_RunDesc;

typedef
	struct CmdlnTexts_ElemDesc *CmdlnTexts_Elem;

typedef
	struct CmdlnTexts_ElemMsg {
		char _prvt0[1];
	} CmdlnTexts_ElemMsg;

typedef
	void (*CmdlnTexts_Handler)(CmdlnTexts_Elem, CmdlnTexts_ElemMsg*, LONGINT *);

typedef
	struct CmdlnTexts_TextDesc *CmdlnTexts_Text;

typedef
	struct CmdlnTexts_ElemDesc {
		CmdlnTexts_Run prev, next;
		LONGINT len;
		CmdlnTexts_FontsFont fnt;
		SHORTINT col, voff;
		BOOLEAN ascii;
		LONGINT W, H;
		CmdlnTexts_Handler handle;
		CmdlnTexts_Text base;
	} CmdlnTexts_ElemDesc;

struct CmdlnTexts__1 { /* CmdlnTexts_ElemDesc */
	CmdlnTexts_Run prev, next;
	LONGINT len;
	CmdlnTexts_FontsFont fnt;
	SHORTINT col, voff;
	BOOLEAN ascii;
	LONGINT W, H;
	CmdlnTexts_Handler handle;
	CmdlnTexts_Text base;
	Files_File file;
	LONGINT org, span;
	CHAR mod[32], proc[32];
};

typedef
	struct CmdlnTexts__1 *CmdlnTexts_Alien;

typedef
	struct CmdlnTexts_BufDesc {
		LONGINT len;
		CmdlnTexts_Run head;
	} CmdlnTexts_BufDesc;

typedef
	CmdlnTexts_BufDesc *CmdlnTexts_Buffer;

typedef
	struct CmdlnTexts_CopyMsg { /* CmdlnTexts_ElemMsg */
		CmdlnTexts_Elem e;
	} CmdlnTexts_CopyMsg;

typedef
	struct CmdlnTexts_FileMsg { /* CmdlnTexts_ElemMsg */
		INTEGER id;
		LONGINT pos;
		Files_Rider r;
	} CmdlnTexts_FileMsg;

typedef
	struct CmdlnTexts_FontDesc {
		CHAR name[32];
	} CmdlnTexts_FontDesc;

typedef
	struct CmdlnTexts_IdentifyMsg { /* CmdlnTexts_ElemMsg */
		CHAR mod[32], proc[32];
	} CmdlnTexts_IdentifyMsg;

typedef
	struct CmdlnTexts_PieceDesc *CmdlnTexts_Piece;

typedef
	struct CmdlnTexts_PieceDesc {
		CmdlnTexts_Run prev, next;
		LONGINT len;
		CmdlnTexts_FontsFont fnt;
		SHORTINT col, voff;
		BOOLEAN ascii;
		Files_File file;
		LONGINT org;
	} CmdlnTexts_PieceDesc;

typedef
	struct CmdlnTexts_Reader {
		BOOLEAN eot;
		CmdlnTexts_FontsFont fnt;
		SHORTINT col, voff;
		CmdlnTexts_Elem elem;
		Files_Rider rider;
		CmdlnTexts_Run run;
		LONGINT org, off;
	} CmdlnTexts_Reader;

typedef
	struct CmdlnTexts_Scanner { /* CmdlnTexts_Reader */
		BOOLEAN eot;
		CmdlnTexts_FontsFont fnt;
		SHORTINT col, voff;
		CmdlnTexts_Elem elem;
		Files_Rider rider;
		CmdlnTexts_Run run;
		LONGINT org, off;
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
	struct CmdlnTexts_TextDesc {
		LONGINT len;
		CmdlnTexts_Run head, cache;
		LONGINT corg;
	} CmdlnTexts_TextDesc;

typedef
	struct CmdlnTexts_Writer {
		CmdlnTexts_Buffer buf;
		CmdlnTexts_FontsFont fnt;
		SHORTINT col, voff;
		Files_Rider rider;
		Files_File file;
	} CmdlnTexts_Writer;


export CmdlnTexts_Elem CmdlnTexts_new;
static CmdlnTexts_Buffer CmdlnTexts_del;
static CmdlnTexts_FontsFont CmdlnTexts_FontsDefault;

export LONGINT *CmdlnTexts_FontDesc__typ;
export LONGINT *CmdlnTexts_RunDesc__typ;
export LONGINT *CmdlnTexts_PieceDesc__typ;
export LONGINT *CmdlnTexts_ElemMsg__typ;
export LONGINT *CmdlnTexts_ElemDesc__typ;
export LONGINT *CmdlnTexts_FileMsg__typ;
export LONGINT *CmdlnTexts_CopyMsg__typ;
export LONGINT *CmdlnTexts_IdentifyMsg__typ;
export LONGINT *CmdlnTexts_BufDesc__typ;
export LONGINT *CmdlnTexts_TextDesc__typ;
export LONGINT *CmdlnTexts_Reader__typ;
export LONGINT *CmdlnTexts_Scanner__typ;
export LONGINT *CmdlnTexts_Writer__typ;
export LONGINT *CmdlnTexts__1__typ;

export void CmdlnTexts_Append (CmdlnTexts_Text T, CmdlnTexts_Buffer B);
export void CmdlnTexts_ChangeLooks (CmdlnTexts_Text T, LONGINT beg, LONGINT end, SET sel, CmdlnTexts_FontsFont fnt, SHORTINT col, SHORTINT voff);
static CmdlnTexts_Elem CmdlnTexts_CloneElem (CmdlnTexts_Elem e);
static CmdlnTexts_Piece CmdlnTexts_ClonePiece (CmdlnTexts_Piece p);
export void CmdlnTexts_Close (CmdlnTexts_Text T, CHAR *name, LONGINT name__len);
export void CmdlnTexts_Copy (CmdlnTexts_Buffer SB, CmdlnTexts_Buffer DB);
export void CmdlnTexts_CopyElem (CmdlnTexts_Elem SE, CmdlnTexts_Elem DE);
export void CmdlnTexts_Delete (CmdlnTexts_Text T, LONGINT beg, LONGINT end);
export CmdlnTexts_Text CmdlnTexts_ElemBase (CmdlnTexts_Elem E);
export LONGINT CmdlnTexts_ElemPos (CmdlnTexts_Elem E);
static void CmdlnTexts_Find (CmdlnTexts_Text T, LONGINT *pos, CmdlnTexts_Run *u, LONGINT *org, LONGINT *off);
static CmdlnTexts_FontsFont CmdlnTexts_FontsThis (CHAR *name, LONGINT name__len);
static void CmdlnTexts_HandleAlien (CmdlnTexts_Elem E, CmdlnTexts_ElemMsg *msg, LONGINT *msg__typ);
export void CmdlnTexts_Insert (CmdlnTexts_Text T, LONGINT pos, CmdlnTexts_Buffer B);
export void CmdlnTexts_Load (Files_Rider *r, LONGINT *r__typ, CmdlnTexts_Text T);
static void CmdlnTexts_Load0 (Files_Rider *r, LONGINT *r__typ, CmdlnTexts_Text T);
static void CmdlnTexts_Merge (CmdlnTexts_Text T, CmdlnTexts_Run u, CmdlnTexts_Run *v);
export void CmdlnTexts_Open (CmdlnTexts_Text T, CHAR *name, LONGINT name__len);
export void CmdlnTexts_OpenBuf (CmdlnTexts_Buffer B);
export void CmdlnTexts_OpenReader (CmdlnTexts_Reader *R, LONGINT *R__typ, CmdlnTexts_Text T, LONGINT pos);
export void CmdlnTexts_OpenScanner (CmdlnTexts_Scanner *S, LONGINT *S__typ, CmdlnTexts_Text T, LONGINT pos);
export void CmdlnTexts_OpenWriter (CmdlnTexts_Writer *W, LONGINT *W__typ);
export LONGINT CmdlnTexts_Pos (CmdlnTexts_Reader *R, LONGINT *R__typ);
export void CmdlnTexts_Read (CmdlnTexts_Reader *R, LONGINT *R__typ, CHAR *ch);
export void CmdlnTexts_ReadElem (CmdlnTexts_Reader *R, LONGINT *R__typ);
export void CmdlnTexts_ReadPrevElem (CmdlnTexts_Reader *R, LONGINT *R__typ);
export void CmdlnTexts_Recall (CmdlnTexts_Buffer *B);
export void CmdlnTexts_Save (CmdlnTexts_Text T, LONGINT beg, LONGINT end, CmdlnTexts_Buffer B);
export void CmdlnTexts_Scan (CmdlnTexts_Scanner *S, LONGINT *S__typ);
export void CmdlnTexts_SetColor (CmdlnTexts_Writer *W, LONGINT *W__typ, SHORTINT col);
export void CmdlnTexts_SetFont (CmdlnTexts_Writer *W, LONGINT *W__typ, CmdlnTexts_FontsFont fnt);
export void CmdlnTexts_SetOffset (CmdlnTexts_Writer *W, LONGINT *W__typ, SHORTINT voff);
static void CmdlnTexts_Splice (CmdlnTexts_Run un, CmdlnTexts_Run v, CmdlnTexts_Run w, CmdlnTexts_Text base);
static void CmdlnTexts_Split (LONGINT off, CmdlnTexts_Run *u, CmdlnTexts_Run *un);
export void CmdlnTexts_Store (Files_Rider *r, LONGINT *r__typ, CmdlnTexts_Text T);
export void CmdlnTexts_Write (CmdlnTexts_Writer *W, LONGINT *W__typ, CHAR ch);
export void CmdlnTexts_WriteDate (CmdlnTexts_Writer *W, LONGINT *W__typ, LONGINT t, LONGINT d);
export void CmdlnTexts_WriteElem (CmdlnTexts_Writer *W, LONGINT *W__typ, CmdlnTexts_Elem e);
export void CmdlnTexts_WriteHex (CmdlnTexts_Writer *W, LONGINT *W__typ, LONGINT x);
export void CmdlnTexts_WriteInt (CmdlnTexts_Writer *W, LONGINT *W__typ, LONGINT x, LONGINT n);
export void CmdlnTexts_WriteLn (CmdlnTexts_Writer *W, LONGINT *W__typ);
export void CmdlnTexts_WriteLongReal (CmdlnTexts_Writer *W, LONGINT *W__typ, LONGREAL x, INTEGER n);
export void CmdlnTexts_WriteLongRealHex (CmdlnTexts_Writer *W, LONGINT *W__typ, LONGREAL x);
export void CmdlnTexts_WriteReal (CmdlnTexts_Writer *W, LONGINT *W__typ, REAL x, INTEGER n);
export void CmdlnTexts_WriteRealFix (CmdlnTexts_Writer *W, LONGINT *W__typ, REAL x, INTEGER n, INTEGER k);
export void CmdlnTexts_WriteRealHex (CmdlnTexts_Writer *W, LONGINT *W__typ, REAL x);
export void CmdlnTexts_WriteString (CmdlnTexts_Writer *W, LONGINT *W__typ, CHAR *s, LONGINT s__len);


/*============================================================================*/

static CmdlnTexts_FontsFont CmdlnTexts_FontsThis (CHAR *name, LONGINT name__len)
{
	CmdlnTexts_FontsFont F = NIL;
	__NEW(F, CmdlnTexts_FontDesc);
	__COPY(name, F->name, 32);
	return F;
}

static void CmdlnTexts_Find (CmdlnTexts_Text T, LONGINT *pos, CmdlnTexts_Run *u, LONGINT *org, LONGINT *off)
{
	CmdlnTexts_Run v = NIL;
	LONGINT m;
	if (*pos >= T->len) {
		*pos = T->len;
		*u = T->head;
		*org = T->len;
		*off = 0;
		T->cache = T->head;
		T->corg = 0;
	} else {
		v = T->cache->next;
		m = *pos - T->corg;
		if (*pos >= T->corg) {
			while (m >= v->len) {
				m -= v->len;
				v = v->next;
			}
		} else {
			while (m < 0) {
				v = v->prev;
				m += v->len;
			}
		}
		*u = v;
		*org = *pos - m;
		*off = m;
		T->cache = v->prev;
		T->corg = *org;
	}
}

static void CmdlnTexts_Split (LONGINT off, CmdlnTexts_Run *u, CmdlnTexts_Run *un)
{
	CmdlnTexts_Piece p = NIL, U = NIL;
	if (off == 0) {
		*un = *u;
		*u = (*un)->prev;
	} else if (off >= (*u)->len) {
		*un = (*u)->next;
	} else {
		__NEW(p, CmdlnTexts_PieceDesc);
		*un = (CmdlnTexts_Run)p;
		U = __GUARDP(*u, CmdlnTexts_PieceDesc, 1);
		__GUARDEQP(p, CmdlnTexts_PieceDesc) = *U;
		p->org += off;
		p->len -= off;
		U->len -= p->len;
		p->ascii = (*u)->ascii;
		p->prev = (CmdlnTexts_Run)U;
		p->next = U->next;
		p->next->prev = (CmdlnTexts_Run)p;
		U->next = (CmdlnTexts_Run)p;
	}
}

static void CmdlnTexts_Merge (CmdlnTexts_Text T, CmdlnTexts_Run u, CmdlnTexts_Run *v)
{
	CmdlnTexts_Piece p = NIL, q = NIL;
	if (((((__ISP(u, CmdlnTexts_PieceDesc, 1) && __ISP(*v, CmdlnTexts_PieceDesc, 1)) && __STRCMP(u->fnt->name, (*v)->fnt->name) == 0) && u->col == (*v)->col) && u->voff == (*v)->voff) && __GUARDP(u, CmdlnTexts_PieceDesc, 1)->ascii == __GUARDP(*v, CmdlnTexts_PieceDesc, 1)->ascii) {
		p = __GUARDP(u, CmdlnTexts_PieceDesc, 1);
		q = __GUARDP(*v, CmdlnTexts_PieceDesc, 1);
		if (p->file == q->file && p->org + p->len == q->org) {
			if (T->cache == u) {
				T->corg += q->len;
			} else if (T->cache == *v) {
				T->cache = T->head;
				T->corg = 0;
			}
			p->len += q->len;
			*v = (*v)->next;
		}
	}
}

static void CmdlnTexts_Splice (CmdlnTexts_Run un, CmdlnTexts_Run v, CmdlnTexts_Run w, CmdlnTexts_Text base)
{
	CmdlnTexts_Run u = NIL;
	if (v != w->next) {
		u = un->prev;
		u->next = v;
		v->prev = u;
		un->prev = w;
		w->next = un;
		do {
			if (__ISP(v, CmdlnTexts_ElemDesc, 1)) {
				__GUARDP(v, CmdlnTexts_ElemDesc, 1)->base = base;
			}
			v = v->next;
		} while (!(v == un));
	}
}

static CmdlnTexts_Piece CmdlnTexts_ClonePiece (CmdlnTexts_Piece p)
{
	CmdlnTexts_Piece q = NIL;
	__NEW(q, CmdlnTexts_PieceDesc);
	__GUARDEQP(q, CmdlnTexts_PieceDesc) = *p;
	return q;
}

static CmdlnTexts_Elem CmdlnTexts_CloneElem (CmdlnTexts_Elem e)
{
	CmdlnTexts_CopyMsg msg;
	msg.e = NIL;
	(*e->handle)(e, (void*)&msg, CmdlnTexts_CopyMsg__typ);
	return msg.e;
}

void CmdlnTexts_CopyElem (CmdlnTexts_Elem SE, CmdlnTexts_Elem DE)
{
	DE->len = SE->len;
	DE->fnt = SE->fnt;
	DE->col = SE->col;
	DE->voff = SE->voff;
	DE->W = SE->W;
	DE->H = SE->H;
	DE->handle = SE->handle;
}

/*----------------------------------------------------------------------------*/
CmdlnTexts_Text CmdlnTexts_ElemBase (CmdlnTexts_Elem E)
{
	return E->base;
}

/*----------------------------------------------------------------------------*/
LONGINT CmdlnTexts_ElemPos (CmdlnTexts_Elem E)
{
	CmdlnTexts_Run u = NIL;
	LONGINT pos;
	u = E->base->head->next;
	pos = 0;
	while (u != (void *) E) {
		pos = pos + u->len;
		u = u->next;
	}
	return pos;
}

/*----------------------------------------------------------------------------*/
static void CmdlnTexts_HandleAlien (CmdlnTexts_Elem E, CmdlnTexts_ElemMsg *msg, LONGINT *msg__typ)
{
	CmdlnTexts_Alien e = NIL;
	Files_Rider r;
	LONGINT i;
	CHAR ch;
	if (__ISP(E, CmdlnTexts__1, 2)) {
		if (__IS(msg__typ, CmdlnTexts_CopyMsg, 1)) {
			if (__IS(msg__typ, CmdlnTexts_CopyMsg, 1)) {
				CmdlnTexts_CopyMsg *msg__ = (void*)msg;
				__NEW(e, CmdlnTexts__1);
				CmdlnTexts_CopyElem((void*)((CmdlnTexts_Alien)E), (void*)e);
				e->file = ((CmdlnTexts_Alien)E)->file;
				e->org = ((CmdlnTexts_Alien)E)->org;
				e->span = ((CmdlnTexts_Alien)E)->span;
				__MOVE(((CmdlnTexts_Alien)E)->mod, e->mod, 32);
				__MOVE(((CmdlnTexts_Alien)E)->proc, e->proc, 32);
				(*msg__).e = (CmdlnTexts_Elem)e;
			} else __WITHCHK;
		} else if (__IS(msg__typ, CmdlnTexts_IdentifyMsg, 1)) {
			if (__IS(msg__typ, CmdlnTexts_IdentifyMsg, 1)) {
				CmdlnTexts_IdentifyMsg *msg__ = (void*)msg;
				__COPY(((CmdlnTexts_Alien)E)->mod, (*msg__).mod, 32);
				__COPY(((CmdlnTexts_Alien)E)->proc, (*msg__).proc, 32);
				(*msg__).mod[31] = 0x01;
			} else __WITHCHK;
		} else if (__IS(msg__typ, CmdlnTexts_FileMsg, 1)) {
			if (__IS(msg__typ, CmdlnTexts_FileMsg, 1)) {
				CmdlnTexts_FileMsg *msg__ = (void*)msg;
				if ((*msg__).id == 1) {
					Files_Set(&r, Files_Rider__typ, ((CmdlnTexts_Alien)E)->file, ((CmdlnTexts_Alien)E)->org);
					i = ((CmdlnTexts_Alien)E)->span;
					while (i > 0) {
						Files_Read(&r, Files_Rider__typ, (void*)&ch);
						Files_Write(&(*msg__).r, Files_Rider__typ, ch);
						i -= 1;
					}
				}
			} else __WITHCHK;
		}
	} else __WITHCHK;
}

void CmdlnTexts_OpenBuf (CmdlnTexts_Buffer B)
{
	CmdlnTexts_Run u = NIL;
	__NEW(u, CmdlnTexts_RunDesc);
	u->next = u;
	u->prev = u;
	B->head = u;
	B->len = 0;
}

/*----------------------------------------------------------------------------*/
void CmdlnTexts_Copy (CmdlnTexts_Buffer SB, CmdlnTexts_Buffer DB)
{
	CmdlnTexts_Run u = NIL, v = NIL, vn = NIL;
	u = SB->head->next;
	v = DB->head->prev;
	while (u != SB->head) {
		if (__ISP(u, CmdlnTexts_PieceDesc, 1)) {
			vn = (CmdlnTexts_Run)CmdlnTexts_ClonePiece(__GUARDP(u, CmdlnTexts_PieceDesc, 1));
		} else {
			vn = (CmdlnTexts_Run)CmdlnTexts_CloneElem(__GUARDP(u, CmdlnTexts_ElemDesc, 1));
		}
		v->next = vn;
		vn->prev = v;
		v = vn;
		u = u->next;
	}
	v->next = DB->head;
	DB->head->prev = v;
	DB->len += SB->len;
}

/*----------------------------------------------------------------------------*/
void CmdlnTexts_Recall (CmdlnTexts_Buffer *B)
{
	*B = CmdlnTexts_del;
	CmdlnTexts_del = NIL;
}

/*----------------------------------------------------------------------------*/
void CmdlnTexts_Save (CmdlnTexts_Text T, LONGINT beg, LONGINT end, CmdlnTexts_Buffer B)
{
	CmdlnTexts_Run u = NIL, v = NIL, w = NIL, wn = NIL;
	LONGINT uo, ud, vo, vd;
	CmdlnTexts_Find(T, &beg, &u, &uo, &ud);
	CmdlnTexts_Find(T, &end, &v, &vo, &vd);
	w = B->head->prev;
	while (u != v) {
		if (__ISP(u, CmdlnTexts_PieceDesc, 1)) {
			wn = (CmdlnTexts_Run)CmdlnTexts_ClonePiece(__GUARDP(u, CmdlnTexts_PieceDesc, 1));
			wn->len -= ud;
			__GUARDP(wn, CmdlnTexts_PieceDesc, 1)->org += ud;
		} else {
			wn = (CmdlnTexts_Run)CmdlnTexts_CloneElem(__GUARDP(u, CmdlnTexts_ElemDesc, 1));
		}
		w->next = wn;
		wn->prev = w;
		w = wn;
		u = u->next;
		ud = 0;
	}
	if (vd > 0) {
		wn = (CmdlnTexts_Run)CmdlnTexts_ClonePiece(__GUARDP(v, CmdlnTexts_PieceDesc, 1));
		wn->len = vd - ud;
		__GUARDP(wn, CmdlnTexts_PieceDesc, 1)->org += ud;
		w->next = wn;
		wn->prev = w;
		w = wn;
	}
	w->next = B->head;
	B->head->prev = w;
	B->len += end - beg;
}

/*----------------------------------------------------------------------------*/
void CmdlnTexts_Insert (CmdlnTexts_Text T, LONGINT pos, CmdlnTexts_Buffer B)
{
	CmdlnTexts_Run u = NIL, un = NIL, v = NIL;
	CmdlnTexts_Piece p = NIL, q = NIL;
	LONGINT uo, ud, len;
	CmdlnTexts_Find(T, &pos, &u, &uo, &ud);
	CmdlnTexts_Split(ud, &u, &un);
	len = B->len;
	v = B->head->next;
	CmdlnTexts_Merge(T, u, &v);
	CmdlnTexts_Splice(un, v, B->head->prev, T);
	T->len += len;
	B->head->next = B->head;
	B->head->prev = B->head;
	B->len = 0;
}

/*----------------------------------------------------------------------------*/
void CmdlnTexts_Append (CmdlnTexts_Text T, CmdlnTexts_Buffer B)
{
	CmdlnTexts_Run v = NIL;
	LONGINT pos, len;
	pos = T->len;
	len = B->len;
	v = B->head->next;
	CmdlnTexts_Merge(T, T->head->prev, &v);
	CmdlnTexts_Splice(T->head, v, B->head->prev, T);
	T->len += len;
	B->head->next = B->head;
	B->head->prev = B->head;
	B->len = 0;
}

/*----------------------------------------------------------------------------*/
void CmdlnTexts_Delete (CmdlnTexts_Text T, LONGINT beg, LONGINT end)
{
	CmdlnTexts_Run c = NIL, u = NIL, un = NIL, v = NIL, vn = NIL;
	LONGINT co, uo, ud, vo, vd;
	CmdlnTexts_Find(T, &beg, &u, &uo, &ud);
	CmdlnTexts_Split(ud, &u, &un);
	c = T->cache;
	co = T->corg;
	CmdlnTexts_Find(T, &end, &v, &vo, &vd);
	CmdlnTexts_Split(vd, &v, &vn);
	T->cache = c;
	T->corg = co;
	__NEW(CmdlnTexts_del, CmdlnTexts_BufDesc);
	CmdlnTexts_OpenBuf(CmdlnTexts_del);
	CmdlnTexts_del->len = end - beg;
	CmdlnTexts_Splice(CmdlnTexts_del->head, un, v, NIL);
	CmdlnTexts_Merge(T, u, &vn);
	u->next = vn;
	vn->prev = u;
	T->len -= end - beg;
}

/*----------------------------------------------------------------------------*/
void CmdlnTexts_ChangeLooks (CmdlnTexts_Text T, LONGINT beg, LONGINT end, SET sel, CmdlnTexts_FontsFont fnt, SHORTINT col, SHORTINT voff)
{
	CmdlnTexts_Run c = NIL, u = NIL, un = NIL, v = NIL, vn = NIL;
	LONGINT co, uo, ud, vo, vd;
	CmdlnTexts_Find(T, &beg, &u, &uo, &ud);
	CmdlnTexts_Split(ud, &u, &un);
	c = T->cache;
	co = T->corg;
	CmdlnTexts_Find(T, &end, &v, &vo, &vd);
	CmdlnTexts_Split(vd, &v, &vn);
	T->cache = c;
	T->corg = co;
	while (un != vn) {
		if (__IN(0, sel) && fnt != NIL) {
			un->fnt = fnt;
		}
		if (__IN(1, sel)) {
			un->col = col;
		}
		if (__IN(2, sel)) {
			un->voff = voff;
		}
		CmdlnTexts_Merge(T, u, &un);
		if (u->next == un) {
			u = un;
			un = un->next;
		} else {
			u->next = un;
			un->prev = u;
		}
	}
	CmdlnTexts_Merge(T, u, &un);
	u->next = un;
	un->prev = u;
}

/*----------------------------------------------------------------------------*/
void CmdlnTexts_OpenReader (CmdlnTexts_Reader *R, LONGINT *R__typ, CmdlnTexts_Text T, LONGINT pos)
{
	CmdlnTexts_Run u = NIL;
	if (pos >= T->len) {
		pos = T->len;
	}
	CmdlnTexts_Find(T, &pos, &u, &(*R).org, &(*R).off);
	(*R).run = u;
	(*R).eot = 0;
	if (__ISP(u, CmdlnTexts_PieceDesc, 1)) {
		Files_Set(&(*R).rider, Files_Rider__typ, __GUARDP(u, CmdlnTexts_PieceDesc, 1)->file, __GUARDP(u, CmdlnTexts_PieceDesc, 1)->org + (*R).off);
	}
}

/*----------------------------------------------------------------------------*/
void CmdlnTexts_Read (CmdlnTexts_Reader *R, LONGINT *R__typ, CHAR *ch)
{
	CmdlnTexts_Run u = NIL;
	LONGINT pos;
	CHAR nextch;
	u = (*R).run;
	(*R).fnt = u->fnt;
	(*R).col = u->col;
	(*R).voff = u->voff;
	(*R).off += 1;
	if (__ISP(u, CmdlnTexts_PieceDesc, 1)) {
		Files_Read(&(*R).rider, Files_Rider__typ, (void*)&*ch);
		(*R).elem = NIL;
		if (*ch == 0x0a && __GUARDP(u, CmdlnTexts_PieceDesc, 1)->ascii) {
			*ch = 0x0d;
		} else if (*ch == 0x0d && __GUARDP(u, CmdlnTexts_PieceDesc, 1)->ascii) {
			pos = Files_Pos(&(*R).rider, Files_Rider__typ);
			Files_Read(&(*R).rider, Files_Rider__typ, (void*)&nextch);
			if (nextch == 0x0a) {
				(*R).off += 1;
			} else {
				Files_Set(&(*R).rider, Files_Rider__typ, __GUARDP(u, CmdlnTexts_PieceDesc, 1)->file, pos);
			}
		}
	} else if (__ISP(u, CmdlnTexts_ElemDesc, 1)) {
		*ch = 0x1c;
		(*R).elem = __GUARDP(u, CmdlnTexts_ElemDesc, 1);
	} else {
		*ch = 0x00;
		(*R).elem = NIL;
		(*R).eot = 1;
	}
	if ((*R).off == u->len) {
		(*R).org += u->len;
		u = u->next;
		if (__ISP(u, CmdlnTexts_PieceDesc, 1)) {
			if (__ISP(u, CmdlnTexts_PieceDesc, 1)) {
				Files_Set(&(*R).rider, Files_Rider__typ, ((CmdlnTexts_Piece)u)->file, ((CmdlnTexts_Piece)u)->org);
			} else __WITHCHK;
		}
		(*R).run = u;
		(*R).off = 0;
	}
}

/*----------------------------------------------------------------------------*/
void CmdlnTexts_ReadElem (CmdlnTexts_Reader *R, LONGINT *R__typ)
{
	CmdlnTexts_Run u = NIL, un = NIL;
	u = (*R).run;
	while (__ISP(u, CmdlnTexts_PieceDesc, 1)) {
		(*R).org += u->len;
		u = u->next;
	}
	if (__ISP(u, CmdlnTexts_ElemDesc, 1)) {
		un = u->next;
		(*R).run = un;
		(*R).org += 1;
		(*R).off = 0;
		(*R).fnt = u->fnt;
		(*R).col = u->col;
		(*R).voff = u->voff;
		(*R).elem = __GUARDP(u, CmdlnTexts_ElemDesc, 1);
		if (__ISP(un, CmdlnTexts_PieceDesc, 1)) {
			if (__ISP(un, CmdlnTexts_PieceDesc, 1)) {
				Files_Set(&(*R).rider, Files_Rider__typ, ((CmdlnTexts_Piece)un)->file, ((CmdlnTexts_Piece)un)->org);
			} else __WITHCHK;
		}
	} else {
		(*R).eot = 1;
		(*R).elem = NIL;
	}
}

/*----------------------------------------------------------------------------*/
void CmdlnTexts_ReadPrevElem (CmdlnTexts_Reader *R, LONGINT *R__typ)
{
	CmdlnTexts_Run u = NIL;
	u = (*R).run->prev;
	while (__ISP(u, CmdlnTexts_PieceDesc, 1)) {
		(*R).org -= u->len;
		u = u->prev;
	}
	if (__ISP(u, CmdlnTexts_ElemDesc, 1)) {
		(*R).run = u;
		(*R).org -= 1;
		(*R).off = 0;
		(*R).fnt = u->fnt;
		(*R).col = u->col;
		(*R).voff = u->voff;
		(*R).elem = __GUARDP(u, CmdlnTexts_ElemDesc, 1);
	} else {
		(*R).eot = 1;
		(*R).elem = NIL;
	}
}

/*----------------------------------------------------------------------------*/
LONGINT CmdlnTexts_Pos (CmdlnTexts_Reader *R, LONGINT *R__typ)
{
	return (*R).org + (*R).off;
}

/*----------------------------------------------------------------------------*/
void CmdlnTexts_OpenScanner (CmdlnTexts_Scanner *S, LONGINT *S__typ, CmdlnTexts_Text T, LONGINT pos)
{
	CmdlnTexts_OpenReader((void*)&*S, S__typ, T, pos);
	(*S).line = 0;
	(*S).nextCh = ' ';
}

/*----------------------------------------------------------------------------*/
static struct Scan__31 {
	CmdlnTexts_Scanner *S;
	LONGINT *S__typ;
	CHAR *ch;
	BOOLEAN *negE;
	INTEGER *e;
	struct Scan__31 *lnk;
} *Scan__31_s;

static void ReadScaleFactor__32 (void);

static void ReadScaleFactor__32 (void)
{
	CmdlnTexts_Read((void*)&*Scan__31_s->S, Scan__31_s->S__typ, &*Scan__31_s->ch);
	if (*Scan__31_s->ch == '-') {
		*Scan__31_s->negE = 1;
		CmdlnTexts_Read((void*)&*Scan__31_s->S, Scan__31_s->S__typ, &*Scan__31_s->ch);
	} else {
		*Scan__31_s->negE = 0;
		if (*Scan__31_s->ch == '+') {
			CmdlnTexts_Read((void*)&*Scan__31_s->S, Scan__31_s->S__typ, &*Scan__31_s->ch);
		}
	}
	while ('0' <= *Scan__31_s->ch && *Scan__31_s->ch <= '9') {
		*Scan__31_s->e = (*Scan__31_s->e * 10 + (int)*Scan__31_s->ch) - 48;
		CmdlnTexts_Read((void*)&*Scan__31_s->S, Scan__31_s->S__typ, &*Scan__31_s->ch);
	}
}

void CmdlnTexts_Scan (CmdlnTexts_Scanner *S, LONGINT *S__typ)
{
	CHAR ch, term;
	BOOLEAN neg, negE, hex;
	SHORTINT i, j, h;
	INTEGER e;
	LONGINT k;
	REAL x, f;
	LONGREAL y, g;
	CHAR d[32];
	struct Scan__31 _s;
	_s.S = S; _s.S__typ = S__typ;
	_s.ch = &ch;
	_s.negE = &negE;
	_s.e = &e;
	_s.lnk = Scan__31_s;
	Scan__31_s = &_s;
	ch = (*S).nextCh;
	i = 0;
	for (;;) {
		if (ch == 0x0d) {
			(*S).line += 1;
		} else if (ch != ' ' && ch != 0x09) {
			break;
		}
		CmdlnTexts_Read((void*)&*S, S__typ, &ch);
	}
	if (('A' <= __CAP(ch) && __CAP(ch) <= 'Z' || ch == '/') || ch == '.') {
		do {
			(*S).s[__X(i, 64)] = ch;
			i += 1;
			CmdlnTexts_Read((void*)&*S, S__typ, &ch);
		} while (!(((__CAP(ch) > 'Z' && ch != '_' || 'A' > __CAP(ch) && ch > '9') || ('0' > ch && ch != '.') && ch != '/') || i == 63));
		(*S).s[__X(i, 64)] = 0x00;
		(*S).len = i;
		(*S).class = 1;
	} else if (ch == '\"') {
		CmdlnTexts_Read((void*)&*S, S__typ, &ch);
		while ((ch != '\"' && ch >= ' ') && i != 63) {
			(*S).s[__X(i, 64)] = ch;
			i += 1;
			CmdlnTexts_Read((void*)&*S, S__typ, &ch);
		}
		(*S).s[__X(i, 64)] = 0x00;
		(*S).len = i + 1;
		CmdlnTexts_Read((void*)&*S, S__typ, &ch);
		(*S).class = 2;
	} else {
		if (ch == '-') {
			neg = 1;
			CmdlnTexts_Read((void*)&*S, S__typ, &ch);
		} else {
			neg = 0;
		}
		if ('0' <= ch && ch <= '9') {
			hex = 0;
			j = 0;
			for (;;) {
				d[__X(i, 32)] = ch;
				i += 1;
				CmdlnTexts_Read((void*)&*S, S__typ, &ch);
				if (ch < '0') {
					break;
				}
				if ('9' < ch) {
					if ('A' <= ch && ch <= 'F') {
						hex = 1;
						ch = (CHAR)((int)ch - 7);
					} else if ('a' <= ch && ch <= 'f') {
						hex = 1;
						ch = (CHAR)((int)ch - 39);
					} else {
						break;
					}
				}
			}
			if (ch == 'H') {
				CmdlnTexts_Read((void*)&*S, S__typ, &ch);
				(*S).class = 3;
				if (i - j > 8) {
					j = i - 8;
				}
				k = (int)d[__X(j, 32)] - 48;
				j += 1;
				if (i - j == 7 && k >= 8) {
					k -= 16;
				}
				while (j < i) {
					k = __ASHL(k, 4) + (LONGINT)((int)d[__X(j, 32)] - 48);
					j += 1;
				}
				if (neg) {
					(*S).i = -k;
				} else {
					(*S).i = k;
				}
			} else if (ch == '.') {
				CmdlnTexts_Read((void*)&*S, S__typ, &ch);
				h = i;
				while ('0' <= ch && ch <= '9') {
					d[__X(i, 32)] = ch;
					i += 1;
					CmdlnTexts_Read((void*)&*S, S__typ, &ch);
				}
				if (ch == 'D') {
					e = 0;
					y = (LONGREAL)0;
					g = (LONGREAL)1;
					do {
						y = y * (LONGREAL)10 + ((int)d[__X(j, 32)] - 48);
						j += 1;
					} while (!(j == h));
					while (j < i) {
						g = g / (LONGREAL)(LONGREAL)10;
						y = ((int)d[__X(j, 32)] - 48) * g + y;
						j += 1;
					}
					ReadScaleFactor__32();
					if (negE) {
						if (e <= 308) {
							y = y / (LONGREAL)Reals_TenL(e);
						} else {
							y = (LONGREAL)0;
						}
					} else if (e > 0) {
						if (e <= 308) {
							y = Reals_TenL(e) * y;
						} else {
							__HALT(40);
						}
					}
					if (neg) {
						y = -y;
					}
					(*S).class = 5;
					(*S).y = y;
				} else {
					e = 0;
					x = (REAL)0;
					f = (REAL)1;
					do {
						x = x * (REAL)10 + ((int)d[__X(j, 32)] - 48);
						j += 1;
					} while (!(j == h));
					while (j < i) {
						f = f / (REAL)(REAL)10;
						x = ((int)d[__X(j, 32)] - 48) * f + x;
						j += 1;
					}
					if (ch == 'E') {
						ReadScaleFactor__32();
					}
					if (negE) {
						if (e <= 38) {
							x = x / (REAL)Reals_Ten(e);
						} else {
							x = (REAL)0;
						}
					} else if (e > 0) {
						if (e <= 38) {
							x = Reals_Ten(e) * x;
						} else {
							__HALT(40);
						}
					}
					if (neg) {
						x = -x;
					}
					(*S).class = 4;
					(*S).x = x;
				}
				if (hex) {
					(*S).class = 0;
				}
			} else {
				(*S).class = 3;
				k = 0;
				do {
					k = k * 10 + (LONGINT)((int)d[__X(j, 32)] - 48);
					j += 1;
				} while (!(j == i));
				if (neg) {
					(*S).i = -k;
				} else {
					(*S).i = k;
				}
				if (hex) {
					(*S).class = 0;
				} else {
					(*S).class = 3;
				}
			}
		} else {
			(*S).class = 6;
			if (neg) {
				(*S).c = '-';
			} else {
				(*S).c = ch;
				CmdlnTexts_Read((void*)&*S, S__typ, &ch);
			}
		}
	}
	(*S).nextCh = ch;
	Scan__31_s = _s.lnk;
}

/*----------------------------------------------------------------------------*/
void CmdlnTexts_OpenWriter (CmdlnTexts_Writer *W, LONGINT *W__typ)
{
	__NEW((*W).buf, CmdlnTexts_BufDesc);
	CmdlnTexts_OpenBuf((*W).buf);
	(*W).fnt = CmdlnTexts_FontsDefault;
	(*W).col = 15;
	(*W).voff = 0;
	(*W).file = Files_New((CHAR*)"", (LONGINT)1);
	Files_Set(&(*W).rider, Files_Rider__typ, (*W).file, 0);
}

/*----------------------------------------------------------------------------*/
void CmdlnTexts_SetFont (CmdlnTexts_Writer *W, LONGINT *W__typ, CmdlnTexts_FontsFont fnt)
{
	(*W).fnt = fnt;
}

/*----------------------------------------------------------------------------*/
void CmdlnTexts_SetColor (CmdlnTexts_Writer *W, LONGINT *W__typ, SHORTINT col)
{
	(*W).col = col;
}

/*----------------------------------------------------------------------------*/
void CmdlnTexts_SetOffset (CmdlnTexts_Writer *W, LONGINT *W__typ, SHORTINT voff)
{
	(*W).voff = voff;
}

/*----------------------------------------------------------------------------*/
void CmdlnTexts_Write (CmdlnTexts_Writer *W, LONGINT *W__typ, CHAR ch)
{
	CmdlnTexts_Run u = NIL, un = NIL;
	CmdlnTexts_Piece p = NIL;
	Files_Write(&(*W).rider, Files_Rider__typ, ch);
	(*W).buf->len += 1;
	un = (*W).buf->head;
	u = un->prev;
	if (((((__ISP(u, CmdlnTexts_PieceDesc, 1) && __GUARDP(u, CmdlnTexts_PieceDesc, 1)->file == (*W).file) && __STRCMP(u->fnt->name, (*W).fnt->name) == 0) && u->col == (*W).col) && u->voff == (*W).voff) && !__GUARDP(u, CmdlnTexts_PieceDesc, 1)->ascii) {
		u->len += 1;
	} else {
		__NEW(p, CmdlnTexts_PieceDesc);
		u->next = (CmdlnTexts_Run)p;
		p->prev = u;
		p->next = un;
		un->prev = (CmdlnTexts_Run)p;
		p->len = 1;
		p->fnt = (*W).fnt;
		p->col = (*W).col;
		p->voff = (*W).voff;
		p->file = (*W).file;
		p->org = Files_Length((*W).file) - 1;
		p->ascii = 0;
	}
}

/*----------------------------------------------------------------------------*/
void CmdlnTexts_WriteElem (CmdlnTexts_Writer *W, LONGINT *W__typ, CmdlnTexts_Elem e)
{
	CmdlnTexts_Run u = NIL, un = NIL;
	if (e->base != NIL) {
		__HALT(99);
	}
	(*W).buf->len += 1;
	e->len = 1;
	e->fnt = (*W).fnt;
	e->col = (*W).col;
	e->voff = (*W).voff;
	un = (*W).buf->head;
	u = un->prev;
	u->next = (CmdlnTexts_Run)e;
	e->prev = u;
	e->next = un;
	un->prev = (CmdlnTexts_Run)e;
}

/*----------------------------------------------------------------------------*/
void CmdlnTexts_WriteLn (CmdlnTexts_Writer *W, LONGINT *W__typ)
{
	CmdlnTexts_Write(&*W, W__typ, 0x0d);
}

/*----------------------------------------------------------------------------*/
void CmdlnTexts_WriteString (CmdlnTexts_Writer *W, LONGINT *W__typ, CHAR *s, LONGINT s__len)
{
	INTEGER i;
	__DUP(s, s__len, CHAR);
	i = 0;
	while (s[__X(i, s__len)] >= ' ') {
		CmdlnTexts_Write(&*W, W__typ, s[__X(i, s__len)]);
		i += 1;
	}
	__DEL(s);
}

/*----------------------------------------------------------------------------*/
void CmdlnTexts_WriteInt (CmdlnTexts_Writer *W, LONGINT *W__typ, LONGINT x, LONGINT n)
{
	INTEGER i;
	LONGINT x0;
	CHAR a[11];
	i = 0;
	if (x < 0) {
		if (x == (-2147483647-1)) {
			CmdlnTexts_WriteString(&*W, W__typ, (CHAR*)" -2147483648", (LONGINT)13);
			return;
		} else {
			n -= 1;
			x0 = -x;
		}
	} else {
		x0 = x;
	}
	do {
		a[__X(i, 11)] = (CHAR)(__MOD(x0, 10) + 48);
		x0 = __DIV(x0, 10);
		i += 1;
	} while (!(x0 == 0));
	while (n > (LONGINT)i) {
		CmdlnTexts_Write(&*W, W__typ, ' ');
		n -= 1;
	}
	if (x < 0) {
		CmdlnTexts_Write(&*W, W__typ, '-');
	}
	do {
		i -= 1;
		CmdlnTexts_Write(&*W, W__typ, a[__X(i, 11)]);
	} while (!(i == 0));
}

/*----------------------------------------------------------------------------*/
void CmdlnTexts_WriteHex (CmdlnTexts_Writer *W, LONGINT *W__typ, LONGINT x)
{
	INTEGER i;
	LONGINT y;
	CHAR a[10];
	i = 0;
	CmdlnTexts_Write(&*W, W__typ, ' ');
	do {
		y = __MASK(x, -16);
		if (y < 10) {
			a[__X(i, 10)] = (CHAR)(y + 48);
		} else {
			a[__X(i, 10)] = (CHAR)(y + 55);
		}
		x = __ASHR(x, 4);
		i += 1;
	} while (!(i == 8));
	do {
		i -= 1;
		CmdlnTexts_Write(&*W, W__typ, a[__X(i, 10)]);
	} while (!(i == 0));
}

/*----------------------------------------------------------------------------*/
void CmdlnTexts_WriteReal (CmdlnTexts_Writer *W, LONGINT *W__typ, REAL x, INTEGER n)
{
	INTEGER e;
	REAL x0;
	CHAR d[9];
	e = Reals_Expo(x);
	if (e == 0) {
		CmdlnTexts_WriteString(&*W, W__typ, (CHAR*)"  0", (LONGINT)4);
		do {
			CmdlnTexts_Write(&*W, W__typ, ' ');
			n -= 1;
		} while (!(n <= 3));
	} else if (e == 255) {
		CmdlnTexts_WriteString(&*W, W__typ, (CHAR*)" NaN", (LONGINT)5);
		while (n > 4) {
			CmdlnTexts_Write(&*W, W__typ, ' ');
			n -= 1;
		}
	} else {
		if (n <= 9) {
			n = 3;
		} else {
			n -= 6;
		}
		do {
			CmdlnTexts_Write(&*W, W__typ, ' ');
			n -= 1;
		} while (!(n <= 8));
		if (x < (REAL)0) {
			CmdlnTexts_Write(&*W, W__typ, '-');
			x = -x;
		} else {
			CmdlnTexts_Write(&*W, W__typ, ' ');
		}
		e = __ASHR((e - 127) * 77, 8);
		if (e >= 0) {
			x = x / (REAL)Reals_Ten(e);
		} else {
			x = Reals_Ten(-e) * x;
		}
		if (x >= (REAL)10) {
			x = 0.1000000014901161 * x;
			e += 1;
		}
		x0 = Reals_Ten(n - 1);
		x = x0 * x + 0.5;
		if (x >= (REAL)10 * x0) {
			x = x * 0.1000000014901161;
			e += 1;
		}
		Reals_Convert(x, n, (void*)d, 9);
		n -= 1;
		CmdlnTexts_Write(&*W, W__typ, d[__X(n, 9)]);
		CmdlnTexts_Write(&*W, W__typ, '.');
		do {
			n -= 1;
			CmdlnTexts_Write(&*W, W__typ, d[__X(n, 9)]);
		} while (!(n == 0));
		CmdlnTexts_Write(&*W, W__typ, 'E');
		if (e < 0) {
			CmdlnTexts_Write(&*W, W__typ, '-');
			e = -e;
		} else {
			CmdlnTexts_Write(&*W, W__typ, '+');
		}
		CmdlnTexts_Write(&*W, W__typ, (CHAR)(__DIV(e, 10) + 48));
		CmdlnTexts_Write(&*W, W__typ, (CHAR)((int)__MOD(e, 10) + 48));
	}
}

/*----------------------------------------------------------------------------*/
static struct WriteRealFix__53 {
	CmdlnTexts_Writer *W;
	LONGINT *W__typ;
	INTEGER *i;
	CHAR (*d)[9];
	struct WriteRealFix__53 *lnk;
} *WriteRealFix__53_s;

static void dig__54 (INTEGER n);
static void seq__56 (CHAR ch, INTEGER n);

static void seq__56 (CHAR ch, INTEGER n)
{
	while (n > 0) {
		CmdlnTexts_Write(&*WriteRealFix__53_s->W, WriteRealFix__53_s->W__typ, ch);
		n -= 1;
	}
}

static void dig__54 (INTEGER n)
{
	while (n > 0) {
		*WriteRealFix__53_s->i -= 1;
		CmdlnTexts_Write(&*WriteRealFix__53_s->W, WriteRealFix__53_s->W__typ, (*WriteRealFix__53_s->d)[__X(*WriteRealFix__53_s->i, 9)]);
		n -= 1;
	}
}

void CmdlnTexts_WriteRealFix (CmdlnTexts_Writer *W, LONGINT *W__typ, REAL x, INTEGER n, INTEGER k)
{
	INTEGER e, i;
	CHAR sign;
	REAL x0;
	CHAR d[9];
	struct WriteRealFix__53 _s;
	_s.W = W; _s.W__typ = W__typ;
	_s.i = &i;
	_s.d = (void*)d;
	_s.lnk = WriteRealFix__53_s;
	WriteRealFix__53_s = &_s;
	e = Reals_Expo(x);
	if (k < 0) {
		k = 0;
	}
	if (e == 0) {
		seq__56(' ', (n - k) - 2);
		CmdlnTexts_Write(&*W, W__typ, '0');
		seq__56(' ', k + 1);
	} else if (e == 255) {
		CmdlnTexts_WriteString(&*W, W__typ, (CHAR*)" NaN", (LONGINT)5);
		seq__56(' ', n - 4);
	} else {
		e = __ASHR((e - 127) * 77, 8);
		if (x < (REAL)0) {
			sign = '-';
			x = -x;
		} else {
			sign = ' ';
		}
		if (e >= 0) {
			x = x / (REAL)Reals_Ten(e);
		} else {
			x = Reals_Ten(-e) * x;
		}
		if (x >= (REAL)10) {
			x = 0.1000000014901161 * x;
			e += 1;
		}
		if (k + e >= 8) {
			k = 8 - e;
		} else if (k + e < 0) {
			k = -e;
			x = (REAL)0;
		}
		x0 = Reals_Ten(k + e);
		x = x0 * x + 0.5;
		if (x >= (REAL)10 * x0) {
			e += 1;
		}
		e += 1;
		i = k + e;
		Reals_Convert(x, i, (void*)d, 9);
		if (e > 0) {
			seq__56(' ', ((n - e) - k) - 2);
			CmdlnTexts_Write(&*W, W__typ, sign);
			dig__54(e);
			CmdlnTexts_Write(&*W, W__typ, '.');
			dig__54(k);
		} else {
			seq__56(' ', (n - k) - 3);
			CmdlnTexts_Write(&*W, W__typ, sign);
			CmdlnTexts_Write(&*W, W__typ, '0');
			CmdlnTexts_Write(&*W, W__typ, '.');
			seq__56('0', -e);
			dig__54(k + e);
		}
	}
	WriteRealFix__53_s = _s.lnk;
}

/*----------------------------------------------------------------------------*/
void CmdlnTexts_WriteRealHex (CmdlnTexts_Writer *W, LONGINT *W__typ, REAL x)
{
	INTEGER i;
	CHAR d[8];
	Reals_ConvertH(x, (void*)d, 8);
	i = 0;
	do {
		CmdlnTexts_Write(&*W, W__typ, d[__X(i, 8)]);
		i += 1;
	} while (!(i == 8));
}

/*----------------------------------------------------------------------------*/
void CmdlnTexts_WriteLongReal (CmdlnTexts_Writer *W, LONGINT *W__typ, LONGREAL x, INTEGER n)
{
	INTEGER e;
	LONGREAL x0;
	CHAR d[16];
	e = Reals_ExpoL(x);
	if (e == 0) {
		CmdlnTexts_WriteString(&*W, W__typ, (CHAR*)"  0", (LONGINT)4);
		do {
			CmdlnTexts_Write(&*W, W__typ, ' ');
			n -= 1;
		} while (!(n <= 3));
	} else if (e == 2047) {
		CmdlnTexts_WriteString(&*W, W__typ, (CHAR*)" NaN", (LONGINT)5);
		while (n > 4) {
			CmdlnTexts_Write(&*W, W__typ, ' ');
			n -= 1;
		}
	} else {
		if (n <= 10) {
			n = 3;
		} else {
			n -= 7;
		}
		do {
			CmdlnTexts_Write(&*W, W__typ, ' ');
			n -= 1;
		} while (!(n <= 16));
		if (x < (LONGREAL)0) {
			CmdlnTexts_Write(&*W, W__typ, '-');
			x = -x;
		} else {
			CmdlnTexts_Write(&*W, W__typ, ' ');
		}
		e = (int)__ASHR((LONGINT)(e - 1023) * 77, 8);
		if (e >= 0) {
			x = x / (LONGREAL)Reals_TenL(e);
		} else {
			x = Reals_TenL(-e) * x;
		}
		if (x >= (LONGREAL)10) {
			x = 0.1 * x;
			e += 1;
		}
		x0 = Reals_TenL(n - 1);
		x = x0 * x + 0.5;
		if (x >= (LONGREAL)10 * x0) {
			x = 0.1 * x;
			e += 1;
		}
		Reals_ConvertL(x, n, (void*)d, 16);
		n -= 1;
		CmdlnTexts_Write(&*W, W__typ, d[__X(n, 16)]);
		CmdlnTexts_Write(&*W, W__typ, '.');
		do {
			n -= 1;
			CmdlnTexts_Write(&*W, W__typ, d[__X(n, 16)]);
		} while (!(n == 0));
		CmdlnTexts_Write(&*W, W__typ, 'D');
		if (e < 0) {
			CmdlnTexts_Write(&*W, W__typ, '-');
			e = -e;
		} else {
			CmdlnTexts_Write(&*W, W__typ, '+');
		}
		CmdlnTexts_Write(&*W, W__typ, (CHAR)(__DIV(e, 100) + 48));
		e = (int)__MOD(e, 100);
		CmdlnTexts_Write(&*W, W__typ, (CHAR)(__DIV(e, 10) + 48));
		CmdlnTexts_Write(&*W, W__typ, (CHAR)((int)__MOD(e, 10) + 48));
	}
}

/*----------------------------------------------------------------------------*/
void CmdlnTexts_WriteLongRealHex (CmdlnTexts_Writer *W, LONGINT *W__typ, LONGREAL x)
{
	INTEGER i;
	CHAR d[16];
	Reals_ConvertHL(x, (void*)d, 16);
	i = 0;
	do {
		CmdlnTexts_Write(&*W, W__typ, d[__X(i, 16)]);
		i += 1;
	} while (!(i == 16));
}

/*----------------------------------------------------------------------------*/
static struct WriteDate__43 {
	CmdlnTexts_Writer *W;
	LONGINT *W__typ;
	struct WriteDate__43 *lnk;
} *WriteDate__43_s;

static void WritePair__44 (CHAR ch, LONGINT x);

static void WritePair__44 (CHAR ch, LONGINT x)
{
	CmdlnTexts_Write(&*WriteDate__43_s->W, WriteDate__43_s->W__typ, ch);
	CmdlnTexts_Write(&*WriteDate__43_s->W, WriteDate__43_s->W__typ, (CHAR)(__DIV(x, 10) + 48));
	CmdlnTexts_Write(&*WriteDate__43_s->W, WriteDate__43_s->W__typ, (CHAR)(__MOD(x, 10) + 48));
}

void CmdlnTexts_WriteDate (CmdlnTexts_Writer *W, LONGINT *W__typ, LONGINT t, LONGINT d)
{
	struct WriteDate__43 _s;
	_s.W = W; _s.W__typ = W__typ;
	_s.lnk = WriteDate__43_s;
	WriteDate__43_s = &_s;
	WritePair__44(' ', __MASK(d, -32));
	WritePair__44('.', __MASK(__ASHR(d, 5), -16));
	WritePair__44('.', __MASK(__ASHR(d, 9), -128));
	WritePair__44(' ', __MASK(__ASHR(t, 12), -32));
	WritePair__44(':', __MASK(__ASHR(t, 6), -64));
	WritePair__44(':', __MASK(t, -64));
	WriteDate__43_s = _s.lnk;
}

/*----------------------------------------------------------------------------*/
static struct Load0__16 {
	CmdlnTexts_Text *T;
	SHORTINT *ecnt;
	Files_File *f;
	CmdlnTexts_FileMsg *msg;
	CHAR (*mods)[64][32], (*procs)[64][32];
	struct Load0__16 *lnk;
} *Load0__16_s;

static void LoadElem__17 (Files_Rider *r, LONGINT *r__typ, LONGINT pos, LONGINT span, CmdlnTexts_Elem *e);

static void LoadElem__17 (Files_Rider *r, LONGINT *r__typ, LONGINT pos, LONGINT span, CmdlnTexts_Elem *e)
{
	Modules_Module M = NIL;
	Modules_Command Cmd;
	CmdlnTexts_Alien a = NIL;
	LONGINT org, ew, eh;
	SHORTINT eno;
	CmdlnTexts_new = NIL;
	Files_ReadLInt(&*r, r__typ, &ew);
	Files_ReadLInt(&*r, r__typ, &eh);
	Files_Read(&*r, r__typ, (void*)&eno);
	if (eno > *Load0__16_s->ecnt) {
		*Load0__16_s->ecnt = eno;
		Files_ReadString(&*r, r__typ, (void*)(*Load0__16_s->mods)[__X(eno, 64)], 32);
		Files_ReadString(&*r, r__typ, (void*)(*Load0__16_s->procs)[__X(eno, 64)], 32);
	}
	org = Files_Pos(&*r, r__typ);
	M = Modules_ThisMod((*Load0__16_s->mods)[__X(eno, 64)], 32);
	if (M != NIL) {
		Cmd = Modules_ThisCommand(M, (*Load0__16_s->procs)[__X(eno, 64)], 32);
		if (Cmd != NIL) {
			(*Cmd)();
		}
	}
	*e = CmdlnTexts_new;
	if (*e != NIL) {
		(*e)->W = ew;
		(*e)->H = eh;
		(*e)->base = *Load0__16_s->T;
		(*Load0__16_s->msg).pos = pos;
		(*(*e)->handle)(*e, (void*)&*Load0__16_s->msg, CmdlnTexts_FileMsg__typ);
		if (Files_Pos(&*r, r__typ) != org + span) {
			*e = NIL;
		}
	}
	if (*e == NIL) {
		Files_Set(&*r, r__typ, *Load0__16_s->f, org + span);
		__NEW(a, CmdlnTexts__1);
		a->W = ew;
		a->H = eh;
		a->handle = CmdlnTexts_HandleAlien;
		a->base = *Load0__16_s->T;
		a->file = *Load0__16_s->f;
		a->org = org;
		a->span = span;
		__COPY((*Load0__16_s->mods)[__X(eno, 64)], a->mod, 32);
		__COPY((*Load0__16_s->procs)[__X(eno, 64)], a->proc, 32);
		*e = (CmdlnTexts_Elem)a;
	}
}

static void CmdlnTexts_Load0 (Files_Rider *r, LONGINT *r__typ, CmdlnTexts_Text T)
{
	CmdlnTexts_Run u = NIL, un = NIL;
	CmdlnTexts_Piece p = NIL;
	CmdlnTexts_Elem e = NIL;
	LONGINT org, pos, hlen, plen;
	SHORTINT ecnt, fno, fcnt, col, voff;
	Files_File f = NIL;
	CmdlnTexts_FileMsg msg;
	CHAR mods[64][32], procs[64][32];
	CHAR name[32];
	CmdlnTexts_FontsFont fnts[32];
	struct Load0__16 _s;
	_s.T = &T;
	_s.ecnt = &ecnt;
	_s.f = &f;
	_s.msg = &msg;
	_s.mods = (void*)mods;
	_s.procs = (void*)procs;
	_s.lnk = Load0__16_s;
	Load0__16_s = &_s;
	pos = Files_Pos(&*r, r__typ);
	f = Files_Base(&*r, r__typ);
	__NEW(u, CmdlnTexts_RunDesc);
	u->len = 2147483647;
	u->fnt = NIL;
	u->col = 15;
	T->head = u;
	ecnt = 0;
	fcnt = 0;
	msg.id = 0;
	msg.r = *r;
	Files_ReadLInt(&msg.r, Files_Rider__typ, &hlen);
	org = (pos - 2) + hlen;
	pos = org;
	Files_Read(&msg.r, Files_Rider__typ, (void*)&fno);
	while (fno != 0) {
		if (fno > fcnt) {
			fcnt = fno;
			Files_ReadString(&msg.r, Files_Rider__typ, (void*)name, 32);
			fnts[__X(fno, 32)] = CmdlnTexts_FontsThis((void*)name, 32);
		}
		Files_Read(&msg.r, Files_Rider__typ, (void*)&col);
		Files_Read(&msg.r, Files_Rider__typ, (void*)&voff);
		Files_ReadLInt(&msg.r, Files_Rider__typ, &plen);
		if (plen > 0) {
			__NEW(p, CmdlnTexts_PieceDesc);
			p->file = f;
			p->org = pos;
			p->ascii = 0;
			un = (CmdlnTexts_Run)p;
			un->len = plen;
		} else {
			LoadElem__17(&msg.r, Files_Rider__typ, pos - org, -plen, &e);
			un = (CmdlnTexts_Run)e;
			un->len = 1;
		}
		un->fnt = fnts[__X(fno, 32)];
		un->col = col;
		un->voff = voff;
		pos += un->len;
		u->next = un;
		un->prev = u;
		u = un;
		Files_Read(&msg.r, Files_Rider__typ, (void*)&fno);
	}
	u->next = T->head;
	T->head->prev = u;
	T->cache = T->head;
	T->corg = 0;
	Files_ReadLInt(&msg.r, Files_Rider__typ, &T->len);
	Files_Set(&*r, r__typ, f, Files_Pos(&msg.r, Files_Rider__typ) + T->len);
	Load0__16_s = _s.lnk;
}

void CmdlnTexts_Load (Files_Rider *r, LONGINT *r__typ, CmdlnTexts_Text T)
{
	INTEGER tag;
	Files_ReadInt(&*r, r__typ, &tag);
	if (tag != -4095) {
		Files_Set(&*r, r__typ, Files_Base(&*r, r__typ), Files_Pos(&*r, r__typ) - 2);
	}
	CmdlnTexts_Load0(&*r, r__typ, T);
}

/*----------------------------------------------------------------------------*/
void CmdlnTexts_Open (CmdlnTexts_Text T, CHAR *name, LONGINT name__len)
{
	Files_File f = NIL;
	Files_Rider r;
	CmdlnTexts_Run u = NIL;
	CmdlnTexts_Piece p = NIL;
	CHAR tag, version;
	LONGINT hlen;
	__DUP(name, name__len, CHAR);
	f = Files_Old(name, name__len);
	if (f == NIL) {
		f = Files_New((CHAR*)"", (LONGINT)1);
	}
	Files_Set(&r, Files_Rider__typ, f, 0);
	Files_Read(&r, Files_Rider__typ, (void*)&tag);
	Files_Read(&r, Files_Rider__typ, (void*)&version);
	if (tag == 0xf0 || tag == 0x01 && version == 0xf0) {
		CmdlnTexts_Load0(&r, Files_Rider__typ, T);
	} else {
		__NEW(u, CmdlnTexts_RunDesc);
		u->len = 2147483647;
		u->fnt = NIL;
		u->col = 15;
		__NEW(p, CmdlnTexts_PieceDesc);
		if (tag == 0xf7 && version == 0x07) {
			Files_Set(&r, Files_Rider__typ, f, 28);
			Files_ReadLInt(&r, Files_Rider__typ, &hlen);
			Files_Set(&r, Files_Rider__typ, f, 22 + hlen);
			Files_ReadLInt(&r, Files_Rider__typ, &T->len);
			p->org = 26 + hlen;
		} else {
			T->len = Files_Length(f);
			p->org = 0;
		}
		if (T->len > 0) {
			p->len = T->len;
			p->fnt = CmdlnTexts_FontsDefault;
			p->col = 15;
			p->voff = 0;
			p->file = f;
			p->ascii = 1;
			u->next = (CmdlnTexts_Run)p;
			u->prev = (CmdlnTexts_Run)p;
			p->next = u;
			p->prev = u;
		} else {
			u->next = u;
			u->prev = u;
		}
		T->head = u;
		T->cache = T->head;
		T->corg = 0;
	}
	__DEL(name);
}

/*----------------------------------------------------------------------------*/
static struct Store__39 {
	SHORTINT *ecnt;
	CmdlnTexts_FileMsg *msg;
	CmdlnTexts_IdentifyMsg *iden;
	CHAR (*mods)[64][32], (*procs)[64][32];
	struct Store__39 *lnk;
} *Store__39_s;

static void StoreElem__40 (Files_Rider *r, LONGINT *r__typ, LONGINT pos, CmdlnTexts_Elem e);

static void StoreElem__40 (Files_Rider *r, LONGINT *r__typ, LONGINT pos, CmdlnTexts_Elem e)
{
	Files_Rider r1;
	LONGINT org, span;
	SHORTINT eno;
	__COPY((*Store__39_s->iden).mod, (*Store__39_s->mods)[__X(*Store__39_s->ecnt, 64)], 32);
	__COPY((*Store__39_s->iden).proc, (*Store__39_s->procs)[__X(*Store__39_s->ecnt, 64)], 32);
	eno = 1;
	while (__STRCMP((*Store__39_s->mods)[__X(eno, 64)], (*Store__39_s->iden).mod) != 0 || __STRCMP((*Store__39_s->procs)[__X(eno, 64)], (*Store__39_s->iden).proc) != 0) {
		eno += 1;
	}
	Files_Set(&r1, Files_Rider__typ, Files_Base(&*r, r__typ), Files_Pos(&*r, r__typ));
	Files_WriteLInt(&*r, r__typ, 0);
	Files_WriteLInt(&*r, r__typ, 0);
	Files_WriteLInt(&*r, r__typ, 0);
	Files_Write(&*r, r__typ, eno);
	if (eno == *Store__39_s->ecnt) {
		*Store__39_s->ecnt += 1;
		Files_WriteString(&*r, r__typ, (*Store__39_s->iden).mod, 32);
		Files_WriteString(&*r, r__typ, (*Store__39_s->iden).proc, 32);
	}
	(*Store__39_s->msg).pos = pos;
	org = Files_Pos(&*r, r__typ);
	(*e->handle)(e, (void*)&*Store__39_s->msg, CmdlnTexts_FileMsg__typ);
	span = Files_Pos(&*r, r__typ) - org;
	Files_WriteLInt(&r1, Files_Rider__typ, -span);
	Files_WriteLInt(&r1, Files_Rider__typ, e->W);
	Files_WriteLInt(&r1, Files_Rider__typ, e->H);
}

void CmdlnTexts_Store (Files_Rider *r, LONGINT *r__typ, CmdlnTexts_Text T)
{
	Files_Rider r1;
	CmdlnTexts_Run u = NIL, un = NIL;
	CmdlnTexts_Elem e = NIL;
	LONGINT org, pos, delta, hlen, rlen;
	SHORTINT ecnt, fno, fcnt;
	CHAR ch;
	CmdlnTexts_FileMsg msg;
	CmdlnTexts_IdentifyMsg iden;
	CHAR mods[64][32], procs[64][32];
	CmdlnTexts_FontsFont fnts[32];
	CHAR block[1024];
	struct Store__39 _s;
	_s.ecnt = &ecnt;
	_s.msg = &msg;
	_s.iden = &iden;
	_s.mods = (void*)mods;
	_s.procs = (void*)procs;
	_s.lnk = Store__39_s;
	Store__39_s = &_s;
	org = Files_Pos(&*r, r__typ);
	msg.id = 1;
	msg.r = *r;
	Files_WriteLInt(&msg.r, Files_Rider__typ, 0);
	u = T->head->next;
	pos = 0;
	delta = 0;
	fcnt = 1;
	ecnt = 1;
	while (u != T->head) {
		if (__ISP(u, CmdlnTexts_ElemDesc, 1)) {
			iden.mod[0] = 0x00;
			(*__GUARDP(u, CmdlnTexts_ElemDesc, 1)->handle)(__GUARDP(u, CmdlnTexts_ElemDesc, 1), (void*)&iden, CmdlnTexts_IdentifyMsg__typ);
		} else {
			iden.mod[0] = 0x01;
		}
		if (iden.mod[0] != 0x00) {
			fnts[__X(fcnt, 32)] = u->fnt;
			fno = 1;
			while (__STRCMP(fnts[__X(fno, 32)]->name, u->fnt->name) != 0) {
				fno += 1;
			}
			Files_Write(&msg.r, Files_Rider__typ, fno);
			if (fno == fcnt) {
				fcnt += 1;
				Files_WriteString(&msg.r, Files_Rider__typ, u->fnt->name, 32);
			}
			Files_Write(&msg.r, Files_Rider__typ, u->col);
			Files_Write(&msg.r, Files_Rider__typ, u->voff);
		}
		if (__ISP(u, CmdlnTexts_PieceDesc, 1)) {
			rlen = u->len;
			un = u->next;
			while (((__ISP(un, CmdlnTexts_PieceDesc, 1) && un->fnt == u->fnt) && un->col == u->col) && un->voff == u->voff) {
				rlen += un->len;
				un = un->next;
			}
			Files_WriteLInt(&msg.r, Files_Rider__typ, rlen);
			pos += rlen;
			u = un;
		} else if (iden.mod[0] != 0x00) {
			StoreElem__40(&msg.r, Files_Rider__typ, pos, __GUARDP(u, CmdlnTexts_ElemDesc, 1));
			pos += 1;
			u = u->next;
		} else {
			delta += 1;
			u = u->next;
		}
	}
	Files_Write(&msg.r, Files_Rider__typ, 0);
	Files_WriteLInt(&msg.r, Files_Rider__typ, T->len - delta);
	hlen = (Files_Pos(&msg.r, Files_Rider__typ) - org) + 2;
	Files_Set(&r1, Files_Rider__typ, Files_Base(&msg.r, Files_Rider__typ), org);
	Files_WriteLInt(&r1, Files_Rider__typ, hlen);
	u = T->head->next;
	while (u != T->head) {
		if (__ISP(u, CmdlnTexts_PieceDesc, 1)) {
			if (__ISP(u, CmdlnTexts_PieceDesc, 1)) {
				if (((CmdlnTexts_Piece)u)->ascii) {
					Files_Set(&r1, Files_Rider__typ, ((CmdlnTexts_Piece)u)->file, ((CmdlnTexts_Piece)u)->org);
					delta = ((CmdlnTexts_Piece)u)->len;
					while (delta > 0) {
						Files_Read(&r1, Files_Rider__typ, (void*)&ch);
						delta -= 1;
						if (ch == 0x0a) {
							Files_Write(&msg.r, Files_Rider__typ, 0x0d);
						} else {
							Files_Write(&msg.r, Files_Rider__typ, ch);
						}
					}
				} else {
					Files_Set(&r1, Files_Rider__typ, ((CmdlnTexts_Piece)u)->file, ((CmdlnTexts_Piece)u)->org);
					delta = ((CmdlnTexts_Piece)u)->len;
					while (delta > 1024) {
						Files_ReadBytes(&r1, Files_Rider__typ, (void*)block, 1024, 1024);
						Files_WriteBytes(&msg.r, Files_Rider__typ, (void*)block, 1024, 1024);
						delta -= 1024;
					}
					Files_ReadBytes(&r1, Files_Rider__typ, (void*)block, 1024, delta);
					Files_WriteBytes(&msg.r, Files_Rider__typ, (void*)block, 1024, delta);
				}
			} else __WITHCHK;
		} else {
			iden.mod[0] = 0x00;
			(*__GUARDP(u, CmdlnTexts_ElemDesc, 1)->handle)(__GUARDP(u, CmdlnTexts_ElemDesc, 1), (void*)&iden, CmdlnTexts_IdentifyMsg__typ);
			if (iden.mod[0] != 0x00) {
				Files_Write(&msg.r, Files_Rider__typ, 0x1c);
			}
		}
		u = u->next;
	}
	__GUARDEQR(r, r__typ, Files_Rider) = msg.r;
	Store__39_s = _s.lnk;
}

/*----------------------------------------------------------------------------*/
void CmdlnTexts_Close (CmdlnTexts_Text T, CHAR *name, LONGINT name__len)
{
	Files_File f = NIL;
	Files_Rider r;
	INTEGER i, res;
	CHAR bak[64];
	__DUP(name, name__len, CHAR);
	f = Files_New(name, name__len);
	Files_Set(&r, Files_Rider__typ, f, 0);
	Files_Write(&r, Files_Rider__typ, 0xf0);
	Files_Write(&r, Files_Rider__typ, 0x01);
	CmdlnTexts_Store(&r, Files_Rider__typ, T);
	i = 0;
	while (name[__X(i, name__len)] != 0x00) {
		i += 1;
	}
	__COPY(name, bak, 64);
	bak[__X(i, 64)] = '.';
	bak[__X(i + 1, 64)] = 'B';
	bak[__X(i + 2, 64)] = 'a';
	bak[__X(i + 3, 64)] = 'k';
	bak[__X(i + 4, 64)] = 0x00;
	Files_Rename(name, name__len, bak, 64, &res);
	Files_Register(f);
	__DEL(name);
}

/*----------------------------------------------------------------------------*/
static void EnumPtrs(void (*P)(void*))
{
	P(CmdlnTexts_new);
	P(CmdlnTexts_del);
	P(CmdlnTexts_FontsDefault);
}

__TDESC(CmdlnTexts_FontDesc__desc, 1, 0) = {__TDFLDS("FontDesc", 32), {-4}};
__TDESC(CmdlnTexts_RunDesc__desc, 1, 3) = {__TDFLDS("RunDesc", 20), {0, 4, 12, -16}};
__TDESC(CmdlnTexts_PieceDesc__desc, 1, 4) = {__TDFLDS("PieceDesc", 28), {0, 4, 12, 20, -20}};
__TDESC(CmdlnTexts_ElemMsg__desc, 1, 0) = {__TDFLDS("ElemMsg", 1), {-4}};
__TDESC(CmdlnTexts_ElemDesc__desc, 1, 4) = {__TDFLDS("ElemDesc", 36), {0, 4, 12, 32, -20}};
__TDESC(CmdlnTexts_FileMsg__desc, 1, 1) = {__TDFLDS("FileMsg", 28), {16, -8}};
__TDESC(CmdlnTexts_CopyMsg__desc, 1, 1) = {__TDFLDS("CopyMsg", 4), {0, -8}};
__TDESC(CmdlnTexts_IdentifyMsg__desc, 1, 0) = {__TDFLDS("IdentifyMsg", 64), {-4}};
__TDESC(CmdlnTexts_BufDesc__desc, 1, 1) = {__TDFLDS("BufDesc", 8), {4, -8}};
__TDESC(CmdlnTexts_TextDesc__desc, 1, 2) = {__TDFLDS("TextDesc", 16), {4, 8, -12}};
__TDESC(CmdlnTexts_Reader__desc, 1, 4) = {__TDFLDS("Reader", 48), {4, 12, 24, 36, -20}};
__TDESC(CmdlnTexts_Scanner__desc, 1, 4) = {__TDFLDS("Scanner", 144), {4, 12, 24, 36, -20}};
__TDESC(CmdlnTexts_Writer__desc, 1, 4) = {__TDFLDS("Writer", 36), {0, 4, 20, 32, -20}};
__TDESC(CmdlnTexts__1__desc, 1, 5) = {__TDFLDS("", 112), {0, 4, 12, 32, 36, -24}};

export void *CmdlnTexts__init(void)
{
	__DEFMOD;
	__IMPORT(Files__init);
	__IMPORT(Modules__init);
	__IMPORT(Reals__init);
	__REGMOD("CmdlnTexts", EnumPtrs);
	__INITYP(CmdlnTexts_FontDesc, CmdlnTexts_FontDesc, 0);
	__INITYP(CmdlnTexts_RunDesc, CmdlnTexts_RunDesc, 0);
	__INITYP(CmdlnTexts_PieceDesc, CmdlnTexts_RunDesc, 1);
	__INITYP(CmdlnTexts_ElemMsg, CmdlnTexts_ElemMsg, 0);
	__INITYP(CmdlnTexts_ElemDesc, CmdlnTexts_RunDesc, 1);
	__INITYP(CmdlnTexts_FileMsg, CmdlnTexts_ElemMsg, 1);
	__INITYP(CmdlnTexts_CopyMsg, CmdlnTexts_ElemMsg, 1);
	__INITYP(CmdlnTexts_IdentifyMsg, CmdlnTexts_ElemMsg, 1);
	__INITYP(CmdlnTexts_BufDesc, CmdlnTexts_BufDesc, 0);
	__INITYP(CmdlnTexts_TextDesc, CmdlnTexts_TextDesc, 0);
	__INITYP(CmdlnTexts_Reader, CmdlnTexts_Reader, 0);
	__INITYP(CmdlnTexts_Scanner, CmdlnTexts_Reader, 1);
	__INITYP(CmdlnTexts_Writer, CmdlnTexts_Writer, 0);
	__INITYP(CmdlnTexts__1, CmdlnTexts_ElemDesc, 2);
/* BEGIN */
	CmdlnTexts_del = NIL;
	__NEW(CmdlnTexts_FontsDefault, CmdlnTexts_FontDesc);
	__MOVE("Syntax10.Scn.Fnt", CmdlnTexts_FontsDefault->name, 17);
	__ENDMOD;
}
