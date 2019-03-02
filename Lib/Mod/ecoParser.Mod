(* Copyright (C) 1997-2015, Oleg N. Cher *)

MODULE EcoParser; (** portable *)

TYPE
	Char      * = SHORTCHAR;
	String    * = ARRAY OF Char;
	StrPtr    * = POINTER TO String;
	CtxPtr    * = POINTER TO CtxNode;
	NodePtr   * = POINTER TO Node;
	ParserPtr * = POINTER TO Parser;

	Tree* = RECORD
		root	: CtxPtr; (* Pointer to root context *)
		this- : CtxPtr; (* Pointer to the last added context *)
		addTo*: CtxPtr; (* Pointer to add to context *)
	END;

	Parser* = (*EXTENSIBLE*) RECORD
		inpBuf*: StrPtr;	(* Pointer to input data buffer *)
		inpIdx*: INTEGER; (* Index of current char *)
		inpAvl*: INTEGER; (* Available data length *)
		ctxNow*: CtxPtr;	(* Pointer to current context *)
		ctxStk : POINTER TO ARRAY OF CtxPtr;
		ctxIdx : INTEGER; (* Top of context stack index *)
		nodFnd*: NodePtr; (* Pointer to found node *)
	END;

	Comparer* = PROCEDURE (VAR prs: Parser): BOOLEAN;
	Method* = PROCEDURE (VAR prs: Parser);

	Node* = (*EXTENSIBLE*) RECORD
		nextNode*: NodePtr;
		comparer*: Comparer;
		nameLen* : INTEGER;
		name*		: StrPtr;
		method*	: Method;
	END;

	CtxNode* = RECORD (Node)
		chldNode*: NodePtr;
		NotFound*: Method;
	END;

	MethodNode = RECORD (Node)
	END;

	SetCtxNode = RECORD (Node);
		setCtx : CtxPtr;
	END;

PROCEDURE (VAR prs: Parser) SetContextByPtr* (ctx: CtxPtr) (*NEW*);
BEGIN
	prs.ctxNow := ctx;
END SetContextByPtr;

PROCEDURE (VAR prs: Parser) PushContext* (ctx: CtxPtr) (*NEW*);
BEGIN
	ASSERT(prs.ctxIdx < LEN(prs.ctxStk^) (* Is stack full? *) );
	prs.ctxStk[prs.ctxIdx] := prs.ctxNow;
	INC(prs.ctxIdx);
	prs.ctxNow := ctx;
END PushContext;

PROCEDURE (VAR prs: Parser) PopContext* (*NEW*);
BEGIN
	ASSERT(prs.ctxIdx # 0 (* Is stack empty? *) );
	DEC(prs.ctxIdx);
	prs.ctxNow := prs.ctxStk[prs.ctxIdx];
END PopContext;

(* --------------------------------------------------------- *)
(*						 Some base Parser.String functions						 *)
(* --------------------------------------------------------- *)
PROCEDURE Length* (IN str: String): INTEGER;
	(* Returns the length of a null-terminated string in chars *)
VAR
	len, maxLen: INTEGER;
BEGIN
	maxLen := SHORT(LEN(str));
	len := 0;
	WHILE (len < maxLen) & (str[len] # 0X) DO INC(len) END;
	RETURN len
END Length;

PROCEDURE Cap* (c: Char): Char;
	(* Returns the capital letter of a character. Russian CP 1251 supported	*)
BEGIN
	RETURN CAP(c)
END Cap;

PROCEDURE Pos* (IN str: String; IN subStr: String; pos: INTEGER): INTEGER;
VAR
	i: INTEGER;
BEGIN
	IF (pos = 0) OR (pos <= Length(str) - Length(subStr)) THEN
		WHILE str[pos] # 0X DO
			i := 0;
			WHILE (str[pos+i] = subStr[i]) & (subStr[i] # 0X) DO INC(i) END;
			IF subStr[i] = 0X THEN RETURN pos END;
			INC(pos);
		END;
	END;
	RETURN -1 (* subStr not found in str *)
END Pos;

(* -------------------------------------------------------- *)
(*	 Built-in procedures for processing a current context	 *)
(* -------------------------------------------------------- *)
PROCEDURE None* (VAR prs: Parser); (* Do nothing *)
BEGIN	 
END None;

PROCEDURE Ctxt (VAR prs: Parser); (* Set found context *)
BEGIN
	prs.ctxNow := prs.nodFnd(CtxPtr);
END Ctxt;

PROCEDURE SetCtx (VAR prs: Parser); (* Set saved context *)
TYPE
	SetCtxNodePtr = POINTER TO SetCtxNode;
BEGIN
	prs.ctxNow := prs.nodFnd(SetCtxNodePtr).setCtx;
END SetCtx;

PROCEDURE Push* (VAR prs: Parser); (* Set found context, push old *)
BEGIN
	prs.PushContext(prs.ctxNow);
	prs.ctxNow := prs.nodFnd(CtxPtr);
END Push;

PROCEDURE (VAR tree: Tree) Init* (*NEW*);
	(* Initialize the context tree *)
BEGIN
	tree.root := NIL; tree.this := NIL; tree.addTo := NIL;
END Init;

PROCEDURE (VAR tree: Tree) To* (ctx : CtxPtr) (*NEW*);
BEGIN
	tree.addTo := ctx;
END To;

PROCEDURE (VAR tree: Tree) AddContext* (
		cmp: Comparer; IN name: String; notfound: Method) (*NEW*);
VAR
	newCtx: CtxPtr;
BEGIN
	NEW(newCtx);
	IF tree.addTo # NIL THEN (* Add a node context *)
		newCtx.nextNode := tree.addTo.chldNode;
		tree.addTo.chldNode := newCtx;
	ELSE (* Add a root context *)
		newCtx.nextNode := NIL;
	END;
	newCtx.comparer := cmp;
	newCtx.nameLen	:= Length(name); (* Add length								 *)
	NEW(newCtx.name, LEN(name));
	newCtx.name^		:= name$;				(*	and name of the context	 *)
	newCtx.method	 := Ctxt;
	newCtx.chldNode := NIL;					(* Pointer to child node			*)
	newCtx.NotFound := notfound;		 (* Method 'Context not found' *)
	IF tree.this = NIL THEN
		tree.root := newCtx;
	END;
	tree.this := newCtx;
END AddContext;

PROCEDURE (VAR ctx: CtxNode) AddContext* (
		cmp: Comparer; IN name: String; notfound: Method) (*NEW*);
VAR
	newCtx: CtxPtr;
BEGIN
	NEW(newCtx); (* Add a node context *)
	newCtx.nextNode := ctx.chldNode;
	ctx.chldNode := newCtx;
	newCtx.comparer := cmp;
	newCtx.nameLen	:= Length(name); (* Add length								 *)
	NEW(newCtx.name, LEN(name));
	newCtx.name^		:= name$;				(*	and name of the context	 *)
	newCtx.method	 := Ctxt;
	newCtx.chldNode := NIL;					(* Pointer to child node			*)
	newCtx.NotFound := notfound;		 (* Method 'Context not found' *)
END AddContext;

PROCEDURE NewContext* (
		cmp: Comparer; IN name: String; notfound: Method): CtxPtr;
VAR
	newCtx: CtxPtr;
BEGIN
	NEW(newCtx); (* Add a node context *)
	newCtx.comparer := cmp;
	newCtx.nameLen	:= Length(name); (* Add length								 *)
	NEW(newCtx.name, LEN(name));
	newCtx.name^		:= name$;				(*	and name of the context	 *)
	newCtx.method	 := Ctxt;
	newCtx.chldNode := NIL;					(* Pointer to child node			*)
	newCtx.NotFound := notfound;		 (* Method 'Context not found' *)
	RETURN newCtx
END NewContext;

PROCEDURE (VAR tree: Tree) AddPushContext* (
		cmp: Comparer; IN name: String; notfound: Method) (*NEW*);
BEGIN
	tree.AddContext(cmp, name, notfound);
	tree.this.method := Push;
END AddPushContext;

PROCEDURE (VAR tree: Tree) AddMethod* (
		cmp: Comparer; IN name: String; meth: Method) (*NEW*);
VAR
	newMeth: POINTER TO MethodNode;
BEGIN
	NEW(newMeth);
	newMeth.nextNode	 := tree.addTo.chldNode;
	tree.addTo.chldNode := newMeth;
	newMeth.comparer	 := cmp;
	newMeth.nameLen		:= Length(name); (* Add length *)
	NEW(newMeth.name, LEN(name));
	newMeth.name^			:= name$; (*	and name of the method *)
	newMeth.method		 := meth;
END AddMethod;

PROCEDURE (VAR tree: Tree) AddSetContext* (
		cmp: Comparer; IN name: String; setContext: CtxPtr) (*NEW*);
VAR
	newSetCtx: POINTER TO SetCtxNode;
BEGIN
	NEW(newSetCtx);
	newSetCtx.nextNode := tree.addTo.chldNode;
	tree.addTo.chldNode := newSetCtx;
	newSetCtx.comparer := cmp;
	newSetCtx.nameLen	:= Length(name); (* Add length *)
	NEW(newSetCtx.name, LEN(name));
	newSetCtx.name^		:= name$; (* and name of the method *)
	newSetCtx.method	 := SetCtx;
	newSetCtx.setCtx	 := setContext;
END AddSetContext;

(* --------------------------------------------------------- *)
(*										Built-in comparers										 *)
(* --------------------------------------------------------- *)
PROCEDURE Never* (VAR prs: Parser): BOOLEAN; (* Always FALSE *)
BEGIN
	RETURN FALSE
END Never;

PROCEDURE Always* (VAR prs: Parser): BOOLEAN; (* Always TRUE *)
BEGIN
	RETURN TRUE
END Always;

PROCEDURE IsNull* (VAR prs: Parser): BOOLEAN; (* Is incoming null? *)
BEGIN
	IF (prs.inpAvl > 0) & (prs.inpBuf[prs.inpIdx] = 0X) THEN
		INC(prs.inpIdx); DEC(prs.inpAvl);
		RETURN TRUE
	END;
	RETURN FALSE
END IsNull;

PROCEDURE IsData* (VAR prs: Parser): BOOLEAN; (* Is incoming data? *)
BEGIN
	RETURN prs.inpAvl > 0
END IsData;

PROCEDURE Contains* (VAR prs: Parser): BOOLEAN; (* Contains string? *)
VAR
	pos: INTEGER;
BEGIN
	IF prs.nodFnd.nameLen > prs.inpAvl THEN RETURN FALSE END;
	RETURN Pos(prs.inpBuf^, prs.nodFnd.name^, 0) # -1
END Contains;

PROCEDURE EquAbs* (VAR prs: Parser): BOOLEAN; (* Absolute equal *)
VAR
	wrdLength, tmpInpIdx, tmpNodeIdx: INTEGER;
BEGIN
	IF prs.nodFnd.nameLen > prs.inpAvl THEN RETURN FALSE END;
	wrdLength := prs.nodFnd.nameLen;
	tmpInpIdx := prs.inpIdx;
	tmpNodeIdx := 0;
	WHILE wrdLength > 0 DO
		IF prs.inpBuf[tmpInpIdx] # prs.nodFnd.name[tmpNodeIdx]
			THEN RETURN FALSE END;
		INC(tmpInpIdx);
		INC(tmpNodeIdx);
		DEC(wrdLength);
	END;
	prs.inpIdx := tmpInpIdx;
	DEC(prs.inpAvl, prs.nodFnd.nameLen);
	RETURN TRUE
END EquAbs;

PROCEDURE EquCap* (VAR prs: Parser): BOOLEAN; (* Case independent *)
VAR
	wrdLength, tmpInpIdx, tmpNodeIdx: INTEGER;
BEGIN
	IF prs.nodFnd.nameLen > prs.inpAvl THEN RETURN FALSE END;
	wrdLength := prs.nodFnd.nameLen;
	tmpInpIdx := prs.inpIdx;
	tmpNodeIdx := 0;
	WHILE wrdLength > 0 DO
		IF Cap(prs.inpBuf[tmpInpIdx]) # prs.nodFnd.name[tmpNodeIdx]
			THEN RETURN FALSE END;
		INC(tmpInpIdx);
		INC(tmpNodeIdx);
		DEC(wrdLength);
	END;
	prs.inpIdx := tmpInpIdx;
	DEC(prs.inpAvl, prs.nodFnd.nameLen);
	RETURN TRUE
END EquCap;

PROCEDURE (VAR prs: Parser) Init*
	(buf: StrPtr; bufLen, ctxDeep: INTEGER) (*NEW*) (*EXTENSIBLE*);
	(* Set input buffer for parsing *)
BEGIN
	prs.inpBuf := buf;
	prs.inpIdx := 0;
	IF buf = NIL THEN
		prs.inpAvl := 0;
	ELSIF bufLen <= LEN(buf^) THEN
		prs.inpAvl := bufLen;
	ELSE
		prs.inpAvl := SHORT(LEN(buf^));
	END;
	IF ctxDeep # 0 THEN
		NEW(prs.ctxStk, ctxDeep);
		prs.ctxIdx := 0;
	END;
END Init;

PROCEDURE (VAR prs: Parser) Exec* () (*NEW*);
	(* Execute one-token parsing event method *)
BEGIN
	prs.nodFnd := prs.ctxNow.chldNode;
	WHILE (prs.nodFnd # NIL) & ~prs.nodFnd.comparer(prs) DO
		prs.nodFnd := prs.nodFnd.nextNode;
	END;
	IF prs.nodFnd # NIL THEN
		prs.nodFnd.method(prs);
	ELSE
		prs.ctxNow.NotFound(prs);
	END;
END Exec;

PROCEDURE NewParser* (buf: StrPtr; bufLen, ctxDeep: INTEGER): ParserPtr;
VAR
	prsPtr: ParserPtr;
BEGIN
	NEW(prsPtr); prsPtr^.Init(buf, bufLen, ctxDeep); RETURN prsPtr
END NewParser;

END EcoParser.
