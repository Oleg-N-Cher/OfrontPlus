MODULE Heap;

  IMPORT SYSTEM;  (* Cannot import anything else as heap initialization must complete
                     before any other modules are initialized. *)

  CONST 
    ModNameLen = 20;
    CmdNameLen = 24;
    SZL        = SIZE(LONGINT);
    Unit       = 4*SZL;     (* smallest possible heap block *)
    nofLists   = 9;         (* number of free_lists *)
    heapSize0  = 8000*Unit; (* startup heap size *)

  (* all blocks look the same:
    free blocks describe themselves: size = Unit
    tag = &tag++
  ->block size
    sentinel = -SZL
    next
  *)

  (* heap chunks *)
    nextChnkOff = LONG(LONG(0));      (* next heap chunk, sorted ascendingly! *)
    endOff      = LONG(LONG(SZL));    (* end of heap chunk *)
    blkOff      = LONG(LONG(3*SZL));  (* first block in a chunk *)

  (* heap blocks *)
    tagOff    = LONG(LONG(0));        (* block starts with tag *)
    sizeOff   = LONG(LONG(SZL));      (* block size in free block relative to block start *)
    sntlOff   = LONG(LONG(2*SZL));    (* pointer offset table sentinel in free block relative to block start *)
    nextOff   = LONG(LONG(3*SZL));    (* next pointer in free block relative to block start *)
    NoPtrSntl = LONG(LONG(-SZL));
    LongZero  = LONG(LONG(0));

  TYPE
    ADR* = POINTER [1] TO ARRAY [1] 1 OF CHAR; (* 32 or 64 bit *)

    ModuleName = ARRAY ModNameLen OF CHAR;
    CmdName    = ARRAY CmdNameLen OF CHAR;

    Module = POINTER TO ModuleDesc;
    Cmd    = POINTER TO CmdDesc;

    EnumProc   = PROCEDURE(P: PROCEDURE(p: SYSTEM.PTR));

    ModuleDesc = RECORD
      next:     Module;
      name:     ModuleName;
      refcnt:   LONGINT;
      cmds:     Cmd;
      types:    ADR;
      enumPtrs: EnumProc;
      reserved1, reserved2: LONGINT
    END ;

    Command = PROCEDURE;

    CmdDesc = RECORD
      next: Cmd;
      name: CmdName;
      cmd:  Command
    END ;

    Finalizer = PROCEDURE(obj: SYSTEM.PTR);

    FinNode   = POINTER TO FinDesc;
    FinDesc   = RECORD
      next:     FinNode;
      obj:      ADR;      (* weak pointer *)
      marked:   BOOLEAN;
      finalize: Finalizer;
    END ;

  VAR
    (* the list of loaded (=initialization started) modules *)
    modules*: SYSTEM.PTR;

    freeList:   ARRAY nofLists + 1 OF ADR;  (* dummy, 16, 32, 48, 64, 80, 96, 112, 128, sentinel *)
    bigBlocks:  ADR;
    allocated*: LONGINT;
    firstTry:   BOOLEAN;

    (* extensible heap *)
    heap:      ADR;      (* the sorted list of heap chunks *)
    heapend:   ADR;      (* max possible pointer value (used for stack collection) *)
    heapsize*: LONGINT;  (* the sum of all heap chunk sizes *)

    (* finalization candidates *)
    fin: FinNode;

    (* garbage collector locking *)
    lockdepth:   INTEGER;
    interrupted: BOOLEAN;

    (* File system file count monitor *)
    FileCount*: INTEGER;


  (* The address arithmetic for the type ADR without converting to LONGINT *)
  PROCEDURE -LessThan(adr1, adr2: ADR): BOOLEAN "((SYSTEM_ADR)(adr1) < (SYSTEM_ADR)(adr2))";
  PROCEDURE -EqualOrLessThan(adr1, adr2: ADR): BOOLEAN "((SYSTEM_ADR)(adr1) <= (SYSTEM_ADR)(adr2))";
  PROCEDURE -GreaterThan(adr1, adr2: ADR): BOOLEAN "((SYSTEM_ADR)(adr1) > (SYSTEM_ADR)(adr2))";
  PROCEDURE -Offset(adr: ADR; offset: LONGINT): ADR "((Heap_ADR)(((SYSTEM_ADR)(adr)) + (offset)))";
  PROCEDURE -ODDadr(adr: ADR): BOOLEAN "__ODD((SYSTEM_ADR)(adr))";
  PROCEDURE -Adr1(): ADR "((Heap_ADR)(SYSTEM_ADR)1)";


  PROCEDURE Lock*;
  BEGIN
    INC(lockdepth);
  END Lock;

  PROCEDURE -PlatformHalt(code: INTEGER) "Platform_Halt(code)";

  PROCEDURE Unlock*;
  BEGIN
    DEC(lockdepth);
    IF interrupted & (lockdepth = 0) THEN
      PlatformHalt(-9);
    END
  END Unlock;


  (*
    PROCEDURE TAS*(VAR flag:BOOLEAN): BOOLEAN; (* added for compatibility with ulmSYSTEM module; noch *)
    VAR oldflag : BOOLEAN;
    BEGIN
       oldflag := flag;
       flag := TRUE;
       RETURN oldflag;
    END TAS;
  *)

  PROCEDURE REGMOD*(VAR name: ModuleName; enumPtrs: EnumProc): SYSTEM.PTR;
    VAR m: Module;
  BEGIN
    (* REGMOD is called at the start of module initialisation code before that modules
       type descriptors have been set up. "NEW" depends on the Heap modules type 
       descriptors being ready for use, therefore, just for the Heap module itself, we 
       must use SYSTEM.NEW. *)
    IF name = "Heap" THEN
      SYSTEM.NEW(m, SIZE(ModuleDesc))
    ELSE
      NEW(m)
    END;
    m.types := NIL; m.cmds := NIL;
    COPY(name, m.name); m.refcnt := 0; m.enumPtrs := enumPtrs; m.next := SYSTEM.VAL(Module, modules);
    modules := m;
    RETURN m
  END REGMOD;

  PROCEDURE REGCMD*(m: Module; VAR name: CmdName; cmd: Command);
    VAR c: Cmd;
  BEGIN
    (* REGCMD is called during module initialisation code before that modules
       type descriptors have been set up. "NEW" depends on the Heap modules type 
       descriptors being ready for use, therefore, just for the commands registered
       by the Heap module itself, we must use SYSTEM.NEW. *)
    IF m.name = "Heap" THEN
      SYSTEM.NEW(c, SIZE(CmdDesc))
    ELSE
      NEW(c)
    END;
    COPY(name, c.name); c.cmd := cmd; c.next := m.cmds; m.cmds := c
  END REGCMD;

  PROCEDURE REGTYP*(m: Module; typ: SYSTEM.PTR);
  BEGIN SYSTEM.PUT(typ, m.types); m.types := SYSTEM.VAL(ADR, typ)
  END REGTYP;

  PROCEDURE INCREF*(m: Module);
  BEGIN INC(m.refcnt)
  END INCREF;


  PROCEDURE -ExternPlatformOSAllocate       "extern SYSTEM_PTR Platform_OSAllocate(INTEGER size);";
  PROCEDURE -OSAllocate(size: LONGINT): ADR "((Heap_ADR)Platform_OSAllocate((INTEGER)(size)))";

  PROCEDURE NewChunk(blksz: LONGINT): ADR;
    VAR chnk: ADR;
  BEGIN
    chnk := OSAllocate(blksz + blkOff);
    IF chnk # NIL THEN
      SYSTEM.PUT(Offset(chnk, + endOff),           Offset(chnk, blkOff + blksz));
      SYSTEM.PUT(Offset(chnk, + blkOff),           Offset(chnk, blkOff + sizeOff));
      SYSTEM.PUT(Offset(chnk, + blkOff + sizeOff), blksz);
      SYSTEM.PUT(Offset(chnk, + blkOff + sntlOff), NoPtrSntl);
      SYSTEM.PUT(Offset(chnk, + blkOff + nextOff), bigBlocks);
      bigBlocks := Offset(chnk, + blkOff);
      INC(heapsize, blksz)
    END ;
    RETURN chnk
  END NewChunk;


  (* FetchAddress fetches a pointer from memory and returns it as a LONGINT. It works
     correctly regardless of the size of an address. Specifically on 32 bit address
     architectures with 64 bit LONGINT, it loads 32 bits and extends it to LONGINT 
     rather than loading 64 bits. *)
  PROCEDURE -FetchAddress(pointer: ADR): ADR "(Heap_ADR)(SYSTEM_ADR)(*((void**)((SYSTEM_ADR)(pointer))))";

  PROCEDURE ExtendHeap(blksz: LONGINT);
    VAR size: LONGINT; chnk, j, next: ADR;
  BEGIN
    IF blksz > 10000*Unit THEN size := blksz
    ELSE size := 10000*Unit  (* additional heuristics *)
    END ;
    chnk := NewChunk(size);
    IF chnk # NIL THEN
      (*sorted insertion*)
      IF LessThan(chnk, heap) THEN
        SYSTEM.PUT(chnk, heap); heap := chnk
      ELSE
        j := heap; next := FetchAddress(j);
        WHILE (next # NIL) & (GreaterThan(chnk, next)) DO
          j := next;
          next := FetchAddress(j)
        END;
        SYSTEM.PUT(chnk, next); SYSTEM.PUT(j, chnk)
      END ;
      IF next = NIL THEN heapend := FetchAddress(Offset(chnk, +endOff)) END
    END
  END ExtendHeap;

  PROCEDURE ^GC*(markStack: BOOLEAN);

  PROCEDURE NEWREC*(tag: SYSTEM.PTR): SYSTEM.PTR;
    VAR
      i, i0, di, blksz, restsize, t: LONGINT; iadr, adr, end, next, prev: ADR;
      new:   SYSTEM.PTR;
  BEGIN
    Lock();
    SYSTEM.GET(tag, blksz);

    ASSERT((Unit = 16) OR (Unit = 32));
    ASSERT(SIZE(SYSTEM.PTR) <= SIZE(LONGINT));
    ASSERT(blksz MOD Unit = 0);

    i0 := blksz DIV Unit; i := i0;
    IF i < nofLists THEN adr := freeList[i];
      WHILE adr = NIL DO INC(i); adr := freeList[i] END
    END ;
    IF i < nofLists THEN (* unlink *)
      next := FetchAddress(Offset(adr, + nextOff));
      freeList[i] := next;
      IF i # i0 THEN (* split *)
        di := i - i0; restsize := di * Unit; end := Offset(adr, + restsize);
        SYSTEM.PUT(Offset(end, sizeOff), + blksz);
        SYSTEM.PUT(Offset(end, sntlOff), + NoPtrSntl);
        SYSTEM.PUT(       end,             Offset(end, sizeOff));
        SYSTEM.PUT(Offset(adr, sizeOff), + restsize);
        SYSTEM.PUT(Offset(adr, nextOff),   freeList[di]);
        freeList[di] := adr;
        adr := Offset(adr, + restsize)
      END
    ELSE
      adr := bigBlocks; prev := NIL;
      LOOP
        IF adr = NIL THEN
          IF firstTry THEN
            GC(TRUE); INC(blksz, Unit);
            IF (heapsize - allocated - blksz) * 4 < heapsize THEN
              (* heap is still almost full; expand to avoid thrashing *)
              ExtendHeap((allocated + blksz) DIV (3*Unit) * (4*Unit) - heapsize)
            END ;
            firstTry := FALSE; new := NEWREC(tag); firstTry := TRUE;
            IF new = NIL THEN
              (* depending on the fragmentation, the heap may not have been extended by
                the anti-thrashing heuristics above *)
              ExtendHeap((allocated + blksz) DIV (3*Unit) * (4*Unit) - heapsize);
              new := NEWREC(tag);  (* will find a free block if heap has been expanded properly *)
            END ;
            Unlock(); RETURN new
          ELSE
            Unlock(); RETURN NIL
          END
        END ;
        SYSTEM.GET(Offset(adr, +sizeOff), t);
        IF t >= blksz THEN EXIT END ;
        prev := adr; adr := FetchAddress(Offset(adr, + nextOff))
      END ;
      restsize := t - blksz; end := Offset(adr, + restsize);
      SYSTEM.PUT(Offset(end, + sizeOff), blksz);
      SYSTEM.PUT(Offset(end, + sntlOff), NoPtrSntl);
      SYSTEM.PUT(       end,           Offset(end, + sizeOff));
      IF restsize > nofLists * Unit THEN (*resize*)
        SYSTEM.PUT(Offset(adr, + sizeOff), restsize)
      ELSE (*unlink*)
        next := FetchAddress(Offset(adr, + nextOff));
        IF prev = NIL THEN bigBlocks := next
        ELSE SYSTEM.PUT(Offset(prev, + nextOff), next);
        END ;
        IF restsize > 0 THEN (*move*)
          di := restsize DIV Unit;
          SYSTEM.PUT(Offset(adr, + sizeOff), restsize);
          SYSTEM.PUT(Offset(adr, + nextOff), freeList[di]);
          freeList[di] := adr
        END
      END ;
      adr := Offset(adr, + restsize)
    END ;
    iadr := Offset(adr, + 4*SZL); end := Offset(adr, + blksz);
    WHILE LessThan(iadr, end) DO  (*deliberately unrolled*)
      SYSTEM.PUT(       iadr,           LongZero);
      SYSTEM.PUT(Offset(iadr, + SZL),   LongZero);
      SYSTEM.PUT(Offset(iadr, + 2*SZL), LongZero);
      SYSTEM.PUT(Offset(iadr, + 3*SZL), LongZero);
      iadr := Offset(iadr, + 4*SZL)
    END ;
    SYSTEM.PUT(Offset(adr, + nextOff),  LongZero);
    SYSTEM.PUT(       adr,              tag);
    SYSTEM.PUT(Offset(adr, + sizeOff),  LongZero);
    SYSTEM.PUT(Offset(adr, + sntlOff),  LongZero);
    INC(allocated, blksz);
    Unlock();
    RETURN SYSTEM.VAL(SYSTEM.PTR, Offset(adr, + SZL))
  END NEWREC;

  PROCEDURE NEWBLK*(size: LONGINT): SYSTEM.PTR;
     VAR blksz: LONGINT; tag: ADR; new: SYSTEM.PTR;
  BEGIN
    Lock();
    blksz := (size + (4*SZL + Unit - 1)) DIV Unit * Unit;  (*size + tag + meta + blksz + sntnl + UnitAlignment*)
    new := NEWREC(SYSTEM.VAL(SYSTEM.PTR, SYSTEM.ADR(blksz)));
    tag := Offset(SYSTEM.VAL(ADR, new), + blksz - 3*SZL);
    SYSTEM.PUT(Offset(tag, - SZL), LongZero); (*reserved for meta info*)
    SYSTEM.PUT(tag,                blksz);
    SYSTEM.PUT(Offset(tag, + SZL), NoPtrSntl);
    SYSTEM.PUT(Offset(SYSTEM.VAL(ADR, new), - SZL), tag);
    Unlock();
    RETURN new
  END NEWBLK;

  PROCEDURE Mark(q: ADR);
    VAR offset: LONGINT; p, tag, fld, n, tagbits: ADR;
  BEGIN
    IF q # NIL THEN
      tagbits := FetchAddress(Offset(q, - SZL));    (* Load the tag for the record at q *)
      IF ~ODDadr(tagbits) THEN                         (* If it has not already been marked *)
        SYSTEM.PUT(Offset(q, - SZL), Offset(tagbits, + 1));           (* Mark it *)
        p := NIL;
        tag := Offset(tagbits, + SZL);              (* Tag addresses first offset *)
        LOOP
          SYSTEM.GET(tag, offset);                  (* Get next ptr field offset *)
          IF offset < 0 THEN                        (* If sentinel. (Value is -8*(#fields+1) *)
            SYSTEM.PUT(Offset(q, - SZL), Offset(tag, + offset + 1));  (* Rotate base ptr into tag *)
            IF p = NIL THEN EXIT END ;
            n := q; q := p;
            tag := FetchAddress(Offset(q, - SZL)); tag := Offset(tag, - 1);
            SYSTEM.GET(tag, offset); fld := Offset(q, + offset);
            p := FetchAddress(fld); SYSTEM.PUT(fld, n)
          ELSE                                      (* offset references a ptr field *)
            fld := Offset(q, + offset);             (* Address the pointer *)
            n := FetchAddress(fld);                 (* Load the pointer *)
            IF n # NIL THEN                         (* If pointer is not NIL *)
              tagbits := FetchAddress(Offset(n, - SZL));     (* Consider record pointed to by this field *)
              IF ~ODDadr(tagbits) THEN
                SYSTEM.PUT(Offset(n, - SZL), Offset(tagbits, + 1));
                SYSTEM.PUT(Offset(q, - SZL), Offset(tag, + 1));
                SYSTEM.PUT(fld, p);
                p := q; q := n;
                tag := tagbits
              END
            END
          END ;
          tag := Offset(tag, + SZL)
        END
      END
    END
  END Mark;

  PROCEDURE MarkP(p: SYSTEM.PTR);  (* for compatibility with EnumPtrs in ANSI mode *)
  BEGIN
    Mark(SYSTEM.VAL(ADR, p))
  END MarkP;

  PROCEDURE Scan;
    VAR i, size, freesize: LONGINT; chnk, adr, end, start, tag: ADR;
  BEGIN bigBlocks := NIL; i := 1;
    WHILE i < nofLists DO freeList[i] := NIL; INC(i) END ;
    freesize := 0; allocated := 0; chnk := heap;
    WHILE chnk # NIL DO
      adr := Offset(chnk, + blkOff);
      end := FetchAddress(Offset(chnk, + endOff));
      WHILE LessThan(adr, end) DO
        tag := FetchAddress(adr);
        IF ODDadr(tag) THEN  (*marked*)
          IF freesize > 0 THEN
            start := Offset(adr, - freesize);
            SYSTEM.PUT(       start,             Offset(start, + SZL));
            SYSTEM.PUT(Offset(start, + sizeOff), freesize);
            SYSTEM.PUT(Offset(start, + sntlOff), NoPtrSntl);
            i := freesize DIV Unit; freesize := 0;
            IF i < nofLists THEN SYSTEM.PUT(Offset(start, + nextOff), freeList[i]); freeList[i] := start
            ELSE                 SYSTEM.PUT(Offset(start, + nextOff), bigBlocks);   bigBlocks   := start
            END
          END ;
          tag := Offset(tag, - 1);
          SYSTEM.PUT(adr, tag);
          SYSTEM.GET(tag, size);
          INC(allocated, size);
          adr := Offset(adr, + size)
        ELSE  (*unmarked*)
          SYSTEM.GET(tag, size);
          INC(freesize, size);
          adr := Offset(adr, + size)
        END
      END ;
      IF freesize > 0 THEN (*collect last block*)
        start := Offset(adr, - freesize);
        SYSTEM.PUT(       start,             Offset(start, + SZL));
        SYSTEM.PUT(Offset(start, + sizeOff), freesize);
        SYSTEM.PUT(Offset(start, + sntlOff), NoPtrSntl);
        i := freesize DIV Unit; freesize := 0;
        IF i < nofLists THEN SYSTEM.PUT(Offset(start, + nextOff), freeList[i]); freeList[i] := start
        ELSE                 SYSTEM.PUT(Offset(start, + nextOff), bigBlocks);   bigBlocks   := start
        END
      END ;
      chnk := FetchAddress(chnk)
    END
  END Scan;

  PROCEDURE Sift (l, r: LONGINT; VAR a: ARRAY OF ADR);
    VAR i, j: LONGINT; x: ADR;
  BEGIN j := l; x := a[j];
    LOOP i := j; j := 2*j + 1;
      IF (j < r) & (LessThan(a[j], a[j+1])) THEN INC(j) END;
      IF (j > r) OR (EqualOrLessThan(a[j], x)) THEN EXIT END;
      a[i] := a[j]
    END;
    a[i] := x
  END Sift;

  PROCEDURE HeapSort (n: LONGINT; VAR a: ARRAY OF ADR);
    VAR l, r: LONGINT; x: ADR;
  BEGIN l := n DIV 2; r := n - 1;
    WHILE l > 0 DO DEC(l); Sift(l, r, a) END;
    WHILE r > 0 DO x := a[0]; a[0] := a[r]; a[r] := x; DEC(r); Sift(l, r, a) END
  END HeapSort;

  PROCEDURE MarkCandidates(n: LONGINT; VAR cand: ARRAY OF ADR);
    VAR i, size: LONGINT; chnk, adr, tag, next, lim, lim1, ptr: ADR;
  BEGIN
    chnk := heap; i := 0; lim := cand[n-1];
    WHILE (chnk # NIL ) & (LessThan(chnk, lim)) DO
      adr := Offset(chnk, + blkOff);
      lim1 := FetchAddress(Offset(chnk, + endOff));
      IF LessThan(lim, lim1) THEN lim1 := lim END ;
      WHILE LessThan(adr, lim1) DO
        tag := FetchAddress(adr);
        IF ODDadr(tag) THEN (*already marked*)
          SYSTEM.GET(Offset(tag, - 1), size); adr := Offset(adr, + size)
        ELSE
          SYSTEM.GET(tag, size);
          ptr := Offset(adr, + SZL);
          WHILE LessThan(cand[i], ptr) DO INC(i) END ;
          IF i = n THEN RETURN END ;
          next := Offset(adr, + size);
          IF LessThan(cand[i], next) THEN Mark(ptr) END ;
          adr := next
        END
      END ;
      chnk := FetchAddress(chnk)
    END
  END MarkCandidates;

  PROCEDURE CheckFin;
    VAR n: FinNode; tag: ADR;
  BEGIN 
    n := fin;
    WHILE n # NIL DO
      tag := FetchAddress(Offset(n.obj, - SZL));
      IF ~ODDadr(tag) THEN n.marked := FALSE; Mark(n.obj)
      ELSE n.marked := TRUE
      END ;
      n := n.next
    END
  END CheckFin;

  PROCEDURE Finalize;
    VAR n, prev: FinNode;
  BEGIN n := fin; prev := NIL;
    WHILE n # NIL DO
      IF ~n.marked THEN
        IF n = fin THEN fin := fin.next ELSE prev.next := n.next END ;
        n.finalize(SYSTEM.VAL(SYSTEM.PTR, n.obj));
        (* new nodes may have been pushed in n.finalize, therefore: *)
        IF prev = NIL THEN n := fin ELSE n := n.next END
      ELSE 
        prev := n; n := n.next
      END
    END
  END Finalize;

  PROCEDURE FINALL*;
    VAR n: FinNode;
  BEGIN
    WHILE fin # NIL DO
      n := fin; fin := fin.next;
      n.finalize(SYSTEM.VAL(SYSTEM.PTR, n.obj))
    END
  END FINALL;

  PROCEDURE -ExternMainStackFrame              "extern SYSTEM_PTR Platform_MainStackFrame;";
  PROCEDURE -PlatformMainStackFrame(): LONGINT "((LONGINT)(SYSTEM_ADR)Platform_MainStackFrame)";

  PROCEDURE MarkStack(n: LONGINT; VAR cand: ARRAY OF ADR);
    VAR
      frame: SYSTEM.PTR;
      inc, nofcand: LONGINT;
      p, stack0, sp: ADR;
      align: RECORD ch: CHAR; p: SYSTEM.PTR END ;
  BEGIN
    IF n > 0 THEN MarkStack(n-1, cand);  (* flush register windows by means of recursive calls *)
      IF n > 100 THEN RETURN END   (* prevent tail recursion optimization *)
    END ;
    IF n = 0 THEN
      nofcand := 0; sp := SYSTEM.VAL(ADR, SYSTEM.ADR(frame));
      stack0 := SYSTEM.VAL(ADR, PlatformMainStackFrame());
      (* check for minimum alignment of pointers *)
      inc := SYSTEM.ADR(align.p) - SYSTEM.ADR(align);
      IF GreaterThan(sp, stack0) THEN inc := -inc END ;
      WHILE sp # stack0 DO
        SYSTEM.GET(sp, p);
        IF (GreaterThan(p, heap)) & (LessThan(p, heapend)) THEN
          IF nofcand = LEN(cand) THEN HeapSort(nofcand, cand); MarkCandidates(nofcand, cand); nofcand := 0 END ;
          cand[nofcand] := p; INC(nofcand)
        END ;
        sp := Offset(sp, + inc)
      END ;
      IF nofcand > 0 THEN HeapSort(nofcand, cand); MarkCandidates(nofcand, cand) END
    END
  END MarkStack;

  PROCEDURE GC*(markStack: BOOLEAN);
    VAR
      m: Module;
      i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15, i16, i17, i18, i19, i20, i21, i22, i23: LONGINT;
      cand: ARRAY 10000 OF ADR;
  BEGIN
    IF (lockdepth = 0) OR (lockdepth = 1) & ~markStack THEN
      Lock();
      m := SYSTEM.VAL(Module, modules);
      WHILE m # NIL DO
        IF m.enumPtrs # NIL THEN m.enumPtrs(MarkP) END ;
        m := m^.next
      END ;
      IF markStack THEN
        (* generate register pressure to force callee saved registers to memory;
          may be simplified by inlining OS calls or processor specific instructions
        *)
        i0 := -100; i1 := -101; i2 := -102; i3 := -103; i4 := -104; i5 := -105; i6 := -106; i7 := -107;
        i8 := 1; i9 := 2; i10 := 3; i11 := 4; i12 := 5; i13 := 6; i14 := 7; i15 := 8;
        i16 := 9; i17 := 10; i18 := 11; i19 := 12; i20 := 13; i21 := 14; i22 := 15; i23 := 16;
        LOOP INC(i0, 1); INC(i1, 2); INC(i2, 3); INC(i3, 4); INC(i4, 5); INC(i5, 6); INC(i6, 7); INC(i7, 8);
          INC(i8, 9); INC(i9, 10); INC(i10, 11); INC(i11, 12); INC(i12, 13); INC(i13, 14); INC(i14, 15); INC(i15, 16);
          INC(i16, 17); INC(i17, 18); INC(i18, 19); INC(i19, 20); INC(i20, 21); INC(i21, 22); INC(i22, 23); INC(i23, 24);
          IF (i0 = -99) & (i15 = 24) THEN MarkStack(32, cand); EXIT END
        END ;
        IF i0 + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 + i13 + i14 + i15
          + i16 + i17 + i18 + i19 + i20 + i21 + i22 + i23 > 10000 THEN RETURN    (* use all variables *)
        END ;
      END;
      CheckFin;
      Scan;
      Finalize;
      Unlock()
    END
  END GC;

  PROCEDURE RegisterFinalizer*(obj: SYSTEM.PTR; finalize: Finalizer);
    VAR f: FinNode;
  BEGIN NEW(f);
    f.obj := SYSTEM.VAL(ADR, obj); f.finalize := finalize; f.marked := TRUE; 
    f.next := fin; fin := f;
  END RegisterFinalizer;


PROCEDURE -ExternHeapInit "extern void *Heap__init();";
PROCEDURE -HeapModuleInit "Heap__init()";

  PROCEDURE InitHeap*;
  (* InitHeap is called by Platform.init before any module bodies have been
     initialised, to enable NEW, SYSTEM.NEW *)
  BEGIN
    heap    := NewChunk(heapSize0);
    heapend := FetchAddress(Offset(heap, + endOff));
    SYSTEM.PUT(heap, LongZero);
    allocated := 0; firstTry := TRUE; freeList[nofLists] := Adr1(); lockdepth := 0;
    FileCount := 0; modules := NIL; heapsize := 0; bigBlocks := NIL; fin := NIL;
    interrupted := FALSE;

    HeapModuleInit;
  END InitHeap;

END Heap.
