MODULE OfrontOPB;	(* RC 6.3.89 / 21.2.94 *)	(* object model 17.1.93 *)
(* build parse tree *)

(* changes
	2015-10-07 jt, support for SYSTEM.ADR("x"), i.e. applied to char constant, added in MOp
	2017-03-16 dcwbrown, allow usage of SYSTEM.VAL in CONST section to explicitly define types of integer constants
	2017-08-16 jt, second operand of INC/DEC may be any integer type not larger than first operand
	2018-02-14 olegncher, issue #44: unlimited length of string literals
*)

	IMPORT OPT := OfrontOPT, OPS := OfrontOPS, OPM := OfrontOPM, SYSTEM;

	CONST
		(* symbol values or ops *)
		times = 1; slash = 2; div = 3; mod = 4; div0 = 5; rem0 = 6;
		and = 7; plus = 8; minus = 9; or = 10; eql = 11;
		neq = 12; lss = 13; leq = 14; gtr = 15; geq = 16;
		in = 17; is = 18; ash = 19; msk = 20; len = 21;
		conv = 22; abs = 23; cap = 24; odd = 25; asr = 26;
		lsl = 27; ror = 28; not = 35; min = 36; max = 37;
		(*SYSTEM*)
		lsh = 29; rot = 30; adr = 31; bit = 33; val = 34;
		unsgn = 40;

		(* object modes *)
		Var = 1; VarPar = 2; Con = 3; Fld = 4; Typ = 5; LProc = 6; XProc = 7;
		SProc = 8; CProc = 9; IProc = 10; Mod = 11; Head = 12; TProc = 13;

		(* Structure forms *)
		Undef = 0; Int8 = 1; Bool = 2; Char8 = 3; Int16 = 4; Int32 = 5; Int64 = 6;
		Real32 = 7; Real64 = 8; Set = 9; String8 = 10; NilTyp = 11; NoTyp = 12;
		Pointer = 13; Byte = 14; ProcTyp = 15; Comp = 16;
		Char16 = 17; String16 = 18;
		intSet = {Int8, Byte, Int16..Int64}; realSet = {Real32, Real64}; charSet = {Char8, Char16};

		(* composite structure forms *)
		Basic = 1; Array = 2; DynArr = 3; Record = 4;

		(* attribute flags (attr.adr, struct.attribute, proc.conval.setval) *)
		extAttr = 20;

		(* nodes classes *)
		Nvar = 0; Nvarpar = 1; Nfield = 2; Nderef = 3; Nindex = 4; Nguard = 5; Neguard = 6;
		Nconst = 7; Ntype = 8; Nproc = 9; Nupto = 10; Nmop = 11; Ndop = 12; Ncall = 13;
		Ninittd = 14; Nif = 15; Ncaselse = 16; Ncasedo = 17; Nenter = 18; Nassign = 19;
		Nifelse = 20; Ncase = 21; Nwhile = 22; Nrepeat = 23; Nloop = 24; Nexit = 25;
		Nreturn = 26; Nwith = 27; Ntrap = 28;

		(*function number*)
		assign = 0;
		haltfn = 0; newfn = 1; absfn = 2; capfn = 3; ordfn = 4;
		entierfn = 5; oddfn = 6; minfn = 7; maxfn = 8; chrfn = 9;
		shortfn = 10; longfn = 11; bitsfn = 12; fltfn = 13; lchrfn = 14;
		sizefn = 15; asrfn = 16; lslfn = 17; rorfn = 18; incfn = 19;
		decfn = 20; inclfn = 21; exclfn = 22; lenfn = 23; copyfn = 24;
		ashfn = 25; ushortfn = 26; packfn = 32; unpkfn = 33; assertfn = 38;

		(*SYSTEM function number*)
		adrfn = 27; lshfn = 28; rotfn = 29; getfn = 30; putfn = 31;
		bitfn = 34; valfn = 35; sysnewfn = 36; movefn = 37;
		typfn = 39; thisrecfn = 40; thisarrfn = 41;

		(* module visibility of objects *)
		internal = 0; external = 1; externalR = 2; inPar = 3; outPar = 4;

		(* procedure flags (conval^.setval) *)
		hasBody = 1; isRedef = 2; slNeeded = 3;

		(* sysflags *)
		nilBit = 1;

		AssertTrap = 0;	(* default trap number *)

	VAR
		typSize*: PROCEDURE (typ: OPT.Struct);
		exp: SHORTINT;	(*side effect of log*)

	PROCEDURE err (n: SHORTINT);
	BEGIN OPM.err(n)
	END err;

	PROCEDURE NewLeaf* (obj: OPT.Object): OPT.Node;
		VAR node: OPT.Node;
	BEGIN
		CASE obj^.mode OF
		  Var:
				node := OPT.NewNode(Nvar); node^.readonly := (obj^.vis = externalR) & (obj^.mnolev < 0)
		| VarPar:
				node := OPT.NewNode(Nvarpar); node^.readonly := (obj^.vis = inPar) OR (obj^.vis = externalR)
		| Con:
				node := OPT.NewNode(Nconst); node^.conval := OPT.NewConst();
				node^.conval^ := obj^.conval^	(* string is not copied, only its ref *)
		| Typ:
				node := OPT.NewNode(Ntype)
		| LProc..IProc:
				node := OPT.NewNode(Nproc)
		ELSE err(127); node := OPT.NewNode(Nvar)
		END;
		node^.obj := obj; node^.typ := obj^.typ;
		RETURN node
	END NewLeaf;

	PROCEDURE Construct* (class: BYTE; VAR x: OPT.Node; y: OPT.Node);
		VAR node: OPT.Node;
	BEGIN
		node := OPT.NewNode(class); node^.typ := OPT.notyp;
		node^.left := x; node^.right := y; x := node
	END Construct;

	PROCEDURE Link* (VAR x, last: OPT.Node; y: OPT.Node);
	BEGIN
		IF x = NIL THEN x := y ELSE last^.link := y END;
		WHILE y^.link # NIL DO y := y^.link END;
		last := y
	END Link;

	PROCEDURE BoolToInt (b: BOOLEAN): INTEGER;
	BEGIN
		IF b THEN RETURN 1 ELSE RETURN 0 END
	END BoolToInt;

	PROCEDURE IntToBool (i: LONGINT): BOOLEAN;
	BEGIN
		RETURN i # 0
	END IntToBool;

	PROCEDURE SetToInt (s: SET): INTEGER;
		VAR x, i: INTEGER;
	BEGIN
		i := 31; x := 0;
		IF 31 IN s THEN x := -1 END;
		WHILE i > 0 DO
			x := x * 2; DEC(i);
			IF i IN s THEN INC(x) END
		END;
		RETURN x
	END SetToInt;

	PROCEDURE IntToSet (x: INTEGER): SET;
		VAR i: INTEGER; s: SET;
	BEGIN
		i := 0; s := {};
		WHILE i < 32 DO
			IF ODD(x) THEN INCL(s, i) END;
			x := x DIV 2; INC(i)
		END;
		RETURN s
	END IntToSet;

	PROCEDURE NewBoolConst* (boolval: BOOLEAN): OPT.Node;
		VAR x: OPT.Node;
	BEGIN
		x := OPT.NewNode(Nconst); x^.typ := OPT.booltyp;
		x^.conval := OPT.NewConst(); x^.conval^.intval := BoolToInt(boolval); RETURN x
	END NewBoolConst;

	PROCEDURE OptIf* (VAR x: OPT.Node);	(* x^.link = NIL *)
		VAR if, pred: OPT.Node;
	BEGIN
		if := x^.left;
		WHILE if^.left^.class = Nconst DO
			IF IntToBool(if^.left^.conval^.intval) THEN x := if^.right; RETURN
			ELSIF if^.link = NIL THEN x := x^.right; RETURN
			ELSE if := if^.link; x^.left := if
			END
		END;
		pred := if; if := if^.link;
		WHILE if # NIL DO
			IF if^.left^.class = Nconst THEN
				IF IntToBool(if^.left^.conval^.intval) THEN
					pred^.link := NIL; x^.right := if^.right; RETURN
				ELSE if := if^.link; pred^.link := if
				END
			ELSE pred := if; if := if^.link
			END
		END
	END OptIf;

	PROCEDURE Nil* (): OPT.Node;
		VAR x: OPT.Node;
	BEGIN
		x := OPT.NewNode(Nconst); x^.typ := OPT.niltyp;
		x^.conval := OPT.NewConst(); x^.conval^.intval := OPM.nilval; RETURN x
	END Nil;

	PROCEDURE EmptySet* (): OPT.Node;
		VAR x: OPT.Node;
	BEGIN
		x := OPT.NewNode(Nconst); x^.typ := OPT.settyp;
		x^.conval := OPT.NewConst(); x^.conval^.setval := {}; RETURN x
	END EmptySet;

	PROCEDURE GetTempVar* (IN name: ARRAY OF SHORTCHAR; typ: OPT.Struct; VAR obj: OPT.Object);
		VAR n: OPS.Name; o: OPT.Object;
	BEGIN
		n := "@@  "; OPT.Insert(n, obj); obj.name^ := name$;	(* avoid err 1 *)
		obj.mode := Var; obj.typ := typ;
		o := OPT.topScope.scope;
		IF o = NIL THEN OPT.topScope.scope := obj
		ELSE
			WHILE o.link # NIL DO o := o.link END;
			o.link := obj
		END
	END GetTempVar;

	(* ---------- constant operations ---------- *)

	PROCEDURE SetIntType (node: OPT.Node; raw: BOOLEAN);
		VAR v: LONGINT;
	BEGIN v := node^.conval^.intval;
		IF ~raw THEN raw := (OPM.Lang # "C") & (OPM.Lang # "7") END;
		IF raw & (MIN(BYTE) <= v) & (v <= MAX(BYTE)) THEN node^.typ := OPT.bytetyp
		ELSIF (raw OR (OPM.AdrSize = 2)) & (MIN(SHORTINT) <= v) & (v <= MAX(SHORTINT))
			THEN node^.typ := OPT.sinttyp
		ELSIF (MIN(INTEGER) <= v) & (v <= MAX(INTEGER)) THEN node^.typ := OPT.inttyp
		ELSE node^.typ := OPT.linttyp
		END
	END SetIntType;

	PROCEDURE NewIntConst* (intval: LONGINT): OPT.Node;
		VAR x: OPT.Node;
	BEGIN
		x := OPT.NewNode(Nconst); x^.conval := OPT.NewConst();
		x^.conval^.intval := intval; SetIntType(x, FALSE); RETURN x
	END NewIntConst;

	PROCEDURE NewArrConst* (VAR x:OPT.Node);	(* prepare node for a constant array *)
		VAR n: INTEGER; typ: OPT.Struct;y: OPT.ConstArr; fp: OPT.Object;
	BEGIN
		n := 1; typ := x^.typ;
		WHILE (typ^.form = Comp) & (typ^.comp = Array) DO
			n := n * typ^.n;	(* multiplication of sizes for all dimensions *)
			typ := typ^.BaseTyp
		END;
		(* n - total number of array elements, typ - element type *)
		IF typ^.form IN intSet + {Bool, Char8} THEN	(* array of integers (incl. BYTE) *)
			fp := OPT.NewObj(); fp^.typ := x^.obj^.typ^.BaseTyp;
			fp^.mode := Var;	(* fp is variable, element of the array *)
			NEW(y);
			(* allocate a piece of memory for n elements of the array *)
			CASE typ^.size OF	(* size of array elements in bytes (for BlackBox). *)
			| 1:	(* BOOLEAN, CHAR, BYTE (for Ofront+) *)
						NEW(y^.val1, n)
			| 2:	(* a type with 2-bytes size *)
						NEW(y^.val2, n)
			| 4:	(* a type with 4-bytes size *)
						NEW(y^.val4, n)
			| 8:	(* a type with 8-bytes size *)
						NEW(y^.val8, n)
			END;
			fp := x^.obj;	(* remember the type "array" *)
			x := Nil();	(* as if it is NIL *)
			x^.typ := OPT.undftyp;	(* so that cannot be performed any operations with the construction *)
			x^.conval^.intval := 0;	(* the number of already filled array elements *)
			x^.conval^.intval2 := n;	(* how many elements of the array should be *)
			x^.conval^.arr := y;	(* the elements themselves *)
			x^.obj := fp	(* constant of type "array" *)
		ELSE err(51)
		END
	END NewArrConst;

	PROCEDURE Min* (form: BYTE): LONGINT;
		VAR min: LONGINT;
	BEGIN
		CASE form OF
		| Byte: min := 0
		| Int8: min := MIN(BYTE)
		| Int16: min := MIN(SHORTINT)
		| Int32: min := MIN(INTEGER)
		| Int64: min := MIN(LONGINT)
		END;
		RETURN min
	END Min;

	PROCEDURE Max* (form: BYTE): LONGINT;
		VAR max: LONGINT;
	BEGIN
		CASE form OF
		| Byte: max := 0FFH
		| Int8: max := MAX(BYTE)
		| Int16: max := MAX(SHORTINT)
		| Int32: max := MAX(INTEGER)
		| Int64: max := MAX(LONGINT)
		END;
		RETURN max
	END Max;

	PROCEDURE Short2Size* (n: LONGINT; form: BYTE): LONGINT;
	BEGIN
		CASE form OF
			| Byte: n := n MOD 100H
			| Int8: n := SHORT(SHORT(SHORT(n)))
			| Int16: n := SHORT(SHORT(n))
			| Int32: n := SHORT(n)
			| Int64:
		END;
		RETURN n
	END Short2Size;

	PROCEDURE NewShortConst* (uintval: LONGINT; size: INTEGER): OPT.Node;
	(* creates of a new signed constant of size bytes from the corresponding
	(encoded with the same combination of bits) unsigned constant uintval *)
		VAR x: OPT.Node;
	BEGIN
		x := OPT.NewNode(Nconst); x^.conval := OPT.NewConst();
		CASE size OF	(* truncates a signed constant to size bytes *)
			| 1: uintval := SHORT(SHORT(SHORT(uintval)))
			| 2: uintval := SHORT(SHORT(uintval))
			| 4: uintval := SHORT(uintval)
			| 8:
		END;
		x^.conval^.intval := uintval; SetIntType(x, FALSE); RETURN x
	END NewShortConst;

	PROCEDURE NewRealConst* (realval: REAL; typ: OPT.Struct): OPT.Node;
		VAR x: OPT.Node;
	BEGIN
		x := OPT.NewNode(Nconst); x^.conval := OPT.NewConst();
		x^.conval^.realval := realval; x^.typ := typ; x^.conval^.intval := OPM.ConstNotAlloc;
		RETURN x
	END NewRealConst;

	PROCEDURE NewString* (str: OPS.String; lstr: POINTER TO ARRAY OF CHAR; len: INTEGER): OPT.Node;
		VAR i, j, c: INTEGER; x: OPT.Node; ext: OPT.ConstExt;
	BEGIN
		x := OPT.NewNode(Nconst); x^.conval := OPT.NewConst();
		IF lstr # NIL THEN
			x.typ := OPT.string16typ;
			NEW(ext, 3 * len); i := 0; j := 0;
			REPEAT c := ORD(lstr[i]); INC(i); OPM.PutUtf8(ext^, c, j) UNTIL c = 0;
			x.conval.ext := ext
		ELSE
			x^.typ := OPT.string8typ; x^.conval^.ext := str
		END;
		x^.conval^.intval := OPM.ConstNotAlloc; x^.conval^.intval2 := len;
		RETURN x
	END NewString;

	PROCEDURE CharToString8 (n: OPT.Node);
		VAR ch: SHORTCHAR;
	BEGIN
		n^.typ := OPT.string8typ; ch := SHORT(CHR(n^.conval^.intval)); NEW(n^.conval^.ext, 2);
		IF ch = 0X THEN n^.conval^.intval2 := 1 ELSE n^.conval^.intval2 := 2; n^.conval^.ext[1] := 0X END;
		n^.conval^.ext[0] := ch; n^.conval^.intval := OPM.ConstNotAlloc; n^.obj := NIL
	END CharToString8;

	PROCEDURE CharToString16 (n: OPT.Node);
		VAR i: INTEGER;
	BEGIN
		n.typ := OPT.string16typ; NEW(n.conval.ext, 4);
		IF n.conval.intval = 0 THEN
			n.conval.ext[0] := 0X; n.conval.intval2 := 1
		ELSE
			i := 0; OPM.PutUtf8(n.conval.ext^, SHORT(n.conval.intval), i);
			n.conval.ext[i] := 0X; n.conval.intval2 := 2
		END;
		n.conval.intval := OPM.ConstNotAlloc; n.obj := NIL
	END CharToString16;

	PROCEDURE String8ToString16 (n: OPT.Node);
		VAR i, j, x: INTEGER; ext, new: OPT.ConstExt;
	BEGIN
		n.typ := OPT.string16typ; ext := n.conval.ext;
		NEW(new, 2 * n.conval.intval2); i := 0; j := 0; 
		REPEAT x := ORD(ext[i]); INC(i); OPM.PutUtf8(new^, x, j) UNTIL x = 0;
		n.conval.ext := new; n.obj := NIL
	END String8ToString16;

	PROCEDURE String16ToString8 (n: OPT.Node);
		VAR i, j, x: INTEGER; ext, new: OPT.ConstExt;
	BEGIN
		n.typ := OPT.string8typ; ext := n.conval.ext;
		NEW(new, n.conval.intval2); i := 0; j := 0;
		REPEAT OPM.GetUtf8(ext^, x, i); new[j] := SHORT(CHR(x MOD 256)); INC(j) UNTIL x = 0;
		n.conval.ext := new; n.obj := NIL
	END String16ToString8;

	PROCEDURE CheckString (n: OPT.Node; typ: OPT.Struct; e: SHORTINT);
		VAR ntyp: OPT.Struct;
	BEGIN
		ntyp := n.typ;
		IF (typ.comp IN {Array, DynArr}) & (typ.BaseTyp.form = Char8) OR (typ.form = String8) THEN
			IF (n.class = Nconst) & (ntyp.form = Char8) THEN CharToString8(n)
			ELSIF (ntyp.comp IN {Array, DynArr}) & (ntyp.BaseTyp.form = Char8) OR (ntyp.form = String8) THEN (* ok *)
			ELSE err(e)
			END
		ELSIF (typ.comp IN {Array, DynArr}) & (typ.BaseTyp.form = Char16) OR (typ.form = String16) THEN
			IF (n.class = Nconst) & (ntyp.form IN charSet) THEN CharToString16(n)
			ELSIF (n.class = Nconst) & (ntyp.form = String8) THEN String8ToString16(n)
			ELSIF (ntyp.comp IN {Array, DynArr}) & (ntyp.BaseTyp.form = Char16) OR (ntyp.form = String16) THEN
				(* ok *)
			ELSE err(e)
			END
		ELSE err(e)
		END
	END CheckString;

	PROCEDURE BindNodes (class: BYTE; typ: OPT.Struct; VAR x: OPT.Node; y: OPT.Node);
		VAR node: OPT.Node;
	BEGIN
		node := OPT.NewNode(class); node^.typ := typ;
		node^.left := x; node^.right := y; x := node
	END BindNodes;

	PROCEDURE NotVar (x: OPT.Node): BOOLEAN;
	BEGIN
		RETURN (x^.class >= Nconst) & ((x^.class # Nmop) OR (x^.subcl # val) OR NotVar(x^.left))
			OR (x^.typ^.form = String8)
			OR (x^.class = Nguard) & NotVar(x^.left)
	END NotVar;

	PROCEDURE DeRef* (VAR x: OPT.Node);
		VAR strobj, bstrobj: OPT.Object; typ, btyp: OPT.Struct;
	BEGIN
		typ := x^.typ;
		IF (x^.class = Nconst) OR (x^.class = Ntype) OR (x^.class = Nproc) THEN err(78)
		ELSIF typ^.form = Pointer THEN
			IF typ = OPT.sysptrtyp THEN err(57) END;
			btyp := typ^.BaseTyp; strobj := typ^.strobj; bstrobj := btyp^.strobj;
			IF (strobj # NIL) & (strobj^.name # OPT.null) & (bstrobj # NIL) & (bstrobj^.name # OPT.null) THEN
				btyp^.pbused := TRUE
			END;
			BindNodes(Nderef, btyp, x, NIL)
		ELSE err(84)
		END
	END DeRef;

	PROCEDURE StrDeref* (VAR x: OPT.Node);
		VAR typ, btyp: OPT.Struct;
	BEGIN
		typ := x^.typ;
		IF (x^.class = Nconst) OR (x^.class = Ntype) OR (x^.class = Nproc) THEN err(78)
		ELSIF (typ^.comp IN {Array, DynArr}) & (typ^.BaseTyp^.form IN charSet) THEN
			IF (typ.BaseTyp # NIL) & (typ.BaseTyp.form = Char8) THEN btyp := OPT.string8typ
			ELSE btyp := OPT.string16typ
			END;
			BindNodes(Nderef, btyp, x, NIL); x^.subcl := 1
		ELSE err(90)
		END
	END StrDeref;

	PROCEDURE Index* (VAR x: OPT.Node; y: OPT.Node);
		VAR f: SHORTINT; typ: OPT.Struct;
	BEGIN
		f := y^.typ^.form;
		IF x^.class >= Nconst THEN err(79)
		ELSIF ~(f IN intSet) OR (y^.class IN {Nproc, Ntype}) THEN err(80); y^.typ := OPT.inttyp END;
		IF x^.typ^.comp = Array THEN typ := x^.typ^.BaseTyp;
			IF (y^.class = Nconst) & ((y^.conval^.intval < 0) OR (y^.conval^.intval >= x^.typ^.n)) THEN err(81) END
		ELSIF x^.typ^.comp = DynArr THEN typ := x^.typ^.BaseTyp;
			IF (y^.class = Nconst) & (y^.conval^.intval < 0) THEN err(81) END
		ELSE err(82); typ := OPT.undftyp
		END;
		BindNodes(Nindex, typ, x, y); x^.readonly := x^.left^.readonly
	END Index;

	PROCEDURE Field* (VAR x: OPT.Node; y: OPT.Object);
	BEGIN (*x^.typ^.comp = Record*)
		IF x^.class >= Nconst THEN err(77) END;
		IF (y # NIL) & (y^.mode IN {Fld, TProc}) THEN
			BindNodes(Nfield, y^.typ, x, NIL); x^.obj := y;
			x^.readonly := x^.left^.readonly OR ((y^.vis = externalR) & (y^.mnolev < 0))
		ELSE err(83); x^.typ := OPT.undftyp
		END
	END Field;

	PROCEDURE TypTest* (VAR x: OPT.Node; obj: OPT.Object; guard: BOOLEAN);

		PROCEDURE GTT (t0, t1: OPT.Struct);
			VAR node: OPT.Node;
		BEGIN
			IF (t0 # NIL) & OPT.SameType(t0, t1) & (guard OR (x^.class # Nguard)) THEN
				IF ~guard THEN x := NewBoolConst(TRUE) END
			ELSIF (t0 = NIL) OR OPT.Extends(t1, t0) OR (t0.form = Undef (*SYSTEM.PTR*)) THEN
				IF guard THEN BindNodes(Nguard, NIL, x, NIL); x^.readonly := x^.left^.readonly
				ELSE node := OPT.NewNode(Nmop); node^.subcl := is; node^.left := x;
					node^.obj := obj; x := node
				END
			ELSE err(85)
			END
		END GTT;

	BEGIN
		IF NotVar(x) THEN err(112)
		ELSIF x^.typ^.form = Pointer THEN
			IF (x^.typ^.BaseTyp^.comp # Record) & (x^.typ # OPT.sysptrtyp) THEN err(85)
			ELSIF obj^.typ^.form = Pointer THEN GTT(x^.typ^.BaseTyp, obj^.typ^.BaseTyp)
			ELSE err(86)
			END
		ELSIF (x^.typ^.comp = Record) & (x^.class = Nvarpar) & (x^.obj^.vis # outPar) & (obj^.typ^.comp = Record) THEN
			GTT(x^.typ, obj^.typ)
		ELSE err(87)
		END;
		IF guard THEN x^.typ := obj^.typ ELSE x^.typ := OPT.booltyp END
	END TypTest;

	PROCEDURE In* (VAR x: OPT.Node; y: OPT.Node);
		VAR f: SHORTINT; k: LONGINT;
	BEGIN f := x^.typ^.form;
		IF (x^.class = Ntype) OR (x^.class = Nproc) OR (y^.class = Ntype) OR (y^.class = Nproc) THEN err(126)
		ELSIF (f IN intSet) & (y^.typ^.form = Set) THEN
			IF x^.class = Nconst THEN
				k := x^.conval^.intval;
				IF (k < 0) OR (k > OPM.MaxSet) THEN err(202)
				ELSIF y^.class = Nconst THEN x^.conval^.intval := BoolToInt(k IN y^.conval^.setval); x^.obj := NIL
				ELSE BindNodes(Ndop, OPT.booltyp, x, y); x^.subcl := in
				END
			ELSE BindNodes(Ndop, OPT.booltyp, x, y); x^.subcl := in
			END
		ELSE err(92)
		END;
		x^.typ := OPT.booltyp
	END In;

	PROCEDURE log (x: LONGINT): LONGINT;
	BEGIN exp := 0;
		IF x > 0 THEN
			WHILE ~ODD(x) DO x := x DIV 2; INC(exp) END
		END;
		RETURN x
	END log;

	PROCEDURE CheckRealType (f: INTEGER; nr: SHORTINT; x: OPT.Const);
		VAR min, max, r: REAL;
	BEGIN
		IF f = Real32 THEN min := OPM.MinReal32; max := OPM.MaxReal32
		ELSE min := OPM.MinReal64; max := OPM.MaxReal64
		END;
		r := ABS(x^.realval);
		IF ((r > max) OR (r < min)) & (r # OPM.InfReal) THEN err(nr); x^.realval := 1.0
		ELSIF f = Real32 THEN x^.realval := SHORT(x^.realval)	(* single precision only *)
		END;
		x^.intval := OPM.ConstNotAlloc
	END CheckRealType;

	PROCEDURE GetRealConstType (f: INTEGER; nr: SHORTINT; x: OPT.Const; VAR typ: OPT.Struct);
	BEGIN
		IF (OPM.Lang = "C") & (f IN realSet) THEN	(* SR *)
			typ := OPT.lrltyp; f := Real64
		END;
		CheckRealType(f, nr, x)
	END GetRealConstType;

	PROCEDURE^ Convert (VAR x: OPT.Node; typ: OPT.Struct);

	PROCEDURE MOp* (op: BYTE; VAR x: OPT.Node);
		VAR f: INTEGER; typ: OPT.Struct; z: OPT.Node;

		PROCEDURE NewOp (op: BYTE; typ: OPT.Struct; z: OPT.Node): OPT.Node;
			VAR node: OPT.Node;
		BEGIN
			node := OPT.NewNode(Nmop); node^.subcl := op; node^.typ := typ;
			node^.left := z; RETURN node
		END NewOp;

	BEGIN z := x;
		IF ((z^.class = Ntype) OR (z^.class = Nproc)) & (op # adr) & (op # typfn) THEN err(126)
		ELSE typ := z^.typ; f := typ^.form;
			CASE op OF
			  not:
					IF f = Bool THEN
						IF z^.class = Nconst THEN
							z^.conval^.intval := BoolToInt(~IntToBool(z^.conval^.intval)); z^.obj := NIL
						ELSE z := NewOp(op, typ, z)
						END
					ELSE err(98)
					END
			| plus:
					IF ~(f IN intSet + realSet) THEN err(96) END
			| minus:
					IF f IN intSet + realSet +{Set}THEN
						IF z^.class = Nconst THEN
							IF f IN intSet THEN
								IF z^.conval^.intval = MIN(LONGINT) THEN err(203)
								ELSE z^.conval^.intval := -z^.conval^.intval; SetIntType(z, FALSE)
								END
							ELSIF f IN realSet THEN z^.conval^.realval := -z^.conval^.realval
							ELSE z^.conval^.setval := -z^.conval^.setval
							END;
							z^.obj := NIL
						ELSE
							IF (OPM.Lang = "C") & (f < Int32) THEN
								IF OPM.AdrSize = 2 THEN Convert(z, OPT.sinttyp) ELSE Convert(z, OPT.inttyp) END
							END;
							z := NewOp(op, z^.typ, z)
						END
					ELSE err(97)
					END
			| abs:
					IF f IN intSet + realSet THEN
						IF z^.class = Nconst THEN
							IF f IN intSet THEN
								IF z^.conval^.intval = MIN(LONGINT) THEN err(203)
								ELSE z^.conval^.intval := ABS(z^.conval^.intval); SetIntType(z, FALSE)
								END
							ELSE z^.conval^.realval := ABS(z^.conval^.realval)
							END;
							z^.obj := NIL
						ELSE
							IF (OPM.Lang = "C") & (f < Int32) THEN
								IF OPM.AdrSize = 2 THEN Convert(z, OPT.sinttyp) ELSE Convert(z, OPT.inttyp) END
							END;
							z := NewOp(op, z^.typ, z)
						END
					ELSE err(111)
					END
			| cap:
					IF f IN charSet THEN
						IF z^.class = Nconst THEN
							IF ODD(z.conval.intval DIV 32) THEN DEC(z.conval.intval, 32) END;
							z^.obj := NIL
						ELSE z := NewOp(op, typ, z)
						END
					ELSE err(111); z^.typ := OPT.char8typ
					END
			| odd:
					IF f IN intSet THEN
						IF z^.class = Nconst THEN
							z^.conval^.intval := BoolToInt(ODD(z^.conval^.intval)); z^.obj := NIL
						ELSE z := NewOp(op, typ, z)
						END
					ELSE err(111)
					END;
					z^.typ := OPT.booltyp
			| adr: (*SYSTEM.ADR*)
					IF (z^.class = Nconst) & (f = Char8) & (z^.conval^.intval >= 20H) THEN
						CharToString8(z); f := String8
					ELSIF (z^.class = Nconst) & (f = Char16) & (z^.conval^.intval >= 20H) THEN
						CharToString16(z); f := String16
					END;
					IF (z.class = Nproc) & (z.obj.mode # CProc) THEN
						IF z.obj.mnolev > 0 THEN err(73)
						ELSIF z.obj.mode = LProc THEN z.obj.mode := XProc
						END;
						z := NewOp(op, typ, z)
					ELSIF z.class = Ntype THEN
						IF (z.obj.typ.sysflag # 0) OR (typ.comp = Basic) & (f # Pointer) THEN err(111) END;
						z := NewOp(op, typ, z)
					ELSIF (z^.class < Nconst) OR (z.class = Nconst) & (f = String8) THEN
						z := NewOp(op, typ, z)
					ELSE err(127)
					END;
					CASE OPM.AdrSize OF 4: z^.typ := OPT.inttyp | 8: z^.typ := OPT.linttyp
					ELSE z^.typ := OPT.sinttyp
					END
			| typfn: (*TYP*)
				z := NewOp(op, typ, z);
				CASE OPM.AdrSize OF 4: z.typ := OPT.inttyp | 8: z.typ := OPT.linttyp ELSE z.typ := OPT.sinttyp
				END
			| unsgn:	(* (unsigned) for div *)
					IF f IN intSet THEN z := NewOp(op, typ, z) ELSE err(127) END
			END
		END;
		x := z
	END MOp;

	PROCEDURE CheckPtr (x, y: OPT.Node);
		VAR g: SHORTINT; p, q, t: OPT.Struct;
	BEGIN g := y^.typ^.form;
		IF g = Pointer THEN
			p := x^.typ^.BaseTyp; q := y^.typ^.BaseTyp;
			IF (p^.comp = Record) & (q^.comp = Record) THEN
				IF p^.extlev < q^.extlev THEN t := p; p := q; q := t END;
				WHILE (p # q) & (p # NIL) & (p # OPT.undftyp) DO p := p^.BaseTyp END;
				IF p = NIL THEN err(100) END
			ELSE err(100)
			END
		ELSIF g # NilTyp THEN err(100)
		END
	END CheckPtr;

	PROCEDURE Promote (VAR left, right: OPT.Node; op: INTEGER);	(* check expression compatibility *)
		VAR f, g: INTEGER; new: OPT.Struct;
	BEGIN
		f := left^.typ^.form; g := right^.typ^.form; new := left^.typ;
		IF (OPM.Lang # "3") & (f IN intSet + realSet) THEN
			IF g IN intSet + realSet THEN
				IF (f = Real32) & (right^.class = Nconst) & (g IN realSet) & (left^.class # Nconst)
				OR (g = Real32) & (left^.class = Nconst) & (f IN realSet) & (right^.class # Nconst) THEN
					new := OPT.realtyp	(* SR *)
				ELSIF (f = Real64) OR (g = Real64) THEN new := OPT.lrltyp
				ELSIF (f = Real32) OR (g = Real32) THEN new := OPT.realtyp	(* SR *)
				ELSIF (op = slash) & (OPM.Lang # "7") THEN new := OPT.lrltyp
				ELSIF (f = Int64) OR (g = Int64) THEN new := OPT.linttyp
				ELSIF (f = Int32) OR (g = Int32) THEN new := OPT.inttyp
				ELSIF OPM.AdrSize = 2 THEN new := OPT.sinttyp
				ELSE new := OPT.inttyp
				END
			ELSE err(100)
			END;
			IF g # new^.form THEN Convert(right, new) END;
			IF f # new^.form THEN Convert(left, new) END
		ELSIF (OPM.Lang # "7") & (f IN charSet + {String8, String16}) THEN
			IF g IN charSet + {String8, String16} THEN
				IF (f = String16) OR (g = String16) OR (f = Char16) & (g = String8) OR (f = String8) & (g = Char16) THEN
					new := OPT.string16typ
				ELSIF ((f = Char16) OR (g = Char16)) & (op # plus) THEN new := OPT.char16typ
				ELSIF (f = String8) OR (g = String8) THEN new := OPT.string8typ
				ELSIF op = plus THEN
					IF (f = Char16) OR (g = Char16) THEN new := OPT.string16typ
					ELSE new := OPT.string8typ
					END
				END;
				IF (new.form IN {String8, String16})
					& ((f IN charSet) & (left.class # Nconst) OR (g IN charSet) & (right.class # Nconst))
				THEN
					err(100)
				END
			ELSE err(100)
			END;
			IF g # new^.form THEN Convert(right, new) END;
			IF f # new^.form THEN Convert(left, new) END
		END
	END Promote;

	PROCEDURE PromoteByte (VAR left, right: OPT.Node);	(* cast SYSTEM.BYTE to other types *)
		VAR f, g: INTEGER; new: OPT.Struct;
	BEGIN
		f := left^.typ^.form; g := right^.typ^.form; new := left^.typ;
		IF (f IN intSet + realSet) & (g IN intSet + realSet) THEN
			IF (f = Real64) OR (g = Real64) THEN new := OPT.lrltyp
			ELSIF (f = Real32) OR (g = Real32) THEN new := OPT.realtyp
			ELSIF (f = Int64) OR (g = Int64) THEN new := OPT.linttyp
			ELSIF (f = Int32) OR (g = Int32) THEN new := OPT.inttyp
			ELSIF (OPM.AdrSize = 2) OR (OPM.Lang <= "3") THEN new := OPT.sinttyp
			ELSE new := OPT.inttyp
			END;
			IF g # new^.form THEN right^.typ := new END;
			IF f # new^.form THEN left^.typ := new END
		END
	END PromoteByte;

	PROCEDURE CheckParameters* (fp, ap: OPT.Object; checkNames: BOOLEAN); (* checks par list match *)
		VAR ft, at: OPT.Struct;
	BEGIN
		WHILE fp # NIL DO
			IF ap # NIL THEN
				ft := fp^.typ; at := ap^.typ;
				IF ~OPT.EqualType(ft, at)
					OR (fp^.mode # ap^.mode) OR (fp^.sysflag # ap^.sysflag) OR (fp^.vis # ap^.vis)
					OR checkNames & (fp^.name^ # ap^.name^) THEN err(115) END;
				ap := ap^.link
			ELSE err(116)
			END;
			fp := fp^.link
		END;
		IF ap # NIL THEN err(116) END
	END CheckParameters;

	PROCEDURE CheckProc (x: OPT.Struct; y: OPT.Object);	(* proc var x := proc y, check compatibility *)
	BEGIN
		IF (y^.mode IN {XProc, IProc, LProc}) & (x^.sysflag = y^.sysflag) THEN
			IF y^.mode = LProc THEN
				IF y^.mnolev = 0 THEN y^.mode := XProc
				ELSE err(73)
				END
			END;
			IF OPT.EqualType(x^.BaseTyp, y^.typ) THEN CheckParameters(x^.link, y^.link, FALSE)
			ELSE err(117)
			END
		ELSE err(113)
		END
	END CheckProc;

	PROCEDURE ConstOp (op: SHORTINT; x, y: OPT.Node);
		VAR f, g, i, j: INTEGER; xval, yval: OPT.Const; ext: OPT.ConstExt; xv, yv: LONGINT;
				temp: BOOLEAN; (* temp avoids err 215 *)

		PROCEDURE ConstCmp (): SHORTINT;
			VAR res: SHORTINT;
		BEGIN
			CASE f OF
			  Undef:
					res := eql
			| Int8, Char8..Int64, Char16:
					IF xval^.intval < yval^.intval THEN res := lss
					ELSIF xval^.intval > yval^.intval THEN res := gtr
					ELSE res := eql
					END
			| Real32, Real64:
					IF xval^.realval < yval^.realval THEN res := lss
					ELSIF xval^.realval > yval^.realval THEN res := gtr
					ELSE res := eql
					END
			| Bool:
					IF xval^.intval # yval^.intval THEN res := neq
					ELSE res := eql
					END
			| Set:
					IF xval^.setval # yval^.setval THEN res := neq
					ELSE res := eql
					END
			| String8, String16:
					IF xval^.ext^ < yval^.ext^ THEN res := lss
					ELSIF xval^.ext^ > yval^.ext^ THEN res := gtr
					ELSE res := eql
					END
			| NilTyp, Pointer, ProcTyp:
					IF xval^.intval # yval^.intval THEN res := neq
					ELSE res := eql
					END
			END;
			x^.typ := OPT.booltyp; RETURN res
		END ConstCmp;

	BEGIN
		f := x^.typ^.form; g := y^.typ^.form; xval := x^.conval; yval := y^.conval;
		IF f # g THEN
			CASE f OF
			  Char8, Char16:
					IF g = String8 THEN CharToString8(x)
					ELSIF g = String16 THEN CharToString16(x)
					ELSE err(100); y^.typ := x^.typ; yval^ := xval^
					END
			| Int8:
					IF g IN intSet THEN x^.typ := y^.typ
					ELSIF g = Real32 THEN x^.typ := OPT.realtyp; xval^.realval := xval^.intval
					ELSIF g = Real64 THEN x^.typ := OPT.lrltyp; xval^.realval := xval^.intval
					ELSE err(100); y^.typ := x^.typ; yval^ := xval^
					END
			| Int16:
					IF g = Int8 THEN y^.typ := OPT.sinttyp
					ELSIF g IN intSet THEN x^.typ := y^.typ
					ELSIF g = Real32 THEN x^.typ := OPT.realtyp; xval^.realval := xval^.intval
					ELSIF g = Real64 THEN x^.typ := OPT.lrltyp; xval^.realval := xval^.intval
					ELSE err(100); y^.typ := x^.typ; yval^ := xval^
					END
			| Int32:
					IF g IN {Int8, Int16} THEN y^.typ := OPT.inttyp
					ELSIF g IN intSet THEN x^.typ := y^.typ
					ELSIF g = Real32 THEN x^.typ := OPT.realtyp; xval^.realval := xval^.intval
					ELSIF g = Real64 THEN x^.typ := OPT.lrltyp; xval^.realval := xval^.intval
					ELSE err(100); y^.typ := x^.typ; yval^ := xval^
					END
			| Int64:
					IF g IN intSet THEN y^.typ := OPT.linttyp
					ELSIF g = Real32 THEN x^.typ := OPT.realtyp; xval^.realval := xval^.intval
					ELSIF g = Real64 THEN x^.typ := OPT.lrltyp; xval^.realval := xval^.intval
					ELSE err(100); y^.typ := x^.typ; yval^ := xval^
					END
			| Real32:
					IF g IN intSet THEN y^.typ := x^.typ; yval^.realval := yval^.intval
					ELSIF g = Real64 THEN x^.typ := OPT.lrltyp
					ELSE err(100); y^.typ := x^.typ; yval^ := xval^
					END
			| Real64:
					IF g IN intSet THEN y^.typ := x^.typ; yval^.realval := yval^.intval
					ELSIF g = Real32 THEN y^.typ := OPT.lrltyp
					ELSE err(100); y^.typ := x^.typ; yval^ := xval^
					END
			| String8, String16:
					IF g = Char8 THEN CharToString8(y); g := String8
					ELSIF g = Char16 THEN CharToString16(y); g := String16
					ELSE err(100); y^.typ := x^.typ; yval^ := xval^
					END
			| NilTyp:
					IF ~(g IN {Pointer, ProcTyp}) THEN err(100) END
			| Pointer:
					CheckPtr(x, y)
			| ProcTyp:
					IF g # NilTyp THEN err(100) END
			ELSE err(100); y^.typ := x^.typ; yval^ := xval^
			END;
			f := x^.typ^.form
		END;	(* {x^.typ = y^.typ} *)
		CASE op OF
		  times:
				IF f IN intSet THEN xv := xval^.intval; yv := yval^.intval;
					IF (xv = 0) OR (yv = 0) OR	(* division with negative numbers is not defined *)
						(xv > 0) & (yv > 0) & (yv <= MAX(LONGINT) DIV xv) OR
						(xv > 0) & (yv < 0) & (yv >= MIN(LONGINT) DIV xv) OR
						(xv < 0) & (yv > 0) & (xv >= MIN(LONGINT) DIV yv) OR
						(xv < 0) & (yv < 0) & (xv # MIN(LONGINT)) & (yv # MIN(LONGINT)) & (-xv <= MAX(LONGINT) DIV (-yv)) THEN
						xval^.intval := xv * yv; SetIntType(x, FALSE)
					ELSE err(204)
					END
				ELSIF f IN realSet THEN
					temp := ABS(yval^.realval) <= 1.0;
					IF temp OR (ABS(xval^.realval) <= OPM.MaxReal64 / ABS(yval^.realval)) THEN
						xval^.realval := xval^.realval * yval^.realval; GetRealConstType(f, 204, xval, x^.typ)
					ELSE err(204)
					END
				ELSIF f = Set THEN
					xval^.setval := xval^.setval * yval^.setval
				ELSIF f # Undef THEN err(101)
				END
		| slash:
				IF f IN intSet THEN
					IF yval^.intval # 0 THEN
						xval^.realval := xval^.intval / yval^.intval; GetRealConstType(Real32, 205, xval, x^.typ)
					ELSE err(205); xval^.realval := 1.0
					END;
					x^.typ := OPT.realtyp
				ELSIF f IN realSet THEN
					temp := ABS(yval^.realval) >= 1.0;
					IF temp OR (ABS(xval^.realval) <= OPM.MaxReal64 * ABS(yval^.realval)) THEN
						xval^.realval := xval^.realval / yval^.realval; GetRealConstType(f, 205, xval, x^.typ)
					ELSE err(205)
					END
				ELSIF f = Set THEN
					xval^.setval := xval^.setval / yval^.setval
				ELSIF f # Undef THEN err(102)
				END
		| div:
				IF f IN intSet THEN
					IF (xval^.intval = MIN(LONGINT)) & (yval^.intval = -1) THEN err(204)
					ELSIF yval^.intval # 0 THEN
						xval^.intval := xval^.intval DIV yval^.intval; SetIntType(x, FALSE)
					ELSE err(205)
					END
				ELSIF f # Undef THEN err(103)
				END
		| div0:
				IF ~(OPM.gpcp IN OPM.opt) THEN err(-303) END;
				IF f IN intSet THEN
					IF (xval^.intval = MIN(LONGINT)) & (yval^.intval = -1) THEN err(204)
					ELSIF yval^.intval # 0 THEN
						xval^.intval := OPM.Div0(xval^.intval, yval^.intval); SetIntType(x, FALSE)
					ELSE err(205)
					END
				ELSIF f # Undef THEN err(140)
				END
		| mod:
				IF f IN intSet THEN
					IF yval^.intval # 0 THEN
						xval^.intval := xval^.intval MOD yval^.intval; SetIntType(x, FALSE)
					ELSE err(205)
					END
				ELSIF f # Undef THEN err(104)
				END
		| rem0:
				IF ~(OPM.gpcp IN OPM.opt) THEN err(-303) END;
				IF f IN intSet THEN
					IF yval^.intval # 0 THEN
						xval^.intval := OPM.Rem0(xval^.intval, yval^.intval); SetIntType(x, FALSE)
					ELSE err(205)
					END
				ELSIF f # Undef THEN err(141)
				END
		| and:
				IF f = Bool THEN
					xval^.intval := BoolToInt(IntToBool(xval^.intval) & IntToBool(yval^.intval))
				ELSE err(94)
				END
		| plus:
				IF f IN intSet THEN
					temp := (yval^.intval >= 0) & (xval^.intval <= MAX(LONGINT) - yval^.intval);
					IF temp OR (yval^.intval < 0) & (xval^.intval >= MIN(LONGINT) - yval^.intval) THEN
							INC(xval^.intval, yval^.intval); SetIntType(x, FALSE)
					ELSE err(206)
					END
				ELSIF f IN realSet THEN
					temp := (yval^.realval >= 0.0) & (xval^.realval <= OPM.MaxReal64 - yval^.realval);
					IF temp OR (yval^.realval < 0.0) & (xval^.realval >= OPM.MinReal64 - yval^.realval) THEN
							xval^.realval := xval^.realval + yval^.realval; GetRealConstType(f, 206, xval, x^.typ)
					ELSE err(206)
					END
				ELSIF f = Set THEN
					xval^.setval := xval^.setval + yval^.setval
				ELSIF ((OPM.Lang = "C") OR (OPM.Lang = "3")) & (f IN {String8, String16}) & (xval.ext # NIL) & (yval.ext # NIL) THEN
					NEW(ext, LEN(xval.ext^) + LEN(yval.ext^));
					i := 0; WHILE xval.ext[i] # 0X DO ext[i] := xval.ext[i]; INC(i) END;
					j := 0; WHILE yval.ext[j] # 0X DO ext[i] := yval.ext[j]; INC(i); INC(j) END;
					ext[i] := 0X; xval.ext := ext; INC(xval.intval2, yval.intval2 - 1)
				ELSIF f # Undef THEN err(105)
				END
		| minus:
				IF f IN intSet THEN
					IF (yval^.intval >= 0) & (xval^.intval >= MIN(LONGINT) + yval^.intval) OR
						(yval^.intval < 0) & (xval^.intval <= MAX(LONGINT) + yval^.intval) THEN
							DEC(xval^.intval, yval^.intval); SetIntType(x, FALSE)
					ELSE err(207)
					END
				ELSIF f IN realSet THEN
					temp := (yval^.realval >= 0.0) & (xval^.realval >= OPM.MinReal64 + yval^.realval);
					IF temp OR (yval^.realval < 0.0) & (xval^.realval <= OPM.MaxReal64 + yval^.realval) THEN
							xval^.realval := xval^.realval - yval^.realval; GetRealConstType(f, 207, xval, x^.typ)
					ELSE err(207)
					END
				ELSIF f = Set THEN
					xval^.setval := xval^.setval - yval^.setval
				ELSIF f # Undef THEN err(106)
				END
		| min:
				IF f IN intSet + realSet + {Char8} THEN
					IF ConstCmp() = gtr THEN xval^ := yval^ END;
					x^.typ := y^.typ
				ELSIF f # Undef THEN err(111)
				END
		| max:
				IF f IN intSet + realSet + {Char8} THEN
					IF ConstCmp() = lss THEN xval^ := yval^ END;
					x^.typ := y^.typ
				ELSIF f # Undef THEN err(111)
				END
		| or:
				IF f = Bool THEN
					xval^.intval := BoolToInt(IntToBool(xval^.intval) OR IntToBool(yval^.intval))
				ELSE err(95)
				END
		| eql:
				xval^.intval := BoolToInt(ConstCmp() = eql)
		| neq:
				xval^.intval := BoolToInt(ConstCmp() # eql)
		| lss:
				IF f IN {Bool, Set, NilTyp, Pointer} THEN err(108)
				ELSE xval^.intval := BoolToInt(ConstCmp() = lss)
				END
		| leq:
				IF f IN {Bool, Set, NilTyp, Pointer} THEN err(108)
				ELSE xval^.intval := BoolToInt(ConstCmp() # gtr)
				END
		| gtr:
				IF f IN {Bool, Set, NilTyp, Pointer} THEN err(108)
				ELSE xval^.intval := BoolToInt(ConstCmp() = gtr)
				END
		| geq:
				IF f IN {Bool, Set, NilTyp, Pointer} THEN err(108)
				ELSE xval^.intval := BoolToInt(ConstCmp() # lss)
				END
		END
	END ConstOp;

	PROCEDURE Convert (VAR x: OPT.Node; typ: OPT.Struct);
		VAR node: OPT.Node; f, g: INTEGER; k: LONGINT; r: REAL;
	BEGIN f := x^.typ^.form; g := typ^.form;
		IF x^.class = Nconst THEN
			IF f IN intSet THEN
				IF g = Set THEN x^.conval^.setval := IntToSet(SHORT(x^.conval^.intval))
				ELSIF g IN intSet THEN
					IF f > g THEN SetIntType(x, TRUE);
						IF (g # Int8) & (x^.typ^.form > g) OR
							(g = Int8) & ((x^.conval^.intval < MIN(BYTE)) OR (x^.conval^.intval > MAX(BYTE)))
						THEN err(203); x^.conval^.intval := 1
						END
					END
				ELSIF g IN realSet THEN x^.conval^.realval := x^.conval^.intval; x^.conval^.intval := OPM.ConstNotAlloc
				ELSE (*g = Char*) k := x^.conval^.intval;
					IF (0 > k) OR (k > 0FFH) THEN err(220) END
				END
			ELSIF f IN realSet THEN
				IF g IN realSet THEN CheckRealType(g, 203, x^.conval)
				ELSE (*g = Int64*)
					r := x^.conval^.realval;
					IF (r < MIN(LONGINT)) OR (r > MAX(LONGINT)) THEN err(203); r := 1 END;
					x^.conval^.intval := ENTIER(r); SetIntType(x, FALSE)
				END
			ELSIF f = Set THEN
				x^.conval^.intval := SetToInt(x^.conval^.setval); x^.conval^.realval := 0; x^.conval^.setval := {}
			ELSIF g = String8 THEN
				IF f = String16 THEN String16ToString8(x)
				ELSIF f IN charSet THEN CharToString8(x)
				ELSE typ := OPT.undftyp
				END
			ELSIF g = String16 THEN
				IF f = String8 THEN String8ToString16(x)
				ELSIF f IN charSet THEN CharToString16(x)
				ELSE typ := OPT.undftyp
				END
			ELSE	(* (f IN {Char, Byte}) & (g IN intSet) OR (f = Undef) *)
			END;
			x^.obj := NIL
		ELSIF (x^.class = Nmop) & (x^.subcl = conv) & ((x^.left^.typ^.form < f) OR (f > g)) THEN
			(* don't create new node *)
			IF x^.left^.typ = typ THEN (* and suppress existing node *) x := x^.left END
		ELSE node := OPT.NewNode(Nmop); node^.subcl := conv; node^.left := x; x := node
		END;
		x^.typ := typ
	END Convert;

	PROCEDURE Op* (op: BYTE; VAR x: OPT.Node; y: OPT.Node);
		VAR f, g: INTEGER; t, z: OPT.Node; typ: OPT.Struct; do: BOOLEAN; val: LONGINT;

		PROCEDURE NewOp (op: BYTE; typ: OPT.Struct; VAR x: OPT.Node; y: OPT.Node);
			VAR node: OPT.Node;
		BEGIN
			node := OPT.NewNode(Ndop); node^.subcl := op; node^.typ := typ;
			node^.left := x; node^.right := y; x := node
		END NewOp;

		PROCEDURE strings (VAR x, y: OPT.Node): BOOLEAN;
			VAR ok, xCharArr, yCharArr: BOOLEAN;
		BEGIN
			xCharArr := ((x^.typ^.comp IN {Array, DynArr}) & (x^.typ^.BaseTyp^.form=Char8)) OR (f=String8);
			yCharArr := (((y^.typ^.comp IN {Array, DynArr}) & (y^.typ^.BaseTyp^.form=Char8)) OR (g=String8));
			IF xCharArr & (g = Char8) & (y^.class = Nconst) THEN CharToString8(y); g := String8; yCharArr := TRUE END;
			IF yCharArr & (f = Char8) & (x^.class = Nconst) THEN CharToString8(x); f := String8; xCharArr := TRUE END;
			ok := xCharArr & yCharArr;
			IF ok THEN	(* replace ""-string compare with 0X-char compare, if possible *)
				IF (f=String8) & (x^.conval^.intval2 = 1) THEN	(* y is array of char *)
					x^.typ := OPT.char8typ; x^.conval^.intval := 0;
					Index(y, NewIntConst(0))
				ELSIF (g=String8) & (y^.conval^.intval2 = 1) THEN	(* x is array of char *)
					y^.typ := OPT.char8typ; y^.conval^.intval := 0;
					Index(x, NewIntConst(0))
				END
			END;
			RETURN ok
		END strings;


	BEGIN z := x;
		IF (z^.class = Ntype) OR (y^.class = Ntype) THEN err(126)
		ELSE
			IF (z^.typ^.form = Byte) OR (y^.typ^.form = Byte) THEN PromoteByte(z, y)
			ELSIF OPM.Lang > "2" THEN Promote(z, y, op)
			END;
			IF (z^.class = Nconst) & (y^.class = Nconst) THEN ConstOp(op, z, y); z^.obj := NIL
			ELSE
				IF z^.class = Nproc THEN	(* comparison of a procedure with a procedure variable *)
					IF y^.class = Nproc THEN err(126)
					ELSIF y^.typ^.form = ProcTyp THEN CheckProc(y^.typ, z^.obj)
					ELSE err(100)
					END
				ELSIF y^.class = Nproc THEN	(* comparison of a procedure variable with a procedure *)
					IF z^.class = Nproc THEN err(126)
					ELSIF z^.typ^.form = ProcTyp THEN CheckProc(z^.typ, y^.obj)
					ELSE err(100)
					END
				ELSE
					f := z^.typ^.form; g := y^.typ^.form;
					IF (z^.typ # y^.typ) & ~((f = g) & (f IN {Int8..Set, Char16})) THEN
						CASE f OF
						  Char8:
								IF z^.class = Nconst THEN CharToString8(z) ELSE err(100) END
						| Char16:
								IF z^.class = Nconst THEN CharToString16(z) ELSE err(100) END
						| Int8:
								IF g IN intSet + realSet THEN Convert(z, y^.typ)
								ELSE err(100)
								END
						| Int16:
								IF g = Int8 THEN Convert(y, z^.typ)
								ELSIF g IN intSet + realSet THEN Convert(z, y^.typ)
								ELSE err(100)
								END
						| Int32:
								IF g IN {Int8, Int16} THEN Convert(y, z^.typ)
								ELSIF g IN intSet + realSet THEN Convert(z, y^.typ)
								ELSE err(100)
								END
						| Int64:
								IF g IN intSet THEN Convert(y, z^.typ)
								ELSIF g IN realSet THEN Convert(z, y^.typ)
								ELSE err(100)
								END
						| Real32:
								IF g IN intSet THEN Convert(y, z^.typ)
								ELSIF g IN realSet THEN Convert(z, y^.typ)
								ELSE err(100)
								END
						| Real64:
								IF g IN intSet + realSet THEN Convert(y, z^.typ)
								ELSIF g IN realSet THEN Convert(y, z^.typ)
								ELSE err(100)
								END
						| NilTyp:
								IF ~(g IN {Pointer, ProcTyp}) THEN err(100) END
						| Pointer:
								CheckPtr(z, y)
						| ProcTyp:
								IF g # NilTyp THEN err(100) END
						| String8:
								IF (g = Char8) & (y^.class = Nconst) THEN CharToString8(y)
								ELSE err(100)
								END
						| String16:
								IF (g = Char16) & (y^.class = Nconst) THEN CharToString16(y)
								ELSE err(100)
								END
						| Comp:
								IF z^.typ^.comp = Record THEN err(100) END
						ELSE err(100)
						END
					END
				END;	(* {z^.typ = y^.typ} *)
				typ := z^.typ; f := typ^.form; g := y^.typ^.form;
				CASE op OF
				  times:
						do := TRUE;
						IF f IN intSet THEN
							IF z^.class = Nconst THEN val := z^.conval^.intval;
								IF val = 1 THEN do := FALSE; z := y
								ELSIF val = 0 THEN do := FALSE
								ELSIF log(val) = 1 THEN
									t := y; y := z; z := t;
									op := ash; y^.typ := OPT.bytetyp; y^.conval^.intval := exp; y^.obj := NIL
								END
							ELSIF y^.class = Nconst THEN val := y^.conval^.intval;
								IF val = 1 THEN do := FALSE
								ELSIF val = 0 THEN do := FALSE; z := y
								ELSIF log(val) = 1 THEN
									op := ash; y^.typ := OPT.bytetyp; y^.conval^.intval := exp; y^.obj := NIL
								END
							END
						ELSIF ~(f IN {Undef, Real32..Set}) THEN err(105); typ := OPT.undftyp
						END;
						IF do THEN NewOp(op, typ, z, y) END
				| slash:
						IF f IN intSet THEN
							IF (y^.class = Nconst) & (y^.conval^.intval = 0) THEN err(205) END;
							Convert(z, OPT.realtyp); Convert(y, OPT.realtyp);
							typ := OPT.realtyp
						ELSIF f IN realSet THEN
							IF (y^.class = Nconst) & (y^.conval^.realval = 0.0) THEN err(205) END
						ELSIF (f # Set) & (f # Undef) THEN err(102); typ := OPT.undftyp
						END;
						NewOp(op, typ, z, y)
				| div:
						do := TRUE;
						IF f IN intSet THEN
							IF y^.class = Nconst THEN val := y^.conval^.intval;
								IF val = 0 THEN err(205)
								ELSIF val = 1 THEN do := FALSE
								ELSIF log(val) = 1 THEN
									op := ash; y^.typ := OPT.bytetyp; y^.conval^.intval := -exp; y^.obj := NIL
								END
							END
						ELSIF f # Undef THEN err(103); typ := OPT.undftyp
						END;
						IF do THEN NewOp(op, typ, z, y) END
				| div0:
						IF ~(OPM.gpcp IN OPM.opt) THEN err(-303) END;
						do := TRUE;
						IF f IN intSet THEN
							IF y^.class = Nconst THEN val := y^.conval^.intval;
								IF val = 0 THEN err(205)
								ELSIF val = 1 THEN do := FALSE
								ELSIF log(val) = 1 THEN
									op := ash; y^.typ := OPT.bytetyp; y^.conval^.intval := -exp; y^.obj := NIL
								END
							END
						ELSIF f # Undef THEN err(140); typ := OPT.undftyp
						END;
						IF do THEN NewOp(op, typ, z, y) END
				| mod:
						IF f IN intSet THEN
							IF y^.class = Nconst THEN
								IF y^.conval^.intval = 0 THEN err(205)
								ELSIF log(y^.conval^.intval) = 1 THEN
									op := msk; y^.conval^.intval := ASH(LONG(-1), exp); y^.obj := NIL
								END
							END
						ELSIF f # Undef THEN err(104); typ := OPT.undftyp
						END;
						NewOp(op, typ, z, y)
				| rem0:
						IF ~(OPM.gpcp IN OPM.opt) THEN err(-303) END;
						IF f IN intSet THEN
							IF y^.class = Nconst THEN
								IF y^.conval^.intval = 0 THEN err(205)
								ELSIF log(y^.conval^.intval) = 1 THEN
									op := msk; y^.conval^.intval := ASH(LONG(-1), exp); y^.obj := NIL
								END
							END
						ELSIF f # Undef THEN err(141); typ := OPT.undftyp
						END;
						NewOp(op, typ, z, y)
				| and:
						IF f = Bool THEN
							IF z^.class = Nconst THEN
								IF IntToBool(z^.conval^.intval) THEN z := y END
							ELSIF (y^.class = Nconst) & IntToBool(y^.conval^.intval) THEN (* optimize z & TRUE -> z *)
					(*	ELSIF (y^.class = Nconst) & ~IntToBool(y^.conval^.intval) THEN
								don't optimize z & FALSE -> FALSE: side effects possible	*)
							ELSE NewOp(op, typ, z, y)
							END
						ELSIF f # Undef THEN err(94); z^.typ := OPT.undftyp
						END
				| plus:
						IF ~(f IN {Undef, Int8, Byte, Int16..String8, String16}) THEN err(105); typ := OPT.undftyp END;
						do := TRUE;
						IF f IN intSet THEN
							IF (z^.class = Nconst) & (z^.conval^.intval = 0) THEN do := FALSE; z := y END;
							IF (y^.class = Nconst) & (y^.conval^.intval = 0) THEN do := FALSE END
						ELSIF f IN {String8, String16} THEN
							IF (OPM.Lang # "C") & (OPM.Lang # "3") THEN err(105); typ := OPT.undftyp; do := FALSE
							ELSE
								IF (z.class = Nconst) & (z.conval.intval2 = 1) THEN do := FALSE; z := y END;
								IF (y.class = Nconst) & (y.conval.intval2 = 1) THEN do := FALSE END;
								IF do THEN
									IF z.class = Ndop THEN
										t := z; WHILE t.right.class = Ndop DO t := t.right END;
										IF (t.right.class = Nconst) & (y.class = Nconst) THEN
											ConstOp(op, t.right, y); do := FALSE
										ELSIF (t.right.class = Nconst) & (y.class = Ndop) & (y.left.class = Nconst) THEN
											ConstOp(op, t.right, y.left); y.left := t.right; t.right := y; do := FALSE
										ELSE
											NewOp(op, typ, t.right, y); do := FALSE
										END
									ELSE
										IF (z.class = Nconst) & (y.class = Ndop) & (y.left.class = Nconst) THEN
											ConstOp(op, z, y.left); y.left := z; z := y; do := FALSE
										END
									END
								END
							END
						END;
						IF do THEN NewOp(op, typ, z, y) END
				| minus:
						IF ~(f IN {Undef, Int8, Byte, Int16..Set}) THEN err(106); typ := OPT.undftyp END;
						IF ~(f IN intSet) OR (y^.class # Nconst) OR (y^.conval^.intval # 0) THEN NewOp(op, typ, z, y) END
				| min, max:
						IF ~(f IN {Undef} + intSet + realSet + charSet) THEN err(111); typ := OPT.undftyp END;
						NewOp(op, typ, z, y)
				| or:
						IF f = Bool THEN
							IF z^.class = Nconst THEN
								IF ~IntToBool(z^.conval^.intval) THEN z := y END
							ELSIF (y^.class = Nconst) & ~IntToBool(y^.conval^.intval) THEN (* optimize z OR FALSE -> z *)
					(*	ELSIF (y^.class = Nconst) & IntToBool(y^.conval^.intval) THEN
								don't optimize z OR TRUE -> TRUE: side effects possible	*)
							ELSE NewOp(op, typ, z, y)
							END
						ELSIF f # Undef THEN err(95); z^.typ := OPT.undftyp
						END
				| eql, neq:
						IF (f IN {Undef..Set, NilTyp, Pointer, ProcTyp, Char16}) OR strings(z, y) THEN typ := OPT.booltyp
						ELSE err(107); typ := OPT.undftyp
						END;
						NewOp(op, typ, z, y)
				| lss, leq, gtr, geq:
						IF (f IN {Undef, Int8, Char8..Real64, Char16}) OR strings(z, y) THEN typ := OPT.booltyp
						ELSE err(108); typ := OPT.undftyp
						END;
						NewOp(op, typ, z, y)
				END
			END
		END;
		x := z
	END Op;

	PROCEDURE SetRange* (VAR x: OPT.Node; y: OPT.Node);
		VAR k, l: LONGINT;
	BEGIN
		IF (x^.class = Ntype) OR (x^.class = Nproc) OR (y^.class = Ntype) OR (y^.class = Nproc) THEN err(126)
		ELSIF (x^.typ^.form IN intSet) & (y^.typ^.form IN intSet) THEN
			IF x^.class = Nconst THEN
				k := x^.conval^.intval;
				IF (0 > k) OR (k > OPM.MaxSet) THEN err(202) END
			END;
			IF y^.class = Nconst THEN
				l := y^.conval^.intval;
				IF (0 > l) OR (l > OPM.MaxSet) THEN err(202) END
			END;
			IF (x^.class = Nconst) & (y^.class = Nconst) THEN
				IF k <= l THEN
					x^.conval^.setval := {k..l}
				ELSE err(201); x^.conval^.setval := {l..k}
				END;
				x^.obj := NIL
			ELSE BindNodes(Nupto, OPT.settyp, x, y)
			END
		ELSE err(93)
		END;
		x^.typ := OPT.settyp
	END SetRange;

	PROCEDURE SetElem* (VAR x: OPT.Node);
		VAR k: LONGINT;
	BEGIN
		IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126)
		ELSIF ~(x^.typ^.form IN intSet) THEN err(93)
		ELSIF x^.class = Nconst THEN
			k := x^.conval^.intval;
			IF (0 <= k) & (k <= OPM.MaxSet) THEN x^.conval^.setval := {k}
			ELSE err(202)
			END;
			x^.obj := NIL
		ELSE BindNodes(Nmop, OPT.settyp, x, NIL); x^.subcl := bit
		END;
		x^.typ := OPT.settyp
	END SetElem;

	PROCEDURE CheckAssign* (x: OPT.Struct; VAR ynode: OPT.Node);	(* x := y *)
		VAR f, g: INTEGER; y, b: OPT.Struct;
	BEGIN
		y := ynode^.typ; f := x^.form; g := y^.form;
		IF (ynode^.class = Ntype) OR (ynode^.class = Nproc) & (f # ProcTyp) THEN err(126) END;
		CASE f OF
		  Undef, String8, String16:
		| Bool, Set:
				IF g # f THEN err(113) END
		| Char8, Char16:
			IF ~(g IN charSet) OR ~OPT.Includes(f, g) THEN err(113)
			ELSIF ynode.class = Nconst THEN Convert(ynode, x)
			END
		| Byte:
				IF g IN intSet THEN
					IF ynode^.class = Nconst THEN
						IF (ynode^.conval^.intval < 0) OR (ynode^.conval^.intval > 0FFH) THEN err(113) END
					ELSIF (OPM.Lang = "C") & (g # Byte) THEN err(113) END
				ELSIF g # Byte THEN err(113) END
		| Int8:
				IF (g IN intSet) & (ynode^.class = Nconst) &
					(MIN(BYTE) <= ynode^.conval^.intval) & (ynode^.conval^.intval <= MAX(BYTE)) THEN (* Ok *)
				ELSIF ~(g IN {Int8, Byte}) THEN err(113) END
		| Int16:
				IF (g IN intSet) & (ynode^.class = Nconst) &
					(MIN(SHORTINT) <= ynode^.conval^.intval) & (ynode^.conval^.intval <= MAX(SHORTINT)) THEN (* Ok *)
				ELSIF ~(g IN {Int8, Byte, Int16}) THEN err(113) END
		| Int32:
				IF (g IN intSet) & (ynode^.class = Nconst) &
					(MIN(INTEGER) <= ynode^.conval^.intval) & (ynode^.conval^.intval <= MAX(INTEGER)) THEN (* Ok *)
				ELSIF ~(g IN {Int8, Byte, Int16, Int32}) THEN err(113) END
		| Int64:
				IF ~(g IN intSet) THEN err(113) END
		| Real32:
				IF (ynode^.class = Nconst) & (g = Real64) & ((OPM.Lang = "C") OR (OPM.Lang = "3")) THEN
					IF ABS(ynode^.conval^.realval) > OPM.MaxReal32 THEN err(203); ynode^.conval^.realval := 0 END
				ELSIF ~(g IN {Int8, Byte, Int16..Real32}) THEN err(113) END
		| Real64:
				IF ~(g IN {Int8, Byte, Int16..Real64}) THEN err(113) END
		| Pointer:
				b := x^.BaseTyp;
				IF OPT.Extends(y, x) OR (g = NilTyp) OR (x = OPT.sysptrtyp) & (g = Pointer) THEN (* ok *)
				ELSIF (b^.comp = DynArr) & (b^.sysflag # 0) THEN	(* pointer to untagged open array *)
					IF ynode^.class = Nconst THEN CheckString(ynode, b, 113)
					ELSIF ~(y^.comp IN {Array, DynArr}) OR ~OPT.EqualType(b^.BaseTyp, y^.BaseTyp) THEN err(113)
					END
				ELSIF (b^.sysflag # 0) & (ynode^.class = Nmop) & (ynode^.subcl = adr) THEN	(* p := ADR(r) *)
					IF (b^.comp = DynArr) & (ynode^.left^.class = Nconst) THEN CheckString(ynode^.left, b, 113)
					ELSIF ~OPT.Extends(ynode^.left^.typ, b) THEN err(113)
					END
				ELSE err(113)
				END
		| ProcTyp:
				IF ynode^.class = Nproc THEN CheckProc(x, ynode^.obj)
				ELSIF OPT.EqualType(x, y) OR (g = NilTyp) THEN (* ok *)
				ELSE err(113)
				END
		| NoTyp, NilTyp:
				err(113)
		| Comp:
				x^.pvused := TRUE;	(* idfp of y guarantees assignment compatibility with x *)
				IF x^.comp = Record THEN
					IF (x = y) & (x^.attribute = extAttr) & (OPM.Lang # "C") THEN (* ok *)
					ELSIF ~OPT.EqualType(x, y) OR (x^.attribute # 0) THEN err(113)
					END
				ELSIF g IN {Char8, Char16, String8, String16} THEN
					IF (x.BaseTyp.form = Char16) & (g = String8) THEN Convert(ynode, OPT.string16typ)
				ELSE CheckString(ynode, x, 113);
				END;
					IF (x.comp = Array) & (ynode.class = Nconst) & (ynode.conval.intval2 > x.n) THEN
						err(114)
					END
				ELSIF x^.comp = Array THEN
					IF (ynode^.class = Nconst) & (g = Char8) THEN CharToString8(ynode); y := ynode^.typ; g := String8
					ELSIF (ynode^.class = Nconst) & (g = Char16) THEN CharToString16(ynode); y := ynode^.typ; g := String16
					END;
					IF x = y THEN (* ok *)
					ELSIF (OPM.Lang = "7") & (g = Comp) & (y^.comp = DynArr)
						& ((y^.BaseTyp = OPT.char8typ) & (x^.BaseTyp = OPT.char8typ)
						OR (y^.BaseTyp = OPT.char16typ) & (x^.BaseTyp = OPT.char16typ))
					THEN
						StrDeref(ynode)
					ELSIF (g = String8) & (x^.BaseTyp = OPT.char8typ)
						OR (g = String16) & (x^.BaseTyp = OPT.char16typ)
					THEN (* check length of string *)
						IF ynode^.conval^.intval2 > x^.n THEN err(114) END
					(* an array of characters is assigned to an array of characters *)
					ELSIF (g = Comp) & (y^.comp = Array)
						& ((y^.BaseTyp = OPT.char8typ) & (x^.BaseTyp = OPT.char8typ)
						OR (y^.BaseTyp = OPT.char16typ) & (x^.BaseTyp = OPT.char16typ))
						& (ynode^.left # NIL) & (ynode^.left^.obj # NIL)
						& (ynode^.left^.obj^.conval # NIL) & (ynode^.left^.obj^.conval^.arr # NIL) THEN
						 	IF y^.n > x^.n THEN err(114) END
					ELSE err(113)
					END
				ELSE (*DynArr*) err(113)
				END
		END;
		IF (ynode^.class = Nconst) & (g < f) & (g IN {Int8, Int16..Real32}) & (f IN {Int16..Real64}) THEN
			Convert(ynode, x)
		END
	END CheckAssign;

	PROCEDURE AssignString (VAR x: OPT.Node; str: OPT.Node);	(* x := str or x[0] := 0X *)
	BEGIN
		ASSERT((str^.class = Nconst) & (str^.typ^.form = String8));
		IF (x^.typ^.comp IN {Array, DynArr}) & (str^.conval^.intval2 = 1) THEN	(* x := "" -> x[0] := 0X *)
			Index(x, NewIntConst(0));
			str^.typ := x^.typ; str^.conval^.intval := 0; str^.obj := NIL
		END;
		BindNodes(Nassign, OPT.notyp, x, str); x^.subcl := assign
	END AssignString;

	PROCEDURE CheckLeaf (x: OPT.Node; dynArrToo: BOOLEAN);
	BEGIN
(*
avoid unnecessary intermediate variables in OFront

		IF (x^.class = Nmop) & (x^.subcl = val) THEN x := x^.left END;
		IF x^.class = Nguard THEN x := x^.left END;	(* skip last (and unique) guard *)
		IF (x^.class = Nvar) & (dynArrToo OR (x^.typ^.comp # DynArr)) THEN x^.obj^.leaf := FALSE END
*)
	END CheckLeaf;

	PROCEDURE StPar0* (VAR par0: OPT.Node; fctno: SHORTINT);	(* par0: first param of standard proc *)
		VAR f: INTEGER; typ: OPT.Struct; x: OPT.Node;
	BEGIN x := par0; f := x^.typ^.form;
		CASE fctno OF
		  haltfn: (*HALT*)
				IF (f IN intSet) & (x^.class = Nconst) THEN
					IF (OPM.MinHaltNr <= x^.conval^.intval) & (x^.conval^.intval <= OPM.MaxHaltNr) THEN
						BindNodes(Ntrap, OPT.notyp, x, x)
					ELSE err(218)
					END
				ELSE err(69)
				END;
				x^.typ := OPT.notyp
		| newfn: (*NEW*)
				typ := OPT.notyp;
				IF NotVar(x) THEN err(112)
				ELSIF f = Pointer THEN
					IF OPM.NEWusingAdr THEN CheckLeaf(x, TRUE) END;
					IF x^.readonly THEN err(76) END;
					f := x^.typ^.BaseTyp^.comp;
					IF f IN {Record, DynArr, Array} THEN
						IF f = DynArr THEN typ := x^.typ^.BaseTyp END;
						BindNodes(Nassign, OPT.notyp, x, NIL); x^.subcl := newfn
					ELSE err(111)
					END
				ELSE err(111)
				END;
				x^.typ := typ
		| absfn: (*ABS*)
				MOp(abs, x)
		| capfn: (*CAP*)
				MOp(cap, x)
		| ordfn: (*ORD*)
				IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126)
				ELSIF f = Bool THEN
					IF OPM.Lang = "3" THEN Convert(x, OPT.bytetyp)
					ELSIF OPM.Lang = "7" THEN Convert(x, OPT.inttyp)
					ELSE err(111)
					END
				ELSIF f = Char8 THEN
					IF (OPM.Lang = "7") & (OPM.AdrSize # 2) THEN Convert(x, OPT.inttyp)
					ELSE Convert(x, OPT.sinttyp)
					END
				ELSIF f = Char16 THEN Convert(x, OPT.inttyp)
				ELSIF (f = Set) & (OPM.Lang # "1") & (OPM.Lang # "2") THEN
					IF (OPM.Lang # "7") & (OPM.SetSize = 1) THEN Convert(x, OPT.bytetyp)
					ELSE Convert(x, OPT.inttyp)
					END
				ELSE err(111)
				END
		| bitsfn: (*BITS*)
				IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126)
				ELSIF f IN intSet THEN Convert(x, OPT.settyp)
				ELSE err(111)
				END
		| entierfn: (*ENTIER*)
				IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126)
				ELSIF f IN realSet THEN
					IF ((OPM.Lang = "C") OR (OPM.Lang = "3")) & (OPM.AdrSize # 2) THEN
						Convert(x, OPT.linttyp)
					ELSE (* "1", "2", "7" *)
						Convert(x, OPT.inttyp)
					END
				ELSE err(111); x^.typ := OPT.inttyp
				END
		| fltfn: (*FLT*)
				IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126)
				ELSIF f IN intSet THEN Convert(x, OPT.realtyp)
				ELSE err(111)
				END
		| oddfn: (*ODD*)
				MOp(odd, x)
		| minfn: (*MIN*)
				IF x^.class = Ntype THEN
					CASE f OF
					  Bool:  x := NewBoolConst(FALSE)
					| Char8:  x := NewIntConst(0); x^.typ := OPT.char8typ
					| Char16:  x := NewIntConst(0); x^.typ := OPT.char16typ
					| Byte: x := NewIntConst(0)
					| Int8:  x := NewIntConst(MIN(BYTE))
					| Int16:  x := NewIntConst(MIN(SHORTINT))
					| Int32:   x := NewIntConst(MIN(INTEGER))
					| Int64:  x := NewIntConst(MIN(LONGINT))
					| Set:   x := NewIntConst(0); x^.typ := OPT.inttyp
					| Real32:  x := NewRealConst(OPM.MinReal32, OPT.realtyp)
					| Real64: x := NewRealConst(OPM.MinReal64, OPT.lrltyp)
					ELSE err(111)
					END;
					x^.hint := 1
				ELSIF (OPM.Lang = "C") OR (OPM.Lang = "3") THEN
					IF ~(f IN intSet + realSet + charSet) THEN err(111) END
				ELSE err(110)
				END
		| maxfn: (*MAX*)
				IF x^.class = Ntype THEN
					CASE f OF
					  Bool:  x := NewBoolConst(TRUE)
					| Char8:  x := NewIntConst(0FFH); x^.typ := OPT.char8typ
					| Char16:  x := NewIntConst(0FFFFH); x^.typ := OPT.char16typ
					| Byte: x := NewIntConst(0FFH)
					| Int8:  x := NewIntConst(MAX(BYTE))
					| Int16:  x := NewIntConst(MAX(SHORTINT))
					| Int32:   x := NewIntConst(MAX(INTEGER))
					| Int64:  x := NewIntConst(MAX(LONGINT))
					| Set:   x := NewIntConst(OPM.MaxSet); x^.typ := OPT.inttyp
					| Real32:  x := NewRealConst(OPM.MaxReal32, OPT.realtyp)
					| Real64: x := NewRealConst(OPM.MaxReal64, OPT.lrltyp)
					ELSE err(111)
					END;
					x^.hint := 1
				ELSIF (OPM.Lang = "C") OR (OPM.Lang = "3") THEN
					IF ~(f IN intSet + realSet + charSet) THEN err(111) END
				ELSE err(110)
				END
		| chrfn: (*CHR*)
				IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126)
				ELSIF f IN {Undef} + intSet THEN Convert(x, OPT.char8typ)
				ELSE err(111); x^.typ := OPT.char8typ
				END
		| lchrfn: (*LCHR*)
				IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126)
				ELSIF f IN {Undef} + intSet THEN Convert(x, OPT.char16typ)
				ELSE err(111); x^.typ := OPT.char16typ
				END
		| shortfn: (*SHORT*)
				IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126)
				ELSE
					IF (OPM.Lang = "C") & (x.typ.comp IN {Array, DynArr}) & (x.typ.BaseTyp.form IN charSet) THEN
						StrDeref(x); f := x.typ.form
					END;
					IF f IN {Byte, Int16} THEN Convert(x, OPT.bytetyp)
					ELSIF f = Int32 THEN Convert(x, OPT.sinttyp)
					ELSIF f = Int64 THEN Convert(x, OPT.inttyp)
					ELSIF f = Real64 THEN Convert(x, OPT.realtyp)
					ELSIF f = Char16 THEN Convert(x, OPT.char8typ)
					ELSIF f = String16 THEN Convert(x, OPT.string8typ)
					ELSE err(111)
					END
				END
		| ushortfn: (*USHORT*)
				IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126)
				ELSE
					IF f IN intSet THEN Convert(x, OPT.ubytetyp) ELSE err(111) END
				END
		| longfn: (*LONG*)
				IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126)
				ELSE
					IF (OPM.Lang = "C") & (x.typ.comp IN {Array, DynArr}) & (x.typ.BaseTyp.form IN charSet) THEN
						StrDeref(x); f := x.typ.form
					END;
					IF f = Int8 THEN Convert(x, OPT.sinttyp)
					ELSIF f IN {Byte, Int16} THEN Convert(x, OPT.inttyp)
					ELSIF f = Int32 THEN Convert(x, OPT.linttyp)
					ELSIF f = Real32 THEN Convert(x, OPT.lrltyp)
					ELSIF f = Char8 THEN Convert(x, OPT.char16typ)
					ELSIF f = String8 THEN Convert(x, OPT.string16typ)
					ELSE err(111)
					END
				END
		| incfn, decfn: (*INC, DEC*)
				IF NotVar(x) THEN err(112)
				ELSIF ~(f IN intSet) THEN err(111)
				ELSIF x^.readonly THEN err(76)
				END
		| inclfn, exclfn: (*INCL, EXCL*)
				IF NotVar(x) THEN err(112)
				ELSIF x^.typ # OPT.settyp THEN err(111); x^.typ := OPT.settyp
				ELSIF x^.readonly THEN err(76)
				END
		| lenfn: (*LEN*)
				IF (* (x^.class = Ntype) OR *) (x^.class = Nproc) THEN err(126)	(* !!! *)
				ELSE
					IF (OPM.Lang = "C") & (x^.typ^.form = Pointer) THEN DeRef(x) END;
					IF (x^.class = Nconst) & (x^.typ^.form = Char8) THEN CharToString8(x)
					ELSIF (x^.class = Nconst) & (x^.typ^.form = Char16) THEN CharToString16(x)
					END;
					IF ~(x^.typ^.comp IN {DynArr, Array}) & ~(x^.typ^.form IN {String8, String16}) THEN err(131) END
				END
		| copyfn: (*COPY*)
				IF (x^.class = Nconst) & (f = Char8) THEN CharToString8(x); f := String8
				ELSIF (x^.class = Nconst) & (f = Char16) THEN CharToString16(x); f := String16
				END;
				IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126)
				ELSIF (~(x^.typ^.comp IN {DynArr, Array}) OR ~(x^.typ^.BaseTyp^.form IN charSet))
					 & ~(f IN {String8, String16}) THEN err(111)
				END
		| ashfn, asrfn, lslfn, rorfn: (*ASH, ASR, LSL, ROR*)
				IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126)
				ELSIF f IN intSet THEN
					IF f < Int32 THEN Convert(x, OPT.inttyp) END
				ELSE err(111); x^.typ := OPT.inttyp
				END
		| adrfn: (*SYSTEM.ADR*)
				CheckLeaf(x, FALSE); MOp(adr, x)
		| typfn: (*TYP*)
				CheckLeaf(x, FALSE);
				IF x.class = Ntype THEN
					IF x.typ.form = Pointer THEN x := NewLeaf(x.typ.BaseTyp.strobj) END;
					IF x.typ.comp # Record THEN err(111) END;
					MOp(adr, x)
				ELSE
					IF x.typ.form = Pointer THEN DeRef(x) END;
					IF x.typ.comp # Record THEN err(111) END;
					MOp(typfn, x)
				END
		| sizefn: (*SIZE*)
				IF x^.class # Ntype THEN err(110); x := NewIntConst(1)
				ELSIF (f IN {Int8..Set, Byte, Char16, Pointer, ProcTyp}) OR (x^.typ^.comp IN {Array, Record}) THEN
					typSize(x^.typ); x^.typ^.pvused := TRUE; x := NewIntConst(x^.typ^.size)
				ELSE err(111); x := NewIntConst(1)
				END
		| thisrecfn, (*THISREC*)
		  thisarrfn: (*THISARR*)
				IF (x.class = Ntype) OR (x.class = Nproc) THEN err(126)
				ELSE
					CASE OPM.AdrSize OF
					| 2: IF f IN {Int8, Byte} THEN Convert(x, OPT.sinttyp) ELSIF f # Int16 THEN err(111) END
					| 4: IF f IN {Int8, Int16, Byte} THEN Convert(x, OPT.inttyp) ELSIF f # Int32 THEN err(111) END
					ELSE
						IF f IN {Int8, Int16, Int32, Byte} THEN Convert(x, OPT.linttyp)
						ELSIF f # Int64 THEN err(111)
						END
					END
				END
		| lshfn, rotfn: (*SYSTEM.LSH, SYSTEM.ROT*)
				IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126)
				ELSIF ~(f IN intSet + charSet + {Int8, Set}) THEN err(111)
				END
		| getfn, putfn, bitfn, movefn: (*SYSTEM.GET, SYSTEM.PUT, SYSTEM.BIT, SYSTEM.MOVE*)
				IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126)
				ELSIF OPM.AdrSize = 4 THEN
					IF (x^.class = Nconst) & (f IN {Int8, Int16}) THEN Convert(x, OPT.inttyp)
					ELSIF ~(f IN {Int32, Pointer}) THEN err(111); x^.typ := OPT.inttyp
					END
				ELSIF OPM.AdrSize = 2 THEN
					IF (x^.class = Nconst) & (f = Int8) THEN Convert(x, OPT.sinttyp)
					ELSIF ~(f IN {Int16, Pointer}) THEN err(111); x^.typ := OPT.sinttyp
					END
				ELSE
					IF (x^.class = Nconst) & (f IN {Int8, Int16, Int32}) THEN Convert(x, OPT.linttyp)
					ELSIF ~(f IN {Int64, Pointer}) THEN err(111); x^.typ := OPT.linttyp
					END
				END
		| valfn: (*SYSTEM.VAL*)
				IF x^.class # Ntype THEN err(110)
				ELSIF (f IN {Undef, String8, String16, NoTyp, NilTyp}) OR (x^.typ^.comp = DynArr) THEN err(111)
				END
		| sysnewfn: (*SYSTEM.NEW*)
				IF NotVar(x) THEN err(112)
				ELSIF f = Pointer THEN
					IF OPM.NEWusingAdr THEN CheckLeaf(x, TRUE) END
				ELSE err(111)
				END
		| assertfn: (*ASSERT*)
				IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126); x := NewBoolConst(FALSE)
				ELSIF f # Bool THEN err(120); x := NewBoolConst(FALSE)
				ELSE MOp(not, x)
				END
		| packfn, unpkfn: (*PACK, UNPK*)
				IF NotVar(x) THEN err(112)
				ELSIF f # Real32 THEN err(111)
				ELSIF x^.readonly THEN err(76)
				END
		END;
		par0 := x
	END StPar0;

	PROCEDURE StPar1* (VAR par0: OPT.Node; x: OPT.Node; fctno: BYTE);	(* x: second parameter of standard proc *)
		VAR typ: OPT.Struct; p, t: OPT.Node; maxInt: LONGINT; f, L, maxExp: INTEGER;	(* max n in ASH(LONG(1), n) on this machine *)

		PROCEDURE NewOp (class, subcl: BYTE; left, right: OPT.Node): OPT.Node;
			VAR node: OPT.Node;
		BEGIN
			node := OPT.NewNode(class); node^.subcl := subcl;
			node^.left := left; node^.right := right; RETURN node
		END NewOp;

	BEGIN p := par0; f := x^.typ^.form;
		CASE fctno OF
		  incfn, decfn: (*INC, DEC*)
				IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126); p^.typ := OPT.notyp
				ELSE
					IF x^.typ # p^.typ THEN
						IF (x^.class = Nconst) & (f IN intSet) THEN Convert(x, p^.typ)
						ELSIF ~(f IN intSet) THEN err(111)
						ELSIF x^.typ^.size > p^.typ^.size THEN err(-301)
						END
					END;
					p := NewOp(Nassign, fctno, p, x);
					p^.typ := OPT.notyp
				END
		| inclfn, exclfn: (*INCL, EXCL*)
				IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126)
				ELSIF f IN intSet THEN
					IF (x^.class = Nconst) & ((0 > x^.conval^.intval) OR (x^.conval^.intval > OPM.MaxSet)) THEN err(202)
					END;
					p := NewOp(Nassign, fctno, p, x)
				ELSE err(111)
				END;
				p^.typ := OPT.notyp
		| lenfn: (*LEN*)
				IF (f IN intSet) & (x^.class = Nconst) THEN
					L := SHORT(x^.conval^.intval); typ := p^.typ;
					WHILE (L > 0) & (typ^.comp IN {DynArr, Array}) DO typ := typ^.BaseTyp; DEC(L) END;
					IF (L # 0) OR ~(typ^.comp IN {DynArr, Array}) THEN err(132)
					ELSE x^.obj := NIL;
						IF typ^.comp = DynArr THEN
							WHILE p^.class = Nindex DO p := p^.left; INC(x^.conval^.intval) END;	(* possible side effect ignored *)
							p := NewOp(Ndop, len, p, x);
							CASE OPM.IndexSize OF
							| 2: p^.typ := OPT.sinttyp
							| 4: p^.typ := OPT.inttyp
							ELSE p^.typ := OPT.linttyp
							END
						ELSE p := x; p^.conval^.intval := typ^.n; SetIntType(p, FALSE)
						END
					END
				ELSE err(69)
				END
		| copyfn: (*COPY*)
				IF NotVar(x) THEN err(112)
				ELSIF x^.readonly THEN err(76)
				ELSIF (x^.class = Nderef) & (x^.typ^.sysflag # 0) & (x^.typ^.n = 0) THEN err(137)
				ELSE
					CheckString(p, x^.typ, 111); t := x; x := p; p := t;
					IF (x^.class = Nconst) & (x^.typ^.form = String8) & (p^.typ^.comp = Array) & (x^.conval^.intval2 <= p^.typ^.n)
					THEN AssignString(p, x)
					ELSE p := NewOp(Nassign, copyfn, p, x)
					END
				END;
				p^.typ := OPT.notyp
		| ashfn: (*ASH*)
				IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126)
				ELSIF f IN intSet THEN
					IF p^.typ^.form = Int64 THEN maxInt := MAX(LONGINT) ELSE maxInt := MAX(INTEGER) END;
					maxExp := SHORT(log(maxInt DIV 2 + 1)); maxExp := exp;
					IF (x^.class = Nconst) & ((-maxExp - 1 > x^.conval^.intval) OR (x^.conval^.intval > maxExp)) THEN err(208)
					ELSIF (p^.class = Nconst) & (x^.class = Nconst) THEN
						IF x^.conval^.intval >= 0 THEN
							IF ABS(p^.conval^.intval) <= maxInt DIV ASH(LONG(1), x^.conval^.intval) THEN
								p^.conval^.intval := ASH(p^.conval^.intval, x^.conval^.intval)
							ELSE err(208); p^.conval^.intval := 1
							END
						ELSE p^.conval^.intval := ASH(p^.conval^.intval, x^.conval^.intval)
						END;
						p^.obj := NIL
					ELSE
						IF f = Int64 THEN Convert(x, OPT.inttyp) END;
						typ := p^.typ; p := NewOp(Ndop, ash, p, x); p^.typ := typ
					END
				ELSE err(111)
				END
		| asrfn: (*ASR*)
				IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126)
				ELSIF f IN intSet THEN
					IF p^.typ^.form = Int64 THEN maxExp := 63 ELSE maxExp := 31 END;
					IF (x^.class = Nconst) & ((x^.conval^.intval < 0) OR (x^.conval^.intval > maxExp)) THEN err(208)
					ELSIF (p^.class = Nconst) & (x^.class = Nconst) THEN
						p^.conval^.intval := ASH(p^.conval^.intval, -x^.conval^.intval);
						p^.obj := NIL
					ELSE
						IF f = Int64 THEN err(113); Convert(x, OPT.inttyp) END;
						typ := p^.typ; p := NewOp(Ndop, asr, p, x); p^.typ := typ
					END
				ELSE err(111)
				END
		| lslfn: (*LSL*)
				IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126)
				ELSIF f IN intSet THEN
					IF p^.typ^.form = Int64 THEN maxExp := 63 ELSE maxExp := 31 END;
					IF (x^.class = Nconst) & ((x^.conval^.intval < 0) OR (x^.conval^.intval > maxExp)) THEN err(210)
					ELSIF (p^.class = Nconst) & (x^.class = Nconst) THEN
						IF p^.typ^.form = Int64 THEN maxInt := p^.conval^.intval;
							p^.conval^.intval := OPM.Lsl(maxInt, SHORT(x^.conval^.intval));
							IF OPM.Lsr(SHORT(p^.conval^.intval), SHORT(x^.conval^.intval)) # maxInt THEN err(-210)
							END
						ELSE maxExp := SHORT(p^.conval^.intval);
							p^.conval^.intval := SYSTEM.LSH(maxExp, SHORT(x^.conval^.intval));
							IF SYSTEM.LSH(SHORT(p^.conval^.intval), -SHORT(x^.conval^.intval)) # maxExp THEN err(-210)
							END
						END;
						p^.obj := NIL
					ELSE
						IF f = Int64 THEN err(113); Convert(x, OPT.inttyp) END;
						typ := p^.typ; p := NewOp(Ndop, lsl, p, x); p^.typ := typ
					END
				ELSE err(111)
				END
		| rorfn: (*ROR*)
				IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126)
				ELSIF f IN intSet THEN
					IF p^.typ^.form = Int64 THEN maxExp := 63 ELSE maxExp := 31 END;
					IF (x^.class = Nconst) & ((x^.conval^.intval < 0) OR (x^.conval^.intval > maxExp)) THEN err(210)
					ELSIF (p^.class = Nconst) & (x^.class = Nconst) THEN
						maxExp := SHORT(x^.conval^.intval);
						IF p^.typ^.form = Int64 THEN p^.conval^.intval := OPM.Ror(p^.conval^.intval, maxExp)
						ELSE p^.conval^.intval := SYSTEM.ROT(SHORT(p^.conval^.intval), -maxExp) END;
						p^.obj := NIL
					ELSE
						IF f = Int64 THEN err(113); Convert(x, OPT.inttyp) END;
						typ := p^.typ; p := NewOp(Ndop, ror, p, x); p^.typ := typ
					END
				ELSE err(111)
				END
		| minfn: (*MIN*)
				IF ((OPM.Lang = "C") OR (OPM.Lang = "3")) & (p^.class # Ntype) THEN Op(min, p, x) ELSE err(64) END
		| maxfn: (*MAX*)
				IF ((OPM.Lang = "C") OR (OPM.Lang = "3")) & (p^.class # Ntype) THEN Op(max, p, x) ELSE err(64) END
		| newfn: (*NEW(p, x...)*)
				IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126)
				ELSIF p^.typ^.comp = DynArr THEN
					IF f IN intSet THEN
						IF (x^.class = Nconst) & ((x^.conval^.intval <= 0) OR (x^.conval^.intval > OPM.MaxIndex)) THEN err(63) END
					ELSE err(111)
					END;
					p^.right := x; p^.typ := p^.typ^.BaseTyp
				ELSE err(64)
				END
		| thisrecfn, (*THISREC*)
		  thisarrfn: (*THISARR*)
				IF (x.class = Ntype) OR (x.class = Nproc) THEN err(126)
				ELSE
					CASE OPM.AdrSize OF
					| 2: IF f IN {Int8, Byte} THEN Convert(x, OPT.sinttyp) ELSIF f # Int16 THEN err(111) END
					| 4: IF f IN {Int8, Int16, Byte} THEN Convert(x, OPT.inttyp) ELSIF f # Int32 THEN err(111) END
					ELSE
						IF f IN {Int8, Int16, Int32, Byte} THEN Convert(x, OPT.linttyp)
						ELSIF f # Int64 THEN err(111)
						END
					END;
					p := NewOp(Ndop, fctno, p, x); p.typ := OPT.undftyp
				END
		| lshfn, rotfn: (*SYSTEM.LSH, SYSTEM.ROT*)
				IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126)
				ELSIF ~(f IN intSet) THEN err(111)
				ELSE
					IF fctno = lshfn THEN p := NewOp(Ndop, lsh, p, x) ELSE p := NewOp(Ndop, rot, p, x) END;
					p^.typ := p^.left^.typ
				END
		| getfn, putfn: (*SYSTEM.GET, SYSTEM.PUT*)
				IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126)
				ELSIF f IN {Undef..Set, Byte, Char16, NilTyp, Pointer, ProcTyp} THEN
					IF fctno = getfn THEN
						IF NotVar(x) THEN err(112) END;
						t := x; x := p; p := t
					END;
					p := NewOp(Nassign, fctno, p, x)
				ELSE err(111)
				END;
				p^.typ := OPT.notyp
		| bitfn: (*SYSTEM.BIT*)
				IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126)
				ELSIF f IN intSet THEN
					p := NewOp(Ndop, bit, p, x)
				ELSE err(111)
				END;
				p^.typ := OPT.booltyp
		| valfn: (*SYSTEM.VAL*)	(* type is changed without considering the byte ordering on the target machine *)
				(* p (1st param): desired type *)
				(* x (2nd param): constant or value to be converted to p *)
				IF (x^.class = Ntype) OR (x^.class = Nproc) OR (f IN {Undef, String8, String16, NoTyp}) OR
					(x^.typ^.comp = DynArr) & (x^.typ^.sysflag = 0) THEN err(126)
				END;
				(* Warn if the result type includes memory past the end of the source variable *)
				OPT.typSize(x^.typ); OPT.typSize(p^.typ);
				IF (x^.class # Nconst) & (x^.typ^.size < p^.typ^.size) THEN err(-308) END;
				IF (x^.class = Nconst) & (x^.typ^.form IN intSet) & (p^.typ^.form IN intSet) THEN
					(* Convert integer constants in place allowing usage in CONST section *)
					IF (p^.typ^.size = 4) & (x^.conval^.intval >= 80000000L) & (x^.conval^.intval <= 0FFFFFFFFL) THEN
						DEC(x^.conval^.intval, 100000000L)
					ELSIF (p^.typ^.size = 2) & (x^.conval^.intval >= 8000H) & (x^.conval^.intval <= 0FFFFH) THEN
						DEC(x^.conval^.intval, 10000H)
					ELSIF (p^.typ^.size = 1) & (x^.conval^.intval >= 80H) & (x^.conval^.intval <= 0FFH) THEN
						DEC(x^.conval^.intval, 100H)
					END;
					Convert(x, p^.typ)
				ELSE
					t := OPT.NewNode(Nmop); t^.subcl := val; t^.left := x; x := t;
(*
					IF (x^.class >= Nconst) OR ((f IN realSet) # (p^.typ^.form IN realSet)) THEN
						t := OPT.NewNode(Nmop); t^.subcl := val; t^.left := x; x := t
					ELSE x^.readonly := FALSE
					END;
*)
					x^.typ := p^.typ;
				END;
				p := x
		| sysnewfn: (*SYSTEM.NEW*)
				IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126)
				ELSIF f IN intSet THEN
					p := NewOp(Nassign, sysnewfn, p, x)
				ELSE err(111)
				END;
				p^.typ := OPT.notyp
		| movefn: (*SYSTEM.MOVE*)
				IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126)
				ELSIF OPM.AdrSize = 4 THEN
					IF (x^.class = Nconst) & (f IN {Int8, Int16}) THEN Convert(x, OPT.inttyp)
					ELSIF ~(f IN {Int32, Pointer}) THEN err(111); x^.typ := OPT.inttyp
					END
				ELSIF OPM.AdrSize = 2 THEN
					IF (x^.class = Nconst) & (f = Int8) THEN Convert(x, OPT.sinttyp)
					ELSIF ~(f IN {Int16, Pointer}) THEN err(111); x^.typ := OPT.sinttyp
					END
				ELSE
					IF (x^.class = Nconst) & (f IN {Int8, Int16, Int32}) THEN Convert(x, OPT.linttyp)
					ELSIF ~(f IN {Int64, Pointer}) THEN err(111); x^.typ := OPT.linttyp
					END
				END;
				p^.link := x
		| assertfn: (*ASSERT*)
				IF (f IN intSet) & (x^.class = Nconst) THEN
					IF (OPM.MinHaltNr <= x^.conval^.intval) & (x^.conval^.intval <= OPM.MaxHaltNr) THEN
						BindNodes(Ntrap, OPT.notyp, x, x);
						x^.conval := OPT.NewConst(); x^.conval^.intval := OPM.errpos;
						Construct(Nif, p, x); p^.conval := OPT.NewConst(); p^.conval^.intval := OPM.errpos;
						Construct(Nifelse, p, NIL); OptIf(p);
						IF p = NIL THEN	(* ASSERT(TRUE) *)
						ELSIF (p^.class = Ntrap) & (OPM.Lang # "7") THEN err(99)
						ELSE p^.subcl := assertfn
						END
					ELSE err(218)
					END
				ELSE err(69)
				END
		| packfn: (*PACK*)
				IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126)
				ELSIF f IN intSet THEN p := NewOp(Nassign, packfn, p, x)
				ELSE err(111)
				END;
				p^.typ := OPT.notyp
		| unpkfn: (*UNPK*)
				IF NotVar(x) THEN err(112)
				ELSIF x^.readonly THEN err(76)
				ELSIF f IN intSet THEN p := NewOp(Nassign, unpkfn, p, x)
				ELSE err(111)
				END;
				p^.typ := OPT.notyp
		ELSE err(64)
		END;
		par0 := p
	END StPar1;

	PROCEDURE StParN* (VAR par0: OPT.Node; x: OPT.Node; fctno, n: SHORTINT);	(* x: n+1-th param of standard proc *)
		VAR node: OPT.Node; f: SHORTINT; p: OPT.Node;
	BEGIN p := par0; f := x^.typ^.form;
		IF fctno = newfn THEN (*NEW(p, ..., x...*)
			IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126)
			ELSIF p^.typ^.comp # DynArr THEN err(64)
			ELSIF f IN intSet THEN
				IF (x^.class = Nconst) & ((x^.conval^.intval <= 0) OR (x^.conval^.intval > OPM.MaxIndex)) THEN err(63) END;
				node := p^.right; WHILE node^.link # NIL DO node := node^.link END;
				node^.link := x; p^.typ := p^.typ^.BaseTyp
			ELSE err(111)
			END
		ELSIF (fctno = movefn) & (n = 2) THEN (*SYSTEM.MOVE*)
			IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126)
			ELSIF f IN intSet THEN
				node := OPT.NewNode(Nassign); node^.subcl := movefn; node^.right := p;
				node^.left := p^.link; p^.link := x; p := node
			ELSE err(111)
			END;
			p^.typ := OPT.notyp
		ELSE err(64)
		END;
		par0 := p
	END StParN;

	PROCEDURE StFct* (VAR par0: OPT.Node; fctno: BYTE; parno: SHORTINT);
		VAR dim: SHORTINT; x, p: OPT.Node;
	BEGIN p := par0;
		IF fctno <= ashfn THEN
			IF (fctno = newfn) & (p^.typ # OPT.notyp) THEN
				IF p^.typ^.comp = DynArr THEN err(65) END;
				p^.typ := OPT.notyp
			ELSIF (fctno = minfn) OR (fctno = maxfn) THEN
				IF (parno < 1) OR (parno = 1) & (p^.hint # 1) THEN err(65)
				ELSIF (p^.hint = 1) & (parno > 1) THEN err(64)
				END;
				p^.hint := 0
			ELSIF fctno <= sizefn THEN (* 1 param *)
				IF parno < 1 THEN err(65) END
			ELSE (* more than 1 param *)
				IF ((fctno = incfn) OR (fctno = decfn)) & (parno = 1) THEN (*INC, DEC*)
					BindNodes(Nassign, OPT.notyp, p, NewIntConst(1)); p^.subcl := fctno; p^.right^.typ := p^.left^.typ
				ELSIF (fctno = lenfn) & (parno = 1) THEN (*LEN*)
					IF p^.typ^.form IN {String8, String16} THEN
						IF p^.class = Nconst THEN p := NewIntConst(p^.conval^.intval2 - 1)
						ELSIF (p^.class = Ndop) & (p^.subcl = plus) THEN	(* propagate to leaf nodes *)
							StFct(p^.left, lenfn, 1); StFct(p^.right, lenfn, 1);
							CASE OPM.IndexSize OF
							| 2: p^.typ := OPT.sinttyp
							| 4: p^.typ := OPT.inttyp
							ELSE p^.typ := OPT.linttyp
							END
						ELSE
							WHILE (p^.class = Nmop) & (p^.subcl = conv) DO p := p^.left END;
							IF OPM.noerr THEN ASSERT(p^.class = Nderef) END;
							CASE OPM.IndexSize OF
							| 2: BindNodes(Ndop, OPT.sinttyp, p, NewIntConst(0));
							| 4: BindNodes(Ndop, OPT.inttyp, p, NewIntConst(0));
							ELSE BindNodes(Ndop, OPT.linttyp, p, NewIntConst(0));
							END;
							p^.subcl := len
						END
					ELSIF p^.typ^.comp = DynArr THEN dim := 0;
						WHILE p^.class = Nindex DO p := p^.left; INC(dim) END;	(* possible side effect ignored *)
						CASE OPM.IndexSize OF
						| 2: BindNodes(Ndop, OPT.sinttyp, p, NewIntConst(dim))
						| 4: BindNodes(Ndop, OPT.inttyp, p, NewIntConst(dim))
						ELSE BindNodes(Ndop, OPT.linttyp, p, NewIntConst(dim))
						END;
						p^.subcl := len
					ELSE
						p := NewIntConst(p^.typ^.n)
					END
				ELSIF parno < 2 THEN err(65)
				END
			END
		ELSIF fctno = assertfn THEN
			IF parno = 1 THEN x := NIL;
				BindNodes(Ntrap, OPT.notyp, x, NewIntConst(AssertTrap));
				x^.conval := OPT.NewConst(); x^.conval^.intval := OPM.errpos;
				Construct(Nif, p, x); p^.conval := OPT.NewConst(); p^.conval^.intval := OPM.errpos;
				Construct(Nifelse, p, NIL); OptIf(p);
				IF p = NIL THEN	(* ASSERT(TRUE) *)
				ELSIF (p^.class = Ntrap) & (OPM.Lang # "7") THEN err(99)
				ELSE p^.subcl := assertfn
				END
			ELSIF parno < 1 THEN err(65)
			END
		ELSE (*SYSTEM*)
			IF fctno = typfn THEN
				IF parno < 1 THEN err(65) END
			ELSIF (fctno = thisrecfn) OR (fctno = thisarrfn) THEN
				IF parno < 2 THEN err(65) END
			ELSIF (parno < 1) OR
				(fctno > adrfn) & (parno < 2) OR
				(fctno = movefn) & (parno < 3) THEN err(65)
			END
		END;
		par0 := p
	END StFct;

	PROCEDURE DynArrParCheck (ftyp, atyp: OPT.Struct; fvarpar: BOOLEAN);	(* check array compatibility *)
		VAR f: SHORTINT;
	BEGIN (* ftyp^.comp = DynArr *)
		IF ~ODD(ftyp^.sysflag) & ODD(atyp^.sysflag) & (atyp^.comp # Array) THEN err(137) END;
		f := atyp^.comp; ftyp := ftyp^.BaseTyp; atyp := atyp^.BaseTyp;
		IF fvarpar & (ftyp = OPT.ubytetyp) THEN (* ok, but ... *)
			IF ~(f IN {Array, DynArr}) OR ~(atyp^.form IN {Int8..Char8, Byte}) THEN
				IF OPM.Lang > "2" THEN err(67) ELSE err(-301) END (* ... warning 301 *)
			ELSIF ~OPT.EqualType(ftyp, atyp) THEN
				IF OPM.Lang > "2" THEN err(66) ELSE err(-301) END
			END
		ELSIF f IN {Array, DynArr} THEN
			IF ftyp^.comp = DynArr THEN DynArrParCheck(ftyp, atyp, fvarpar)
			ELSIF ~fvarpar & (ftyp.form = Pointer) & OPT.Extends(atyp, ftyp) THEN (* ok *)
			ELSIF ~OPT.EqualType(ftyp, atyp) THEN err(66)
			END
		ELSE err(67)
		END
	END DynArrParCheck;

	PROCEDURE CheckReceiver (VAR x: OPT.Node; fp: OPT.Object);
	BEGIN
		IF fp^.typ^.form = Pointer THEN
			IF x^.class = Nderef THEN x := x^.left (*undo DeRef*) ELSE (*x^.typ^.comp = Record*) err(71) END
		END
	END CheckReceiver;

	PROCEDURE PrepCall* (VAR x: OPT.Node; VAR fpar: OPT.Object);
	BEGIN
		IF (x^.obj # NIL) & (x^.obj^.mode IN {LProc, XProc, TProc, CProc}) THEN
			fpar := x^.obj^.link;
			IF x^.obj^.mode = TProc THEN CheckReceiver(x^.left, fpar); fpar := fpar^.link END
		ELSIF (x^.class # Ntype) & (x^.typ # NIL) & (x^.typ^.form = ProcTyp) THEN
			fpar := x^.typ^.link
		ELSE err(121); fpar := NIL; x^.typ := OPT.undftyp
		END
	END PrepCall;

	PROCEDURE Param* (ap: OPT.Node; fp: OPT.Object); (* checks parameter compatibilty *)
	BEGIN
		IF fp.typ.form # Undef THEN
			IF fp^.mode = VarPar THEN
				IF ODD(fp^.sysflag DIV nilBit) & (ap^.typ = OPT.niltyp) THEN (* ok *)
				ELSIF (fp^.typ^.comp = Record) & (fp^.typ^.sysflag = 0) & (ap^.class = Ndop) & (ap.subcl = thisrecfn) THEN (* ok *)
				ELSIF (fp^.typ^.comp = DynArr) & (fp^.typ^.sysflag = 0) & (fp^.typ^.n = 0) & (ap^.class = Ndop) & (ap.subcl = thisarrfn) THEN
					(* ok *)
				ELSE
					IF fp^.vis = inPar THEN
						IF ~NotVar(ap) THEN CheckLeaf(ap, FALSE) END
					ELSE
						IF NotVar(ap) THEN err(122)
						ELSE CheckLeaf(ap, FALSE)
						END;
						IF ap^.readonly THEN err(76) END;
					END;
					IF fp.typ.comp = DynArr THEN
						IF ap^.typ^.form IN charSet + {String8, String16} THEN CheckString(ap, fp^.typ, 67)
						ELSE DynArrParCheck(fp.typ, ap.typ, fp.vis # inPar) END
					ELSIF (fp^.typ = OPT.sysptrtyp) & (ap^.typ^.form = Pointer) THEN (* ok *)
					ELSIF (fp^.vis # outPar) & (fp^.typ^.comp = Record) & OPT.Extends(ap^.typ, fp^.typ) THEN (* ok *)
					ELSIF fp^.vis = inPar THEN CheckAssign(fp^.typ, ap)
					ELSIF ~OPT.EqualType(fp^.typ, ap^.typ) THEN err(123)
					ELSIF (fp^.typ^.form = Pointer) & (ap^.class = Nguard) THEN err(123)
					END
				END
			ELSIF fp^.typ^.comp = DynArr THEN
				IF (ap^.class = Nconst) & (ap^.typ^.form = Char8) THEN CharToString8(ap)
				ELSIF (ap^.class = Nconst) & (ap^.typ^.form = Char16) THEN CharToString16(ap)
				END;
				IF (ap^.typ^.form = String8) & (fp^.typ^.BaseTyp^.form = Char8)
					OR (ap^.typ^.form = String16) & (fp^.typ^.BaseTyp^.form = Char16)
				THEN (* ok *)
				ELSIF ap^.class >= Nconst THEN err(59)
				ELSE DynArrParCheck(fp^.typ, ap^.typ, FALSE)
				END
			ELSE CheckAssign(fp^.typ, ap)
			END
		END
	END Param;

	PROCEDURE StaticLink* (dlev: BYTE);
		VAR scope: OPT.Object;
	BEGIN
		scope := OPT.topScope;
		WHILE dlev > 0 DO DEC(dlev);
			INCL(scope^.link^.conval^.setval, slNeeded);
			scope := scope^.left
		END
	END StaticLink;

	PROCEDURE Call* (VAR x: OPT.Node; apar: OPT.Node; fp: OPT.Object);
		VAR typ: OPT.Struct; p: OPT.Node; lev: BYTE;
	BEGIN
		IF x^.class = Nproc THEN typ := x^.typ;
			lev := x^.obj^.mnolev;
			IF lev > 0 THEN StaticLink(SHORT(SHORT(OPT.topScope^.mnolev-lev))) END;
			IF x^.obj^.mode = IProc THEN err(121) END
		ELSIF (x^.class = Nfield) & (x^.obj^.mode = TProc) THEN typ := x^.typ;
			x^.class := Nproc; p := x^.left; x^.left := NIL; p^.link := apar; apar := p; fp := x^.obj^.link
		ELSE typ := x^.typ^.BaseTyp
		END;
		BindNodes(Ncall, typ, x, apar); x^.obj := fp
	END Call;

	PROCEDURE Enter* (VAR procdec: OPT.Node; stat: OPT.Node; proc: OPT.Object);
		VAR x: OPT.Node;
	BEGIN
		x := OPT.NewNode(Nenter); x^.typ := OPT.notyp; x^.obj := proc;
		x^.left := procdec; x^.right := stat; procdec := x
	END Enter;

	PROCEDURE Return* (VAR x: OPT.Node; proc: OPT.Object);
		VAR node: OPT.Node;
	BEGIN
		IF proc = NIL THEN (* return from module *)
			IF x # NIL THEN err(124) END
		ELSE
			IF x # NIL THEN CheckAssign(proc^.typ, x)
			ELSIF proc^.typ # OPT.notyp THEN err(124)
			END
		END;
		node := OPT.NewNode(Nreturn); node^.typ := OPT.notyp; node^.obj := proc; node^.left := x; x := node
	END Return;

	PROCEDURE Assign* (VAR x: OPT.Node; y: OPT.Node);
		VAR z: OPT.Node;
	BEGIN
		IF (x^.class >= Nconst) OR (x^.typ^.form = String8) THEN err(56) END;
		CheckAssign(x^.typ, y);
		IF x^.readonly THEN err(76) END;
		IF (y^.class = Nconst) & (y^.typ^.form = String8) & (x^.typ^.form # Pointer) THEN AssignString(x, y)
		ELSE
			IF x^.typ^.comp = Record THEN
				IF x^.class = Nguard THEN z := x^.left ELSE z := x END;
				IF (z^.class = Nderef) & (z^.left^.class = Nguard) THEN
					z^.left := z^.left^.left	(* skip guard before dereferencing *)
				END;
				IF (x^.typ^.strobj # NIL) & ((z^.class = Nderef) OR (z^.class = Nvarpar)) THEN
					BindNodes(Neguard, x^.typ, z, NIL); x := z
				END
			END;
			BindNodes(Nassign, OPT.notyp, x, y); x^.subcl := assign
		END
	END Assign;

	PROCEDURE Inittd* (VAR inittd, last: OPT.Node; typ: OPT.Struct);
		VAR node: OPT.Node;
	BEGIN
		node := OPT.NewNode(Ninittd); node^.typ := typ;
		node^.conval := OPT.NewConst(); node^.conval^.intval := typ^.txtpos;
		IF inittd = NIL THEN inittd := node ELSE last^.link := node END;
		last := node
	END Inittd;

	(* handling of temporary variables for string operations *)
	
	PROCEDURE Overlap (left, right: OPT.Node): BOOLEAN;
	BEGIN
		IF right.class = Nconst THEN
			RETURN FALSE
		ELSIF (right.class = Ndop) & (right.subcl = plus) THEN
			RETURN Overlap(left, right.left) OR Overlap(left, right.right)
		ELSE
			WHILE right.class = Nmop DO right := right.left END;
			IF right.class = Nderef THEN right := right.left END;
			IF left.typ.BaseTyp # right.typ.BaseTyp THEN RETURN FALSE END;
			LOOP
				IF left.class = Nvarpar THEN
					WHILE (right.class = Nindex) OR (right.class = Nfield) OR (right.class = Nguard) DO
						right := right.left
					END;
					RETURN (right.class # Nvar) OR (right.obj.mnolev < left.obj.mnolev)
				ELSIF right.class = Nvarpar THEN
					WHILE (left.class = Nindex) OR (left.class = Nfield) OR (left.class = Nguard) DO left := left.left END;
					RETURN (left.class # Nvar) OR (left.obj.mnolev < right.obj.mnolev)
				ELSIF (left.class = Nvar) & (right.class = Nvar) THEN
					RETURN left.obj = right.obj
				ELSIF (left.class = Nderef) & (right.class = Nderef) THEN
					RETURN TRUE
				ELSIF (left.class = Nindex) & (right.class = Nindex) THEN
					IF (left.right.class = Nconst) & (right.right.class = Nconst)
						& (left.right.conval.intval # right.right.conval.intval) THEN RETURN FALSE END;
					left := left.left; right := right.left
				ELSIF (left.class = Nfield) & (right.class = Nfield) THEN
					IF left.obj # right.obj THEN RETURN FALSE END;
					left := left.left; right := right.left;
					WHILE left.class = Nguard DO left := left.left END;
					WHILE right.class = Nguard DO right := right.left END
				ELSE
					RETURN FALSE
				END
			END
		END
	END Overlap;

	PROCEDURE GetStaticLength (n: OPT.Node; OUT length: INTEGER);
		VAR x: INTEGER;
	BEGIN
		IF n.class = Nconst THEN
			length := n.conval.intval2 - 1
		ELSIF (n.class = Ndop) & (n.subcl = plus) THEN
			GetStaticLength(n.left, length); GetStaticLength(n.right, x);
			IF (length >= 0) & (x >= 0) THEN length := length + x ELSE length := -1 END
		ELSE
			WHILE (n.class = Nmop) & (n.subcl = conv) DO n := n.left END;
			IF (n.class = Nderef) & (n.subcl = 1) THEN n := n.left END;
			IF n.typ.comp = Array THEN
				length := n.typ.n - 1
			ELSIF n.typ.comp = DynArr THEN
				length := -1
			ELSE	(* error case *)
				length := 4
			END
		END
	END GetStaticLength;

	PROCEDURE GetMaxLength (n: OPT.Node; VAR stat, last: OPT.Node; OUT length: OPT.Node);
		VAR x: OPT.Node; d: INTEGER; obj: OPT.Object;
	BEGIN
		IF n.class = Nconst THEN
			length := NewIntConst(n.conval.intval2 - 1)
		ELSIF (n.class = Ndop) & (n.subcl = plus) THEN
			GetMaxLength(n.left, stat, last, length); GetMaxLength(n.right, stat, last, x);
			IF (length.class = Nconst) & (x.class = Nconst) THEN ConstOp(plus, length, x)
			ELSE BindNodes(Ndop, length.typ, length, x); length.subcl := plus
			END
		ELSE
			WHILE (n.class = Nmop) & (n.subcl = conv) DO n := n.left END;
			IF (n.class = Nderef) & (n.subcl = 1) THEN n := n.left END;
			IF n.typ.comp = Array THEN
				length := NewIntConst(n.typ.n - 1)
			ELSIF n.typ.comp = DynArr THEN
				d := 0;
				WHILE n.class = Nindex DO n := n.left; INC(d) END;
				ASSERT((n.class = Nderef) OR (n.class = Nvar) OR (n.class = Nvarpar));
				IF (n.class = Nderef) & (n.left.class # Nvar) & (n.left.class # Nvarpar) THEN
					GetTempVar("@tmp", n.left.typ, obj);
					x := NewLeaf(obj); Assign(x, n.left); Link(stat, last, x);
					n.left := NewLeaf(obj);	(* tree is manipulated here *)
					n := NewLeaf(obj); DeRef(n)
				END;
				IF (n.typ.sysflag # 0) & (n.typ.comp = DynArr) & (n.typ.BaseTyp.form = Char8) THEN
					StrDeref(n);
					BindNodes(Ndop, OPT.inttyp, n, NewIntConst(d)); n.subcl := len;
					BindNodes(Ndop, OPT.inttyp, n, NewIntConst(1)); n.subcl := plus
				ELSE
					BindNodes(Ndop, OPT.inttyp, n, NewIntConst(d)); n.subcl := len;
				END;
				length := n
			ELSE	(* error case *)
				length := NewIntConst(4)
			END
		END
	END GetMaxLength;

	PROCEDURE CheckBuffering* (
		VAR n: OPT.Node; left: OPT.Node; par: OPT.Object; VAR stat, last: OPT.Node
	);
		VAR length, x: OPT.Node; obj: OPT.Object; typ: OPT.Struct; len, xlen: INTEGER;
	BEGIN
		IF (n.typ.form = String8)
			& ((n.class = Ndop) & (n.subcl = plus) & ((left = NIL) OR Overlap(left, n.right))
				OR (n.class = Nmop) & (n.subcl = conv) & (left = NIL)
				OR (par # NIL) & (par.vis = inPar) & (par.typ.comp = Array)) THEN
			IF (par # NIL) & (par.typ.comp = Array) THEN
				len := par.typ.n - 1
			ELSE
				IF left # NIL THEN GetStaticLength(left, len) ELSE len := -1 END;
				GetStaticLength(n, xlen);
				IF (len = -1) OR (xlen # -1) & (xlen < len) THEN len := xlen END
			END;
			IF len # -1 THEN
				typ := OPT.NewStr(Comp, Array); typ.n := len + 1; typ.BaseTyp := n.typ.BaseTyp;
				GetTempVar("@str", typ, obj);
				x := NewLeaf(obj); Assign(x, n); Link(stat, last, x);
				n := NewLeaf(obj)
			ELSE
				IF left # NIL THEN GetMaxLength(left, stat, last, length)
				ELSE GetMaxLength(n, stat, last, length)
				END;
				typ := OPT.NewStr(Pointer, Basic);
				typ.BaseTyp := OPT.NewStr(Comp, DynArr); typ.BaseTyp.BaseTyp := n.typ.BaseTyp;
				GetTempVar("@ptr", typ, obj);
				x := NewLeaf(obj); Construct(Nassign, x, length); x.subcl := newfn; Link(stat, last, x);
				x := NewLeaf(obj); DeRef(x); Assign(x, n); Link(stat, last, x);
				n := NewLeaf(obj); DeRef(n)
			END;
			StrDeref(n)
		END
	END CheckBuffering;
	
	PROCEDURE CheckVarParBuffering* (VAR n: OPT.Node; VAR stat, last: OPT.Node);
		VAR x: OPT.Node; obj: OPT.Object;
	BEGIN
		IF (n.class # Nvar) OR (n.obj.mnolev <= 0) THEN
			GetTempVar("@ptr", n.typ, obj);
			x := NewLeaf(obj); Assign(x, n); Link(stat, last, x);
			n := NewLeaf(obj)
		END
	END CheckVarParBuffering;

END OfrontOPB.
