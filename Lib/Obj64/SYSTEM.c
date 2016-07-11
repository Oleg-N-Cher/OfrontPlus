/*  Ofront 1.2 -spke */
#include "SYSTEM.h"

struct SYSTEM__1 {
	CHAR ch;
	SYSTEM_PTR p;
};

typedef
	struct SYSTEM_CmdDesc *SYSTEM_Cmd;

typedef
	CHAR SYSTEM_CmdName[24];

typedef
	void (*SYSTEM_Command)(void);

typedef
	struct SYSTEM_CmdDesc {
		SYSTEM_Cmd next;
		SYSTEM_CmdName name;
		SYSTEM_Command cmd;
	} SYSTEM_CmdDesc;

typedef
	void (*SYSTEM_EnumProc)(void(*)(SYSTEM_PTR));

typedef
	struct SYSTEM_FinDesc *SYSTEM_FinNode;

typedef
	void (*SYSTEM_Finalizer)(SYSTEM_PTR);

typedef
	struct SYSTEM_FinDesc {
		SYSTEM_FinNode next;
		LONGINT obj;
		BOOLEAN marked;
		SYSTEM_Finalizer finalize;
	} SYSTEM_FinDesc;

typedef
	struct SYSTEM_ModuleDesc *SYSTEM_Module;

typedef
	CHAR SYSTEM_ModuleName[20];

typedef
	struct SYSTEM_ModuleDesc {
		SYSTEM_Module next;
		SYSTEM_ModuleName name;
		LONGINT refcnt;
		SYSTEM_Cmd cmds;
		LONGINT types;
		SYSTEM_EnumProc enumPtrs;
		LONGINT reserved1, reserved2;
	} SYSTEM_ModuleDesc;


export SYSTEM_PTR SYSTEM_modules;
static LONGINT SYSTEM_freeList[10];
static LONGINT SYSTEM_bigBlocks;
export LONGINT SYSTEM_allocated;
static BOOLEAN SYSTEM_firstTry;
static LONGINT SYSTEM_heap, SYSTEM_heapend;
export LONGINT SYSTEM_heapsize;
static SYSTEM_FinNode SYSTEM_fin;
export SHORTINT SYSTEM_gclock;

export LONGINT *SYSTEM_ModuleDesc__typ;
export LONGINT *SYSTEM_CmdDesc__typ;
export LONGINT *SYSTEM_FinDesc__typ;
export LONGINT *SYSTEM__1__typ;

static void SYSTEM_CheckFin (void);
static void SYSTEM_ExtendHeap (LONGINT blksz);
export void SYSTEM_FINALL (void);
static void SYSTEM_Finalize (void);
export void SYSTEM_GC (BOOLEAN markStack);
static void SYSTEM_HeapSort (LONGINT n, LONGINT *a, LONGINT a__len);
export void SYSTEM_INCREF (SYSTEM_Module m);
static void SYSTEM_InitHeap (void);
static void SYSTEM_Mark (LONGINT q);
static void SYSTEM_MarkCandidates (LONGINT n, LONGINT *cand, LONGINT cand__len);
static void SYSTEM_MarkP (SYSTEM_PTR p);
static void SYSTEM_MarkStack (LONGINT n, LONGINT *cand, LONGINT cand__len);
export SYSTEM_PTR SYSTEM_NEWBLK (LONGINT size);
export SYSTEM_PTR SYSTEM_NEWREC (LONGINT tag);
static LONGINT SYSTEM_NewChunk (LONGINT blksz);
export void SYSTEM_REGCMD (SYSTEM_Module m, SYSTEM_CmdName name, SYSTEM_Command cmd);
export void SYSTEM_REGFIN (SYSTEM_PTR obj, SYSTEM_Finalizer finalize);
export SYSTEM_PTR SYSTEM_REGMOD (SYSTEM_ModuleName name, SYSTEM_EnumProc enumPtrs);
export void SYSTEM_REGTYP (SYSTEM_Module m, LONGINT typ);
static void SYSTEM_Scan (void);
static void SYSTEM_Sift (LONGINT l, LONGINT r, LONGINT *a, LONGINT a__len);

#define SYSTEM_Lock()	Lock
#define SYSTEM_Mainfrm()	SYSTEM_mainfrm
#define SYSTEM_Unlock()	Unlock
#define SYSTEM_malloc(size)	(LONGINT)malloc(size)

/*============================================================================*/

SYSTEM_PTR SYSTEM_REGMOD (SYSTEM_ModuleName name, SYSTEM_EnumProc enumPtrs)
{
	SYSTEM_Module m = NIL;
	if (__STRCMP(name, "SYSTEM") == 0) {
		__SYSNEW(m, 80);
		m->cmds = NIL;
	} else {
		__NEW(m, SYSTEM_ModuleDesc);
	}
	__COPY(name, m->name, 20);
	m->refcnt = 0;
	m->enumPtrs = enumPtrs;
	m->next = (SYSTEM_Module)SYSTEM_modules;
	SYSTEM_modules = (SYSTEM_PTR)m;
	return (void*)m;
}

/*----------------------------------------------------------------------------*/
void SYSTEM_REGCMD (SYSTEM_Module m, SYSTEM_CmdName name, SYSTEM_Command cmd)
{
	SYSTEM_Cmd c = NIL;
	__NEW(c, SYSTEM_CmdDesc);
	__COPY(name, c->name, 24);
	c->cmd = cmd;
	c->next = m->cmds;
	m->cmds = c;
}

/*----------------------------------------------------------------------------*/
void SYSTEM_REGTYP (SYSTEM_Module m, LONGINT typ)
{
	__PUT(typ, m->types, LONGINT);
	m->types = typ;
}

/*----------------------------------------------------------------------------*/
void SYSTEM_INCREF (SYSTEM_Module m)
{
	m->refcnt += 1;
}

/*----------------------------------------------------------------------------*/
static LONGINT SYSTEM_NewChunk (LONGINT blksz)
{
	LONGINT chnk;
	chnk = SYSTEM_malloc(blksz + 24);
	if (chnk != 0) {
		__PUT(chnk + 8, chnk + (24 + blksz), LONGINT);
		__PUT(chnk + 24, chnk + 32, LONGINT);
		__PUT(chnk + 32, blksz, LONGINT);
		__PUT(chnk + 40, -8, LONGINT);
		__PUT(chnk + 48, SYSTEM_bigBlocks, LONGINT);
		SYSTEM_bigBlocks = chnk + 24;
		SYSTEM_heapsize += blksz;
	}
	return chnk;
}

static void SYSTEM_ExtendHeap (LONGINT blksz)
{
	LONGINT size, chnk, j, next;
	if (blksz > 320000) {
		size = blksz;
	} else {
		size = 320000;
	}
	chnk = SYSTEM_NewChunk(size);
	if (chnk != 0) {
		if (chnk < SYSTEM_heap) {
			__PUT(chnk, SYSTEM_heap, LONGINT);
			SYSTEM_heap = chnk;
		} else {
			j = SYSTEM_heap;
			__GET(j, next, LONGINT);
			while (next != 0 && chnk > next) {
				j = next;
				__GET(j, next, LONGINT);
			}
			__PUT(chnk, next, LONGINT);
			__PUT(j, chnk, LONGINT);
		}
		if (next == 0) {
			__GET(chnk + 8, SYSTEM_heapend, LONGINT);
		}
	}
}

SYSTEM_PTR SYSTEM_NEWREC (LONGINT tag)
{
	LONGINT i, i0, di, blksz, restsize, t, adr, end, next, prev;
	SYSTEM_PTR new = NIL;
	SYSTEM_Lock();
	__GET(tag, blksz, LONGINT);
		i0 = __ASHR(blksz, 5);
	i = i0;
	if (i < 9) {
		adr = SYSTEM_freeList[i];
		while (adr == 0) {
			i += 1;
			adr = SYSTEM_freeList[i];
		}
	}
	if (i < 9) {
		__GET(adr + 24, next, LONGINT);
		SYSTEM_freeList[i] = next;
		if (i != i0) {
			di = i - i0;
			restsize = __ASHL(di, 5);
			end = adr + restsize;
			__PUT(end + 8, blksz, LONGINT);
			__PUT(end + 16, -8, LONGINT);
			__PUT(end, end + 8, LONGINT);
			__PUT(adr + 8, restsize, LONGINT);
			__PUT(adr + 24, SYSTEM_freeList[di], LONGINT);
			SYSTEM_freeList[di] = adr;
			adr += restsize;
		}
	} else {
		adr = SYSTEM_bigBlocks;
		prev = 0;
		for (;;) {
			if (adr == 0) {
				if (SYSTEM_firstTry) {
					SYSTEM_GC(1);
					blksz += 32;
					if (__ASHL((SYSTEM_heapsize - SYSTEM_allocated) - blksz, 2) < SYSTEM_heapsize) {
						SYSTEM_ExtendHeap(__ASHL(__DIV(SYSTEM_allocated + blksz, 96), 7) - SYSTEM_heapsize);
					}
					SYSTEM_firstTry = 0;
					new = SYSTEM_NEWREC(tag);
					SYSTEM_firstTry = 1;
					if (new == NIL) {
						SYSTEM_ExtendHeap(__ASHL(__DIV(SYSTEM_allocated + blksz, 96), 7) - SYSTEM_heapsize);
						new = SYSTEM_NEWREC(tag);
					}
					SYSTEM_Unlock();
					return new;
				} else {
					SYSTEM_Unlock();
					return NIL;
				}
			}
			__GET(adr + 8, t, LONGINT);
			if (t >= blksz) {
				break;
			}
			prev = adr;
			__GET(adr + 24, adr, LONGINT);
		}
		restsize = t - blksz;
		end = adr + restsize;
		__PUT(end + 8, blksz, LONGINT);
		__PUT(end + 16, -8, LONGINT);
		__PUT(end, end + 8, LONGINT);
		if (restsize > 288) {
			__PUT(adr + 8, restsize, LONGINT);
		} else {
			__GET(adr + 24, next, LONGINT);
			if (prev == 0) {
				SYSTEM_bigBlocks = next;
			} else {
				__PUT(prev + 24, next, LONGINT);
			}
			if (restsize > 0) {
				di = __ASHR(restsize, 5);
				__PUT(adr + 8, restsize, LONGINT);
				__PUT(adr + 24, SYSTEM_freeList[di], LONGINT);
				SYSTEM_freeList[di] = adr;
			}
		}
		adr += restsize;
	}
	i = adr + 32;
	end = adr + blksz;
	while (i < end) {
		__PUT(i, 0, LONGINT);
		__PUT(i + 8, 0, LONGINT);
		__PUT(i + 16, 0, LONGINT);
		__PUT(i + 24, 0, LONGINT);
		i += 32;
	}
	__PUT(adr + 24, 0, LONGINT);
	__PUT(adr, tag, LONGINT);
	__PUT(adr + 8, 0, LONGINT);
	__PUT(adr + 16, 0, LONGINT);
	SYSTEM_allocated += blksz;
	SYSTEM_Unlock();
	return (SYSTEM_PTR)(adr + 8);
}

/*----------------------------------------------------------------------------*/
SYSTEM_PTR SYSTEM_NEWBLK (LONGINT size)
{
	LONGINT blksz, tag;
	SYSTEM_PTR new = NIL;
	SYSTEM_Lock();
	blksz = __ASHL(__ASHR(size + 63, 5), 5);
	new = SYSTEM_NEWREC((LONGINT)&blksz);
	tag = ((LONGINT)new + blksz) - 24;
	__PUT(tag - 8, 0, LONGINT);
	__PUT(tag, blksz, LONGINT);
	__PUT(tag + 8, -8, LONGINT);
	__PUT((LONGINT)new - 8, tag, LONGINT);
	SYSTEM_Unlock();
	return new;
}

/*----------------------------------------------------------------------------*/
static void SYSTEM_Mark (LONGINT q)
{
	LONGINT p, tag, fld, n, offset, tagbits;
	if (q != 0) {
		__GET(q - 8, tagbits, LONGINT);
		if (!__ODD(tagbits)) {
			__PUT(q - 8, tagbits + 1, LONGINT);
			p = 0;
			tag = tagbits + 8;
			for (;;) {
				__GET(tag, offset, LONGINT);
				if (offset < 0) {
					__PUT(q - 8, (tag + offset) + 1, LONGINT);
					if (p == 0) {
						break;
					}
					n = q;
					q = p;
					__GET(q - 8, tag, LONGINT);
					tag -= 1;
					__GET(tag, offset, LONGINT);
					fld = q + offset;
					__GET(fld, p, LONGINT);
					__PUT(fld, n, LONGINT);
				} else {
					fld = q + offset;
					__GET(fld, n, LONGINT);
					if (n != 0) {
						__GET(n - 8, tagbits, LONGINT);
						if (!__ODD(tagbits)) {
							__PUT(n - 8, tagbits + 1, LONGINT);
							__PUT(q - 8, tag + 1, LONGINT);
							__PUT(fld, p, LONGINT);
							p = q;
							q = n;
							tag = tagbits;
						}
					}
				}
				tag += 8;
			}
		}
	}
}

static void SYSTEM_MarkP (SYSTEM_PTR p)
{
	SYSTEM_Mark((LONGINT)p);
}

static void SYSTEM_Scan (void)
{
	LONGINT chnk, adr, end, start, tag, i, size, freesize;
	SYSTEM_bigBlocks = 0;
	i = 1;
	while (i < 9) {
		SYSTEM_freeList[i] = 0;
		i += 1;
	}
	freesize = 0;
	SYSTEM_allocated = 0;
	chnk = SYSTEM_heap;
	while (chnk != 0) {
		adr = chnk + 24;
		__GET(chnk + 8, end, LONGINT);
		while (adr < end) {
			__GET(adr, tag, LONGINT);
			if (__ODD(tag)) {
				if (freesize > 0) {
					start = adr - freesize;
					__PUT(start, start + 8, LONGINT);
					__PUT(start + 8, freesize, LONGINT);
					__PUT(start + 16, -8, LONGINT);
					i = __ASHR(freesize, 5);
					freesize = 0;
					if (i < 9) {
						__PUT(start + 24, SYSTEM_freeList[i], LONGINT);
						SYSTEM_freeList[i] = start;
					} else {
						__PUT(start + 24, SYSTEM_bigBlocks, LONGINT);
						SYSTEM_bigBlocks = start;
					}
				}
				tag -= 1;
				__PUT(adr, tag, LONGINT);
				__GET(tag, size, LONGINT);
				SYSTEM_allocated += size;
				adr += size;
			} else {
				__GET(tag, size, LONGINT);
				freesize += size;
				adr += size;
			}
		}
		if (freesize > 0) {
			start = adr - freesize;
			__PUT(start, start + 8, LONGINT);
			__PUT(start + 8, freesize, LONGINT);
			__PUT(start + 16, -8, LONGINT);
			i = __ASHR(freesize, 5);
			freesize = 0;
			if (i < 9) {
				__PUT(start + 24, SYSTEM_freeList[i], LONGINT);
				SYSTEM_freeList[i] = start;
			} else {
				__PUT(start + 24, SYSTEM_bigBlocks, LONGINT);
				SYSTEM_bigBlocks = start;
			}
		}
		__GET(chnk, chnk, LONGINT);
	}
}

static void SYSTEM_Sift (LONGINT l, LONGINT r, LONGINT *a, LONGINT a__len)
{
	LONGINT i, j, x;
	j = l;
	x = a[j];
	for (;;) {
		i = j;
		j = __ASHL(j, 1) + 1;
		if (j < r && a[j] < a[j + 1]) {
			j += 1;
		}
		if (j > r || a[j] <= x) {
			break;
		}
		a[i] = a[j];
	}
	a[i] = x;
}

static void SYSTEM_HeapSort (LONGINT n, LONGINT *a, LONGINT a__len)
{
	LONGINT l, r, x;
	l = __ASHR(n, 1);
	r = n - 1;
	while (l > 0) {
		l -= 1;
		SYSTEM_Sift(l, r, (void*)a, a__len);
	}
	while (r > 0) {
		x = a[0];
		a[0] = a[r];
		a[r] = x;
		r -= 1;
		SYSTEM_Sift(l, r, (void*)a, a__len);
	}
}

static void SYSTEM_MarkCandidates (LONGINT n, LONGINT *cand, LONGINT cand__len)
{
	LONGINT chnk, adr, tag, next, lim, lim1, i, ptr, size;
	chnk = SYSTEM_heap;
	i = 0;
	lim = cand[n - 1];
	while (chnk != 0 && chnk < lim) {
		adr = chnk + 24;
		__GET(chnk + 8, lim1, LONGINT);
		if (lim < lim1) {
			lim1 = lim;
		}
		while (adr < lim1) {
			__GET(adr, tag, LONGINT);
			if (__ODD(tag)) {
				__GET(tag - 1, size, LONGINT);
				adr += size;
			} else {
				__GET(tag, size, LONGINT);
				ptr = adr + 8;
				while (cand[i] < ptr) {
					i += 1;
				}
				if (i == n) {
					return;
				}
				next = adr + size;
				if (cand[i] < next) {
					SYSTEM_Mark(ptr);
				}
				adr = next;
			}
		}
		__GET(chnk, chnk, LONGINT);
	}
}

static void SYSTEM_CheckFin (void)
{
	SYSTEM_FinNode n = NIL;
	LONGINT tag;
	n = SYSTEM_fin;
	while (n != NIL) {
		__GET(n->obj - 8, tag, LONGINT);
		if (!__ODD(tag)) {
			n->marked = 0;
			SYSTEM_Mark(n->obj);
		} else {
			n->marked = 1;
		}
		n = n->next;
	}
}

static void SYSTEM_Finalize (void)
{
	SYSTEM_FinNode n = NIL, prev = NIL;
	n = SYSTEM_fin;
	prev = NIL;
	while (n != NIL) {
		if (!n->marked) {
			if (n == SYSTEM_fin) {
				SYSTEM_fin = SYSTEM_fin->next;
			} else {
				prev->next = n->next;
			}
			(*n->finalize)((SYSTEM_PTR)n->obj);
			if (prev == NIL) {
				n = SYSTEM_fin;
			} else {
				n = n->next;
			}
		} else {
			prev = n;
			n = n->next;
		}
	}
}

void SYSTEM_FINALL (void)
{
	SYSTEM_FinNode n = NIL;
	while (SYSTEM_fin != NIL) {
		n = SYSTEM_fin;
		SYSTEM_fin = SYSTEM_fin->next;
		(*n->finalize)((SYSTEM_PTR)n->obj);
	}
}

/*----------------------------------------------------------------------------*/
static void SYSTEM_MarkStack (LONGINT n, LONGINT *cand, LONGINT cand__len)
{
	SYSTEM_PTR frame = NIL;
	LONGINT inc, nofcand, sp, p, stack0, ptr;
	struct SYSTEM__1 align;
	if (n > 0) {
		SYSTEM_MarkStack(n - 1, cand, cand__len);
		if (n > 100) {
			return;
		}
	}
	if (n == 0) {
		nofcand = 0;
		sp = (LONGINT)&frame;
		stack0 = (LONGINT)SYSTEM_Mainfrm();
		inc = (LONGINT)&align.p - (LONGINT)&align;
		if (sp > stack0) {
			inc = -inc;
		}
		while (sp != stack0) {
			__GET(sp, p, LONGINT);
			if (p > SYSTEM_heap && p < SYSTEM_heapend) {
				if (nofcand == cand__len) {
					SYSTEM_HeapSort(nofcand, (void*)cand, cand__len);
					SYSTEM_MarkCandidates(nofcand, (void*)cand, cand__len);
					nofcand = 0;
				}
				cand[nofcand] = p;
				nofcand += 1;
			}
			sp += inc;
		}
		if (nofcand > 0) {
			SYSTEM_HeapSort(nofcand, (void*)cand, cand__len);
			SYSTEM_MarkCandidates(nofcand, (void*)cand, cand__len);
		}
	}
}

void SYSTEM_GC (BOOLEAN markStack)
{
	SYSTEM_Module m = NIL;
	LONGINT i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15, i16, i17, i18, i19, i20, i21, i22, i23;
	LONGINT cand[10000];
	if (SYSTEM_gclock == 0 || SYSTEM_gclock == 1 && !markStack) {
		SYSTEM_Lock();
		m = (SYSTEM_Module)SYSTEM_modules;
		while (m != NIL) {
			if (m->enumPtrs != NIL) {
				(*m->enumPtrs)(SYSTEM_MarkP);
			}
			m = m->next;
		}
		if (markStack) {
			i0 = -100;
			i1 = -101;
			i2 = -102;
			i3 = -103;
			i4 = -104;
			i5 = -105;
			i6 = -106;
			i7 = -107;
			i8 = 1;
			i9 = 2;
			i10 = 3;
			i11 = 4;
			i12 = 5;
			i13 = 6;
			i14 = 7;
			i15 = 8;
			i16 = 9;
			i17 = 10;
			i18 = 11;
			i19 = 12;
			i20 = 13;
			i21 = 14;
			i22 = 15;
			i23 = 16;
			for (;;) {
				i0 += 1;
				i1 += 2;
				i2 += 3;
				i3 += 4;
				i4 += 5;
				i5 += 6;
				i6 += 7;
				i7 += 8;
				i8 += 9;
				i9 += 10;
				i10 += 11;
				i11 += 12;
				i12 += 13;
				i13 += 14;
				i14 += 15;
				i15 += 16;
				i16 += 17;
				i17 += 18;
				i18 += 19;
				i19 += 20;
				i20 += 21;
				i21 += 22;
				i22 += 23;
				i23 += 24;
				if (i0 == -99 && i15 == 24) {
					SYSTEM_MarkStack(32, (void*)cand, 10000);
					break;
				}
			}
			if (((((((((((((((((((((((i0 + i1) + i2) + i3) + i4) + i5) + i6) + i7) + i8) + i9) + i10) + i11) + i12) + i13) + i14) + i15) + i16) + i17) + i18) + i19) + i20) + i21) + i22) + i23 > 10000) {
				return;
			}
		}
		SYSTEM_CheckFin();
		SYSTEM_Scan();
		SYSTEM_Finalize();
		SYSTEM_Unlock();
	}
}

/*----------------------------------------------------------------------------*/
void SYSTEM_REGFIN (SYSTEM_PTR obj, SYSTEM_Finalizer finalize)
{
	SYSTEM_FinNode f = NIL;
	__NEW(f, SYSTEM_FinDesc);
	f->obj = (LONGINT)obj;
	f->finalize = finalize;
	f->marked = 1;
	f->next = SYSTEM_fin;
	SYSTEM_fin = f;
}

/*----------------------------------------------------------------------------*/
static void SYSTEM_InitHeap (void)
{
	SYSTEM_heap = SYSTEM_NewChunk(256000);
	__GET(SYSTEM_heap + 8, SYSTEM_heapend, LONGINT);
	__PUT(SYSTEM_heap, 0, LONGINT);
	SYSTEM_allocated = 0;
	SYSTEM_firstTry = 1;
	SYSTEM_freeList[9] = 1;
	SYSTEM_gclock = 0;
}

static void EnumPtrs(void (*P)(void*))
{
	P(SYSTEM_modules);
	P(SYSTEM_fin);
}

__TDESC(SYSTEM_ModuleDesc__desc, 1, 2) = {__TDFLDS("ModuleDesc", 80), {0, 40, -24}};
__TDESC(SYSTEM_CmdDesc__desc, 1, 1) = {__TDFLDS("CmdDesc", 40), {0, -16}};
__TDESC(SYSTEM_FinDesc__desc, 1, 1) = {__TDFLDS("FinDesc", 32), {0, -16}};
__TDESC(SYSTEM__1__desc, 1, 1) = {__TDFLDS("", 16), {8, -16}};

export void *SYSTEM__init(void)
{
	__DEFMOD;
	__REGMOD("SYSTEM", EnumPtrs);
	__INITYP(SYSTEM_ModuleDesc, SYSTEM_ModuleDesc, 0);
	__INITYP(SYSTEM_CmdDesc, SYSTEM_CmdDesc, 0);
	__INITYP(SYSTEM_FinDesc, SYSTEM_FinDesc, 0);
	__INITYP(SYSTEM__1, SYSTEM__1, 0);
/* BEGIN */
	__ENDMOD;
}
