MODULE OfrontOPC;	(* copyright (c) J. Templ 12.7.95 / 3.7.96 *)
(* C source code generator version

	2000-04-30 jt, synchronized with BlackBox version, in particular
		various promotion rules changed (long) => (LONGINT), xxxL avoided
	2015-10-07 jt, fixed endless recursion in Stars for imported fields of anonymous type POINTER TO DynArr;
		param typ has a strobj pointer to an anonymous object, i.e. typ.strobj.name = OPT.null
	2018-02-14 olegncher, issue #44: unlimited length of strings (as in BB)
*)

	IMPORT OPT := OfrontOPT, OPM := OfrontOPM;

	CONST
		(* structure forms *)
		Byte = 1; Bool = 2; Char = 3; SInt = 4; Int = 5; LInt = 6;
		Real = 7; LReal = 8; Set = 9; String = 10; NilTyp = 11; NoTyp = 12;
		Pointer = 13; ProcTyp = 14; Comp = 15;

		(* composite structure forms *)
		Array = 2; DynArr = 3; Record = 4;

		(* object history *)
		removed = 4;

		(* object modes *)
		Var = 1; VarPar = 2; Fld = 4; Typ = 5; LProc = 6; XProc = 7;
		CProc = 9; Mod = 11; TProc = 13;

		(* symbol values and ops *)
		eql = 9; neq = 10; lss = 11; leq = 12; gtr = 13; geq = 14;

		(* nodes classes *)
		Ninittd = 14;

		(* module visibility of objects *)
		internal = 0; external = 1;

		UndefinedType = 0; (* named type not yet defined *)
		ProcessingType = 1; (* pointer type is being processed *)
		PredefinedType = 2; (* for all predefined types *)
		DefinedInHdr = 3+OPM.HeaderFile; (* named type has been defined in header file *)
		DefinedInBdy = 3+OPM.BodyFile; (* named type has been defined in body file *)


		HeaderMsg = "Ofront+ 1.0 -";
		BasicIncludeFile = "SYSTEM";
		Static = "static ";
		Export = "export ";	(* particularily introduced for VC++ declspec() *)
		Extern = "import ";	(* particularily introduced for VC++ declspec() *)
		EXTERN = "__EXTERN ";	(* import from a dynamic library *)
		STDCALL = "__STDCALL ";	(* stdcall calling convention *)
		Struct = "struct ";
		LocalScope = "_s"; (* name of a local intermediate scope (variable name) *)
		GlobalScope = "_s"; (* pointer to current scope extension *)
		LinkName = "lnk"; (* pointer to previous scope field *)
		FlagExt = "__h";
		LenExt = "__len";
		DynTypExt = "__typ";
		TagExt = "__typ";

		OpenParen = "(";
		CloseParen = ")";
		OpenBrace = "{";
		CloseBrace = "}";
		OpenBracket = "[";
		CloseBracket = "]";
		Underscore = "_";
		Quotes = 22X;
		SingleQuote = 27X;
		Tab = 9X;
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
		indentLevel: SHORTINT;
		ptrinit, mainprog, ansi: BOOLEAN;
		hashtab: ARRAY 105 OF BYTE;
		keytab: ARRAY 36, 9 OF CHAR;
		GlbPtrs: BOOLEAN;
		BodyNameExt: ARRAY 13 OF CHAR;

	PROCEDURE Init*;
	BEGIN
		indentLevel := 0;
		ptrinit := OPM.ptrinit IN OPM.opt;
		mainprog := OPM.mainprog IN OPM.opt;
		ansi := OPM.ansi IN OPM.opt;
		IF ansi THEN BodyNameExt := "__init(void)" ELSE BodyNameExt := "__init()" END
	END Init;

	PROCEDURE Indent* (count: SHORTINT);
	BEGIN INC(indentLevel, count)
	END Indent;

	PROCEDURE BegStat*;
		VAR i: SHORTINT;
	BEGIN i := indentLevel;
		WHILE i > 0 DO OPM.Write(Tab); DEC (i) END
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

	PROCEDURE Str1(IN s: ARRAY OF CHAR; x: INTEGER);
		VAR ch: CHAR; i: SHORTINT;
	BEGIN ch := s[0]; i := 0;
		WHILE ch # 0X DO
			IF ch = "#" THEN OPM.WriteInt(x)
			ELSE OPM.Write(ch);
			END;
			INC(i); ch := s[i]
		END
	END Str1;

	PROCEDURE Length(IN s: ARRAY OF CHAR): SHORTINT;
		VAR i: SHORTINT;
	BEGIN i := 0;
		WHILE s[i] # 0X DO INC(i) END;
		RETURN i
	END Length;

	PROCEDURE PerfectHash (IN s: ARRAY OF CHAR): SHORTINT;
		VAR i, h: SHORTINT;
	BEGIN i := 0; h := 0;
		WHILE (s[i] # 0X) & (i < 5) DO h := 3*h + ORD(s[i]); INC(i) END;
		RETURN h MOD 105
	END PerfectHash;

	PROCEDURE Ident* (obj: OPT.Object);
		VAR mode, level, h: SHORTINT;
	BEGIN
		mode := obj^.mode; level := obj^.mnolev;
		IF (mode IN {Var, Typ, LProc}) & (level > 0) OR (mode IN {Fld, VarPar}) THEN
			OPM.WriteString(obj^.name^);
			h := PerfectHash(obj^.name^);
			IF hashtab[h] >= 0 THEN
				IF keytab[hashtab[h]] = obj^.name^ THEN OPM.Write(Underscore) END
			END
		ELSE
			IF (mode # Typ) OR (obj^.linkadr # PredefinedType) THEN
				IF mode = TProc THEN Ident(obj^.link^.typ^.strobj)
				ELSIF level < 0 THEN (* use unaliased module name *)
					OPM.WriteString(OPT.GlbMod[-level].name^);
					IF OPM.currFile = OPM.HeaderFile THEN OPT.GlbMod[-level].vis := 1 (*include in header file*) END ;
				ELSE OPM.WriteString(OPM.modName)
				END ;
				OPM.Write(Underscore)
			ELSIF obj = OPT.sysptrtyp^.strobj THEN
				OPM.WriteString("SYSTEM_")
			END ;
			OPM.WriteString(obj^.name^)
		END
	END Ident;

	PROCEDURE Stars (typ: OPT.Struct; VAR openClause: BOOLEAN);
		VAR pointers: SHORTINT;
	BEGIN
		openClause := FALSE;
		IF ((typ^.strobj = NIL) OR (typ^.strobj^.name = OPT.null)) & (typ^.comp # Record) THEN
			IF typ^.comp IN {Array, DynArr} THEN
				Stars (typ^.BaseTyp, openClause);
				openClause := (typ^.comp = Array)
			ELSIF typ^.form = ProcTyp THEN
				OPM.Write(OpenParen);
				IF typ^.sysflag # 0 THEN OPM.WriteString(STDCALL) END; OPM.Write(Star)
			ELSE
				pointers := 0;
				WHILE ((typ^.strobj = NIL) OR (typ^.strobj^.name = OPT.null)) & (typ^.form = Pointer) DO
					INC (pointers); typ := typ^.BaseTyp
				END ;
				IF pointers > 0 THEN
					IF typ^.comp # DynArr THEN Stars (typ, openClause) END ;
					IF openClause THEN OPM.Write(OpenParen); openClause := FALSE END ;
					WHILE pointers > 0 DO OPM.Write(Star); DEC (pointers) END
				END
			END
		END
	END Stars;

	PROCEDURE ^AnsiParamList (obj: OPT.Object; showParamNames, showOberonParams: BOOLEAN);

	PROCEDURE DeclareObj(dcl: OPT.Object; scopeDef: BOOLEAN);
		VAR
			typ: OPT.Struct;
			varPar, openClause: BOOLEAN; form, comp: SHORTINT;
	BEGIN
		typ := dcl^.typ;
		varPar := ((dcl^.mode = VarPar) & (typ^.comp # Array)) OR (typ^.comp = DynArr) OR scopeDef;
		Stars(typ, openClause);
		IF varPar THEN
			IF openClause THEN OPM.Write(OpenParen) END ;
			OPM.Write(Star)
		END ;
		IF dcl.name # OPT.null THEN Ident(dcl) END ;
		IF varPar & openClause THEN OPM.Write(CloseParen) END ;
		openClause := FALSE;
		LOOP
			form := typ^.form;
			comp := typ^.comp;
			IF ((typ^.strobj # NIL) & (typ^.strobj^.name # OPT.null)) OR (form = NoTyp) OR (comp = Record) THEN EXIT
			ELSIF (form = Pointer) & (typ^.BaseTyp^.comp # DynArr) THEN
				openClause := TRUE
			ELSIF (form = ProcTyp) OR (comp IN {Array, DynArr}) THEN
				IF openClause THEN OPM.Write(CloseParen); openClause := FALSE END ;
				IF form = ProcTyp THEN
					IF ansi THEN OPM.Write(")"); AnsiParamList(typ^.link, FALSE, TRUE)
					ELSE OPM.WriteString(")()")
					END ;
					EXIT
				ELSIF comp = Array THEN
					OPM.Write(OpenBracket); OPM.WriteInt(typ^.n); OPM.Write(CloseBracket)
				END
			ELSE
				EXIT
			END ;
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
		RETURN (obj^.mnolev >= 0) & (obj^.linkadr # 3+OPM.currFile ) & (obj^.linkadr # PredefinedType) OR (obj^.name = OPT.null)
	END Undefined;

	PROCEDURE ^FieldList (typ: OPT.Struct; last: BOOLEAN; VAR off, n, curAlign: INTEGER);

	PROCEDURE DeclareBase(dcl: OPT.Object); (* declare the specifier of object dcl*)
		VAR typ, prev: OPT.Struct; obj: OPT.Object; nofdims: SHORTINT; off, n, dummy: INTEGER;
	BEGIN
		typ := dcl^.typ; prev := typ;
		WHILE ((typ^.strobj = NIL) OR (typ^.comp = DynArr) OR Undefined(typ^.strobj)) & (typ^.comp # Record) & (typ^.form # NoTyp)
			& ~((typ^.form = Pointer) & (typ^.BaseTyp^.comp = DynArr)) DO
			prev := typ; typ := typ^.BaseTyp
		END ;
		obj := typ^.strobj;
		IF typ^.form = NoTyp THEN	(* proper procedure *)
			OPM.WriteString(VoidType)
		ELSIF (obj # NIL) & ~Undefined(obj) THEN	(* named type, already declared *)
			Ident(obj)
		ELSIF typ^.comp = Record THEN
			OPM.WriteString(Struct); Andent(typ);
			IF (prev.form # Pointer) & ((obj # NIL) OR (dcl.name = OPT.null)) THEN
				(* named record type not yet declared OR anonymous record with empty name *)
				IF (typ^.BaseTyp # NIL) & (typ^.BaseTyp^.strobj.vis # internal) THEN
					OPM.WriteString(" { /* "); Ident(typ^.BaseTyp^.strobj); OPM.WriteString(" */"); OPM.WriteLn; Indent(1)
				ELSE OPM.Write(Blank); BegBlk
				END ;
				FieldList(typ, TRUE, off, n, dummy);
				EndBlk0
			END
		ELSIF (typ^.form = Pointer) & (typ^.BaseTyp^.comp = DynArr) THEN
			typ := typ^.BaseTyp^.BaseTyp; nofdims := 1;
			WHILE typ^.comp = DynArr DO INC(nofdims); typ := typ^.BaseTyp END ;
			OPM.WriteString(Struct); BegBlk;
			BegStat;
			IF OPM.IndexSize = OPM.IntSize THEN Str1("INTEGER len[#]", nofdims)
			ELSE Str1("LONGINT len[#]", nofdims)
			END;
			EndStat;
			BegStat; NEW(obj); NEW(obj.typ);	(* aux. object for easy declaration *)
			obj.typ.form := Comp; obj.typ.comp := Array; obj.typ.n := 1; obj.typ.BaseTyp := typ; obj.mode := Fld; obj.name := OPT.NewName("data");
			obj.linkadr := UndefinedType; DeclareBase(obj); OPM.Write(Blank);  DeclareObj(obj, FALSE);
			EndStat; EndBlk0
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
			IF cnt  MOD 16 = 0 THEN OPM.WriteLn; OPM.Write(Tab) END
		ELSIF (typ^.comp = Record) & (typ^.sysflag MOD 100H = 0) THEN
			btyp := typ^.BaseTyp;
			IF btyp # NIL THEN PutPtrOffsets(btyp, adr, cnt) END ;
			fld := typ^.link;
			WHILE (fld # NIL) & (fld^.mode = Fld) DO
				IF fld^.name^ # OPM.HdPtrName THEN PutPtrOffsets(fld^.typ, adr + fld^.adr, cnt)
				ELSE
					OPM.WriteInt(adr + fld^.adr); OPM.WriteString(", "); INC(cnt);
					IF cnt MOD 16 = 0 THEN OPM.WriteLn; OPM.Write(Tab) END
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
		IF showParamName THEN Ident(par); OPM.WriteString(LenExt) END ;
		dim := 1; typ := par^.typ^.BaseTyp;
		WHILE (typ^.comp = DynArr) & ~ODD(typ^.sysflag) DO
			IF ansiDefine THEN
				IF OPM.IndexSize = OPM.IntSize THEN OPM.WriteString(", INTEGER ")
				ELSE OPM.WriteString(", LONGINT ")
				END
			ELSE OPM.WriteString(Comma)
			END ;
			IF showParamName THEN Ident(par); OPM.WriteString(LenExt); OPM.WriteInt(dim) END ;
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
					ELSE OPM.WriteString(Export)
					END ;
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
				END ;
				OPM.WriteString(", "); Ident(obj);
				Str1(", #, ", obj^.adr DIV 10000H);
				IF obj^.typ = OPT.notyp THEN OPM.WriteString(VoidType) ELSE Ident(obj^.typ^.strobj) END ;
				OPM.WriteString("(*)");
				IF ansi THEN
					AnsiParamList(obj^.link, FALSE, TRUE);
				ELSE
					OPM.WriteString("()");
				END ;
				OPM.WriteString(", ");
				DeclareParams(obj^.link, TRUE, TRUE);
				OPM.Write(")"); OPM.WriteLn
			END ;
			DefineTProcMacros(obj^.right, empty)
		END
	END DefineTProcMacros;

	PROCEDURE DefineType(str: OPT.Struct); (* define a type object *)
		VAR obj, field, par: OPT.Object; empty: BOOLEAN;
	BEGIN
		IF (OPM.currFile = OPM.BodyFile) OR (str^.ref < OPM.MaxStruct (*for hidden exports*) ) THEN
			obj := str^.strobj;
			IF (obj = NIL) OR Undefined(obj) THEN
				IF obj # NIL THEN (* check for cycles *)
					IF obj^.linkadr = ProcessingType THEN
						IF str^.form # Pointer THEN OPM.Mark(244, str^.txtpos); obj^.linkadr := PredefinedType END
					ELSE obj^.linkadr := ProcessingType
					END
				END ;
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
					END ;
					DefineType(str^.BaseTyp)
				ELSIF str^.form = ProcTyp THEN
					IF str^.BaseTyp # OPT.notyp THEN DefineType(str^.BaseTyp) END ;
					field := str^.link;
					WHILE field # NIL DO DefineType(field^.typ); field := field^.link END
				END
			END ;
			IF (obj # NIL) & Undefined(obj) THEN
				OPM.WriteString("typedef"); OPM.WriteLn; OPM.Write(Tab); Indent(1);
				obj^.linkadr := ProcessingType;
				DeclareBase(obj); OPM.Write(Blank);
				obj^.typ^.strobj := NIL; (* SG: trick to make DeclareObj declare the type *)
				DeclareObj(obj, FALSE);
				obj^.typ^.strobj := obj; (* SG: revert trick *)
				obj^.linkadr := 3+OPM.currFile;
				EndStat; Indent(-1); OPM.WriteLn;
				IF obj^.typ^.comp = Record THEN empty := TRUE;
					DeclareTProcs(str^.link, empty); DefineTProcMacros(str^.link, empty);
					IF ~empty THEN OPM.WriteLn END
				END
			END
		END
	END DefineType;

	PROCEDURE Prefixed(x: OPT.ConstExt; IN y: ARRAY OF CHAR): BOOLEAN;
		VAR i: SHORTINT;
	BEGIN i := 0;
		WHILE x[i] = y[i] DO INC(i) END ;
		RETURN y[i] = 0X
	END Prefixed;

	PROCEDURE CProcDefs(obj: OPT.Object; vis: SHORTINT);
		VAR i: INTEGER; ext: OPT.ConstExt;
	BEGIN
		IF obj # NIL THEN
			CProcDefs(obj^.left, vis);
			(* bug: obj.history cannot be used to cover unexported and deleted CProcs; use special flag obj.adr = 1 *)
			IF (obj^.mode = CProc) & (obj^.vis >= vis) & (obj^.adr = 1) THEN
				ext := obj.conval.ext;
				IF ext # NIL THEN
					IF (ext^[0] # "#") & ~(Prefixed(ext, "extern ") OR Prefixed(ext, Extern)) THEN
						OPM.WriteString("#define "); Ident(obj); DeclareParams(obj^.link, TRUE, TRUE);
						OPM.Write(Tab)
					END;
					FOR i := 0 TO LEN(ext^) - 1 DO OPM.Write(ext^[i]) END;
				ELSE
					OPM.WriteString(EXTERN);
					IF obj^.typ = OPT.notyp THEN OPM.WriteString(VoidType) ELSE Ident(obj^.typ^.strobj) END;
					OPM.Write(Blank); IF obj^.sysflag # 0 THEN OPM.WriteString(STDCALL) END;
					OPM.WriteString(obj^.name^); AnsiParamList(obj^.link, TRUE, FALSE); OPM.Write(";");
					OPM.WriteLn;
					OPM.WriteString("#define "); Ident(obj); DeclareParams(obj^.link, TRUE, TRUE);
					OPM.Write(Tab);
					OPM.WriteString(obj^.name^); DeclareParams(obj^.link, TRUE, FALSE)
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

	PROCEDURE TDescDecl* (typ: OPT.Struct);
		VAR nofptrs: INTEGER;
			o: OPT.Object;
	BEGIN
		IF ~ODD(typ^.sysflag) THEN
			BegStat; OPM.WriteString("__TDESC(");
			Andent(typ); OPM.WriteString("__desc");
			Str1(", #", typ^.n + 1); Str1(", #) = {__TDFLDS(", NofPtrs(typ));
			OPM.Write('"');
			IF typ^.strobj # NIL THEN OPM.WriteString(typ^.strobj^.name^) END ;
			Str1('", #), {', typ^.size);
			nofptrs := 0; PutPtrOffsets(typ, 0, nofptrs); Str1("#}}", -(nofptrs + 1) * OPM.PointerSize);
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
			IF align = OPM.IntSize THEN OPM.WriteString("INTEGER")
			ELSIF align = OPM.LIntSize THEN OPM.WriteString("LONGINT")
			ELSIF align = OPM.LRealSize THEN OPM.WriteString("LONGREAL")
			END ;
			Str1(" _prvt#", n); INC(n); EndStat;
			curAlign := align
		END ;
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
				BegStat; DeclareBase(fld); OPM.Write(Blank); DeclareObj(fld, FALSE);
				off := fld.adr + fld.typ.size; base := fld.typ; fld := fld.link;
				WHILE (fld # NIL) & (fld.mode = Fld) & (fld.typ = base) & (fld.adr = off)
(* ?? *)		& ((OPM.currFile = OPM.BodyFile) OR (fld.vis # internal) OR (fld.typ.strobj = NIL)) DO
					OPM.WriteString(", "); DeclareObj(fld, FALSE); off := fld.adr + fld.typ.size; fld := fld.link
				END ;
				EndStat
			END
		END ;
		IF last THEN
			adr := typ.size - typ^.sysflag DIV 100H;
			IF adr = 0 THEN gap := 1 (* avoid empty struct *) ELSE gap := adr - off END ;
			IF gap > 0 THEN FillGap(gap, off, align, n, curAlign) END
		END
	END FieldList;

	PROCEDURE IdentList (obj: OPT.Object; vis: SHORTINT);
	(* generate var and param lists; vis: 0 all global vars, local var, 1 exported(R) var, 2 par list, 3 scope var *)
		VAR base: OPT.Struct; first: BOOLEAN; lastvis: SHORTINT;
	BEGIN
		base := NIL; first := TRUE;
		WHILE (obj # NIL) & (obj^.mode # TProc) DO
			IF (vis IN {0, 2}) OR ((vis = 1) & (obj^.vis # 0)) OR ((vis = 3) & ~obj^.leaf) THEN
				IF (obj^.typ # base) OR (obj^.vis # lastvis) THEN	(* new variable base type definition required *)
					IF ~first THEN EndStat END ;
					first := FALSE;
					base := obj^.typ; lastvis := obj^.vis;
					BegStat;
					IF (vis = 1) & (obj^.vis # internal) THEN OPM.WriteString(Extern)
					ELSIF (obj^.mnolev = 0) & (vis = 0) THEN
						IF obj^.vis = internal THEN OPM.WriteString(Static)
						ELSE OPM.WriteString(Export)
						END
					END ;
					IF (vis = 2) & (obj^.mode = Var) & (base^.form = Real) THEN OPM.WriteString("double")
					ELSE DeclareBase(obj)
					END
				ELSE OPM.Write(",");
				END ;
				OPM.Write(Blank);
				IF (vis = 2) & (obj^.mode = Var) & (base^.form = Real) THEN OPM.Write("_") END ;
				DeclareObj(obj, vis = 3);
				IF (obj^.typ^.comp = DynArr) & ~ODD(obj^.typ^.sysflag) THEN (* declare len parameter(s) *)
					EndStat; BegStat;
					base := OPT.linttyp;
					IF OPM.IndexSize = OPM.IntSize THEN OPM.WriteString("INTEGER ")
					ELSE OPM.WriteString("LONGINT ")
					END;
					LenList(obj, FALSE, TRUE)
				ELSIF (obj^.mode = VarPar) & (obj^.typ^.comp = Record) & ~ODD(obj^.typ^.sysflag) THEN
					EndStat; BegStat;
					OPM.WriteString("SYSTEM_ADRINT *"); Ident(obj); OPM.WriteString(TagExt);
					base := NIL
				ELSIF ptrinit & (vis = 0) & (obj^.mnolev > 0) & (obj^.typ^.form = Pointer) THEN
					OPM.WriteString(" = NIL")
				END
			END ;
			obj := obj^.link
		END ;
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
					OPM.Write(Blank); DeclareObj(obj, FALSE)
				ELSE
					name := obj^.name; obj^.name := OPT.null; DeclareObj(obj, FALSE); obj^.name := name
				END ;
				IF showOberonParams THEN
					IF (obj^.typ^.comp = DynArr) & ~ODD(obj^.typ^.sysflag) THEN
						IF OPM.IndexSize = OPM.IntSize THEN OPM.WriteString(", INTEGER ")
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

	PROCEDURE ProcHeader(proc: OPT.Object; define: BOOLEAN);
	BEGIN
		IF proc^.typ = OPT.notyp THEN OPM.WriteString(VoidType) ELSE Ident(proc^.typ^.strobj) END;
		OPM.Write(Blank); IF proc^.sysflag # 0 THEN OPM.WriteString(STDCALL) END;
		Ident(proc); OPM.Write(Blank);
		IF ansi THEN
			AnsiParamList(proc^.link, TRUE, TRUE);
			IF ~define THEN OPM.Write(";") END;
			OPM.WriteLn
		ELSIF define THEN
			DeclareParams(proc^.link, FALSE, TRUE);
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
				IF vis = external THEN OPM.WriteString(Extern)
				ELSIF obj^.vis = internal THEN OPM.WriteString(Static)
				ELSE OPM.WriteString(Export)
				END ;
				ProcHeader(obj, FALSE);
			END ;
			ProcPredefs(obj^.right, vis);
		END;
	END ProcPredefs;

	PROCEDURE Include(IN name: ARRAY OF CHAR);
	BEGIN
		OPM.WriteString("#include "); OPM.Write(Quotes); OPM.WriteString(name);
		OPM.WriteString(".h"); OPM.Write(Quotes); OPM.WriteLn
	END Include;

	PROCEDURE IncludeImports(obj: OPT.Object; vis: SHORTINT);
	BEGIN
		IF obj # NIL THEN
			IncludeImports(obj^.left, vis);
			IF (obj^.mode = Mod) & (obj^.mnolev # 0) & (OPT.GlbMod[-obj^.mnolev].vis >= vis) THEN	(* @self and SYSTEM have mnolev = 0 *)
				Include(OPT.GlbMod[-obj^.mnolev].name^)	(* use unaliased module name *)
			END;
			IncludeImports(obj^.right, vis);
		END;
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
		IdentList(OPT.topScope^.scope, 1); OPM.WriteLn;
		GenDynTypes(n, external); OPM.WriteLn;
		ProcPredefs(OPT.topScope^.right, 1);
		OPM.WriteString(Extern); OPM.WriteString("void *");
		OPM.WriteString(OPM.modName); OPM.WriteString(BodyNameExt);
		EndStat; OPM.WriteLn;
		CProcDefs(OPT.topScope^.right, 1); OPM.WriteLn;
		OPM.WriteString("#endif"); OPM.WriteLn
	END GenHdr;

	PROCEDURE GenHeaderMsg;
		VAR opt: BOOLEAN;
	BEGIN
		opt := FALSE;
		OPM.WriteString("/* "); OPM.WriteString(HeaderMsg);
		IF OPM.extsf IN OPM.opt THEN OPM.Write("e"); opt := TRUE END;
		IF OPM.newsf IN OPM.opt THEN OPM.Write("s"); opt := TRUE END;
		IF OPM.mainprog IN OPM.opt THEN OPM.Write("m"); opt := TRUE END;
		IF OPM.inxchk IN OPM.opt THEN OPM.Write("x"); opt := TRUE END;
		IF OPM.vcpp IN OPM.opt THEN OPM.Write("v"); opt := TRUE END;
		IF OPM.ranchk IN OPM.opt THEN OPM.Write("r"); opt := TRUE END;
		IF OPM.typchk IN OPM.opt THEN OPM.Write("t"); opt := TRUE END;
		IF OPM.assert IN OPM.opt THEN OPM.Write("a"); opt := TRUE END;
		IF ~(OPM.ansi IN OPM.opt) THEN OPM.Write("k"); opt := TRUE END;
		IF OPM.ptrinit IN OPM.opt THEN OPM.Write("p"); opt := TRUE END;
		IF OPM.include0 IN OPM.opt THEN OPM.Write("i"); opt := TRUE END;
		IF OPM.Language # 0X THEN OPM.Write(OPM.Language) END;
		IF opt THEN OPM.WriteString(" -") END;
		OPM.WriteInt(OPM.AddressSize);
		IF OPM.AddressSize # 2 THEN OPM.WriteInt(OPM.Alignment) ELSE OPM.WriteInt(OPM.SetSize) END;
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
		IncludeImports(OPT.topScope^.right, 1); OPM.WriteLn
	END GenHdrIncludes;

	PROCEDURE GenBdy*(n: OPT.Node);
	BEGIN
		OPM.currFile := OPM.BodyFile;
		GenHeaderMsg;
		Include(BasicIncludeFile);
		IncludeImports(OPT.topScope^.right, 0); OPM.WriteLn;
		DefAnonRecs(n);
		TypeDefs(OPT.topScope^.right, 0); OPM.WriteLn;
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
			IF (obj^.mode = Mod) & (obj^.mnolev # 0) THEN
				BegStat; OPM.WriteString("__IMPORT(");
				OPM.WriteString(OPT.GlbMod[-obj^.mnolev].name^); OPM.WriteString("__init");
				OPM.Write(CloseParen); EndStat
			END ;
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
						OPM.Write(Tab); OPM.WriteString("void (*P)();");
					END ;
					OPM.WriteLn;
					BegBlk
				END ;
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
				END ;
				EndStat
			END ;
			var := var^.link
		END ;
		IF GlbPtrs THEN
			EndBlk; OPM.WriteLn
		END
	END GenEnumPtrs;

	PROCEDURE EnterBody*;
	BEGIN
		OPM.WriteLn;
		IF mainprog THEN
			IF ansi THEN
				OPM.WriteString("int main(int argc, char **argv)"); OPM.WriteLn
			ELSE
				OPM.WriteString("main(argc, argv)"); OPM.WriteLn;
				OPM.Write(Tab); OPM.WriteString("int argc; char **argv;"); OPM.WriteLn
			END
		ELSE
			OPM.WriteString(Export); OPM.WriteString("void *");
			OPM.WriteString(OPM.modName); OPM.WriteString(BodyNameExt); OPM.WriteLn;
		END ;
		BegBlk; BegStat;
		IF mainprog THEN OPM.WriteString("__INIT(argc, argv)") ELSE OPM.WriteString("__DEFMOD") END ;
		EndStat;
		InitImports(OPT.topScope^.right);
		BegStat;
		IF mainprog THEN OPM.WriteString('__REGMAIN("') ELSE OPM.WriteString('__REGMOD("') END ;
		OPM.WriteString(OPM.modName);
		IF GlbPtrs THEN OPM.WriteString('", EnumPtrs)') ELSE OPM.WriteString('", 0)') END ;
		EndStat;
		IF OPM.modName # "SYSTEM" THEN RegCmds(OPT.topScope) END
	END EnterBody;

	PROCEDURE ExitBody*;
	BEGIN
		BegStat;
		IF mainprog THEN OPM.WriteString("__FINI;") ELSE OPM.WriteString("__ENDMOD;") END ;
		OPM.WriteLn; EndBlk
	END ExitBody;

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
			OPM.Write(Tab); OPM.WriteString("__RETCHK;"); OPM.WriteLn
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
			WHILE var # NIL DO
				IF (var^.typ^.comp = DynArr) & (var^.mode = Var) & (var^.typ^.sysflag = 0) THEN
					IF indent THEN BegStat ELSE indent := TRUE END ;
					OPM.WriteString(DelFunc); Ident(var); OPM.Write(CloseParen); EndStat
				END ;
				var := var^.link
			END
		END ;
		IF eoBlock THEN EndBlk; OPM.WriteLn
		ELSIF indent THEN BegStat
		END;
		IF eoBlock & (proc^.vis = external) THEN
			OPM.WriteString("/*----------------------------------------------------------------------------*/"); OPM.WriteLn
		END
	END ExitProc;

	PROCEDURE CompleteIdent*(obj: OPT.Object);
		VAR comp, level: SHORTINT;
	BEGIN
		(* obj^.mode IN {Var, VarPar} *)
		level := obj^.mnolev;
		IF obj^.adr = 1 THEN	(* WITH-variable *)
			IF obj^.typ^.comp = Record THEN Ident(obj); OPM.WriteString("__")
			ELSE (* cast with guard pointer type *)
				OPM.WriteString("(("); Ident(obj^.typ^.strobj); OPM.Write(")"); Ident(obj); OPM.Write(")")
			END
		ELSIF (level # OPM.level) & (level > 0) THEN (* intermediate var *)
			comp := obj^.typ^.comp;
			IF (obj^.mode # VarPar) & (comp # DynArr) THEN OPM.Write(Star); END;
			OPM.WriteString(obj^.scope^.name^); OPM.WriteString(GlobalScope);
			OPM.WriteString("->"); Ident(obj)
		ELSE
			Ident(obj)
		END
	END CompleteIdent;

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

	PROCEDURE WriteCharLiteral(ch: CHAR); (* with escape for special printable characters *)
	BEGIN
		IF (ch >= " ") & (ch <= "~") THEN
			OPM.Write(SingleQuote);
			IF (ch = "\") OR (ch = "?") OR (ch = SingleQuote) OR (ch = Quotes) THEN OPM.Write("\") END;
			OPM.Write(ch); OPM.Write(SingleQuote)
		ELSE
			OPM.WriteString("0x"); OPM.WriteHex(ORD(ch))
		END
	END WriteCharLiteral;

	PROCEDURE WriteStringLiteral(IN str: ARRAY OF CHAR; len: INTEGER);
		VAR ch: CHAR; i: INTEGER;
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

	PROCEDURE Case*(caseVal: INTEGER; form: SHORTINT);
	BEGIN
		OPM.WriteString(CaseStat);
		CASE form OF
		|	Char :
					WriteCharLiteral(CHR(caseVal))
		|	Byte, SInt, Int, LInt :
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
		Str1("__HALT(#)", n)
	END Halt;

	PROCEDURE Len* (obj: OPT.Object; array: OPT.Struct; dim: INTEGER);
	BEGIN
		IF array^.comp = DynArr THEN
			IF ODD(array^.sysflag) THEN OPM.err(136) END;
			CompleteIdent(obj); OPM.WriteString(LenExt);
			IF dim # 0 THEN OPM.WriteInt(dim) END
		ELSE (* array *)
			WHILE dim > 0 DO array := array^.BaseTyp; DEC(dim) END ;
			OPM.WriteInt(array^.n); OPM.PromoteIntConstToLInt()
		END
	END Len;

	PROCEDURE Constant* (con: OPT.Const; form: SHORTINT);
		VAR i: SHORTINT; s: SET; hex: INTEGER; skipLeading: BOOLEAN;
	BEGIN
		CASE form OF
		|	Bool, Byte, SInt, Int, LInt:
					OPM.WriteInt(con^.intval)
		|	Char:
					WriteCharLiteral(CHR(con^.intval))
		|	Real:
					OPM.WriteReal(con^.realval, "f")
		|	LReal:
					OPM.WriteReal(con^.realval, 0X)
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
		|	String:
					WriteStringLiteral(con^.ext^, con^.intval2 - 1)
		|	NilTyp:
					OPM.WriteString(NilConst)
		END;
	END Constant;


	PROCEDURE InitKeywords;
		VAR n, i: BYTE;

		PROCEDURE Enter(IN s: ARRAY OF CHAR);
			VAR h: SHORTINT;
		BEGIN h := PerfectHash(s); hashtab[h] := n; COPY(s, keytab[n]); INC(n)
		END Enter;

	BEGIN n := 0;
		FOR i := 0 TO 104 DO hashtab[i] := -1 END ;
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
