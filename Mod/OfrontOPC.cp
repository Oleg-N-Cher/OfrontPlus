MODULE OfrontOPC;	(* copyright (c) J. Templ 12.7.95 / 16.5.98 *)

(* C source code generator version

	2000-04-30 jt, synchronized with BlackBox version, in particular
		various promotion rules changed (long) => (LONGINT), xxxL avoided
	2015-10-07 jt, fixed endless recursion in Stars for imported fields of anonymous type POINTER TO DynArr;
		param typ has a strobj pointer to an anonymous object, i.e. typ.strobj.name = OPT.null
	2018-02-14 olegncher, issue #44: unlimited length of strings (as in BB)
*)

	IMPORT OPT := OfrontOPT, OPM := OfrontOPM;

	CONST
		genDelFunc = FALSE;

	CONST
		(* structure forms *)
		Byte = 1; Bool = 2; Char8 = 3; SInt = 4; Int = 5; LInt = 6;
		Real = 7; LReal = 8; Set = 9; String8 = 10; NilTyp = 11; NoTyp = 12;
		Pointer = 13; UByte = 14; ProcTyp = 15; Comp = 16;
		Char16 = 17; String16 = 18;

		(* composite structure forms *)
		Basic = 1; Array = 2; DynArr = 3; Record = 4;

		(* object history *)
		removed = 4;

		(* object modes *)
		Var = 1; VarPar = 2; Fld = 4; Typ = 5; LProc = 6; XProc = 7;
		CProc = 9; Mod = 11; TProc = 13;

		(* symbol values and ops *)
		eql = 11; neq = 12; lss = 13; leq = 14; gtr = 15; geq = 16;

		(* nodes classes *)
		Ninittd = 14;

		(* module visibility of objects *)
		internal = 0; external = 1; externalR = 2; outPar = 4;

		(* sysflag *)
		union = 7; (* must be odd *)

		UndefinedType = 0; (* named type not yet defined *)
		ProcessingType = 1; (* pointer type is being processed *)
		PredefinedType = 2; (* for all predefined types *)
		RecursiveType = 3; (* for all types in recursion *)
		TemporaryType = 4; (* for all types in recursion *)
		MaxType = 5;


		HeaderMsg = "Ofront+ 1.0 -";
		BasicIncludeFile = "SYSTEM";
		Static = "static ";
		Export = "export ";	(* particularily introduced for VC++ declspec() *)
		Extern = "import ";	(* particularily introduced for VC++ declspec() *)
		EXPORT = "__EXPORT ";	(* export into a dynamic library *)
		EXTERN = "__EXTERN ";	(* import from a dynamic library *)
		FASTCALL = "__FASTCALL ";	(* fastcall calling convention *)
		STDCALL = "__STDCALL ";	(* stdcall calling convention *)
		Struct = "struct ";
		Union = "union ";
		LocalScope = "_s"; (* name of a local intermediate scope (variable name) *)
		GlobalScope = "_s"; (* pointer to current scope extension *)
		LinkName = "lnk"; (* pointer to previous scope field *)
		FlagExt = "__oh";
		LenExt = "__len";
		DynTypExt = "__typ";
		TagExt = "__typ";
		RecExt = "__rec";

		OpenParen = "(";
		CloseParen = ")";
		OpenBrace = "{";
		CloseBrace = "}";
		OpenBracket = "[";
		CloseBracket = "]";
		Underscore = "_";
		Quotes = 22X;
		SingleQuote = 27X;
		Colon = ": ";
		Semicolon = ";";
		Comma = ", ";
		Becomes = " = ";
		Star = "*";
		Blank = " ";
		Dot = ".";

		DupFunc = "__DUP("; (* duplication of dynamic arrays *)
		DupArrFunc = "__DUPARR("; (* duplication of fixed size arrays *)
		DelFunc = "__DEL("; (* removal of dynamic arrays *)

		NilConst = "NIL";

		VoidType = "void";
		CaseStat = "case ";

	VAR
		indentLevel: INTEGER;
		ptrinit, mainprog, ansi, oldc, dynlib, windows: BOOLEAN;
		hashtab: ARRAY 105 OF BYTE;
		keytab: ARRAY 36, 9 OF SHORTCHAR;
		GlbPtrs: BOOLEAN;
		BodyNameExt, CloseNameExt: ARRAY 15 OF SHORTCHAR;

		arrayTypes: ARRAY 1024 OF OPT.Struct;	(* table of all unnamed heap allocated array types *)
		arrayIndex: INTEGER;

	PROCEDURE Init*;
	BEGIN
		indentLevel := 0;
		ptrinit := OPM.ptrinit IN OPM.opt;
		mainprog := OPM.mainprog IN OPM.opt;
		ansi := OPM.ansi IN OPM.opt;
		oldc := OPM.oldc IN OPM.opt;
		dynlib := OPM.dynlib IN OPM.opt;
		windows := FALSE;
		IF ansi THEN
			BodyNameExt := "__init (void)"; CloseNameExt := "__close (void)"
		ELSE
			BodyNameExt := "__init ()"; CloseNameExt := "__close ()"
		END;
		arrayIndex := 0
	END Init;

	PROCEDURE Indent* (count: SHORTINT);
	BEGIN INC(indentLevel, count)
	END Indent;

	PROCEDURE BegStat*;
		VAR i: INTEGER;
	BEGIN i := indentLevel;
		WHILE i > 0 DO OPM.WriteTab; DEC(i) END
	END BegStat;

	PROCEDURE EndStat*;
	BEGIN OPM.Write(Semicolon); OPM.WriteLn
	END EndStat;

	PROCEDURE BegBlk*;
	BEGIN OPM.Write(OpenBrace); OPM.WriteLn; INC(indentLevel)
	END BegBlk;

	PROCEDURE EndBlk*;
	BEGIN DEC(indentLevel); BegStat; OPM.Write(CloseBrace); OPM.WriteLn
	END EndBlk;

	PROCEDURE EndBlk0*;
	BEGIN DEC(indentLevel); BegStat; OPM.Write(CloseBrace)
	END EndBlk0;

	PROCEDURE Str1(IN s: ARRAY OF SHORTCHAR; x: INTEGER);
		VAR ch: SHORTCHAR; i: SHORTINT;
	BEGIN ch := s[0]; i := 0;
		WHILE ch # 0X DO
			IF ch = "#" THEN OPM.WriteInt(x)
			ELSE OPM.Write(ch);
			END;
			INC(i); ch := s[i]
		END
	END Str1;

	PROCEDURE Length(IN s: ARRAY OF SHORTCHAR): SHORTINT;
		VAR i: SHORTINT;
	BEGIN i := 0;
		WHILE s[i] # 0X DO INC(i) END;
		RETURN i
	END Length;

	PROCEDURE PerfectHash (IN s: ARRAY OF SHORTCHAR): SHORTINT;
		VAR i, h: SHORTINT;
	BEGIN i := 0; h := 0;
		WHILE (s[i] # 0X) & (i < 5) DO h := SHORT(3*h + ORD(s[i])); INC(i) END;
		RETURN SHORT(h MOD 105)
	END PerfectHash;

	PROCEDURE WriteName (IN s: ARRAY OF SHORTCHAR);
		VAR i: INTEGER; ch: SHORTCHAR;
	BEGIN
		IF s = "Windows" THEN OPM.WriteString("Windows_")
		ELSE
			i := 0; ch := s[0];
			WHILE ch # 0X DO
				IF ch = "^" THEN OPM.WriteString(RecExt) ELSE OPM.Write(ch) END;
				INC(i); ch := s[i]
			END
		END
	END WriteName;

	PROCEDURE Ident* (obj: OPT.Object);
		VAR mode, level, h: SHORTINT;
	BEGIN
		mode := obj^.mode; level := obj^.mnolev;
		IF (obj^.entry # NIL) & (level <= 0) THEN
			OPM.WriteString(obj^.entry^);
			IF (OPM.currFile = OPM.HeaderFile) & (level < 0) THEN OPT.GlbMod[-level].vis := 1 END
		ELSIF (mode IN {Var, Typ, LProc}) & (level > 0)
			OR (mode IN {Fld, VarPar}) & ~((obj^.conval # NIL) & (obj^.conval^.arr # NIL))	(* not a constant array *)
		THEN
			IF ~oldc THEN OPM.WriteString(obj^.name^) ELSE OPM.Write(obj^.name^[0]) END;
			h := PerfectHash(obj^.name^);
			IF hashtab[h] >= 0 THEN
				IF keytab[hashtab[h]] = obj^.name^ THEN OPM.Write(Underscore) END
			END
		ELSIF (mode = Typ) & (obj^.typ^.form IN {Byte..Set, UByte, Char16}) THEN
			CASE obj^.typ^.form OF
				| Byte: OPM.WriteString("BYTE")
				| Bool: OPM.WriteString("BOOLEAN")
				| Char8, UByte: OPM.WriteString("CHAR")
				| Char16: OPM.WriteString("LONGCHAR")
				| SInt: OPM.WriteString("SHORTINT")
				| Int: OPM.WriteString("INTEGER")
				| LInt: OPM.WriteString("LONGINT")
				| Real: OPM.WriteString("SHORTREAL")
				| LReal: OPM.WriteString("REAL")
				| Set: OPM.WriteString("SET")
			END
		ELSE
			IF (mode # Typ) OR (obj^.linkadr # PredefinedType) THEN
				IF mode = TProc THEN Ident(obj^.link^.typ^.strobj)
				ELSIF level < 0 THEN (* use unaliased module name *)
					IF ~oldc THEN OPM.WriteString(OPT.GlbMod[-level].name^)
					ELSE OPM.Write(OPT.GlbMod[-level].name^[0])
					END;
					IF OPM.currFile = OPM.HeaderFile THEN OPT.GlbMod[-level].vis := 1 (*include in header file*) END
				ELSIF ~oldc THEN OPM.WriteString(OPM.modName)
				END;
				OPM.Write(Underscore)
			ELSIF obj = OPT.sysptrtyp^.strobj THEN
				OPM.WriteString("SYSTEM_")
			END;
			WriteName(obj^.name^)
		END
	END Ident;

	PROCEDURE Stars (typ: OPT.Struct; forProc: BOOLEAN; VAR openClause: BOOLEAN);
		VAR pointers: SHORTINT; t0: OPT.Struct;
	BEGIN
		openClause := FALSE;
		IF (typ^.comp # Record) & ((typ^.strobj = NIL) OR (typ^.strobj^.name^ = "")) THEN
			IF typ^.comp IN {Array, DynArr} THEN
				Stars(typ^.BaseTyp, forProc, openClause);
				openClause := (typ^.comp = Array)
			ELSIF typ^.form = ProcTyp THEN
				Stars(typ^.BaseTyp, forProc, openClause);
				OPM.Write(OpenParen);
				IF typ^.sysflag = 1 THEN OPM.WriteString(STDCALL)
				ELSIF typ^.sysflag = 2 THEN OPM.WriteString(FASTCALL)
				END;
				OPM.Write(Star)
			ELSIF typ^.form = Pointer THEN
				pointers := 0; t0 := typ;
				WHILE (typ^.form = Pointer) & ((typ^.strobj = NIL) OR (typ^.strobj^.name^ = "")) DO
					INC(pointers); typ := typ^.BaseTyp
				END;
				IF typ^.comp # DynArr THEN ASSERT(t0 # typ); Stars(typ, forProc, openClause) END;
				IF pointers > 0 THEN
					IF openClause & ~forProc THEN OPM.Write(OpenParen) END;
					openClause := FALSE;
					WHILE pointers > 0 DO OPM.Write(Star); DEC(pointers) END
				END
			END
		END
	END Stars;

	PROCEDURE^ AnsiParamList (obj: OPT.Object; showParamNames, showOberonParams: BOOLEAN);
	PROCEDURE^ DeclareReturnType (retTyp: OPT.Struct);

	PROCEDURE DeclareObj(dcl: OPT.Object; vis: INTEGER);
		VAR
			typ: OPT.Struct;
			varPar, openClause: BOOLEAN; form, comp: SHORTINT;
	BEGIN
		typ := dcl^.typ;
		varPar := ((dcl^.mode = VarPar) & (typ^.comp # Array)) OR (typ^.comp = DynArr) OR (vis = 3);
		Stars(typ, FALSE, openClause);
		IF varPar THEN
			IF openClause THEN OPM.Write(OpenParen) END;
			OPM.Write(Star)
		END;
		IF dcl.name # OPT.null THEN
			Ident(dcl);
			IF vis = 4 THEN OPM.WriteString("__copy") END
		END;
		IF varPar & openClause THEN OPM.Write(CloseParen) END;
		openClause := FALSE;
		LOOP
			form := typ^.form;
			comp := typ^.comp;
			IF (typ^.strobj # NIL) & (typ^.strobj^.name # OPT.null) & (comp # DynArr)
				OR (form = NoTyp) OR (comp = Record)
			THEN
				EXIT
			ELSIF (form = Pointer) & (typ^.BaseTyp^.comp # DynArr) THEN
				openClause := TRUE
			ELSIF (form = ProcTyp) OR (comp IN {Array, DynArr}) THEN
				IF openClause THEN OPM.Write(CloseParen); openClause := FALSE END;
				IF form = ProcTyp THEN
					IF ansi THEN OPM.Write(")"); AnsiParamList(typ^.link, FALSE, TRUE)
					ELSE OPM.WriteString(")()")
					END;
					DeclareReturnType(typ^.BaseTyp);
					EXIT
				ELSIF comp = Array THEN
					OPM.Write(OpenBracket); OPM.WriteInt(typ^.n); OPM.Write(CloseBracket)
				END
			ELSE
				EXIT
			END;
			typ := typ^.BaseTyp
		END
	END DeclareObj;

	PROCEDURE Andent*(typ: OPT.Struct);	(* ident of possibly anonymous record type *)
	BEGIN
		IF (typ^.strobj = NIL) OR (typ^.align >= 10000H) THEN
			OPM.WriteString(OPM.modName); Str1("__#", typ^.align DIV 10000H)
		ELSE Ident(typ^.strobj)
		END
	END Andent;

	PROCEDURE Undefined(obj: OPT.Object): BOOLEAN;
	BEGIN
		(* imported anonymous types have obj^.name = OPT.null; used e.g. for repeating inherited fields *)
		RETURN (obj^.mnolev >= 0) & (obj^.linkadr # MaxType+OPM.currFile )
			& (obj^.linkadr # PredefinedType)
			& (obj^.linkadr # RecursiveType)
			OR (obj^.name = OPT.null)
	END Undefined;

	PROCEDURE^ FieldList (typ: OPT.Struct; last: BOOLEAN; VAR off, n, curAlign: INTEGER);
	PROCEDURE^ Universal (typ: OPT.Struct): BOOLEAN;
	PROCEDURE^ UniversalArrayName (typ: OPT.Struct);

	PROCEDURE DeclareBase(dcl: OPT.Object); (* declare the specifier of object dcl*)
		VAR typ, prev: OPT.Struct; obj: OPT.Object; nofdims: SHORTINT; off, n, dummy: INTEGER;
	BEGIN
		typ := dcl^.typ; prev := typ;
		WHILE ((typ^.strobj = NIL) OR (typ^.comp = DynArr) OR Undefined(typ^.strobj)) & (typ^.comp # Record) & (typ^.form # NoTyp)
			& ~((typ^.form = Pointer) & (typ^.BaseTyp^.comp = DynArr) & ~ODD(typ^.BaseTyp^.sysflag)) DO
			prev := typ; typ := typ^.BaseTyp
		END ;
		obj := typ^.strobj;
		IF typ^.form = NoTyp THEN	(* proper procedure *)
			OPM.WriteString(VoidType)
		ELSIF (obj # NIL) & ~Undefined(obj) THEN	(* named type, already declared *)
			Ident(obj)
		ELSIF typ^.comp = Record THEN
			IF typ^.sysflag MOD 100H = union THEN OPM.WriteString(Union)
			ELSE OPM.WriteString(Struct)
			END;
			IF (obj = NIL) OR (obj.name # OPT.null) THEN Andent(typ)
			ELSE OPM.WriteString("/* "); Andent(typ); OPM.WriteString(" */")
			END;
			IF (prev.form # Pointer) & ((obj # NIL) OR (dcl.name = OPT.null)) THEN
				(* named record type not yet declared OR anonymous record with empty name *)
				IF (typ^.BaseTyp # NIL) & (typ^.BaseTyp^.strobj.vis # internal) THEN
					OPM.WriteString(" { /* "); Ident(typ^.BaseTyp^.strobj); OPM.WriteString(" */"); OPM.WriteLn; Indent(1)
				ELSE OPM.Write(Blank); BegBlk
				END ;
				FieldList(typ, TRUE, off, n, dummy);
				EndBlk0
			END
		ELSIF (typ^.form = Pointer) & (typ^.BaseTyp^.comp = DynArr) & (typ^.BaseTyp^.sysflag <= 0) THEN
			typ := typ^.BaseTyp;
			OPM.WriteString(Struct);
			IF Universal(typ) THEN
				UniversalArrayName(typ)
			ELSE
				Andent(typ)
			END
		END
	END DeclareBase;

	PROCEDURE NofPtrs* (typ: OPT.Struct): INTEGER;
		VAR fld: OPT.Object; btyp: OPT.Struct; n: INTEGER;
	BEGIN
		IF (typ^.form = Pointer) & (typ^.sysflag = 0) THEN RETURN 1
		ELSIF (typ^.comp = Record) & (typ^.sysflag MOD 100H = 0) THEN
			btyp := typ^.BaseTyp;
			IF btyp # NIL THEN n := NofPtrs(btyp) ELSE n := 0 END ;
			fld := typ^.link;
			WHILE (fld # NIL) & (fld^.mode = Fld) DO
				IF fld^.name^ # OPM.HdPtrName THEN n := n + NofPtrs(fld^.typ)
				ELSE INC(n)
				END ;
				fld := fld^.link
			END ;
			RETURN n
		ELSIF typ^.comp = Array THEN
			btyp := typ^.BaseTyp; n := typ^.n;
			WHILE btyp^.comp = Array DO n := btyp^.n * n; btyp := btyp^.BaseTyp END ;
			RETURN NofPtrs(btyp) * n
		ELSE RETURN 0
		END
	END NofPtrs;

	PROCEDURE PutPtrOffsets (typ: OPT.Struct; adr: INTEGER; VAR cnt: INTEGER);
		VAR fld: OPT.Object; btyp: OPT.Struct; n, i: INTEGER;
	BEGIN
		IF (typ^.form = Pointer) & (typ^.sysflag = 0) THEN
			OPM.WriteInt(adr); OPM.WriteString(", "); INC(cnt);
			IF cnt MOD 16 = 0 THEN OPM.WriteLn; OPM.WriteTab END
		ELSIF (typ^.comp = Record) & (typ^.sysflag MOD 100H = 0) THEN
			btyp := typ^.BaseTyp;
			IF btyp # NIL THEN PutPtrOffsets(btyp, adr, cnt) END ;
			fld := typ^.link;
			WHILE (fld # NIL) & (fld^.mode = Fld) DO
				IF fld^.name^ # OPM.HdPtrName THEN PutPtrOffsets(fld^.typ, adr + fld^.adr, cnt)
				ELSE
					OPM.WriteInt(adr + fld^.adr); OPM.WriteString(", "); INC(cnt);
					IF cnt MOD 16 = 0 THEN OPM.WriteLn; OPM.WriteTab END
				END ;
				fld := fld^.link
			END
		ELSIF typ^.comp = Array THEN
			btyp := typ^.BaseTyp; n := typ^.n;
			WHILE btyp^.comp = Array DO n := btyp^.n * n; btyp := btyp^.BaseTyp END ;
			IF NofPtrs(btyp) > 0 THEN i := 0;
				WHILE i < n DO PutPtrOffsets(btyp, adr + i * btyp^.size, cnt); INC(i) END
			END
		END
	END PutPtrOffsets;

	PROCEDURE InitTProcs(typ, obj: OPT.Object);
	BEGIN
		IF obj # NIL THEN
			InitTProcs(typ, obj^.left);
			IF obj^.mode = TProc THEN
				BegStat;
				OPM.WriteString("__INITBP(");
				Ident(typ); OPM.WriteString(Comma); Ident(obj);
				Str1(", #)", obj^.adr DIV 10000H);
				EndStat
			END ;
			InitTProcs(typ, obj^.right)
		END
	END InitTProcs;

	PROCEDURE PutBase(typ: OPT.Struct);
	BEGIN
		IF typ # NIL THEN
			PutBase(typ^.BaseTyp);
			Ident(typ^.strobj); OPM.WriteString(DynTypExt); OPM.WriteString(", ")
		END
	END PutBase;

	PROCEDURE LenList(par: OPT.Object; ansiDefine, showParamName: BOOLEAN);
		VAR typ: OPT.Struct; dim: SHORTINT;
	BEGIN
		IF showParamName THEN Ident(par); OPM.WriteString(LenExt) END;
		dim := 1; typ := par^.typ^.BaseTyp;
		WHILE (typ^.comp = DynArr) & ~ODD(typ^.sysflag) DO
			IF ansiDefine THEN
				CASE OPM.IndexSize OF
				| 2: OPM.WriteString(", SHORTINT ")
				| 4: OPM.WriteString(", INTEGER ")
				ELSE OPM.WriteString(", LONGINT ")
				END
			ELSE OPM.WriteString(Comma)
			END;
			IF showParamName THEN Ident(par); OPM.WriteString(LenExt); OPM.WriteInt(dim) END;
			typ := typ^.BaseTyp; INC(dim)
		END
	END LenList;

	PROCEDURE DeclareParams(par: OPT.Object; macro, showOberonParams: BOOLEAN);
	BEGIN
		OPM.Write(OpenParen);
		WHILE par # NIL DO
			IF macro THEN OPM.WriteString(par.name^)
			ELSE
				IF (par^.mode = Var) & (par^.typ^.form = Real) THEN OPM.Write("_") END ;
				Ident(par)
			END ;
			IF showOberonParams THEN
				IF (par^.typ^.comp = DynArr) & ~ODD(par^.typ^.sysflag) THEN
					OPM.WriteString(Comma); LenList(par, FALSE, TRUE);
				ELSIF (par^.mode = VarPar) & (par^.typ^.comp = Record) & ~ODD(par^.typ^.sysflag) THEN
					OPM.WriteString(Comma); OPM.WriteString(par.name^); OPM.WriteString(TagExt)
				END
			END;
			par := par^.link;
			IF par # NIL THEN OPM.WriteString(Comma) END
		END ;
		OPM.Write(CloseParen)
	END DeclareParams;

	PROCEDURE ^DefineType(str: OPT.Struct);
	PROCEDURE ^ProcHeader(proc: OPT.Object; define: BOOLEAN);

	PROCEDURE DefineTProcTypes(obj: OPT.Object);	(* define all types that are used in a TProc definition *)
		VAR par: OPT.Object;
	BEGIN
		IF obj^.typ # OPT.notyp THEN DefineType(obj^.typ) END ;
		IF ansi THEN par := obj^.link;
			WHILE par # NIL DO DefineType(par^.typ); par := par^.link END
		END
	END DefineTProcTypes;

	PROCEDURE DeclareTProcs(obj: OPT.Object; VAR empty: BOOLEAN);
	BEGIN
		IF obj # NIL THEN
			DeclareTProcs(obj^.left, empty);
			IF obj^.mode = TProc THEN
				IF obj^.typ # OPT.notyp THEN DefineType(obj^.typ) END ;
				IF OPM.currFile = OPM.HeaderFile THEN
					IF obj^.vis = external THEN
						DefineTProcTypes(obj);
						OPM.WriteString(Extern); empty := FALSE;
						ProcHeader(obj, FALSE)
					END
				ELSE empty := FALSE;
					DefineTProcTypes(obj);
					IF obj^.vis = internal THEN OPM.WriteString(Static)
					ELSIF dynlib THEN OPM.WriteString(EXPORT)
					ELSE OPM.WriteString(Export)
					END;
					ProcHeader(obj, FALSE)
				END
			END ;
			DeclareTProcs(obj^.right, empty)
		END
	END DeclareTProcs;

	PROCEDURE BaseTProc*(obj: OPT.Object): OPT.Object;
		VAR typ, base: OPT.Struct; mno: INTEGER;
	BEGIN typ := obj^.link^.typ;	(* receiver type *)
		IF typ^.form = Pointer THEN typ := typ^.BaseTyp END ;
		base := typ^.BaseTyp; mno := obj^.adr DIV 10000H;
		WHILE (base # NIL) & (mno < base^.n) DO typ := base; base := typ^.BaseTyp END ;
		OPT.FindField(obj^.name^, typ, obj);
		RETURN obj
	END BaseTProc;

	PROCEDURE DefineTProcMacros(obj: OPT.Object; VAR empty: BOOLEAN);
		VAR oc: BOOLEAN;
	BEGIN
		IF obj # NIL THEN
			DefineTProcMacros(obj^.left, empty);
			IF (obj^.mode = TProc) & (obj = BaseTProc(obj)) & ((OPM.currFile # OPM.HeaderFile) OR (obj^.vis = external)) THEN
				OPM.WriteString("#define __");
				Ident(obj);
				DeclareParams(obj^.link, TRUE, TRUE);
				OPM.WriteString(" __SEND(");
				IF obj^.link^.typ^.form = Pointer THEN
					OPM.WriteString("__TYPEOF("); Ident(obj^.link); OPM.Write(")")
				ELSE Ident(obj^.link); OPM.WriteString(TagExt)
				END;
				OPM.WriteString(", "); Ident(obj);
				Str1(", #, ", obj^.adr DIV 10000H);
				DeclareBase(obj);
				IF obj^.typ^.form # NoTyp THEN Stars(obj^.typ, TRUE, oc) END;
				OPM.WriteString("(*)");
				IF ansi THEN
					AnsiParamList(obj^.link, FALSE, TRUE)
				ELSE
					OPM.WriteString("()")
				END;
				DeclareReturnType(obj^.typ);
				OPM.WriteString(", ");
				DeclareParams(obj^.link, TRUE, TRUE);
				OPM.Write(")"); OPM.WriteLn
			END;
			DefineTProcMacros(obj^.right, empty)
		END
	END DefineTProcMacros;

	PROCEDURE DefineType(str: OPT.Struct); (* define a type object *)
		VAR obj, field, par: OPT.Object; empty: BOOLEAN;
	BEGIN
		IF (str^.comp = DynArr) OR (str^.form IN {Byte..Set, UByte, Char16}) THEN RETURN END;
		IF (OPM.currFile = OPM.BodyFile) OR (str^.ref < OPM.MaxStruct (*for hidden exports*) ) THEN
			obj := str^.strobj;
			IF (obj = NIL) OR Undefined(obj) THEN
				IF obj # NIL THEN (* check for cycles *)
					IF obj^.linkadr = ProcessingType THEN
						IF str^.form # Pointer THEN obj^.linkadr := RecursiveType END
					ELSE obj^.linkadr := ProcessingType
					END
				END;
				IF str^.comp = Record THEN
					(* the following exports the base type of an exported type even if the former is non-exported *)
					IF str^.BaseTyp # NIL THEN DefineType(str^.BaseTyp) END ;
					field := str^.link;
					WHILE (field # NIL) & (field^.mode = Fld) DO
						IF (field^.vis # internal) OR (OPM.currFile = OPM.BodyFile) THEN DefineType(field^.typ) END ;
						field := field^.link
					END
				ELSIF str^.form = Pointer THEN
					IF str^.BaseTyp^.comp # Record THEN DefineType(str^.BaseTyp) END
				ELSIF str^.comp IN {Array, DynArr} THEN
					IF (str^.BaseTyp^.strobj # NIL) & (str^.BaseTyp^.strobj^.linkadr = ProcessingType) THEN (*cyclic base type*)
						OPM.Mark(244, str^ .txtpos); str^.BaseTyp^.strobj^.linkadr := PredefinedType
					END;
					IF (obj # NIL) & (obj^.linkadr = RecursiveType) THEN
						obj^.linkadr := TemporaryType;
						DefineType(str);
						obj^.linkadr := RecursiveType
					ELSE
						DefineType(str^.BaseTyp)
					END
				ELSIF str^.form = ProcTyp THEN
					IF str^.BaseTyp # OPT.notyp THEN DefineType(str^.BaseTyp) END ;
					field := str^.link;
					WHILE field # NIL DO DefineType(field^.typ); field := field^.link END
				END
			END;
			IF (obj # NIL) & (Undefined(obj) OR (obj^.linkadr = TemporaryType)) THEN
				IF (obj^.linkadr # RecursiveType) OR (str.comp = Array) THEN
					OPM.WriteString("typedef"); OPM.WriteLn; OPM.WriteTab; Indent(1);
					obj^.linkadr := ProcessingType;
					DeclareBase(obj); OPM.Write(Blank);
					obj^.typ^.strobj := NIL; (* SG: trick to make DeclareObj declare the type *)
					DeclareObj(obj, 0);
					obj^.typ^.strobj := obj; (* SG: revert trick *)
					obj^.linkadr := MaxType+OPM.currFile;
					EndStat; Indent(-1); OPM.WriteLn;
					IF obj^.typ^.comp = Record THEN empty := TRUE;
						DeclareTProcs(str^.link, empty); DefineTProcMacros(str^.link, empty);
						IF ~empty THEN OPM.WriteLn END
					END;
					IF (str^.form = Pointer) & (str^.BaseTyp^.comp = Array) THEN
						obj := str.BaseTyp.strobj;
						IF (obj # NIL) & (obj^.linkadr = RecursiveType) THEN
							obj^.linkadr := TemporaryType;
							DefineType(str^.BaseTyp);
							obj^.linkadr := RecursiveType
						END
					END
				END
			END
		END
	END DefineType;

	PROCEDURE Prefixed(x: OPT.ConstExt; IN y: ARRAY OF SHORTCHAR): BOOLEAN;
		VAR i: INTEGER;
	BEGIN i := 0;
		WHILE (i < LEN(x)) & (x[i] = y[i]) DO INC(i) END;
		RETURN y[i] = 0X
	END Prefixed;

	PROCEDURE CProcDefs(obj: OPT.Object; vis: SHORTINT);
		VAR i: INTEGER; ext: OPT.ConstExt; oc: BOOLEAN;
	BEGIN
		IF obj # NIL THEN
			CProcDefs(obj^.left, vis);
			(* bug: obj.history cannot be used to cover unexported and deleted CProcs; use special flag obj.adr = 1 *)
			IF (obj^.mode = CProc) & (obj^.vis >= vis) & (obj^.adr = 1) THEN
				ext := obj.conval.ext;
				IF ext # NIL THEN
					IF (ext^[0] # "#") & ~(Prefixed(ext, "extern ") OR Prefixed(ext, Extern)) THEN
						OPM.WriteString("#define "); Ident(obj); DeclareParams(obj^.link, TRUE, TRUE);
						OPM.WriteTab
					END;
					FOR i := 0 TO LEN(ext^) - 1 DO OPM.Write(ext^[i]) END;
				ELSE
					OPM.WriteString(Extern); DeclareBase(obj); OPM.Write(Blank);
					IF obj^.typ # OPT.notyp THEN Stars(obj^.typ, TRUE, oc) END;
					IF obj^.sysflag = 1 THEN OPM.WriteString(STDCALL)
					ELSIF obj^.sysflag = 2 THEN OPM.WriteString(FASTCALL)
					END;
					Ident(obj); OPM.Write(Blank); AnsiParamList(obj^.link, TRUE, FALSE); OPM.Write(";")
				END;
				OPM.WriteLn
			END;
			CProcDefs(obj^.right, vis)
		END
	END CProcDefs;

	PROCEDURE TypeDefs* (obj: OPT.Object; vis(*replaced by test on currFile in DefineType*): SHORTINT);
	BEGIN
		IF obj # NIL THEN
			TypeDefs(obj^.left, vis);
			(* test typ.txtpos to skip types that have been unexported; obj.history # removed is not enough!*)
			IF (obj^.mode = Typ) & (obj^.typ^.txtpos > 0) THEN DefineType(obj^.typ) END ;
			TypeDefs(obj^.right, vis)
		END
	END TypeDefs;

	PROCEDURE DefAnonRecs(n: OPT.Node);
		VAR o: OPT.Object; typ: OPT.Struct;
	BEGIN
		WHILE (n # NIL) & (n^.class = Ninittd) DO
			typ := n^.typ;
			IF (typ^.strobj = NIL) & ((OPM.currFile = OPM.BodyFile) OR (typ.ref < OPM.MaxStruct)) THEN
				DefineType(typ);	(* declare base and field types, if any *)
				NEW(o); o.typ := typ; o.name := OPT.null; DeclareBase(o); EndStat; OPM.WriteLn
				(* simply defines a named struct, but not a type;
					o.name = OPT.null signals field list expansion for DeclareBase in this very special case *)
			END ;
			n := n^.link
		END
	END DefAnonRecs;

	PROCEDURE UniversalEx (typ, initial: OPT.Struct; check: BOOLEAN): BOOLEAN;
		VAR p: OPT.Object;
	BEGIN
		IF check & (typ = initial) THEN
			RETURN TRUE
		ELSIF typ.comp IN {Array, Record} THEN
			RETURN (typ.strobj # NIL) & (typ.strobj.name^ # "")
		ELSIF typ.form IN {Comp, Pointer} THEN
			RETURN UniversalEx(typ.BaseTyp, initial, TRUE)
		ELSIF typ.form = ProcTyp THEN
			p := typ.link;
			WHILE p # NIL DO
				IF ~UniversalEx(p.typ, initial, TRUE) THEN RETURN FALSE END;
				p := p.link
			END;
			RETURN UniversalEx(typ.BaseTyp, initial, TRUE)
		END;
		RETURN TRUE
	END UniversalEx;

	PROCEDURE Universal (typ: OPT.Struct): BOOLEAN;
	BEGIN
		RETURN UniversalEx(typ, typ, FALSE)
	END Universal;

	PROCEDURE UniversalArrayNameEx (typ, initial: OPT.Struct; check: BOOLEAN);
		VAR p: OPT.Object;
	BEGIN
		IF check & (typ = initial) THEN
		ELSIF typ.form IN {Bool, Char8, Char16, Byte, SInt, Int, LInt, UByte, Set, Real, LReal} THEN
			IF typ.BaseTyp # OPT.undftyp THEN typ := typ.BaseTyp END;	(* basic type alias *)
			OPM.WriteString(typ.strobj.name^)
		ELSE
			IF typ.comp = Record THEN
				ASSERT((typ.strobj # NIL) & (typ.strobj.name^ # ""), 100);
				Ident(typ.strobj)
			ELSIF typ.comp = Array THEN
				ASSERT((typ.strobj # NIL) & (typ.strobj.name^ # ""), 100);
				Ident(typ.strobj); OPM.WriteString("_OBJ")
			ELSIF typ.comp = DynArr THEN
				UniversalArrayNameEx(typ.BaseTyp, initial, TRUE); OPM.WriteString("_ARRAY")
			ELSIF typ.form = Pointer THEN
				UniversalArrayNameEx(typ.BaseTyp, initial, TRUE); OPM.WriteString("_PTR")
			ELSIF typ.form = ProcTyp THEN
				OPM.WriteString("X__"); p := typ.link;
				WHILE p # NIL DO
					UniversalArrayNameEx(p.typ, initial, TRUE); OPM.WriteString("__");
					p := p.link
				END;
				OPM.Write("X");
				IF typ.BaseTyp # OPT.notyp THEN
					OPM.Write("R");
					OPM.WriteString("__"); UniversalArrayNameEx(typ.BaseTyp, initial, TRUE)
				END
			ELSE
				HALT(100)
			END
		END
	END UniversalArrayNameEx;

	PROCEDURE UniversalArrayName (typ: OPT.Struct);
	BEGIN
		UniversalArrayNameEx(typ, typ, FALSE)
	END UniversalArrayName;

	PROCEDURE DefAnonArrays;
		VAR i: INTEGER; atyp, typ, t: OPT.Struct; obj: OPT.Object;
	BEGIN
		i := 0;
		WHILE i < arrayIndex DO
			atyp := arrayTypes[i]; typ := atyp;
			IF Universal(atyp) THEN
				OPM.WriteString("#ifndef "); UniversalArrayName(atyp); OPM.WriteString("_DEF"); OPM.WriteLn;
				OPM.WriteString("#define "); UniversalArrayName(atyp); OPM.WriteString("_DEF"); OPM.WriteLn
			END;
			OPM.WriteString("typedef struct ");
			IF Universal(atyp) THEN UniversalArrayName(atyp) ELSE Andent(atyp) END;
			OPM.Write(" "); BegBlk;
			IF atyp.comp = DynArr THEN
				BegStat;
				CASE OPM.IndexSize OF
				| 2: Str1("SHORTINT len[#]", atyp.n + 1)
				| 4: Str1("INTEGER len[#]", atyp.n + 1)
				ELSE Str1("LONGINT len[#]", atyp.n + 1)
				END;
				WHILE typ^.comp = DynArr DO typ := typ^.BaseTyp END;	(* remove open levels *)
				IF typ.form = Pointer THEN	(* replace by anonymous pointer *)
					NEW(t); t.BaseTyp := typ.BaseTyp;
					t.sysflag := typ.sysflag; typ := t;
					typ.form := Pointer; typ.comp := Basic
				END;
				NEW(t); t.BaseTyp := typ; typ := t;	(* replace by one level with one element *)
				typ.form := Comp; typ.comp := Array; typ.n := 1;
				EndStat
			END;
			BegStat; NEW(obj);	(* aux. object for easy declaration *)
			obj.typ := typ; obj.mode := Fld;
			obj.name := OPT.NewName("data");
			obj.linkadr := UndefinedType; DeclareBase(obj); OPM.Write(Blank); DeclareObj(obj, 0);
			EndStat; EndBlk0; OPM.Write(" ");
			IF Universal(atyp) THEN
				UniversalArrayName(atyp); OPM.Write(";"); OPM.WriteLn;
				OPM.WriteString("#endif "); OPM.WriteLn
			ELSE
				Andent(atyp); OPM.Write(";"); OPM.WriteLn
			END;
			OPM.WriteLn;
			INC(i)
		END
	END DefAnonArrays;

	PROCEDURE InsertArrayType* (typ: OPT.Struct);
		VAR i: INTEGER;
	BEGIN
		ASSERT(typ.comp IN {Array, DynArr});
		i := 0;
		WHILE i < arrayIndex DO
			IF OPT.EqualType(typ, arrayTypes[i]) THEN RETURN END;
			INC(i)
		END;
		arrayTypes[arrayIndex] := typ;
		INC(arrayIndex)
	END InsertArrayType;

	PROCEDURE CleanupArrays*;
		VAR i: INTEGER;
	BEGIN
		i := 0;
		WHILE i < arrayIndex DO arrayTypes[i] := NIL; INC(i) END
	END CleanupArrays;

	PROCEDURE TDescDecl* (typ: OPT.Struct);
		VAR nofptrs: INTEGER;
			o: OPT.Object;
	BEGIN
		IF ~ODD(typ^.sysflag) THEN
			BegStat; OPM.WriteString("__TDESC(");
			Andent(typ); OPM.WriteString("__desc");
			Str1(", #", typ^.n + 1); Str1(", #) = {__TDFLDS(", NofPtrs(typ));
			OPM.Write('"');
			IF typ^.strobj # NIL THEN WriteName(typ^.strobj^.name^) END;
			Str1('", #), {', typ^.size);
			nofptrs := 0; PutPtrOffsets(typ, 0, nofptrs); Str1("#}}", -(nofptrs + 1) * OPM.AdrSize);
			EndStat
		END
	END TDescDecl;

	PROCEDURE InitTDesc*(typ: OPT.Struct);
	BEGIN
		IF ~ODD(typ^.sysflag) THEN
			BegStat; OPM.WriteString("__INITYP(");
			Andent(typ); OPM.WriteString(", ");
			IF typ^.BaseTyp # NIL THEN Andent(typ^.BaseTyp) ELSE Andent(typ) END ;
			Str1(", #)", typ^.extlev);
			EndStat;
			IF typ^.strobj # NIL THEN InitTProcs(typ^.strobj, typ^.link) END
		END
	END InitTDesc;

	PROCEDURE Align*(VAR adr: INTEGER; base: INTEGER);
	BEGIN
		CASE base OF
		|  2: INC(adr, adr MOD 2)
		|  4: INC(adr, (-adr) MOD 4)
		|  8: INC(adr, (-adr) MOD 8)
		|16: INC(adr, (-adr) MOD 16)
		ELSE (*1*)
		END
	END Align;

	PROCEDURE SizeAlignment(size: INTEGER): INTEGER;
		VAR alignment: INTEGER;
	BEGIN
		IF size < OPM.Alignment THEN
			(* Round up to next power of 2 *)
			alignment := 1; WHILE alignment < size DO alignment := alignment * 2 END
		ELSE
			alignment := OPM.Alignment
		END;
		RETURN alignment
	END SizeAlignment;

	PROCEDURE BaseAlignment*(typ: OPT.Struct): INTEGER;
	BEGIN
		IF typ^.form = Comp THEN
			IF typ^.comp = Record THEN RETURN typ^.align MOD 10000H
			ELSE RETURN BaseAlignment(typ^.BaseTyp)
			END
		END;
		RETURN SizeAlignment(typ^.size)
	END BaseAlignment;

	PROCEDURE FillGap(gap, off, align: INTEGER; VAR n, curAlign: INTEGER);
		VAR adr: INTEGER;
	BEGIN
		adr := off; Align(adr, align);
		IF (curAlign < align) & (gap - (adr - off) >= align) THEN (* preserve alignment of the enclosing struct! *)
			DEC(gap, (adr - off) + align);
			BegStat;
			CASE align OF
			| 2: OPM.WriteString("SHORTINT")
			| 4: OPM.WriteString("INTEGER")
			ELSE OPM.WriteString("LONGINT")
			END;
			Str1(" _prvt#", n); INC(n); EndStat;
			curAlign := align
		END;
		IF gap > 0 THEN BegStat; Str1("char _prvt#", n); INC(n); Str1("[#]", gap); EndStat END
	END FillGap;

	PROCEDURE FieldList (typ: OPT.Struct; last: BOOLEAN; VAR off, n, curAlign: INTEGER);
		VAR fld: OPT.Object; base: OPT.Struct; gap, adr, align, fldAlign: INTEGER;
	BEGIN
		fld := typ.link; align := typ^.align MOD 10000H;
		IF typ.BaseTyp # NIL THEN FieldList(typ.BaseTyp, FALSE, off, n, curAlign)
		ELSE off := 0; n := 0; curAlign := 1
		END ;
		WHILE (fld # NIL) & (fld.mode = Fld) DO
			IF (OPM.currFile = OPM.HeaderFile) & (fld.vis = internal) OR
				(OPM.currFile = OPM.BodyFile) & (fld.vis = internal) & (typ^.mno # 0) THEN
				fld := fld.link;
				WHILE (fld # NIL) & (fld.mode = Fld) & (fld.vis = internal) DO fld := fld.link END ;
			ELSE
				(* mimic OPV.TypSize to detect gaps caused by private fields *)
				adr := off; fldAlign := BaseAlignment(fld^.typ); Align(adr, fldAlign);
				gap := fld.adr - adr;
				IF fldAlign > curAlign THEN curAlign := fldAlign END ;
				IF gap > 0 THEN FillGap(gap, off, align, n, curAlign) END ;
				BegStat; DeclareBase(fld); OPM.Write(Blank); DeclareObj(fld, 0);
				off := fld.adr + fld.typ.size; base := fld.typ; fld := fld.link;
				IF (fld # NIL) & (fld.typ.form = base.form) & (base.form IN {Byte..Set, UByte, Char16}) THEN
					base := fld.typ
				END;
				WHILE (fld # NIL) & (fld.mode = Fld) & (fld.typ = base) & (fld.adr = off)
(* ?? *)		& ((OPM.currFile = OPM.BodyFile) OR (fld.vis # internal) OR (fld.typ.strobj = NIL)) DO
					OPM.WriteString(", "); DeclareObj(fld, 0); off := fld.adr + fld.typ.size; fld := fld.link;
					IF (fld # NIL) & (fld.typ.form = base.form) & (base.form IN {Byte..Set, UByte, Char16}) THEN
						base := fld.typ
					END
				END;
				EndStat
			END
		END;
		IF last THEN
			adr := typ.size - typ^.sysflag DIV 100H;
			IF adr = 0 THEN gap := 1 (* avoid empty struct *) ELSE gap := adr - off END ;
			IF gap > 0 THEN FillGap(gap, off, align, n, curAlign) END
		END
	END FieldList;

	PROCEDURE HasPtrs (typ: OPT.Struct): BOOLEAN;
		VAR fld: OPT.Object;
	BEGIN
		IF typ.form IN {Pointer, ProcTyp} THEN
			RETURN TRUE
		ELSIF typ.comp = Record THEN
			fld := typ.link;
			WHILE (fld # NIL) & (fld.mode = Fld) DO
				IF (fld.name^ = OPM.HdPtrName) OR (fld.name^ = OPM.HdProcName) OR HasPtrs(fld.typ) THEN RETURN TRUE END;
				fld := fld.link
			END;
			IF typ.BaseTyp # NIL THEN RETURN HasPtrs(typ.BaseTyp) END
		ELSIF typ.comp = Array THEN
			RETURN HasPtrs(typ.BaseTyp)
		END;
		RETURN FALSE
	END HasPtrs;

	PROCEDURE WriteConstArr (VAR obj: OPT.Object; typ: OPT.Struct);
	(* генерация конструкция "константный массив". typ - текущий уровень массива *)
	VAR apar: OPT.Node; i: INTEGER;

	PROCEDURE WriteElem (arr: OPT.ConstArr; i: INTEGER; bt: OPT.Struct);
		(* выводит в листинг i-й элемент константного массива arr *)
		BEGIN
			CASE bt^.size OF
				| 1: IF bt^.form IN {Char8, UByte} THEN OPM.WriteInt(arr.val1[i] MOD 100H) ELSE OPM.WriteInt(arr.val1[i]) END
				| 2: IF bt^.form = Char16 THEN OPM.WriteInt(arr.val1[i] MOD 10000H) ELSE OPM.WriteInt(arr.val2[i]) END
				| 4: OPM.WriteInt(arr.val4[i])
				| 8: OPM.WriteInt(arr.val8[i])
			END
		END WriteElem;

	BEGIN
		OPM.WriteString("{"); (* скобка (  *)
		i := 0;
		IF typ^.BaseTyp^.form # Comp THEN (* массив из простых элементов *)
			FOR i := 0 TO typ^.n-2 DO
				WriteElem(obj^.conval^.arr , i+SHORT(obj^.conval^.intval), typ^.BaseTyp); OPM.WriteString(",");
				IF (i+1) MOD 10 = 0 THEN OPM.WriteLn; BegStat END
			END;
			WriteElem(obj^.conval^.arr , typ^.n-1+SHORT(obj^.conval^.intval), typ^.BaseTyp); (* последний элемент *)
			INC(obj^.conval^.intval, typ^.n); (* учли выведенные элементы *)
		ELSE (* массив из массивов *)
			FOR i := 0 TO typ^.n-2 DO
				WriteConstArr (obj, typ^.BaseTyp);  (* рекурсивная обработка подмассива *)
				OPM.WriteString(","); OPM.WriteLn;
				BegStat;
			END;
			WriteConstArr (obj, typ^.BaseTyp); (* последний элемент *)
		END;
		OPM.WriteString("}")
	END WriteConstArr;

	PROCEDURE IdentList (obj: OPT.Object; vis: SHORTINT);
	(* generate var and param lists; vis: 0 all global vars, local var, 1 exported(R) var, 2 par list, 3 scope var *)
		VAR base: OPT.Struct; first, constarr: BOOLEAN; lastvis: SHORTINT;
	BEGIN
		base := NIL; first := TRUE;
		WHILE (obj # NIL) & (obj^.mode # TProc) DO
			constarr := (obj^.conval # NIL) & (obj^.conval^.arr # NIL);
			IF constarr & (vis # 0) & (obj^.vis # externalR) THEN	(* not exported in header *)
			ELSIF (vis IN {0, 2}) OR ((vis = 1) & (obj^.vis # 0)) OR ((vis = 3) & ~obj^.leaf) THEN
				IF (base # NIL) & (obj^.typ^.form = base^.form) & (base^.form IN {Byte..Set, UByte, Char16}) THEN
					base := obj^.typ 
				END;
				IF (obj^.typ # base) OR (obj^.vis # lastvis) OR constarr THEN	(* each const. array separately *)
				(* new variable base type definition required *)
					IF ~first THEN EndStat END ;
					first := FALSE;
					base := obj^.typ; lastvis := obj^.vis;
					BegStat;
					IF (vis = 1) & (obj^.vis # internal) THEN OPM.WriteString(Extern)
					ELSIF (obj^.mnolev = 0) & (vis = 0) THEN
						IF (obj^.vis = internal) OR constarr & (obj^.vis # externalR) THEN OPM.WriteString(Static)
						ELSIF dynlib THEN
							IF mainprog & (obj^.entry = NIL) THEN obj^.entry := obj^.name END;
							OPM.WriteString(EXPORT)
						ELSE OPM.WriteString(Export)
						END
					END;
					IF constarr THEN OPM.WriteString("__CONSTARR ") END;
					IF (vis = 2) & (obj^.mode = Var) & (base^.form = Real) THEN OPM.WriteString("REAL")
					ELSE DeclareBase(obj)
					END
				ELSE OPM.Write(",")
				END;
				OPM.Write(Blank);
				IF (vis = 2) & (obj^.mode = Var) & (base^.form = Real) THEN OPM.Write("_") END ;
				DeclareObj(obj, vis);
				IF (obj^.typ^.comp = DynArr) & ~ODD(obj^.typ^.sysflag) THEN (* declare len parameter(s) *)
					EndStat; BegStat;
					base := OPT.linttyp;
					CASE OPM.IndexSize OF
					| 2: OPM.WriteString("SHORTINT ")
					| 4: OPM.WriteString("INTEGER ")
					ELSE OPM.WriteString("LONGINT ")
					END;
					LenList(obj, FALSE, TRUE)
				ELSIF (obj^.mode = VarPar) & (obj^.typ^.comp = Record) & ~ODD(obj^.typ^.sysflag) THEN
					EndStat; BegStat;
					OPM.WriteString("SYSTEM_ADRINT *"); Ident(obj); OPM.WriteString(TagExt);
					base := NIL
				ELSIF constarr & (vis = 0) THEN	(* элементы конст. массива *)
					OPM.WriteString(" ="); OPM.WriteLn; Indent(1);
					BegStat;
					obj^.conval^.intval := 0;
					WriteConstArr(obj, obj^.typ);
					Indent(-1)
				ELSIF ptrinit & (vis = 0) & (obj^.mnolev > 0) THEN
					IF obj^.typ^.form IN {Pointer, ProcTyp} THEN OPM.WriteString(" = NIL"); base := NIL
					ELSIF (obj^.typ^.form = Comp) & HasPtrs(obj^.typ) THEN OPM.WriteString(" = {0}")
					END
				END
			END;
			obj := obj^.link
		END;
		IF ~first THEN EndStat END
	END IdentList;

	PROCEDURE AnsiParamList (obj: OPT.Object; showParamNames, showOberonParams: BOOLEAN);
		VAR name: OPT.ConstExt(*OPS.String*);
	BEGIN
		OPM.Write("(");
		IF (obj = NIL) OR (obj^.mode = TProc) THEN OPM.WriteString("void")
		ELSE
			LOOP
				DeclareBase(obj);
				IF showParamNames THEN
					OPM.Write(Blank); DeclareObj(obj, 0)
				ELSE
					name := obj^.name; obj^.name := OPT.null; DeclareObj(obj, 0); obj^.name := name
				END;
				IF showOberonParams THEN
					IF (obj^.typ^.comp = DynArr) & ~ODD(obj^.typ^.sysflag) THEN
						CASE OPM.IndexSize OF
						| 2: OPM.WriteString(", SHORTINT ")
						| 4: OPM.WriteString(", INTEGER ")
						ELSE OPM.WriteString(", LONGINT ")
						END;
						LenList(obj, TRUE, showParamNames)
					ELSIF (obj^.mode = VarPar) & (obj^.typ^.comp = Record) & ~ODD(obj^.typ^.sysflag) THEN
						OPM.WriteString(", SYSTEM_ADRINT *");
						IF showParamNames THEN Ident(obj); OPM.WriteString(TagExt) END
					END
				END;
				IF (obj^.link = NIL) OR (obj^.link.mode = TProc) THEN EXIT END ;
				OPM.WriteString(", ");
				obj := obj^.link
			END
		END ;
		OPM.Write(")")
	END AnsiParamList;

	PROCEDURE DeclareReturnType (retTyp: OPT.Struct);
	BEGIN
		IF (retTyp^.form = ProcTyp) & ((retTyp^.strobj = NIL) OR (retTyp^.strobj^.name^ = "")) THEN
			IF ansi THEN OPM.Write(")"); AnsiParamList(retTyp^.link, FALSE, TRUE)
			ELSE OPM.WriteString(")()")
			END;
			DeclareReturnType(retTyp^.BaseTyp)
		END
	END DeclareReturnType;

	PROCEDURE ProcHeader(proc: OPT.Object; define: BOOLEAN);
		VAR oc: BOOLEAN;
	BEGIN
		DeclareBase(proc); OPM.Write(Blank);
		IF proc^.typ^.form # NoTyp THEN Stars(proc^.typ, TRUE, oc) END;
		IF proc^.sysflag = 1 THEN OPM.WriteString(STDCALL)
		ELSIF proc^.sysflag = 2 THEN OPM.WriteString(FASTCALL)
		END;
		Ident(proc); OPM.Write(Blank);
		IF ansi THEN
			AnsiParamList(proc^.link, TRUE, TRUE);
			DeclareReturnType(proc^.typ);
			IF ~define THEN OPM.Write(";") END;
			OPM.WriteLn
		ELSIF define THEN
			DeclareParams(proc^.link, FALSE, TRUE);
			DeclareReturnType(proc^.typ);
			OPM.WriteLn;
			Indent(1); IdentList(proc^.link, 2(* map REAL to double *)); Indent(-1)
		ELSE OPM.WriteString("();"); OPM.WriteLn
		END
	END ProcHeader;

	PROCEDURE ProcPredefs (obj: OPT.Object; vis: BYTE); (* forward declaration of procedures *)
	BEGIN
		IF obj # NIL THEN
			ProcPredefs(obj^.left, vis);
			IF (obj^.mode IN {LProc, XProc}) & (obj^.vis >= vis) & ((obj^.history # removed) OR (obj^.mode = LProc)) THEN
				(* previous XProc may be deleted or become LProc after interface change*)
				IF vis = external THEN
					IF (obj^.entry # NIL) OR dynlib THEN OPM.WriteString(EXTERN)
					ELSE OPM.WriteString(Extern)
					END
				ELSIF obj^.vis = internal THEN
					IF (obj^.mnolev = 0) & (obj^.entry # NIL) THEN OPM.WriteString(EXTERN)
					ELSE OPM.WriteString(Static)
					END
				ELSIF dynlib THEN
					IF mainprog & (obj^.entry = NIL) THEN obj^.entry := obj^.name END;
					OPM.WriteString(EXPORT)
				ELSE OPM.WriteString(Export)
				END;
				ProcHeader(obj, FALSE)
			END;
			ProcPredefs(obj^.right, vis)
		END
	END ProcPredefs;

	PROCEDURE Include(IN name: ARRAY OF SHORTCHAR);
	BEGIN
		OPM.WriteString("#include "); OPM.Write(Quotes); OPM.WriteString(name);
		OPM.WriteString(".oh"); OPM.Write(Quotes); OPM.WriteLn
	END Include;

	PROCEDURE IncludeImports(vis: SHORTINT);
		VAR mod: OPT.Object; i: INTEGER;
	BEGIN
		i := 1;
		WHILE i < OPT.nofGmod DO
			mod := OPT.GlbMod[i];
			IF mod^.vis >= vis THEN
				Include(mod^.name^);	(* use unaliased module name *)
				IF mod.name^ = "WinApi" THEN windows := TRUE END
			END;
			INC(i)
		END
	END IncludeImports;

	PROCEDURE GenDynTypes (n: OPT.Node; vis: SHORTINT);
		VAR typ: OPT.Struct;
	BEGIN
		WHILE (n # NIL) & (n^.class = Ninittd) DO
			typ := n^.typ;
			IF ~ODD(typ^.sysflag) & ((vis = internal) OR (typ^.ref < OPM.MaxStruct (*type needed in symbol file*))) THEN
				BegStat;
				IF vis = external THEN OPM.WriteString(Extern)
				ELSIF (typ^.strobj # NIL) & (typ^.strobj^.mnolev > 0) THEN OPM.WriteString(Static)
				ELSIF dynlib THEN OPM.WriteString(EXPORT)
				ELSE OPM.WriteString(Export)
				END ;
				OPM.WriteString("SYSTEM_ADRINT *"); Andent(typ); OPM.WriteString(DynTypExt);
				EndStat
			END ;
			n := n^.link
		END
	END GenDynTypes;

	PROCEDURE GenHdr*(n: OPT.Node);
	BEGIN
		(* includes are delayed until it is known which ones are needed in the header *)
		OPM.currFile := OPM.HeaderFile;
		DefAnonRecs(n);
		TypeDefs(OPT.topScope^.right, 1); OPM.WriteLn;
		DefAnonArrays;
		IdentList(OPT.topScope^.scope, 1); OPM.WriteLn;
		GenDynTypes(n, external); OPM.WriteLn;
		ProcPredefs(OPT.topScope^.right, 1);
		IF ~(OPM.noinit IN OPM.opt) THEN
			IF dynlib THEN OPM.WriteString(EXTERN) ELSE OPM.WriteString(Extern) END;
			OPM.WriteString("void *");
			OPM.WriteString(OPM.modName); OPM.WriteString(BodyNameExt);
			EndStat
		END;
		OPM.WriteLn;
		CProcDefs(OPT.topScope^.right, 1); OPM.WriteLn;
		OPM.WriteString("#endif"); OPM.WriteLn
	END GenHdr;

	PROCEDURE GenHeaderMsg;
	BEGIN
		OPM.WriteString("/* "); OPM.WriteString(HeaderMsg);
		IF mainprog THEN OPM.Write("m")
		ELSIF OPM.newsf IN OPM.opt THEN OPM.Write("s")
		ELSIF OPM.extsf IN OPM.opt THEN OPM.Write("e")
		END;
		IF ~(OPM.inxchk IN OPM.opt) THEN OPM.Write("x") END;
		IF OPM.vcpp IN OPM.opt THEN OPM.Write("v") END;
		IF OPM.ranchk IN OPM.opt THEN OPM.Write("r") END;
		IF ~(OPM.typchk IN OPM.opt) THEN OPM.Write("t") END;
		IF ~(OPM.assert IN OPM.opt) THEN OPM.Write("a") END;
		IF OPM.oldc IN OPM.opt THEN OPM.Write("o") END;
		IF ~ansi THEN OPM.Write("k") END;
		IF ~ptrinit THEN OPM.Write("p") END;
		IF OPM.include0 IN OPM.opt THEN OPM.Write("i") END;
		IF dynlib THEN OPM.Write("d") END;
		IF OPM.widechar IN OPM.opt THEN OPM.Write("w") END;
		OPM.Write(OPM.Lang); IF OPM.for IN OPM.opt THEN OPM.Write("f") END;
		OPM.WriteString(" -"); OPM.WriteInt(OPM.AdrSize);
		IF OPM.AdrSize # 2 THEN OPM.WriteInt(OPM.Alignment) ELSE OPM.WriteInt(OPM.SetSize) END;
		OPM.WriteString(" */"); OPM.WriteLn
	END GenHeaderMsg;

	PROCEDURE GenHdrIncludes*;
	BEGIN
		OPM.currFile := OPM.HeaderInclude;
		GenHeaderMsg;
		OPM.WriteLn;
		OPM.WriteString("#ifndef "); OPM.WriteString(OPM.modName); OPM.WriteString(FlagExt); OPM.WriteLn;
		OPM.WriteString("#define "); OPM.WriteString(OPM.modName); OPM.WriteString(FlagExt); OPM.WriteLn;
		OPM.WriteLn;
		Include(BasicIncludeFile);
		IncludeImports(1); OPM.WriteLn
	END GenHdrIncludes;

	PROCEDURE GenBdy*(n: OPT.Node);
	BEGIN
		OPM.currFile := OPM.BodyFile;
		GenHeaderMsg;
		Include(BasicIncludeFile);
		IncludeImports(0); OPM.WriteLn;
		DefAnonRecs(n);
		TypeDefs(OPT.topScope^.right, 0); OPM.WriteLn;
		DefAnonArrays;
		IdentList(OPT.topScope^.scope, 0); OPM.WriteLn;
		GenDynTypes(n, internal); OPM.WriteLn;
		ProcPredefs(OPT.topScope^.right, 0); OPM.WriteLn;
		CProcDefs(OPT.topScope^.right, 0); OPM.WriteLn;
		OPM.WriteString("/*============================================================================*/");
		OPM.WriteLn; OPM.WriteLn
	END GenBdy;

	PROCEDURE RegCmds(obj: OPT.Object);
	BEGIN
		IF obj # NIL THEN
			RegCmds(obj^.left);
			IF (obj^.mode = XProc) & (obj^.history # removed) THEN
				IF (obj^.vis # 0) & (obj^.link = NIL) & (obj^.typ = OPT.notyp) THEN (*command*)
					BegStat; OPM.WriteString('__REGCMD("');
					OPM.WriteString(obj.name^); OPM.WriteString('", '); Ident(obj); OPM.Write(")"); EndStat
				END
			END ;
			RegCmds(obj^.right)
		END
	END RegCmds;

	PROCEDURE InitImports(obj: OPT.Object);
	BEGIN
		IF obj # NIL THEN
			InitImports(obj^.left);
			IF (obj^.mode = Mod) & (obj^.mnolev # 0) & ~ODD(OPT.GlbMod[-obj^.mnolev].sysflag) THEN
				BegStat; OPM.WriteString("__IMPORT(");
				OPM.WriteString(OPT.GlbMod[-obj^.mnolev].name^); OPM.WriteString("__init");
				OPM.Write(CloseParen); EndStat
			END;
			InitImports(obj^.right)
		END
	END InitImports;

	PROCEDURE GenEnumPtrs* (var: OPT.Object);
		VAR typ: OPT.Struct; n: INTEGER;
	BEGIN GlbPtrs := FALSE;
		WHILE var # NIL DO
			typ := var^.typ;
			IF NofPtrs(typ) > 0 THEN
				IF ~GlbPtrs THEN GlbPtrs := TRUE;
					OPM.WriteString(Static);
					IF ansi THEN
						OPM.WriteString("void EnumPtrs(void (*P)(void*))")
					ELSE
						OPM.WriteString("void EnumPtrs(P)"); OPM.WriteLn;
						OPM.WriteTab; OPM.WriteString("void (*P)();")
					END;
					OPM.WriteLn;
					BegBlk
				END;
				BegStat;
				IF typ^.form = Pointer THEN
					OPM.WriteString("P("); Ident(var); OPM.Write(")");
				ELSIF typ^.comp = Record THEN
					OPM.WriteString("__ENUMR(&"); Ident(var); OPM.WriteString(", ");
					Andent(typ); OPM.WriteString(DynTypExt); Str1(", #", typ^.size); OPM.WriteString(", 1, P)")
				ELSIF typ^.comp = Array THEN
					n := typ^.n; typ := typ^.BaseTyp;
					WHILE typ^.comp = Array DO n := n * typ^.n; typ := typ^.BaseTyp END ;
					IF typ^.form = Pointer THEN
						OPM.WriteString("__ENUMP("); Ident(var); Str1(", #, P)", n)
					ELSIF typ^.comp = Record THEN
						OPM.WriteString("__ENUMR("); Ident(var); OPM.WriteString(", ");
						Andent(typ); OPM.WriteString(DynTypExt); Str1(", #", typ^.size); Str1(", #, P)", n)
					END
				END;
				EndStat
			END;
			var := var^.link
		END;
		IF GlbPtrs THEN
			EndBlk; OPM.WriteLn
		END
	END GenEnumPtrs;

	PROCEDURE EnterBody*;
	BEGIN
		OPM.WriteLn;
		IF mainprog & ~dynlib THEN
			IF ansi THEN
				OPM.WriteString("int main (int argc, char **argv)")
			ELSE
				OPM.WriteString("main (argc, argv)"); OPM.WriteLn;
				OPM.WriteTab; OPM.WriteString("int argc; char **argv;")
			END
		ELSE
			IF mainprog THEN OPM.WriteString("__BEGIN void ")
			ELSIF dynlib THEN OPM.WriteString(EXPORT + "void *")
			ELSE OPM.WriteString(Export + "void *")
			END;
			OPM.WriteString(OPM.modName); OPM.WriteString(BodyNameExt)
		END;
		OPM.WriteLn; BegBlk;
		IF mainprog & ~dynlib THEN BegStat; OPM.WriteString("__INIT(argc, argv)"); EndStat
		ELSIF ~mainprog THEN BegStat; OPM.WriteString("__DEFMOD"); EndStat
		END;
		InitImports(OPT.topScope^.right);
		BegStat;
		IF mainprog THEN OPM.WriteString('__REGMAIN("') ELSE OPM.WriteString('__REGMOD("') END;
		OPM.WriteString(OPM.modName);
		IF GlbPtrs THEN OPM.WriteString('", EnumPtrs)') ELSE OPM.WriteString('", 0)') END;
		EndStat;
		IF OPM.modName # "SYSTEM" THEN RegCmds(OPT.topScope) END
	END EnterBody;

	PROCEDURE ExitBody*;
	BEGIN
		IF ~(mainprog & dynlib) THEN
			BegStat;
			IF mainprog THEN OPM.WriteString("__FINI;") ELSE OPM.WriteString("__ENDMOD;") END;
			OPM.WriteLn
		END;
		EndBlk
	END ExitBody;

	PROCEDURE EnterClose*;
	BEGIN
		OPM.WriteLn;
		OPM.WriteString("__CLOSE void "); OPM.WriteString(OPM.modName);
		OPM.WriteString(CloseNameExt); OPM.WriteLn;
		BegBlk;
		OPM.WriteString("/* CLOSE */"); OPM.WriteLn
	END EnterClose;

	PROCEDURE ExitClose*;
	BEGIN
		EndBlk
	END ExitClose;

	PROCEDURE DllMainBody* (close: BOOLEAN);
	BEGIN
		IF mainprog & dynlib & windows THEN
			OPM.WriteLn;
			OPM.WriteString("int __STDCALL DllMainCRTStartup (void *hinst, int reason, void *reserved)");
			OPM.WriteLn; BegBlk; BegStat; OPM.WriteString("switch (reason) "); BegBlk;
			BegStat; OPM.WriteString("case 1: ");
				OPM.WriteString(OPM.modName); OPM.WriteString("__init(); break"); EndStat;
			IF close THEN BegStat; OPM.WriteString("case 0: ");
				OPM.WriteString(OPM.modName); OPM.WriteString("__close(); break"); EndStat
			END;
			EndBlk; BegStat; OPM.WriteString("return 1"); EndStat; EndBlk
		END
	END DllMainBody;

	PROCEDURE DefineInter* (proc: OPT.Object); (* define intermediate scope record and variable *)
		VAR scope: OPT.Object;
	BEGIN
		scope := proc^.scope;
		OPM.WriteString(Static); OPM.WriteString(Struct); OPM.WriteString(scope^.name^); OPM.Write(Blank);
		BegBlk;
		IdentList(proc^.link, 3); (* parameters *)
		IdentList(scope^.scope, 3); (* local variables *)
		BegStat; (* scope link field declaration *)
		OPM.WriteString(Struct); OPM.WriteString(scope^.name^);
		OPM.Write(Blank); OPM.Write(Star); OPM.WriteString(LinkName); EndStat;
		EndBlk0; OPM.Write(Blank);
		OPM.Write(Star); OPM.WriteString(scope^.name^); OPM.WriteString(GlobalScope); EndStat; OPM.WriteLn;
		ProcPredefs (scope^.right, 0);
		OPM.WriteLn;
	END DefineInter;

	PROCEDURE NeedsRetval*(proc: OPT.Object): BOOLEAN; (* aux. variable __retval needed for return *)
	BEGIN (* simple rule; ignores DUPlicated value arrays because not needed for Gcc *)
		RETURN (proc^.typ # OPT.notyp) & ~proc^.scope^.leaf
	END NeedsRetval;

	PROCEDURE EnterProc* (proc: OPT.Object);
		VAR var, scope: OPT.Object; typ: OPT.Struct; dim: SHORTINT;
	BEGIN
		IF proc^.vis # external THEN OPM.WriteString(Static) END ;
		ProcHeader(proc, TRUE);
		BegBlk;
		scope := proc^.scope;
		IdentList(scope^.scope, 0);
		IF ~scope^.leaf THEN (* declare intermediate procedure scope record variable*)
			BegStat; OPM.WriteString(Struct); OPM.WriteString(scope^.name^);
			OPM.Write(Blank); OPM.WriteString(LocalScope); EndStat
		END ;
		IF NeedsRetval(proc) THEN BegStat; Ident(proc^.typ^.strobj); OPM.WriteString(" __retval"); EndStat END ;
		var := proc^.link;
		WHILE var # NIL DO (* declare copy of fixed size value array parameters *)
			IF (var^.typ^.comp = Array) & (var^.mode = Var) THEN
				BegStat;
				IF var^.typ^.strobj = NIL THEN OPM.Mark(200, var^.typ^.txtpos) ELSE Ident(var^.typ^.strobj) END ;
				OPM.Write(Blank); Ident(var); OPM.WriteString("__copy");
				EndStat
			END ;
			var := var^.link
		END ;
		IF ~ansi THEN
			var := proc^.link;
			WHILE var # NIL DO (* "unpromote" value real parameters *)
				IF (var^.typ^.form = Real) & (var^.mode = Var) THEN
					BegStat;
					Ident(var^.typ^.strobj); OPM.Write(Blank); Ident(var); OPM.WriteString(" = _"); Ident(var);
					EndStat
				END ;
				var := var^.link
			END
		END ;
		var := proc^.link;
		WHILE var # NIL DO (* copy value array parameters *)
			IF (var^.typ^.comp IN {Array, DynArr}) & (var^.mode = Var) & (var^.typ^.sysflag = 0) THEN
				BegStat;
				IF var^.typ^.comp = Array THEN
					OPM.WriteString(DupArrFunc);
					Ident(var)
				ELSE
					OPM.WriteString(DupFunc);
					Ident(var); OPM.WriteString(Comma); Ident(var); OPM.WriteString(LenExt);
					typ := var^.typ^.BaseTyp; dim := 1;
					WHILE typ^.comp = DynArr DO
						OPM.WriteString(" * "); Ident(var); OPM.WriteString(LenExt); OPM.WriteInt(dim);
						typ := typ^.BaseTyp; INC(dim)
					END
				END;
				OPM.Write(CloseParen); EndStat
			ELSIF ptrinit & (var.mode = VarPar) & (var.vis = outPar) & (var.typ.form IN {Pointer, ProcTyp}) THEN
				BegStat;
				OPM.Write("*"); Ident(var);
				IF var.typ.form IN {Pointer, ProcTyp} THEN OPM.WriteString(" = NIL")
				ELSE OPM.WriteString(" = {0}")
				END;
				EndStat
			END;
			var := var^.link
		END;
		IF ~scope^.leaf THEN
			var := proc^.link; (* copy addresses of parameters into local scope record *)
			WHILE var # NIL DO
				IF ~var^.leaf THEN (* only if used by a nested procedure *)
					BegStat;
					OPM.WriteString(LocalScope); OPM.Write(Dot); Ident(var);
					OPM.WriteString(Becomes);
					IF var^.typ^.comp IN {Array, DynArr} THEN OPM.WriteString("(void*)")
						(* K&R and ANSI differ in the type: array or element type*)
					ELSIF var^.mode # VarPar THEN OPM.Write("&")
					END ;
					Ident(var);
					IF (var^.typ^.comp = DynArr) & ~ODD(var^.typ^.sysflag) THEN
						typ := var^.typ; dim := 0;
						REPEAT (* copy len(s) *)
							OPM.WriteString("; ");
							OPM.WriteString(LocalScope); OPM.Write(Dot); Ident(var); OPM.WriteString(LenExt);
							IF dim # 0 THEN OPM.WriteInt(dim) END ;
							OPM.WriteString(Becomes); Ident(var); OPM.WriteString(LenExt);
							IF dim # 0 THEN OPM.WriteInt(dim) END ;
							typ := typ^.BaseTyp
						UNTIL typ^.comp # DynArr;
					ELSIF (var^.mode = VarPar) & (var^.typ^.comp = Record) & ~ODD(var^.typ^.sysflag) THEN
						OPM.WriteString("; ");
						OPM.WriteString(LocalScope); OPM.Write(Dot); Ident(var); OPM.WriteString(TagExt);
						OPM.WriteString(Becomes); Ident(var); OPM.WriteString(TagExt)
					END ;
					EndStat
				END;
				var := var^.link;
			END;
			var := scope^.scope; (* copy addresses of local variables into scope record *)
			WHILE var # NIL DO
				IF ~var^.leaf THEN (* only if used by a nested procedure *)
					BegStat;
					OPM.WriteString(LocalScope); OPM.Write(Dot); Ident(var); OPM.WriteString(Becomes);
					IF var^.typ^.comp # Array THEN OPM.Write("&")
					ELSE OPM.WriteString("(void*)")	(* K&R and ANSI differ in the type: array or element type*)
					END ;
					Ident(var); EndStat
				END ;
				var := var^.link
			END;
			(* now link new scope *)
			BegStat; OPM.WriteString(LocalScope); OPM.Write(Dot); OPM.WriteString(LinkName);
			OPM.WriteString(Becomes); OPM.WriteString(scope^.name^); OPM.WriteString(GlobalScope); EndStat;
			BegStat; OPM.WriteString(scope^.name^); OPM.WriteString(GlobalScope); OPM.WriteString(Becomes);
			OPM.Write("&"); OPM.WriteString(LocalScope); EndStat
		END
	END EnterProc;

	PROCEDURE ExitProc*(proc: OPT.Object; eoBlock, implicitRet: BOOLEAN);
		VAR var: OPT.Object; indent: BOOLEAN;
	BEGIN
		indent := eoBlock;
		IF implicitRet & (proc^.typ # OPT.notyp) THEN
			OPM.WriteTab; OPM.WriteString("__RETCHK("); OPM.WriteModPos; OPM.WriteString(");"); OPM.WriteLn
		ELSIF ~eoBlock OR implicitRet THEN
			IF ~proc^.scope^.leaf THEN
				(* link scope pointer of nested proc back to previous scope *)
				IF indent THEN BegStat ELSE indent := TRUE END ;
				OPM.WriteString(proc^.scope^.name^); OPM.WriteString(GlobalScope);
				OPM.WriteString(Becomes); OPM.WriteString(LocalScope); OPM.Write(Dot); OPM.WriteString(LinkName);
				EndStat
			END;
			(* delete array value parameters *)
			var := proc^.link;
			WHILE genDelFunc & (var # NIL) DO
				IF (var^.typ^.comp = DynArr) & (var^.mode = Var) & (var^.typ^.sysflag = 0) THEN
					IF indent THEN BegStat ELSE indent := TRUE END;
					OPM.WriteString(DelFunc); Ident(var); OPM.Write(CloseParen); EndStat
				END;
				var := var^.link
			END
		END;
		IF eoBlock THEN EndBlk; OPM.WriteLn
		ELSIF indent THEN BegStat
		END;
		IF eoBlock & (proc^.vis = external) THEN
			OPM.WriteString("/*----------------------------------------------------------------------------*/"); OPM.WriteLn
		END
	END ExitProc;

	PROCEDURE CompleteIdent* (obj: OPT.Object);
		VAR comp, level: SHORTINT;
	BEGIN
		(* obj^.mode IN {Var, VarPar} *)
		level := obj^.mnolev;
		IF obj^.adr = 1 THEN	(* WITH-variable *)
			IF obj^.typ^.comp = Record THEN Ident(obj); OPM.WriteString("__")
			ELSE (* cast with guard pointer type *)
				OPM.WriteString("(("); Ident(obj^.typ^.strobj);
				IF obj.mode = VarPar THEN OPM.Write(Star) END;
				OPM.Write(")"); Ident(obj); OPM.Write(")")
			END
		ELSIF (level # OPM.level) & (level > 0) THEN (* intermediate var *)
			comp := obj^.typ^.comp;
			IF (comp = Array) OR (obj^.mode # VarPar) & (comp # DynArr) THEN OPM.WriteString("(*") END;
			OPM.WriteString(obj^.scope^.name^); OPM.WriteString(GlobalScope);
			OPM.WriteString("->"); Ident(obj);
			IF (comp = Array) OR (obj^.mode # VarPar) & (comp # DynArr) THEN OPM.Write(")") END
		ELSE
			Ident(obj)
		END
	END CompleteIdent;

	PROCEDURE CompleteIdentAdr* (obj: OPT.Object);
	BEGIN
		IF (obj^.adr = 1) & (obj^.typ^.comp # Record) THEN	(* WITH-variable *)
			OPM.Write("("); Ident(obj^.typ^.strobj); OPM.WriteString("*)&"); Ident(obj)
		ELSE
			OPM.Write("&"); CompleteIdent(obj)
		END
	END CompleteIdentAdr;

	PROCEDURE TypeOf*(ap: OPT.Object);
	BEGIN
		ASSERT(ap.typ.comp = Record);
		IF ap.mode = VarPar THEN
			IF ap.mnolev # OPM.level THEN	(*intermediate level var-par record; possible WITH-guarded*)
				OPM.WriteString(ap^.scope^.name^); OPM.WriteString("_s->"); Ident(ap)
			ELSE (*local var-par record*)
				Ident(ap)
			END ;
			OPM.WriteString(TagExt)
		ELSIF ap^.typ^.strobj # NIL THEN
			Ident(ap^.typ^.strobj); OPM.WriteString(DynTypExt)
		ELSE Andent(ap.typ)	(*anonymous ap type, p^ *)
		END
	END TypeOf;

	PROCEDURE Cmp*(rel: SHORTINT);
	BEGIN
		CASE rel OF
			eql :
					OPM.WriteString(" == ");
		|	neq :
					OPM.WriteString(" != ");
		|	lss :
					OPM.WriteString(" < ");
		|	leq :
					OPM.WriteString(" <= ");
		|	gtr :
					OPM.WriteString(" > ");
		|	geq :
					OPM.WriteString(" >= ");
		END;
	END Cmp;

	PROCEDURE WriteCharLiteral (ch: CHAR; wide: BOOLEAN);
	BEGIN
		IF (ch >= " ") & (ch <= "~") THEN
			OPM.Write(SingleQuote);
			IF (ch = "\") OR (ch = "?") OR (ch = SingleQuote) OR (ch = Quotes) THEN OPM.Write("\") END;
			OPM.Write(SHORT(ch)); OPM.Write(SingleQuote)
		ELSIF wide & (ch > 0FFX) THEN
			OPM.WriteString("0x"); OPM.WriteHex(ORD(ch) DIV 256); OPM.WriteHex(ORD(ch) MOD 256)
		ELSE
			ASSERT(ch <= 0FFX); OPM.WriteString("0x"); OPM.WriteHex(ORD(ch))
		END
	END WriteCharLiteral;

	PROCEDURE WriteStringLiteral (IN str: ARRAY OF SHORTCHAR; len: INTEGER);
		VAR ch: SHORTCHAR; i: INTEGER;
	BEGIN
		OPM.Write(Quotes);
		i := 0;
		WHILE i < len DO ch := str[i];
			IF (ch >= " ") & (ch <= "~") THEN
				IF (ch = "\") OR (ch = "?") OR (ch = SingleQuote) OR (ch = Quotes) THEN OPM.Write("\") END;
				OPM.Write(ch)
			ELSE
				OPM.WriteString("\x"); OPM.WriteHex(ORD(ch));
				(* terminate \x escape sequence without implementation dependency *)
				IF i + 1 < len THEN (* strings in Component Pascal may contain a special char *)
					CASE str[i + 1] OF
					"0".."9", "a".."f", "A".."F": OPM.Write(Quotes); OPM.Write(Quotes)
					ELSE
					END
				END
			END;
			INC(i)
		END;
		OPM.Write(Quotes)
	END WriteStringLiteral;

	PROCEDURE Case* (caseVal: INTEGER; form: SHORTINT);
	BEGIN
		OPM.WriteString(CaseStat);
		CASE form OF
		|	Char8, Char16:
					WriteCharLiteral(CHR(caseVal), form = Char16)
		|	Byte, UByte, SInt, Int, LInt:
					OPM.WriteInt(caseVal)
		END;
		OPM.WriteString(Colon)
	END Case;

	PROCEDURE SetInclude* (exclude: BOOLEAN);
	BEGIN
		IF exclude THEN OPM.WriteString(" &= ~"); ELSE OPM.WriteString(" |= "); END;
	END SetInclude;

	PROCEDURE Increment* (decrement: BOOLEAN);
	BEGIN
		IF decrement THEN OPM.WriteString(" -= "); ELSE OPM.WriteString(" += "); END;
	END Increment;

	PROCEDURE Halt* (n: INTEGER);
	BEGIN
		Str1("__HALT(#, ", n); OPM.WriteModPos; OPM.Write(")")
	END Halt;

	PROCEDURE Len* (obj: OPT.Object; array: OPT.Struct; dim: INTEGER);
		VAR d: INTEGER;
	BEGIN d := dim;
		WHILE d > 0 DO array := array^.BaseTyp; DEC(d) END;
		IF array^.comp = DynArr THEN
			IF ODD(array^.sysflag) THEN OPM.err(136) END;
			CompleteIdent(obj); OPM.WriteString(LenExt);
			IF dim # 0 THEN OPM.WriteInt(dim) END
		ELSE (* array *)
			OPM.WriteInt(array^.n); OPM.PromoteIntConstToLInt()
		END
	END Len;

	PROCEDURE Constant* (con: OPT.Const; form: SHORTINT);
		VAR i, x: INTEGER; s: SET; hex: INTEGER; skipLeading: BOOLEAN;
	BEGIN
		CASE form OF
		|	Bool, Byte, UByte, SInt, Int, LInt:
					OPM.WriteInt(con^.intval)
		|	Char8, Char16:
					WriteCharLiteral(CHR(con^.intval), form = Char16)
		|	Real:
					IF con^.realval = INF THEN OPM.WriteString("__INFS")
					ELSIF con^.realval = -INF THEN OPM.WriteString("-__INFS")
					ELSE OPM.WriteReal(con^.realval, "f")
					END
		|	LReal:
					IF con^.realval = INF THEN OPM.WriteString("__INF")
					ELSIF con^.realval = -INF THEN OPM.WriteString("-__INF")
					ELSE OPM.WriteReal(con^.realval, 0X)
					END
		|	Set:
					OPM.WriteString("0x");
					skipLeading := TRUE;
					s := con^.setval; i := MAX(SET) + 1;
					REPEAT
						hex := 0;
						REPEAT
							DEC(i); hex := 2 * hex;
							IF i IN s THEN INC(hex) END
						UNTIL i MOD 8 = 0;
						IF (hex # 0) OR ~skipLeading THEN
							OPM.WriteHex(hex);
							skipLeading := FALSE
						END
					UNTIL i = 0;
					IF skipLeading THEN OPM.Write("0") END
		|	String8:
					IF ansi THEN OPM.WriteString("(CHAR*)") END; (* force to unsigned char *)
					WriteStringLiteral(con^.ext^, con^.intval2 - 1)
		|	String16:
					OPM.WriteString("((LONGCHAR[]){");
					i := 0; OPM.GetUtf8(con^.ext, x, i);
					WHILE x # 0 DO
						OPM.WriteInt(x); OPM.WriteString(", "); OPM.GetUtf8(con^.ext, x, i)
					END;
					OPM.WriteString("0})")
		|	NilTyp:
					OPM.WriteString(NilConst)
		END;
	END Constant;


	PROCEDURE InitKeywords;
		VAR n, i: BYTE;

		PROCEDURE Enter(IN s: ARRAY OF SHORTCHAR);
			VAR h: SHORTINT;
		BEGIN h := PerfectHash(s); hashtab[h] := n; keytab[n] := s$; INC(n)
		END Enter;

	BEGIN n := 0;
		FOR i := 0 TO 104 DO hashtab[i] := -1 END;
		Enter("asm");
		Enter("auto");
		Enter("break");
		Enter("case");
		Enter("char");
		Enter("const");
		Enter("continue");
		Enter("default");
		Enter("do");
		Enter("double");
		Enter("else");
		Enter("enum");
		Enter("extern");
		Enter("export");	(* pseudo keyword used by ofront *)
		Enter("float");
		Enter("for");
		Enter("fortran");
		Enter("goto");
		Enter("if");
		Enter("import");	(* pseudo keyword used by ofront*)
		Enter("int");
		Enter("long");
		Enter("register");
		Enter("return");
		Enter("short");
		Enter("signed");
		Enter("sizeof");
		Enter("static");
		Enter("struct");
		Enter("switch");
		Enter("typedef");
		Enter("union");
		Enter("unsigned");
		Enter("void");
		Enter("volatile");
		Enter("while");

(* what about common predefined names from cpp as e.g.
               Operating System:   ibm, gcos, os, tss and unix
               Hardware:           interdata, pdp11,  u370,  u3b,
                                   u3b2,   u3b5,  u3b15,  u3b20d,
                                   vax, ns32000,  iAPX286,  i386,
                                   sparc , and sun
               UNIX system variant:
                                   RES, and RT
               The lint(1V) command:
                                   lint
 *)
	END InitKeywords;

BEGIN InitKeywords
END OfrontOPC.
