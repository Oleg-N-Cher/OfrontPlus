/*  Ofront 1.2 -spke */
#include "SYSTEM.h"

struct Heap__1 {
	CHAR ch;
	SYSTEM_PTR p;
};

typedef
	CHAR (*Heap_ADR)[1];

typedef
	struct Heap_CmdDesc *Heap_Cmd;

typedef
	CHAR Heap_CmdName[24];

typedef
	void (*Heap_Command)(void);

typedef
	struct Heap_CmdDesc {
		Heap_Cmd next;
		Heap_CmdName name;
		Heap_Command cmd;
	} Heap_CmdDesc;

typedef
	void (*Heap_EnumProc)(void(*)(SYSTEM_PTR));

typedef
	struct Heap_FinDesc *Heap_FinNode;

typedef
	void (*Heap_Finalizer)(SYSTEM_PTR);

typedef
	struct Heap_FinDesc {
		Heap_FinNode next;
		Heap_ADR obj;
		BOOLEAN marked;
		Heap_Finalizer finalize;
	} Heap_FinDesc;

typedef
	struct Heap_ModuleDesc *Heap_Module;

typedef
	CHAR Heap_ModuleName[20];

typedef
	struct Heap_ModuleDesc {
		Heap_Module next;
		Heap_ModuleName name;
		LONGINT refcnt;
		Heap_Cmd cmds;
		Heap_ADR types;
		Heap_EnumProc enumPtrs;
		LONGINT reserved1, reserved2;
	} Heap_ModuleDesc;


export SYSTEM_PTR Heap_modules;
static Heap_ADR Heap_freeList[10];
static Heap_ADR Heap_bigBlocks;
export LONGINT Heap_allocated;
static BOOLEAN Heap_firstTry;
static Heap_ADR Heap_heap, Heap_heapend;
export LONGINT Heap_heapsize;
static Heap_FinNode Heap_fin;
static INTEGER Heap_lockdepth;
static BOOLEAN Heap_interrupted;
export INTEGER Heap_FileCount;

export LONGINT *Heap_ModuleDesc__typ;
export LONGINT *Heap_CmdDesc__typ;
export LONGINT *Heap_FinDesc__typ;
export LONGINT *Heap__1__typ;

static void Heap_CheckFin (void);
static void Heap_ExtendHeap (LONGINT blksz);
export void Heap_FINALL (void);
static void Heap_Finalize (void);
export void Heap_GC (BOOLEAN markStack);
static void Heap_HeapSort (LONGINT n, Heap_ADR *a, LONGINT a__len);
export void Heap_INCREF (Heap_Module m);
export void Heap_InitHeap (void);
export void Heap_Lock (void);
static void Heap_Mark (Heap_ADR q);
static void Heap_MarkCandidates (LONGINT n, Heap_ADR *cand, LONGINT cand__len);
static void Heap_MarkP (SYSTEM_PTR p);
static void Heap_MarkStack (LONGINT n, Heap_ADR *cand, LONGINT cand__len);
export SYSTEM_PTR Heap_NEWBLK (LONGINT size);
export SYSTEM_PTR Heap_NEWREC (SYSTEM_PTR tag);
static Heap_ADR Heap_NewChunk (LONGINT blksz);
export void Heap_REGCMD (Heap_Module m, Heap_CmdName name, Heap_Command cmd);
export SYSTEM_PTR Heap_REGMOD (Heap_ModuleName name, Heap_EnumProc enumPtrs);
export void Heap_REGTYP (Heap_Module m, SYSTEM_PTR typ);
export void Heap_RegisterFinalizer (SYSTEM_PTR obj, Heap_Finalizer finalize);
static void Heap_Scan (void);
static void Heap_Sift (LONGINT l, LONGINT r, Heap_ADR *a, LONGINT a__len);
export void Heap_Unlock (void);

#define Heap_Adr1()	((Heap_ADR)(SYSTEM_ADR)1)
#define Heap_EqualOrLessThan(adr1, adr2)	((SYSTEM_ADR)(adr1) <= (SYSTEM_ADR)(adr2))
extern void *Heap__init();
extern SYSTEM_PTR Platform_MainStackFrame;
extern SYSTEM_PTR Platform_OSAllocate(INTEGER size);
#define Heap_FetchAddress(pointer)	(Heap_ADR)(SYSTEM_ADR)(*((void**)((SYSTEM_ADR)(pointer))))
#define Heap_GreaterThan(adr1, adr2)	((SYSTEM_ADR)(adr1) > (SYSTEM_ADR)(adr2))
#define Heap_HeapModuleInit()	Heap__init()
#define Heap_LessThan(adr1, adr2)	((SYSTEM_ADR)(adr1) < (SYSTEM_ADR)(adr2))
#define Heap_ODDadr(adr)	__ODD((SYSTEM_ADR)(adr))
#define Heap_OSAllocate(size)	((Heap_ADR)Platform_OSAllocate((INTEGER)(size)))
#define Heap_Offset(adr, offset)	((Heap_ADR)(((SYSTEM_ADR)(adr)) + (offset)))
#define Heap_PlatformHalt(code)	Platform_Halt(code)
#define Heap_PlatformMainStackFrame()	((LONGINT)(SYSTEM_ADR)Platform_MainStackFrame)

/*============================================================================*/

void Heap_Lock (void)
{
	Heap_lockdepth += 1;
}

/*----------------------------------------------------------------------------*/
void Heap_Unlock (void)
{
	Heap_lockdepth -= 1;
	if (Heap_interrupted && Heap_lockdepth == 0) {
		Heap_PlatformHalt(-9);
	}
}

/*----------------------------------------------------------------------------*/
SYSTEM_PTR Heap_REGMOD (Heap_ModuleName name, Heap_EnumProc enumPtrs)
{
	Heap_Module m = NIL;
	if (__STRCMP(name, "Heap") == 0) {
		__SYSNEW(m, 80);
	} else {
		__NEW(m, Heap_ModuleDesc);
	}
	m->types = NIL;
	m->cmds = NIL;
	__COPY(name, m->name, 20);
	m->refcnt = 0;
	m->enumPtrs = enumPtrs;
	m->next = (Heap_Module)Heap_modules;
	Heap_modules = (SYSTEM_PTR)m;
	return (void*)m;
}

/*----------------------------------------------------------------------------*/
void Heap_REGCMD (Heap_Module m, Heap_CmdName name, Heap_Command cmd)
{
	Heap_Cmd c = NIL;
	if (__STRCMP(m->name, "Heap") == 0) {
		__SYSNEW(c, 40);
	} else {
		__NEW(c, Heap_CmdDesc);
	}
	__COPY(name, c->name, 24);
	c->cmd = cmd;
	c->next = m->cmds;
	m->cmds = c;
}

/*----------------------------------------------------------------------------*/
void Heap_REGTYP (Heap_Module m, SYSTEM_PTR typ)
{
	__PUT(typ, m->types, Heap_ADR);
	m->types = (Heap_ADR)typ;
}

/*----------------------------------------------------------------------------*/
void Heap_INCREF (Heap_Module m)
{
	m->refcnt += 1;
}

/*----------------------------------------------------------------------------*/
static Heap_ADR Heap_NewChunk (LONGINT blksz)
{
	Heap_ADR chnk = NIL;
	chnk = Heap_OSAllocate(blksz + 24);
	if (chnk != NIL) {
		__PUT(Heap_Offset(chnk, 8), Heap_Offset(chnk, 24 + blksz), Heap_ADR);
		__PUT(Heap_Offset(chnk, 24), Heap_Offset(chnk, 32), Heap_ADR);
		__PUT(Heap_Offset(chnk, 32), blksz, LONGINT);
		__PUT(Heap_Offset(chnk, 40), -8, LONGINT);
		__PUT(Heap_Offset(chnk, 48), Heap_bigBlocks, Heap_ADR);
		Heap_bigBlocks = Heap_Offset(chnk, 24);
		Heap_heapsize += blksz;
	}
	return chnk;
}

static void Heap_ExtendHeap (LONGINT blksz)
{
	LONGINT size;
	Heap_ADR chnk = NIL, j = NIL, next = NIL;
	if (blksz > 320000) {
		size = blksz;
	} else {
		size = 320000;
	}
	chnk = Heap_NewChunk(size);
	if (chnk != NIL) {
		if (Heap_LessThan(chnk, Heap_heap)) {
			__PUT(chnk, Heap_heap, Heap_ADR);
			Heap_heap = chnk;
		} else {
			j = Heap_heap;
			next = Heap_FetchAddress(j);
			while (next != NIL && Heap_GreaterThan(chnk, next)) {
				j = next;
				next = Heap_FetchAddress(j);
			}
			__PUT(chnk, next, Heap_ADR);
			__PUT(j, chnk, Heap_ADR);
		}
		if (next == NIL) {
			Heap_heapend = Heap_FetchAddress(Heap_Offset(chnk, 8));
		}
	}
}

SYSTEM_PTR Heap_NEWREC (SYSTEM_PTR tag)
{
	LONGINT i, i0, di, blksz, restsize, t;
	Heap_ADR iadr = NIL, adr = NIL, end = NIL, next = NIL, prev = NIL;
	SYSTEM_PTR new = NIL;
	Heap_Lock();
	__GET(tag, blksz, LONGINT);
		i0 = __ASHR(blksz, 5, LONGINT);
	i = i0;
	if (i < 9) {
		adr = Heap_freeList[i];
		while (adr == NIL) {
			i += 1;
			adr = Heap_freeList[i];
		}
	}
	if (i < 9) {
		next = Heap_FetchAddress(Heap_Offset(adr, 24));
		Heap_freeList[i] = next;
		if (i != i0) {
			di = i - i0;
			restsize = __ASHL(di, 5, LONGINT);
			end = Heap_Offset(adr, restsize);
			__PUT(Heap_Offset(end, 8), blksz, LONGINT);
			__PUT(Heap_Offset(end, 16), -8, LONGINT);
			__PUT(end, Heap_Offset(end, 8), Heap_ADR);
			__PUT(Heap_Offset(adr, 8), restsize, LONGINT);
			__PUT(Heap_Offset(adr, 24), Heap_freeList[di], Heap_ADR);
			Heap_freeList[di] = adr;
			adr = Heap_Offset(adr, restsize);
		}
	} else {
		adr = Heap_bigBlocks;
		prev = NIL;
		for (;;) {
			if (adr == NIL) {
				if (Heap_firstTry) {
					Heap_GC(1);
					blksz += 32;
					if (__ASHL((Heap_heapsize - Heap_allocated) - blksz, 2, LONGINT) < Heap_heapsize) {
						Heap_ExtendHeap(__ASHL(__DIV(Heap_allocated + blksz, 96), 7, LONGINT) - Heap_heapsize);
					}
					Heap_firstTry = 0;
					new = Heap_NEWREC(tag);
					Heap_firstTry = 1;
					if (new == NIL) {
						Heap_ExtendHeap(__ASHL(__DIV(Heap_allocated + blksz, 96), 7, LONGINT) - Heap_heapsize);
						new = Heap_NEWREC(tag);
					}
					Heap_Unlock();
					return new;
				} else {
					Heap_Unlock();
					return NIL;
				}
			}
			__GET(Heap_Offset(adr, 8), t, LONGINT);
			if (t >= blksz) {
				break;
			}
			prev = adr;
			adr = Heap_FetchAddress(Heap_Offset(adr, 24));
		}
		restsize = t - blksz;
		end = Heap_Offset(adr, restsize);
		__PUT(Heap_Offset(end, 8), blksz, LONGINT);
		__PUT(Heap_Offset(end, 16), -8, LONGINT);
		__PUT(end, Heap_Offset(end, 8), Heap_ADR);
		if (restsize > 288) {
			__PUT(Heap_Offset(adr, 8), restsize, LONGINT);
		} else {
			next = Heap_FetchAddress(Heap_Offset(adr, 24));
			if (prev == NIL) {
				Heap_bigBlocks = next;
			} else {
				__PUT(Heap_Offset(prev, 24), next, Heap_ADR);
			}
			if (restsize > 0) {
				di = __ASHR(restsize, 5, LONGINT);
				__PUT(Heap_Offset(adr, 8), restsize, LONGINT);
				__PUT(Heap_Offset(adr, 24), Heap_freeList[di], Heap_ADR);
				Heap_freeList[di] = adr;
			}
		}
		adr = Heap_Offset(adr, restsize);
	}
	iadr = Heap_Offset(adr, 32);
	end = Heap_Offset(adr, blksz);
	while (Heap_LessThan(iadr, end)) {
		__PUT(iadr, 0, LONGINT);
		__PUT(Heap_Offset(iadr, 8), 0, LONGINT);
		__PUT(Heap_Offset(iadr, 16), 0, LONGINT);
		__PUT(Heap_Offset(iadr, 24), 0, LONGINT);
		iadr = Heap_Offset(iadr, 32);
	}
	__PUT(Heap_Offset(adr, 24), 0, LONGINT);
	__PUT(adr, tag, SYSTEM_PTR);
	__PUT(Heap_Offset(adr, 8), 0, LONGINT);
	__PUT(Heap_Offset(adr, 16), 0, LONGINT);
	Heap_allocated += blksz;
	Heap_Unlock();
	return (SYSTEM_PTR)Heap_Offset(adr, 8);
}

/*----------------------------------------------------------------------------*/
SYSTEM_PTR Heap_NEWBLK (LONGINT size)
{
	LONGINT blksz;
	Heap_ADR tag = NIL;
	SYSTEM_PTR new = NIL;
	Heap_Lock();
	blksz = __ASHL(__ASHR(size + 63, 5, LONGINT), 5, LONGINT);
	new = Heap_NEWREC((SYSTEM_PTR)((LONGINT)&blksz));
	tag = Heap_Offset((Heap_ADR)new, blksz - 24);
	__PUT(Heap_Offset(tag, -8), 0, LONGINT);
	__PUT(tag, blksz, LONGINT);
	__PUT(Heap_Offset(tag, 8), -8, LONGINT);
	__PUT(Heap_Offset((Heap_ADR)new, -8), tag, Heap_ADR);
	Heap_Unlock();
	return new;
}

/*----------------------------------------------------------------------------*/
static void Heap_Mark (Heap_ADR q)
{
	LONGINT offset;
	Heap_ADR p = NIL, tag = NIL, fld = NIL, n = NIL, tagbits = NIL;
	if (q != NIL) {
		tagbits = Heap_FetchAddress(Heap_Offset(q, -8));
		if (!Heap_ODDadr(tagbits)) {
			__PUT(Heap_Offset(q, -8), Heap_Offset(tagbits, 1), Heap_ADR);
			p = NIL;
			tag = Heap_Offset(tagbits, 8);
			for (;;) {
				__GET(tag, offset, LONGINT);
				if (offset < 0) {
					__PUT(Heap_Offset(q, -8), Heap_Offset(tag, offset + 1), Heap_ADR);
					if (p == NIL) {
						break;
					}
					n = q;
					q = p;
					tag = Heap_FetchAddress(Heap_Offset(q, -8));
					tag = Heap_Offset(tag, -1);
					__GET(tag, offset, LONGINT);
					fld = Heap_Offset(q, offset);
					p = Heap_FetchAddress(fld);
					__PUT(fld, n, Heap_ADR);
				} else {
					fld = Heap_Offset(q, offset);
					n = Heap_FetchAddress(fld);
					if (n != NIL) {
						tagbits = Heap_FetchAddress(Heap_Offset(n, -8));
						if (!Heap_ODDadr(tagbits)) {
							__PUT(Heap_Offset(n, -8), Heap_Offset(tagbits, 1), Heap_ADR);
							__PUT(Heap_Offset(q, -8), Heap_Offset(tag, 1), Heap_ADR);
							__PUT(fld, p, Heap_ADR);
							p = q;
							q = n;
							tag = tagbits;
						}
					}
				}
				tag = Heap_Offset(tag, 8);
			}
		}
	}
}

static void Heap_MarkP (SYSTEM_PTR p)
{
	Heap_Mark((Heap_ADR)p);
}

static void Heap_Scan (void)
{
	LONGINT i, size, freesize;
	Heap_ADR chnk = NIL, adr = NIL, end = NIL, start = NIL, tag = NIL;
	Heap_bigBlocks = NIL;
	i = 1;
	while (i < 9) {
		Heap_freeList[i] = NIL;
		i += 1;
	}
	freesize = 0;
	Heap_allocated = 0;
	chnk = Heap_heap;
	while (chnk != NIL) {
		adr = Heap_Offset(chnk, 24);
		end = Heap_FetchAddress(Heap_Offset(chnk, 8));
		while (Heap_LessThan(adr, end)) {
			tag = Heap_FetchAddress(adr);
			if (Heap_ODDadr(tag)) {
				if (freesize > 0) {
					start = Heap_Offset(adr, -freesize);
					__PUT(start, Heap_Offset(start, 8), Heap_ADR);
					__PUT(Heap_Offset(start, 8), freesize, LONGINT);
					__PUT(Heap_Offset(start, 16), -8, LONGINT);
					i = __ASHR(freesize, 5, LONGINT);
					freesize = 0;
					if (i < 9) {
						__PUT(Heap_Offset(start, 24), Heap_freeList[i], Heap_ADR);
						Heap_freeList[i] = start;
					} else {
						__PUT(Heap_Offset(start, 24), Heap_bigBlocks, Heap_ADR);
						Heap_bigBlocks = start;
					}
				}
				tag = Heap_Offset(tag, -1);
				__PUT(adr, tag, Heap_ADR);
				__GET(tag, size, LONGINT);
				Heap_allocated += size;
				adr = Heap_Offset(adr, size);
			} else {
				__GET(tag, size, LONGINT);
				freesize += size;
				adr = Heap_Offset(adr, size);
			}
		}
		if (freesize > 0) {
			start = Heap_Offset(adr, -freesize);
			__PUT(start, Heap_Offset(start, 8), Heap_ADR);
			__PUT(Heap_Offset(start, 8), freesize, LONGINT);
			__PUT(Heap_Offset(start, 16), -8, LONGINT);
			i = __ASHR(freesize, 5, LONGINT);
			freesize = 0;
			if (i < 9) {
				__PUT(Heap_Offset(start, 24), Heap_freeList[i], Heap_ADR);
				Heap_freeList[i] = start;
			} else {
				__PUT(Heap_Offset(start, 24), Heap_bigBlocks, Heap_ADR);
				Heap_bigBlocks = start;
			}
		}
		chnk = Heap_FetchAddress(chnk);
	}
}

static void Heap_Sift (LONGINT l, LONGINT r, Heap_ADR *a, LONGINT a__len)
{
	LONGINT i, j;
	Heap_ADR x = NIL;
	j = l;
	x = a[j];
	for (;;) {
		i = j;
		j = __ASHL(j, 1, LONGINT) + 1;
		if (j < r && Heap_LessThan(a[j], a[j + 1])) {
			j += 1;
		}
		if (j > r || Heap_EqualOrLessThan(a[j], x)) {
			break;
		}
		a[i] = a[j];
	}
	a[i] = x;
}

static void Heap_HeapSort (LONGINT n, Heap_ADR *a, LONGINT a__len)
{
	LONGINT l, r;
	Heap_ADR x = NIL;
	l = __ASHR(n, 1, LONGINT);
	r = n - 1;
	while (l > 0) {
		l -= 1;
		Heap_Sift(l, r, (void*)a, a__len);
	}
	while (r > 0) {
		x = a[0];
		a[0] = a[r];
		a[r] = x;
		r -= 1;
		Heap_Sift(l, r, (void*)a, a__len);
	}
}

static void Heap_MarkCandidates (LONGINT n, Heap_ADR *cand, LONGINT cand__len)
{
	LONGINT i, size;
	Heap_ADR chnk = NIL, adr = NIL, tag = NIL, next = NIL, lim = NIL, lim1 = NIL, ptr = NIL;
	chnk = Heap_heap;
	i = 0;
	lim = cand[n - 1];
	while (chnk != NIL && Heap_LessThan(chnk, lim)) {
		adr = Heap_Offset(chnk, 24);
		lim1 = Heap_FetchAddress(Heap_Offset(chnk, 8));
		if (Heap_LessThan(lim, lim1)) {
			lim1 = lim;
		}
		while (Heap_LessThan(adr, lim1)) {
			tag = Heap_FetchAddress(adr);
			if (Heap_ODDadr(tag)) {
				__GET(Heap_Offset(tag, -1), size, LONGINT);
				adr = Heap_Offset(adr, size);
			} else {
				__GET(tag, size, LONGINT);
				ptr = Heap_Offset(adr, 8);
				while (Heap_LessThan(cand[i], ptr)) {
					i += 1;
				}
				if (i == n) {
					return;
				}
				next = Heap_Offset(adr, size);
				if (Heap_LessThan(cand[i], next)) {
					Heap_Mark(ptr);
				}
				adr = next;
			}
		}
		chnk = Heap_FetchAddress(chnk);
	}
}

static void Heap_CheckFin (void)
{
	Heap_FinNode n = NIL;
	Heap_ADR tag = NIL;
	n = Heap_fin;
	while (n != NIL) {
		tag = Heap_FetchAddress(Heap_Offset(n->obj, -8));
		if (!Heap_ODDadr(tag)) {
			n->marked = 0;
			Heap_Mark(n->obj);
		} else {
			n->marked = 1;
		}
		n = n->next;
	}
}

static void Heap_Finalize (void)
{
	Heap_FinNode n = NIL, prev = NIL;
	n = Heap_fin;
	prev = NIL;
	while (n != NIL) {
		if (!n->marked) {
			if (n == Heap_fin) {
				Heap_fin = Heap_fin->next;
			} else {
				prev->next = n->next;
			}
			(*n->finalize)((SYSTEM_PTR)n->obj);
			if (prev == NIL) {
				n = Heap_fin;
			} else {
				n = n->next;
			}
		} else {
			prev = n;
			n = n->next;
		}
	}
}

void Heap_FINALL (void)
{
	Heap_FinNode n = NIL;
	while (Heap_fin != NIL) {
		n = Heap_fin;
		Heap_fin = Heap_fin->next;
		(*n->finalize)((SYSTEM_PTR)n->obj);
	}
}

/*----------------------------------------------------------------------------*/
static void Heap_MarkStack (LONGINT n, Heap_ADR *cand, LONGINT cand__len)
{
	SYSTEM_PTR frame = NIL;
	LONGINT inc, nofcand;
	Heap_ADR p = NIL, stack0 = NIL, sp = NIL;
	struct Heap__1 align;
	if (n > 0) {
		Heap_MarkStack(n - 1, cand, cand__len);
		if (n > 100) {
			return;
		}
	}
	if (n == 0) {
		nofcand = 0;
		sp = (Heap_ADR)((LONGINT)&frame);
		stack0 = (Heap_ADR)Heap_PlatformMainStackFrame();
		inc = (LONGINT)&align.p - (LONGINT)&align;
		if (Heap_GreaterThan(sp, stack0)) {
			inc = -inc;
		}
		while (sp != stack0) {
			__GET(sp, p, Heap_ADR);
			if (Heap_GreaterThan(p, Heap_heap) && Heap_LessThan(p, Heap_heapend)) {
				if (nofcand == cand__len) {
					Heap_HeapSort(nofcand, (void*)cand, cand__len);
					Heap_MarkCandidates(nofcand, (void*)cand, cand__len);
					nofcand = 0;
				}
				cand[nofcand] = p;
				nofcand += 1;
			}
			sp = Heap_Offset(sp, inc);
		}
		if (nofcand > 0) {
			Heap_HeapSort(nofcand, (void*)cand, cand__len);
			Heap_MarkCandidates(nofcand, (void*)cand, cand__len);
		}
	}
}

void Heap_GC (BOOLEAN markStack)
{
	Heap_Module m = NIL;
	LONGINT i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15, i16, i17, i18, i19, i20, i21, i22, i23;
	Heap_ADR cand[10000];
	if (Heap_lockdepth == 0 || Heap_lockdepth == 1 && !markStack) {
		Heap_Lock();
		m = (Heap_Module)Heap_modules;
		while (m != NIL) {
			if (m->enumPtrs != NIL) {
				(*m->enumPtrs)(Heap_MarkP);
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
					Heap_MarkStack(32, (void*)cand, 10000);
					break;
				}
			}
			if (((((((((((((((((((((((i0 + i1) + i2) + i3) + i4) + i5) + i6) + i7) + i8) + i9) + i10) + i11) + i12) + i13) + i14) + i15) + i16) + i17) + i18) + i19) + i20) + i21) + i22) + i23 > 10000) {
				return;
			}
		}
		Heap_CheckFin();
		Heap_Scan();
		Heap_Finalize();
		Heap_Unlock();
	}
}

/*----------------------------------------------------------------------------*/
void Heap_RegisterFinalizer (SYSTEM_PTR obj, Heap_Finalizer finalize)
{
	Heap_FinNode f = NIL;
	__NEW(f, Heap_FinDesc);
	f->obj = (Heap_ADR)obj;
	f->finalize = finalize;
	f->marked = 1;
	f->next = Heap_fin;
	Heap_fin = f;
}

/*----------------------------------------------------------------------------*/
void Heap_InitHeap (void)
{
	Heap_heap = Heap_NewChunk(256000);
	Heap_heapend = Heap_FetchAddress(Heap_Offset(Heap_heap, 8));
	__PUT(Heap_heap, 0, LONGINT);
	Heap_allocated = 0;
	Heap_firstTry = 1;
	Heap_freeList[9] = Heap_Adr1();
	Heap_lockdepth = 0;
	Heap_FileCount = 0;
	Heap_modules = NIL;
	Heap_heapsize = 0;
	Heap_bigBlocks = NIL;
	Heap_fin = NIL;
	Heap_interrupted = 0;
	Heap_HeapModuleInit();
}

/*----------------------------------------------------------------------------*/
static void EnumPtrs(void (*P)(void*))
{
	P(Heap_modules);
	P(Heap_fin);
}

__TDESC(Heap_ModuleDesc__desc, 1, 2) = {__TDFLDS("ModuleDesc", 80), {0, 40, -24}};
__TDESC(Heap_CmdDesc__desc, 1, 1) = {__TDFLDS("CmdDesc", 40), {0, -16}};
__TDESC(Heap_FinDesc__desc, 1, 1) = {__TDFLDS("FinDesc", 32), {0, -16}};
__TDESC(Heap__1__desc, 1, 1) = {__TDFLDS("", 16), {8, -16}};

export void *Heap__init(void)
{
	__DEFMOD;
	__REGMOD("Heap", EnumPtrs);
	__REGCMD("FINALL", Heap_FINALL);
	__REGCMD("InitHeap", Heap_InitHeap);
	__REGCMD("Lock", Heap_Lock);
	__REGCMD("Unlock", Heap_Unlock);
	__INITYP(Heap_ModuleDesc, Heap_ModuleDesc, 0);
	__INITYP(Heap_CmdDesc, Heap_CmdDesc, 0);
	__INITYP(Heap_FinDesc, Heap_FinDesc, 0);
	__INITYP(Heap__1, Heap__1, 0);
/* BEGIN */
	__ENDMOD;
}
