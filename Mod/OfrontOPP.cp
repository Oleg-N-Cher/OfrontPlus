MODULE OfrontOPP;	(* NW, RC 6.3.89 / 10.2.94 *)	(* object model 4.12.93 *)

(* changes
  	2015-10-07 jt, check for IMPORT SYSTEM added in CheckSysFlag
*)

	IMPORT
		OPB := OfrontOPB, OPT := OfrontOPT, OPS := OfrontOPS, OPM := OfrontOPM;

	CONST
		AnchorVarPar = TRUE;

		(* numtyp values *)
		char = 1; integer = 2; real = 3; longreal = 4;

		(* symbol values *)
		null = 0; times = 1; slash = 2; div = 3; mod = 4;
		div0 = 5; rem0 = 6; and = 7; plus = 8; minus = 9;
		or = 10; eql = 11; neq = 12; lss = 13; leq = 14;
		gtr = 15; geq = 16; in = 17; is = 18; arrow = 19;
		period = 20; comma = 21; colon = 22; upto = 23; rparen = 24;
		rbrak = 25; rbrace = 26; of = 27; then = 28; do = 29;
		to = 30; by = 31; lparen = 32; lbrak = 33; lbrace = 34;
		not = 35; becomes = 36; number = 37; nil = 38; string = 39;
		ident = 40; semicolon = 41; bar = 42; end = 43; else = 44;
		elsif = 45; until = 46; if = 47; case = 48; while = 49;
		repeat = 50; for = 51; loop = 52; with = 53; exit = 54;
		return = 55; array = 56; record = 57; pointer = 58; begin = 59;
		const = 60; type = 61; var = 62; procedure = 63; import = 64;
		module = 65; out = 66; dollar = 67; close = 68;
		at = 69; raw = 70; eof = 71;
		unsgn = 40;	(* pseudo-operation 'unsigned' for div *)

		(* object modes *)
		Var = 1; VarPar = 2; Con = 3; Fld = 4; Typ = 5; LProc = 6; XProc = 7;
		SProc = 8; CProc = 9; IProc = 10; Mod = 11; Head = 12; TProc = 13; Attr = 20;

		(* Structure forms *)
		Undef = 0; Byte = 1; Bool = 2; Char8 = 3; SInt = 4; Int = 5; LInt = 6;
		Real = 7; LReal = 8; Set = 9; String8 = 10; NilTyp = 11; NoTyp = 12;
		Pointer = 13; UByte = 14; ProcTyp = 15; Comp = 16;
		Char16 = 17; String16 = 18;
		intSet = {Byte, UByte, SInt..LInt}; charSet = {Char8, Char16};

		(* composite structure forms *)
		Basic = 1; Array = 2; DynArr = 3; Record = 4;

		(*function number*)
		haltfn = 0; newfn = 1; incfn = 19; decfn = 20; sysnewfn = 36;

		(* nodes classes *)
		Nvar = 0; Nvarpar = 1; Nfield = 2; Nderef = 3; Nindex = 4; Nguard = 5; Neguard = 6;
		Nconst = 7; Ntype = 8; Nproc = 9; Nupto = 10; Nmop = 11; Ndop = 12; Ncall = 13;
		Ninittd = 14; Nif = 15; Ncaselse = 16; Ncasedo = 17; Nenter = 18; Nassign = 19;
		Nifelse = 20; Ncase = 21; Nwhile = 22; Nrepeat = 23; Nloop = 24; Nexit = 25;
		Nreturn = 26; Nwith = 27; Ntrap = 28; Ncomp = 29; Nraw = 30; Nwhiled = 11;
		Nwhilede = 12;

		(* node subclasses *)
		super = 1;

		(* module visibility of objects *)
		internal = 0; external = 1; externalR = 2; inPar = 3; outPar = 4;

		(* procedure flags (conval^.setval) *)
		hasBody = 1; isRedef = 2; slNeeded = 3;

		(* attribute flags (attr.adr, struct.attribute, proc.conval.setval)*)
		newAttr = 16; absAttr = 17; limAttr = 18; empAttr = 19; extAttr = 20;

		(* sysflags *)
		nilBit = 1; union = 7; (* must be odd *)

	TYPE
		Elem = POINTER TO RECORD
			next: Elem;
			struct: OPT.Struct;
			obj, base: OPT.Object;
			pos: INTEGER;
			name: OPS.String
		END;

		CaseTable = ARRAY OPM.MaxCases OF
			RECORD
				low, high: INTEGER
			END;

	VAR
		sym, level: BYTE;
		LoopLevel, ReturnLevel: SHORTINT;
		TDinit, lastTDinit: OPT.Node;
		userList, recList: Elem;

	PROCEDURE^ Type(VAR typ: OPT.Struct; VAR name: OPS.String);
	PROCEDURE^ Expression(VAR x: OPT.Node);
	PROCEDURE^ Block(VAR procdec, statseq: OPT.Node);

	(* forward type handling *)

	PROCEDURE IncompleteType (typ: OPT.Struct): BOOLEAN;
	BEGIN
		IF typ.form = Pointer THEN
			IF typ = OPT.sysptrtyp THEN RETURN FALSE END;
			typ := typ.BaseTyp
		END;
		RETURN (typ = OPT.undftyp) OR (typ.comp = Record) & (typ.BaseTyp = OPT.undftyp)
	END IncompleteType;

	PROCEDURE SetType (struct: OPT.Struct; obj: OPT.Object; typ: OPT.Struct; name: OPS.String);
		VAR u: Elem;
	BEGIN
		IF obj # NIL THEN obj.typ := typ ELSE struct.BaseTyp := typ END;
		IF name # NIL THEN
			NEW(u); u.struct := struct; u.obj := obj; u.pos := OPM.errpos; u.name := name;
			u.next := userList; userList := u
		END
	END SetType;

	PROCEDURE CheckAlloc (VAR typ: OPT.Struct; dynAllowed: BOOLEAN; pos: INTEGER);
	BEGIN
		typ.pvused := TRUE;
		IF typ.comp = DynArr THEN
			IF ~dynAllowed THEN OPM.Mark(88, pos); typ := OPT.undftyp END
		ELSIF typ.comp = Record THEN
			IF (typ.attribute = absAttr) OR (typ.attribute = limAttr) & (typ.mno # 0) THEN
				OPM.Mark(193, pos); typ := OPT.undftyp
			END
		END
	END CheckAlloc;

	PROCEDURE CheckRecursiveType (outer, inner: OPT.Struct; pos: INTEGER);
		VAR fld: OPT.Object;
	BEGIN
		IF outer = inner THEN OPM.Mark(58, pos)
		ELSIF inner.comp IN {Array, DynArr} THEN CheckRecursiveType(outer, inner.BaseTyp, pos)
		ELSIF inner.comp = Record THEN
			fld := inner.link;
			WHILE (fld # NIL) & (fld.mode = Fld) DO
				CheckRecursiveType(outer, fld.typ, pos);
				fld := fld.link
			END;
			IF inner.BaseTyp # NIL THEN CheckRecursiveType(outer, inner.BaseTyp, pos) END
		END
	END CheckRecursiveType;

	PROCEDURE FixType (struct: OPT.Struct; obj: OPT.Object; typ: OPT.Struct; pos: INTEGER);
	(* fix forward reference *)
		VAR t: OPT.Struct; f, bf: OPT.Object; i: SHORTINT;
	BEGIN
		IF obj # NIL THEN
			IF obj.mode = Var THEN	(* variable type *)
				IF struct # NIL THEN	(* receiver type *)
					IF (typ.form # Pointer) OR (typ.BaseTyp # struct) THEN OPM.Mark(180, pos) END;
				ELSE CheckAlloc(typ, obj.mnolev > level, pos)	(* TRUE for parameters *)
				END
			ELSIF obj.mode = VarPar THEN	(* varpar type *)
				IF struct # NIL THEN	(* varpar receiver type *)
					IF typ # struct THEN OPM.Mark(180, pos) END
				END
			ELSIF obj.mode = Fld THEN	(* field type *)
				CheckAlloc(typ, FALSE, pos);
				CheckRecursiveType(struct, typ, pos)
			ELSIF obj.mode = TProc THEN	(* proc return type *)
				IF typ.form = Comp THEN typ := OPT.undftyp; OPM.Mark(54, pos) END
			ELSIF obj.mode = Typ THEN	(* alias type *)
				IF typ.form IN {Byte..Set, Char16, LInt} THEN	(* make alias structure *)
					t := OPT.NewStr(typ.form, Basic); i := t.ref;
					t^ := typ^; t.ref := i; t.strobj := obj; t.mno := 0;
					t.BaseTyp := typ; typ := t
				END;
				IF obj.vis # internal THEN
					IF typ.comp = Record THEN typ.exp := TRUE
					ELSIF typ.form = Pointer THEN typ.BaseTyp.exp := TRUE
					END
				END
			ELSE HALT(100)
			END;
			obj.typ := typ
		ELSE
			IF struct.form = Pointer THEN	(* pointer base type *)
				IF (typ.comp # Record) & ~(typ.comp IN {Array, DynArr}) THEN
					typ := OPT.undftyp; OPM.Mark(57, pos)
				END;
				IF (struct.strobj # NIL) & (struct.strobj.vis # internal) THEN typ.exp := TRUE END;
			ELSIF struct.comp = Array THEN	(* array base type *)
				CheckAlloc(typ, FALSE, pos);
				CheckRecursiveType(struct, typ, pos)
			ELSIF struct.comp = DynArr THEN	(* array base type *)
				CheckAlloc(typ, TRUE, pos);
				CheckRecursiveType(struct, typ, pos)
			ELSIF struct.comp = Record THEN	(* record base type *)
				IF typ.form = Pointer THEN typ := typ.BaseTyp END;
				typ.pvused := TRUE; struct.extlev := SHORT(SHORT(typ.extlev + 1));
				IF (typ.attribute = 0) OR (typ.attribute = limAttr) & (typ.mno # 0) THEN OPM.Mark(181, pos)
				ELSIF (struct.attribute = absAttr) & (typ.attribute # absAttr) THEN OPM.Mark(191, pos)
				ELSIF (typ.attribute = limAttr) & (struct.attribute # limAttr) THEN OPM.Mark(197, pos)
				END;
				f := struct.link;
				WHILE f # NIL DO	(* check for field name conflicts *)
					OPT.FindField(f.name, typ, bf);
					IF bf # NIL THEN OPM.Mark(1, pos) END;
					f := f.link
				END;
				CheckRecursiveType(struct, typ, pos)
			ELSIF struct.form = ProcTyp THEN	(* proc type return type *)
				IF typ.form = Comp THEN typ := OPT.undftyp; OPM.Mark(54, pos) END
			ELSE HALT(100)
			END;
			struct.BaseTyp := typ
		END
	END FixType;

	PROCEDURE CheckForwardTypes;
		VAR u, next: Elem; progress: BOOLEAN; name: OPS.Name;
	BEGIN
		u := userList; userList := NIL;
		WHILE u # NIL DO
			next := u.next; name := u.name$; OPT.Find(name, u.base);
			IF u.base = NIL THEN OPM.Mark(0, u.pos)
			ELSIF u.base.mode # Typ THEN OPM.Mark(72, u.pos)
			ELSE u.next := userList; userList := u	(* reinsert *)
			END;
			u := next
		END;
		REPEAT	(* iteration for multy level alias *)
			u := userList; userList := NIL; progress := FALSE;
			WHILE u # NIL DO
				next := u.next;
				IF IncompleteType(u.base.typ) THEN
					u.next := userList; userList := u	(* reinsert *)
				ELSE
					progress := TRUE;
					FixType(u.struct, u.obj, u.base.typ, u.pos)
				END;
				u := next
			END
		UNTIL (userList = NIL) OR ~progress;
		u := userList;	(* remaining type relations are cyclic *)
		WHILE u # NIL DO
			IF (u.obj = NIL) OR (u.obj.mode = Typ) THEN OPM.Mark(58, u.pos) END;
			u := u.next
		END;
	END CheckForwardTypes;

	PROCEDURE CheckUnimpl (m: OPT.Object; typ: OPT.Struct; pos: INTEGER);
		VAR obj: OPT.Object;
	BEGIN
		IF m # NIL THEN
			IF (m.mode = TProc) & (absAttr IN m.conval.setval) THEN
				OPT.FindField(m.name^, typ, obj);
				IF (obj = NIL) OR (obj.mode # TProc) OR (absAttr IN obj.conval.setval) THEN
					OPM.Mark(192, pos);
					OPM.LogWLn; OPM.LogWStr("  ");
					IF typ.strobj # NIL THEN
						OPM.LogWStr(m.name); OPM.LogWStr(" not implemented in "); OPM.LogWStr(typ.strobj.name)
					ELSE
						OPM.LogWStr(m.name); OPM.LogWStr(" not implemented")
					END
				END
			END;
			CheckUnimpl(m.left, typ, pos);
			CheckUnimpl(m.right, typ, pos)
		END
	END CheckUnimpl;
	
	PROCEDURE CheckRecords (rec: Elem);
		VAR b: OPT.Struct;
	BEGIN
		WHILE rec # NIL DO	(* check for unimplemented methods in base type *)
			b := rec.struct.BaseTyp;
			WHILE (b # NIL) & (b # OPT.undftyp) DO
				CheckUnimpl(b.link, rec.struct, rec.pos);
				b := b.BaseTyp
			END;
			rec := rec.next
		END
	END CheckRecords;

	PROCEDURE Sign(n: LONGINT): INTEGER;
	BEGIN
		IF n = 0 THEN RETURN 0 END;
		IF n > 0 THEN RETURN 1 END;
		RETURN -1
	END Sign;

	PROCEDURE err(n: SHORTINT);
	BEGIN OPM.err(n)
	END err;

	PROCEDURE CheckSym(s: SHORTINT);
	BEGIN
		IF sym = s THEN OPS.Get(sym)
		ELSIF (OPM.Lang # "3") OR (s # semicolon) THEN OPM.err(s)
		END
	END CheckSym;

	PROCEDURE qualident(VAR id: OPT.Object);
		VAR obj: OPT.Object; lev: BYTE;
	BEGIN (*sym = ident*)
		OPT.Find(OPS.name, obj); OPS.Get(sym);
		IF (sym = period) & (obj # NIL) & (obj^.mode = Mod) THEN
			OPS.Get(sym);
			IF sym = ident THEN
				OPT.FindImport(obj, obj); OPS.Get(sym)
			ELSE err(ident); obj := NIL
			END
		END ;
		IF obj = NIL THEN err(0);
			obj := OPT.NewObj(); obj^.mode := Var; obj^.typ := OPT.undftyp; obj^.adr := 0
		ELSE lev := obj^.mnolev;
			IF (obj^.mode IN {Var, VarPar}) & (lev # level) THEN
				obj^.leaf := FALSE;
				IF lev > 0 THEN OPB.StaticLink(SHORT(SHORT(level-lev))) END
			END
		END;
		id := obj
	END qualident;

	PROCEDURE ConstExpression(VAR x: OPT.Node);
	BEGIN Expression(x);
		IF x^.class # Nconst THEN
			err(50); x := OPB.NewIntConst(1)
		END
	END ConstExpression;

	PROCEDURE CheckMark(obj: OPT.Object);	(* !!! *)
	BEGIN OPS.Get(sym);
		IF (sym = times) OR (sym = minus) THEN
			IF (sym = minus) & OPT.SYSimported & (obj^.mode = CProc) THEN obj^.sysflag := -1
			ELSIF ((OPM.Lang = "1") OR (OPM.Lang = "7")) & (sym = minus) THEN err(48)
			ELSIF (level > 0) OR ~(obj^.mode IN {Var, Fld}) & (sym = minus) THEN err(48)
			END;
			IF (sym = times) & ~((OPM.Lang = "7") & (obj^.mode = Var)) THEN obj^.vis := external
			ELSE obj^.vis := externalR
			END;
			OPS.Get(sym)
		ELSIF OPM.foreign IN OPM.opt THEN obj^.vis := external
		ELSE obj^.vis := internal
		END;
		IF obj^.mode IN {LProc, XProc, Var} THEN
			IF sym = lbrak THEN OPS.Get(sym);
				IF obj^.mnolev # 0 THEN err(48)
				ELSIF ~(OPT.SYSimported OR (OPM.foreign IN OPM.opt)) THEN err(135)
				END;
				IF (sym = number) & (OPS.numtyp = char) THEN
					NEW(obj^.entry, 2); obj^.entry[0] := SHORT(CHR(OPS.intval)); obj^.entry[1] := 0X
				ELSIF sym = string THEN
					IF OPS.str^ # "" THEN obj^.entry := OPS.str END;
					OPS.Get(sym)
				ELSE err(string)
				END;
				CheckSym(rbrak)
			ELSIF OPM.foreign IN OPM.opt THEN obj^.entry := obj^.name
			END
		END
	END CheckMark;

	PROCEDURE CheckSysFlag (VAR sysflag: SHORTINT; default: SHORTINT; record: BOOLEAN);
		VAR x: OPT.Node; sf: LONGINT;
	BEGIN
		IF sym = lbrak THEN OPS.Get(sym);
			IF ~(OPT.SYSimported OR (OPM.foreign IN OPM.opt)) THEN err(135) END;
			IF (sym = ident) & ((OPS.name = "notag") OR (OPS.name = "untagged")) THEN
				OPS.Get(sym); sysflag := 1
			ELSIF record & (sym = ident) & (OPS.name = "union") THEN
				OPS.Get(sym); sysflag := union
			ELSE
				ConstExpression(x);
				IF x^.typ^.form IN intSet THEN sf := x^.conval^.intval;
					IF (sf < 0) OR (sf > OPM.MaxSysFlag) THEN err(178); sf := 0 END
				ELSE err(51); sf := 0
				END;
				sysflag := SHORT(SHORT(sf))
			END;
			CheckSym(rbrak)
		ELSE sysflag := default
		END
	END CheckSysFlag;

	PROCEDURE CheckSysFlagVarPar (VAR sysflag: SHORTINT);
	BEGIN
		IF sym = lbrak THEN OPS.Get(sym);
			IF ~(OPT.SYSimported OR (OPM.foreign IN OPM.opt)) THEN err(135) END;
			IF (sym = ident) & (OPS.name = "nil") THEN OPS.Get(sym); sysflag := nilBit
			ELSE OPS.Get(sym); err(178); sysflag := 0
			END;
			CheckSym(rbrak)
		ELSE sysflag := 0
		END
	END CheckSysFlagVarPar;

	PROCEDURE CheckSysFlagProc (VAR sysflag: SHORTINT);
	BEGIN
		IF sym = lbrak THEN OPS.Get(sym);
			IF ~(OPT.SYSimported OR (OPM.foreign IN OPM.opt)) THEN err(135) END;
			IF (sym = ident) & (OPS.name = "ccall") THEN OPS.Get(sym); sysflag := 0
			ELSIF (sym = ident) & ((OPS.name = "stdcall") OR (OPS.name = "callback")) THEN OPS.Get(sym); sysflag := 1
			ELSIF (sym = ident) & (OPS.name = "fastcall") THEN OPS.Get(sym); sysflag := 2
			ELSE OPS.Get(sym); err(178); sysflag := 0
			END;
			CheckSym(rbrak)
		ELSE sysflag := 0
		END
	END CheckSysFlagProc;

	PROCEDURE CheckOverwrite (proc, base: OPT.Object; rec: OPT.Struct);
		VAR o, bo: OPT.Object;
	BEGIN
		IF base # NIL THEN
			IF base.conval.setval * {absAttr, empAttr, extAttr} = {} THEN err(182) END;
			IF (proc.link.mode # base.link.mode) OR (proc.link.vis # base.link.vis)
				OR ~OPT.Extends(proc.link.typ, base.link.typ) THEN err(115) END;
			o := proc.link; bo := base.link;
			WHILE (o # NIL) & (bo # NIL) DO
				IF (bo.sysflag # 0) & (o.sysflag = 0) THEN	(* propagate sysflags *)
					o.sysflag := bo.sysflag
				END;
				o := o.link; bo := bo.link
			END;
			OPB.CheckParameters(proc.link.link, base.link.link, FALSE);
			IF ~OPT.Extends(proc.typ, base.typ) THEN err(117) END;
			IF (base.vis # proc.vis) & ((proc.vis # internal) OR rec.exp) THEN err(183) END;
			INCL(proc.conval.setval, isRedef)
		END
	END CheckOverwrite;

	PROCEDURE GetAttributes (proc, base: OPT.Object; owner: OPT.Struct);	(* read method attributes *)
		VAR attr, battr: SET; o: OPT.Object;
	BEGIN
		attr := {};
		IF sym = comma THEN	(* read attributes *)
			OPS.Get(sym);
			IF sym = ident THEN
				OPT.Find(OPS.name, o);
				IF (o # NIL) & (o.mode = SProc) & (o.adr = newfn) THEN
					IF OPM.Lang = "C" THEN INCL(attr, newAttr) ELSE err(178) END;
					OPS.Get(sym);
					IF sym = comma THEN
						OPS.Get(sym);
						IF sym = ident THEN OPT.Find(OPS.name, o) ELSE o := NIL; err(ident) END
					ELSE o := NIL
					END
				END;
				IF o # NIL THEN
					IF (o.mode # Attr) OR (o.adr = limAttr) OR (OPM.Lang # "C") THEN err(178)
					ELSE INCL(attr, o.adr)
					END;
					OPS.Get(sym)
				END
			ELSE err(ident)
			END
		END;
		IF (base = NIL) & ~(newAttr IN attr) THEN INCL(attr, newAttr); IF OPM.Lang = "C" THEN err(185) END
		ELSIF (base # NIL) & (newAttr IN attr) THEN err(186)
		END;
		IF (OPM.Lang = "2") OR (OPM.Lang = "3") THEN INCL(attr, extAttr) END;
		IF absAttr IN attr THEN
			IF owner.attribute # absAttr THEN err(190) END;
			IF (proc.vis = internal) & owner.exp THEN err(179) END
		END;
		IF (owner.attribute = 0) OR (owner.attribute = limAttr) THEN
			IF (empAttr IN attr) & (newAttr IN attr) THEN err(187)
(*
			ELSIF extAttr IN attr THEN err(188)
*)
			END
		END;
		IF base # NIL THEN
			battr := base.conval.setval;
			IF empAttr IN battr THEN
				IF absAttr IN attr THEN err(189) END
			ELSIF ~(absAttr IN battr) THEN
				IF (absAttr IN attr) OR (empAttr IN attr) THEN err(189) END
			END
		END;
		IF empAttr IN attr THEN
			IF proc.typ # OPT.notyp THEN err(195)
			ELSE
				o := proc.link; WHILE (o # NIL) & (o.vis # outPar) DO o := o.link END;
				IF o # NIL THEN err(195) END
			END
		END;
		proc.conval.setval := attr
	END GetAttributes;

	PROCEDURE RecordType (VAR typ: OPT.Struct; attr: OPT.Object);
		VAR fld, first, last: OPT.Object; r: Elem; ftyp: OPT.Struct; name: OPS.String;
	BEGIN typ := OPT.NewStr(Comp, Record); typ^.BaseTyp := NIL;
		IF OPM.Lang # "C" THEN typ^.attribute := extAttr END;
		CheckSysFlag(typ^.sysflag, 0, TRUE);
		IF attr # NIL THEN
			IF (OPM.Lang = "C") & (attr.adr # empAttr) THEN typ.attribute := SHORT(SHORT(attr.adr))
			ELSE err(178)
			END
		END;
		IF sym = lparen THEN
			OPS.Get(sym); (*record extension*)
			IF sym = ident THEN
				Type(ftyp, name);
				IF ftyp.form = Pointer THEN ftyp := ftyp.BaseTyp END;
				SetType(typ, NIL, ftyp, name);
				IF (ftyp.comp = Record) (* & (ftyp # OPT.anytyp) *) THEN
					ftyp.pvused := TRUE; typ^.extlev := SHORT(SHORT(ftyp.extlev + 1));
					IF (ftyp.attribute = 0) OR (ftyp.attribute = limAttr) & (ftyp.mno # 0) THEN err(181)
					ELSIF (typ.attribute = absAttr) & (ftyp.attribute # absAttr) THEN err(191)
					ELSIF (ftyp.attribute = limAttr) & (typ.attribute # limAttr) THEN err(197)
					END
				ELSIF ftyp # OPT.undftyp THEN err(53)
				END
			ELSE err(ident)
			END;
			IF typ.attribute # absAttr THEN	(* save typ for unimplemented method check *)
				NEW(r); r.struct := typ; r.pos := OPM.errpos; r.next := recList; recList := r
			END;
			CheckSym(rparen)
		END;
		OPT.OpenScope(0, NIL);
		first := NIL; last := NIL;
		LOOP
			IF sym = ident THEN
				LOOP
					IF sym = ident THEN
						IF (typ^.BaseTyp # NIL) & (typ.BaseTyp # OPT.undftyp) THEN
							OPT.FindField(OPS.name, typ^.BaseTyp, fld);
							IF fld # NIL THEN err(1) END
						END;
						OPT.Insert(OPS.name, fld);
						fld^.mode := Fld; fld^.link := NIL; fld^.typ := OPT.undftyp;
						CheckMark(fld);
						IF first = NIL THEN first := fld END;
						IF last = NIL THEN typ^.link := fld ELSE last^.link := fld END;
						last := fld
					ELSE err(ident)
					END;
					IF sym = comma THEN OPS.Get(sym)
					ELSIF sym = ident THEN err(comma)
					ELSE EXIT
					END
				END;
				CheckSym(colon); Type(ftyp, name);
				CheckAlloc(ftyp, FALSE, OPM.errpos);
				WHILE first # NIL DO
					SetType(typ, first, ftyp, name); first := first^.link
				END
			END;
			IF sym = semicolon THEN OPS.Get(sym)
			ELSIF sym = ident THEN err(semicolon)
			ELSE EXIT
			END
		END;
		OPT.CloseScope;
		OPB.Inittd(TDinit, lastTDinit, typ); CheckSym(end)
	END RecordType;

	PROCEDURE ArrayType(VAR typ: OPT.Struct);
		VAR x: OPT.Node; n: LONGINT; sysflag: SHORTINT; name: OPS.String;
	BEGIN CheckSysFlag(sysflag, 0, FALSE);
		IF sym = of THEN	(*dynamic array*)
			typ := OPT.NewStr(Comp, DynArr); typ^.mno := 0; typ^.sysflag := sysflag;
			OPS.Get(sym); Type(typ^.BaseTyp, name); SetType(typ, NIL, typ^.BaseTyp, name);
			CheckAlloc(typ^.BaseTyp, TRUE, OPM.errpos);
			IF typ^.BaseTyp^.comp = DynArr THEN typ^.n := typ^.BaseTyp^.n + 1
			ELSE typ^.n := 0
			END
		ELSE
			typ := OPT.NewStr(Comp, Array); typ^.sysflag := sysflag; ConstExpression(x);
			IF x^.typ^.form IN intSet THEN n := x^.conval^.intval;
				IF (n <= 0) OR (n > OPM.MaxIndex) THEN err(63); n := 1 END
			ELSE err(51); n := 1
			END;
			typ^.n := SHORT(n);
			IF sym = of THEN
				OPS.Get(sym); Type(typ^.BaseTyp, name); SetType(typ, NIL, typ^.BaseTyp, name);
				CheckAlloc(typ^.BaseTyp, FALSE, OPM.errpos)
			ELSIF sym = comma THEN
				OPS.Get(sym); IF sym # of THEN ArrayType(typ^.BaseTyp) END
			ELSE err(35)
			END;
			IF typ^.BaseTyp^.comp = DynArr THEN typ^.BaseTyp := OPT.undftyp; err(88) END
		END
	END ArrayType;

	PROCEDURE PointerType(VAR typ: OPT.Struct);
		VAR name: OPS.String;
	BEGIN typ := OPT.NewStr(Pointer, Basic); CheckSysFlag(typ^.sysflag, 0, FALSE);
		CheckSym(to);
		Type(typ^.BaseTyp, name); SetType(typ, NIL, typ^.BaseTyp, name);
		IF (typ^.BaseTyp # OPT.undftyp) & (typ^.BaseTyp^.comp = Basic) THEN
			typ^.BaseTyp := OPT.undftyp; err(57)
		END
	END PointerType;

	PROCEDURE FormalParameters(VAR firstPar: OPT.Object; VAR resTyp: OPT.Struct; VAR name: OPS.String);
		VAR mode, vis: BYTE; sys: SHORTINT; par, first, last: OPT.Object; typ: OPT.Struct; errpos: INTEGER;
	BEGIN first := NIL; last := firstPar;
		IF (sym = ident) OR (sym = var) OR (OPM.Lang = "C") & ((sym = in) OR (sym = out)) THEN
			LOOP
				sys := 0; vis := 0;
				IF sym = var THEN OPS.Get(sym); mode := VarPar
				ELSIF (sym = in) & (OPM.Lang = "C") THEN OPS.Get(sym); mode := VarPar; vis := inPar
				ELSIF (sym = out) & (OPM.Lang = "C") THEN OPS.Get(sym); mode := VarPar; vis := outPar
				ELSE mode := Var
				END;
				IF mode = VarPar THEN CheckSysFlagVarPar(sys) END;
				errpos := -1;	(* position of 1st "-" parameter *)
				LOOP
					IF sym = ident THEN
						OPT.Insert(OPS.name, par); OPS.Get(sym);
						IF (sym = minus) & (OPM.Lang <= "2") & (mode # VarPar) THEN
							OPS.Get(sym); par^.mode := VarPar; par^.vis := inPar;
							IF ~(OPM.oakwood IN OPM.opt) THEN err(-304) END;
							IF errpos = -1 THEN errpos := OPM.errpos END
						ELSE
							par^.mode := mode; par^.vis := vis
						END;
						par^.link := NIL; par^.sysflag := SHORT(sys);
						IF first = NIL THEN first := par END ;
						IF firstPar = NIL THEN firstPar := par ELSE last^.link := par END ;
						last := par
					ELSE err(ident)
					END;
					IF sym = comma THEN OPS.Get(sym)
					ELSIF sym = ident THEN err(comma)
					ELSIF sym = var THEN err(comma); OPS.Get(sym)
					ELSE EXIT
					END
				END;
				CheckSym(colon); Type(typ, name);
				IF mode # VarPar THEN CheckAlloc(typ, TRUE, OPM.errpos) END;
				IF (errpos # -1) & (typ^.form # Undef) & (typ^.form # Comp) & (typ^.sysflag = 0) THEN
					OPM.Mark(177, errpos)
				(* IN only allowed for records and arrays *)
				ELSIF (mode = VarPar) & (vis = inPar) & (typ^.form # Undef) & (typ^.form # Comp) & (typ^.sysflag = 0) THEN
					err(177)
				ELSIF ODD(sys) & ((typ.comp = Record) OR (typ.comp = DynArr)) & ~ODD(typ.sysflag) THEN err(142)
				(* untagged open array not allowed as value parameter *)
				ELSIF (mode = Var) & (typ^.comp = DynArr) & ODD(typ^.sysflag) THEN OPM.err(145)
				END;
				IF (mode = Var) & (typ^.form = Comp)
					& ((OPM.Lang = "3") OR (OPM.Lang = "7")) & ~(OPM.foreign IN OPM.opt)
				THEN
					par := first;
					WHILE par # NIL DO
						par^.mode := VarPar; par^.vis := inPar;	(* r/o array or record *)
						par := par^.link
					END
				END;
				(* typ^.pbused is set when parameter type name is parsed *)
				WHILE first # NIL DO SetType(NIL, first, typ, name); first := first^.link END;
				IF sym = semicolon THEN OPS.Get(sym)
				ELSIF sym = ident THEN err(semicolon)
				ELSE EXIT
				END
			END
		END;
		CheckSym(rparen);
		name := NIL;
		IF sym = colon THEN
			OPS.Get(sym);
			Type(resTyp, name);
			IF resTyp^.form = Comp THEN resTyp := OPT.undftyp; err(54) END
		ELSE resTyp := OPT.notyp
		END
	END FormalParameters;

	PROCEDURE Type (VAR typ: OPT.Struct; VAR name: OPS.String);	(* name # NIL => forward reference *)
		VAR id: OPT.Object; tname: OPS.String;
	BEGIN typ := OPT.undftyp; name := NIL;
		IF sym < lparen THEN err(12);
			REPEAT OPS.Get(sym) UNTIL sym >= lparen
		END ;
		IF sym = ident THEN
			OPT.Find(OPS.name, id);
			IF (id = NIL) OR (id.mode = -1) OR (id.mode = Typ) & IncompleteType(id.typ) THEN	(* forward type definition *)
				name := OPT.NewName(OPS.name); OPS.Get(sym);
				IF (id = NIL) & (sym = period) THEN	(* missing module *)
					err(0); OPS.Get(sym); name := NIL;
					IF sym = ident THEN OPS.Get(sym) END
				ELSIF sym = record THEN	(* wrong attribute *)
					err(178); OPS.Get(sym); name := NIL; RecordType(typ, NIL)
				END
			ELSE
				qualident(id);
				IF id^.mode = Typ THEN
					typ := id^.typ
				ELSIF id^.mode = Attr THEN
					IF sym = record THEN
						OPS.Get(sym); RecordType(typ, id)
					ELSE err(12)
					END
				ELSE err(52)
				END
			END
		ELSIF sym = array THEN
			OPS.Get(sym); ArrayType(typ)
		ELSIF sym = record THEN
			OPS.Get(sym); RecordType(typ, NIL)
		ELSIF sym = pointer THEN
			OPS.Get(sym); PointerType(typ)
		ELSIF sym = procedure THEN
			OPS.Get(sym); typ := OPT.NewStr(ProcTyp, Basic); CheckSysFlagProc(typ^.sysflag);
			IF sym = lparen THEN
				OPS.Get(sym); OPT.OpenScope(level, NIL);
				FormalParameters(typ^.link, typ^.BaseTyp, tname); SetType(typ, NIL, typ^.BaseTyp, tname); OPT.CloseScope
			ELSE typ^.BaseTyp := OPT.notyp; typ^.link := NIL
			END
		ELSE err(12)
		END;
		LOOP
			IF (sym >= semicolon) & (sym <= procedure) OR (sym = rparen) OR (sym = eof)
				OR (sym = number) OR (sym = comma) OR (sym = string) THEN EXIT END;
			err(15); IF sym = ident THEN EXIT END;
			OPS.Get(sym)
		END
	END Type;

	PROCEDURE ActualParameters(VAR aparlist: OPT.Node; fpar: OPT.Object; VAR pre, lastp: OPT.Node);
		VAR apar, last, n: OPT.Node;
	BEGIN aparlist := NIL; last := NIL;
		IF sym # rparen THEN
			LOOP
				IF (OPM.Lang = "3") & (fpar # NIL) THEN
					IF (fpar^.mode = VarPar) & (fpar^.vis # inPar) THEN
						IF sym = at THEN OPS.Get(sym) ELSE err(42) END
					ELSIF sym = at THEN OPS.Get(sym); err(39)
					END
				END;
				Expression(apar);
				IF fpar # NIL THEN
					IF (OPM.Lang = "C") & (apar.typ.form = Pointer) & (fpar.typ.form = Comp) THEN OPB.DeRef(apar) END;
					OPB.Param(apar, fpar);
					IF (fpar.mode = Var) OR (fpar.vis = inPar) THEN OPB.CheckBuffering(apar, NIL, fpar, pre, lastp) END;
					OPB.Link(aparlist, last, apar);
					IF AnchorVarPar & (fpar.mode = VarPar)	(* source output: avoid double evaluation *)
							 & ((fpar.mode = VarPar) & (fpar.typ.comp = Record) & (fpar.typ.sysflag = 0)
								OR (fpar.typ.comp = DynArr) & (fpar.typ.sysflag = 0)) THEN
						n := apar;
						WHILE n.class IN {Nfield, Nindex, Nguard} DO n := n.left END;
						IF (n.class = Nderef) & (n.subcl = 0) THEN
							IF n.left.class = Nguard THEN n := n.left END;
							OPB.CheckVarParBuffering(n.left, pre, lastp)
						END
					END;
					fpar := fpar^.link;
				ELSE err(64)
				END;
				IF sym = comma THEN OPS.Get(sym)
				ELSIF (lparen <= sym) & (sym <= ident) THEN err(comma)
				ELSE EXIT
				END
			END
		END;
		IF fpar # NIL THEN err(65) END
	END ActualParameters;

	PROCEDURE selector(VAR x: OPT.Node);
		VAR obj, proc, p, fpar: OPT.Object; y, apar, pre, lastp: OPT.Node; typ: OPT.Struct; name: OPS.Name;
	BEGIN
		LOOP
			IF sym = lbrak THEN OPS.Get(sym);
				LOOP
					IF (x^.typ # NIL) & (x^.typ^.form = Pointer) THEN OPB.DeRef(x) END ;
					Expression(y); OPB.Index(x, y);
					IF sym = comma THEN OPS.Get(sym) ELSE EXIT END
				END ;
				CheckSym(rbrak)
			ELSIF sym = period THEN OPS.Get(sym);
				IF sym = ident THEN name := OPS.name; OPS.Get(sym);
					IF x^.typ # NIL THEN
						IF x^.typ^.form = Pointer THEN OPB.DeRef(x) END;
						IF x^.typ^.comp = Record THEN
							typ := x.typ; OPT.FindField(name, x^.typ, obj); OPB.Field(x, obj);
							IF (obj # NIL) & (obj^.mode = TProc) THEN
								IF sym = arrow THEN  (* super call *) OPS.Get(sym);
									y := x^.left;
									IF y^.class = Nderef THEN y := y^.left END;	(* y = record variable *)
									IF y^.obj # NIL THEN
										proc := OPT.topScope;	(* find innermost scope which owner is a TProc *)
										WHILE (proc^.link # NIL) & (proc^.link^.mode # TProc) DO proc := proc^.left END ;
										IF (proc^.link = NIL) OR (proc^.link^.link # y^.obj) THEN err(75) END ;
										typ := y^.obj^.typ;
										IF typ^.form = Pointer THEN typ := typ^.BaseTyp END ;
										OPT.FindField(x^.obj^.name^, typ^.BaseTyp, p);
										IF p # NIL THEN
											x^.subcl := super; x.typ := p.typ;	(* correct result type *)
											IF p.conval.setval * {absAttr, empAttr} # {} THEN err(194) END;
											IF (p.vis = externalR) & (p.mnolev < 0) & (proc.link.name^ # name) THEN err(196) END
										ELSE err(74)
										END
									ELSE err(75)
									END
								ELSE
									proc := obj;
									IF (x.left.readonly) & (proc.link.mode = VarPar) & (proc.link.vis = 0) THEN err(76) END;
									WHILE (proc.mnolev >= 0) & ~(newAttr IN proc.conval.setval) & (typ.BaseTyp # NIL) DO
										(* find base method *)
										typ := typ.BaseTyp; OPT.FindField(name, typ, proc);
									END;
									IF (proc.vis = externalR) & (proc.mnolev < 0) THEN err(196) END;
								END;
								IF (obj^.typ # OPT.notyp) & (sym # lparen) THEN err(lparen) END
							END
						ELSE err(53)
						END
					ELSE err(52)
					END
				ELSE err(ident)
				END
			ELSIF sym = arrow THEN OPS.Get(sym); OPB.DeRef(x)
			ELSIF (sym = dollar) & ((OPM.Lang = "C") OR (OPM.Lang = "3")) THEN
				IF (OPM.Lang = "C") & (x^.typ^.form = Pointer) THEN OPB.DeRef(x) END;
				OPS.Get(sym); OPB.StrDeref(x)
			ELSIF sym = lparen THEN
				IF (x.obj # NIL) & (x.obj.mode IN {XProc, LProc, CProc, TProc}) THEN typ := x.obj.typ
				ELSIF x.typ.form = ProcTyp THEN typ := x.typ.BaseTyp
				ELSIF x.class IN {Ntype, Nproc} THEN EXIT	(* standard procedure or constant array *)
				ELSE typ := NIL
				END;
				IF typ # OPT.notyp THEN
					OPS.Get(sym);
					IF typ = NIL THEN	(* type guard *)
						IF OPM.Lang = "3" THEN CheckSym(colon) END;
						IF sym = ident THEN
							qualident(obj);
							IF obj.mode = Typ THEN OPB.TypTest(x, obj, TRUE)
							ELSE err(52)
							END
						ELSE err(ident)
						END;
						CheckSym(rparen)
					ELSE	(* function call *)
						pre := NIL; lastp := NIL;
						OPB.PrepCall(x, fpar);
						IF (x.obj # NIL) & (x.obj.mode = TProc) THEN OPB.CheckBuffering(x.left, NIL, x.obj.link, pre, lastp)
						END;
						ActualParameters(apar, fpar, pre, lastp);
						OPB.Call(x, apar, fpar);
						IF pre # NIL THEN OPB.Construct(Ncomp, pre, x); pre.typ := x.typ; x := pre END;
						IF level > 0 THEN OPT.topScope.link.leaf := FALSE END;
						CheckSym(rparen);
						IF (OPM.Lang # "3") & (OPM.Lang # "C") & ((sym = arrow) OR (sym = period)) THEN err(126) END
					END
				ELSE EXIT
				END
			ELSE EXIT
			END
		END
	END selector;

	PROCEDURE StandProcCall(VAR x: OPT.Node);
		VAR y: OPT.Node; m: BYTE; n: SHORTINT;
	BEGIN m := SHORT(SHORT(x^.obj^.adr)); n := 0;
		IF sym = lparen THEN OPS.Get(sym);
			IF sym # rparen THEN
				LOOP
					IF n = 0 THEN Expression(x); OPB.StPar0(x, m); n := 1
					ELSIF n = 1 THEN Expression(y); OPB.StPar1(x, y, m); n := 2
					ELSE Expression(y); OPB.StParN(x, y, m, n); INC(n)
					END ;
					IF sym = comma THEN OPS.Get(sym)
					ELSIF (lparen <= sym) & (sym <= ident) THEN err(comma)
					ELSE EXIT
					END
				END ;
				CheckSym(rparen)
			ELSE OPS.Get(sym)
			END ;
			OPB.StFct(x, m, n)
		ELSE err(lparen)
		END ;
		IF (level > 0) & ((m = newfn) OR (m = sysnewfn)) THEN OPT.topScope^.link^.leaf := FALSE END
	END StandProcCall;

	PROCEDURE Element(VAR x: OPT.Node);
		VAR y: OPT.Node;
	BEGIN Expression(x);
		IF sym = upto THEN
			OPS.Get(sym); Expression(y); OPB.SetRange(x, y)
		ELSE OPB.SetElem(x)
		END
	END Element;

	PROCEDURE Sets(VAR x: OPT.Node);
		VAR y: OPT.Node;
	BEGIN
		IF sym # rbrace THEN
			Element(x);
			LOOP
				IF sym = comma THEN OPS.Get(sym)
				ELSIF (lparen <= sym) & (sym <= ident) THEN err(comma)
				ELSE EXIT
				END ;
				Element(y); OPB.Op(plus, x, y)
			END
		ELSE x := OPB.EmptySet()
		END ;
		CheckSym(rbrace)
	END Sets;

	PROCEDURE String2Chars (a: OPT.Node; VAR x: OPT.Node);	(* переводит строку в массив символов *)
		VAR i: INTEGER;
	BEGIN
		FOR i := 0 TO a^.conval^.intval2 - 1 DO
			IF x^.conval^.intval+i < LEN(x^.conval^.arr.val1) THEN
				x^.conval^.arr.val1[x^.conval^.intval+i] := SHORT(ORD(a^.conval^.ext^[i]))
			ELSE
				err(64)
			END
		END
	END String2Chars;

	PROCEDURE ConstArray (VAR x: OPT.Node; typ: OPT.Struct);
	(* конструкция "константный массив". typ - текущий уровень массива *)
		VAR apar: OPT.Node; n, i: INTEGER; fp: OPT.Object; y: OPT.ConstArr;
	BEGIN
		IF (sym # lparen) & (typ^.BaseTyp^.form IN charSet) THEN (* это должна быть строка *)
			ConstExpression(apar); (* попытаемся взять эту строку *)
			IF (apar^.typ^.comp = Basic) & (apar^.typ^.form IN {String8, String16}) &
				(apar^.conval^.intval2 <= typ^.n) THEN (* взяли строку допустимой длины *)
				String2Chars(apar, x);
				INC(x^.conval^.intval, typ^.n);
			ELSIF (apar^.typ^.comp = Basic) & (apar^.typ^.form IN charSet) THEN (* прочитался символ *)
				IF x^.conval^.intval < LEN(x^.conval^.arr.val1) THEN
					x^.conval^.arr.val1[x^.conval^.intval] := SHORT(SHORT(SHORT(apar^.conval^.intval)));
					INC(x^.conval^.intval, typ^.n)
				ELSE
					err(64)
				END
			ELSE
				err(63)
			END
		ELSE
		CheckSym(lparen);
		n := typ^.n; (* количество элементов массива *)
		i := 0;
		IF typ^.BaseTyp^.form IN intSet + charSet + {Bool} THEN (* массив из целых (в т.ч. BYTE)  *)
			y := x^.conval^.arr;
			fp := OPT.NewObj(); fp^.typ := typ^.BaseTyp;
			fp^.mode := Var;  (* fp - переменная, элемент массива *)
			IF sym # rparen THEN
				LOOP ConstExpression(apar); (* берем очередной элемент *)
					IF i < n THEN
						IF (i = 0) & (typ^.BaseTyp^.form IN charSet + {Byte})  (* считывали первый символ *)
							& (apar^.typ^.comp = Basic) & (apar^.typ^.form IN {String8, String16})  (* а прочитали строку *)
							& (apar^.conval^.intval2 <= typ^.n)  (* допустимой длины *) THEN
							String2Chars(apar, x);
						ELSE
							OPB.Param(apar, fp); (* проверим на совместимость*)
							(* тут нужно положить элемент в кусок памяти *)
							CASE fp^.typ^.size OF
							| 1: (* BOOLEAN, CHAR, BYTE (для Ofront'а) *)
									IF x^.conval^.intval+i < LEN(y.val1^) THEN
										y.val1[x^.conval^.intval+i] := SHORT(SHORT(SHORT(apar^.conval^.intval)))
									ELSE err(64) END
							| 2: (* типы с размером в 2 байта *)
									IF x^.conval^.intval+i < LEN(y.val2^) THEN
										y.val2[x^.conval^.intval+i] := SHORT(SHORT(apar^.conval^.intval))
									ELSE err(64) END
							| 4: (* типы с размером в 4 байта *)
									IF x^.conval^.intval+i < LEN(y.val4^) THEN
										y.val4[x^.conval^.intval+i] := SHORT(apar^.conval^.intval)
									ELSE err(64) END
							| 8: (* типы с размером в 8 байт *)
									IF x^.conval^.intval+i < LEN(y.val8^) THEN
										y.val8[x^.conval^.intval+i] := apar^.conval^.intval
									ELSE err(64) END
							END
						END;
						INC(i)
					ELSE err(64)
					END;
					IF sym = comma THEN OPS.Get(sym)
					ELSIF (lparen <= sym) & (sym <= ident) THEN err(comma)
					ELSE EXIT
					END
				END
			END;
			IF i # n THEN
				IF (i = 1) & (typ^.BaseTyp^.form IN charSet + {Byte}) THEN
				(* один символ (мы его уже считали) может означать целую строку *)
				ELSE
			 		err(65)
				END
			END;
			INC(x^.conval^.intval, n); (* учли прочитанные элементы, даже если они не прочитаны *)
		ELSIF (typ^.BaseTyp^.form = Comp) & (typ^.BaseTyp^.comp = Array) THEN (* массив из массивов *)
			IF sym # rparen THEN
				LOOP
					ConstArray (x, typ^.BaseTyp);  (* рекурсивная обработка подмассива *)
					IF i < n THEN
						(*	??? проверим на совместимость *)
						INC(i)
					ELSE err(64); EXIT
					END;
					IF sym = comma THEN OPS.Get(sym)
					ELSIF (lparen <= sym) & (sym <= ident) THEN err(comma); EXIT
					ELSE EXIT
					END
				END
			END;
			IF i # n THEN err(65) END
		ELSE err(51)
		END;
		CheckSym(rparen); (* скобка ) *)
		END
	END ConstArray;

PROCEDURE Factor(VAR x: OPT.Node);
		VAR id: OPT.Object;
	BEGIN
		IF sym < lparen THEN err(13);
			REPEAT OPS.Get(sym) UNTIL sym >= lparen
		END;
		IF sym = ident THEN
			qualident(id); x := OPB.NewLeaf(id); selector(x);
			IF (x^.class = Nproc) & (x^.obj^.mode = SProc) THEN StandProcCall(x)	(* x may be NIL *)
			ELSIF (sym = lparen) & (OPM.Lang = "3") & (x^.obj^.mode = Typ) & (x^.obj^.typ^.form = Comp)
			& (x^.obj^.typ^.comp = Array) THEN
				OPB.NewArrConst(x); ConstArray(x, x^.obj^.typ); (* константный массив верхнего уровня *)
				IF sym # semicolon THEN err(semicolon) END (* на всякий случай проверим, что в конце ";" *)
			END
		ELSIF sym = number THEN
			CASE OPS.numtyp OF
			   char:
					x := OPB.NewIntConst(OPS.intval); x^.typ := OPT.char8typ;
					IF OPS.intval > 255 THEN x.typ := OPT.char16typ END
			| integer: x := OPB.NewIntConst(OPS.intval)
			| real: x := OPB.NewRealConst(OPS.realval, OPT.realtyp)
			| longreal: x := OPB.NewRealConst(OPS.lrlval, OPT.lrltyp)
			END;
			OPS.Get(sym)
		ELSIF sym = string THEN
			x := OPB.NewString(OPS.str, OPS.lstr, SHORT(OPS.intval)); OPS.Get(sym)
		ELSIF sym = nil THEN
			x := OPB.Nil(); OPS.Get(sym)
		ELSIF sym = lparen THEN
			OPS.Get(sym); Expression(x); CheckSym(rparen)
		ELSIF sym = lbrak THEN
			OPS.Get(sym); err(lparen); Expression(x); CheckSym(rparen)
		ELSIF sym = lbrace THEN OPS.Get(sym); Sets(x)
		ELSIF sym = not THEN
			OPS.Get(sym); Factor(x); OPB.MOp(not, x)
		ELSE err(13); OPS.Get(sym); x := NIL
		END;
		IF x = NIL THEN x := OPB.NewIntConst(1); x^.typ := OPT.undftyp END
	END Factor;

	PROCEDURE Term(VAR x: OPT.Node);
		VAR y: OPT.Node; mulop: BYTE;
	BEGIN Factor(x);
		WHILE (times <= sym) & (sym <= and) DO
			mulop := sym; OPS.Get(sym);
			Factor(y); OPB.Op(mulop, x, y)
		END
	END Term;

	PROCEDURE SimpleExpression(VAR x: OPT.Node);
		VAR y: OPT.Node; addop: BYTE;
	BEGIN
		IF sym = minus THEN OPS.Get(sym); Term(x); OPB.MOp(minus, x)
		ELSIF sym = plus THEN OPS.Get(sym); Term(x); OPB.MOp(plus, x)
		ELSE Term(x)
		END;
		WHILE (plus <= sym) & (sym <= or) DO
			addop := sym; OPS.Get(sym); Term(y);
			IF (OPM.Lang = "C") & (x.typ.form = Pointer) THEN OPB.DeRef(x) END;
			IF (x.typ.comp IN {Array, DynArr}) & (x.typ.BaseTyp.form IN charSet) THEN
				OPB.StrDeref(x)
			END;
			IF (OPM.Lang = "C") & (y.typ.form = Pointer) THEN OPB.DeRef(y) END;
			IF (y.typ.comp IN {Array, DynArr}) & (y.typ.BaseTyp.form IN charSet) THEN
				OPB.StrDeref(y)
			END;
			OPB.Op(addop, x, y)
		END
	END SimpleExpression;

	PROCEDURE Expression(VAR x: OPT.Node);
		VAR y, pre, last: OPT.Node; obj: OPT.Object; relation: BYTE;
	BEGIN SimpleExpression(x);
		IF (eql <= sym) & (sym <= geq) THEN
			relation := sym; OPS.Get(sym); SimpleExpression(y);
			pre := NIL; last := NIL;
			IF (x.typ.comp IN {Array, DynArr}) & (x.typ.BaseTyp.form IN charSet) THEN
				OPB.StrDeref(x)
			END;
			IF (y.typ.comp IN {Array, DynArr}) & (y.typ.BaseTyp.form IN charSet) THEN
				OPB.StrDeref(y)
			END;
			OPB.CheckBuffering(x, NIL, NIL, pre, last);
			OPB.CheckBuffering(y, NIL, NIL, pre, last);
			OPB.Op(relation, x, y);
			IF pre # NIL THEN OPB.Construct(Ncomp, pre, x); pre.typ := x.typ; x := pre END
		ELSIF sym = in THEN
			OPS.Get(sym); SimpleExpression(y); OPB.In(x, y)
		ELSIF sym = is THEN
			OPS.Get(sym);
			IF sym = ident THEN
				qualident(obj);
				IF obj^.mode = Typ THEN OPB.TypTest(x, obj, FALSE)
				ELSE err(52)
				END
			ELSE err(ident)
			END
		END
	END Expression;

	PROCEDURE Receiver(VAR mode, vis: BYTE; VAR name: OPS.Name; VAR typ, rec: OPT.Struct);
		VAR tname: OPS.String;
	BEGIN typ := OPT.undftyp; rec := NIL; vis := 0;
		IF sym = var THEN OPS.Get(sym); mode := VarPar
		ELSIF (OPM.Lang = "C") & (sym = in) THEN OPS.Get(sym); mode := VarPar; vis := inPar
		ELSE mode := Var END;
		name := OPS.name; CheckSym(ident); CheckSym(colon);
		IF sym # ident THEN err(ident) END;
		Type(typ, tname);
		IF tname = NIL THEN
			IF typ^.form = Pointer THEN rec := typ^.BaseTyp ELSE rec := typ END;
			IF ~((mode = Var) & (typ^.form = Pointer) & (rec^.comp = Record) OR
				(mode = VarPar) & (typ^.comp = Record)) THEN err(70); rec := NIL END;
			IF (rec # NIL) & (rec^.mno # level) THEN err(72); rec := NIL END
		ELSE err(0)
		END;
		CheckSym(rparen);
		IF rec = NIL THEN rec := OPT.NewStr(Comp, Record); rec^.BaseTyp := NIL END
	END Receiver;

	PROCEDURE ProcedureDeclaration(VAR x: OPT.Node);
		VAR proc, fwd: OPT.Object;
			name: OPS.Name;
			mode: BYTE;
			forward: BOOLEAN;
			sys: SHORTINT;

		PROCEDURE GetCode;
			VAR ext: OPT.ConstExt; n, c, i: INTEGER; s: OPS.String;

			PROCEDURE EnsureLen(len: INTEGER);
				VAR j: INTEGER; s2: OPS.String;
			BEGIN
				IF len > LEN(s^) THEN	(* if overflow then increase size of array s *)
					NEW(s2, LEN(s^) * 2); FOR j := 0 TO n - 1 DO s2^[j] := s^[j] END; s := s2
				END
			END EnsureLen;

		BEGIN
			n := 0; NEW(s, 128); IF sym = comma THEN err(13) END;
			LOOP
				IF sym = number THEN c := SHORT(OPS.intval); EnsureLen(n + 1);
					IF OPS.numtyp # char THEN err(63); c := 1; n := 0
					ELSIF CHR(c) < " " THEN err(3)
					END;
					OPS.Get(sym); s^[n] := SHORT(CHR(c)); INC(n)
				ELSIF sym = string THEN
					c := SHORT(OPS.intval) - 1 (*exclude 0X*);
					IF c = 0 (*empty string*) THEN err(63) END; EnsureLen(n + c);
					FOR i := 0 TO c - 1 DO s^[n + i] := OPS.str^[i] END;
					OPS.Get(sym); INC(n, c)
				END;
				IF sym = comma THEN OPS.Get(sym); IF (sym # number) & (sym # string) THEN err(13) END
				ELSIF (sym = number) OR (sym = string) THEN err(comma)
				ELSE EXIT
				END
			END;
			IF n # 0 THEN NEW(ext, n); i := 0; IF sys > 0 THEN err(127) END;
				WHILE i < n DO ext^[i] := s^[i]; INC(i) END;
			ELSE ext := NIL
			END;
			proc^.conval^.ext := ext;
			INCL(proc^.conval^.setval, hasBody)
		END GetCode;

		PROCEDURE GetParams;
			VAR name: OPS.String;
		BEGIN
			proc^.mode := mode; proc^.typ := OPT.notyp;
			proc^.sysflag := SHORT(sys);
			proc^.conval := OPT.NewConst(); proc^.conval^.setval := {};
			IF sym = lparen THEN
				OPS.Get(sym); FormalParameters(proc^.link, proc^.typ, name);
				IF name # NIL THEN err(0) END
			END;
			CheckForwardTypes; userList := NIL;
			IF fwd # NIL THEN
				OPB.CheckParameters(proc^.link, fwd^.link, TRUE);
				IF proc^.typ # fwd^.typ THEN err(117) END ;
				proc := fwd; OPT.topScope := proc^.scope;
				IF mode = IProc THEN proc^.mode := IProc END
			END
		END GetParams;

		PROCEDURE Body;
			VAR procdec, statseq: OPT.Node; c: INTEGER;
		BEGIN
			c := OPM.errpos;
			INCL(proc^.conval^.setval, hasBody);
			CheckSym(semicolon); Block(procdec, statseq);
			OPB.Enter(procdec, statseq, proc); x := procdec;
			x^.conval := OPT.NewConst(); x^.conval^.intval := c;
			CheckSym(end);
			IF sym = ident THEN
				IF OPS.name # proc^.name^ THEN err(4) END;
				OPS.Get(sym)
			ELSE err(ident)
			END
		END Body;

		PROCEDURE TProcDecl;
			VAR baseProc, o: OPT.Object;
				objTyp, recTyp: OPT.Struct;
				objMode, objVis: BYTE;
				objName: OPS.Name;
				pnode: OPT.Node;
				fwdAttr: SET;
		BEGIN
			OPS.Get(sym); mode := TProc;
			IF level > 0 THEN err(73) END ;
			Receiver(objMode, objVis, objName, objTyp, recTyp);
			IF sym = ident THEN
				name := OPS.name;
				OPT.FindField(name, recTyp, fwd);
				OPT.FindField(name, recTyp^.BaseTyp, baseProc);
				IF (baseProc # NIL) & (baseProc^.mode # TProc) THEN baseProc := NIL END ;
				IF fwd = baseProc THEN fwd := NIL END ;
				IF (fwd # NIL) & (fwd^.mnolev # level) THEN fwd := NIL END ;
				IF (fwd # NIL) & (fwd^.mode = TProc) & (fwd^.conval^.setval * {hasBody, absAttr, empAttr} = {}) THEN
					(* there exists a corresponding forward declaration *)
					proc := OPT.NewObj(); proc^.leaf := TRUE;
					proc^.mode := TProc; proc^.conval := OPT.NewConst();
					CheckMark(proc);
					IF fwd^.vis # proc^.vis THEN err(118) END;
					fwdAttr := fwd^.conval^.setval
				ELSE
					IF fwd # NIL THEN err(1); fwd := NIL END ;
					OPT.OpenScope(0, NIL); OPT.topScope^.right := recTyp^.link; OPT.Insert(name, proc);
					recTyp^.link := OPT.topScope^.right; OPT.CloseScope;
					proc^.mode := TProc; proc^.conval := OPT.NewConst();
					CheckMark(proc);
					IF recTyp^.strobj # NIL THEN	(* preserve declaration order *)
						o := recTyp^.strobj^.link;
						IF o = NIL THEN recTyp^.strobj^.link := proc
						ELSE
							WHILE o^.nlink # NIL DO o := o^.nlink END;
							o^.nlink := proc
						END
					END
				END;
				INC(level); OPT.OpenScope(level, proc);
				OPT.Insert(objName, proc^.link); proc^.link^.mode := objMode; proc^.link^.vis := objVis; proc^.link^.typ := objTyp;
				ASSERT(OPT.topScope # NIL);
				GetParams;	(* may change proc := fwd !!! *)
				ASSERT(OPT.topScope # NIL);
				GetAttributes(proc, baseProc, recTyp);
				IF (fwd # NIL) & (fwdAttr / proc.conval.setval * {absAttr, empAttr, extAttr} # {}) THEN err(184) END;
				CheckOverwrite(proc, baseProc, recTyp);
				IF ~forward THEN
					IF empAttr IN proc^.conval^.setval THEN	(* insert empty procedure *)
						pnode := NIL; OPB.Enter(pnode, NIL, proc);
						pnode^.conval := OPT.NewConst();
						pnode^.conval^.intval := OPM.errpos;
						pnode^.conval^.intval2 := OPM.errpos;
						x := pnode;
					ELSIF ~(absAttr IN proc^.conval^.setval) THEN Body
					END;
					proc^.adr := 0
				ELSE
					proc^.adr := OPM.errpos;
					IF proc^.conval^.setval * {empAttr, absAttr} # {} THEN err(184) END
				END;
				DEC(level); OPT.CloseScope
			ELSE err(ident)
			END
		END TProcDecl;

	BEGIN proc := NIL; forward := FALSE; x := NIL; mode := LProc; sys := 0;
		IF (sym # ident) & (sym # lparen) THEN
			CASE sym OF
			| times: OPS.Get(sym) (* mode set later in OPB.CheckAssign *)
			| arrow: forward := TRUE; OPS.Get(sym)
			| plus: mode := IProc; OPS.Get(sym)
			| minus: mode := CProc; OPS.Get(sym)
			ELSE
			END;
			IF (mode IN {IProc, CProc}) & ~OPT.SYSimported THEN err(135) END;
			CheckSysFlagProc(sys)
		END;
		IF (sym = lparen) & (mode = LProc) & (sys = 0) & (OPM.Lang # "1") & (OPM.Lang # "7") THEN TProcDecl
		ELSIF sym = ident THEN OPT.Find(OPS.name, fwd);
			name := OPS.name;
			IF (fwd # NIL) & ((fwd^.mnolev # level) OR (fwd^.mode = SProc)) THEN fwd := NIL END ;
			IF (fwd # NIL) & (fwd^.mode IN {LProc, XProc}) & ~(hasBody IN fwd^.conval^.setval) THEN
				(* there exists a corresponding forward declaration *)
				proc := OPT.NewObj(); proc^.leaf := TRUE;
				IF (mode IN {IProc, CProc}) OR (fwd^.sysflag # sys) THEN err(133) END;
				proc^.mode := mode; proc^.conval := OPT.NewConst();
				CheckMark(proc);
				IF fwd^.vis # proc^.vis THEN err(118) END
			ELSE
				IF fwd # NIL THEN err(1); fwd := NIL END;
				OPT.Insert(name, proc);
				proc^.mode := mode; proc^.conval := OPT.NewConst();
				CheckMark(proc);
				IF mode = CProc THEN IF sys > 0 THEN err(127) END; sys := proc^.sysflag END
			END;
			IF (proc^.vis # internal) & (mode = LProc) THEN mode := XProc END;
			IF (mode # LProc) & (level > 0) THEN err(73) END;
			INC(level); OPT.OpenScope(level, proc);
			proc^.link := NIL; GetParams;	(* may change proc := fwd !!! *)
			IF mode = CProc THEN GetCode
			ELSIF proc^.entry # NIL THEN INCL(proc^.conval^.setval, hasBody)
			ELSIF ~forward THEN Body; proc.adr := 0
			ELSE proc.adr := OPM.errpos
			END;
			DEC(level); OPT.CloseScope
		ELSE err(ident)
		END
	END ProcedureDeclaration;

	PROCEDURE CaseLabelList(VAR lab: OPT.Node; LabelForm: SHORTINT; VAR n: SHORTINT; VAR tab: CaseTable);
		VAR x, y, lastlab: OPT.Node; i, f: SHORTINT; xval, yval: INTEGER;
	BEGIN lab := NIL; lastlab := NIL;
		LOOP ConstExpression(x); f := x^.typ^.form;
			IF f IN intSet + charSet THEN xval := SHORT(x^.conval^.intval)
			ELSE err(61); xval := 1
			END ;
			IF (f IN intSet) # (LabelForm IN intSet) THEN err(60) END;
			IF sym = upto THEN
				OPS.Get(sym); ConstExpression(y); yval := SHORT(y^.conval^.intval);
				IF (y^.typ^.form # f) & ~((f IN intSet) & (y^.typ^.form IN intSet)) THEN err(60) END ;
				IF yval < xval THEN err(63); yval := xval END
			ELSE yval := xval
			END ;
			x^.conval^.intval2 := yval;
			(*enter label range into ordered table*)  i := n;
			IF i < OPM.MaxCases THEN
				LOOP
					IF i = 0 THEN EXIT END ;
					IF tab[i-1].low <= yval THEN
						IF tab[i-1].high >= xval THEN err(62) END ;
						EXIT
					END ;
					tab[i] := tab[i-1]; DEC(i)
				END ;
				tab[i].low := xval; tab[i].high := yval; INC(n)
			ELSE err(213)
			END ;
			OPB.Link(lab, lastlab, x);
			IF sym = comma THEN OPS.Get(sym)
			ELSIF (sym = number) OR (sym = ident) THEN err(comma)
			ELSE EXIT
			END
		END
	END CaseLabelList;

	PROCEDURE StatSeq(VAR stat: OPT.Node);
		VAR fpar, id, t, obj: OPT.Object; idtyp: OPT.Struct; e: BOOLEAN; L: LONGINT;
				s, x, y, z, apar, last, lastif, pre, lastp: OPT.Node; pos: INTEGER; name: OPS.Name;

		PROCEDURE CasePart(VAR x: OPT.Node);
			VAR n: SHORTINT; low, high: INTEGER; e: BOOLEAN;
					tab: CaseTable; cases, lab, y, lastcase: OPT.Node;
		BEGIN
			Expression(x); pos := OPM.errpos;
			IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126)
			ELSIF ~(x^.typ^.form IN intSet + charSet) THEN err(125)
			END;
			CheckSym(of); cases := NIL; lastcase := NIL; n := 0;
			LOOP
				IF sym < bar THEN
					CaseLabelList(lab, x^.typ^.form, n, tab);
					CheckSym(colon); StatSeq(y);
					OPB.Construct(Ncasedo, lab, y); OPB.Link(cases, lastcase, lab)
				END;
				IF sym = bar THEN OPS.Get(sym) ELSE EXIT END
			END;
			IF n > 0 THEN low := tab[0].low; high := tab[n-1].high;
				IF high - low > OPM.MaxCaseRange THEN err(209) END
			ELSE low := 1; high := 0
			END;
			e := (sym = else) OR (OPM.Lang = "7");
			IF e & (OPM.Lang # "7") THEN OPS.Get(sym); StatSeq(y) ELSE y := NIL END;
			OPB.Construct(Ncaselse, cases, y); OPB.Construct(Ncase, x, cases);
			cases^.conval := OPT.NewConst();
			cases^.conval^.intval := low; cases^.conval^.intval2 := high;
			IF e THEN cases^.conval^.setval := {1} ELSE cases^.conval^.setval := {} END
		END CasePart;

		PROCEDURE SetPos(x: OPT.Node);
		BEGIN
			x^.conval := OPT.NewConst(); x^.conval^.intval := pos
		END SetPos;

		PROCEDURE CheckBool(VAR x: OPT.Node);
		BEGIN
			IF (x^.class = Ntype) OR (x^.class = Nproc) THEN err(126); x := OPB.NewBoolConst(FALSE)
			ELSIF x^.typ^.form # Bool THEN err(120); x := OPB.NewBoolConst(FALSE)
			END ;
			pos := OPM.errpos
		END CheckBool;

		PROCEDURE ForOriginal;
		BEGIN
			OPS.Get(sym);
			IF sym = ident THEN qualident(id);
				IF ~(id^.typ^.form IN intSet) THEN err(68) END;
				CheckSym(becomes); Expression(y); pos := OPM.errpos;
				x := OPB.NewLeaf(id); OPB.Assign(x, y); SetPos(x);
				CheckSym(to); Expression(y); pos := OPM.errpos;
				IF (OPM.Lang # "7") & (y^.class # Nconst) THEN
					name := "@@"; OPT.Insert(name, t); t^.name := OPT.NewName("@for");	(* avoid err 1 *)
					t^.mode := Var; t^.typ := x^.left^.typ;
					obj := OPT.topScope^.scope;
					IF obj = NIL THEN OPT.topScope^.scope := t
					ELSE
						WHILE obj^.link # NIL DO obj := obj^.link END;
						obj^.link := t
					END;
					z := OPB.NewLeaf(t); OPB.Assign(z, y); SetPos(z); OPB.Link(stat, last, z);
					y := OPB.NewLeaf(t)
				ELSIF (y^.typ^.form IN charSet + {Undef, Bool}) THEN err(113)
				ELSE OPB.CheckAssign(x^.left^.typ, y)
				END;
				OPB.Link(stat, last, x);
				IF sym = by THEN OPS.Get(sym); ConstExpression(z) ELSE z := OPB.NewIntConst(1) END ;
				pos := OPM.errpos; x := OPB.NewLeaf(id);
				IF z^.conval^.intval > 0 THEN OPB.Op(leq, x, y)
				ELSIF z^.conval^.intval < 0 THEN OPB.Op(geq, x, y)
				ELSE err(63); OPB.Op(geq, x, y)
				END ;
				CheckSym(do); StatSeq(s);
				y := OPB.NewLeaf(id); OPB.StPar1(y, z, incfn); SetPos(y);
				IF s = NIL THEN s := y
				ELSE z := s;
					WHILE z^.link # NIL DO z := z^.link END ;
					z^.link := y
				END ;
				CheckSym(end); OPB.Construct(Nwhile, x, s)
			ELSE err(ident)
			END
		END ForOriginal;

		PROCEDURE CalcNumIters(A, B: OPT.Node; Step: LONGINT);
		(* Вычисляем количество итераций цикла FOR. Результат:
		e=TRUE, если L = точное количество итераций
		e=FALSE, если L -верхняя граница кол-ва итераций (количество итераций <=L) *)
			VAR Nmin, Nmax: LONGINT;
		BEGIN
			IF Step>0 THEN
				IF A^.class = Nconst THEN Nmin := A^.conval^.intval
											ELSE Nmin := OPB.Min(id^.typ^.form)
				END;
				IF B^.class = Nconst THEN Nmax := B^.conval^.intval
											ELSE Nmax := OPB.Max(id^.typ^.form)
				END;
				L := OPM.UDiv(Nmax - Nmin, Step) + 1;
			ELSE
				IF A^.class = Nconst THEN Nmax := A^.conval^.intval
											ELSE Nmax := OPB.Max(id^.typ^.form)
				END;
				IF B^.class = Nconst THEN Nmin:= B^.conval^.intval
											ELSE Nmin:= OPB.Min(id^.typ^.form)
				END;
				L := OPM.UDiv(Nmax - Nmin, -Step) + 1
			END;
			e := (A^.class = Nconst) & (B^.class = Nconst);
			(* если обе константы, то можно вычислить точно *)
		END CalcNumIters;

		PROCEDURE CalcNlast(A, B: OPT.Node; Step: LONGINT): LONGINT;
		(* Вычисляем значение переменной цикла на последней итерации *)
		BEGIN
			ASSERT( (A^.class = Nconst) & (B^.class = Nconst) );
			RETURN A^.conval^.intval + (L-1) * Step
		END CalcNlast;

		PROCEDURE FindCond(A, B: OPT.Node; Step: LONGINT);
		(* найти оптимальное условие id?0 для замены цикла FOR
		циклом REPEAT UNTIL id?0 *)
		VAR Nlast: LONGINT; sgnPostLast, sgnPredLast: INTEGER; cond: SHORTINT;
		BEGIN
			cond := null; (* пока условие не нашли *)
			Nlast := CalcNlast(A, B, Step); (* последнее за границы типа не выйдет *)
			(* в отличие от последующего или предыдущего *)
			sgnPostLast := Sign(OPB.Short2Size(Nlast + Step, id^.typ^.form));
			sgnPredLast := Sign(OPB.Short2Size(Nlast - Step, id^.typ^.form));
			IF Nlast = 0 THEN cond := -eql
			ELSIF ( Sign(Nlast) = -Sign(Step) ) & (Sign(Nlast) # sgnPostLast )THEN
				CASE sgnPostLast OF
				|-1: cond := lss;
				| 0: cond := eql;
				| 1: cond := geq;
				END
			ELSIF Sign(Nlast) = Sign(Step) THEN
				IF (Sign(Nlast) # sgnPostLast) & (Sign(A^.conval^.intval + Step) # sgnPostLast) THEN
					(* тут можно бы воспользоваться флажком переполнения , если бы из С был доступ *)
					CASE sgnPostLast OF
					|-1: cond := lss;
					| 0: cond := eql;
					| 1: IF A^.conval^.intval + Step = 0 THEN cond := gtr ELSE cond := geq END;
					END;
				END
			END;

			IF ((cond = null) OR (cond = gtr)) & (Sign(Nlast) = Sign(Step)) &
				 (sgnPredLast # Sign(Nlast)) THEN
					CASE Sign(Nlast) OF
					|-1: cond := -lss
					| 0: cond := -eql
					| 1: IF sgnPredLast = 0 THEN cond := -gtr ELSE cond := -geq END
					END
				END;
			IF cond # null THEN
			(* auxiliary variable is unnecessary *)
				t := id;
				apar := OPB.NewLeaf(id);
				IF cond < 0 THEN
					 idtyp := NIL;
					 OPB.Assign(apar, OPB.NewIntConst(0))
				ELSE
					idtyp := id^.typ;
					ASSERT(idtyp # NIL);
					IF apar^.typ = OPT.ubytetyp THEN apar^.typ := OPT.bytetyp END;
					OPB.Assign(apar, z)
				 END;
				 apar^.subcl := SHORT(SHORT(ABS(cond)))
			ELSE
				t := NIL	(* auxiliary variable is needed *)
			END;
			(* если t=id то можно обойтись без вспомогательной переменной, применив
			либо REPEAT INC(id,Step); тело цикла; UNTIL id cond 0	(при idtyp=NIL)
			либо REPEAT тело цикла; INC(id,Step) UNTIL id cond 0	(при idtyp # NIL)
			где cond задается как apar^.subcl *)
		END FindCond;

		PROCEDURE ForImproved;	(* Concept and implementation by Oleg Komlev *)
			PROCEDURE varFor(id: OPT.Object): BOOLEAN;
			BEGIN
				RETURN (id^.mode = Var) & (id^.vis = 0) & (id^.mnolev = level)
			END varFor;
		BEGIN (* построение синтаксического дерева для FOR *)
			OPS.Get(sym);	(* взять символ после FOR *)
			IF sym = ident THEN qualident(id);	(* если это идентификатор, уточнить его *)
				IF id^.typ^.form IN intSet THEN	(* он должен быть целого типа *)
					IF ~varFor(id) THEN err(91) END;	(* он должен быть нужной локальности *)
					CheckSym(becomes); Expression(apar); pos := OPM.errpos;	(* потом д б  «:=  А » *)
					x := OPB.NewLeaf(id); OPB.Assign(x, apar); SetPos(x);	(* строим узел х=«id :=  А» *)
					(* apar = "A" *)
					CheckSym(to); Expression(y); pos := OPM.errpos;	(* далее д б ТО выражение (В) *)
					IF id^.typ = OPT.ubytetyp THEN y^.typ := OPT.ubytetyp END;
					IF sym = by THEN	(* если далее задан шаг BY Step,*)
						OPS.Get(sym); ConstExpression(z)	(* то читаем его как константное выражение*)
					ELSE
						z := OPB.NewIntConst(1)	(*иначе берем константу 1*)
					END;
					(* x = "id := A"; y = B; z = Step; apar = "A" *)
					obj := NIL;	(* by default IF is needed *)
					IF y^.class = Nconst THEN	(* if B is a constant *)
						IF (y^.conval^.intval < OPB.Min(x^.left^.typ^.form)) OR (y^.conval^.intval > OPB.Max(x^.left^.typ^.form))
						 OR (x^.left^.typ^.form # Byte) & ((y^.typ^.form IN charSet + {Undef, Bool}) OR (y^.typ^.form > x^.left^.typ^.form))
						THEN
							err(113); y := OPB.NewIntConst(0)	(* it must be a constant of a compatible type *)
						END;
						IF apar^.class = Nconst THEN (* А и В - константы*)
							IF (z^.conval^.intval > 0) & (apar^.conval^.intval <= y^.conval^.intval ) OR
								(z^.conval^.intval < 0) & (apar^.conval^.intval >= y^.conval^.intval ) THEN
									obj := id;	(* IF condition is not needed *)
							ELSE
									id := NIL (* цикл не выполнится ни разу *)
							END
						ELSE	(* В - константа, А -нет*)
								IF (z^.conval^.intval > 0) & (y^.conval^.intval = OPB.Max(id^.typ^.form)) OR
								(z^.conval^.intval < 0) & (y^.conval^.intval = OPB.Min(id^.typ^.form)) THEN
									obj := id; (* условие IF не нужно*)
								END
						END
					ELSE
						IF apar^.class = Nconst THEN	(* A - константа, B -нет*)
							IF (z^.conval^.intval > 0) & (apar^.conval^.intval = OPB.Min(id^.typ^.form)) OR
							(z^.conval^.intval < 0) & (apar^.conval^.intval = OPB.Max(id^.typ^.form)) THEN
								obj := id; (* условие IF не нужно*)
							END
						END
					END;

					IF id # NIL THEN
						CalcNumIters(apar, y, z^.conval^.intval) ;
						IF y^.class # Nconst THEN SetPos(y) END (* запомнить позицию В ??? *)
					END;
					pos := OPM.errpos;	(* теперь указатель ошибок указывает на Step*)
					IF (z^.conval^.intval = 0) OR (z^.typ^.form IN charSet + {Undef, Bool}) OR (z^.typ^.form > x^.left^.typ^.form) THEN
						IF (x^.left^.typ^.form # Byte) OR (z^.conval^.intval < MIN(BYTE)) OR (z^.conval^.intval > MAX(BYTE))
						THEN err(63); z := OPB.NewIntConst(1)
						END;
					END;

					(* id = NIL - цикл не выполнится ни разу *)
					(* иначе: x = "id:=A"; z=Step; apar="A"; y = "B" *)
					(* если e=TRUE, то L = кол-во итераций иначе L>=кол-во итераций *)
					IF id # NIL THEN
						IF L # 1 THEN (* попытаться обойтись без вспомогательной переменной*)
							IF e THEN
								FindCond(apar, y, z^.conval^.intval);
							ELSE
								(* если точно не подсчитать, то нужна вспомогательная переменная
								хотя в некоторых случаях можно и тут обойтись без нее*)
								IF z^.conval^.intval = OPB.Min(id^.typ^.form) THEN
									t := id; (* вспомогательная переменная не нужна *)
									idtyp := id^.typ; (* инкремент в конце цикла *)
									apar := OPB.NewLeaf(id);
									OPB.Assign(apar, z);
									apar^.subcl := geq (* условие id>=0 *)
								ELSE
									t := NIL	(* need an auxiliary variable *)
								END
							END
						END;
						IF t # id THEN	(* create an auxiliary variable *)
							name := "@@"; OPT.Insert(name, t); t^.name := OPT.NewName("@for");	(* avoid err 1 *)
							t^.mode := Var;
							IF x^.left^.typ = OPT.ubytetyp THEN t^.typ := OPT.bytetyp ELSE t^.typ := x^.left^.typ END;
							fpar := OPT.topScope^.scope;
							IF fpar = NIL THEN OPT.topScope^.scope := t
							ELSE
								WHILE fpar^.link # NIL DO fpar := fpar^.link END;
								fpar^.link := t
							END;
							apar := OPB.NewLeaf(t);
							OPB.Assign(apar, OPB.NewIntConst(-1));
							apar^.subcl := eql	(* condition t=0 *)
						END;

						IF (t = id) & (idtyp = NIL) THEN	(* а вот тут возможен выход константы за границы типа,
						поэтому свернем ее *)
							x^.right := OPB.NewShortConst(x^.right^.conval^.intval - z^.conval^.intval, id^.typ^.size);
						END;
						OPB.Link(stat, last, x); x := NIL;	(* добавим в дерево id := A или id := A -Step *)

						IF (t # id) & (L # 1) THEN (* строим t:= колво итераций *)
							IF e THEN
								s := OPB.NewShortConst(L, id^.typ^.size)
							ELSE
								IF (obj = NIL) & ( y^.class # Nconst) THEN (* нужно t := B*)
									pos := SHORT(y^.conval^.intval); (* восстановим позицию *)
									x := OPB.NewLeaf(t); OPB.Assign(x, y); SetPos(x); (* t:=B *)
									OPB.Link(stat, last, x); (* добавим в дерево t:= B *)
									y := x
								END;
								pos := OPM.errpos;
								x := OPB.NewLeaf(id);
								IF x^.typ = OPT.ubytetyp THEN x^.typ := OPT.bytetyp END;
								IF y^.class = Nconst THEN s := OPB.NewIntConst(y^.conval^.intval)
								ELSIF y^.class = Nassign THEN s := OPB.NewLeaf(t)
								ELSE s := y
								END;
								IF z^.conval^.intval > 0 THEN	(* подготовим выражение "кол-во повторений" *)
									OPB.Op(minus, s, x); (* B-x *)
									OPB.MOp(unsgn, s);
									OPB.Op(div, s, OPB.NewIntConst( z^.conval^.intval));	(* (B-x) div Step *)
									OPB.Op(plus, s, OPB.NewIntConst(1));	(* (B-x) div Step +1 *)
								ELSE	(* в зависимости от знака Step*)
									OPB.Op(minus, x, s); (* x-B *)
									OPB.MOp(unsgn, x);
									OPB.Op(div, x, OPB.NewIntConst( -z^.conval^.intval));	(* (x-B) div Step *)
									OPB.Op(plus, x, OPB.NewIntConst(1));	(* (x-B) div Step +1 *)
									s := x
								END
							END;
							x := OPB.NewLeaf(t); OPB.Assign(x, s); SetPos(x);(* строим узел х=«t:= колво итераций*)
						ELSE	(* фиктивный узел id=0 *)
							x := OPB.NewLeaf(id); OPB.Assign(x, OPB.NewIntConst(0)); SetPos(x);
						END
					ELSE x := NIL  (* от цикла не остается ничего *)
					END; (* id # NIL *)

					CheckSym(do); StatSeq(s); CheckSym(end);

					IF id # NIL THEN
						(* х=«t:= колво итераций; z=Step; apar="A"; y = "t := B"(или просто "В"); s="тело цикла" *)
						IF (t = id) & (idtyp = NIL) THEN
							x^.link := s;
							s := OPB.NewLeaf(id); OPB.StPar1(s, z, incfn); SetPos(s);	(* s= "INC(id,Step)" *)
							IF x^.link # NIL THEN s^.link := x^.link; x^.link := NIL END;
						END;
						(* s="INC(id,Step)";тело цикла" *)

						IF L # 1 THEN
							(* строим спецусловие для ForUntil: y=«id := z» или «id := 0» *)
							SetPos(apar);
							OPB.Construct(Nrepeat, s, apar); (* построили REPEAT UNTIL *)
							SetPos(s);
							IF t # id THEN (* нужен инкремент id*)
								apar := OPB.NewLeaf(id); OPB.StPar1(apar, z, incfn);
								SetPos(apar);	(* apar= "INC(id,Step)" *)
								x^.link := apar;

								apar := s^.left; (* тело цикла*)
								IF apar = NIL THEN s^.left := x^.link
								ELSE
									WHILE apar^.link # NIL DO apar := apar^.link END ;
									(* apar - последний оператор в теле цикла*)
									apar^.link := x^.link;
								END;
								x^.link := NIL;
							END;

							(* х="t:= колво итераций"; z=Step; y = "t:= B" ( "B") ; s="REPAT... UNTIL" *)
							IF t # id THEN x^.link := s;	(* х="t:= колво итераций; REPAT... UNTIL" *)
										ELSE x := s
							END;

						ELSE
							x := s
						END;

						(* build IF *)
						IF obj = NIL (* the loop body may not be executed even once *) THEN (* need IF *)
							IF y^.class = Nassign THEN y := OPB.NewLeaf(t) END;	(* assigned t := B *)
							s := OPB.NewLeaf(id);
							IF id^.typ = OPT.ubytetyp THEN OPB.MOp(unsgn, y) END;
							IF z^.conval^.intval > 0 THEN OPB.Op(leq, s, y) ELSE OPB.Op(geq, s, y) END;
							OPB.Construct(Nif, s, x); SetPos(s); lastif := s;	(* build IF id <= B THEN ... *)
							OPB.Construct(Nifelse, s, NIL);	(* no ELSE branch *)
							OPB.OptIf(s);
							x := s
						END;
						pos := OPM.errpos
					ELSE
						x := NIL	(* nothing is left of the loop *)
					END	(* id # NIL *)
				ELSE err(68); sym := while	(* id is not integer, process the block as if WHILE *)
				END
			ELSE err(ident)
			END
		END ForImproved;

	BEGIN stat := NIL; last := NIL; INC(ReturnLevel);
		LOOP x := NIL;
			IF sym < ident THEN err(14);
				REPEAT OPS.Get(sym) UNTIL sym >= ident
			END;
			IF sym = ident THEN
				qualident(id); x := OPB.NewLeaf(id); selector(x);
				IF sym = becomes THEN
					OPS.Get(sym); Expression(y);
					IF (OPM.Lang = "C") & (y.typ.form = Pointer) & (x.typ.form = Comp) THEN OPB.DeRef(y) END;
					pre := NIL; lastp := NIL;
					OPB.CheckBuffering(y, x, NIL, pre, lastp);
					OPB.Assign(x, y);
					IF pre # NIL THEN SetPos(x); OPB.Construct(Ncomp, pre, x); x := pre END
				ELSIF sym = eql THEN
					err(becomes); OPS.Get(sym); Expression(y); OPB.Assign(x, y)
				ELSIF (x^.class = Nproc) & (x^.obj^.mode = SProc) THEN
					StandProcCall(x);
					IF (x # NIL) & (x^.typ # OPT.notyp) THEN err(55) END
				ELSIF (OPM.Lang # "3") & (x.class = Ncall) THEN err(55)
				ELSE
					pre := NIL; lastp := NIL;
					OPB.PrepCall(x, fpar);
					IF (x.obj # NIL) & (x.obj.mode = TProc) THEN OPB.CheckBuffering(x.left, NIL, x.obj.link, pre, lastp) END;
					IF sym = lparen THEN
						OPS.Get(sym); ActualParameters(apar, fpar, pre, lastp); CheckSym(rparen)
					ELSE apar := NIL;
						IF fpar # NIL THEN err(65) END
					END;
					OPB.Call(x, apar, fpar);
					IF x^.typ # OPT.notyp THEN err(55) END;
					IF pre # NIL THEN SetPos(x); OPB.Construct(Ncomp, pre, x); x := pre END;
					IF level > 0 THEN OPT.topScope^.link^.leaf := FALSE END
				END;
				pos := OPM.errpos
			ELSIF sym = if THEN
				OPS.Get(sym); Expression(x); CheckBool(x); CheckSym(then); StatSeq(y);
				OPB.Construct(Nif, x, y); SetPos(x); lastif := x;
				WHILE sym = elsif DO
					OPS.Get(sym); Expression(y); CheckBool(y); CheckSym(then); StatSeq(z);
					OPB.Construct(Nif, y, z); SetPos(y); OPB.Link(x, lastif, y)
				END;
				IF sym = else THEN OPS.Get(sym); StatSeq(y) ELSE y := NIL END;
				OPB.Construct(Nifelse, x, y); CheckSym(end); OPB.OptIf(x); pos := OPM.errpos
			ELSIF sym = case THEN
				OPS.Get(sym); CasePart(x); CheckSym(end)
			ELSIF sym = while THEN
				OPS.Get(sym); Expression(x); CheckBool(x); CheckSym(do); StatSeq(y);
				IF sym = end THEN OPB.Construct(Nwhile, x, y)
				ELSIF (sym = elsif) & (OPM.Lang = "7") THEN
					OPB.Construct(Nwhiled, z, NIL); SetPos(z); OPB.Link(stat, last, z);
					OPB.Construct(Nif, x, y); SetPos(x); lastif := x;
					REPEAT
						OPS.Get(sym); Expression(y); CheckBool(y); CheckSym(do); StatSeq(z);
						OPB.Construct(Nif, y, z); SetPos(y); OPB.Link(x, lastif, y)
					UNTIL sym # elsif;
					OPB.Construct(Nifelse, x, NIL); OPB.OptIf(x); pos := OPM.errpos;
					SetPos(x); OPB.Link(stat, last, x); OPB.Construct(Nwhilede, x, NIL)
				END;
				CheckSym(end)
			ELSIF sym = repeat THEN
				OPS.Get(sym); StatSeq(x);
				IF sym = until THEN OPS.Get(sym); Expression(y); CheckBool(y)
				ELSE err(until)
				END;
				OPB.Construct(Nrepeat, x, y)
			ELSIF sym = for THEN
				IF OPM.for IN OPM.opt THEN ForImproved ELSE ForOriginal END
			ELSIF sym = loop THEN
				OPS.Get(sym); INC(LoopLevel); StatSeq(x); DEC(LoopLevel);
				OPB.Construct(Nloop, x, NIL); CheckSym(end); pos := OPM.errpos
			ELSIF sym = with THEN
				OPS.Get(sym); idtyp := NIL; x := NIL;
				LOOP
					IF sym = ident THEN
						qualident(id); y := OPB.NewLeaf(id);
						IF (id # NIL) & (id^.typ^.form = Pointer) & ((id^.mode = VarPar) OR ~id^.leaf) THEN
							err(245)	(* jt: do not allow WITH on non-local pointers *)
						END;
						CheckSym(colon);
						IF sym = ident THEN qualident(t);
							IF t^.mode = Typ THEN
								IF id # NIL THEN
									idtyp := id^.typ; OPB.TypTest(y, t, FALSE); id^.typ := t^.typ
								ELSE err(130)
								END
							ELSE err(52)
							END
						ELSE err(ident)
						END
					ELSE err(ident)
					END;
					pos := OPM.errpos; CheckSym(do); StatSeq(s); OPB.Construct(Nif, y, s); SetPos(y);
					IF idtyp # NIL THEN id^.typ := idtyp; idtyp := NIL END;
					IF x = NIL THEN x := y; lastif := x ELSE OPB.Link(x, lastif, y) END;
					IF sym = bar THEN OPS.Get(sym) ELSE EXIT END
				END;
				e := sym = else;
				IF e THEN OPS.Get(sym); StatSeq(s) ELSE s := NIL END;
				OPB.Construct(Nwith, x, s); CheckSym(end);
				IF e THEN x^.subcl := 1 END
			ELSIF sym = exit THEN
				OPS.Get(sym);
				IF LoopLevel = 0 THEN err(47) END;
				OPB.Construct(Nexit, x, NIL);
				pos := OPM.errpos
			ELSIF sym = return THEN OPS.Get(sym);
				IF OPM.Lang = "7" THEN
					IF (sym < semicolon) & (ReturnLevel = 1) THEN Expression(x) ELSE err(49) END
				ELSIF sym < semicolon THEN Expression(x) END;
				IF level > 0 THEN OPB.Return(x, OPT.topScope^.link)
				ELSE (* not standard Oberon *) OPB.Return(x, NIL); err(-49)
				END;
				pos := OPM.errpos;
				IF OPM.Lang = "7" THEN SetPos(x); OPB.Link(stat, last, x); EXIT END
			ELSE
				WHILE sym = raw DO
					NEW(x); x^.conval := OPT.NewConst(); x^.conval^.ext := OPS.str; OPB.Construct(Nraw, x, NIL);
					SetPos(x); OPB.Link(stat, last, x); x := NIL; OPS.Get(sym)
				END
			END;
			IF x # NIL THEN SetPos(x); OPB.Link(stat, last, x) END;
			IF (OPM.Lang = "7") & (sym = return) THEN (* no semicolon before RETURN *)
			ELSIF sym = semicolon THEN OPS.Get(sym)
			ELSIF (sym <= ident) OR (if <= sym) & (sym <= return)  THEN
				IF OPM.Lang # "3" THEN err(semicolon) END
			ELSE EXIT
			END
		END;
		DEC(ReturnLevel)
	END StatSeq;

	PROCEDURE Block(VAR procdec, statseq: OPT.Node);
		VAR typ: OPT.Struct;
			obj, first, last, o: OPT.Object;
			x, lastdec: OPT.Node;
			i: SHORTINT;
			rname: OPS.Name;
			name: OPS.String;
			rec: Elem;

	BEGIN
		IF ((sym < begin) OR (sym > var))
			& (sym # procedure) & (sym # end) & (sym # close) & (sym # return) & (sym # raw) THEN err(33) END;
		first := NIL; last := NIL; userList := NIL; recList := NIL;
		LOOP
			IF sym = const THEN
				OPS.Get(sym);
				WHILE sym = ident DO
					OPT.Insert(OPS.name, obj); obj^.mode := Con; CheckMark(obj);
					obj^.typ := OPT.sinttyp; obj^.mode := Var;	(* Var to avoid recursive definition *)
					IF sym = eql THEN
						OPS.Get(sym); ConstExpression(x)
					ELSIF sym = becomes THEN
						err(eql); OPS.Get(sym); ConstExpression(x)
					ELSE err(eql); x := OPB.NewIntConst(1)
					END ;
					obj^.mode := Con; obj^.typ := x^.typ; obj^.conval := x^.conval; (* ConstDesc ist not copied *)
					IF obj^.conval^.arr # NIL THEN  (* константный массив *)
						obj^.mode := VarPar; (* преобразуем в переменную-параметр *)
						IF obj^.vis = internal THEN obj^.vis := inPar ELSE obj^.vis := externalR END;
						IF x^.obj # NIL THEN obj^.typ := x^.obj^.typ END;
						IF last = NIL THEN OPT.topScope^.scope := obj ELSE last^.link := obj END;
						last := obj; first := NIL
					END;
					CheckSym(semicolon)
				END
			END;
			IF sym = type THEN
				OPS.Get(sym);
				WHILE sym = ident DO
					OPT.Insert(OPS.name, obj); obj^.mode := Typ; obj^.typ := OPT.undftyp;
					CheckMark(obj); obj.mode := -1;
					IF sym # eql THEN err(eql) END;
					IF (sym = eql) OR (sym = becomes) OR (sym = colon) THEN
						OPS.Get(sym); Type(obj^.typ, name); SetType(NIL, obj, obj^.typ, name)
					END;
					obj.mode := Typ;
					IF obj.typ.form IN {Byte..Set, Char16, LInt} THEN	(* make alias structure *)
						typ := OPT.NewStr(obj.typ.form, Basic); i := typ.ref;
						typ^ := obj.typ^; typ.ref := i; typ.strobj := NIL; typ.mno := 0; typ.txtpos := OPM.errpos;
						typ.BaseTyp := obj.typ; obj.typ := typ
					END;
					IF obj^.typ^.strobj = NIL THEN obj^.typ^.strobj := obj END;
					IF obj.typ.form = Pointer THEN	(* !!! *)
						typ := obj.typ.BaseTyp;
						IF (typ # NIL) & (typ.comp = Record) & (typ.strobj = NIL) THEN
							(* pointer to unnamed record: name record as "pointerName^" *)
							rname := obj.name$; i := 0;
							WHILE rname[i] # 0X DO INC(i) END;
							rname[i] := "^"; rname[i+1] := 0X;
							OPT.Insert(rname, o); o.mode := Typ; o.typ := typ; typ.strobj := o
						END
					END;
					IF obj.vis # internal THEN
						typ := obj.typ;
						IF typ.form = Pointer THEN typ := typ.BaseTyp END;
						IF typ.comp = Record THEN typ.exp := TRUE END
					END;
					CheckSym(semicolon)
				END
			END;
			IF sym = var THEN
				OPS.Get(sym);
				WHILE sym = ident DO
					LOOP
						IF sym = ident THEN
							OPT.Insert(OPS.name, obj);
							obj^.mode := Var; obj^.link := NIL; obj^.leaf := obj^.vis = internal; obj^.typ := OPT.undftyp;
							CheckMark(obj);
							IF first = NIL THEN first := obj END ;
							IF last = NIL THEN OPT.topScope^.scope := obj ELSE last^.link := obj END ;
							last := obj
						ELSE err(ident)
						END ;
						IF sym = comma THEN OPS.Get(sym)
						ELSIF sym = ident THEN err(comma)
						ELSE EXIT
						END
					END ;
					CheckSym(colon); Type(typ, name);
					CheckAlloc(typ, FALSE, OPM.errpos);
					WHILE first # NIL DO SetType(NIL, first, typ, name); first := first^.link END;
					CheckSym(semicolon)
				END
			END;
			IF (sym < const) OR (sym > var) THEN EXIT END ;
		END;
		CheckForwardTypes;
		userList := NIL; rec := recList; recList := NIL;
		OPT.topScope^.adr := OPM.errpos;
		procdec := NIL; lastdec := NIL;
		WHILE sym = raw DO
			NEW(x); x^.conval := OPT.NewConst(); x^.conval^.ext := OPS.str; OPB.Construct(Nraw, x, NIL);
			x^.conval := OPT.NewConst(); x^.conval^.intval := OPM.errpos; OPB.Link(statseq, procdec, x);
			OPS.Get(sym)
		END;
		IF (sym # procedure) & (sym # begin) & (sym # end) & (sym # close) & (sym # return) THEN err(34) END;
		WHILE sym = procedure DO
			OPS.Get(sym); ProcedureDeclaration(x);
			IF x # NIL THEN
				IF lastdec = NIL THEN procdec := x ELSE lastdec^.link := x END ;
				lastdec := x
			END;
			CheckSym(semicolon)
		END;
		IF OPM.noerr & (OPM.Lang = "C") THEN CheckRecords(rec) END;
		IF (sym = begin) & ~((level = 0) & (OPM.noinit IN OPM.opt)) THEN OPS.Get(sym); StatSeq(statseq)
		ELSIF (OPM.Lang = "7") & (sym = return) THEN StatSeq(statseq)
		ELSE statseq := NIL
		END;
		IF (level = 0) & (TDinit # NIL) THEN
			lastTDinit^.link := statseq; statseq := TDinit
		END
	END Block;

	PROCEDURE Module*(VAR prog: OPT.Node; opt: SET);
		VAR impName, aliasName: OPS.Name;
				procdec, statseq: OPT.Node;
				c: INTEGER; done: BOOLEAN;
	BEGIN
		OPS.Init; LoopLevel := 0; ReturnLevel := 0; level := 0; OPS.Get(sym);
		IF sym = module THEN OPS.Get(sym) ELSE err(16) END;
		IF sym = lbrak THEN OPS.Get(sym);
			IF (sym = ident) & (OPS.name = "foreign") THEN
				OPM.opt := OPM.opt + {OPM.foreign, OPM.noinit}; OPS.Get(sym)
			ELSIF (sym = ident) & (OPS.name = "main") THEN
				INCL(OPM.opt, OPM.mainprog); OPS.Get(sym)
			ELSIF (sym = ident) & (OPS.name = "noinit") THEN
				INCL(OPM.opt, OPM.noinit); OPS.Get(sym)
			ELSE
				OPS.Get(sym); err(178)
			END;
			CheckSym(rbrak)
		END;
		IF sym = ident THEN
			OPM.LogW(" "); OPM.LogWStr(OPS.name);
			OPT.Init(OPS.name, OPM.Lang, opt); OPS.Get(sym);
			CheckSym(semicolon);
			IF sym = import THEN OPS.Get(sym);
				LOOP
					IF sym = ident THEN
						aliasName := OPS.name$; impName := aliasName$; OPS.Get(sym);
						IF sym = becomes THEN OPS.Get(sym);
							IF sym = ident THEN impName := OPS.name$; OPS.Get(sym) ELSE err(ident) END
						END;
						OPT.Import(aliasName, impName, done)
					ELSE err(ident)
					END;
					IF sym = comma THEN OPS.Get(sym)
					ELSIF sym = ident THEN err(comma)
					ELSE EXIT
					END
				END;
				CheckSym(semicolon)
			END;
			IF OPM.noerr THEN TDinit := NIL; lastTDinit := NIL; c := OPM.errpos;
				Block(procdec, statseq); OPB.Enter(procdec, statseq, NIL); prog := procdec;
				prog^.conval := OPT.NewConst(); prog^.conval^.intval := c;
				IF sym = close THEN OPS.Get(sym); StatSeq(prog.link) END;
				CheckSym(end);
				IF sym = ident THEN
					IF OPS.name # OPT.SelfName THEN err(4) END;
					OPS.Get(sym)
				ELSE err(ident)
				END;
				IF sym # period THEN err(period) END
			END
		ELSE err(ident)
		END;
		TDinit := NIL; lastTDinit := NIL
	END Module;

END OfrontOPP.
