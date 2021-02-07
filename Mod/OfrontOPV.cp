MODULE OfrontOPV;	(* J. Templ 16.2.95 / 3.7.96 *)

	(* 30.4.2000 jt, various promotion rules changed (long) => (LONGINT), xxxL avoided
		26.7.2002 jt, bug fix in Len: wrong result if called for fixed Array
	*)

	IMPORT OPT := OfrontOPT, OPC := OfrontOPC, OPM := OfrontOPM, OPS := OfrontOPS;

	CONST
		(* object modes *)
		Var = 1; VarPar = 2; Fld = 4; Typ = 5; LProc = 6; XProc = 7;
		CProc = 9; IProc = 10; Mod = 11; TProc = 13;

		(* symbol values or ops *)
		times = 1; slash = 2; div = 3; mod = 4; div0 = 5; rem0 = 6;
		and = 7; plus = 8; minus = 9; or = 10; eql = 11;
		neq = 12; lss = 13; leq = 14; gtr = 15; geq = 16;
		in = 17; is = 18; ash = 19; msk = 20; len = 21;
		conv = 22; abs = 23; cap = 24; odd = 25; asr = 26;
		lsl = 27; ror = 28; not = 35; min = 36; max = 37;
		(*SYSTEM*)
		lsh = 29; rot = 30; adr = 31; bit = 33; val = 34;
		unsgn = 40; (* псевдооперация unsigned для div *)

		(* structure forms *)
		Byte = 1; Bool = 2; Char = 3; SInt = 4; Int = 5; LInt = 6;
		Real = 7; LReal = 8; Set = 9; String = 10; NilTyp = 11; Pointer = 13; ProcTyp = 15; Comp = 16;

		(* composite structure forms *)
		Array = 2; DynArr = 3; Record = 4;

		(* nodes classes *)
		Nvar = 0; Nvarpar = 1; Nfield = 2; Nderef = 3; Nindex = 4; Nguard = 5; Neguard = 6;
		Nconst = 7; Ntype = 8; Nproc = 9; Nupto = 10; Nmop = 11; Ndop = 12; Ncall = 13;
		Ninittd = 14; Nenter = 18; Nassign = 19;
		Nifelse =20; Ncase = 21; Nwhile = 22; Nrepeat = 23; Nloop = 24; Nexit = 25;
		Nreturn = 26; Nwith = 27; Ntrap = 28; Ncomp = 30; Nraw = 31;

		(*function number*)
		assign = 0; newfn = 1; incfn = 18; decfn = 19;
		inclfn = 20; exclfn = 21; copyfn = 23; packfn = 31; unpkfn = 32; assertfn = 37;

		(*SYSTEM function number*)
		getfn = 29; putfn = 30; sysnewfn = 35; movefn = 36;

		(*procedure flags*)
		hasBody = 1; isRedef = 2; absAttr = 17; empAttr = 19; extAttr = 20;

		super = 1;

		UndefinedType = 0; (* named type not yet defined *)
		ProcessingType = 1; (* pointer type is being processed *)
		PredefinedType = 2; (* for all predefined types *)
		DefinedInHdr = 3+OPM.HeaderFile; (* named type has been defined in header file *)
		DefinedInBdy = 3+OPM.BodyFile; (* named type has been defined in body file *)

		OpenParen = "(";
		CloseParen = ")";
		OpenBracket = "[";
		CloseBracket = "]";
		Blank = " ";
		Comma = ", ";
		Deref = "*";
		EntierFunc = "__ENTIER(";
		IsFunc = "__IS(";
		IsPFunc = "__ISP(";
		GuardPtrFunc = "__GUARDP(";
		GuardRecFunc = "__GUARDR(";
		TypeFunc = "__TYPEOF(";
		SetOfFunc = "__SETOF(";
		SetRangeFunc = "__SETRNG(";
		CopyFunc = "__COPY(";
		MoveFunc = "__MOVE(";
		GetFunc = "__GET(";
		PutFunc = "__PUT(";
		DynTypExt = "__typ";
		WithChk = "__WITHCHK(";
		Break = "break";
		ElseStat = "else ";

		MinPrec = -1;
		MaxPrec = 12;
		ProcTypeVar = 11; (* precedence number when a call is made with a proc type variable *)

		internal = 0;

	TYPE
		ExitInfo = RECORD level, label: SHORTINT END ;


	VAR
		assert, inxchk, mainprog, ansi, oldc: BOOLEAN;
		stamp: SHORTINT;	(* unique number for nested objects *)
		recno: SHORTINT;	(* number of anonymous record types *)

		exit: ExitInfo;	(* to check if EXIT is simply a break *)
		nofExitLabels: SHORTINT;


	PROCEDURE Init*;
	BEGIN
		OPC.Init;
		stamp := 0; recno := 0; nofExitLabels := 0;
		assert := OPM.assert IN OPM.opt;
		inxchk := OPM.inxchk IN OPM.opt;
		mainprog := OPM.mainprog IN OPM.opt;
		ansi := OPM.ansi IN OPM.opt;
		oldc := OPM.oldc IN OPM.opt
	END Init;

	PROCEDURE TypSize*(typ: OPT.Struct);
		VAR f, c: SHORTINT; offset, size, base, fbase, off0: INTEGER;
			fld: OPT.Object; btyp: OPT.Struct;
	BEGIN
		IF typ = OPT.undftyp THEN OPM.err(58)
		ELSIF typ^.size = -1 THEN
			f := typ^.form; c := typ^.comp;
			IF c = Record THEN btyp := typ^.BaseTyp;
				IF btyp = NIL THEN offset := 0; base := 1; (* OPM.RecAlign *)
				ELSE TypSize(btyp); offset := btyp^.size - btyp^.sysflag DIV 100H; base := btyp^.align
				END;
				fld := typ^.link;
				WHILE (fld # NIL) & (fld^.mode = Fld) DO
					btyp := fld^.typ; TypSize(btyp);
					size := btyp^.size; fbase := OPC.BaseAlignment(btyp);
					OPC.Align(offset, fbase);
					fld^.adr := offset; INC(offset, size);
					IF fbase > base THEN base := fbase END ;
					fld := fld^.link
				END;
				(* base is now the largest alignment of any field *)
				off0 := offset;
				IF offset = 0 THEN offset := 1 END;	(* 1 byte filler to avoid empty struct *)
				OPC.Align(offset, base);
				IF (typ^.strobj = NIL) & (typ^.align MOD 10000H = 0) THEN INC(recno); INC(base, recno * 10000H) END;
				typ^.size := offset; typ^.align := base;
				(* encode the trailing gap into the symbol table to allow dense packing of extended records *)
				typ^.sysflag := SHORT(typ^.sysflag MOD 100H + SHORT((offset - off0)*100H));
				typ^.n := -1  (* methods not counted yet *)
			ELSIF c = Array THEN
				TypSize(typ^.BaseTyp);
				typ^.size := typ^.n * typ^.BaseTyp^.size
			ELSIF f = Pointer THEN
				typ^.size := OPM.AdrSize;
				IF typ^.BaseTyp = OPT.undftyp THEN OPM.Mark(128, typ^.n)
				ELSE TypSize(typ^.BaseTyp)
				END
			ELSIF f = ProcTyp THEN
				typ^.size := OPM.AdrSize;
			ELSIF c = DynArr THEN
				btyp := typ^.BaseTyp; TypSize(btyp);
				IF ODD(typ^.sysflag) THEN typ^.size := 4
				ELSIF btyp^.comp = DynArr THEN typ^.size := btyp^.size + 4	(* describes dim not size *)
				ELSE typ^.size := 8
				END
			END
		END
	END TypSize;

	PROCEDURE CountTProcs(rec: OPT.Struct);
		VAR btyp: OPT.Struct;

		PROCEDURE TProcs(obj: OPT.Object);	(* obj.mnolev = 0, TProcs of base type already counted *)
			VAR redef: OPT.Object;
		BEGIN
			IF obj # NIL THEN
				TProcs(obj.left);
				IF obj.mode = TProc THEN
					OPT.FindField(obj.name^, rec^.BaseTyp, redef);
					IF redef # NIL THEN
						obj.adr := redef.adr (*mthno*);
						IF ~(isRedef IN obj.conval.setval) OR (redef.conval.setval * {extAttr, absAttr, empAttr} = {}) THEN
							OPM.Mark(119, rec.txtpos)
						END
					ELSE obj.adr := 10000H*rec.n; INC(rec.n)
					END;
					IF obj.conval.setval * {hasBody, absAttr, empAttr} = {} THEN OPM.Mark(129, obj.adr) END
				END;
				TProcs(obj.right)
			END
		END TProcs;

	BEGIN
		IF rec.n = -1 THEN
			IF rec.sysflag > 0 THEN rec.n := 0 ELSE rec.n := 1(*OPT.anytyp.n*) END;
			btyp := rec.BaseTyp;
			IF btyp # NIL THEN CountTProcs(btyp); rec.n := btyp.n END;
			TProcs(rec.link)
		END
	END CountTProcs;

	PROCEDURE ^Traverse (obj, outerScope: OPT.Object; exported: BOOLEAN);

	PROCEDURE TraverseType (typ: OPT.Struct);
		VAR p: OPT.Object;
	BEGIN
		IF ~typ^.allocated THEN
			typ^.allocated := TRUE;
			TypSize(typ);
			IF typ^.comp = Record THEN
				CountTProcs(typ);
				IF typ.extlev > 14 THEN OPM.Mark(233, typ.txtpos) END;
				IF typ^.BaseTyp # NIL THEN TraverseType(typ^.BaseTyp) END;
				Traverse(typ^.link, typ^.strobj, FALSE)
			ELSIF typ^.form = Pointer THEN
				IF typ^.BaseTyp = OPT.undftyp THEN OPM.Mark(128, typ.txtpos) END;
				TraverseType(typ^.BaseTyp);
				IF (typ^.BaseTyp.comp = DynArr) & (typ^.BaseTyp^.sysflag <= 0) THEN OPC.InsertArrayType(typ^.BaseTyp) END
			ELSIF typ^.form = ProcTyp THEN
				TraverseType(typ^.BaseTyp);
				p := typ^.link;
				WHILE p # NIL DO TraverseType(p.typ); p := p.link END
			ELSE (* typ^.comp IN {Array, DynArr} *)
				TraverseType(typ^.BaseTyp)
			END
		END
	END TraverseType;

	PROCEDURE Stamp(VAR s: OPS.String);
		VAR i, j, k: SHORTINT; n: ARRAY 10 OF SHORTCHAR; res: OPS.Name;
	BEGIN INC(stamp);
		i := 0; j := stamp;
		WHILE s[i] # 0X DO res[i] := s[i]; INC(i) END ;
		IF i > 25 THEN i := 25 END ;
		res[i] := "_"; res[i+1] := "_"; INC(i, 2); k := 0;
		REPEAT n[k] := SHORT(CHR((j MOD 10) + ORD("0"))); j := SHORT(j DIV 10); INC(k) UNTIL j = 0;
		REPEAT DEC(k); res[i] := n[k]; INC(i) UNTIL k = 0;
		res[i] := 0X;
		IF LEN(s^) <= i THEN NEW(s, i+1) END; s^ := res$
	END Stamp;

	PROCEDURE Traverse (obj, outerScope: OPT.Object; exported: BOOLEAN);
		VAR mode: SHORTINT; scope, p: OPT.Object;
	BEGIN
		IF obj # NIL THEN
			Traverse(obj^.left, outerScope, exported);
			IF obj^.name[0] = "@" THEN obj^.name[0] := "_"; Stamp(obj^.name) END;	(* translate and make unique @for, ... *)
			obj^.linkadr := UndefinedType;
			mode := obj^.mode;
			IF (mode = Typ) & ((obj^.vis # internal) = exported) THEN TraverseType(obj^.typ);
			ELSIF mode IN {Var, VarPar, Fld} THEN TraverseType(obj^.typ)
			END;
			IF ~exported THEN (* do this only once *)
				IF (mode IN {LProc, Typ}) & (obj^.mnolev > 0) THEN
					obj.entry := obj.name; NEW(obj.name, LEN(obj.name$) + 12); Stamp(obj^.name)
				END;
				IF mode IN {Var, VarPar, Typ} THEN
					obj^.scope := outerScope
				ELSIF mode IN {LProc, XProc, TProc, CProc, IProc} THEN
					TraverseType(obj^.typ);
					p := obj^.link;
					WHILE p # NIL DO TraverseType(p.typ); p := p.link END;
					scope := obj^.scope;
					scope^.leaf := TRUE;
					NEW(scope.name, LEN(obj.name$) + 12); scope.name^ := obj.name$; Stamp(scope^.name);
					IF mode = CProc THEN obj^.adr := 1 (* c.f. OPC.CProcDefs *) END;
					IF scope^.mnolev > 1 THEN outerScope^.leaf := FALSE END;
					Traverse (obj^.scope^.right, obj^.scope, FALSE);
					IF obj.conval.setval * {hasBody, absAttr, empAttr} = {} THEN OPM.Mark(129, obj.adr) END
				END
			END;
			Traverse(obj^.right, outerScope, exported)
		END
	END Traverse;

	PROCEDURE AdrAndSize* (topScope: OPT.Object);
	BEGIN
		OPM.errpos := topScope^.adr;	(* text position of scope used if error *)
		topScope^.leaf := TRUE;
		Traverse(topScope^.right, topScope, TRUE);	(* first pass only on exported types and procedures	*)
		Traverse(topScope^.right, topScope, FALSE);	(* second pass *)
		(* mark basic types as predefined, OPC.Ident can avoid qualification*)
		OPT.chartyp^.strobj^.linkadr := PredefinedType;
		OPT.settyp^.strobj^.linkadr := PredefinedType;
		OPT.realtyp^.strobj^.linkadr := PredefinedType;
		OPT.inttyp^.strobj^.linkadr := PredefinedType;
		OPT.linttyp^.strobj^.linkadr := PredefinedType;
		OPT.lrltyp^.strobj^.linkadr := PredefinedType;
		OPT.sinttyp^.strobj^.linkadr := PredefinedType;
		OPT.booltyp^.strobj^.linkadr := PredefinedType;
		OPT.bytetyp^.strobj^.linkadr := PredefinedType;
		OPT.ubytetyp^.strobj^.linkadr := PredefinedType;
		OPT.sysptrtyp^.strobj^.linkadr := PredefinedType;
	END AdrAndSize;

(* ____________________________________________________________________________________________________________________________________________________________________ *)

	PROCEDURE Precedence (class, subclass, form, comp: SHORTINT): SHORTINT;
	BEGIN
		CASE class OF
			Nconst, Nvar, Nfield, Nindex, Nproc, Ncall:
					RETURN 10
		|	Nguard: IF OPM.typchk IN OPM.opt THEN RETURN 10 ELSE RETURN 9 (*cast*) END
		|	Nvarpar:
					IF comp IN {Array, DynArr} THEN RETURN 10 ELSE RETURN 9 END (* arrays don't need deref *)
		|	Nderef:
					RETURN 9
		|	Nmop:
					CASE subclass OF
						not, minus, adr, val, conv, unsgn:
								RETURN 9
					|	is, abs, cap, odd, bit:
								RETURN 10
					END
		|	Ndop:
					CASE subclass OF
						times:
								IF form = Set THEN RETURN 4 ELSE RETURN 8 END
					|	slash:
								IF form = Set THEN RETURN 3 ELSE RETURN 8 END
					|	div, mod:
								RETURN 10 (* div/mod are replaced by functions *)
					|	div0, rem0:
								RETURN 8
					|	plus:
								IF form = Set THEN RETURN 2 ELSE RETURN 7 END
					|	minus:
								IF form = Set THEN RETURN 4 ELSE RETURN 7 END
					|	lss, leq, gtr, geq:
								RETURN 6
					|	eql, neq:
								RETURN 5
					|	and:
								RETURN 1
					|	or:
								RETURN 0
					|	len, in, ash, msk, bit, lsh, rot, asr, lsl, ror, min, max:
								RETURN 10
					END;
		|	Nupto:
					RETURN 10
		|	Ntype, Neguard: (* ignored anyway *)
					RETURN MaxPrec
		|	Ncomp:
					RETURN 10
		END;
	END Precedence;

	PROCEDURE^ expr (n: OPT.Node; prec: SHORTINT);
	PROCEDURE^ design(n: OPT.Node; prec: SHORTINT);

	PROCEDURE Len(n: OPT.Node; dim: INTEGER; incl0x: BOOLEAN);
		VAR d: INTEGER; array: OPT.Struct;
	BEGIN
		WHILE (n^.class = Nindex) & (n^.typ^.comp = DynArr(*26.7.2002*)) DO INC(dim); n := n^.left END;
		IF (n^.typ^.form = String) OR (n^.class = Nderef) & (n^.typ^.sysflag # 0) & (n^.typ^.n = 0) THEN
			OPM.WriteString("__STRLEN("); expr(n, MinPrec); OPM.Write(CloseParen);
			IF incl0x THEN OPM.WriteString(" + 1") END
		ELSIF (n^.class = Nderef) & (n^.typ^.comp = DynArr) THEN d := dim; array := n^.typ;
			WHILE d > 0 DO array := array^.BaseTyp; DEC(d) END;
			IF array^.comp = DynArr THEN
				design(n^.left, 10); OPM.WriteString("->len["); OPM.WriteInt(dim); OPM.Write("]")
			ELSE
				OPM.WriteInt(array^.n); OPM.PromoteIntConstToLInt()
			END
		ELSE
			OPC.Len(n^.obj, n^.typ, dim)
		END
	END Len;

	PROCEDURE SameExp (n1, n2: OPT.Node): BOOLEAN;
	BEGIN
		IF (n2^.class = Nderef) & (n2^.typ^.form = String) THEN n2 := n2^.left END;
		WHILE (n1^.class = n2^.class) & (n1^.typ = n2^.typ) DO
			CASE n1^.class OF
			| Nvar, Nvarpar, Nproc: RETURN n1^.obj = n2^.obj
			| Nconst: RETURN (n1^.typ^.form IN {Byte, SInt..LInt}) & (n1^.conval^.intval = n2^.conval^.intval)
			| Nfield: IF n1^.obj # n2^.obj THEN RETURN FALSE END
			| Nderef, Nguard:
			| Nindex: IF ~SameExp(n1^.right, n2^.right) THEN RETURN FALSE END
			| Nmop: IF (n1^.subcl # n2^.subcl) OR (n1^.subcl = is) THEN RETURN FALSE END
			| Ndop: IF (n1^.subcl # n2^.subcl) OR ~SameExp(n1^.right, n2^.right) THEN RETURN FALSE END
			ELSE RETURN FALSE
			END;
			n1 := n1^.left; n2 := n2^.left
		END;
		RETURN FALSE
	END SameExp;

	PROCEDURE SideEffects(n: OPT.Node): BOOLEAN;
	BEGIN
		IF n # NIL THEN RETURN (n^.class = Ncall) OR SideEffects(n^.left) OR SideEffects(n^.right)
		ELSE RETURN FALSE
		END
	END SideEffects;

	PROCEDURE Entier(n: OPT.Node; prec: SHORTINT);
	BEGIN
		IF n^.typ^.form IN {Real, LReal} THEN
			OPM.WriteString(EntierFunc); expr(n, MinPrec); OPM.Write(CloseParen)
		ELSE expr(n, prec)
		END
	END Entier;

	PROCEDURE LEntier(n: OPT.Node; prec: SHORTINT);
	BEGIN
		IF n^.typ^.form IN {Real, LReal} THEN
			OPM.WriteString("__ENTIERL("); expr(n, MinPrec); OPM.Write(CloseParen)
		ELSE expr(n, prec)
		END
	END LEntier;

	PROCEDURE Convert(n: OPT.Node; form, prec: SHORTINT);
		VAR from: SHORTINT;
	BEGIN from := n^.typ^.form;
		CASE form OF
		| Byte:
			IF from = Bool THEN OPM.WriteString("(BYTE)"); expr(n, 9)
			ELSE
				IF OPM.ranchk IN OPM.opt THEN OPM.WriteString("__SHORT");
					IF SideEffects(n) THEN OPM.Write("F") END ;
					OPM.Write(OpenParen); Entier(n, MinPrec); OPM.WriteString(Comma);
					OPM.WriteInt(MAX(BYTE) + 1); OPM.WriteString(Comma); OPM.WriteModPos; OPM.Write(CloseParen)
				ELSE OPM.WriteString("(BYTE)"); Entier(n, 9)
				END
			END
		| SInt:
			IF OPM.ranchk IN OPM.opt THEN OPM.WriteString("__SHORT");
				IF SideEffects(n) THEN OPM.Write("F") END ;
				OPM.Write(OpenParen); Entier(n, MinPrec); OPM.WriteString(Comma);
				OPM.WriteInt(MAX(SHORTINT) + 1); OPM.WriteString(Comma); OPM.WriteModPos; OPM.Write(CloseParen)
			ELSE OPM.WriteString("(SHORTINT)"); Entier(n, 9)
			END
		| Int:
			IF (from < Int) OR (from = Set) THEN OPM.WriteString("(INTEGER)"); expr(n, 9)
			ELSE
				IF OPM.ranchk IN OPM.opt THEN OPM.WriteString("__SHORT");
					IF SideEffects(n) THEN OPM.Write("F") END ;
					OPM.Write(OpenParen); Entier(n, MinPrec); OPM.WriteString(Comma);
					OPM.WriteInt(MAX(INTEGER) + 1); OPM.WriteString(Comma); OPM.WriteModPos; OPM.Write(CloseParen)
				ELSE OPM.WriteString("(INTEGER)"); Entier(n, 9)
				END
			END
		| LInt:
			IF from < LInt THEN OPM.WriteString("(LONGINT)") END; LEntier(n, 9)
		| Char:
			IF OPM.ranchk IN OPM.opt THEN OPM.WriteString("__CHR");
				IF SideEffects(n) THEN OPM.Write("F") END;
				OPM.Write(OpenParen); Entier(n, MinPrec); OPM.WriteString(Comma); OPM.WriteModPos; OPM.Write(CloseParen)
			ELSE OPM.WriteString("(CHAR)"); Entier(n, 9)
			END
		| Set: OPM.WriteString("(SET)"); Entier(n, 9)
		| Real: OPM.WriteString("(SHORTREAL)"); expr(n, prec)
		| LReal: OPM.WriteString("(REAL)"); expr(n, prec)
		ELSE expr(n, prec)
		END
	END Convert;

	PROCEDURE TypeOf(n: OPT.Node);
	BEGIN
		IF n^.typ^.form = Pointer THEN
			OPM.WriteString(TypeFunc); expr(n, MinPrec); OPM.Write(")")
		ELSIF n^.class IN {Nvar, Nindex, Nfield} THEN	(* dyn rec type = stat rec type *)
			OPC.Andent(n^.typ); OPM.WriteString(DynTypExt)
		ELSIF n^.class = Nderef THEN	(* p^ *)
			OPM.WriteString(TypeFunc); expr(n^.left, MinPrec); OPM.Write(")")
		ELSIF n^.class = Nguard THEN	(* r(T) *)
			TypeOf(n^.left)	(* skip guard *)
		ELSIF (n^.class = Nmop) & (n^.subcl = val) THEN
			(*SYSTEM.VAL(typ, var par rec)*)
			OPC.TypeOf(n^.left^.obj)
		ELSIF n^.typ^.form = NilTyp THEN	(* NIL *)
			OPM.WriteString("NIL")
		ELSE	(* var par rec *)
			OPC.TypeOf(n^.obj)
		END
	END TypeOf;

	PROCEDURE Index(n, d: OPT.Node; prec, dim: SHORTINT);
	BEGIN
		IF ~inxchk
		OR (n^.right^.class = Nconst) & ((n^.right^.conval^.intval = 0) OR (n^.left^.typ^.comp # DynArr))
		OR (n^.left^.typ^.comp = DynArr) & ODD(n^.left^.typ^.sysflag) THEN
			expr(n^.right, prec)
		ELSE
			IF SideEffects(n^.right) THEN OPM.WriteString("__XF(") ELSE OPM.WriteString("__X(") END;
			expr(n^.right, MinPrec); OPM.WriteString(Comma); Len(d, dim, FALSE); OPM.WriteString(Comma);
			OPM.WriteModPos; OPM.Write(CloseParen)
		END
	END Index;

	PROCEDURE design(n: OPT.Node; prec: SHORTINT);
		VAR obj: OPT.Object; typ: OPT.Struct;
			class, designPrec, comp: SHORTINT;
			d, x: OPT.Node; dims, i: SHORTINT;
	BEGIN
		comp := n^.typ^.comp; obj := n^.obj; class := n^.class;
		designPrec := Precedence(class, n^.subcl, n^.typ^.form, comp);
		IF (class = Nvar) & (obj^.mnolev > 0) & (obj^.mnolev # OPM.level) & (prec = 10) THEN designPrec := 9 END ;
		IF prec > designPrec THEN OPM.Write(OpenParen) END;
		IF prec = ProcTypeVar THEN OPM.Write(Deref) END; (* proc var calls must be dereferenced in K&R C *)
		CASE class OF
			Nproc:
					OPC.Ident(n^.obj)
		|	Nvar:
					OPC.CompleteIdent(n^.obj)
		|	Nvarpar:
					IF ~(comp IN {Array, DynArr}) THEN OPM.Write(Deref) END; (* deref var parameter *)
					OPC.CompleteIdent(n^.obj)
		|	Nfield:
					IF n^.left^.class = Nderef THEN design(n^.left^.left, designPrec); OPM.WriteString("->")
					ELSE design(n^.left, designPrec); OPM.Write(".")
					END ;
					OPC.Ident(n^.obj)
		|	Nderef:
					IF n.subcl # 0 THEN
						expr(n^.left, designPrec)
					ELSE
						IF n^.typ^.comp = DynArr THEN
							IF n^.typ^.sysflag > 0 THEN design(n^.left, designPrec)
							ELSE design(n^.left, 10); OPM.WriteString("->data")
							END
						ELSE OPM.Write(Deref); expr(n^.left, designPrec)
						END
					END
		|	Nindex:
					d := n^.left;
					IF d^.typ^.comp = DynArr THEN dims := 0;
						WHILE d^.class = Nindex DO d := d^.left; INC(dims) END ;
						IF n^.typ^.comp = DynArr THEN OPM.Write("&") END ;
						design(d, designPrec);
						OPM.Write(OpenBracket);
						IF n^.typ^.comp = DynArr THEN OPM.Write("(") END ;
						i := dims; x := n;
						WHILE x # d DO	(* apply Horner schema *)
							IF x^.left # d THEN Index(x, d, 7, i); OPM.WriteString(" + "); Len(d, i, FALSE);  OPM.WriteString(" * ("); DEC(i)
							ELSE Index(x, d, MinPrec, i)
							END;
							x := x^.left
						END;
						FOR i := 1 TO dims DO OPM.Write(")") END;
						IF n^.typ^.comp = DynArr THEN
							(*  element type is DynArr; finish Horner schema with virtual indices = 0*)
							OPM.Write(")");
							WHILE i <= d^.typ^.n DO
								OPM.WriteString(" * "); Len(d, i, FALSE);
								INC(i)
							END
						END;
						OPM.Write(CloseBracket)
					ELSE
						design(n^.left, designPrec);
						OPM.Write(OpenBracket);
						Index(n, n^.left, MinPrec, 0);
						OPM.Write(CloseBracket)
					END
		|	Nguard:
					typ := n^.typ; obj := n^.left^.obj;
					IF OPM.typchk IN OPM.opt THEN
						IF typ^.comp = Record THEN OPM.WriteString(GuardRecFunc);
							IF obj^.mnolev # OPM.level THEN (*intermediate level var-par record*)
								OPM.WriteString(obj^.scope^.name^); OPM.WriteString("__curr->"); OPC.Ident(obj)
							ELSE (*local var-par record*)
								OPC.Ident(obj)
							END ;
						ELSE (*Pointer*)
							IF typ^.BaseTyp^.strobj = NIL THEN OPM.WriteString("__GUARDA(") ELSE OPM.WriteString(GuardPtrFunc) END ;
							expr(n^.left, MinPrec); typ := typ^.BaseTyp
						END ;
						OPM.WriteString(Comma);
						OPC.Andent(typ); OPM.WriteString(Comma);
						OPM.WriteInt(typ^.extlev); OPM.Write(")")
					ELSE
						IF typ^.comp = Record THEN (* do not cast record directly, cast pointer to record *)
							OPM.WriteString("*("); OPC.Ident(typ^.strobj); OPM.WriteString("*)"); OPC.CompleteIdent(obj)
						ELSE (*simply cast pointer*)
							OPM.Write("("); OPC.Ident(typ^.strobj); OPM.Write(")"); expr(n^.left, designPrec)
						END
					END
		|	Neguard:
					IF OPM.typchk IN OPM.opt THEN
						IF n^.left^.class = Nvarpar THEN OPM.WriteString("__GUARDEQR(");
							OPC.CompleteIdent(n^.left^.obj); OPM.WriteString(Comma); TypeOf(n^.left);
						ELSE OPM.WriteString("__GUARDEQP("); expr(n^.left^.left, MinPrec)
						END ; (* __GUARDEQx includes deref *)
						OPM.WriteString(Comma); OPC.Ident(n^.left^.typ^.strobj); OPM.Write(")")
					ELSE
						expr(n^.left, MinPrec)	(* always lhs of assignment *)
					END
		|	Nmop:
					IF n^.subcl = val THEN design(n^.left, prec) END
		END ;
		IF prec > designPrec THEN OPM.Write(CloseParen) END
	END design;

	PROCEDURE ActualPar(n: OPT.Node; fp: OPT.Object);
		VAR typ, aptyp: OPT.Struct; comp, form, mode, prec, dim: SHORTINT;
	BEGIN
		OPM.Write(OpenParen);
		WHILE n # NIL DO typ := fp^.typ;
			IF (typ^.form = n^.typ^.form) & (typ^.form IN {Byte..Set}) THEN typ := n^.typ END;
			comp := typ^.comp; form := typ^.form; mode := fp^.mode; prec := MinPrec;
			IF (mode = VarPar) & (n^.class = Nmop) & (n^.subcl = val) THEN	(* avoid cast in lvalue *)
				OPM.Write(OpenParen); OPC.Ident(n^.typ^.strobj); OPM.WriteString("*)"); prec := 10
			END;
			IF ~(n^.typ^.comp IN {Array, DynArr}) THEN
				IF (mode = VarPar) & ~((comp IN {Array, DynArr}) & (n^.class = Nconst)) THEN
					IF n^.typ^.form # NilTyp THEN
						IF ansi & (typ # n^.typ) THEN OPM.WriteString("(void*)") END;
						OPM.Write("&")
					END; prec := 9
				ELSIF ansi THEN
					IF (comp IN {Array, DynArr}) & (n^.class = Nconst) THEN
						OPM.WriteString("(CHAR*)")	(* force to unsigned char *)
					ELSIF (form = Pointer) & (typ # n^.typ) & (n^.typ # OPT.niltyp) THEN
						OPM.WriteString("(void*)")	(* type extension *)
					ELSIF (comp = Record) & (n^.typ # typ) THEN (* record value projection *)
						OPM.WriteString("*("); OPC.Andent(typ); OPM.WriteString("*)&"); prec := 9
					END
				ELSE
					IF (form IN {Real, LReal}) & (n^.typ^.form IN {Byte, SInt, Int, LInt}) THEN (* real promotion *)
						OPM.WriteString("(double)"); prec := 9
					ELSIF (form = LInt) & (n^.typ^.form < LInt) THEN (* integral promotion *)
						OPM.WriteString("(LONGINT)"); prec := 9
					END
				END
			ELSIF ansi THEN
				(* casting of params should be simplified eventually *)
				IF ((mode = VarPar) OR (typ^.BaseTyp^.form = Comp)) & (typ # n^.typ) & (prec = MinPrec) THEN
					OPM.WriteString("(void*)")
				END
			END;
			IF (mode = VarPar) & (n^.class = Nmop) & (n^.subcl = val) THEN expr(n^.left, prec)	(* avoid cast in lvalue *)
			ELSE expr(n, prec)
			END;
			IF (form = LInt) & (n^.class = Nconst)
			& (n^.conval^.intval <= MAX(INTEGER)) & (n^.conval^.intval >= MIN(INTEGER)) THEN
				OPM.PromoteIntConstToLInt()
			ELSIF (comp = Record) & (mode = VarPar) THEN
				IF ~ODD(typ^.sysflag) THEN OPM.WriteString(", "); TypeOf(n) END
			ELSIF (comp = DynArr) & ~ODD(typ^.sysflag) THEN
				IF ODD(n^.typ^.sysflag) & (n^.typ^.comp # Array) THEN OPM.err(137) END;
				IF n^.class = Nconst THEN (* ap is string constant *)
					OPM.WriteString(Comma);
					IF OPM.IndexSize > 4 THEN OPM.WriteString("(LONGINT)") END;
					OPM.WriteInt(n^.conval^.intval2)
				ELSE
					aptyp := n^.typ; dim := 0;
					WHILE (typ^.comp = DynArr) & (typ^.BaseTyp^.form # Byte) DO
						OPM.WriteString(Comma); Len(n, dim, TRUE);
						typ := typ^.BaseTyp; aptyp := aptyp^.BaseTyp; INC(dim)
					END ;
					IF (typ^.comp = DynArr) & (typ^.BaseTyp^.form = Byte) THEN
						OPM.WriteString(Comma);
						WHILE aptyp^.comp = DynArr DO
							Len(n, dim, FALSE); OPM.WriteString(" * "); INC(dim); aptyp := aptyp^.BaseTyp
						END ;
						OPM.WriteInt(aptyp^.size); OPM.PromoteIntConstToLInt()
					END
				END
			END ;
			n := n^.link; fp := fp^.link;
			IF n # NIL THEN OPM.WriteString(Comma) END
		END ;
		OPM.Write(CloseParen)
	END ActualPar;

	PROCEDURE SuperProc(n: OPT.Node): OPT.Object;
		VAR obj: OPT.Object; typ: OPT.Struct;
	BEGIN typ := n^.right^.typ;	(* receiver type *)
		IF typ^.form = Pointer THEN typ := typ^.BaseTyp END ;
		OPT.FindField(n^.left^.obj^.name^, typ^.BaseTyp, obj);
		RETURN obj
	END SuperProc;

	PROCEDURE^ compStat (n: OPT.Node; exp: BOOLEAN);

	PROCEDURE OpParentheses (ePrec, prec: SHORTINT): BOOLEAN;	(* to silence Clang warnings *)
		CONST Clang = TRUE;
	BEGIN
		RETURN (ePrec = 4) & (prec = 2) OR (ePrec = 1) & (prec = 0)
	END OpParentheses;

	PROCEDURE expr_val (n: OPT.Node; prec: SHORTINT; lvalue: BOOLEAN);
		VAR
			class: SHORTINT;
			subclass: SHORTINT;
			form: SHORTINT;
			exprPrec: SHORTINT;
			typ: OPT.Struct;
			l, r: OPT.Node;
			proc: OPT.Object;
	BEGIN
		class := n^.class; subclass := n^.subcl; form := n^.typ^.form;
		l := n^.left; r := n^.right;
		exprPrec := Precedence (class, subclass, form, n^.typ^.comp);
		IF ((exprPrec <= prec) OR OpParentheses(exprPrec, prec)) & (class IN {Nconst, Nupto, Nmop, Ndop, Ncall, Nguard, Neguard}) THEN
			OPM.Write(OpenParen)
		END;
		CASE class OF
			Nconst:
					OPC.Constant(n^.conval, form)
		|	Nupto:	(* n^.typ = OPT.settyp *)
					OPM.WriteString(SetRangeFunc); expr(l, MinPrec); OPM.WriteString(Comma); expr (r, MinPrec);
					OPM.Write(CloseParen)
		|	Nmop:
					CASE subclass OF
						not:
								OPM.Write("!"); expr(l, exprPrec)
					|	minus:
								IF form = Set THEN OPM.Write("~") ELSE OPM.Write("-"); END ;
								expr(l, exprPrec)
					|	is:
								typ := n^.obj^.typ;
								IF l^.typ^.comp = Record THEN OPM.WriteString(IsFunc); OPC.TypeOf(l^.obj)
								ELSE OPM.WriteString(IsPFunc); expr(l, MinPrec); typ := typ^.BaseTyp
								END ;
								OPM.WriteString(Comma);
								OPC.Andent(typ); OPM.WriteString(Comma);
								OPM.WriteInt(typ^.extlev); OPM.Write(")")
					|	conv:
								IF form = String THEN
									expr(l, exprPrec)
								ELSE
									Convert(l, form, exprPrec)
								END
					|	abs:
								IF SideEffects(l) THEN
									IF l^.typ^.form = LReal THEN OPM.WriteString("__ABSFD(")
									ELSIF l^.typ^.form = Real THEN OPM.WriteString("__ABSFF(")
									ELSIF l^.typ^.form = LInt THEN OPM.WriteString("__ABSFL(")
									ELSIF (l^.typ^.form < Int) & (OPM.AdrSize = 2) THEN OPM.WriteString("__ABSFS(")
									ELSE OPM.WriteString("__ABSF(")
									END
								ELSE OPM.WriteString("__ABS(")
								END;
								expr(l, MinPrec); OPM.Write(CloseParen)
					|	cap:
								OPM.WriteString("__CAP("); expr(l, MinPrec); OPM.Write(CloseParen)
					|	odd:
								OPM.WriteString("__ODD("); expr(l, MinPrec); OPM.Write(CloseParen)
					|	adr: (*SYSTEM*)
								IF ~oldc THEN OPM.WriteString("(SYSTEM_ADRINT)") END;
								IF l^.class = Nvarpar THEN OPC.CompleteIdent(l^.obj)
								ELSE
									IF (l^.typ^.form # String) & ~(l^.typ^.comp IN {Array, DynArr}) THEN OPM.Write("&") END;
									expr(l, exprPrec)
								END
					|	val: (*SYSTEM*)
								IF ~lvalue & ~(n^.typ^.form IN {Real, LReal, Comp})
								& ~(l^.typ^.form IN {Real, LReal, Comp}) THEN	(* use cast *)
									OPM.Write(OpenParen); OPC.Ident(n^.typ^.strobj); OPM.Write(CloseParen);
									expr(l, exprPrec)
								ELSIF l^.class IN {Nvar, Nvarpar, Nfield, Nindex} THEN	(* lvalue *)
									OPM.WriteString("__VAL("); OPC.Ident(n^.typ^.strobj); OPM.WriteString(Comma);
									expr(l, MinPrec); OPM.Write(CloseParen)
								ELSE	(* use real <-> int conversions *)
									IF n^.typ^.form = Real THEN OPM.WriteString("__VALSR((INTEGER)")
									ELSIF n^.typ^.form = LReal THEN OPM.WriteString("__VALR((LONGINT)")
									ELSE OPM.Write("("); OPC.Ident(n^.typ^.strobj); OPM.Write(")");
									END;
									IF l^.typ^.form = Real THEN OPM.WriteString("__VALI("); expr(l, MinPrec); OPM.Write(")")
									ELSIF l.typ.form = LReal THEN OPM.WriteString("__VALL("); expr(l, MinPrec); OPM.Write(")")
									ELSE expr(l, exprPrec)
									END;
									IF n.typ.form IN {Real, LReal} THEN OPM.Write(")") END
								END
					|	bit:
								OPM.WriteString(SetOfFunc); expr(l, MinPrec); OPM.Write(CloseParen)
					|	unsgn:
								CASE form OF
								| Byte: OPM.WriteString("(__U_BYTE)")
								| SInt: OPM.WriteString("(__U_SHORTINT)")
								| Int: OPM.WriteString("(__U_INTEGER)")
								ELSE OPM.WriteString("(__U_LONGINT)")
								END;
								OPM.Write("("); expr(l, MinPrec); OPM.Write(")")
					ELSE OPM.err(200)
					END
		|	Ndop:
					CASE subclass OF
					  len:
					  		Len(l, SHORT(r^.conval^.intval), FALSE)
					| in, ash, msk, bit, lsh, rot, div, mod, asr, lsl, ror, min, max:
							CASE subclass OF
							|	in:
										OPM.WriteString("__IN(")
							|	ash:
										IF r^.class = Nconst THEN
											IF r^.conval^.intval >= 0 THEN OPM.WriteString("__ASHL(")
											ELSE OPM.WriteString("__ASHR(")
											END
(*
	The macro __ASH(x, n) uses n twice. If n is not a constant and not a simple
	variable but is a function that may have a side-effect, it would produce this
	side-effect twice. The function __ASHF is used to avoid using n twice. J.Templ
*)
										ELSIF ~SideEffects(r) THEN OPM.WriteString("__ASH(")
										ELSIF form = LInt THEN OPM.WriteString("__ASHFL(")
										ELSE OPM.WriteString("__ASHF(")
										END
							|	asr:
										OPM.WriteString("__ASHR(")
							|	lsl:
										OPM.WriteString("__LSHL(")
							|	ror:
										OPM.WriteString("__ROTR(")
							|	msk:
										OPM.WriteString("__MASK(");
							|	bit:
										OPM.WriteString("__BIT(")
							|	lsh:
										IF r^.class = Nconst THEN
											IF r^.conval^.intval >= 0 THEN OPM.WriteString("__LSHL(")
											ELSE OPM.WriteString("__LSHR(")
											END
										ELSE OPM.WriteString("__LSH(")
										END
							|	rot:
										IF r^.class = Nconst THEN
											IF r^.conval^.intval >= 0 THEN OPM.WriteString("__ROTL(")
											ELSE OPM.WriteString("__ROTR(")
											END
										ELSE OPM.WriteString("__ROT(")
										END
							|	div:
										IF ~SideEffects(n) & (r^.class = Nconst) & (r^.conval^.intval > 0) THEN OPM.WriteString("__DIV(")
										ELSIF form = LInt THEN OPM.WriteString("__DIVFL(")
										ELSE OPM.WriteString("__DIVF(")
										END
							|	mod:
										IF ~SideEffects(n) & (r^.class = Nconst) & (r^.conval^.intval > 0) THEN OPM.WriteString("__MOD(")
										ELSIF form = LInt THEN OPM.WriteString("__MODFL(")
										ELSE OPM.WriteString("__MODF(")
										END
							|	min:
										IF ~SideEffects(n) THEN OPM.WriteString("__MIN(")
										ELSIF form = Real THEN OPM.WriteString("__MINFF(")
										ELSIF form = LReal THEN OPM.WriteString("__MINFD(")
										ELSIF form = LInt THEN OPM.WriteString("__MINFL(")
										ELSE OPM.WriteString("__MINF(")
										END
							|	max:
										IF ~SideEffects(n) THEN OPM.WriteString("__MAX(")
										ELSIF form = Real THEN OPM.WriteString("__MAXFF(")
										ELSIF form = LReal THEN OPM.WriteString("__MAXFD(")
										ELSIF form = LInt THEN OPM.WriteString("__MAXFL(")
										ELSE OPM.WriteString("__MAXF(")
										END
							END;
							expr(l, MinPrec);
							OPM.WriteString(Comma);
							IF (subclass IN {ash, lsh, rot}) & (r^.class = Nconst) & (r^.conval^.intval < 0) THEN
								OPM.WriteInt(-r^.conval^.intval)
							ELSE expr(r, MinPrec)
							END;
							IF subclass IN {ash, lsh, rot, asr, lsl, ror} THEN
								OPM.WriteString(Comma); OPC.Ident(l^.typ^.strobj)
							END;
							OPM.Write(CloseParen)
					| eql .. geq:
							IF l^.typ^.form IN {String, Comp} THEN
								OPM.WriteString("__STRCMP(");
								expr(l, MinPrec); OPM.WriteString(Comma); expr(r, MinPrec); OPM.Write(CloseParen);
								OPC.Cmp(subclass); OPM.Write("0")
							ELSE
								expr(l, exprPrec); OPC.Cmp(subclass);
								typ := l^.typ;
								IF (typ^.form = Pointer) & (r^.typ.form # NilTyp) & (r^.typ # typ) & (r^.typ # OPT.sysptrtyp) THEN
									OPM.WriteString("(void *) ")
								END;
								expr(r, exprPrec)
							END
					ELSE
						expr(l, exprPrec);
						CASE subclass OF
							times:
									IF form = Set THEN OPM.WriteString(" & ")
										ELSE OPM.WriteString(" * ")
									END
						|	slash:
									IF form = Set THEN OPM.WriteString(" ^ ")
									ELSE OPM.WriteString(" / ");
										IF (r^.obj = NIL) OR (r^.obj^.typ^.form IN {Byte, SInt, Int, LInt}) THEN
											OPM.Write(OpenParen); OPC.Ident(n^.typ^.strobj); OPM.Write(CloseParen)
										END
									END
						|	and:
									OPM.WriteString(" && ")
						|	plus:
									IF form = String THEN OPM.err(265)
									ELSIF form = Set THEN OPM.WriteString(" | ")
									ELSE OPM.WriteString(" + ")
									END
						|	minus:
									IF form = Set THEN OPM.WriteString(" & ~")
									ELSE OPM.WriteString(" - ")
									END
						|	div0:
									OPM.WriteString(" / ")
						|	rem0:
									OPM.WriteString(" % ")
						|	or:
									OPM.WriteString(" || ")
						END;
						expr(r, exprPrec)
					END
		|	Ncall:
					IF (l^.obj # NIL) & (l^.obj^.mode = TProc) THEN
						IF l^.subcl = super THEN proc := SuperProc(n)
						ELSE OPM.WriteString("__"); proc := OPC.BaseTProc(l^.obj)
						END ;
						OPC.Ident(proc);
						n^.obj := proc^.link
					ELSIF l^.class = Nproc THEN design(l, 10)
					ELSE design(l, ProcTypeVar)
					END ;
					ActualPar(r, n^.obj)
		|	Ncomp:
					OPM.Write("("); compStat(l, TRUE); expr(r, MinPrec); OPM.Write(")")
		ELSE
					design(n, prec); (* not exprPrec! *)
		END;
		IF ((exprPrec <= prec) OR OpParentheses(exprPrec, prec)) & (class IN {Nconst, Nupto, Nmop, Ndop, Ncall, Nguard}) THEN
			OPM.Write(CloseParen)
		END
	END expr_val;

	PROCEDURE expr (n: OPT.Node; prec: SHORTINT);
	BEGIN
		expr_val(n, prec, FALSE)
	END expr;

	PROCEDURE^ stat(n: OPT.Node; outerProc: OPT.Object);

	PROCEDURE IfStat(n: OPT.Node; withtrap: BOOLEAN; outerProc: OPT.Object);
		VAR if: OPT.Node; obj: OPT.Object; typ: OPT.Struct; adr: INTEGER;
	BEGIN	(* n^.class IN {Nifelse, Nwith} *)
		if := n^.left; (* name := ""; *)
		WHILE if # NIL DO
			OPM.WriteString("if "); expr(if^.left, MaxPrec);	(* if *)
			OPM.Write(Blank); OPC.BegBlk;
			IF (n^.class = Nwith) & (if^.left^.left # NIL) THEN (* watch out for const expr *)
				obj := if^.left^.left^.obj; typ := obj^.typ; adr := obj^.adr;
				IF typ^.comp = Record THEN
					(* introduce alias pointer for var records; T1 *name__ = rec; *)
					OPC.BegStat; OPC.Ident(if^.left^.obj); OPM.WriteString(" *");
					OPM.WriteString(obj.name^); OPM.WriteString("__ = (void*)");
					obj^.adr := 0; (* for nested WITH with same variable; always take the original name *)
					OPC.CompleteIdent(obj);
					OPC.EndStat
				END ;
				obj^.adr := 1;	(* signal special handling of variable name to OPC.CompleteIdent *)
				obj^.typ := if^.left^.obj^.typ;
				stat(if^.right, outerProc);
				obj^.typ := typ; obj^.adr := adr
			ELSE
				stat(if^.right, outerProc)
			END ;
			if := if^.link;
			IF (if # NIL) OR (n^.right # NIL) OR withtrap THEN OPC.EndBlk0(); OPM.WriteString(" else ");
			ELSE OPC.EndBlk()
			END
		END;
		IF withtrap THEN OPM.WriteString(WithChk); OPM.WriteModPos; OPM.Write(")"); OPC.EndStat
		ELSIF n^.right # NIL THEN OPC.BegBlk; stat(n^.right, outerProc); OPC.EndBlk
		END
	END IfStat;

	PROCEDURE CaseStat(n: OPT.Node; outerProc: OPT.Object);
		VAR switchCase, label: OPT.Node;
			low, high: INTEGER; form, i: SHORTINT;
	BEGIN
		OPM.WriteString("switch "); expr(n^.left, MaxPrec);
		OPM.Write(Blank); OPC.BegBlk;
		form := n^.left^.typ^.form;
		switchCase := n^.right^.left;
		WHILE switchCase # NIL DO	(* switchCase^.class = Ncasedo *)
			label := switchCase^.left;
			i := 0;
			WHILE label # NIL DO (* label^.class = NConst *)
				low := SHORT(label^.conval^.intval);
				high := label^.conval^.intval2;
				WHILE low <= high DO
					IF i = 0 THEN OPC.BegStat END ;
					OPC.Case(low, form);
					INC(low); INC(i);
					IF i = 5 THEN OPM.WriteLn; i := 0 END
				END ;
				label := label^.link
			END ;
			IF i > 0 THEN OPM.WriteLn END ;
			OPC.Indent(1);
			stat(switchCase^.right, outerProc);
			OPC.BegStat; OPM.WriteString(Break); OPC.EndStat;
			OPC.Indent(-1);
			switchCase := switchCase^.link
		END ;
		OPC.BegStat; OPM.WriteString("default: ");
		IF n^.right^.conval^.setval # {} THEN	(* else branch *)
			OPC.Indent(1); OPM.WriteLn; stat(n^.right^.right, outerProc);
			OPC.BegStat; OPM.WriteString(Break); OPC.Indent(-1)
		ELSE
			OPM.WriteString("__CASECHK("); OPM.WriteModPos; OPM.Write(")")
		END;
		OPC.EndStat; OPC.EndBlk
	END CaseStat;

	PROCEDURE ImplicitReturn(n: OPT.Node): BOOLEAN;
	BEGIN
		WHILE (n # NIL) & (n.class # Nreturn) DO n := n^.link END ;
		RETURN n = NIL
	END ImplicitReturn;

	PROCEDURE NewArr(d, x: OPT.Node);
		VAR typ, base: OPT.Struct; nofdim, nofdyn: SHORTINT;
	BEGIN
		typ := d^.typ^.BaseTyp; base := typ; nofdim := 0; nofdyn := 0;
		WHILE base^.comp = DynArr DO INC(nofdim); INC(nofdyn); base := base^.BaseTyp END ;
		design(d, MinPrec); OPM.WriteString(" = __NEWARR(");
		WHILE base^.comp = Array DO INC(nofdim); base := base^.BaseTyp END ;
		IF (base^.comp = Record) & (OPC.NofPtrs(base) # 0) THEN
			IF ODD(base^.sysflag) THEN OPM.err(138) END;
			OPC.Andent(base); OPM.WriteString(DynTypExt)
		ELSIF base^.form = Pointer THEN OPM.WriteString("POINTER__typ")
		ELSE OPM.WriteString("NIL")
		END ;
		OPM.WriteString(", "); OPM.WriteInt(base^.size); OPM.PromoteIntConstToLInt();	(* element size *)
		OPM.WriteString(", "); OPM.WriteInt(OPC.BaseAlignment(base));	(* element alignment *)
		OPM.WriteString(", "); OPM.WriteInt(nofdim);	(* total number of dimensions = number of additional parameters *)
		OPM.WriteString(", "); OPM.WriteInt(nofdyn);	(* number of dynamic dimensions *)
		WHILE typ # base DO
			OPM.WriteString(", (SYSTEM_ARRLEN)");
			IF typ^.comp = DynArr THEN
				IF x^.class = Nconst THEN expr(x, MinPrec) ELSE expr(x, 10) END;
				x := x^.link
			ELSE OPM.WriteInt(typ^.n)
			END;
			typ := typ^.BaseTyp
		END;
		OPM.Write(")")
	END NewArr;

	PROCEDURE DefineTDescs(n: OPT.Node);
	BEGIN
		WHILE (n # NIL) & (n^.class = Ninittd) DO OPC.TDescDecl(n^.typ); n := n^.link END
	END DefineTDescs;

	PROCEDURE InitTDescs(n: OPT.Node);
	BEGIN
		WHILE (n # NIL) & (n^.class = Ninittd) DO OPC.InitTDesc(n^.typ); n := n^.link END
	END InitTDescs;

	PROCEDURE AddCopy (left, right: OPT.Node; first: BOOLEAN);
	BEGIN
		IF first THEN OPM.WriteString("__STRCOPY(") ELSE OPM.WriteString("__STRAPND(") END;
		IF ansi & (right^.class = Nconst) THEN OPM.WriteString("(CHAR*)") END;
		expr(right, MinPrec); OPM.WriteString(Comma); expr(left, MinPrec); OPM.WriteString(Comma);
		IF left^.typ^.sysflag # 0 THEN OPM.WriteString("-1") ELSE Len(left, 0, FALSE) END;
		OPM.WriteString(Comma); OPM.WriteModPos;
		OPM.Write(")")
	END AddCopy;

	PROCEDURE StringCopy (left, right: OPT.Node; exp: BOOLEAN);
	BEGIN
		ASSERT(right^.class # Nconst);
		IF right^.class = Ndop THEN
			ASSERT(right^.subcl = plus);
			IF ~SameExp(left, right^.left) THEN
				AddCopy(left, right^.left, TRUE);
				IF exp THEN OPM.WriteString(", ") ELSE OPC.EndStat; OPC.BegStat END
			END;
			right := right^.right;
			WHILE right^.class = Ndop DO
				ASSERT(right^.subcl = plus);
				AddCopy(left, right^.left, FALSE);
				IF exp THEN OPM.WriteString(", ") ELSE OPC.EndStat; OPC.BegStat END;
				right := right^.right
			END;
			AddCopy(left, right, FALSE)
		ELSE
			AddCopy(left, right, TRUE)
		END
	END StringCopy;

	PROCEDURE compStat (n: OPT.Node; exp: BOOLEAN);
		VAR l, r: OPT.Node;
	BEGIN
		WHILE (n # NIL) & OPM.noerr DO
			IF ~exp THEN OPC.BegStat END;
			ASSERT(n^.class = Nassign);
			l := n^.left; r := n^.right;
			IF n^.subcl = assign THEN
				IF r^.typ^.form = String THEN
					IF r^.class # Nconst THEN
						StringCopy(l, r, exp)
					ELSE
						OPM.WriteString(MoveFunc);
						expr(r, MinPrec); OPM.WriteString(Comma); expr(l, MinPrec); OPM.WriteString(Comma);
						OPM.WriteInt(r^.conval^.intval2 * r^.typ^.BaseTyp.size); OPM.Write(CloseParen)
					END
				ELSE ASSERT(l^.typ^.form IN {Pointer, Char});
					design(l, MinPrec); OPM.WriteString(" = "); expr(r, MinPrec)
				END
			ELSE ASSERT(n^.subcl = newfn);
				ASSERT(l^.typ^.BaseTyp^.comp = DynArr);
				NewArr(l, r)
			END;
			IF exp THEN OPM.WriteString(", "); OPM.WriteLn; OPC.BegStat; OPM.Write(9X) ELSE OPC.EndStat END;
			n := n^.link
		END
	END compStat;

	PROCEDURE stat(n: OPT.Node; outerProc: OPT.Object);
		VAR proc: OPT.Object; saved: ExitInfo; l, r: OPT.Node; i: INTEGER; ch: SHORTCHAR;

		(* строит условие цикла do...while() для оператора FOR *)
		PROCEDURE ForUntil (id: OPT.Node; cond: SHORTINT; z: OPT.Node);
		(* id-переменная цикла, cond - условие завершения цикла, z -шаг *)
			VAR step: LONGINT;
		BEGIN
			step := z^.conval^.intval;
			IF cond = neq THEN OPM.WriteString( "!" ) ; END;
			IF step = 1 THEN OPM.WriteString( "++" ) ; OPC.CompleteIdent(id^.obj)
			ELSIF step = -1 THEN OPM.WriteString( "--" ) ; OPC.CompleteIdent(id^.obj)
			ELSIF step = 0 THEN OPC.CompleteIdent(id^.obj)  (* только сравнение, изменение переменной происходит в начале цикла *)
			ELSE (* шаг не единичный *)
				IF cond # eql THEN OPM.WriteString( "(" ) END;
				OPC.CompleteIdent(id^.obj); OPM.WriteString( "+=" ); expr(z, MinPrec);
				IF cond # eql THEN OPM.WriteString( ")" ) END;
			END;
			CASE cond OF (* условие ~cond *)
			eql, neq :    (* empty *);
			| lss  : OPM.WriteString(" >= 0");
			| leq  : OPM.WriteString(" > 0");
			| gtr   : OPM.WriteString(" <= 0");
			| geq : OPM.WriteString(" < 0");
			END;
		END ForUntil;

	BEGIN
		WHILE (n # NIL) & OPM.noerr DO
			OPM.errpos := SHORT(n^.conval^.intval);
			IF ~(n^.class IN {Ninittd, Ncomp, Nraw}) THEN OPC.BegStat END;
			CASE n^.class OF
				Nenter:
						IF n^.obj = NIL THEN (* enter module *)
							INC(OPM.level); stat(n^.left, outerProc); DEC(OPM.level);
							IF ~(OPM.noinit IN OPM.opt) THEN
								OPC.GenEnumPtrs(OPT.topScope^.scope);
								DefineTDescs(n^.right); OPC.EnterBody; InitTDescs(n^.right);
								OPM.WriteString("/* BEGIN */"); OPM.WriteLn;
								stat(n^.right, outerProc); OPC.ExitBody
							END
						ELSE (* enter proc *)
							proc := n^.obj;
							OPC.TypeDefs(proc^.scope^.right, 0);
							IF ~proc^.scope^.leaf THEN OPC.DefineInter (proc) END ; (* define intermediate procedure scope *)
							INC(OPM.level); stat(n^.left, proc); DEC(OPM.level);
							OPC.EnterProc(proc); stat(n^.right, proc);
							OPC.ExitProc(proc, TRUE, ImplicitReturn(n^.right));
						END
			|	Ninittd: (* done in enter module *)
			|	Nassign:
					CASE n^.subcl OF
						assign:
								l := n^.left; r := n^.right;
								IF l^.typ^.comp IN {Array, DynArr} THEN
									IF (r^.typ^.form = String) & (r^.class # Nconst) THEN
										StringCopy(l, r, FALSE)
									ELSE
										OPM.WriteString(MoveFunc);
										expr(r, MinPrec); OPM.WriteString(Comma); expr(l, MinPrec); OPM.WriteString(Comma);
										IF r^.typ = OPT.stringtyp THEN i := r^.conval^.intval2 ELSE i := r^.typ^.size END;
										OPM.WriteInt(MIN(i, l^.typ^.size)); OPM.Write(CloseParen)
									END
								ELSE
									IF (l^.typ^.form = Pointer) & (l^.obj # NIL) & (l^.obj^.adr = 1) & (l^.obj^.mode = Var) THEN
										l^.obj^.adr := 0; design(l, MinPrec); l^.obj^.adr := 1;			(* avoid cast of WITH-variable *)
										IF r^.typ^.form # NilTyp THEN OPM.WriteString(" = (void*)")
										ELSE OPM.WriteString(" = ")
										END
									ELSE
										design(l, MinPrec); OPM.WriteString(" = ")
									END ;
									IF l^.typ = r^.typ THEN expr(r, MinPrec)
									ELSIF (l^.typ^.form = Pointer) & (r^.typ^.form # NilTyp) & (l^.typ^.strobj # NIL) THEN
										OPM.Write("("); OPC.Ident(l^.typ^.strobj); OPM.Write(")"); expr(r, MinPrec)
									ELSIF l^.typ^.comp = Record THEN
										OPM.WriteString("*("); OPC.Andent(l^.typ); OPM.WriteString("*)&"); expr(r, 9)
									ELSE expr(r, MinPrec)
									END
								END
					|	newfn:
								IF n^.left^.typ^.BaseTyp^.comp = Record THEN
									OPM.WriteString("__NEW("); design(n^.left, MinPrec); OPM.WriteString(", ");
									OPC.Andent(n^.left^.typ^.BaseTyp); OPM.WriteString(")")
								ELSIF n^.left^.typ^.BaseTyp^.comp IN {Array, DynArr} THEN
									NewArr(n^.left, n^.right)
								END
					|	incfn, decfn:
								expr_val(n^.left, MinPrec, TRUE); OPC.Increment(n^.subcl = decfn); expr(n^.right, MinPrec)
					|	inclfn, exclfn:
								expr_val(n^.left, MinPrec, TRUE); OPC.SetInclude(n^.subcl = exclfn); OPM.WriteString(SetOfFunc); expr(n^.right, MinPrec);
								OPM.Write(CloseParen)
					|	copyfn:
								OPM.WriteString(CopyFunc);
								expr(n^.right, MinPrec); OPM.WriteString(", "); expr(n^.left, MinPrec); OPM.WriteString(", ");
								IF (n^.left^.typ^.comp = Array) & (n^.right^.typ^.comp = Array) THEN
									OPM.WriteInt(MIN(n^.left^.typ^.n, n^.right^.typ^.n))
								ELSIF (n^.left^.typ^.comp = Array) & (n^.right^.typ = OPT.stringtyp) & (n^.right^.conval # NIL) THEN
									OPM.WriteInt(MIN(n^.left^.typ^.n, n^.right^.conval^.intval2))
								ELSE
									OPM.WriteString("__MIN("); Len(n^.left, 0, FALSE); OPM.WriteString(Comma);
									Len(n^.right, 0, FALSE); OPM.Write(CloseParen)
								END;
								OPM.Write(CloseParen)
					|	packfn:
								OPM.WriteString("__PACK(&"); expr(n^.left, MinPrec); OPM.WriteString(Comma);
								expr(n^.right, MinPrec); OPM.Write(CloseParen)
					|	unpkfn:
								OPM.WriteString("__UNPK(&"); expr(n^.left, MinPrec); OPM.WriteString(Comma + "&");
								expr(n^.right, MinPrec); OPM.Write(CloseParen)
					|	(*SYSTEM*)movefn:
								OPM.WriteString(MoveFunc);
								expr(n^.right, MinPrec); OPM.WriteString(Comma); expr(n^.left, MinPrec); OPM.WriteString(Comma);
								expr(n^.right^.link, MinPrec);
								OPM.Write(CloseParen)
					|	(*SYSTEM*)getfn:
								OPM.WriteString(GetFunc); expr(n^.right, MinPrec); OPM.WriteString(Comma); expr(n^.left, MinPrec);
								OPM.WriteString(Comma); OPC.Ident(n^.left^.typ^.strobj); OPM.Write(CloseParen)
					|	(*SYSTEM*)putfn:
								OPM.WriteString(PutFunc); expr(n^.left, MinPrec); OPM.WriteString(Comma); expr(n^.right, MinPrec);
								OPM.WriteString(Comma); OPC.Ident(n^.right^.typ^.strobj); OPM.Write(CloseParen)
					|	(*SYSTEM*)sysnewfn:
								OPM.WriteString("__SYSNEW(");
								design(n^.left, MinPrec); OPM.WriteString(", ");
								expr(n^.right, MinPrec);
								OPM.Write(")")
					END
			|	Ncall:
						IF (n^.left^.obj # NIL) & (n^.left^.obj^.mode = TProc) THEN
							IF n^.left^.subcl = super THEN proc := SuperProc(n)
							ELSE OPM.WriteString("__"); proc := OPC.BaseTProc(n^.left^.obj)
							END ;
							OPC.Ident(proc);
							n^.obj := proc^.link
						ELSIF n^.left^.class = Nproc THEN design(n^.left, 10)
						ELSE design(n^.left, ProcTypeVar)
						END ;
						ActualPar(n^.right, n^.obj)
			|	Nifelse:
						IF n^.subcl # assertfn THEN IfStat(n, FALSE, outerProc)
						ELSIF assert THEN OPM.WriteString("__ASSERT("); expr(n^.left^.left^.left, MinPrec); OPM.WriteString(Comma);
							OPM.WriteInt(n^.left^.right^.right^.conval^.intval); OPM.WriteString(Comma); OPM.WriteModPos; OPM.Write(CloseParen); OPC.EndStat
						END
			|	Ncase:
						INC(exit.level); CaseStat(n, outerProc); DEC(exit.level)
			|	Nwhile:
						INC(exit.level); OPM.WriteString("while "); expr(n^.left, MaxPrec);
						OPM.Write(Blank); OPC.BegBlk; stat(n^.right, outerProc); OPC.EndBlk;
						DEC(exit.level)
			|	Nrepeat:
						INC(exit.level); OPM.WriteString("do "); OPC.BegBlk; stat(n^.left, outerProc); OPC.EndBlk0;
						IF n^.right^.class = Nassign THEN (* цикл FOR *)
							OPM.WriteString(" while (");
							ForUntil(n^.right^.left, n^.right^.subcl, n^.right^.right);
							OPM.Write(")")
						ELSE (* обычный REPEAT/UNTIL *)
							OPM.WriteString(" while (!");  expr(n^.right, 9); OPM.Write(CloseParen)
						END;
						DEC(exit.level)
			|	Nloop:
						saved := exit; exit.level := 0; exit.label := -1;
						OPM.WriteString("for (;;) "); OPC.BegBlk; stat(n^.left, outerProc); OPC.EndBlk;
						IF exit.label # -1 THEN
							OPC.BegStat; OPM.WriteString("exit__"); OPM.WriteInt(exit.label); OPM.Write(":"); OPC.EndStat
						END ;
						exit := saved
			|	Nexit:
						IF exit.level = 0 THEN OPM.WriteString(Break)
						ELSE
							IF exit.label = -1 THEN exit.label := nofExitLabels; INC(nofExitLabels) END ;
							OPM.WriteString("goto exit__"); OPM.WriteInt(exit.label)
						END
			|	Nreturn:
						IF OPM.level = 0 THEN
							IF mainprog THEN OPM.WriteString("__FINI") ELSE OPM.WriteString("__ENDMOD") END
						ELSIF OPC.NeedsRetval(outerProc) THEN
							OPM.WriteString("__retval = ");
							IF (n^.left^.typ^.form = Pointer) & (n^.obj^.typ # n^.left^.typ) THEN
								OPM.WriteString("(void*)"); expr(n^.left, 10)
							ELSE
								expr(n^.left, MinPrec)
							END ;
							OPC.EndStat;
							OPC.BegStat; OPC.ExitProc(outerProc, FALSE, FALSE); OPC.EndStat;
							OPC.BegStat; OPM.WriteString("return __retval")
						ELSE
							OPC.ExitProc(outerProc, FALSE, FALSE);
							OPM.WriteString("return");
							IF n^.left # NIL THEN OPM.Write(Blank);
								IF (n^.left^.typ^.form = Pointer) & (n^.obj^.typ # n^.left^.typ) THEN
									OPM.WriteString("(void*)"); expr(n^.left, 10)
								ELSE
									expr(n^.left, MinPrec)
								END
							END
						END
			|	Nwith:
						IfStat(n, n^.subcl = 0, outerProc)
			|	Ntrap:
						OPC.Halt(SHORT(n^.right^.conval^.intval))
			|	Ncomp:
						compStat(n^.left, FALSE); stat(n^.right, outerProc)
			|	Nraw:
						IF OPT.SYSimported THEN
							i := 0; ch := n^.left^.conval^.ext[0];
							WHILE ch # 0X DO
								IF ch = 0DX THEN OPM.WriteLn ELSE OPM.Write(ch) END;
								INC(i); ch := n^.left^.conval^.ext[i]
							END
						END
			END;
			IF ~(n^.class IN {Nenter, Ninittd, Nifelse, Nwith, Ncase, Nwhile, Nloop, Ncomp, Nraw}) THEN
				OPC.EndStat
			END;
			n := n^.link
		END
	END stat;

	PROCEDURE Module*(prog: OPT.Node);
	BEGIN
		IF ~mainprog THEN OPC.GenHdr(prog^.right); OPC.GenHdrIncludes END;
		IF ~(OPM.foreign IN OPM.opt) THEN OPC.GenBdy(prog^.right); stat(prog, NIL) END;
		OPC.CleanupArrays
	END Module;

END OfrontOPV.
