(*
* The Ofront+ runtime system, Version 1.0
* Copyright (c) Software Templ OG, 1994-2018, with advice from David C. W. Brown
*)

MODULE Heap;

  IMPORT S := SYSTEM;  (* Cannot import anything else as heap initialization must complete
                          before any other modules are initialized. *)
  TYPE
    ADDRESS = S.ADRINT;
    
  CONST
    ModNameLen    = 32;
    CmdNameLen    = 24;
    SZA           = SIZE(ADDRESS);    (* Size of address *)
    Unit          = 4*SZA;            (* Smallest possible heap block *)
    ldUnit        = 4 + SZA DIV 8;    (* Unit = 2^ldUnit, for unsigned division expressed as logical shift right *)
    nofLists      = 9;                (* Number of freelist entries excluding sentinel *)
    heapSize0     = 8000*Unit;        (* Startup heap size *)

  (* Allocated and free blocks look the same: they start with a type tag pointing to a type descriptor.
    A type descriptor starts with the block size followed by the pointer offset table (>=0) followed by a sentinel (< 0)
    that describes the size of the pointer offset table. Free blocks have a pseudo type descriptor inside.
    The size of free blocks must be <= Unit. Free blocks look like this:
    tag = &tag++
  -> blksize
    sentinel = -SZA
    next
  *)

  (* heap chunks *)
    nextChnkOff = S.VAL(ADDRESS, 0);      (* next heap chunk, sorted ascendingly! *)
    endOff      = S.VAL(ADDRESS, SZA);    (* end of heap chunk *)
    blkOff      = S.VAL(ADDRESS, 3*SZA);  (* first block in a chunk, starts with tag *)

  (* free heap blocks *)
    tagOff      = S.VAL(ADDRESS, 0*SZA);  (* any block starts with a tag *)
    sizeOff     = S.VAL(ADDRESS, 1*SZA);  (* block size in free block relative to block start *)
    sntlOff     = S.VAL(ADDRESS, 2*SZA);  (* pointer offset table sentinel in free block relative to block start *)
    nextOff     = S.VAL(ADDRESS, 3*SZA);  (* next pointer in free block relative to block start *)
    NoPtrSntl   = S.VAL(ADDRESS, -SZA);
    AddressZero = S.VAL(ADDRESS, 0);

  TYPE
    ModuleName* = ARRAY ModNameLen OF SHORTCHAR;
    CmdName*    = ARRAY CmdNameLen OF SHORTCHAR;

    Module* = POINTER TO ModuleDesc;
    Cmd*    = POINTER TO CmdDesc;

    EnumProc*   = PROCEDURE(P: PROCEDURE(p: S.PTR));

    ModuleDesc* = RECORD
      next-:     Module;
      name-:     ModuleName;
      refcnt-:   INTEGER;
      cmds-:     Cmd;
      types-:    S.PTR;
      enumPtrs-: EnumProc;
      reserved1,
      reserved2: ADDRESS
    END;

    Command* = PROCEDURE;

    CmdDesc* = RECORD
      next-: Cmd;
      name-: CmdName;
      cmd-:  Command
    END;

    Finalizer = PROCEDURE(obj: S.PTR);

    FinNode = POINTER TO FinDesc;
    FinDesc = RECORD
      next:     FinNode;
      obj:      ADDRESS;  (* weak pointer *)
      marked:   BOOLEAN;
      finalize: Finalizer
    END;

  VAR
    (* the list of loaded (=initialization started) modules *)
    modules-:   S.PTR; (*POINTER [1] TO ModuleDesc;*)

    freeList:   ARRAY nofLists + 1 OF ADDRESS;  (* dummy, 16, 32, 48, 64, 80, 96, 112, 128, sentinel *)
    bigBlocks:  ADDRESS;
    allocated*: ADDRESS;
    firstTry:   BOOLEAN;

    (* extensible heap *)
    heap-:          ADDRESS;  (* the sorted list of heap chunks *)
    heapMin:        ADDRESS;  (* Range of valid pointer values, used for stack collection *)
    heapMax:        ADDRESS;
    heapsize*:      ADDRESS;  (* the sum of all heap chunk sizes *)
    heapMinExpand*: ADDRESS;  (* minimum heap expansion size *)

    (* finalization candidates *)
    fin: FinNode;

    (* garbage collector locking *)
    lockGC*:     SHORTINT;
    lockdepth:   SHORTINT;
    interrupted: BOOLEAN;

    (* File system file count monitor *)
    FileCount*: INTEGER;


  PROCEDURE Lock*;
  BEGIN
    INC(lockdepth)
  END Lock;

  PROCEDURE -SystemHalt(code: INTEGER; mod: ARRAY OF SHORTCHAR; pos: INTEGER) "__HALT(code, mod, pos)";

  PROCEDURE Unlock*;
  BEGIN
    DEC(lockdepth);
    IF interrupted & (lockdepth = 0) THEN
      SystemHalt(-9, "Heap", 122)
    END
  END Unlock;


  PROCEDURE -uLT (x, y: ADDRESS): BOOLEAN  "((__U_ADRINT)x <  (__U_ADRINT)y)";
  PROCEDURE -uLE (x, y: ADDRESS): BOOLEAN  "((__U_ADRINT)x <= (__U_ADRINT)y)";

  PROCEDURE REGMOD* (IN name: ARRAY OF SHORTCHAR; enumPtrs: EnumProc): S.PTR;
    VAR m: Module;
  BEGIN
    ASSERT(LEN(name$) < ModNameLen, 114);
    (* REGMOD is called at the start of module initialization code before that modules
       type descriptors have been set up. 'NEW' depends on the Heap modules type
       descriptors being ready for use, therefore, just for the Heap module itself, we
       must use S.NEW. *)
    IF name = "Heap" THEN
      S.NEW(m, SIZE(ModuleDesc))
    ELSE
      NEW(m)
    END;
    m.types := NIL; m.cmds := NIL;

    m.name := name$;
    m.refcnt := 0; m.enumPtrs := enumPtrs; m.next := S.VAL(Module, modules);
    modules := m;
    RETURN m
  END REGMOD;

  PROCEDURE FreeModule* (IN name: ARRAY OF SHORTCHAR): INTEGER;
  (* Returns 0 if freed, -1 if not found, refcount if found and refcount > 0. *)
    VAR m, p: Module;
  BEGIN m := S.VAL(Module, modules);
    WHILE (m # NIL) & (m.name # name) DO p := m; m := m.next END;
    IF (m # NIL) & (m.refcnt = 0) THEN
      IF m = S.VAL(Module, modules) THEN modules := m.next
      ELSE p.next := m.next
      END;
      RETURN 0
    ELSE
      IF m = NIL THEN RETURN -1 ELSE RETURN m.refcnt END
    END
  END FreeModule;


  PROCEDURE REGCMD* (mod: S.PTR; IN name: ARRAY OF SHORTCHAR; cmd: Command);
    VAR c: Cmd; m: Module;
  BEGIN
    ASSERT(LEN(name$) < CmdNameLen, 114);
    (* REGCMD is called during module initialization code before that modules
       type descriptors have been set up. 'NEW' depends on the Heap modules type
       descriptors being ready for use, therefore, just for the commands registered
       by the Heap module itself, we must use S.NEW. *)
    m := mod;
    IF m.name = "Heap" THEN
      S.NEW(c, SIZE(CmdDesc))
    ELSE
      NEW(c)
    END;
    c.name := name$; c.cmd := cmd; c.next := m.cmds; m.cmds := c
  END REGCMD;

  PROCEDURE REGTYP* (mod: S.PTR; typ: S.PTR);
    VAR m: Module;
  BEGIN m := mod; S.PUT(typ, m.types); m.types := typ
  END REGTYP;

  PROCEDURE INCREF* (mod: S.PTR);
    VAR m: Module;
  BEGIN m := mod; INC(m.refcnt)
  END INCREF;


  PROCEDURE- ExternPlatformOSAllocate "extern SYSTEM_ADRINT Platform_OSAllocate (SYSTEM_ADRINT size);";
  PROCEDURE- OSAllocate (size: ADDRESS): ADDRESS "Platform_OSAllocate(size)";
  PROCEDURE- ExternPlatformOSFree     "extern void Platform_OSFree (SYSTEM_ADRINT address);";
  PROCEDURE- OSFree (address: ADDRESS)           "Platform_OSFree(address)";

  PROCEDURE NewChunk(blksz: ADDRESS): ADDRESS;
    VAR chnk, blk, end: ADDRESS;
  BEGIN
    chnk := OSAllocate(blksz + blkOff);
    IF chnk # 0 THEN
      blk := chnk + blkOff; (* Heap chunk consists of a single block *)
      end := blk  + blksz;
      S.PUT(chnk + endOff,  end);
      S.PUT(blk  + tagOff,  blk + sizeOff);
      S.PUT(blk  + sizeOff, blksz);
      S.PUT(blk  + sntlOff, NoPtrSntl);
      S.PUT(blk  + nextOff, bigBlocks);
      bigBlocks := blk;  (* Prepend block to list of big blocks *)
      INC(heapsize, blksz);
      (* Maintain heap range limits *)
      IF uLT(blk + SZA, heapMin) THEN heapMin := blk + SZA END;
      IF uLT(heapMax,   end)     THEN heapMax := end       END
    END;
    RETURN chnk
  END NewChunk;

  PROCEDURE ExtendHeap(blksz: ADDRESS);
    VAR size, chnk, j, next: ADDRESS;
  BEGIN
    ASSERT(blksz MOD Unit = 0);
    IF uLT(heapMinExpand, blksz) THEN size := blksz
    ELSE size := heapMinExpand  (* additional heuristics for avoiding many small heap expansions *)
    END;
    chnk := NewChunk(size);
    IF chnk # 0 THEN
      (*sorted insertion*)
      IF uLT(chnk, heap) THEN
        S.PUT(chnk, heap); heap := chnk
      ELSE
        j := heap; S.GET(j, next);
        WHILE (next # 0) & uLT(next, chnk) DO
          j := next;
          S.GET(j, next)
        END;
        S.PUT(chnk, next); S.PUT(j, chnk)
      END
    ELSIF ~firstTry THEN
      (* Heap memory exhausted, i.e. heap is not expanded and NEWREC() will return NIL.
         In order to be able to report a trap due to NIL access, there is more
         memory needed, which may be available by reducing heapMinExpand. *)
      heapMinExpand := Unit
    (* ELSE firstTry: ignore failed heap expansion for anti-thrashing heuristics. *)
    END
  END ExtendHeap;

  PROCEDURE ^GC*(markStack: BOOLEAN);

  PROCEDURE NEWREC* (tag: S.PTR): S.PTR;
    VAR
      i, i0, di, blksz, restsize, t, adr, end, next, prev: ADDRESS;
      new: S.PTR;
  BEGIN
    Lock();
    S.GET(tag, blksz);

    ASSERT(SIZE(S.PTR) = SIZE(ADDRESS));
    ASSERT(blksz MOD Unit = 0);

    i0 := S.LSH(blksz, -ldUnit); (*uDIV Unit*)
    i := i0;
    IF i < nofLists THEN adr := freeList[i];
      WHILE adr = 0 DO INC(i); adr := freeList[i] END
    END;
    IF i < nofLists THEN (* Unlink from freelist[i] *)
      S.GET(adr + nextOff, next);
      freeList[i] := next;
      IF i # i0 THEN (* Split *)
        di := i - i0; restsize := di * Unit; end := adr + restsize;
        S.PUT(end + sizeOff, blksz);
        S.PUT(end + sntlOff, NoPtrSntl);
        S.PUT(end,           end + sizeOff);
        S.PUT(adr + sizeOff, restsize);
        S.PUT(adr + nextOff, freeList[di]);
        freeList[di] := adr;
        INC(adr, restsize)
      END
    ELSE (* Search in bigBlocks *)
      adr := bigBlocks; prev := 0;
      LOOP
        IF adr = 0 THEN (* Nothing free *)
          IF firstTry THEN
            GC(TRUE); INC(blksz, Unit);
            (* Anti-thrashing heuristics: ensure 1/5 of the heap will not be allocated *)
            t := S.LSH(allocated + blksz, -(2+ldUnit)) (*uDIV (4*Unit)*) * (5*Unit) ; (* Minimum preferred heapsize *)
            IF uLT(heapsize, t) THEN ExtendHeap(t - heapsize)
              (* If there is not enough heap memory then the heap will be expanded below by blksz *)
            END;
            firstTry := FALSE; new := NEWREC(tag);
            IF new = NIL THEN (* Heap is 1/5 free but fragmentation prevented allocation *)
              ExtendHeap(blksz);
              new := NEWREC(tag)  (* Will find a free block if heap has been expanded successfully *)
            END;
            firstTry := TRUE;
            Unlock(); RETURN new
          ELSE
            Unlock(); RETURN NIL
          END
        END;
        S.GET(adr+sizeOff, t);
        IF uLE(blksz, t) THEN EXIT END;
        prev := adr; S.GET(adr + nextOff, adr)
      END;
      restsize := t - blksz; end := adr + restsize;
      S.PUT(end + sizeOff, blksz);
      S.PUT(end + sntlOff, NoPtrSntl);
      S.PUT(end,           end + sizeOff);
      IF uLT(nofLists * Unit, restsize) THEN (* Resize *)
        S.PUT(adr + sizeOff, restsize)
      ELSE (* Unlink *)
        S.GET(adr + nextOff, next);
        IF prev = 0 THEN bigBlocks := next
        ELSE S.PUT(prev + nextOff, next);
        END;
        IF restsize # 0 THEN (* Move *)
          di := restsize DIV Unit;
          S.PUT(adr + sizeOff, restsize);
          S.PUT(adr + nextOff, freeList[di]);
          freeList[di] := adr
        END
      END;
      INC(adr, restsize)
    END;
    i := adr + 4*SZA; end := adr + blksz;
    WHILE uLT(i, end) DO  (* Deliberately unrolled *)
      S.PUT(i,         AddressZero);
      S.PUT(i + SZA,   AddressZero);
      S.PUT(i + 2*SZA, AddressZero);
      S.PUT(i + 3*SZA, AddressZero);
      INC(i, 4*SZA)
    END;
    S.PUT(adr + nextOff, AddressZero);
    S.PUT(adr,           tag);
    S.PUT(adr + sizeOff, AddressZero);
    S.PUT(adr + sntlOff, AddressZero);
    INC(allocated, blksz);
    Unlock();
    RETURN S.VAL(S.PTR, adr + SZA)
  END NEWREC;

  PROCEDURE NEWBLK*(size: ADDRESS): S.PTR;
     VAR blksz, tag: ADDRESS; new: S.PTR;
  BEGIN
    Lock();
    blksz := (size + (4*SZA + Unit - 1)) DIV Unit * Unit;  (*size + tag + meta + blksz + sntnl + UnitAlignment*)
    new := NEWREC(S.VAL(S.PTR, S.ADR(blksz)));
    tag := S.VAL(ADDRESS, new) + blksz - 3*SZA;
    S.PUT(tag - SZA,                      AddressZero); (*reserved for meta info*)
    S.PUT(tag,                            blksz);
    S.PUT(tag + SZA,                      NoPtrSntl);
    S.PUT(S.VAL(ADDRESS, new) - SZA, tag);
    Unlock();
    RETURN new
  END NEWBLK;

  PROCEDURE Mark(q: ADDRESS);
    VAR p, tag, offset, fld, n, tagbits: ADDRESS;
  BEGIN
    IF q # 0 THEN
      S.GET(q - SZA, tagbits);                 (* Load the tag for the record at q *)
      IF ~ODD(tagbits) THEN                    (* If it has not already been marked *)
        S.PUT(q - SZA, tagbits + 1);           (* Mark it *)
        p := 0;
        tag := tagbits + SZA;                  (* Tag addresses first offset *)
        LOOP
          S.GET(tag, offset);                  (* Get next ptr field offset *)
          IF offset < 0 THEN                   (* Sentinel reached: Value is -8*(#fields+1) *)
            S.PUT(q - SZA, tag + offset + 1);  (* Rotate base ptr into tag *)
            IF p = 0 THEN EXIT END;
            n := q; q := p;
            S.GET(q - SZA, tag); DEC(tag, 1);
            S.GET(tag, offset); fld := q + offset;
            S.GET(fld, p); S.PUT(fld, n)
          ELSE                                 (* offset references a ptr field *)
            fld := q + offset;                 (* ADDRESS the pointer *)
            S.GET(fld, n);                     (* Load the pointer *)
            IF n # 0 THEN                      (* If pointer is not NIL *)
              S.GET(n - SZA, tagbits);         (* Consider record pointed to by this field *)
              IF ~ODD(tagbits) THEN
                S.PUT(n - SZA, tagbits + 1);
                S.PUT(q - SZA, tag + 1);
                S.PUT(fld, p);
                p := q; q := n;
                tag := tagbits
              END
            END
          END;
          INC(tag, SZA)
        END
      END
    END
  END Mark;

  PROCEDURE MarkP(p: S.PTR);  (* for compatibility with EnumPtrs in ANSI mode *)
  BEGIN
    Mark(S.VAL(ADDRESS, p))
  END MarkP;

  PROCEDURE Scan;
    VAR chnk, prevChnk, nextChnk, adr, end, start, tag, i, size, freesize: ADDRESS;
  BEGIN bigBlocks := 0; i := 1;
    WHILE i < nofLists DO freeList[i] := 0; INC(i) END;
    freesize := 0; allocated := 0; chnk := heap; prevChnk := 0;
    WHILE chnk # 0 DO
      adr := chnk + blkOff;
      S.GET(chnk + endOff, end);
      WHILE uLT(adr, end) DO
        S.GET(adr, tag);
        IF ODD(tag) THEN  (* Marked *)
          IF freesize # 0 THEN
            start := adr - freesize;
            S.PUT(start,         start+SZA);
            S.PUT(start+sizeOff, freesize);
            S.PUT(start+sntlOff, NoPtrSntl);
            i := S.LSH(freesize, -ldUnit) (*uDIV Unit*);
            freesize := 0;
            IF uLT(i, nofLists) THEN S.PUT(start + nextOff, freeList[i]); freeList[i] := start
            ELSE                     S.PUT(start + nextOff, bigBlocks);   bigBlocks   := start
            END
          END;
          DEC(tag, 1);
          S.PUT(adr, tag);
          S.GET(tag, size);
          INC(allocated, size);
          INC(adr, size)
        ELSE  (*unmarked*)
          S.GET(tag, size);
          INC(freesize, size);
          INC(adr, size)
        END
      END;
      (* new variant with recycling of empty chunks *)
      S.GET(chnk + nextChnkOff, nextChnk);
      IF freesize # 0 THEN (*collect last block(s)*)
        start := adr - freesize;
        IF (start = chnk + blkOff) & (prevChnk # nextChnk) THEN (* chnk is empty and not the only node *)
          DEC(heapsize, freesize);
          IF prevChnk = 0 THEN heap := nextChnk; heapMin := nextChnk + blkOff + SZA
          ELSIF nextChnk # 0 THEN S.PUT(prevChnk + nextChnkOff, nextChnk)
          ELSE S.PUT(prevChnk + nextChnkOff, nextChnk); heapMax := end
          END;
          OSFree(chnk)
        ELSE
          S.PUT(start, start+SZA);
          S.PUT(start+sizeOff, freesize);
          S.PUT(start+sntlOff, NoPtrSntl);
          i := S.LSH(freesize, -ldUnit) (*uDIV Unit*);
          IF uLT(i, nofLists) THEN S.PUT(start + nextOff, freeList[i]); freeList[i] := start
          ELSE S.PUT(start + nextOff, bigBlocks); bigBlocks := start
          END;
          prevChnk := chnk
        END;
        freesize := 0
      ELSE prevChnk := chnk
      END;
      chnk :=  nextChnk
      (* old variant without recycling of empty chunks
      IF freesize # 0 THEN (*collect last block(s)*)
        start := adr - freesize;
        S.PUT(start,         start+SZA);
        S.PUT(start+sizeOff, freesize);
        S.PUT(start+sntlOff, NoPtrSntl);
        i := S.LSH(freesize, -ldUnit) (*uDIV Unit*);
        freesize := 0;
        IF uLT(i, nofLists) THEN S.PUT(start + nextOff, freeList[i]); freeList[i] := start
        ELSE                     S.PUT(start + nextOff, bigBlocks);   bigBlocks   := start
        END
      END;
      S.GET(chnk, chnk)
      *)
    END
  END Scan;

  PROCEDURE Sift (l, r: INTEGER; VAR a: ARRAY OF ADDRESS);
    VAR i, j: INTEGER; x: ADDRESS;
  BEGIN j := l; x := a[j];
    LOOP i := j; j := 2*j + 1;
      IF (j < r) &  uLT(a[j], a[j+1]) THEN INC(j) END;
      IF (j > r) OR uLE(a[j], x) THEN EXIT END;
      a[i] := a[j]
    END;
    a[i] := x
  END Sift;

  PROCEDURE HeapSort (n: INTEGER; VAR a: ARRAY OF ADDRESS);
    VAR l, r: INTEGER; x: ADDRESS;
  BEGIN l := n DIV 2; r := n - 1;
    WHILE l > 0 DO DEC(l); Sift(l, r, a) END;
    WHILE r > 0 DO x := a[0]; a[0] := a[r]; a[r] := x; DEC(r); Sift(l, r, a) END
  END HeapSort;

  PROCEDURE MarkCandidates (n: INTEGER; VAR cand: ARRAY OF ADDRESS);
    VAR i: INTEGER; chnk, end, adr, tag, next, ptr, size: ADDRESS;
  BEGIN
    ASSERT(n > 0);
    chnk := heap; i := 0;
    WHILE chnk # 0 DO
      S.GET(chnk + endOff, end);
      adr := chnk + blkOff;
      WHILE uLT(adr, end) DO
        S.GET(adr, tag);
        IF ODD(tag) THEN (*already marked*)
          S.GET(tag-1, size); INC(adr, size); ptr := adr + SZA;
          WHILE uLT(cand[i], ptr) DO INC(i); IF i = n THEN RETURN END END;
        ELSE
          S.GET(tag, size); ptr := adr + SZA; INC(adr, size);
          WHILE uLT(cand[i], ptr) DO INC(i); IF i = n THEN RETURN END END;
          IF uLT(cand[i], adr) THEN Mark(ptr) END
        END;
        IF uLE(end, cand[i]) THEN (*skip rest of this heap chunk*) adr := end END
      END;
      S.GET(chnk + nextChnkOff, chnk)
    END
  END MarkCandidates;

  PROCEDURE CheckFin;
    VAR n: FinNode; tag: ADDRESS;
  BEGIN
    n := fin;
    WHILE n # NIL DO
      S.GET(n.obj - SZA, tag);
      IF ~ODD(tag) THEN n.marked := FALSE; Mark(n.obj)
      ELSE n.marked := TRUE
      END;
      n := n.next
    END
  END CheckFin;

  PROCEDURE Finalize;
    VAR n, prev: FinNode;
  BEGIN n := fin; prev := NIL;
    WHILE n # NIL DO
      IF ~n.marked THEN
        IF n = fin THEN fin := fin.next ELSE prev.next := n.next END;
        n.finalize(S.VAL(S.PTR, n.obj));
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
      n.finalize(S.VAL(S.PTR, n.obj))
    END
  END FINALL;

  PROCEDURE -ExternMainStackFrame            "extern void *SYSTEM_MainStackFrame;";
  PROCEDURE -SystemMainStackFrame(): ADDRESS "(SYSTEM_ADRINT)SYSTEM_MainStackFrame";

  PROCEDURE MarkStack0* (sp: ADDRESS); (* exported in order to prevent inlining by C optimizers *)
    VAR
      nofcand: INTEGER; inc, p, stack0: ADDRESS;
      align: RECORD ch: SHORTCHAR; p: S.PTR END;
      cand: ARRAY 10000 OF ADDRESS;
  BEGIN
    nofcand := 0; stack0 := SystemMainStackFrame();
    (* check for minimum alignment of pointers *)
    inc := S.ADR(align.p) - S.ADR(align);
    IF uLT(stack0, sp) THEN inc := -inc END;
    WHILE sp # stack0 DO
      S.GET(sp, p);
      IF uLE(heapMin, p) & uLT(p, heapMax) THEN
        IF nofcand = LEN(cand) THEN HeapSort(nofcand, cand); MarkCandidates(nofcand, cand); nofcand := 0 END;
        cand[nofcand] := p; INC(nofcand)
      END;
      INC(sp, inc)
    END;
    IF nofcand > 0 THEN HeapSort(nofcand, cand); MarkCandidates(nofcand, cand) END
  END MarkStack0;

  PROCEDURE MarkStack* (); (* exported in order to prevent inlining by C optimizers *)
    VAR x: ADDRESS;
  BEGIN
    MarkStack0(S.ADR(x))
  END MarkStack;

  (* registers must be stored on the stack before doing garbage collection on the stack. *)
  PROCEDURE- includesetjmp "#include <setjmp.h>";
  PROCEDURE- setjmp "jmp_buf env; setjmp(env);";
  
  PROCEDURE CallMarkStack; (* Pretty portable version: 
    uses jump buffer for saving processor state incl. registers on the stack. *)
  BEGIN
    setjmp(); MarkStack()
  END CallMarkStack;

  PROCEDURE GC* (markStack: BOOLEAN);
    VAR m: Module;
  BEGIN
    IF (lockGC = 0) OR (lockGC = 1) & ~markStack THEN 
      Lock();
      m := S.VAL(Module, modules);
      WHILE m # NIL DO
        IF m.enumPtrs # NIL THEN m.enumPtrs(MarkP) END;
        m := m^.next
      END;
      IF markStack THEN
        (* indirect call for allowing to flush the registers onto the stack *)
        CallMarkStack()
      END;
      CheckFin;
      Scan;
      Finalize;
      Unlock()
    END
  END GC;

  PROCEDURE RegisterFinalizer*(obj: S.PTR; finalize: Finalizer);
    VAR f: FinNode;
  BEGIN NEW(f);
    f.obj := S.VAL(ADDRESS, obj); f.finalize := finalize; f.marked := TRUE;
    f.next := fin; fin := f;
  END RegisterFinalizer;


  PROCEDURE -ExternHeapInit "extern void *Heap__init();";
  PROCEDURE -HeapModuleInit 'Heap__init()';

  PROCEDURE InitHeap*;
  (* InitHeap is called by Platform.init before any module bodies have been
     initialized, to enable NEW, S.NEW *)
  BEGIN
    heap          := 0;
    heapsize      := 0;
    allocated     := 0;
    lockdepth     := 0;
    heapMin       := -1;        (* all bits set = max unsigned value *)
    heapMax       := 0;
    bigBlocks     := 0;
    heapMinExpand := heapSize0;

    heap := NewChunk(heapSize0);
    S.PUT(heap + nextChnkOff, AddressZero);

    firstTry    := TRUE;
    freeList[nofLists] := 1; (* Sentinel, # 0 *)

    FileCount   := 0;
    modules     := NIL;
    fin         := NIL;
    lockGC      := 0;
    interrupted := FALSE;

    HeapModuleInit;
  END InitHeap;

END Heap.
