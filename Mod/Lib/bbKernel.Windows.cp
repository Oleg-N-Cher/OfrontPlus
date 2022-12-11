MODULE bbKernel;
(**
	project	= "BlackBox"
	organization	= "www.oberon.ch"
	contributors	= "Oberon microsystems"
	version	= "System/Rsrc/About"
	copyright	= "System/Rsrc/About"
	license	= "Docu/BB-License"
	changes	= ""
	issues	= ""

**)

	(* green color means COM-specific code *)

	IMPORT S := SYSTEM, WinApi;

	CONST
		strictStackSweep = TRUE;

		nameLen* = 256;

		littleEndian* = TRUE;
		timeResolution* = 1000;	(* ticks per second *)

		processor* = 10;	(* i386 *)

		objType* = "ocf";	(* file types *)
		symType* = "osf";
		docType* = "odc";

		(* loader constants *)
		done* = 0;
		fileNotFound* = 1;
		syntaxError* = 2;
		objNotFound* = 3;
		illegalFPrint* = 4;
		cyclicImport* = 5;
		noMem* = 6;
		commNotFound* = 7;
		commSyntaxError* = 8;
		moduleNotFound* = 9;

		any = 1000000;

		CX = 1;
		SP = 4;	(* register number of stack pointer *)
		FP = 5;	(* register number of frame pointer *)
		ML = 3;	(* register which holds the module list at program start *)

		N = 128 DIV 16;	(* free lists *)

		(* kernel flags in module desc *)
		init = 16; dyn = 17; dll = 24; iptrs = 30;

		(* meta interface consts *)
		mConst = 1; mTyp = 2; mVar = 3; mProc = 4; mField = 5;

		debug = FALSE;


	TYPE
		Name* = ARRAY nameLen OF CHAR;
		Utf8Name* = ARRAY nameLen OF SHORTCHAR;
		Command* = PROCEDURE;

		Module* = POINTER TO RECORD [untagged]
			next-: Module;
			opts-: SET;	(* 0..15: compiler opts, 16..31: kernel flags *)
			refcnt-: INTEGER;	(* <0: module invalidated *)
			compTime-, loadTime-: ARRAY 6 OF SHORTINT;
			ext-: INTEGER;	(* currently not used *)
			term-: Command;	(* terminator *)
			nofimps-, nofptrs-: INTEGER;
			csize-, dsize-, rsize-: INTEGER;
			code-, data-, refs-: INTEGER;
			procBase-, varBase-: INTEGER;	(* meta base addresses *)
			names-: POINTER TO ARRAY [untagged] OF SHORTCHAR;	(* names[0] = 0X *)
			ptrs-: POINTER TO ARRAY [untagged] OF INTEGER;
			imports-: POINTER TO ARRAY [untagged] OF Module;
			export-: Directory;	(* exported objects (name sorted) *)
			name-: Utf8Name
		END;

		Type* = POINTER TO RECORD [untagged]
			(* record: ptr to method n at offset - 4 * (n+1) *)
			size-: INTEGER;	(* record: size, array: #elem, dyn array: 0, proc: sigfp *)
			mod-: Module;
			id-: INTEGER;	(* name idx * 256 + lev * 16 + attr * 4 + form *)
			base-: ARRAY 16 OF Type;	(* signature if form = ProcTyp *)
			fields-: Directory;	(* new fields (declaration order) *)
			ptroffs-: ARRAY any OF INTEGER	(* array of any length *)
		END;

		Object* = POINTER TO ObjDesc;

		ObjDesc* = RECORD [untagged]
			fprint-: INTEGER;
			offs-: INTEGER;	(* pvfprint for record types *)
			id-: INTEGER;	(* name idx * 256 + vis * 16 + mode *)
			struct-: Type	(* id of basic type or pointer to typedesc/signature *)
		END;

		Directory* = POINTER TO RECORD [untagged]
			num-: INTEGER;	(* number of entries *)
			obj-: ARRAY any OF ObjDesc	(* array of any length *)
		END;

		Signature* = POINTER TO RECORD [untagged]
			retStruct-: Type;	(* id of basic type or pointer to typedesc or 0 *)
			num-: INTEGER;	(* number of parameters *)
			par-: ARRAY any OF RECORD [untagged]	(* parameters *)
				id-: INTEGER;	(* name idx * 256 + kind *)
				struct-: Type	(* id of basic type or pointer to typedesc *)
			END
		END;

		Handler* = PROCEDURE;(*

		Reducer* = POINTER TO ABSTRACT RECORD
			next: Reducer
		END;

		Identifier* = ABSTRACT RECORD
			typ*: INTEGER;
			obj-: ANYPTR
		END;

		TrapCleaner* = POINTER TO ABSTRACT RECORD
			next: TrapCleaner
		END;

		TryHandler* = PROCEDURE (a, b, c: INTEGER);


		(* meta extension suport *)

		ItemExt* = POINTER TO ABSTRACT RECORD END;

		ItemAttr* = RECORD
			obj*, vis*, typ*, adr*: INTEGER;
			mod*: Module;
			desc*: Type;
			ptr*: S.PTR;
			ext*: ItemExt
		END;*)

		Hook* = POINTER TO ABSTRACT RECORD END;

		(*LoaderHook* = POINTER TO ABSTRACT RECORD (Hook)
			res*: INTEGER;
			importing*, imported*, object*: ARRAY 256 OF CHAR
		END;

		Block = POINTER TO RECORD [untagged]
			tag: Type;
			last: INTEGER;		(* arrays: last element *)
			actual: INTEGER;	(* arrays: used during mark phase *)
			first: INTEGER		(* arrays: first element *)
		END;

		FreeBlock = POINTER TO FreeDesc;

		FreeDesc = RECORD [untagged]
			tag: Type;		(* f.tag = ADR(f.size) *)
			size: INTEGER;
			next: FreeBlock
		END;

		Cluster = POINTER TO RECORD [untagged]
			size: INTEGER;	(* total size *)
			next: Cluster;
			max: INTEGER	(* exe: reserved size, dll: original address *)
			(* start of first block *)
		END;

		FList = POINTER TO RECORD
			next: FList;
			blk: Block;
			iptr, aiptr: BOOLEAN
		END;

		CList = POINTER TO RECORD
			next: CList;
			do: Command;
			trapped: BOOLEAN
		END;


		PtrType = RECORD v: S.PTR END;	(* used for array of pointer *)
		Char8Type = RECORD v: SHORTCHAR END;
		Char16Type = RECORD v: CHAR END;
		Int8Type = RECORD v: BYTE END;
		Int16Type = RECORD v: SHORTINT END;
		Int32Type = RECORD v: INTEGER END;
		Int64Type = RECORD v: LONGINT END;
		BoolType = RECORD v: BOOLEAN END;
		SetType = RECORD v: SET END;
		Real32Type = RECORD v: SHORTREAL END;
		Real64Type = RECORD v: REAL END;
		ProcType = RECORD v: PROCEDURE END;
		UPtrType = RECORD v: INTEGER END;
		IntPtrType = RECORD p: COM.IUnknown END;	(* used for array of interface pointer *)

		StrPtr = POINTER TO ARRAY [untagged] OF SHORTCHAR;

		IntPtr = POINTER TO RECORD [untagged] p: COM.IUnknown END;

		Interface = POINTER TO RECORD	(* COMPILER DEPENDENT *)
			vtab: INTEGER;
			ref: INTEGER;	(* must correspond to Block.actual *)
			unk: COM.IUnknown
		END;


		(* Exception handling *)

		ExcpFramePtr* = POINTER TO ExcpFrame;
		ExcpFrame* = EXTENSIBLE RECORD [untagged]
			link*: ExcpFramePtr;
			handler*: PROCEDURE(excpRec: WinApi.PtrEXCEPTION_RECORD;
										estFrame: ExcpFramePtr;
										context: WinApi.PtrCONTEXT;
										dispCont: INTEGER): INTEGER;
		END;
		ComExcpFramePtr = POINTER TO RECORD (ExcpFrame)
			par: INTEGER
		END;

		Coroutine* = POINTER TO CoroutineDesc;
		CoroutineDesc = RECORD
			prev, next: POINTER [untagged] TO CoroutineDesc; (* circular list *)
			fiber: WinApi.PtrVoid;
			stackBase, stackTop: INTEGER; (* stackBase = 0 means not yet started *)
			trapStack: TrapCleaner;
			trapChecker: Handler;
			stackChecked: BOOLEAN;
		END;

	*)VAR(*
		baseStack: INTEGER;	(* modList, root, and baseStack must be together for remote debugging *)
		root: Cluster;	(* cluster list *)
		modList-: Module;	(* root of module list *)
		*)trapCount-: INTEGER;(*
		err-, pc-, sp-, fp-, stack-, val-: INTEGER;
		mainWnd*: INTEGER;

		free: ARRAY N OF FreeBlock;	(* free list *)
		sentinelBlock: FreeDesc;
		sentinel: FreeBlock;
		candidates: ARRAY 1024 OF INTEGER;
		nofcand: INTEGER;
		allocated: INTEGER;	(* bytes allocated on BlackBox heap *)
		total: INTEGER;	(* current total size of BlackBox heap *)
		used: INTEGER;	(* bytes allocated on system heap *)
		finalizers: FList;
		hotFinalizers: FList;
		cleaners: CList;
		reducers: Reducer;
		trapStack: TrapCleaner;
		actual: Module;	(* valid during module initialization *)

		res: INTEGER;	(* auxiliary global variables used for trap handling *)
		old: INTEGER;

		trapViewer, trapChecker: Handler;
		trapped, guarded, secondTrap: BOOLEAN;
		interrupted: BOOLEAN;
		static, inDll, dllMem, terminating: BOOLEAN;
		retAd: INTEGER;
		restart: Command;

		heap: WinApi.HANDLE;
		excpPtr: ExcpFramePtr;
		mainThread: WinApi.HANDLE;*)

		told, shift: INTEGER;	(* used in Time() *)

		(*loader: LoaderHook;
		loadres: INTEGER;

		wouldFinalize: BOOLEAN;

		watcher*: PROCEDURE (event: INTEGER);	(* for debugging *)

		currentCoroutine, mainCoroutine: Coroutine;
		mainCoroutineRefcnt: INTEGER;


	(* code procedures for exception handling *)

	PROCEDURE [code] PushFP 055H;
	PROCEDURE [code] PopFP 05DH;
	PROCEDURE [code] PushBX 053H;
	PROCEDURE [code] PopBX 05BH;
	PROCEDURE [code] PushSI 056H;
	PROCEDURE [code] PopSI 05EH;
	PROCEDURE [code] PushDI 057H;
	PROCEDURE [code] PopDI 05FH;
	PROCEDURE [code] LdSP8 08DH, 065H, 0F8H;
	PROCEDURE [code] Return0 (ret: INTEGER) 0C3H;
	PROCEDURE [code] ReturnCX (ret: INTEGER) 05AH, 001H, 0CCH, 0FFH, 0E2H;
		(* POP DX; ADD SP,CX; JP DX *)
	PROCEDURE [code] FPageWord (offs: INTEGER): INTEGER 64H, 8BH, 0H;	(* MOV EAX,FS:[EAX] *)
	PROCEDURE [code] InstallExcp* (VAR e: ExcpFrame) 64H, 8BH, 0DH, 0, 0, 0, 0, 89H, 8, 64H, 0A3H, 0, 0, 0, 0;
	PROCEDURE [code] RemoveExcp* (VAR e: ExcpFrame) 8BH, 0, 64H, 0A3H, 0, 0, 0, 0;

	(* code procedures for fpu *)

	PROCEDURE [code] FINIT 0DBH, 0E3H;
	PROCEDURE [code] FLDCW 0D9H, 06DH, 0FCH;	(* -4, FP *)
	PROCEDURE [code] FSTCW 0D9H, 07DH, 0FCH;	(* -4, FP *)

	(* code procedure for memory erase *)

	PROCEDURE [code] Erase (adr, words: INTEGER)
		089H, 0C7H,	(* MOV EDI, EAX *)
		031H, 0C0H,	(* XOR EAX, EAX *)
		059H,			(* POP ECX *)
		0F2H, 0ABH;	(* REP STOS *)

	(* code procedure for stack allocate *)

	PROCEDURE [code] ALLOC (* argument in CX *)
	(*
		PUSH	EAX
		ADD	ECX,-5
		JNS	L0
		XOR	ECX,ECX
	L0: AND	ECX,-4	(n-8+3)/4*4
		MOV	EAX,ECX
		AND	EAX,4095
		SUB	ESP,EAX
		MOV	EAX,ECX
		SHR	EAX,12
		JEQ	L2
	L1: PUSH	0
		SUB	ESP,4092
		DEC	EAX
		JNE	L1
	L2: ADD	ECX,8
		MOV	EAX,[ESP,ECX,-4]
		PUSH	EAX
		MOV	EAX,[ESP,ECX,-4]
		SHR	ECX,2
		RET
	*);

	(* code procedures for COM support *)

	PROCEDURE [code] ADDREF
	(*
		MOV	ECX,[ESP,4]
		INC	[ECX,4]
		MOV	EAX,[ECX,8]
		OR	EAX,EAX
		JE	L1
		PUSH	EAX
		MOV	EAX,[EAX]
		CALL	[EAX,4]
		MOV	ECX,[ESP,4]
	L1: MOV	EAX,[ECX,4]
		RET	4
	*)
		08BH, 04CH, 024H, 004H,
		0FFH, 041H, 004H,
		08BH, 041H, 008H,
		009H, 0C0H,
		074H, 00AH,
		050H,
		08BH, 000H,
		0FFH, 050H, 004H,
		08BH, 04CH, 024H, 004H,
		08BH, 041H, 004H,
		0C2H, 004H, 000H;

	PROCEDURE [code] RELEASE
	(*
		MOV	ECX,[ESP,4]
		MOV	EAX,[ECX,8]
		OR	EAX,EAX
		JE	L1
		PUSH	EAX
		MOV	EAX,[EAX]
		CALL	[EAX,8]
		MOV	ECX,[ESP,4]
	L1: DEC	[ECX,4]
		MOV	EAX,[ECX,4]
		RET	4
	*)
		08BH, 04CH, 024H, 004H,
		08BH, 041H, 008H,
		009H, 0C0H,
		074H, 00AH,
		050H,
		08BH, 000H,
		0FFH, 050H, 008H,
		08BH, 04CH, 024H, 004H,
		0FFH, 049H, 004H,
		08BH, 041H, 004H,
		0C2H, 004H, 000H;

	PROCEDURE [code] CALLREL
	(*
		MOV	EAX,[ESP,4]
		CMP	[EAX,4],1
		JNE	L1
		PUSH	ESI
		PUSH	EDI
		PUSH	EAX
		MOV	EAX,[EAX,-4]
		CALL	[EAX,-8]
		POP	EDI
		POP	ESI
	L1:
	*)
		08BH, 044H, 024H, 004H,
		083H, 078H, 004H, 001H,
		075H, 00BH,
		056H,
		057H,
		050H,
		08BH, 040H, 0FCH,
		0FFH, 050H, 0F8H,
		05FH,
		05EH;

	PROCEDURE (VAR id: Identifier) Identified* (): BOOLEAN,	NEW, ABSTRACT;
	PROCEDURE (r: Reducer) Reduce* (full: BOOLEAN),	NEW, ABSTRACT;
	PROCEDURE (c: TrapCleaner) Cleanup*,	NEW, EMPTY;


	(* meta extension suport *)

	PROCEDURE (e: ItemExt) Lookup* (name: ARRAY OF CHAR; VAR i: ANYREC), NEW, ABSTRACT;
	PROCEDURE (e: ItemExt) Index* (index: INTEGER; VAR elem: ANYREC), NEW, ABSTRACT;
	PROCEDURE (e: ItemExt) Deref* (VAR ref: ANYREC), NEW, ABSTRACT;

	PROCEDURE (e: ItemExt) Valid* (): BOOLEAN, NEW, ABSTRACT;
	PROCEDURE (e: ItemExt) Size* (): INTEGER, NEW, ABSTRACT;
	PROCEDURE (e: ItemExt) BaseTyp* (): INTEGER, NEW, ABSTRACT;
	PROCEDURE (e: ItemExt) Len* (): INTEGER, NEW, ABSTRACT;

	PROCEDURE (e: ItemExt) Call* (OUT ok: BOOLEAN), NEW, ABSTRACT;
	PROCEDURE (e: ItemExt) BoolVal* (): BOOLEAN, NEW, ABSTRACT;
	PROCEDURE (e: ItemExt) PutBoolVal* (x: BOOLEAN), NEW, ABSTRACT;
	PROCEDURE (e: ItemExt) CharVal* (): CHAR, NEW, ABSTRACT;
	PROCEDURE (e: ItemExt) PutCharVal* (x: CHAR), NEW, ABSTRACT;
	PROCEDURE (e: ItemExt) IntVal* (): INTEGER, NEW, ABSTRACT;
	PROCEDURE (e: ItemExt) PutIntVal* (x: INTEGER), NEW, ABSTRACT;
	PROCEDURE (e: ItemExt) LongVal* (): LONGINT, NEW, ABSTRACT;
	PROCEDURE (e: ItemExt) PutLongVal* (x: LONGINT), NEW, ABSTRACT;
	PROCEDURE (e: ItemExt) RealVal* (): REAL, NEW, ABSTRACT;
	PROCEDURE (e: ItemExt) PutRealVal* (x: REAL), NEW, ABSTRACT;
	PROCEDURE (e: ItemExt) SetVal* (): SET, NEW, ABSTRACT;
	PROCEDURE (e: ItemExt) PutSetVal* (x: SET), NEW, ABSTRACT;
	PROCEDURE (e: ItemExt) PtrVal* (): ANYPTR, NEW, ABSTRACT;
	PROCEDURE (e: ItemExt) PutPtrVal* (x: ANYPTR), NEW, ABSTRACT;
	PROCEDURE (e: ItemExt) GetSStringVal* (OUT x: ARRAY OF SHORTCHAR;
																	OUT ok: BOOLEAN), NEW, ABSTRACT;
	PROCEDURE (e: ItemExt) PutSStringVal* (IN x: ARRAY OF SHORTCHAR;
																	OUT ok: BOOLEAN), NEW, ABSTRACT;
	PROCEDURE (e: ItemExt) GetStringVal* (OUT x: ARRAY OF CHAR; OUT ok: BOOLEAN), NEW, ABSTRACT;
	PROCEDURE (e: ItemExt) PutStringVal* (IN x: ARRAY OF CHAR; OUT ok: BOOLEAN), NEW, ABSTRACT;*)


	(* -------------------- miscellaneous tools -------------------- *)

	PROCEDURE IsAlpha* (ch: CHAR): BOOLEAN;
		VAR type: SHORTINT; res: INTEGER;
	BEGIN
		(* note that WinApi.IsCharAlphaW returns FALSE for all Hiragana and Katakana syllables and is slower *)
		res := WinApi.GetStringTypeW(WinApi.CT_CTYPE1, S.VAL(WinApi.PtrWSTR, S.ADR(ch)), 1, type);
		RETURN BITS(WinApi.C1_ALPHA) * BITS(type) # {}
	END IsAlpha;

	PROCEDURE Upper* (ch: CHAR): CHAR;
		VAR ord: S.ADRINT;
	BEGIN
		IF ("a" <= ch) & (ch <= "z") THEN RETURN CAP(ch) (* common case optimized *)
		ELSIF ch > 7FX THEN
			ord := ORD(ch);
			RETURN CHR(S.VAL(S.ADRINT, WinApi.CharUpperW(S.VAL(WinApi.PtrWSTR, ord))))
		ELSE RETURN ch
		END
	END Upper;

	PROCEDURE IsUpper* (ch: CHAR): BOOLEAN;
	BEGIN
		IF ("A" <= ch) & (ch <= "Z") THEN RETURN TRUE
		ELSIF ch > 7FX THEN RETURN WinApi.IsCharUpperW(ch) # 0
		ELSE RETURN FALSE
		END
	END IsUpper;

	PROCEDURE Lower* (ch: CHAR): CHAR;
		VAR ord: S.ADRINT;
	BEGIN
		IF ("A" <= ch) & (ch <= "Z") THEN RETURN CHR(ORD(ch) + 32)
		ELSIF ch > 7FX THEN
			ord := ORD(ch);
			RETURN CHR(S.VAL(S.ADRINT, WinApi.CharLowerW(S.VAL(WinApi.PtrWSTR, ord))))
		ELSE RETURN ch
		END
	END Lower;

	PROCEDURE IsLower* (ch: CHAR): BOOLEAN;
	BEGIN
		IF ("a" <= ch) & (ch <= "z") THEN RETURN TRUE
		ELSIF ch > 7FX THEN RETURN WinApi.IsCharLowerW(ch) # 0
		ELSE RETURN FALSE
		END
	END IsLower;

	PROCEDURE Utf8ToString* (IN in: ARRAY OF SHORTCHAR; OUT out: ARRAY OF CHAR; 
											OUT res: INTEGER);
		VAR i, j, val, max: INTEGER; ch: SHORTCHAR;
		
		PROCEDURE FormatError();
		BEGIN out := in$; res := 2 (*format error*)
		END FormatError;
		
	BEGIN
		ch := in[0]; i := 1; j := 0; max := LEN(out) - 1;
		WHILE (ch # 0X) & (j < max) DO
			IF ch < 80X THEN
				out[j] := ch; INC(j)
			ELSIF ch < 0E0X THEN
				val := ORD(ch) - 192;
				IF val < 0 THEN FormatError; RETURN END ;
				ch := in[i]; INC(i); val := val * 64 + ORD(ch) - 128;
				IF (ch < 80X) OR (ch >= 0E0X) THEN FormatError; RETURN END ;
				out[j] := CHR(val); INC(j)
			ELSIF ch < 0F0X THEN 
				val := ORD(ch) - 224;
				ch := in[i]; INC(i); val := val * 64 + ORD(ch) - 128;
				IF (ch < 80X) OR (ch >= 0E0X) THEN FormatError; RETURN END ;
				ch := in[i]; INC(i); val := val * 64 + ORD(ch) - 128;
				IF (ch < 80X) OR (ch >= 0E0X) THEN FormatError; RETURN END ;
				out[j] := CHR(val); INC(j)
			ELSE
				FormatError; RETURN
			END ;
			ch := in[i]; INC(i)
		END;
		out[j] := 0X;
		IF ch = 0X THEN res := 0 (*ok*) ELSE res := 1 (*truncated*) END
	END Utf8ToString;

	PROCEDURE StringToUtf8* (IN in: ARRAY OF CHAR; OUT out: ARRAY OF SHORTCHAR; 
											OUT res: INTEGER);
		VAR i, j, val, max: INTEGER;
	BEGIN
		i := 0; j := 0; max := LEN(out) - 3;
		WHILE (in[i] # 0X) & (j < max) DO
			val := ORD(in[i]); INC(i);
			IF val < 128 THEN
				out[j] := SHORT(CHR(val)); INC(j)
			ELSIF val < 2048 THEN
				out[j] := SHORT(CHR(val DIV 64 + 192)); INC(j);
				out[j] := SHORT(CHR(val MOD 64 + 128)); INC(j)
			ELSE
				out[j] := SHORT(CHR(val DIV 4096 + 224)); INC(j); 
				out[j] := SHORT(CHR(val DIV 64 MOD 64 + 128)); INC(j);
				out[j] := SHORT(CHR(val MOD 64 + 128)); INC(j)
			END;
		END;
		out[j] := 0X;
		IF in[i] = 0X THEN res := 0 (*ok*) ELSE res :=  1 (*truncated*) END
	END StringToUtf8;

	PROCEDURE SplitName* (name: ARRAY OF CHAR; VAR head, tail: ARRAY OF CHAR);
		(* portable *)
		VAR i, j: INTEGER; ch, lch: CHAR;
	BEGIN
		i := 0; ch := name[0];
		IF ch # 0X THEN
			REPEAT
				head[i] := ch; lch := ch; INC(i); ch := name[i]
			UNTIL (ch = 0X) OR (ch = ".") OR IsUpper(ch) & ~IsUpper(lch);
			IF ch = "." THEN i := 0; ch := name[0] END;
			head[i] := 0X; j := 0;
			WHILE ch # 0X DO tail[j] := ch; INC(i); INC(j); ch := name[i] END;
			tail[j] := 0X;
			IF tail = "" THEN tail := head$; head := "" END
		ELSE head := ""; tail := ""
		END
	END SplitName;

	PROCEDURE MakeFileName* (VAR name: ARRAY OF CHAR; type: ARRAY OF CHAR);
		VAR i, j: INTEGER; ext: ARRAY 8 OF CHAR; ch: CHAR;
	BEGIN
		i := 0;
		WHILE (name[i] # 0X) & (name[i] # ".") DO INC(i) END;
		IF name[i] = "." THEN
			IF name[i + 1] = 0X THEN name[i] := 0X END
		ELSE
			IF type = "" THEN ext := docType ELSE ext := type$ END;
			IF i < LEN(name) - LEN(ext$) - 1 THEN
				name[i] := "."; INC(i); j := 0; ch := ext[0];
				WHILE ch # 0X DO
					name[i] := Lower(ch); INC(i); INC(j); ch := ext[j]
				END;
				name[i] := 0X
			END
		END
	END MakeFileName;

	PROCEDURE Time* (): LONGINT;
		VAR t: INTEGER;
	BEGIN
		t := WinApi.GetTickCount();
		IF t < told THEN INC(shift) END;
		told := t;
		RETURN shift * 100000000L + t
	END Time;

	PROCEDURE Beep* ();
		VAR res: INTEGER;
	BEGIN
		res := WinApi.MessageBeep(BITS(-1))
	END Beep;(*

	PROCEDURE SearchProcVar* (var: INTEGER; VAR m: Module; VAR adr: INTEGER);
	BEGIN
		adr := var; m := NIL;
		IF var # 0 THEN
			m := modList;
			WHILE (m # NIL) & ((var < m.code) OR (var >= m.code + m.csize)) DO m := m.next END;
			IF m # NIL THEN DEC(adr, m.code) END
		END
	END SearchProcVar;


	(* -------------------- system memory management --------------------- *)

	PROCEDURE GrowHeapMem (size: INTEGER; VAR c: Cluster);
		(* grow to at least size bytes, typically at least 256 kbytes are allocated *)
		CONST N = 262144;
		VAR adr, s: INTEGER;
	BEGIN
		ASSERT(size >= c.size, 100);
		IF size <= c.max THEN
			s := (size + (N - 1)) DIV N * N;
			adr := WinApi.VirtualAlloc(S.VAL(INTEGER, c), s, {12}, {6});	(* commit; exec, read, write *)
			IF adr # 0 THEN
				INC(used, s - c.size); INC(total, s - c.size); c.size := s
			END
		END
		(* post: (c.size unchanged) OR (c.size >= size) *)
	END GrowHeapMem;

	PROCEDURE AllocHeapMem (size: INTEGER; VAR c: Cluster);
		(* allocate at least size bytes, typically at least 256 kbytes are allocated *)
		CONST M = 1536 * 100000H;	(* 1.5 GByte, see note on Stack Size *)
		CONST N = 65536;	(* cluster size for dll *)
		VAR adr, s: INTEGER;
	BEGIN
		IF dllMem THEN
			INC(size, 16);
			ASSERT(size > 0, 100); adr := 0;
			IF size < N THEN adr := WinApi.HeapAlloc(heap, {0}, N) END;
			IF adr = 0 THEN adr := WinApi.HeapAlloc(heap, {0}, size) END;
			IF adr = 0 THEN c := NIL
			ELSE
				c := S.VAL(Cluster, (adr + 15) DIV 16 * 16); c.max := adr;
				c.size := WinApi.HeapSize(heap, {0}, adr) - (S.VAL(INTEGER, c) - adr);
				INC(used, c.size); INC(total, c.size)
			END
		ELSE
			adr := 0; s := M;
			REPEAT
				adr := WinApi.VirtualAlloc(01000000H, s, {13}, {6});	(* reserve; exec, read, write *)
				IF adr = 0 THEN
					adr := WinApi.VirtualAlloc(0, s, {13}, {6})	(* reserve; exec, read, write *)
				END;
				s := s DIV 2
			UNTIL adr # 0;
			IF adr = 0 THEN c := NIL
			ELSE
				adr := WinApi.VirtualAlloc(adr, 1024, {12}, {6});	(* commit; exec, read, write *)
				c := S.VAL(Cluster, adr);
				c.max := s * 2; c.size := 0; c.next := NIL;
				GrowHeapMem(size, c);
				IF c.size < size THEN c := NIL END
			END
		END
		(* post: (c = NIL) OR (c MOD 16 = 0) & (c.size >= size) *)
	END AllocHeapMem;

	PROCEDURE FreeHeapMem (c: Cluster);
		VAR res: INTEGER;
	BEGIN
		DEC(used, c.size); DEC(total, c.size);
		IF dllMem THEN
			res := WinApi.HeapFree(heap, {0}, c.max)
		END
	END FreeHeapMem;

	PROCEDURE HeapFull (size: INTEGER): BOOLEAN;
		VAR ms: WinApi.MEMORYSTATUS;
	BEGIN
		ms.dwLength := SIZE(WinApi.MEMORYSTATUS);
		ms.dwMemoryLoad := -1;
		WinApi.GlobalMemoryStatus(ms);
		RETURN used + size > ms.dwTotalPhys
	END HeapFull;

	PROCEDURE AllocModMem* (descSize, modSize: INTEGER; VAR descAdr, modAdr: INTEGER);
		VAR res: INTEGER;
	BEGIN
		descAdr := WinApi.VirtualAlloc(0, descSize, {12, 13}, {6});	(* reserve & commit; exec, read, write *)
		IF descAdr # 0 THEN
			modAdr := WinApi.VirtualAlloc(0, modSize, {12, 13}, {6});	(* reserve & commit; exec, read, write *)
			IF modAdr # 0 THEN INC(used, descSize + modSize)
			ELSE res := WinApi.VirtualFree(descAdr, 0, {15}); descAdr := 0
			END
		ELSE modAdr := 0
		END
	END AllocModMem;

	PROCEDURE DeallocModMem* (descSize, modSize, descAdr, modAdr: INTEGER);
		VAR res: INTEGER;
	BEGIN
		DEC(used, descSize + modSize);
		res := WinApi.VirtualFree(descAdr, 0, {15});	(* release *)
		res := WinApi.VirtualFree(modAdr, 0, {15})	(* release *)
	END DeallocModMem;

	PROCEDURE InvalModMem (modSize, modAdr: INTEGER);
		VAR res: INTEGER;
	BEGIN
		DEC(used, modSize);
		res := WinApi.VirtualFree(modAdr, modSize, {14})	(* decommit *)
	END InvalModMem;

	PROCEDURE IsReadable* (from, to: INTEGER): BOOLEAN;
		(* check wether memory between from (incl.) and to (excl.) may be read *)
	BEGIN
		RETURN WinApi.IsBadReadPtr(from, to - from) = 0
	END IsReadable;


	(* --------------------- COM reference counting -------------------- *)

	PROCEDURE [noframe] AddRef* (p: INTEGER): INTEGER;	(* COMPILER DEPENDENT *)
	BEGIN
		ADDREF
(*
		INC(p.ref);
		IF p.unk # NIL THEN p.unk.AddRef() END;
		RETURN p.ref
*)
	END AddRef;

	PROCEDURE [noframe] Release* (p: INTEGER): INTEGER;	(* COMPILER DEPENDENT *)
	BEGIN
		RELEASE
(*
		IF p.unk # NIL THEN p.unk.Release() END;
		DEC(p.ref);
		RETURN p.ref
*)
	END Release;

	PROCEDURE [noframe] Release2* (p: INTEGER): INTEGER;	(* COMPILER DEPENDENT *)
	BEGIN
		CALLREL;
		RELEASE
(*
		IF p.ref = 1 THEN p.RELEASE END;
		IF p.unk # NIL THEN p.unk.Release() END;
		DEC(p.ref);
		RETURN p.ref
*)
	END Release2;

	PROCEDURE RecFinalizer (obj: ANYPTR);
		VAR i: INTEGER; type: Type; p: IntPtr;
	BEGIN
		S.GET(S.VAL(INTEGER, obj) - 4, type);
		i := 0;
		WHILE type.ptroffs[i] >= 0 DO INC(i) END;
		INC(i);
		WHILE type.ptroffs[i] >= 0 DO
			p := S.VAL(IntPtr, S.VAL(INTEGER, obj) + type.ptroffs[i]); INC(i);
			p.p := NIL	(* calls p.p.Release *)
		END
	END RecFinalizer;

	PROCEDURE ArrFinalizer (obj: S.PTR);
		VAR last, adr, i, j: INTEGER; type: Type; p: IntPtr;
	BEGIN
		S.GET(S.VAL(INTEGER, obj) - 4, type);
		type := S.VAL(Type, S.VAL(INTEGER, type) - 2);	(* remove array flag *)
		S.GET(S.VAL(INTEGER, obj), last);
		S.GET(S.VAL(INTEGER, obj) + 8, adr);
		j := 0;
		WHILE type.ptroffs[j] >= 0 DO INC(j) END;
		INC(j);
		WHILE adr <= last DO
			i := j;
			WHILE type.ptroffs[i] >= 0 DO
				p := S.VAL(IntPtr, adr + type.ptroffs[i]); INC(i);
				p.p := NIL	(* calls p.p.Release *)
			END;
			INC(adr, type.size)
		END
	END ArrFinalizer;

	PROCEDURE ReleaseIPtrs (mod: Module);
		VAR i: INTEGER; p: IntPtr;
	BEGIN
		IF iptrs IN mod.opts THEN
			EXCL(mod.opts, iptrs);
			i := mod.nofptrs;
			WHILE mod.ptrs[i] # -1 DO
				p := S.VAL(IntPtr, mod.varBase + mod.ptrs[i]); INC(i);
				p.p := NIL	(* calls p.p.Release *)
			END
		END
	END ReleaseIPtrs;


	(* --------------------- NEW implementation (portable) -------------------- *)

	PROCEDURE^ NewBlock (size: INTEGER): Block;

	PROCEDURE NewRec* (typ: INTEGER): INTEGER;	(* implementation of NEW(ptr) *)
		VAR size: INTEGER; b: Block; tag: Type; l: FList;
	BEGIN
		IF ODD(typ) THEN	(* record contains interface pointers *)
			tag := S.VAL(Type, typ - 1);
			b := NewBlock(tag.size);
			IF b = NIL THEN RETURN 0 END;
			b.tag := tag;
			l := S.VAL(FList, S.ADR(b.last));	(* anchor new object! *)
			l := S.VAL(FList, NewRec(S.TYP(FList)));	(* NEW(l) *)
			l.blk := b; l.iptr := TRUE; l.next := finalizers; finalizers := l;
			RETURN S.ADR(b.last)
		ELSE
			tag := S.VAL(Type, typ);
			b := NewBlock(tag.size);
			IF b = NIL THEN RETURN 0 END;
			b.tag := tag; S.GET(typ - 4, size);
			IF size # 0 THEN	(* record uses a finalizer *)
				l := S.VAL(FList, S.ADR(b.last));	(* anchor new object! *)
				l := S.VAL(FList, NewRec(S.TYP(FList)));	(* NEW(l) *)
				l.blk := b; l.next := finalizers; finalizers := l
			END;
			RETURN S.ADR(b.last)
		END
	END NewRec;

	PROCEDURE NewArr* (eltyp, nofelem, nofdim: INTEGER): INTEGER;	(* impl. of NEW(ptr, dim0, dim1, ...) *)
		VAR b: Block; size, headSize: INTEGER; t: Type; fin: BOOLEAN; l: FList;
	BEGIN
		IF (nofdim < 0) OR (nofdim > (MAX(INTEGER) - 12) DIV 4) THEN RETURN 0 END;
		headSize := 4 * nofdim + 12;
		fin := FALSE;
		CASE eltyp OF
		| -1: eltyp := S.ADR(IntPtrType); fin := TRUE
		| 0: eltyp := S.ADR(PtrType)
		| 1: eltyp := S.ADR(Char8Type)
		| 2: eltyp := S.ADR(Int16Type)
		| 3: eltyp := S.ADR(Int8Type)
		| 4: eltyp := S.ADR(Int32Type)
		| 5: eltyp := S.ADR(BoolType)
		| 6: eltyp := S.ADR(SetType)
		| 7: eltyp := S.ADR(Real32Type)
		| 8: eltyp := S.ADR(Real64Type)
		| 9: eltyp := S.ADR(Char16Type)
		| 10: eltyp := S.ADR(Int64Type)
		| 11: eltyp := S.ADR(ProcType)
		| 12: eltyp := S.ADR(UPtrType)
		ELSE	(* eltyp is desc *)
			IF ODD(eltyp) THEN DEC(eltyp); fin := TRUE END
		END;
		t := S.VAL(Type, eltyp);
		ASSERT(t.size > 0, 100);
		IF (nofelem < 0) OR (nofelem > (MAX(INTEGER) - headSize) DIV t.size) THEN RETURN 0 END;
		size := headSize + nofelem * t.size;
		b := NewBlock(size);
		IF b = NIL THEN RETURN 0 END;
		b.tag := S.VAL(Type, eltyp + 2);	(* tag + array mark *)
		b.last := S.ADR(b.last) + size - t.size;	(* pointer to last elem *)
		b.first := S.ADR(b.last) + headSize;	(* pointer to first elem *)
		IF fin THEN
			l := S.VAL(FList, S.ADR(b.last));	(* anchor new object! *)
			l := S.VAL(FList, NewRec(S.TYP(FList)));	(* NEW(l) *)
			l.blk := b; l.aiptr := TRUE; l.next := finalizers; finalizers := l
		END;
		RETURN S.ADR(b.last)
	END NewArr;


	(* -------------------- handler installation (portable) --------------------- *)

	PROCEDURE ThisFinObj* (VAR id: Identifier): ANYPTR;
		VAR l: FList;
	BEGIN
		ASSERT(id.typ # 0, 100);
		l := finalizers;
		WHILE l # NIL DO
			IF S.VAL(INTEGER, l.blk.tag) = id.typ THEN
				id.obj := S.VAL(ANYPTR, S.ADR(l.blk.last));
				IF id.Identified() THEN RETURN id.obj END
			END;
			l := l.next
		END;
		RETURN NIL
	END ThisFinObj;

	PROCEDURE InstallReducer* (r: Reducer);
	BEGIN
		r.next := reducers; reducers := r
	END InstallReducer;

	PROCEDURE InstallTrapViewer* (h: Handler);
	BEGIN
		trapViewer := h
	END InstallTrapViewer;

	PROCEDURE InstallTrapChecker* (h: Handler);
	BEGIN
		trapChecker := h
	END InstallTrapChecker;

	PROCEDURE PushTrapCleaner* (c: TrapCleaner);
		VAR t: TrapCleaner;
	BEGIN
		t := trapStack; WHILE (t # NIL) & (t # c) DO t := t.next END;
		ASSERT(t = NIL, 20);
		c.next := trapStack; trapStack := c
	END PushTrapCleaner;

	PROCEDURE PopTrapCleaner* (c: TrapCleaner);
		VAR t: TrapCleaner;
	BEGIN
		t := NIL;
		WHILE (trapStack # NIL) & (t # c) DO
			t := trapStack; trapStack := trapStack.next
		END
	END PopTrapCleaner;

	PROCEDURE InstallCleaner* (p: Command);
		VAR c: CList;
	BEGIN
		c := S.VAL(CList, NewRec(S.TYP(CList)));	(* NEW(c) *)
		c.do := p; c.trapped := FALSE; c.next := cleaners; cleaners := c
	END InstallCleaner;

	PROCEDURE RemoveCleaner* (p: Command);
		VAR c0, c: CList;
	BEGIN
		c := cleaners; c0 := NIL;
		WHILE (c # NIL) & (c.do # p) DO c0 := c; c := c.next END;
		IF c # NIL THEN
			IF c0 = NIL THEN cleaners := cleaners.next ELSE c0.next := c.next END
		END
	END RemoveCleaner;

	PROCEDURE Cleanup*;
		VAR c, c0: CList;
	BEGIN
		c := cleaners; c0 := NIL;
		WHILE c # NIL DO
			IF ~c.trapped THEN
				c.trapped := TRUE; c.do; c.trapped := FALSE; c0 := c
			ELSE
				IF c0 = NIL THEN cleaners := cleaners.next
				ELSE c0.next := c.next
				END
			END;
			c := c.next
		END
	END Cleanup;

	(* -------------------- meta information (portable) --------------------- *)

	PROCEDURE (h: LoaderHook) ThisMod* (IN name: ARRAY OF CHAR): Module, NEW, ABSTRACT;

	PROCEDURE SetLoaderHook*(h: LoaderHook);
	BEGIN
		loader := h
	END SetLoaderHook;

	PROCEDURE InitModule (mod: Module);	(* initialize linked modules *)
		VAR body: Command;
	BEGIN
		IF ~(dyn IN mod.opts) & (mod.next # NIL) & ~(init IN mod.next.opts) THEN InitModule(mod.next) END;
		IF ~(init IN mod.opts) THEN
			body := S.VAL(Command, mod.code);
			INCL(mod.opts, init);
			actual := mod; body(); actual := NIL
		END
	END InitModule;

	PROCEDURE ThisLoadedMod* (IN name: ARRAY OF CHAR): Module;	(* loaded modules only *)
		VAR m: Module; res: INTEGER; n: Utf8Name;
	BEGIN
		StringToUtf8(name, n, res); ASSERT(res = 0);
		loadres := done;
		m := modList;
		WHILE (m # NIL) & ((m.name # n) OR (m.refcnt < 0)) DO m := m.next END;
		IF (m # NIL) & ~(init IN m.opts) THEN InitModule(m) END;
		IF m = NIL THEN loadres := moduleNotFound END;
		RETURN m
	END ThisLoadedMod;

	PROCEDURE ThisMod* (IN name: ARRAY OF CHAR): Module;
	BEGIN
		IF loader # NIL THEN
			loader.res := done;
			RETURN loader.ThisMod(name)
		ELSE
			RETURN ThisLoadedMod(name)
		END
	END ThisMod;

	PROCEDURE LoadMod* (IN name: ARRAY OF CHAR);
		VAR m: Module;
	BEGIN
		m := ThisMod(name)
	END LoadMod;

	PROCEDURE GetLoaderResult* (OUT res: INTEGER; OUT importing, imported, object: ARRAY OF CHAR);
	BEGIN
		IF loader # NIL THEN
			res := loader.res;
			importing := loader.importing$;
			imported := loader.imported$;
			object := loader.object$
		ELSE
			res := loadres;
			importing := "";
			imported := "";
			object := ""
		END
	END GetLoaderResult;

	PROCEDURE ThisObject* (mod: Module; IN name: ARRAY OF CHAR): Object;
		VAR l, r, m, res: INTEGER; p: StrPtr; n: Utf8Name;
	BEGIN
		StringToUtf8(name, n, res); ASSERT(res = 0);
		l := 0; r := mod.export.num;
		WHILE l < r DO	(* binary search *)
			m := (l + r) DIV 2;
			p := S.VAL(StrPtr, S.ADR(mod.names[mod.export.obj[m].id DIV 256]));
			IF p^ = n THEN RETURN S.VAL(Object, S.ADR(mod.export.obj[m])) END;
			IF p^ < n THEN l := m + 1 ELSE r := m END
		END;
		RETURN NIL
	END ThisObject;

	PROCEDURE ThisDesc* (mod: Module; fprint: INTEGER): Object;
		VAR i, n: INTEGER;
	BEGIN
		i := 0; n := mod.export.num;
		WHILE (i < n) & (mod.export.obj[i].id DIV 256 = 0) DO
			IF mod.export.obj[i].offs = fprint THEN RETURN S.VAL(Object, S.ADR(mod.export.obj[i])) END;
			INC(i)
		END;
		RETURN NIL
	END ThisDesc;

	PROCEDURE ThisField* (rec: Type; IN name: ARRAY OF CHAR): Object;
		VAR n, res: INTEGER; p: StrPtr; obj: Object; m: Module; nn: Utf8Name;
	BEGIN
		StringToUtf8(name, nn, res); ASSERT(res = 0);
		m := rec.mod;
		obj := S.VAL(Object, S.ADR(rec.fields.obj[0])); n := rec.fields.num;
		WHILE n > 0 DO
			p := S.VAL(StrPtr, S.ADR(m.names[obj.id DIV 256]));
			IF p^ = nn THEN RETURN obj END;
			DEC(n); INC(S.VAL(INTEGER, obj), 16)
		END;
		RETURN NIL
	END ThisField;

	PROCEDURE ThisCommand* (mod: Module; IN name: ARRAY OF CHAR): Command;
		VAR x: Object; sig: Signature;
	BEGIN
		x := ThisObject(mod, name);
		IF (x # NIL) & (x.id MOD 16 = mProc) THEN
			sig := S.VAL(Signature, x.struct);
			IF (sig.retStruct = NIL) & (sig.num = 0) THEN RETURN S.VAL(Command, mod.procBase + x.offs) END
		END;
		RETURN NIL
	END ThisCommand;

	PROCEDURE ThisType* (mod: Module; IN name: ARRAY OF CHAR): Type;
		VAR x: Object;
	BEGIN
		x := ThisObject(mod, name);
		IF (x # NIL) & (x.id MOD 16 = mTyp) & (S.VAL(INTEGER, x.struct) DIV 256 # 0) THEN
			RETURN x.struct
		ELSE
			RETURN NIL
		END
	END ThisType;*)

	PROCEDURE TypeOf* (IN rec: ANYREC): Type;
	BEGIN
		RETURN S.VAL(Type, S.TYP(rec))
	END TypeOf;

	(*PROCEDURE LevelOf* (t: Type): SHORTINT;
	BEGIN
		RETURN SHORT(t.id DIV 16 MOD 16)
	END LevelOf;

	PROCEDURE NewObj* (VAR o: S.PTR; t: Type);
		VAR i: INTEGER;
	BEGIN
		IF t.size = -1 THEN o := NIL
		ELSE
			i := 0; WHILE t.ptroffs[i] >= 0 DO INC(i) END;
			IF t.ptroffs[i+1] >= 0 THEN INC(S.VAL(INTEGER, t)) END;	(* with interface pointers *)
			o := S.VAL(S.PTR, NewRec(S.VAL(INTEGER, t)))	(* generic NEW *)
		END
	END NewObj;

	PROCEDURE GetModName* (mod: Module; OUT name: Name);
		VAR res: INTEGER;
	BEGIN
		Utf8ToString(mod.name, name, res); ASSERT(res = 0)
	END GetModName;

	PROCEDURE GetObjName* (mod: Module; obj: Object; OUT name: Name);
		VAR p: StrPtr; res: INTEGER;
	BEGIN
		p := S.VAL(StrPtr, S.ADR(mod.names[obj.id DIV 256]));
		Utf8ToString(p^$, name, res); ASSERT(res = 0)
	END GetObjName;

	PROCEDURE GetTypeName* (t: Type; OUT name: Name);
		VAR p: StrPtr; res: INTEGER;
	BEGIN
		p := S.VAL(StrPtr, S.ADR(t.mod.names[t.id DIV 256]));
		Utf8ToString(p^$, name, res); ASSERT(res = 0)
	END GetTypeName;

	PROCEDURE RegisterMod* (mod: Module);
		VAR i: INTEGER; t: WinApi.SYSTEMTIME;
	BEGIN
		mod.next := modList; modList := mod; mod.refcnt := 0; INCL(mod.opts, dyn); i := 0;
		WHILE i < mod.nofimps DO
			IF mod.imports[i] # NIL THEN INC(mod.imports[i].refcnt) END;
			INC(i)
		END;
		WinApi.GetLocalTime(t);
		mod.loadTime[0] := t.wYear;
		mod.loadTime[1] := t.wMonth;
		mod.loadTime[2] := t.wDay;
		mod.loadTime[3] := t.wHour;
		mod.loadTime[4] := t.wMinute;
		mod.loadTime[5] := t.wSecond;
		IF ~(init IN mod.opts) THEN InitModule(mod) END
	END RegisterMod;

	PROCEDURE^ Collect*;

	PROCEDURE UnloadMod* (mod: Module);
		VAR i: INTEGER; t: Command;
	BEGIN
		IF mod.refcnt = 0 THEN
			t := mod.term; mod.term := NIL;
			IF t # NIL THEN t();	(* terminate module *)
				IF mod.refcnt # 0 THEN RETURN END
			END;
			i := 0;
			WHILE i < mod.nofptrs DO	(* release global pointers *)
				S.PUT(mod.varBase + mod.ptrs[i], 0); INC(i)
			END;
			ReleaseIPtrs(mod);	(* release global interface pointers *)
			Collect;	(* call finalizers *)
			i := 0;
			WHILE i < mod.nofimps DO	(* release imported modules *)
				IF mod.imports[i] # NIL THEN DEC(mod.imports[i].refcnt) END;
				INC(i)
			END;
			mod.refcnt := -1;
			IF dyn IN mod.opts THEN	(* release memory *)
				InvalModMem(mod.data + mod.dsize - mod.refs, mod.refs)
			END
		END
	END UnloadMod;

	(* -------------------- dynamic procedure call --------------------- *)	(* COMPILER DEPENDENT *)

	PROCEDURE [code] PUSH (p: INTEGER) 050H;	(* push AX *)
	PROCEDURE [code] CALL (a: INTEGER) 0FFH, 0D0H;	(* call AX *)
	PROCEDURE [code] RETI (): LONGINT;
	PROCEDURE [code] RETR (): REAL;

	(*
		type				par
		32 bit scalar	value
		64 bit scalar	low hi
		var scalar		address
		record			address tag
		array			address size
		open array	address length_N-1 .. length_0
	*)
	PROCEDURE Call* (adr: INTEGER; sig: Signature; IN par: ARRAY OF INTEGER; n: INTEGER): LONGINT;
		VAR p, kind, sp, size: INTEGER; typ: Type; r: REAL;
	BEGIN
		p := sig.num;
		WHILE p > 0 DO	(* push parameters from right to left *)
			DEC(p);
			typ := sig.par[p].struct;
			kind := sig.par[p].id MOD 16;
			IF (S.VAL(INTEGER, typ) DIV 256 = 0) OR (typ.id MOD 4 IN {0, 3}) THEN	(* scalar, ANYREC *)
				IF (kind = 10) & ((S.VAL(INTEGER, typ) = 8) OR (S.VAL(INTEGER, typ) = 10)) THEN	(* 64 bit *)
					DEC(n); PUSH(par[n])	(* push hi word *)
				ELSIF S.VAL(INTEGER, typ) = 11 THEN   (* ANYREC *)
					ASSERT(kind # 10); (* not a value par, also checked by compiler *)
					DEC(n); PUSH(par[n])   (* push tag *)
				END;
				DEC(n); PUSH(par[n])	(* push value/address *)
			ELSIF typ.id MOD 4 = 1 THEN	(* record *)
				IF kind # 10 THEN	(* var par *)
					DEC(n); PUSH(par[n]);	(* push tag *)
					DEC(n); PUSH(par[n])	(* push address *)
				ELSE
					DEC(n, 2);	(* skip tag *)
					S.GETREG(SP, sp); sp := (sp - typ.size) DIV 4 * 4; S.PUTREG(SP, sp);	(* allocate space *)
					S.MOVE(par[n], sp, typ.size)	(* copy to stack *)
				END
			ELSIF typ.size = 0 THEN	(* open array *)
				size := typ.id DIV 16 MOD 16;	(* number of open dimensions *)
				WHILE size > 0 DO
					DEC(size); DEC(n); PUSH(par[n])	(* push length *)
				END;
				DEC(n); PUSH(par[n])	(* push address *)
			ELSE	(* fix array *)
				IF kind # 10 THEN	(* var par *)
					DEC(n, 2); PUSH(par[n])	(* push address *)
				ELSE
					DEC(n); size := par[n]; DEC(n);
					S.GETREG(SP, sp); sp := (sp - size) DIV 4 * 4; S.PUTREG(SP, sp);	(* allocate space *)
					S.MOVE(par[n], sp, size)	(* copy to stack *)
				END
			END
		END;
		ASSERT(n = 0);
		IF S.VAL(INTEGER, sig.retStruct) = 7 THEN	(* shortreal *)
			CALL(adr);
			RETURN S.VAL(INTEGER, SHORT(RETR()))	(* return value in fpu register *)
		ELSIF S.VAL(INTEGER, sig.retStruct) = 8 THEN	(* real *)
			CALL(adr); r := RETR();
			RETURN S.VAL(LONGINT, r)	(* return value in fpu register *)
		ELSE
			CALL(adr);
			RETURN RETI()	(* return value in integer registers *)
		END
	END Call;

	(* -------------------- reference information (portable) --------------------- *)

	PROCEDURE RefCh (VAR ref: INTEGER; OUT ch: SHORTCHAR);
	BEGIN
		S.GET(ref, ch); INC(ref)
	END RefCh;

	PROCEDURE RefNum (VAR ref: INTEGER; OUT x: INTEGER);
		VAR s, n: INTEGER; ch: SHORTCHAR;
	BEGIN
		s := 0; n := 0; RefCh(ref, ch);
		WHILE ORD(ch) >= 128 DO INC(n, ASH(ORD(ch) - 128, s) ); INC(s, 7); RefCh(ref, ch) END;
		x := n + ASH(ORD(ch) MOD 64 - ORD(ch) DIV 64 * 64, s)
	END RefNum;

	PROCEDURE RefName (VAR ref: INTEGER; OUT n: Utf8Name);
		VAR i: INTEGER; ch: SHORTCHAR;
	BEGIN
		i := 0; RefCh(ref, ch);
		WHILE ch # 0X DO n[i] := ch; INC(i); RefCh(ref, ch) END;
		n[i] := 0X
	END RefName;

	PROCEDURE GetRefProc* (VAR ref: INTEGER; OUT adr: INTEGER; OUT name: Utf8Name);
		VAR ch: SHORTCHAR;
	BEGIN
		S.GET(ref, ch);
		WHILE ch >= 0FDX DO	(* skip variables *)
			INC(ref); RefCh(ref, ch);
			IF ch = 10X THEN INC(ref, 4) END;
			RefNum(ref, adr); RefName(ref, name); S.GET(ref, ch)
		END;
		WHILE (ch > 0X) & (ch < 0FCX) DO	(* skip source refs *)
			INC(ref); RefNum(ref, adr); S.GET(ref, ch)
		END;
		IF ch = 0FCX THEN INC(ref); RefNum(ref, adr); RefName(ref, name)
		ELSE adr := 0
		END
	END GetRefProc;

	PROCEDURE GetRefVar* (VAR ref: INTEGER; OUT mode, form: SHORTCHAR; OUT desc: Type;
																OUT adr: INTEGER; OUT name: Utf8Name);
	BEGIN
		S.GET(ref, mode); desc := NIL;
		IF mode >= 0FDX THEN
			mode := SHORT(CHR(ORD(mode) - 0FCH));
			INC(ref); RefCh(ref, form);
			IF form = 10X THEN
				S.GET(ref, desc); INC(ref, 4); form := SHORT(CHR(16 + desc.id MOD 4))
			END;
			RefNum(ref, adr); RefName(ref, name)
		ELSE
			mode := 0X; form := 0X; adr := 0
		END
	END GetRefVar;

	PROCEDURE SourcePos* (mod: Module; codePos: INTEGER): INTEGER;
		VAR ref, pos, ad, d: INTEGER; ch: SHORTCHAR; name: Utf8Name;
	BEGIN
		IF mod # NIL THEN	(* mf, 12.02.04 *)
			ref := mod.refs; pos := 0; ad := 0; S.GET(ref, ch);
			WHILE ch # 0X DO
				WHILE (ch > 0X) & (ch < 0FCX) DO	(* srcref: {dAdr,dPos} *)
					INC(ad, ORD(ch)); INC(ref); RefNum(ref, d);
					IF ad > codePos THEN RETURN pos END;
					INC(pos, d); S.GET(ref, ch)
				END;
				IF ch = 0FCX THEN	(* proc: 0FCX,Adr,Name *)
					INC(ref); RefNum(ref, d); RefName(ref, name); S.GET(ref, ch);
					IF (d > codePos) & (pos > 0) THEN RETURN pos END 
				END;
				WHILE ch >= 0FDX DO	(* skip variables: Mode, Form, adr, Name *)
					INC(ref); RefCh(ref, ch);
					IF ch = 10X THEN INC(ref, 4) END;
					RefNum(ref, d); RefName(ref, name); S.GET(ref, ch)
				END
			END;
		END;
		RETURN -1
	END SourcePos;

	(* -------------------- dynamic link libraries --------------------- *)

	PROCEDURE LoadDll* (IN name: ARRAY OF CHAR; VAR ok: BOOLEAN);
		VAR h: WinApi.HANDLE;
	BEGIN
		ok := FALSE;
		h := WinApi.LoadLibraryW(name);
		IF h # 0 THEN ok := TRUE END
	END LoadDll;

	PROCEDURE ThisDllObj* (mode, fprint: INTEGER; IN dll, name: ARRAY OF CHAR): INTEGER;
		VAR ad: WinApi.FARPROC; h: WinApi.HANDLE; res: INTEGER; sname: Utf8Name;
	BEGIN
		ad := NIL;
		IF mode = mProc THEN
			h := WinApi.GetModuleHandleW(dll);
			IF h # 0 THEN StringToUtf8(name, sname, res); ASSERT(res = 0);
				ad := WinApi.GetProcAddress(h, sname)
			END
		END;
		RETURN S.VAL(INTEGER, ad)
	END ThisDllObj;

	(* -------------------- garbage collector (portable) --------------------- *)

	PROCEDURE Mark (this: Block);
		VAR father, son: Block; tag: Type; flag, offset, actual: INTEGER;
	BEGIN
		IF ~ODD(S.VAL(INTEGER, this.tag)) THEN
			father := NIL;
			LOOP
				INC(S.VAL(INTEGER, this.tag));
				flag := S.VAL(INTEGER, this.tag) MOD 4;
				tag := S.VAL(Type, S.VAL(INTEGER, this.tag) - flag);
				IF flag >= 2 THEN actual := this.first; this.actual := actual
				ELSE actual := S.ADR(this.last)
				END;
				LOOP
					offset := tag.ptroffs[0];
					IF offset < 0 THEN
						INC(S.VAL(INTEGER, tag), offset + 4);	(* restore tag *)
						IF (flag >= 2) & (actual < this.last) & (offset < -4) THEN	(* next array element *)
							INC(actual, tag.size); this.actual := actual
						ELSE	(* up *)
							this.tag := S.VAL(Type, S.VAL(INTEGER, tag) + flag);
							IF father = NIL THEN RETURN END;
							son := this; this := father;
							flag := S.VAL(INTEGER, this.tag) MOD 4;
							tag := S.VAL(Type, S.VAL(INTEGER, this.tag) - flag);
							offset := tag.ptroffs[0];
							IF flag >= 2 THEN actual := this.actual ELSE actual := S.ADR(this.last) END;
							S.GET(actual + offset, father); S.PUT(actual + offset, S.ADR(son.last));
							INC(S.VAL(INTEGER, tag), 4)
						END
					ELSE
						S.GET(actual + offset, son);
						IF son # NIL THEN
							DEC(S.VAL(INTEGER, son), 4);
							IF ~ODD(S.VAL(INTEGER, son.tag)) THEN	(* down *)
								this.tag := S.VAL(Type, S.VAL(INTEGER, tag) + flag);
								S.PUT(actual + offset, father); father := this; this := son;
								EXIT
							END
						END;
						INC(S.VAL(INTEGER, tag), 4)
					END
				END
			END
		END
	END Mark;

	PROCEDURE MarkGlobals;
		VAR m: Module; i, p: INTEGER;
	BEGIN
		m := modList;
		WHILE m # NIL DO
			IF m.refcnt >= 0 THEN
				i := 0;
				WHILE i < m.nofptrs DO
					S.GET(m.varBase + m.ptrs[i], p); INC(i);
					IF p # 0 THEN Mark(S.VAL(Block, p - 4)) END
				END
			END;
			m := m.next
		END
	END MarkGlobals;

(* This is the specification for the code procedure following below:

	PROCEDURE Next (b: Block): Block;	(* next block in same cluster *)
		VAR size: INTEGER;
	BEGIN
		S.GET(S.VAL(INTEGER, b.tag) DIV 4 * 4, size);
		IF ODD(S.VAL(INTEGER, b.tag) DIV 2) THEN INC(size, b.last - S.ADR(b.last)) END;
		RETURN S.VAL(Block, S.VAL(INTEGER, b) + (size + 19) DIV 16 * 16)
	END Next;

*)
	PROCEDURE [code] Next (b: Block): Block	(* next block in same cluster *)
	(*
		MOV	ECX,[EAX]	b.tag
		AND	CL,0FCH	b.tag DIV * 4
		MOV	ECX,[ECX]	size
		TESTB	[EAX],02H	ODD(b.tag DIV 2)
		JE	L1
		ADD	ECX,[EAX,4]	size + b.last
		SUB	ECX,EAX
		SUB	ECX,4	size + b.last - ADR(b.last)
		L1:
		ADD	ECX,19	size + 19
		AND	CL,0F0H	(size + 19) DIV 16 * 16
		ADD	EAX,ECX	b + size
	*)
	08BH, 008H,
	080H, 0E1H, 0FCH,
	08BH, 009H,
	0F6H, 000H, 002H,
	074H, 008H,
	003H, 048H, 004H,
	029H, 0C1H,
	083H, 0E9H, 004H,
	083H, 0C1H, 013H,
	080H, 0E1H, 0F0H,
	001H, 0C8H;

	PROCEDURE CheckCandidates;
	(* pre: nofcand > 0 *)
		VAR i, j, h, p, end: INTEGER; c: Cluster; blk, next: Block;
	BEGIN
		(* sort candidates (shellsort) *)
		h := 1; REPEAT h := h*3 + 1 UNTIL h > nofcand;
		REPEAT h := h DIV 3; i := h;
			WHILE i < nofcand DO p := candidates[i]; j := i;
				WHILE (j >= h) & (candidates[j-h] > p) DO
					candidates[j] := candidates[j-h]; j := j-h
				END;
				candidates[j] := p; INC(i)
			END
		UNTIL h = 1;
		(* sweep *)
		c := root; i := 0;
		WHILE c # NIL DO
			blk := S.VAL(Block, S.VAL(INTEGER, c) + 12);
			end := S.VAL(INTEGER, blk) + (c.size - 12) DIV 16 * 16;
			WHILE candidates[i] < S.VAL(INTEGER, blk) DO
				INC(i);
				IF i = nofcand THEN RETURN END
			END;
			WHILE S.VAL(INTEGER, blk) < end DO
				next := Next(blk);
				IF candidates[i] < S.VAL(INTEGER, next) THEN
					IF (S.VAL(INTEGER, blk.tag) # S.ADR(blk.last))	(* not a free block *)
							& (~strictStackSweep OR (candidates[i] = S.ADR(blk.last))) THEN
						Mark(blk)
					END;
					REPEAT
						INC(i);
						IF i = nofcand THEN RETURN END
					UNTIL candidates[i] >= S.VAL(INTEGER, next)
				END;
				IF (S.VAL(INTEGER, blk.tag) MOD 4 = 0) & (S.VAL(INTEGER, blk.tag) # S.ADR(blk.last))
						& (blk.tag.base[0] = NIL) & (blk.actual > 0) THEN	(* referenced interface record *)
					Mark(blk)
				END;
				blk := next
			END;
			c := c.next
		END
	END CheckCandidates;

	(* coroutine stacks are only checked if they are anchored in globals. If all coroutine stacks were checked,
	unreferenced coroutines would survive forever because the stack contains a reference to the coroutine.
	mainCoroutine and currentCoroutine are always anchored globally. *)
	PROCEDURE MarkReferencedCoroutines(min, max: INTEGER);
		VAR c, stop: Coroutine; sp, sb, tag, p: INTEGER;
	BEGIN
		candidates[0] := max; nofcand := 1;	(* ensure complete scan for interface mark*)
		c := mainCoroutine;
		REPEAT c.stackChecked := FALSE; c := c.next UNTIL c = mainCoroutine;
		REPEAT
			c := mainCoroutine; stop := c;
			REPEAT (* tries to minimize the number of CheckCandidates calls *)
				sb := c.stackBase;
				S.GET(S.VAL(INTEGER, c) - SIZE(ANYPTR), tag);
				IF ODD(tag) & (~c.stackChecked) & (sb # 0) THEN
					c.stackChecked := TRUE;
					sp := c.stackTop;
					WHILE sp < sb DO
						S.GET(sp, p);
						IF (p > min) & (p < max) & (~strictStackSweep OR (p MOD 16 = 0)) THEN
							candidates[nofcand] := p; INC(nofcand);
							IF nofcand = LEN(candidates) - 1 THEN CheckCandidates; nofcand := 0; stop := c END
						END;
						INC(sp, 4)
					END
				END;
				c := c.next
			UNTIL c = stop;
			IF nofcand > 0 THEN CheckCandidates; nofcand := 0; c := NIL END;
		UNTIL c = mainCoroutine
	END MarkReferencedCoroutines;
	
	PROCEDURE MarkLocals;
		VAR sp, p, min, max: INTEGER; c: Cluster;
	BEGIN
		S.GETREG(FP, sp); nofcand := 0; c := root;
		WHILE c.next # NIL DO c := c.next END;
		min := S.VAL(INTEGER, root); max := S.VAL(INTEGER, c) + c.size;
		IF currentCoroutine = NIL THEN (* classical single stack *)
			WHILE sp < baseStack DO
				S.GET(sp, p);
				IF (p > min) & (p < max) & (~strictStackSweep OR (p MOD 16 = 0)) THEN
					candidates[nofcand] := p; INC(nofcand);
					IF nofcand = LEN(candidates) - 1 THEN CheckCandidates; nofcand := 0 END
				END;
				INC(sp, 4)
			END;
			candidates[nofcand] := max; INC(nofcand);	(* ensure complete scan for interface mark*)
			IF nofcand > 0 THEN CheckCandidates END
		ELSE (* with coroutine support *)
			currentCoroutine.stackTop := sp;
			MarkReferencedCoroutines(min, max)
		END
	END MarkLocals;

	PROCEDURE MarkFinObj;
		VAR f: FList;
	BEGIN
		wouldFinalize := FALSE;
		f := finalizers;
		WHILE f # NIL DO
			IF ~ODD(S.VAL(INTEGER, f.blk.tag)) THEN wouldFinalize := TRUE END;
			Mark(f.blk);
			f := f.next
		END;
		f := hotFinalizers;
		WHILE f # NIL DO IF ~ODD(S.VAL(INTEGER, f.blk.tag)) THEN wouldFinalize := TRUE END;
			Mark(f.blk);
			f := f.next
		END
	END MarkFinObj;

	PROCEDURE CheckFinalizers;
		VAR f, g, h, k: FList;
	BEGIN
		f := finalizers; g := NIL;
		IF hotFinalizers = NIL THEN k := NIL
		ELSE
			k := hotFinalizers;
			WHILE k.next # NIL DO k := k.next END
		END;
		WHILE f # NIL DO
			h := f; f := f.next;
			IF ~ODD(S.VAL(INTEGER, h.blk.tag)) THEN
				IF g = NIL THEN finalizers := f ELSE g.next := f END;
				IF k = NIL THEN hotFinalizers := h ELSE k.next := h END;
				k := h; h.next := NIL
			ELSE g := h
			END
		END;
		h := hotFinalizers;
		WHILE h # NIL DO Mark(h.blk); h := h.next END
	END CheckFinalizers;

	PROCEDURE ExecFinalizer (a, b, c: INTEGER);
		VAR f: FList; fin: PROCEDURE(this: ANYPTR);
	BEGIN
		f := S.VAL(FList, a);
		IF f.aiptr THEN ArrFinalizer(S.VAL(ANYPTR, S.ADR(f.blk.last)))
		ELSE
			S.GET(S.VAL(INTEGER, f.blk.tag) - 4, fin);	(* method 0 *)
			IF (fin # NIL) & (f.blk.tag.mod.refcnt >= 0) THEN fin(S.VAL(ANYPTR, S.ADR(f.blk.last))) END;
			IF f.iptr THEN RecFinalizer(S.VAL(ANYPTR, S.ADR(f.blk.last))) END
		END
	END ExecFinalizer;

	PROCEDURE^ Try* (h: TryHandler; a, b, c: INTEGER);	(* COMPILER DEPENDENT *)

	PROCEDURE CallFinalizers;
		VAR f: FList;
	BEGIN
		WHILE hotFinalizers # NIL DO
			f := hotFinalizers; hotFinalizers := hotFinalizers.next;
			Try(ExecFinalizer, S.VAL(INTEGER, f), 0, 0)
		END;
		wouldFinalize := FALSE
	END CallFinalizers;

	PROCEDURE Insert (blk: FreeBlock; size: INTEGER);	(* insert block in free list *)
		VAR i: INTEGER;
	BEGIN
		blk.size := size - 4; blk.tag := S.VAL(Type, S.ADR(blk.size));
		i := MIN(N - 1, (blk.size DIV 16));
		blk.next := free[i]; free[i] := blk
	END Insert;

	PROCEDURE Sweep (dealloc: BOOLEAN);
		VAR cluster, last, c: Cluster; blk, next: Block; fblk, b, t: FreeBlock; end, i: INTEGER;
	BEGIN
		cluster := root; last := NIL; allocated := 0;
		i := N;
		REPEAT DEC(i); free[i] := sentinel UNTIL i = 0;
		WHILE cluster # NIL DO
			blk := S.VAL(Block, S.VAL(INTEGER, cluster) + 12);
			end := S.VAL(INTEGER, blk) + (cluster.size - 12) DIV 16 * 16;
			fblk := NIL;
			WHILE S.VAL(INTEGER, blk) < end DO
				next := Next(blk);
				IF ODD(S.VAL(INTEGER, blk.tag)) THEN
					IF fblk # NIL THEN
						Insert(fblk, S.VAL(INTEGER, blk) - S.VAL(INTEGER, fblk));
						fblk := NIL
					END;
					DEC(S.VAL(INTEGER, blk.tag));	(* unmark *)
					INC(allocated, S.VAL(INTEGER, next) - S.VAL(INTEGER, blk))
				ELSIF fblk = NIL THEN
					fblk := S.VAL(FreeBlock, blk)
				END;
				blk := next
			END;
			IF dealloc & dllMem
					& (S.VAL(INTEGER, fblk) = S.VAL(INTEGER, cluster) + 12) THEN	(* deallocate cluster *)
				c := cluster; cluster := cluster.next;
				IF last = NIL THEN root := cluster ELSE last.next := cluster END;
				FreeHeapMem(c)
			ELSE
				IF fblk # NIL THEN Insert(fblk, end - S.VAL(INTEGER, fblk)) END;
				last := cluster; cluster := cluster.next
			END
		END;
		(* reverse free list *)
		i := N;
		REPEAT
			DEC(i);
			b := free[i]; fblk := sentinel;
			WHILE b # sentinel DO t := b; b := t.next; t.next := fblk; fblk := t END;
			free[i] := fblk
		UNTIL i = 0
	END Sweep;

	PROCEDURE Collect*;
	BEGIN
		IF root # NIL THEN
			CallFinalizers;	(* trap cleanup *)
			IF debug & (watcher # NIL) THEN watcher(1) END;
			MarkGlobals;
			MarkLocals;
			CheckFinalizers;
			Sweep(TRUE);
			CallFinalizers
		END
	END Collect;

	PROCEDURE FastCollect*;
	BEGIN
		IF root # NIL THEN
			IF debug & (watcher # NIL) THEN watcher(2) END;
			MarkGlobals;
			MarkLocals;
			MarkFinObj;
			Sweep(FALSE)
		END
	END FastCollect;

	PROCEDURE WouldFinalize* (): BOOLEAN;
	BEGIN
		RETURN wouldFinalize
	END WouldFinalize;

	(* --------------------- memory allocation (portable) -------------------- *)

	PROCEDURE OldBlock (size: INTEGER): FreeBlock;	(* size MOD 16 = 0 *)
		VAR b, l: FreeBlock; s, i: INTEGER;
	BEGIN
		IF debug & (watcher # NIL) THEN watcher(3) END;
		s := size - 4;
		i := MIN(N - 1, s DIV 16);
		WHILE (i # N - 1) & (free[i] = sentinel) DO INC(i) END;
		b := free[i]; l := NIL;
		WHILE b.size < s DO l := b; b := b.next END;
		IF b # sentinel THEN
			IF l = NIL THEN free[i] := b.next ELSE l.next := b.next END
		ELSE b := NIL
		END;
		RETURN b
	END OldBlock;

	PROCEDURE LastBlock (limit: INTEGER): FreeBlock;	(* size MOD 16 = 0 *)
		VAR b, l: FreeBlock; s, i: INTEGER;
	BEGIN
		s := limit - 4;
		i := 0;
		REPEAT
			b := free[i]; l := NIL;
			WHILE (b # sentinel) & (S.VAL(INTEGER, b) + b.size # s) DO l := b; b := b.next END;
			IF b # sentinel THEN
				IF l = NIL THEN free[i] := b.next ELSE l.next := b.next END
			ELSE b := NIL
			END;
			INC(i)
		UNTIL (b # NIL) OR (i = N);
		RETURN b
	END LastBlock;

	PROCEDURE NewBlock (size: INTEGER): Block;
		VAR tsize, a, s: INTEGER; b: FreeBlock; new, c: Cluster; r: Reducer;
	BEGIN
		ASSERT(size >= 0, 20);
		IF size > MAX(INTEGER) - 19 THEN RETURN NIL END;
		tsize := (size + 19) DIV 16 * 16;
		b := OldBlock(tsize);	(* 1) search for free block *)
		IF b = NIL THEN
			IF dllMem THEN
				FastCollect; b := OldBlock(tsize);	(* 2) collect *)
				IF b = NIL THEN
					Collect; b := OldBlock(tsize);	(* 2a) fully collect *)
				END;
				IF b = NIL THEN
					AllocHeapMem(tsize + 12, new);	(* 3) allocate new cluster *)
					IF new # NIL THEN
						IF (root = NIL) OR (S.VAL(INTEGER, new) < S.VAL(INTEGER, root)) THEN
							new.next := root; root := new
						ELSE
							c := root;
							WHILE (c.next # NIL) & (S.VAL(INTEGER, new) > S.VAL(INTEGER, c.next)) DO c := c.next END;
							new.next := c.next; c.next := new
						END;
						b := S.VAL(FreeBlock, S.VAL(INTEGER, new) + 12);
						b.size := (new.size - 12) DIV 16 * 16 - 4
					ELSE
						RETURN NIL	(* 4) give up *)
					END
				END
			ELSE
				FastCollect; b := OldBlock(tsize);	(* 2) collect *)
				IF b = NIL THEN
					Collect; b := OldBlock(tsize);	(* 2a) fully collect *)
				END;
				IF (b = NIL) & HeapFull(tsize) & (reducers # NIL) THEN	(* 3) little space => reduce once *)
					r := reducers; reducers := NIL;
					WHILE r # NIL DO r.Reduce(FALSE); r := r.next END;
					Collect
				END;
				IF b = NIL THEN
					IF tsize <= MAX(INTEGER) DIV 3 * 2 - allocated THEN
						s := (tsize + allocated) DIV 2 * 3
					ELSIF tsize <= root.max - allocated THEN
						s := root.max
					ELSE
						RETURN NIL
					END;
					a := 12 + (root.size - 12) DIV 16 * 16;
					IF s <= total THEN
						b := OldBlock(tsize);
						IF b = NIL THEN s := a + tsize END
					ELSIF s < a + tsize THEN
						s := a + tsize
					END;
					IF (b = NIL) & (total < s) THEN	(* 4) enlarge heap *)
						GrowHeapMem(s, root);
						IF root.size >= s THEN
							b := LastBlock(S.VAL(INTEGER, root) + a);
							IF b # NIL THEN
								b.size := (root.size - a + b.size + 4) DIV 16 * 16 - 4
							ELSE
								b := S.VAL(FreeBlock, S.VAL(INTEGER, root) + a);
								b.size := (root.size - a) DIV 16 * 16 - 4
							END
						ELSIF reducers # NIL THEN	(* 5) no space => fully reduce *)
							r := reducers; reducers := NIL;
							WHILE r # NIL DO r.Reduce(TRUE); r := r.next END;
							Collect
						END
					END
				END;
				IF b = NIL THEN
					b := OldBlock(tsize);
					IF b = NIL THEN RETURN NIL END	(* 6) give up *)
				END
			END
		END;
		(* b # NIL *)
		a := b.size + 4 - tsize;
		IF a > 0 THEN Insert(S.VAL(FreeBlock, S.VAL(INTEGER, b) + tsize), a) END;
		IF size > 0 THEN Erase(S.ADR(b.size), (size + 3) DIV 4) END;
		INC(allocated, tsize);
		RETURN S.VAL(Block, b)
	END NewBlock;

	PROCEDURE Allocated* (): INTEGER;
	BEGIN
		RETURN allocated
	END Allocated;

	PROCEDURE Used* (): INTEGER;
	BEGIN
		RETURN used
	END Used;

	PROCEDURE Root* (): INTEGER;
	BEGIN
		RETURN S.VAL(INTEGER, root)
	END Root;


	(* -------------------- Trap Handling --------------------- *)

	PROCEDURE^ InitFpu;

	PROCEDURE Start* (code: Command);
	BEGIN
		restart := code;
		S.GETREG(SP, baseStack);	(* save base stack *)
		code()
	END Start;

	PROCEDURE Quit* (exitCode: INTEGER);
		VAR m: Module; term: Command; t: BOOLEAN;
	BEGIN
		trapViewer := NIL; trapChecker := NIL; restart := NIL;
		t := terminating; terminating := TRUE; m := modList;
		WHILE m # NIL DO	(* call terminators *)
			IF ~static OR ~t THEN
				term := m.term; m.term := NIL;
				IF term # NIL THEN term() END
			END;
			ReleaseIPtrs(m);
			m := m.next
		END;
		CallFinalizers;
		hotFinalizers := finalizers; finalizers := NIL;
		CallFinalizers;
		WinOle.OleUninitialize();
		IF ~inDll THEN
			RemoveExcp(excpPtr^);
			WinApi.ExitProcess(exitCode)	(* never returns *)
		END
	END Quit;

	PROCEDURE FatalError* (id: INTEGER; str: ARRAY OF CHAR);
		VAR res: INTEGER; title: ARRAY 16 OF CHAR;
	BEGIN
		title := "Error xy";
		title[6] := CHR(id DIV 10 + ORD("0"));
		title[7] := CHR(id MOD 10 + ORD("0"));
		res := WinApi.MessageBoxW(0, str, title, {});
		IF res = 0 THEN (* win2000/XP *)
			res := WinApi.LoadLibraryA("COMCTL32.dll");
			res := WinApi.MessageBoxW(0, str, title, {})
		END;
		WinOle.OleUninitialize();
		IF ~inDll THEN RemoveExcp(excpPtr^) END;
		WinApi.ExitProcess(1)
		(* never returns *)
	END FatalError;

	PROCEDURE DefaultTrapViewer;
		VAR len, ref, end, x, a, b, c: INTEGER; mod: Module;
			modName, name: Name; res: INTEGER; n: Utf8Name; out: ARRAY 1024 OF CHAR;

		PROCEDURE WriteString (IN s: ARRAY OF CHAR);
			VAR i: INTEGER;
		BEGIN
			i := 0;
			WHILE (len < LEN(out) - 1) & (s[i] # 0X) DO out[len] := s[i]; INC(i); INC(len) END
		END WriteString;

		PROCEDURE WriteHex (x, n: INTEGER);
			VAR i, y: INTEGER;
		BEGIN
			IF len + n < LEN(out) THEN
				i := len + n - 1;
				WHILE i >= len DO
					y := x MOD 16; x := x DIV 16;
					IF y > 9 THEN y := y + (ORD("A") - ORD("0") - 10) END;
					out[i] := CHR(y + ORD("0")); DEC(i)
				END;
				INC(len, n)
			END
		END WriteHex;

		PROCEDURE WriteLn;
		BEGIN
			IF len < LEN(out) - 1 THEN out[len] := 0DX; INC(len) END
		END WriteLn;

	BEGIN
		len := 0;
		IF err = 129 THEN WriteString("invalid with")
		ELSIF err = 130 THEN WriteString("invalid case")
		ELSIF err = 131 THEN WriteString("function without return")
		ELSIF err = 132 THEN WriteString("type guard")
		ELSIF err = 133 THEN WriteString("implied type guard")
		ELSIF err = 134 THEN WriteString("value out of range")
		ELSIF err = 135 THEN WriteString("index out of range")
		ELSIF err = 136 THEN WriteString("string too long")
		ELSIF err = 137 THEN WriteString("stack overflow")
		ELSIF err = 138 THEN WriteString("integer overflow")
		ELSIF err = 139 THEN WriteString("division by zero")
		ELSIF err = 140 THEN WriteString("infinite real result")
		ELSIF err = 141 THEN WriteString("real underflow")
		ELSIF err = 142 THEN WriteString("real overflow")
		ELSIF err = 143 THEN WriteString("undefined real result")
		ELSIF err = 200 THEN WriteString("keyboard interrupt")
		ELSIF err = 202 THEN WriteString("illegal instruction:  ");
			WriteHex(val, 4)
		ELSIF err = 203 THEN WriteString("illegal memory read [ad = ");
			WriteHex(val, 8); WriteString("]")
		ELSIF err = 204 THEN WriteString("illegal memory write [ad = ");
			WriteHex(val, 8); WriteString("]")
		ELSIF err = 205 THEN WriteString("illegal execution [ad = ");
			WriteHex(val, 8); WriteString("]")
		ELSIF err < 0 THEN WriteString("exception #"); WriteHex(-err, 2)
		ELSE err := err DIV 100 * 256 + err DIV 10 MOD 10 * 16 + err MOD 10;
			WriteString("trap #"); WriteHex(err, 3)
		END;
		a := pc; b := fp; c := 12;
		REPEAT
			WriteLn; WriteString("- ");
			mod := modList;
			WHILE (mod # NIL) & ((a < mod.code) OR (a >= mod.code + mod.csize)) DO mod := mod.next END;
			IF mod # NIL THEN
				DEC(a, mod.code);
				IF mod.refcnt >= 0 THEN
					GetModName(mod, modName); WriteString(modName); ref := mod.refs;
					REPEAT GetRefProc(ref, end, n) UNTIL (end = 0) OR (a < end);
					IF a < end THEN
						Utf8ToString(n, name, res); WriteString("."); WriteString(name)
					END
				ELSE
					GetModName(mod, modName); WriteString("("); WriteString(modName); WriteString(")")
				END;
				WriteString("  ")
			END;
			WriteString("(pc="); WriteHex(a, 8);
			WriteString(", fp="); WriteHex(b, 8); WriteString(")");
			IF (b >= sp) & (b < stack) THEN
				S.GET(b+4, a);	(* stacked pc *)
				S.GET(b, b);	(* dynamic link *)
				DEC(c)
			ELSE c := 0
			END
		UNTIL c = 0;
		out[len] := 0X;
		x := WinApi.MessageBoxW(0, out, "BlackBox", {});
		IF x = 0 THEN (* win2000/XP *)
			res := WinApi.LoadLibraryA("COMCTL32.dll");
			res := WinApi.MessageBoxW(0, out, "BlackBox", {})
		END;
	END DefaultTrapViewer;

	PROCEDURE TrapCleanup;
		VAR t: TrapCleaner;
	BEGIN
		WHILE trapStack # NIL DO
			t := trapStack; trapStack := trapStack.next; t.Cleanup
		END;
		IF (trapChecker # NIL) & (err # 128) THEN trapChecker END
	END TrapCleanup;

	PROCEDURE Unwind(f: ExcpFramePtr);	(* COMPILER DEPENDENT *)
		CONST Label = 27;	(* offset of Label: from proc start *)
	BEGIN
		PushFP;
		WinApi.RtlUnwind(S.VAL(WinApi.PtrVoid, f), S.ADR(Unwind) + Label, NIL, 0);
		(* Label: *)
		PopFP
	END Unwind;
	
	(* stack overflow is detected by means of a so-called 'page guard', which offers a one-shot protection *)
	PROCEDURE ResetStackGuard;
		VAR stackLimit, regionSize: INTEGER; si: WinApi.SYSTEM_INFO;
	BEGIN
		stackLimit := FPageWord(8); (* cf. WinApi.NT_TIB.StackLimit *)
		WinApi.GetSystemInfo(si);
		regionSize := si.dwPageSize * 2; (* 2 pages must be protected in order to catch all kinds of stack overflows *)
		res := WinApi.VirtualProtect(stackLimit, regionSize, WinApi.PAGE_READWRITE+WinApi.PAGE_GUARD, old);
		IF res = 0 THEN res := WinApi.VirtualProtect(stackLimit, regionSize, WinApi.PAGE_NOACCESS, old) END
	END ResetStackGuard;

	PROCEDURE TrapHandler (excpRec: WinApi.PtrEXCEPTION_RECORD; estFrame: ExcpFramePtr;
											context: WinApi.PtrCONTEXT; dispCont: INTEGER): INTEGER;
		(* same parameter size as Try *)
	BEGIN
		IF excpRec^.ExceptionFlags * {1, 2} = {} THEN
			IF (excpRec.ExceptionCode MOD 256 = 4) & ~interrupted THEN	(* wrong trace trap *)
				context.Dr7 := 0;	(* disable all debug traps *)
				LdSP8; PopSI; PopDI; PopFP;	(* COMPILER DEPENDENT *)
				Return0(0)	(* return continueExecution without parameter remove *)
			END;
			Unwind(estFrame);
			IF trapped & (excpRec.ExceptionCode MOD 256 # 1) & (excpRec.ExceptionCode MOD 256 # 253) THEN
				DefaultTrapViewer;
				IF ~secondTrap THEN trapped := FALSE; secondTrap := TRUE END
			END;
			err := -(excpRec.ExceptionCode MOD 256);
			pc := context.Eip; sp := context.Esp; fp := context.Ebp;
			IF (currentCoroutine = NIL) OR (currentCoroutine.stackBase = baseStack) THEN stack := baseStack
			ELSE (* don't display a coroutine's TryHandler *)
				stack := currentCoroutine.stackBase - 3*SIZE(ANYPTR) (* COMPILER DEPENDENT *)
			END;
			IF err = -4 THEN err := 200	(* keyboard interrupt *)
			ELSIF err = -5 THEN
				val := excpRec.ExceptionInformation[1];
				IF val = pc THEN	(* call to undef adr *)
					err := 205; S.GET(sp, pc); INC(sp, 4); DEC(pc)
				ELSIF excpRec.ExceptionInformation[0] = 0 THEN	(* illegal read *)
					err := 203
				ELSE	(* illegal write *)
					err := 204
				END
			ELSIF (err = -29) OR (err = -30) THEN	(* illegal instruction *)
				err := 202; val := 0;
				IF IsReadable(excpRec.ExceptionAddress, excpRec.ExceptionAddress + 4) THEN
					S.GET(excpRec.ExceptionAddress, val);
					IF val MOD 100H = 8DH THEN	(* lea reg,reg *)
						IF val DIV 100H MOD 100H = 0F0H THEN err := val DIV 10000H MOD 100H	(* trap *)
						ELSIF val DIV 1000H MOD 10H = 0EH THEN
							err := 128 + val DIV 100H MOD 10H	(* run time error *)
						END
					END
				END
			ELSIF err = -142 THEN DEC(pc); err := 140	(* fpu: div by zero *)
			ELSIF (err = -144) OR (err = -146) THEN DEC(pc); err := 143	;	(* fpu: invalid op *)
				val := context.FloatSave.ControlWord MOD 4096 * 65536 + context.FloatSave.StatusWord MOD 65536
			ELSIF err = -145 THEN DEC(pc); err := 142	(* fpu: overflow *)
			ELSIF err = -147 THEN DEC(pc); err := 141	(* fpu: underflow *)
			ELSIF err = -148 THEN err := 139	(* division by zero *)
			ELSIF err = -149 THEN err := 138	(* integer overflow *)
			ELSIF (err = -1) OR (err = -253) THEN err := 137	(* stack overflow *)
			END;
			INC(trapCount);
			InitFpu;
			IF err # 137 THEN	(* stack overflow handling is delayed *)
				TrapCleanup;
				IF err = 128 THEN	(* do nothing *)
				ELSIF(trapViewer # NIL) & (restart # NIL) & ~trapped & ~guarded THEN
					trapped := TRUE; trapViewer()
				ELSE DefaultTrapViewer
				END
			END;
			trapped := FALSE; secondTrap := FALSE;
			IF dispCont = 0 THEN	(* InterfaceTrapHandler *)	(* COMPILER DEPENDENT *)
				RemoveExcp(estFrame^);
				S.PUTREG(CX, estFrame(ComExcpFramePtr).par);
				S.PUTREG(SP, S.VAL(INTEGER, estFrame) + 12);
				IF err = 137 THEN	(* retrigger stack overflow *)
					TrapCleanup; DefaultTrapViewer;
					ResetStackGuard
				END;
				PopSI; PopDI; PopBX; PopFP;
				ReturnCX(WinApi.E_UNEXPECTED)
			ELSIF estFrame # excpPtr THEN	(* Try failed *)	(* COMPILER DEPENDENT *)
				RemoveExcp(estFrame^);
				res := S.VAL(INTEGER, estFrame);
				S.PUTREG(FP, res + (SIZE(ExcpFrame) + 8));	(* restore fp *)
				S.PUTREG(SP, res - 4);	(* restore stack *)
				IF err = 137 THEN	(* retrigger stack overflow *)
					TrapCleanup; DefaultTrapViewer;
					ResetStackGuard
				END;
				PopBX;
				RETURN 0	(* return from Try *)
			ELSIF restart # NIL THEN	(* Start failed *)
				S.PUTREG(FP, baseStack);	(* restore fp *)
				S.PUTREG(SP, baseStack);	(* restore stack *)
				IF err = 137 THEN	(* retrigger stack overflow *)
					TrapCleanup; DefaultTrapViewer;
					ResetStackGuard
				END;
				restart();
				Quit(1)
			ELSE	(* boot process failed *)
				Quit(1)
			END
			(* never returns *)
		ELSE
			LdSP8; PopSI; PopDI; PopFP;	(* COMPILER DEPENDENT *)
			Return0(1)	(* return continueSearch without parameter remove *)
		END
	END TrapHandler;

	PROCEDURE SetTrapGuard* (on: BOOLEAN);
	BEGIN
		guarded := on
	END SetTrapGuard;

	PROCEDURE Try* (h: TryHandler; a, b, c: INTEGER);	(* COMPILER DEPENDENT *)
		(* same parameter size as TrapHandler *)
		VAR excp: ExcpFrame;	(* no other local variables! *)
	BEGIN
		PushBX;
		excp.handler := TrapHandler;
		InstallExcp(excp);
		h(a, b, c);
		RemoveExcp(excp);
		PopBX
	END Try;

	PROCEDURE InterfaceTrapHandler* (excpRec, estFrame, context,
																dispCont: INTEGER): INTEGER;	(* known to compiler *)
		VAR res: INTEGER;
	BEGIN
		res := TrapHandler(S.VAL(WinApi.PtrEXCEPTION_RECORD, excpRec),
								S.VAL(ExcpFramePtr, estFrame),
								S.VAL(WinApi.PtrCONTEXT, context),
								0);
		(* LdSP8 removes parameters of TrapHandler *)
		LdSP8; PopSI; PopDI; PopFP;	(* COMPILER DEPENDENT *)
		Return0(1);	(* return continueSearch without parameter remove *)
		IF FALSE THEN RETURN 0 END
	END InterfaceTrapHandler;

	(* -------------------- keyboard interrupt handling --------------------- *)

	PROCEDURE KeyboardWatcher (main: INTEGER): INTEGER;	(* runs in a thread *)
		TYPE P = PROCEDURE(w: INTEGER): INTEGER;
		VAR res, a: INTEGER; top, h: WinApi.HANDLE; done: BOOLEAN;
			context: WinApi.CONTEXT; mod: Module; isHungAppWindow: P;
	BEGIN
		done := FALSE;
		isHungAppWindow := NIL;
		h := WinApi.LoadLibraryA("user32.dll");
		IF h # 0 THEN
			isHungAppWindow := S.VAL(P, WinApi.GetProcAddress(h, "IsHungAppWindow"))
		END;
		LOOP
			res := WinApi.GetAsyncKeyState(WinApi.VK_CANCEL);
			IF res >= 0 THEN	(* key released *)
				done := FALSE;
				WinApi.Sleep(10);
			ELSIF ~done THEN	(* key pressed *)
				top := WinApi.GetForegroundWindow();
				IF (mainWnd # top) & (isHungAppWindow # NIL)
						& (top # 0) & (isHungAppWindow(top) # 0)
						& (mainWnd # 0) & (isHungAppWindow(mainWnd) # 0) THEN
					(* main window replaced by ghost window *)
					(* check if application window is topmost nonresponding window *)
					REPEAT
						top := WinApi.GetWindow(top, WinApi.GW_HWNDNEXT)
					UNTIL (top = 0) OR (isHungAppWindow(top) # 0) & (WinApi.IsWindowVisible(top) # 0)
				END;
				IF mainWnd = top THEN
					res := WinApi.SuspendThread(main);
					context.ContextFlags := {0, 16};
					res := WinApi.GetThreadContext(main, context);
					mod := modList; a := context.Eip;
					WHILE (mod # NIL) & ((a < mod.code) OR (a >= mod.code + mod.csize)) DO
						mod := mod.next
					END;
					IF (mod # NIL) & (mod.name = "Kernel") THEN mod := NIL END;
					IF mod # NIL THEN
						interrupted := TRUE;
						INCL(S.VAL(SET, context.EFlags), 8);	(* set trap flag *)
						res := WinApi.SetThreadContext(main, context);
						done := TRUE
					END;
					res := WinApi.ResumeThread(main);
					WinApi.Sleep(1);
					interrupted := FALSE
				END
			END
		END;
		RETURN 0
	END KeyboardWatcher;

	PROCEDURE InstallKeyboardInt;
		VAR res, id: INTEGER; t, main: WinApi.HANDLE;
	BEGIN
		res := WinApi.DuplicateHandle(WinApi.GetCurrentProcess(), WinApi.GetCurrentThread(),
					WinApi.GetCurrentProcess(), main, {1, 3, 4, 16..19}, 0, {});
		t := WinApi.CreateThread(NIL, 4096, KeyboardWatcher, main, {}, id);
	END InstallKeyboardInt;

	(* -------------------- coroutine support --------------------- *)

	PROCEDURE BeginCoroutines* (VAR main: Coroutine);
	BEGIN
		ASSERT(main # NIL, 20);
		IF mainCoroutine = NIL THEN
			main.stackBase := baseStack;
			main.next := main; main.prev := main;
			main.fiber := WinApi.ConvertThreadToFiber(0);
			IF main.fiber # 0 THEN
				currentCoroutine := main; mainCoroutine := main; mainCoroutineRefcnt := 1
			ELSE main := NIL
			END
		ELSE
			INC(mainCoroutineRefcnt); main := mainCoroutine
		END
	END BeginCoroutines;
	
	PROCEDURE EndCoroutines*;
		VAR res: INTEGER;
	BEGIN
		IF mainCoroutineRefcnt > 0 THEN
			DEC(mainCoroutineRefcnt);
			IF mainCoroutineRefcnt = 0 THEN
				currentCoroutine := NIL; mainCoroutine := NIL;
				res := WinApi.ConvertFiberToThread()
			END
		END
	END EndCoroutines;
	
	PROCEDURE AddCoroutine* (this: Coroutine; startProc: PROCEDURE (par: INTEGER); par: INTEGER;
												stackMin, stackMax: INTEGER; VAR done: BOOLEAN);
	BEGIN
		ASSERT(this.fiber = 0, 20); (* reuse of 'this'  without RemoveCoroutine *)
		this.stackBase := 0; (* reset because of possible reuse of 'this' *)
		(* Windows seems to use the default also for a very small stack size, so we better avoid it *)
		IF (stackMin # 0) & (stackMin < 1000) THEN stackMin := 1000 END;
		IF (stackMax # 0) & (stackMax < 2000) THEN stackMax := 2000 END;
		this.fiber := WinApi.CreateFiberEx(stackMin, stackMax, {}, startProc, par);
		IF this.fiber # 0 THEN
			done := TRUE;
			this.next := currentCoroutine.next; this.prev := currentCoroutine;
			currentCoroutine.next.prev := this; currentCoroutine.next := this
		ELSE (* swap space or virtual address space exhausted *)
			done := FALSE
		END
	END AddCoroutine;

	PROCEDURE SetCoroutineBase* (this: Coroutine);
	BEGIN
		ASSERT(this.stackBase = 0);
		this.stackBase := S.ADR(this) + SIZE(ANYPTR); (* COMPILER DEPENDENT *)
	END SetCoroutineBase;

	PROCEDURE RemoveCoroutine* (this: Coroutine);
		VAR t: TrapCleaner; chk: Handler;
	BEGIN
		IF this.next # NIL THEN
			this.prev.next := this.next; this.next.prev := this.prev;
			this.next := NIL; this.prev := NIL
		END;
		IF (this # currentCoroutine) & (this.fiber # 0) THEN
			WinApi.DeleteFiber(this.fiber); this.fiber := 0;
			WHILE this.trapStack # NIL DO
				t := this.trapStack; this.trapStack := t.next; t.Cleanup
			END;
			chk := this.trapChecker; this.trapChecker := NIL;
			IF (chk # NIL) & (err # 128) THEN chk() END
		END;
	END RemoveCoroutine;

	PROCEDURE TransferCoroutine* (target: Coroutine);
	BEGIN
		ASSERT(target.next # NIL, 20);
		IF target # currentCoroutine THEN
			currentCoroutine.trapStack := trapStack; currentCoroutine.trapChecker := trapChecker;
			S.GETREG(FP, currentCoroutine.stackTop);
			trapStack := target.trapStack; trapChecker := target.trapChecker;
			currentCoroutine := target;
			WinApi.SwitchToFiber(target.fiber)
		END
	END TransferCoroutine;

	(* -------------------- Initialization --------------------- *)

	PROCEDURE InitFpu;	(* COMPILER DEPENDENT *)
		(* could be eliminated, delayed for backward compatibility *)
		VAR cw: SET;
	BEGIN
		FINIT;
		FSTCW;
		(* denorm, underflow, precision, zero div, overflow masked *)
		(* invalid trapped *)
		(* round to nearest, temp precision *)
		cw := cw - {0..5, 8..11} + {1, 2, 3, 4, 5, 8, 9};
		FLDCW
	END InitFpu;

	PROCEDURE Init;
		VAR excp: ExcpFrame; res: COM.RESULT; i: INTEGER;
	BEGIN
		IF ~inDll THEN
			excpPtr := S.VAL(ExcpFramePtr, S.ADR(excp));
			IF static THEN
				(* use exception desc area provided by the linker *)
				INC(S.VAL(INTEGER, excpPtr), 32)	(* COMPILER DEPENDENT *)
			END;
			excpPtr.handler := TrapHandler;	(* init exception handling *)
			InstallExcp(excpPtr^)
		END;
		allocated := 0; total := 0; used := 0;
		sentinelBlock.size := MAX(INTEGER);
		sentinel := S.ADR(sentinelBlock);

		S.PUTREG(ML, S.ADR(modList));
		WinApi.OutputDebugStringW("BlackBox started");

		IF dllMem THEN
			i := N;
			REPEAT DEC(i); free[i] := sentinel UNTIL i = 0;
			heap := WinApi.GetProcessHeap()
		ELSE
			i := N;
			REPEAT DEC(i); free[i] := sentinel UNTIL i = 0;
			AllocHeapMem(1, root); ASSERT(root # NIL, 100);
			i := MIN(N - 1, (root.size - 12) DIV 16 - 1);
			free[i] := S.VAL(FreeBlock, S.VAL(INTEGER, root) + 12);
			free[i].next := sentinel;
			free[i].size := (root.size - 12) DIV 16 * 16 - 4;
			free[i].tag:=S.VAL(Type, S.ADR(free[i].size))
		END;

		res := WinOle.OleInitialize(0);
		IF inDll THEN
			baseStack := FPageWord(4)	(* begin of stack segment *)
		ELSE
			InstallKeyboardInt;
			InitFpu
		END;
		IF ~static THEN
			InitModule(modList);
			IF ~inDll THEN Quit(1) END
		END;
		told := 0; shift := 0
	END Init;

BEGIN
	IF modList = NIL THEN	(* only once *)
		S.GETREG(ML, modList);	(* linker loads module list to BX *)
		S.GETREG(SP, baseStack);
		static := init IN modList.opts;
		inDll := dll IN modList.opts; dllMem := inDll;
		Init
	END
CLOSE
	IF ~terminating THEN
		terminating := TRUE;
		Quit(0)
	END*)
END bbKernel.
