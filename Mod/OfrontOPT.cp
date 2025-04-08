MODULE OfrontOPT;	(* NW, RC 6.3.89 / 23.1.92 *)	(* object model 24.2.94 *)

(*
2002-04-11  jt: SHORT(SHORT(-mno)) used for -mno for BB 1.4
2002-08-20  jt: NewStr: txtpos remains 0 for structs read from symbol files
2016-10-12  jt: issue #7: FPrintStr improved: constant added to btyp^.pbfp for avoiding a collision pattern
2018-02-14  olegncher: issue #44: unlimited length of strings (as in BB)
*)

	IMPORT
		OPS := OfrontOPS, OPM := OfrontOPM;

	TYPE
		Const* = POINTER TO ConstDesc;
		Object* = POINTER TO ObjDesc;
		Struct* = POINTER TO StrDesc;
		Node* = POINTER TO NodeDesc;
		ConstExt* = OPS.String;

		ConstArr* = POINTER TO RECORD
			val1*: POINTER TO ARRAY OF BYTE;
			val2*: POINTER TO ARRAY OF SHORTINT;
			val4*: POINTER TO ARRAY OF INTEGER;
			val8*: POINTER TO ARRAY OF LONGINT
		END;

		ConstDesc* = RECORD
			ext*: ConstExt;	(* string or code for code proc *)
			intval*: LONGINT;	(* constant value or adr, proc par size, text position or least case label *)
			intval2*: INTEGER;	(* string length, proc var size or larger case label *)
			setval*: SET;	(* constant value, procedure body present or "ELSE" present in case *)
			realval*: REAL;	(* real or longreal constant value *)
			arr*: ConstArr	(*  constant array data *)
		END;

		ObjDesc* = RECORD
			left*, right*, link*, scope*: Object;
			name*: OPS.String;	(* name = null OR name^ # "" *)
			leaf*: BOOLEAN;
			sysflag*: BYTE;
			mode*, mnolev*: BYTE;	(* mnolev < 0 -> mno = -mnolev *)
			vis*: BYTE;	(* internal, external, externalR, inPar, outPar *)
			history*: BYTE;	(* relevant if name # "" *)
			used*, fpdone*: BOOLEAN;
			fprint*: INTEGER;
			typ*: Struct;	(* actual type, changed in with statements *)
			ptyp*: Struct;	(* original type if typ is changed *)
			conval*: Const;
			nlink*: Object;	(* link for name list, declaration order for methods, library link for imp obj *)
			entry*: OPS.String;	(* entry name *)
			adr*, linkadr*: INTEGER;
			x*: SHORTINT	(* linkadr and x can be freely used by the backend *)
		END ;

		StrDesc* = RECORD
			form*, comp*, mno*, extlev*: BYTE;
			ref*, sysflag*: SHORTINT;
			n*, size*, align*, txtpos*: INTEGER;	(* align is alignment for records and len offset for dynarrs *)
			allocated*, pbused*, pvused*, exp*, fpdone, idfpdone: BOOLEAN;
			attribute*: BYTE;
			idfp, pbfp*, pvfp*:INTEGER;
			BaseTyp*: Struct;
			link*, strobj*: Object
		END ;

		NodeDesc* = RECORD
			left*, right*, link*: Node;
			class*, subcl*, hint*: BYTE;
			readonly*: BOOLEAN;
			typ*: Struct;
			obj*: Object;
			conval*: Const
		END ;

	CONST
		maxImps = 64;	(* must be <= MAX(SHORTINT) *)
		maxStruct = OPM.MaxStruct;	(* must be < MAX(INTEGER) DIV 2 *)
		FirstRef = 32;	(* this number affects the structure of .sym files *)

	VAR
		typSize*: PROCEDURE(typ: Struct);
		topScope*: Object;
		undftyp*, bytetyp*, booltyp*, char8typ*, sinttyp*, inttyp*, linttyp*,
		realtyp*, lrltyp*, settyp*, string8typ*, niltyp*, notyp*, sysptrtyp*, ubytetyp*,
		anytyp*, anyptrtyp*, char16typ*, string16typ*: Struct;
		char8, int8, int16, int32, int64, adrint, byte, ubyte, real32, real64, char16: Object;
		nofGmod*: BYTE;	(*nof imports*)
		GlbMod*: ARRAY maxImps OF Object;	(* ^.right = first object, ^.name = module import name (not alias) *)
		SelfName*: OPS.Name;	(* name of module being compiled *)
		SYSimported*: BOOLEAN;
		null-: OPS.String;	(* "" *)

	CONST
		(* object modes *)
		Var = 1; VarPar = 2; Con = 3; Fld = 4; Typ = 5; LProc = 6; XProc = 7;
		SProc = 8; CProc = 9; IProc = 10; Mod = 11; Head = 12; TProc = 13; Attr = 20;

		(* structure forms *)
		Undef = 0; Byte = 1; Bool = 2; Char8 = 3; SInt = 4; Int = 5; LInt = 6;
		Real = 7; LReal = 8; Set = 9; String8 = 10; NilTyp = 11; NoTyp = 12;
		Pointer = 13; UByte = 14; ProcTyp = 15; Comp = 16;
		AnyPtr = 15; AnyRec = 16;	(* sym file only *)
		Char16 = 17; String16 = 18;

		(* composite structure forms *)
		Basic = 1; Array = 2; DynArr = 3; Record = 4;

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

		(* attribute flags (attr.adr, struct.attribute, proc.conval.setval) *)
		newAttr = 16; absAttr = 17; limAttr = 18; empAttr = 19; extAttr = 20;

		(* module visibility of objects *)
		internal = 0; external = 1; externalR = 2; inPar = 3; outPar = 4;

		(* history of imported objects *)
		inserted = 0; same = 1; pbmodified = 2; pvmodified = 3; removed = 4; inconsistent = 5;

		(* sysflags *)
		noInit = 1; doClose = 2; inBit = 2; outBit = 4;

		(* symbol file items *)
		Smname = 16; Send = 18; Stype = 19; Salias = 20; Svar = 21; Srvar = 22;
		Svalpar = 23; Svarpar = 24; Sfld = 25; Srfld = 26; Shdptr = 27; Shdpro = 28; Stpro = 29; Shdtpro = 30;
		Sxpro = 31; Sipro = 32; Scpro = 33; Sstruct = 34; Ssys = 35; Sptr = 36; Sarr = 37; Sdarr = 38; Srec = 39; Spro = 40;
		Sentry = 43; Sinpar = 25; Soutpar = 26;
		Slimrec = 25; Sabsrec = 26; Sextrec = 27; Slimpro = 31; Sabspro = 32; Semppro = 33; Sextpro = 34; Simpo = 22;

	TYPE
		ImpCtxt = RECORD
			nextTag, reffp: INTEGER;
			nofr, minr, nofm: SHORTINT;
			self: BOOLEAN;
			ref: ARRAY maxStruct OF Struct;
			old: ARRAY maxStruct OF Object;
			pvfp: ARRAY maxStruct OF INTEGER;	(* set only if old # NIL *)
			glbmno: ARRAY maxImps OF BYTE	(* index is local mno *)
		END;

		ExpCtxt = RECORD
			reffp: INTEGER;
			ref: SHORTINT;
			nofm: BYTE;
			locmno: ARRAY maxImps OF BYTE	(* index is global mno *)
		END ;

	VAR
		universe, syslink, infinity: Object;
		impCtxt: ImpCtxt;
		expCtxt: ExpCtxt;
		nofhdfld: INTEGER;
		newsf, findpc, extsf, sfpresent, symExtended, symNew: BOOLEAN;
		depth: INTEGER;

	PROCEDURE err(n: SHORTINT);
	BEGIN OPM.err(n)
	END err;

	PROCEDURE NewConst*(): Const;
		VAR const: Const;
	BEGIN NEW(const); RETURN const
	END NewConst;

	PROCEDURE NewObj*(): Object;
		VAR obj: Object;
	BEGIN NEW(obj); obj^.name := null; RETURN obj
	END NewObj;

	PROCEDURE NewStr*(form, comp: BYTE): Struct;
		VAR typ: Struct;
	BEGIN NEW(typ); typ^.form := form; typ^.comp := comp; typ^.ref := maxStruct;	(* ref >= maxStruct: not exported yet *)
		IF form # Undef THEN typ^.txtpos := OPM.errpos END ;	(* txtpos remains 0 for structs read from symbol file *)
		typ^.size := -1; typ^.BaseTyp := undftyp; RETURN typ
	END NewStr;

	PROCEDURE NewNode*(class: BYTE): Node;
		VAR node: Node;
	BEGIN NEW(node); node^.class := class; RETURN node
	END NewNode;

	PROCEDURE NewName* (IN name: ARRAY OF SHORTCHAR): OPS.String;
		VAR i: INTEGER; p: OPS.String;
	BEGIN
		i := LEN(name$);
		IF i > 0 THEN NEW(p, i + 1); p^ := name$; RETURN p
		ELSE RETURN null
		END
	END NewName;

	PROCEDURE OpenScope*(level: BYTE; owner: Object);
		VAR head: Object;
	BEGIN head := NewObj();
		head^.mode := Head; head^.mnolev := level; head^.link := owner;
		IF owner # NIL THEN owner^.scope := head END ;
		head^.left := topScope; head^.right := NIL; head^.scope := NIL; topScope := head
	END OpenScope;

	PROCEDURE CloseScope*;
	BEGIN IF topScope # NIL THEN topScope := topScope^.left END
	END CloseScope;

	PROCEDURE Close*;
		VAR i: SHORTINT;
	BEGIN	(* garbage collection *)
		CloseScope;
		i := 0; WHILE i < maxImps DO GlbMod[i] := NIL; INC(i) END;
		i := FirstRef; WHILE i < maxStruct DO impCtxt.ref[i] := NIL; impCtxt.old[i] := NIL; INC(i) END
	END Close;

	PROCEDURE SameType* (x, y: Struct): BOOLEAN;
	BEGIN
		RETURN (x = y) OR (x.form = y.form) & ~(x.form IN {Pointer, ProcTyp, Comp}) OR (x = undftyp) OR (y = undftyp)
	END SameType;

	PROCEDURE EqualType* (x, y: Struct): BOOLEAN;
		VAR xp, yp: Object; n: INTEGER;
	BEGIN
		n := 0;
		WHILE (n < 100) & (x # y)
			& (((x.comp = DynArr) & (y.comp = DynArr) & (x.sysflag = y.sysflag))
				OR ((x.form = Pointer) & (y.form = Pointer))
				OR ((x.form = ProcTyp) & (y.form = ProcTyp))) DO
			IF x.form = ProcTyp THEN
				IF x.sysflag # y.sysflag THEN RETURN FALSE END;
				xp := x.link; yp := y.link;
				INC(depth);
				WHILE (xp # NIL) & (yp # NIL) & (xp.mode = yp.mode) & (xp.sysflag = yp.sysflag)
						& (xp.vis = yp.vis) & (depth < 100) & EqualType(xp.typ, yp.typ) DO
					xp := xp.link; yp := yp.link
				END;
				DEC(depth);
				IF (xp # NIL) OR (yp # NIL) THEN RETURN FALSE END
			END;
			x := x.BaseTyp; y := y.BaseTyp; INC(n)
		END;
		RETURN SameType(x, y)
	END EqualType;

	PROCEDURE Extends* (x, y: Struct): BOOLEAN;
	BEGIN
		IF (x.form = Pointer) & (y.form = Pointer) THEN x := x.BaseTyp; y := y.BaseTyp END;
		IF (x.comp = Record) & (y.comp = Record) THEN
			IF (y = anytyp) & (x.sysflag = 0) THEN RETURN TRUE END;
			WHILE (x # NIL) & (x # undftyp) & (x # y) DO x := x.BaseTyp END
		END;
		RETURN (x # NIL) & EqualType(x, y)
	END Extends;

	PROCEDURE Includes* (xform, yform: INTEGER): BOOLEAN;
	BEGIN
		CASE xform OF
		| Char16: RETURN yform IN {Char8, Char16, Byte}
		| SInt: RETURN yform IN {Char8, Byte, SInt}
		| Int: RETURN yform IN {Char8, Char16, Byte, SInt, Int}
		| LInt: RETURN yform IN {Char8, Char16, Byte, SInt, Int, LInt}
		| Real: RETURN yform IN {Char8, Char16, Byte, SInt, Int, LInt, Real}
		| LReal: RETURN yform IN {Char8, Char16, Byte, SInt, Int, LInt, Real, LReal}
		| String16: RETURN yform IN {String8, String16}
		ELSE RETURN xform = yform
		END
	END Includes;

	PROCEDURE FindImport*(mod: Object; VAR res: Object);
		VAR obj: Object;
	BEGIN obj := mod^.scope;
		LOOP
			IF obj = NIL THEN EXIT END ;
			IF OPS.name < obj^.name^ THEN obj := obj^.left
			ELSIF OPS.name > obj^.name^ THEN obj := obj^.right
			ELSE (*found*)
				IF (obj^.mode = Typ) & (obj^.vis = internal) THEN obj := NIL
				ELSE obj^.used := TRUE
				END ;
				EXIT
			END
		END ;
		res := obj
	END FindImport;

	PROCEDURE Find*(IN name: ARRAY OF SHORTCHAR; VAR res: Object);
		VAR obj, head: Object;
	BEGIN head := topScope;
		LOOP obj := head^.right;
			LOOP
				IF obj = NIL THEN EXIT END ;
				IF name < obj^.name^ THEN obj := obj^.left
				ELSIF name > obj^.name^ THEN obj := obj^.right
				ELSE (* found, obj^.used not set for local objects *) EXIT
				END
			END ;
			IF obj # NIL THEN EXIT END ;
			head := head^.left;
			IF head = NIL THEN EXIT END
		END ;
		res := obj
	END Find;

	PROCEDURE FindFld (IN name: ARRAY OF SHORTCHAR; typ: Struct; VAR res: Object);
		VAR obj: Object;
	BEGIN
		WHILE (typ # NIL) & (typ # undftyp) DO obj := typ^.link;
			WHILE obj # NIL DO
				IF name < obj^.name^ THEN obj := obj^.left
				ELSIF name > obj^.name^ THEN obj := obj^.right
				ELSE (*found*) res := obj; RETURN
				END
			END;
			typ := typ^.BaseTyp
		END;
		res := NIL
	END FindFld;

	PROCEDURE FindField* (IN name: ARRAY OF SHORTCHAR; typ: Struct; VAR res: Object);
	BEGIN
		FindFld(name, typ, res);
		IF (res = NIL) & (typ.sysflag = 0) THEN FindFld(name, anytyp, res) END
	END FindField;

	PROCEDURE FindBaseField* (IN name: ARRAY OF SHORTCHAR; typ: Struct; VAR res: Object);
	BEGIN
		FindFld(name, typ.BaseTyp, res);
		IF (res = NIL) & (typ.sysflag = 0) THEN FindFld(name, anytyp, res) END
	END FindBaseField;

	PROCEDURE Insert* (IN name: ARRAY OF SHORTCHAR; VAR obj: Object);
		VAR ob0, ob1: Object; left: BOOLEAN; mnolev: BYTE;
	BEGIN ob0 := topScope; ob1 := ob0^.right; left := FALSE;
		LOOP
			IF ob1 # NIL THEN
				IF name < ob1^.name^ THEN ob0 := ob1; ob1 := ob0^.left; left := TRUE
				ELSIF name > ob1^.name^ THEN ob0 := ob1; ob1 := ob0^.right; left := FALSE
				ELSE (*double def*) err(1); ob0 := ob1; ob1 := ob0^.right
				END
			ELSE (*insert*) ob1 := NewObj(); ob1^.leaf := TRUE;
				IF left THEN ob0^.left := ob1 ELSE ob0^.right := ob1 END ;
				ob1^.left := NIL; ob1^.right := NIL; ob1^.name := NewName(name);
				mnolev := topScope^.mnolev; ob1^.mnolev := mnolev;
				EXIT
			END
		END ;
		obj := ob1
	END Insert;

	PROCEDURE Insert2 (IN name, lowcase: ARRAY OF SHORTCHAR; VAR obj: Object);
	BEGIN
		IF OPS.lowcase THEN Insert(lowcase, obj) ELSE Insert(name, obj) END
	END Insert2;

	PROCEDURE InsertThisField (obj: Object; typ: Struct; VAR old: Object);
		VAR ob0, ob1: Object; left: BOOLEAN; name: OPS.String;
	BEGIN
		IF typ.link = NIL THEN typ.link := obj
		ELSE
			ob1 := typ.link; name := obj.name;
			REPEAT
				IF name^ < ob1.name^ THEN ob0 := ob1; ob1 := ob1.left; left := TRUE
				ELSIF name^ > ob1.name^ THEN ob0 := ob1; ob1 := ob1.right; left := FALSE
				ELSE old := ob1; RETURN
				END
			UNTIL ob1 = NIL;
			IF left THEN ob0.left := obj ELSE ob0.right := obj END
		END
	END InsertThisField;

	PROCEDURE InsertField (IN name, lowcase: ARRAY OF SHORTCHAR; typ: Struct; VAR obj: Object);
		VAR old: Object;
	BEGIN
		obj := NewObj(); obj.leaf := TRUE;
		IF OPS.lowcase THEN obj.name := NewName(lowcase) ELSE obj.name := NewName(name) END;
		InsertThisField(obj, typ, old);
		IF old # NIL THEN err(1) END	(*double def*)
	END InsertField;

(*-------------------------- Fingerprinting --------------------------*)

	PROCEDURE FPrintName (VAR fp: INTEGER; IN name: ARRAY OF SHORTCHAR);
		VAR i: INTEGER; ch: SHORTCHAR;
	BEGIN i := 0;
		REPEAT ch := name[i]; OPM.FPrint(fp, ORD(ch)); INC(i) UNTIL ch = 0X
	END FPrintName;

	PROCEDURE ^IdFPrint(typ: Struct);

	PROCEDURE FPrintSign(VAR fp: INTEGER; result: Struct; par: Object);
	(* depends on assignment compatibility of params only *)
	BEGIN
		IdFPrint(result); OPM.FPrint(fp, result^.idfp);
		WHILE par # NIL DO
			OPM.FPrint(fp, par^.mode); IdFPrint(par^.typ); OPM.FPrint(fp, par^.typ^.idfp);
			IF (par^.mode = VarPar) & (par^.vis # 0) THEN OPM.FPrint(fp, par^.vis) END; (* IN / OUT *)
			IF par^.sysflag # 0 THEN OPM.FPrint(fp, par^.sysflag) END;
			(* par^.name and par^.adr not considered *)
			par := par^.link
		END
	END FPrintSign;

	PROCEDURE IdFPrint(typ: Struct);	(* idfp codifies assignment compatibility *)
		VAR btyp: Struct; strobj: Object; idfp: INTEGER; f, c: SHORTINT;
	BEGIN
		IF ~typ^.idfpdone THEN
			typ^.idfpdone := TRUE;	(* may be recursive, temporary idfp is 0 in that case *)
			idfp := 0; f := typ^.form; c := typ^.comp; OPM.FPrint(idfp, f); OPM.FPrint(idfp, c);
			btyp := typ^.BaseTyp; strobj := typ^.strobj;
			IF (strobj # NIL) & (strobj^.name # null) THEN
				FPrintName(idfp, GlbMod[typ^.mno]^.name^); FPrintName(idfp, strobj^.name^)
			END;
			IF (f = Pointer) OR (c = Record) & (btyp # NIL) OR (c = DynArr) THEN
				IdFPrint(btyp); OPM.FPrint(idfp, btyp^.idfp)
			ELSIF c = Array THEN IdFPrint(btyp); OPM.FPrint(idfp, btyp^.idfp); OPM.FPrint(idfp, typ^.n)
			ELSIF f = ProcTyp THEN FPrintSign(idfp, btyp, typ^.link)
			END;
			IF typ^.sysflag # 0 THEN OPM.FPrint(idfp, typ^.sysflag) END;  (* J. Templ, moved from FPrintStr *)
			typ^.idfp := idfp
		END
	END IdFPrint;

	PROCEDURE FPrintStr(typ: Struct);
		VAR f, c: SHORTINT; btyp: Struct; strobj, bstrobj: Object; pbfp, pvfp: INTEGER;

		PROCEDURE ^FPrintFlds(fld: Object; adr: INTEGER; visible: BOOLEAN);

		PROCEDURE FPrintHdFld(typ: Struct; fld: Object; adr: INTEGER);	(* modifies pvfp only *)
			VAR i, j, n: INTEGER; btyp: Struct;
		BEGIN
			IF typ^.comp = Record THEN FPrintFlds(typ^.link, adr, FALSE)
			ELSIF typ^.comp = Array THEN btyp := typ^.BaseTyp; n := typ^.n;
				WHILE btyp^.comp = Array DO n := btyp^.n * n; btyp := btyp^.BaseTyp END ;
				IF (btyp^.form = Pointer) OR (btyp^.comp = Record) THEN
					j := nofhdfld; FPrintHdFld(btyp, fld, adr);
					IF j # nofhdfld THEN i := 1;
						WHILE (i < n) & (nofhdfld <= OPM.MaxHdFld) DO
							INC(adr, btyp^.size); FPrintHdFld(btyp, fld, adr); INC(i)
						END
					END
				END
			ELSIF OPM.ExpHdPtrFld & ((typ^.form = Pointer) OR (fld^.name^ = OPM.HdPtrName)) THEN
				OPM.FPrint(pvfp, Pointer); OPM.FPrint(pvfp, adr); INC(nofhdfld)
			ELSIF OPM.ExpHdProcFld & ((typ^.form = ProcTyp) OR (fld^.name^ = OPM.HdProcName)) THEN
				OPM.FPrint(pvfp, ProcTyp); OPM.FPrint(pvfp, adr); INC(nofhdfld)
			END
		END FPrintHdFld;

		PROCEDURE FPrintFlds(fld: Object; adr: INTEGER; visible: BOOLEAN);	(* modifies pbfp and pvfp *)
		BEGIN
			WHILE (fld # NIL) & (fld^.mode = Fld) DO
				IF (fld^.vis # internal) & visible THEN
					OPM.FPrint(pbfp, fld^.vis); FPrintName(pbfp, fld^.name^); OPM.FPrint(pbfp, fld^.adr);
					FPrintStr(fld^.typ); OPM.FPrint(pbfp, fld^.typ^.pbfp); OPM.FPrint(pvfp, fld^.typ^.pvfp)
				ELSE FPrintHdFld(fld^.typ, fld, fld^.adr + adr)
				END ;
				fld := fld^.link
			END
		END FPrintFlds;

		PROCEDURE FPrintTProcs(obj: Object);	(* modifies pbfp and pvfp *)
			VAR fp: INTEGER;
		BEGIN
			IF obj # NIL THEN
				FPrintTProcs(obj^.left);
				IF obj^.mode = TProc THEN
					IF obj^.vis # internal THEN
						fp := 0;
						IF obj.vis = externalR THEN OPM.FPrint(fp, externalR) END;
						IF limAttr IN obj.conval.setval THEN OPM.FPrint(fp, limAttr)
						ELSIF absAttr IN obj.conval.setval THEN OPM.FPrint(fp, absAttr)
						ELSIF empAttr IN obj.conval.setval THEN OPM.FPrint(fp, empAttr)
						ELSIF extAttr IN obj.conval.setval THEN OPM.FPrint(fp, extAttr)
						END;
						OPM.FPrint(fp, TProc); OPM.FPrint(fp, obj^.adr DIV 10000H);
						FPrintSign(fp, obj^.typ, obj^.link); FPrintName(fp, obj^.name^);
						OPM.FPrint(pvfp, fp); OPM.FPrint(pbfp, fp)
					ELSIF OPM.ExpHdTProc THEN
						OPM.FPrint(pvfp, TProc); OPM.FPrint(pvfp, obj^.adr DIV 10000H)
					END
				END ;
				FPrintTProcs(obj^.right)
			END
		END FPrintTProcs;

	BEGIN
		IF ~typ^.fpdone THEN
			IdFPrint(typ); pbfp := typ^.idfp;
			(* IF typ^.sysflag # 0 THEN OPM.FPrint(pbfp, typ^.sysflag) END;   J. Templ, moved to IdFPrint *)
			pvfp := pbfp; typ^.pbfp := pbfp; typ^.pvfp := pvfp;	(* initial fprints may be used recursively *)
			typ^.fpdone := TRUE;
			f := typ^.form; c := typ^.comp; btyp := typ^.BaseTyp;
			IF f = Pointer THEN
				strobj := typ^.strobj; bstrobj := btyp^.strobj;
				IF (strobj = NIL) OR (strobj^.name = null) OR (bstrobj = NIL) OR (bstrobj^.name = null) THEN
					FPrintStr(btyp); OPM.FPrint(pbfp, btyp^.pbfp + 12345(*constant added for fixing issue #7*)); pvfp := pbfp
				(* else use idfp as pbfp and as pvfp, do not call FPrintStr(btyp) here, else cycle not broken *)
				END
			ELSIF f = ProcTyp THEN (* use idfp as pbfp and as pvfp *)
			ELSIF c IN {Array, DynArr} THEN FPrintStr(btyp); OPM.FPrint(pbfp, btyp^.pvfp); pvfp := pbfp
			ELSE (* c = Record *)
				IF btyp # NIL THEN FPrintStr(btyp); OPM.FPrint(pbfp, btyp^.pbfp); OPM.FPrint(pvfp, btyp^.pvfp) END ;
				OPM.FPrint(pvfp, typ^.size); OPM.FPrint(pvfp, typ^.align); OPM.FPrint(pvfp, typ^.n);
				nofhdfld := 0; FPrintFlds(typ^.link, 0, TRUE);
				IF nofhdfld > OPM.MaxHdFld THEN OPM.Mark(225, typ^.txtpos) END ;
				FPrintTProcs(typ^.link); OPM.FPrint(pvfp, pbfp); strobj := typ^.strobj;
				IF (strobj = NIL) OR (strobj^.name = null) THEN pbfp := pvfp END
			END ;
			typ^.pbfp := pbfp; typ^.pvfp := pvfp
		END
	END FPrintStr;

	PROCEDURE FPrintObj(obj: Object);
		VAR fprint, f, m: INTEGER; rval: SHORTREAL; ext: ConstExt;
	BEGIN
		IF ~obj^.fpdone THEN
			fprint := 0; obj^.fpdone := TRUE;
			IF obj^.mode = VarPar THEN OPM.FPrint(fprint, Var) ELSE OPM.FPrint(fprint, obj^.mode) END;
			IF obj^.mode = Con THEN
				f := obj^.typ^.form; OPM.FPrint(fprint, f);
				CASE f OF
				| Byte, Bool, Char8, Char16, SInt, Int:
					OPM.FPrint(fprint, SHORT(obj^.conval^.intval))
				| LInt:
					OPM.FPrint(fprint, SHORT(obj^.conval^.intval));
					OPM.FPrint(fprint, SHORT(ASH(obj^.conval^.intval, -32)))
				| Set:
					OPM.FPrintSet(fprint, obj^.conval^.setval)
				| Real:
					rval := SHORT(obj^.conval^.realval); OPM.FPrintReal(fprint, rval)
				| LReal:
					OPM.FPrintLReal(fprint, obj^.conval^.realval)
				| String8, String16:
					FPrintName(fprint, obj^.conval^.ext^)
				| NilTyp:
				ELSE err(127)
				END
			ELSIF obj^.mode IN {Var, VarPar} THEN
				OPM.FPrint(fprint, obj^.vis); FPrintStr(obj^.typ); OPM.FPrint(fprint, obj^.typ^.pbfp)
			ELSIF obj^.mode IN {XProc, IProc}  THEN
				FPrintSign(fprint, obj^.typ, obj^.link)
			ELSIF obj^.mode = CProc THEN
				FPrintSign(fprint, obj^.typ, obj^.link); ext := obj^.conval^.ext;
				IF ext # NIL THEN
					m := LEN(ext^); f := 0; OPM.FPrint(fprint, m);
					WHILE f < m DO OPM.FPrint(fprint, ORD(ext^[f])); INC(f) END
				ELSE OPM.FPrint(fprint, 0)
				END
			ELSIF obj^.mode = Typ THEN
				FPrintStr(obj^.typ); OPM.FPrint(fprint, obj^.typ^.pbfp)
			END;
			IF obj.sysflag # 0 THEN OPM.FPrint(fprint, obj.sysflag) END;
			IF (obj.mode IN {LProc, XProc, Var}) & (obj.entry # NIL) THEN FPrintName(fprint, obj.entry) END;
			obj^.fprint := fprint
		END
	END FPrintObj;

	PROCEDURE FPrintErr(obj: Object; errno: SHORTINT);
		VAR i, j: SHORTINT; ch: SHORTCHAR;
	BEGIN
		IF obj^.mnolev # 0 THEN
			OPM.objname := GlbMod[-obj^.mnolev]^.name^$; i := 0;
			WHILE OPM.objname[i] # 0X DO INC(i) END ;
			OPM.objname[i] := "."; j := 0; INC(i);
			REPEAT ch := obj^.name[j]; OPM.objname[i] := ch; INC(j); INC(i) UNTIL ch = 0X;
		ELSE
			OPM.objname := obj^.name^$
		END;
		IF errno = 249 THEN
			IF OPM.noerr THEN err(errno) END
		ELSIF errno = 253 THEN	(* extension *)
			IF ~symNew & ~symExtended & ~extsf THEN err(errno) END ;
			symExtended := TRUE
		ELSE
			IF ~symNew & ~newsf THEN err(errno) END ;
			symNew := TRUE
		END
	END FPrintErr;

(*-------------------------- Import --------------------------*)

	PROCEDURE InsertImport*(obj: Object; VAR root, old: Object);
		VAR ob0, ob1: Object; left: BOOLEAN;
	BEGIN
		IF root = NIL THEN root := obj; old := NIL
		ELSE
			ob0 := root; ob1 := ob0^.right; left := FALSE;
			IF obj^.name^ < ob0^.name^ THEN ob1 := ob0^.left; left := TRUE
			ELSIF obj^.name^ > ob0^.name^ THEN ob1 := ob0^.right; left := FALSE
			ELSE old := ob0; RETURN
			END ;
			LOOP
				IF ob1 # NIL THEN
					IF obj^.name^ < ob1^.name^ THEN ob0 := ob1; ob1 := ob1^.left; left := TRUE
					ELSIF obj^.name^ > ob1^.name^ THEN ob0 := ob1; ob1 := ob1^.right; left := FALSE
					ELSE old := ob1; EXIT
					END
				ELSE ob1 := obj;
					IF left THEN ob0^.left := ob1 ELSE ob0^.right := ob1 END ;
					ob1^.left := NIL; ob1^.right := NIL; old := NIL; EXIT
				END
			END
		END
	END InsertImport;

	PROCEDURE InName(OUT name: OPS.String);
		VAR i: SHORTINT; ch: SHORTCHAR; n: OPS.Name;
	BEGIN i := 0;
		REPEAT
			OPM.SymRCh(ch); n[i] := ch; INC(i)
		UNTIL ch = 0X;
		IF i > 1 THEN NEW(name, i); name^ := n$ ELSE name := null END
	END InName;

	PROCEDURE InMod(VAR mno: BYTE);	(* mno is global *)
		VAR head: Object; name: OPS.String; mn: INTEGER; i, sysflag: BYTE;
	BEGIN
		mn := SHORT(OPM.SymRInt());
		IF mn = 0 THEN mno := impCtxt.glbmno[0]
		ELSE
			IF mn = Ssys THEN
				sysflag := SHORT(SHORT(SHORT(OPM.SymRInt())));
				IF ODD(sysflag DIV doClose) & ~impCtxt.self THEN INCL(OPM.opt, OPM.close) END;
				mn := SHORT(OPM.SymRInt())
			ELSE
				sysflag := 0
			END;
			IF mn = Smname THEN
				InName(name);
				IF (name^ = SelfName) & ~impCtxt.self THEN err(154) END;
				i := 0;
				WHILE (i < nofGmod) & (name^ # GlbMod[i].name^) DO INC(i) END;
				IF i < nofGmod THEN mno := i	(*module already present*)
				ELSE
					head := NewObj(); head^.mode := Head; head^.name := name;
					head^.sysflag := sysflag;
					mno := nofGmod; head^.mnolev := SHORT(SHORT(-mno));
					IF nofGmod < maxImps THEN
						GlbMod[mno] := head; INC(nofGmod)
					ELSE err(227)
					END
				END;
				impCtxt.glbmno[impCtxt.nofm] := mno; INC(impCtxt.nofm)
			ELSE
				mno := impCtxt.glbmno[-mn]
			END
		END
	END InMod;

	PROCEDURE InConstant(f: INTEGER; conval: Const);
		VAR ch: SHORTCHAR; i, x, y: INTEGER; ext, t: ConstExt; rval: SHORTREAL; str: OPS.Name;
	BEGIN
		CASE f OF
		| Char8, Bool:
			OPM.SymRCh(ch); conval^.intval := ORD(ch)
		| Byte, SInt, Int, LInt:
			conval^.intval := OPM.SymRInt()
		| Char16:
			OPM.SymRCh(ch); conval.intval := ORD(ch);
			OPM.SymRCh(ch); conval.intval := conval.intval + ORD(ch) * 256
		| Set:
			OPM.SymRSet(conval^.setval)
		| Real:
			OPM.SymRReal(rval); conval^.realval := rval;
			conval^.intval := OPM.ConstNotAlloc
		| LReal:
			OPM.SymRLReal(conval^.realval);
			conval^.intval := OPM.ConstNotAlloc
		| String8, String16:
			i := 0;
			REPEAT
				OPM.SymRCh(ch);
				IF i < LEN(str) - 1 THEN str[i] := ch
				ELSIF i = LEN(str) - 1 THEN str[i] := 0X; NEW(ext, 2 * LEN(str)); ext^ := str$; ext[i] := ch
				ELSIF i < LEN(ext^) - 1 THEN ext[i] := ch
				ELSE t := ext; t[i] := 0X; NEW(ext, 2 * LEN(t^)); ext^ := t^$; ext[i] := ch
				END;
				INC(i)
			UNTIL ch = 0X;
			IF i < LEN(str) THEN NEW(ext, i); ext^ := str$ END;
			conval.ext := ext; conval^.intval := OPM.ConstNotAlloc;
			IF f = String8 THEN conval^.intval2 := i
			ELSE
				i := 0; y := 0;
				REPEAT OPM.GetUtf8(ext^, x, i); INC(y) UNTIL x = 0;
				conval.intval2 := y
			END
		| NilTyp:
			conval^.intval := OPM.nilval
		END
	END InConstant;

	PROCEDURE ^InStruct(VAR typ: Struct);

	PROCEDURE InSign(mno: BYTE; VAR res: Struct; VAR par: Object);
		VAR last, new: Object; tag: INTEGER;
	BEGIN
		InStruct(res);
		tag := SHORT(OPM.SymRInt()); last := NIL;
		WHILE tag # Send DO
			new := NewObj(); new^.mnolev := SHORT(SHORT(-mno));
			IF last = NIL THEN par := new ELSE last^.link := new END;
			IF tag = Ssys THEN
				new.sysflag := SHORT(SHORT(SHORT(OPM.SymRInt()))); tag := SHORT(OPM.SymRInt());
				IF ODD(new.sysflag DIV inBit) THEN new.vis := inPar
				ELSIF ODD(new.sysflag DIV outBit) THEN new.vis := outPar
				END
			END;
			IF tag = Svalpar THEN new^.mode := Var
			ELSE new^.mode := VarPar;
				IF tag = Sinpar THEN new.vis := inPar
				ELSIF tag = Soutpar THEN new.vis := outPar
				END
			END;
			InStruct(new^.typ); new^.adr := SHORT(OPM.SymRInt()); InName(new^.name);
			last := new; tag := SHORT(OPM.SymRInt())
		END
	END InSign;

	PROCEDURE InFld(): Object;	(* first number in impCtxt.nextTag, mno set outside *)
		VAR tag: INTEGER; obj: Object;
	BEGIN
		tag := impCtxt.nextTag; obj := NewObj();
		IF tag <= Srfld THEN
			obj^.mode := Fld;
			IF tag = Srfld THEN obj^.vis := externalR ELSE obj^.vis := external END ;
			InStruct(obj^.typ); InName(obj^.name);
			obj^.adr := SHORT(OPM.SymRInt())
		ELSE
			obj^.mode := Fld;
			IF tag = Shdptr THEN obj^.name := NewName(OPM.HdPtrName) ELSE obj^.name := NewName(OPM.HdProcName) END;
			obj^.typ := undftyp; obj^.vis := internal;
			obj^.adr := SHORT(OPM.SymRInt())
		END;
		RETURN obj
	END InFld;

	PROCEDURE InTProc(mno: BYTE): Object;	(* first number in impCtxt.nextTag *)
		VAR tag: INTEGER; obj: Object;
	BEGIN
		tag := impCtxt.nextTag;
		obj := NewObj(); obj^.mnolev := SHORT(SHORT(-mno));
		IF tag = Shdtpro THEN
			obj^.mode := TProc; obj^.name := NewName(OPM.HdTProcName);
			obj^.link := NewObj();	(* dummy, easier in Browser *)
			obj^.typ := undftyp; obj^.vis := internal;
			obj^.adr := 10000H*SHORT(OPM.SymRInt())
		ELSE
			obj.vis := external;
			IF tag = Simpo THEN obj.vis := externalR; tag := SHORT(OPM.SymRInt()) END;
			obj^.mode := TProc; obj^.conval := NewConst(); obj^.conval^.intval := -1;
			InSign(mno, obj^.typ, obj^.link); InName(obj^.name);
			obj^.adr := 10000H*SHORT(OPM.SymRInt());
			IF tag = Slimpro THEN INCL(obj.conval.setval, limAttr)
			ELSIF tag = Sabspro THEN INCL(obj.conval.setval, absAttr)
			ELSIF tag = Semppro THEN INCL(obj.conval.setval, empAttr)
			ELSIF tag = Sextpro THEN INCL(obj.conval.setval, extAttr)
			END
		END;
		RETURN obj
	END InTProc;

	PROCEDURE InStruct(VAR typ: Struct);
		VAR mno: BYTE; ref: SHORTINT; tag: INTEGER; name: OPS.String;
			t: Struct; obj, last, fld, old, dummy: Object;
	BEGIN
		tag := SHORT(OPM.SymRInt());
		IF tag # Sstruct THEN typ := impCtxt.ref[-tag]
		ELSE
			ref := impCtxt.nofr; INC(impCtxt.nofr);
			IF ref < impCtxt.minr THEN impCtxt.minr := ref END ;
			InMod(mno); InName(name); obj := NewObj();
			IF name = null THEN
				IF impCtxt.self THEN old := NIL	(* do not insert type desc anchor here, but in OPL *)
				ELSE obj^.name := NewName("@"); InsertImport(obj, GlbMod[mno].right, old(*=NIL*)); obj^.name := null
				END ;
				typ := NewStr(Undef, Basic)
			ELSE obj^.name := name; InsertImport(obj, GlbMod[mno].right, old);
				IF old # NIL THEN	(* recalculate fprints to compare with old fprints *)
					FPrintObj(old); impCtxt.pvfp[ref] := old^.typ^.pvfp;
					IF impCtxt.self THEN	(* do not overwrite old typ *)
						typ := NewStr(Undef, Basic)
					ELSE	(* overwrite old typ for compatibility reason *)
						typ := old^.typ; typ^.link := NIL; typ^.sysflag := 0;
						typ^.fpdone := FALSE; typ^.idfpdone := FALSE
					END
				ELSE typ := NewStr(Undef, Basic)
				END
			END ;
			impCtxt.ref[ref] := typ; impCtxt.old[ref] := old; typ^.ref := SHORT(ref + maxStruct);
			(* ref >= maxStruct: not exported yet, ref used for err 155 *)
			typ^.mno := mno; typ^.allocated := TRUE;
			typ^.strobj := obj; obj^.mode := Typ; obj^.typ := typ;
			obj^.mnolev := SHORT(SHORT(-mno)); obj^.vis := internal; (* name not visible here *)
			tag := SHORT(OPM.SymRInt());
			IF tag = Ssys THEN
				typ^.sysflag := SHORT(SHORT(OPM.SymRInt())); tag := SHORT(OPM.SymRInt())
			END;
			CASE tag OF
			| Sptr:
				typ^.form := Pointer; typ^.size := OPM.AdrSize; typ^.n := 0; InStruct(typ^.BaseTyp)
			| Sarr:
				typ^.form := Comp; typ^.comp := Array; InStruct(typ^.BaseTyp); typ^.n := SHORT(OPM.SymRInt());
				typSize(typ) (* no bounds address !! *)
			| Sdarr:
				typ^.form := Comp; typ^.comp := DynArr; InStruct(typ^.BaseTyp);
				IF typ^.BaseTyp^.comp = DynArr THEN typ^.n := typ^.BaseTyp^.n + 1
				ELSE typ^.n := 0
				END ;
				typSize(typ)
			| Srec, Sabsrec, Slimrec, Sextrec:
				typ^.form := Comp; typ^.comp := Record; InStruct(typ^.BaseTyp);
				IF typ^.BaseTyp = notyp THEN typ^.BaseTyp := NIL END;
				typ.extlev := 0; t := typ.BaseTyp;
				(* do not take extlev from base type due to possible cycles! *)
				WHILE (t # NIL) & (t^.comp = Record) DO INC(typ^.extlev); t := t.BaseTyp END;
				typ^.size := SHORT(OPM.SymRInt()); typ^.align := SHORT(OPM.SymRInt());
				typ^.n := SHORT(OPM.SymRInt());
				IF tag = Sabsrec THEN typ^.attribute := absAttr
				ELSIF tag = Slimrec THEN typ^.attribute := limAttr
				ELSIF tag = Sextrec THEN typ^.attribute := extAttr
				END;
				impCtxt.nextTag := SHORT(OPM.SymRInt()); last := NIL;
				WHILE (impCtxt.nextTag >= Sfld) & (impCtxt.nextTag <= Shdpro) OR (impCtxt.nextTag = Ssys) DO
					fld := InFld(); fld^.mnolev := SHORT(SHORT(-mno));
					IF last # NIL THEN last^.link := fld END ;
					last := fld; InsertImport(fld, typ^.link, dummy);
					impCtxt.nextTag := SHORT(OPM.SymRInt())
				END ;
				WHILE impCtxt.nextTag # Send DO fld := InTProc(mno);
					InsertImport(fld, typ^.link, dummy);
					impCtxt.nextTag := SHORT(OPM.SymRInt())
				END
			| Spro:
				typ^.form := ProcTyp; typ^.size := OPM.AdrSize; InSign(mno, typ^.BaseTyp, typ^.link)
			| Salias:
				InStruct(t);
				typ^.form := t^.form; typ^.comp := Basic; typ^.size := t^.size;
				typ^.pbfp := t^.pbfp; typ^.pvfp := t^.pvfp; typ^.fpdone := TRUE;
				typ^.idfp := t^.idfp; typ^.idfpdone := TRUE; typ^.BaseTyp := t
			END;
			IF ref = impCtxt.minr THEN
				WHILE ref < impCtxt.nofr DO
					t := impCtxt.ref[ref]; FPrintStr(t);
					obj := t^.strobj;	(* obj^.typ^.strobj = obj, else obj^.fprint differs (alias) *)
					IF obj^.name # null THEN FPrintObj(obj) END ;
					old := impCtxt.old[ref];
					IF old # NIL THEN t^.strobj := old;	(* restore strobj *)
						IF impCtxt.self THEN
							IF old^.mnolev < 0 THEN
								IF old^.history # inconsistent THEN
									IF old^.fprint # obj^.fprint THEN old^.history := pbmodified
									ELSIF impCtxt.pvfp[ref] # t^.pvfp THEN old^.history := pvmodified
									END
								(* ELSE remain inconsistent *)
								END
							ELSIF old^.fprint # obj^.fprint THEN old^.history := pbmodified
							ELSIF impCtxt.pvfp[ref] # t^.pvfp THEN old^.history := pvmodified
							ELSIF old^.vis = internal THEN old^.history := same	(* may be changed to "removed" in InObj *)
							ELSE old^.history := inserted	(* may be changed to "same" in InObj *)
							END
						ELSE
							(* check private part, delay error message until really used *)
							IF impCtxt.pvfp[ref] # t^.pvfp THEN old^.history := inconsistent END ;
							IF old^.fprint # obj^.fprint THEN FPrintErr(old, 249) END
						END
					ELSIF impCtxt.self THEN obj^.history := removed
					ELSE obj^.history := same
					END ;
					INC(ref)
				END ;
				impCtxt.minr := maxStruct
			END
		END
	END InStruct;

	PROCEDURE InObj(mno: BYTE): Object;	(* first number in impCtxt.nextTag *)
		VAR ch: SHORTCHAR; obj, old: Object; typ: Struct;
			i, s, tag: INTEGER; ext: ConstExt;
	BEGIN
		tag := impCtxt.nextTag;
		IF tag = Stype THEN
			InStruct(typ); obj := typ^.strobj;
			IF ~impCtxt.self THEN obj^.vis := external END	(* type name visible now, obj^.fprint already done *)
		ELSE
			obj := NewObj(); obj^.mnolev := SHORT(SHORT(-mno)); obj^.vis := external;
			IF tag = Ssys THEN
				obj.sysflag := SHORT(SHORT(SHORT(OPM.SymRInt()))); tag := SHORT(OPM.SymRInt())
			END;
			IF tag = Sentry THEN
				InName(obj.entry); tag := SHORT(OPM.SymRInt())
			END;
			IF (tag <= UByte) OR (tag = Char16) OR (tag = String16) THEN	(* Constant *)
				obj^.mode := Con; obj^.typ := impCtxt.ref[tag]; obj^.conval := NewConst(); InConstant(tag, obj^.conval)
			ELSIF tag >= Sxpro THEN
				obj^.conval := NewConst();
				obj^.conval^.intval := -1;
				InSign(mno, obj^.typ, obj^.link);
				CASE tag OF
				| Sxpro: obj^.mode := XProc
				| Sipro: obj^.mode := IProc
				| Scpro: obj^.mode := CProc; s := SHORT(OPM.SymRInt());
					IF s > 0 THEN
						NEW(ext, s); i := 0;
						WHILE i < s DO OPM.SymRCh(ext^[i]); INC(i) END
					ELSE ext := NIL
					END;
					obj^.conval^.ext := ext
				END
			ELSIF tag = Salias THEN
				obj^.mode := Typ; InStruct(obj^.typ)
			ELSE
				obj^.mode := Var;
				IF tag = Srvar THEN obj^.vis := externalR END ;
				InStruct(obj^.typ)
			END ;
			InName(obj^.name)
		END ;
		FPrintObj(obj);
		IF (obj^.mode = Var) & ((obj^.typ^.strobj = NIL) OR (obj^.typ^.strobj^.name = null)) THEN
			(* compute a global fingerprint to avoid structural type equivalence for anonymous types *)
			OPM.FPrint(impCtxt.reffp, obj^.typ^.ref - maxStruct)
		END ;
		IF tag # Stype THEN
			InsertImport(obj, GlbMod[mno].right, old);
			IF impCtxt.self THEN
				IF old # NIL THEN
					(* obj is from old symbol file, old is new declaration *)
					IF old^.vis = internal THEN old^.history := removed
					ELSE FPrintObj(old); FPrintStr(old.typ);	(* FPrint(obj) already called *)
						IF obj^.fprint # old^.fprint THEN old^.history := pbmodified
						ELSIF obj^.typ^.pvfp # old^.typ^.pvfp THEN old^.history := pvmodified
						ELSE old^.history := same
						END
					END
				ELSE obj^.history := removed	(* OutObj not called if mnolev < 0 *)
				END
			(* ELSE old = NIL, or file read twice, consistent, OutObj not called *)
			END
		ELSE	(* obj already inserted in InStruct *)
			IF impCtxt.self THEN	(* obj^.mnolev = 0 *)
				IF obj^.vis = internal THEN obj^.history := removed
				ELSIF obj^.history = inserted THEN obj^.history := same
				END
			(* ELSE OutObj not called for obj with mnolev < 0 *)
			END
		END;
		RETURN obj
	END InObj;

	PROCEDURE Import*(IN aliasName, name: ARRAY OF SHORTCHAR; VAR done: BOOLEAN);
		VAR obj: Object; mno: BYTE;	(* done used in Browser *)
	BEGIN
		IF name = "SYSTEM" THEN SYSimported := TRUE;
			Insert(aliasName, obj); obj^.mode := Mod; obj^.mnolev := 0; obj^.scope := syslink; obj^.typ := notyp
		ELSE
			impCtxt.nofr := FirstRef; impCtxt.minr := maxStruct; impCtxt.nofm := 0;
			impCtxt.self := aliasName = "@self"; impCtxt.reffp := 0;
			OPM.OldSym(name, done);
			IF done THEN
				OPM.checksum := 0;	(* start checksum here to avoid problems with proc id fixup *)
				InMod(mno);
				impCtxt.nextTag := SHORT(OPM.SymRInt());
				WHILE ~OPM.eofSF() DO
					obj := InObj(mno); impCtxt.nextTag := SHORT(OPM.SymRInt())
				END ;
				Insert(aliasName, obj);
				obj^.mode := Mod; obj^.scope := GlbMod[mno].right;
				GlbMod[mno].link := obj;
				obj^.mnolev := SHORT(SHORT(-mno)); obj^.typ := notyp;
				OPM.CloseOldSym
			ELSIF impCtxt.self THEN
				newsf := TRUE; extsf := TRUE; sfpresent := FALSE
			ELSE err(152)	(*sym file not found*)
			END
		END
	END Import;

(*-------------------------- Export --------------------------*)

	PROCEDURE OutName(IN name: ARRAY OF SHORTCHAR);
		VAR i: INTEGER; ch: SHORTCHAR;
	BEGIN i := 0;
		REPEAT ch := name[i]; OPM.SymWCh(ch); INC(i) UNTIL ch = 0X
	END OutName;

	PROCEDURE OutMod (mno: SHORTINT);
	BEGIN
		IF expCtxt.locmno[mno] < 0 THEN (* new mod *)
			IF GlbMod[mno].sysflag # 0 THEN OPM.SymWInt(Ssys); OPM.SymWInt(GlbMod[mno].sysflag) END;
			OPM.SymWInt(Smname);
			expCtxt.locmno[mno] := expCtxt.nofm; INC(expCtxt.nofm);
			OutName(GlbMod[mno].name^)
		ELSE OPM.SymWInt(-expCtxt.locmno[mno])
		END
	END OutMod;

	PROCEDURE ^OutStr(typ: Struct);
	PROCEDURE ^OutFlds(fld: Object; adr: INTEGER; visible: BOOLEAN);

	PROCEDURE OutHdFld(typ: Struct; fld: Object; adr: INTEGER);
		VAR i, j, n: INTEGER; btyp: Struct;
	BEGIN
		IF typ^.comp = Record THEN OutFlds(typ^.link, adr, FALSE)
		ELSIF typ^.comp = Array THEN btyp := typ^.BaseTyp; n := typ^.n;
			WHILE btyp^.comp = Array DO n := btyp^.n * n; btyp := btyp^.BaseTyp END ;
			IF (btyp^.form = Pointer) OR (btyp^.comp = Record) THEN
				j := nofhdfld; OutHdFld(btyp, fld, adr);
				IF j # nofhdfld THEN i := 1;
					WHILE (i < n) & (nofhdfld <= OPM.MaxHdFld) DO
						INC(adr, btyp^.size); OutHdFld(btyp, fld, adr); INC(i)
					END
				END
			END
		ELSIF OPM.ExpHdPtrFld & ((typ^.form = Pointer) OR (fld^.name^ = OPM.HdPtrName)) THEN
			OPM.SymWInt(Shdptr); OPM.SymWInt(adr); INC(nofhdfld)
		ELSIF OPM.ExpHdProcFld & ((typ^.form = ProcTyp) OR (fld^.name^ = OPM.HdProcName)) THEN
			OPM.SymWInt(Shdpro); OPM.SymWInt(adr); INC(nofhdfld)
		END
	END OutHdFld;

	PROCEDURE OutFlds(fld: Object; adr: INTEGER; visible: BOOLEAN);
	BEGIN
		WHILE (fld # NIL) & (fld^.mode = Fld) DO
			IF (fld^.vis # internal) & visible THEN
				IF fld^.vis = externalR THEN OPM.SymWInt(Srfld) ELSE OPM.SymWInt(Sfld) END ;
				OutStr(fld^.typ); OutName(fld^.name^); OPM.SymWInt(fld^.adr)
			ELSE OutHdFld(fld^.typ, fld, fld^.adr + adr)
			END ;
			fld := fld^.link
		END
	END OutFlds;

	PROCEDURE OutSign(result: Struct; par: Object);
	BEGIN
		OutStr(result);
		WHILE par # NIL DO
			IF par^.sysflag # 0 THEN OPM.SymWInt(Ssys); OPM.SymWInt(par^.sysflag) END;
			IF par^.mode = Var THEN OPM.SymWInt(Svalpar)
			ELSIF par^.vis = inPar THEN OPM.SymWInt(Sinpar)
			ELSIF par^.vis = outPar THEN OPM.SymWInt(Soutpar)
			ELSE OPM.SymWInt(Svarpar) END;
			OutStr(par^.typ);
			OPM.SymWInt(par^.adr);
			OutName(par^.name^); par := par^.link
		END ;
		OPM.SymWInt(Send)
	END OutSign;

	PROCEDURE OutTProcs(typ: Struct; obj: Object);
	BEGIN
		IF obj # NIL THEN
			IF obj^.mode = TProc THEN
				IF obj^.vis # internal THEN
					IF obj^.vis = externalR THEN OPM.SymWInt(Simpo) END;
					IF limAttr IN obj^.conval^.setval THEN OPM.SymWInt(Slimpro)
					ELSIF absAttr IN obj^.conval^.setval THEN OPM.SymWInt(Sabspro)
					ELSIF empAttr IN obj^.conval^.setval THEN OPM.SymWInt(Semppro)
					ELSIF extAttr IN obj^.conval^.setval THEN OPM.SymWInt(Sextpro)
					ELSE OPM.SymWInt(Stpro)
					END;
					OutSign(obj^.typ, obj^.link); OutName(obj^.name^);
					OPM.SymWInt(obj^.adr DIV 10000H)
				ELSIF OPM.ExpHdTProc THEN
					OPM.SymWInt(Shdtpro);
					OPM.SymWInt(obj^.adr DIV 10000H)
				END
			END;
			OutTProcs(typ, obj^.left);
			OutTProcs(typ, obj^.right)
		END
	END OutTProcs;

	PROCEDURE OutStr(typ: Struct);	(* OPV.TypeAlloc already applied *)
		VAR strobj: Object;
	BEGIN
		IF typ^.ref < expCtxt.ref THEN OPM.SymWInt(-typ^.ref)
		ELSE
			OPM.SymWInt(Sstruct);
			typ^.ref := expCtxt.ref; INC(expCtxt.ref);
			IF expCtxt.ref >= maxStruct THEN err(228) END ;
			OutMod(typ^.mno); strobj := typ^.strobj;
			IF (strobj # NIL) & (strobj^.name # null) THEN OutName(strobj^.name^);
				CASE strobj^.history OF
				| pbmodified: FPrintErr(strobj, 252)
				| pvmodified: FPrintErr(strobj, 251)
				| inconsistent: FPrintErr(strobj, 249)
				ELSE (* checked in OutObj or correct indirect export *)
				END
			ELSE OPM.SymWCh(0X)	(* anonymous => never inconsistent, pvfp influences the client fp *)
			END;
			IF typ^.sysflag # 0 THEN OPM.SymWInt(Ssys); OPM.SymWInt(typ^.sysflag) END ;
			CASE typ^.form OF
			| Pointer:
				OPM.SymWInt(Sptr); OutStr(typ^.BaseTyp)
			| ProcTyp:
				OPM.SymWInt(Spro); OutSign(typ^.BaseTyp, typ^.link)
			| Comp:
				CASE typ^.comp OF
				| Array:
					OPM.SymWInt(Sarr); OutStr(typ^.BaseTyp); OPM.SymWInt(typ^.n)
				| DynArr:
					OPM.SymWInt(Sdarr); OutStr(typ^.BaseTyp)
				| Record:
					IF typ^.attribute = limAttr THEN OPM.SymWInt(Slimrec)
					ELSIF typ^.attribute = absAttr THEN OPM.SymWInt(Sabsrec)
					ELSIF typ^.attribute = extAttr THEN OPM.SymWInt(Sextrec)
					ELSE OPM.SymWInt(Srec)
					END;
					IF typ^.BaseTyp = NIL THEN OutStr(notyp) ELSE OutStr(typ^.BaseTyp) END ;
					(* BaseTyp should be Notyp, too late to change *)
					OPM.SymWInt(typ^.size); OPM.SymWInt(typ^.align); OPM.SymWInt(typ^.n);
					nofhdfld := 0; OutFlds(typ^.link, 0, TRUE);
					IF nofhdfld > OPM.MaxHdFld THEN OPM.Mark(223, typ^.txtpos) END ;
					OutTProcs(typ, typ^.link); OPM.SymWInt(Send)
				END
			ELSE	(* alias structure *)
				OPM.SymWInt(Salias); OutStr(typ^.BaseTyp)
			END
		END
	END OutStr;

	PROCEDURE OutConstant(obj: Object);
		VAR f: SHORTINT; rval: SHORTREAL;
	BEGIN
		f := obj^.typ^.form; OPM.SymWInt(f);
		CASE f OF
		| Bool, Char8:
			OPM.SymWCh(SHORT(CHR(obj^.conval^.intval)))
		| Char16:
			OPM.SymWCh(SHORT(CHR(obj.conval.intval MOD 256)));
			OPM.SymWCh(SHORT(CHR(obj.conval.intval DIV 256)))
		| Byte, SInt, Int, LInt:
			OPM.SymWInt(obj^.conval^.intval)
		| Set:
			OPM.SymWSet(obj^.conval^.setval)
		| Real:
			rval := SHORT(obj^.conval^.realval); OPM.SymWReal(rval)
		| LReal:
			OPM.SymWLReal(obj^.conval^.realval)
		| String8, String16:
			OutName(obj^.conval^.ext^)
		| NilTyp:
		ELSE err(127)
		END
	END OutConstant;

	PROCEDURE OutObj(obj: Object);
		VAR i, j: INTEGER; ext: ConstExt;
	BEGIN
		IF obj # NIL THEN
			OutObj(obj^.left);
			IF obj^.mode IN {Con, Typ, Var, VarPar, LProc, XProc, CProc, IProc} THEN
				IF obj^.history = removed THEN FPrintErr(obj, 250)
				ELSIF obj^.vis IN {external, externalR} THEN
					CASE obj^.history OF
					| inserted: FPrintErr(obj, 253)
					| same:	(* ok *)
					| pbmodified: FPrintErr(obj, 252)
					| pvmodified: FPrintErr(obj, 251)
					END;
					IF obj^.sysflag # 0 THEN OPM.SymWInt(Ssys); OPM.SymWInt(obj^.sysflag) END;
					IF obj^.mode IN {LProc, XProc, Var} THEN	(* name alias for types handled in OutStr *)
						IF obj^.entry # NIL THEN OPM.SymWInt(Sentry); OutName(obj^.entry) END
					END;
					CASE obj^.mode OF
					| Con:
						OutConstant(obj); OutName(obj^.name^)
					| Typ:
						IF obj^.typ^.strobj = obj THEN OPM.SymWInt(Stype); OutStr(obj^.typ)
						ELSE OPM.SymWInt(Salias); OutStr(obj^.typ); OutName(obj^.name^)
						END
					| Var, VarPar:
						IF obj^.vis = externalR THEN OPM.SymWInt(Srvar) ELSE OPM.SymWInt(Svar) END;
						OutStr(obj^.typ); OutName(obj^.name^);
						IF (obj^.typ^.strobj = NIL) OR (obj^.typ^.strobj^.name = null) THEN
							(* compute fingerprint to avoid structural type equivalence *)
							OPM.FPrint(expCtxt.reffp, obj^.typ^.ref)
						END
					| XProc:
						OPM.SymWInt(Sxpro); OutSign(obj^.typ, obj^.link); OutName(obj^.name^)
					| IProc:
						OPM.SymWInt(Sipro); OutSign(obj^.typ, obj^.link); OutName(obj^.name^)
					| CProc:
						OPM.SymWInt(Scpro); OutSign(obj^.typ, obj^.link); ext := obj^.conval^.ext;
						i := 0; IF ext # NIL THEN j := LEN(ext^) ELSE j := 0 END; OPM.SymWInt(j);
						WHILE i < j DO OPM.SymWCh(ext^[i]); INC(i) END;
						OutName(obj^.name^)
					END
				END
			END ;
			OutObj(obj^.right)
		END
	END OutObj;

	PROCEDURE Export*(VAR ext, new: BOOLEAN);
			VAR i: SHORTINT; nofmod: BYTE; done: BOOLEAN; oldCSum: INTEGER;
	BEGIN
		symExtended := FALSE; symNew := FALSE; nofmod := nofGmod;
		Import("@self", SelfName, done); nofGmod := nofmod;
		oldCSum := OPM.checksum;
		IF OPM.noerr THEN	(* ~OPM.noerr => ~done *)
			OPM.NewSym(SelfName);
			IF OPM.noerr THEN
				OPM.checksum := 0;	(* start checksum here to avoid problems with proc id fixup *)
				IF {OPM.foreign, OPM.noinit} * OPM.opt # {} THEN
					OPM.SymWInt(Ssys); OPM.SymWInt(noInit)
				ELSIF OPM.close IN OPM.opt THEN
					OPM.SymWInt(Ssys); OPM.SymWInt(doClose)
				END;
				OPM.SymWInt(Smname); OutName(SelfName);
				expCtxt.reffp := 0; expCtxt.ref := FirstRef;
				expCtxt.nofm := 1; expCtxt.locmno[0] := 0;
				i := 1; WHILE i < maxImps DO expCtxt.locmno[i] := -1; INC(i) END ;
				OutObj(topScope^.right);
				ext := sfpresent & symExtended; new := ~sfpresent OR symNew OR (OPM.checksum # oldCSum);
				IF OPM.noerr & sfpresent & (impCtxt.reffp # expCtxt.reffp) THEN
					new := TRUE;
					IF ~extsf THEN err(155) END
				END ;
				newsf := FALSE; symNew := FALSE;	(* because of call to FPrintErr from OPL *)
				IF ~OPM.noerr OR findpc THEN OPM.DeleteNewSym END
				(* OPM.RegisterNewSym is called in OP2 after writing the object file *)
			END
		END
	END Export;	(* no new symbol file if ~OPM.noerr or findpc *)


	PROCEDURE InitStruct(VAR typ: Struct; form: BYTE);
	BEGIN
		typ := NewStr(form, Basic); typ^.ref := form; typ^.size := 1 (*OPM.ByteSize*); typ^.allocated := TRUE;
		typ^.strobj := NewObj(); typ^.pbfp := form; typ^.pvfp := form; typ^.fpdone := TRUE;
		typ^.idfp := form; typ^.idfpdone := TRUE
	END InitStruct;

	PROCEDURE EnterBoolConst (IN name, lowcase: ARRAY OF SHORTCHAR; value: INTEGER);
		VAR obj: Object;
	BEGIN
		Insert2(name, lowcase, obj); obj^.conval := NewConst();
		obj^.mode := Con; obj^.typ := booltyp; obj^.conval^.intval := value
	END EnterBoolConst;

	PROCEDURE EnterRealConst (IN name, lowcase: ARRAY OF SHORTCHAR; val: REAL; VAR obj: Object);
	BEGIN
		Insert2(name, lowcase, obj); obj^.conval := NewConst();
		obj^.mode := Con; obj^.typ := realtyp; obj^.conval^.realval := val
	END EnterRealConst;

	PROCEDURE EnterTyp (IN name, lowcase: ARRAY OF SHORTCHAR; form: BYTE; size: SHORTINT; VAR res: Struct);
		VAR obj: Object; typ: Struct;
	BEGIN
		Insert2(name, lowcase, obj);
		typ := NewStr(form, Basic); obj^.mode := Typ; obj^.typ := typ; obj^.vis := external;
		typ^.strobj := obj; typ^.size := size; typ^.ref := form; typ^.allocated := TRUE;
		typ^.pbfp := form; typ^.pvfp := form; typ^.fpdone := TRUE;
		typ^.idfp := form; typ^.idfpdone := TRUE; res := typ
	END EnterTyp;

	PROCEDURE EnterTypeAlias (IN name, lowcase: ARRAY OF SHORTCHAR; VAR res: Object; typ: Struct);
		VAR obj: Object;
	BEGIN
		Insert2(name, lowcase, obj); obj^.mode := Typ; obj^.typ := typ; obj^.vis := external;
		res := obj
	END EnterTypeAlias;

	PROCEDURE EnterProc (IN name, lowcase: ARRAY OF SHORTCHAR; num: SHORTINT);
		VAR obj: Object;
	BEGIN Insert2(name, lowcase, obj);
		obj^.mode := SProc; obj^.typ := notyp; obj^.adr := num
	END EnterProc;

	PROCEDURE EnterAttr (IN name, lowcase: ARRAY OF SHORTCHAR; num: SHORTINT);
		VAR obj: Object;
	BEGIN Insert2(name, lowcase, obj);
		obj.mode := Attr; obj.adr := num
	END EnterAttr;

	PROCEDURE EnterTProc (ptr, rec: Struct; IN name, lowcase: ARRAY OF SHORTCHAR; typ: SHORTINT);
		VAR obj, par: Object;
	BEGIN
		InsertField(name, lowcase, rec, obj);
		obj.mnolev := -128;	(* for correct implement only behaviour *)
		obj.mode := TProc; (*obj.num := num;*) obj.conval := NewConst();
		obj.conval.setval := obj.conval.setval + {newAttr};
		obj.typ := notyp; obj.vis := externalR;
		INCL(obj.conval.setval, empAttr);
		par := NewObj(); par.name := NewName("this"); par.mode := Var;
		par.adr := 8; par.typ := ptr;
		par.link := obj.link; obj.link := par;
	END EnterTProc;

	PROCEDURE InitScope (lang: SHORTCHAR);
	BEGIN
		topScope := NIL; OpenScope(0, NIL); OPM.errpos := 0;
		InitStruct(undftyp, Undef); InitStruct(notyp, NoTyp);
		InitStruct(string8typ, String8); InitStruct(niltyp, NilTyp); niltyp.size := OPM.AdrSize;
		InitStruct(string16typ, String16);
		undftyp^.BaseTyp := undftyp;

		(*initialization of module SYSTEM*)
		EnterTyp("PTR", "ptr", Pointer, OPM.AdrSize, sysptrtyp);
		IF lang <= "3" THEN
			EnterTyp("BYTE", "byte", UByte, 1, ubytetyp)
		END;
		EnterTyp("CHAR8", "char8", Char8, 1, char8typ);
		EnterTyp("CHAR16", "char16", Char16, 2, char16typ);
		EnterTyp("REAL32", "real32", Real, 4, realtyp);
		EnterTyp("REAL64", "real64", LReal, 8, lrltyp);
		IF lang # "3" THEN
			EnterTyp("INT8", "int8", Byte, 1, bytetyp);
			EnterTyp("INT16", "int16", SInt, 2, sinttyp);
			EnterTyp("INT32", "int32", Int, 4, inttyp);
			EnterTyp("INT64", "int64", LInt, 8, linttyp);
			CASE OPM.AdrSize OF
			| 2: EnterTypeAlias("ADRINT", "adrint", adrint, sinttyp);
			| 4: EnterTypeAlias("ADRINT", "adrint", adrint, inttyp);
			ELSE EnterTypeAlias("ADRINT", "adrint", adrint, linttyp)
			END
		END;
		EnterProc("ADR", "adr", adrfn);
		EnterProc("TYP", "typ", typfn);
		EnterProc("LSH", "lsh", lshfn);
		EnterProc("ROT", "rot", rotfn);
		EnterProc("GET", "get", getfn);
		EnterProc("PUT", "put", putfn);
		EnterProc("BIT", "bit", bitfn);
		EnterProc("VAL", "val", valfn);
		EnterProc("NEW", "new", sysnewfn);
		EnterProc("MOVE", "move", movefn);
		EnterProc("THISREC", "thisrec", thisrecfn);
		EnterProc("THISARR", "thisarr", thisarrfn);
		IF lang > "2" THEN
			EnterProc("COPY", "copy", copyfn)
		ELSE
			EnterProc("LCHR", "lchr", lchrfn)
		END;
		IF lang = "7" THEN
			EnterProc("SIZE", "size", sizefn);
			EnterProc("SHORT", "short", shortfn);
			EnterProc("LONG", "long", longfn)
		ELSE
			EnterProc("PACK", "pack", packfn);
			EnterProc("UNPK", "unpk", unpkfn)
		END;
		syslink := topScope^.right;
		universe := topScope; topScope^.right := NIL;

		IF lang = "C" THEN
			EnterTyp("ANYREC", "anyrec", AnyRec, 0, anytyp)
		ELSE
			EnterTyp("ANYREC@", "anyrec@", AnyRec, 0, anytyp)
		END;
		anytyp.form := Comp; anytyp.comp := Record; anytyp.n := 1;
		anytyp.BaseTyp := NIL; anytyp.extlev := -1;	(* !!! *)
		anytyp.attribute := absAttr;
		IF lang = "C" THEN
			EnterTyp("ANYPTR", "anyptr", AnyPtr, OPM.AdrSize, anyptrtyp)
		ELSE
			EnterTyp("ANYPTR@", "anyptr@", AnyPtr, OPM.AdrSize, anyptrtyp)
		END;
		anyptrtyp.form := Pointer; anyptrtyp.BaseTyp := anytyp;
		EnterTProc(anyptrtyp, anytyp, "FINALIZE", "finalize", 0);

		EnterTyp("BOOLEAN", "boolean", Bool, 1, booltyp);
		EnterTyp("SET", "set", Set, OPM.SetSize, settyp);
		IF lang = "C" THEN
			EnterTypeAlias("SHORTCHAR", "shortchar", char8, char8typ);
			EnterTypeAlias("CHAR", "char", char16, char16typ);
			EnterTypeAlias("BYTE", "byte", int8, bytetyp);
			EnterTypeAlias("SHORTINT", "shortint", int16, sinttyp);
			EnterTypeAlias("INTEGER", "integer", int32, inttyp);
			EnterTypeAlias("LONGINT", "longint", int64, linttyp);
			EnterTypeAlias("SHORTREAL", "shortreal", real32, realtyp);
			EnterTypeAlias("REAL", "real", real64, lrltyp);
			EnterTyp("UBYTE", "ubyte", UByte, 1, ubytetyp)
		ELSIF lang = "3" THEN
			EnterTyp("CHAR", "char", Char8, 1, char8typ);
			EnterTypeAlias("LONGCHAR", "longchar", char16, char16typ);
			EnterTyp("INT8", "int8", Byte, 1, bytetyp);
			EnterTyp("INT16", "int16", SInt, 2, sinttyp);
			EnterTyp("INT32", "int32", Int, 4, inttyp);
			EnterTyp("INT64", "int64", LInt, 8, linttyp);
			EnterTypeAlias("REAL", "real", real32, realtyp);
			IF OPM.AdrSize = 2 THEN
				EnterTypeAlias("INTEGER", "integer", int16, sinttyp)
			ELSE
				EnterTypeAlias("INTEGER", "integer", int32, inttyp)
			END;
			CASE OPM.AdrSize OF
			| 2: EnterTypeAlias("ADRINT", "adrint", adrint, sinttyp);
			| 4: EnterTypeAlias("ADRINT", "adrint", adrint, inttyp);
			ELSE EnterTypeAlias("ADRINT", "adrint", adrint, linttyp)
			END
		ELSIF lang <= "2" THEN
			EnterTypeAlias("CHAR", "char", char8, char8typ);
			EnterTypeAlias("SHORTINT", "shortint", int8, bytetyp);
			EnterTypeAlias("INTEGER", "integer", int16, sinttyp);
			EnterTypeAlias("LONGINT", "longint", int32, inttyp);
			EnterTypeAlias("HUGEINT", "hugeint", int64, linttyp);
			EnterTypeAlias("REAL", "real", real32, realtyp);
			EnterTypeAlias("LONGREAL", "longreal", real64, lrltyp);
		ELSE (* "7" *)
			EnterTyp("BYTE", "byte", UByte, 1, ubytetyp);
			IF OPM.widechar IN OPM.opt THEN
				EnterTypeAlias("CHAR", "char", char16, char16typ)
			ELSE
				EnterTypeAlias("CHAR", "char", char8, char8typ)
			END;
			IF OPM.AdrSize = 2 THEN
				EnterTypeAlias("INTEGER", "integer", int16, sinttyp);
				EnterTypeAlias("LONGINT", "longint", int16, sinttyp)
			ELSE
				EnterTypeAlias("INTEGER", "integer", int32, inttyp);
				EnterTypeAlias("LONGINT", "longint", int32, inttyp)
			END;
			EnterTypeAlias("REAL", "real", real32, realtyp)
		END;
		string8typ.BaseTyp := char8typ;
		string16typ.BaseTyp := char16typ;
		EnterBoolConst("FALSE", "false", 0);	(* 0 and 1 are compiler internal representation only *)
		EnterBoolConst("TRUE", "true",  1);
		IF lang = "C" THEN
			EnterRealConst("INF", "inf", OPM.InfReal, infinity)
		ELSE
			EnterRealConst("INF@", "inf@", OPM.InfReal, infinity)
		END;
		EnterProc("HALT", "halt", haltfn);
		EnterProc("NEW", "new", newfn);
		EnterProc("ABS", "abs", absfn);
		EnterProc("CAP", "cap", capfn);
		EnterProc("ORD", "ord", ordfn);
		IF lang = "7" THEN
			EnterProc("ASR", "asr", asrfn);
			EnterProc("LSL", "lsl", lslfn);
			EnterProc("ROR", "ror", rorfn);
			EnterProc("FLOOR", "floor", entierfn);
			EnterProc("FLT", "flt", fltfn);
			EnterProc("PACK", "pack", packfn);
			EnterProc("UNPK", "unpk", unpkfn);
			IF OPM.widechar IN OPM.opt THEN
				EnterProc("CHR", "chr", lchrfn)
			ELSE
				EnterProc("CHR", "chr", chrfn)
			END
		ELSE
			EnterProc("ASH", "ash", ashfn);
			EnterProc("ENTIER", "entier", entierfn);
			EnterProc("MIN", "min", minfn);
			EnterProc("MAX", "max", maxfn);
			EnterProc("SHORT", "short", shortfn);
			EnterProc("LONG", "long", longfn);
			EnterProc("SIZE", "size", sizefn);
			IF lang = "C" THEN EnterProc("CHR", "chr", lchrfn)
			ELSE
				EnterProc("CHR", "chr", chrfn);
				IF lang = "3" THEN EnterProc("LCHR", "lchr", lchrfn) END
			END
		END;
		EnterProc("ODD", "odd", oddfn);
		EnterProc("INC", "inc", incfn);
		EnterProc("DEC", "dec", decfn);
		EnterProc("INCL", "incl", inclfn);
		EnterProc("EXCL", "excl", exclfn);
		EnterProc("LEN", "len", lenfn);
		IF lang <= "2" THEN
			EnterProc("COPY", "copy", copyfn)
		END;
		EnterProc("ASSERT", "assert", assertfn);
		IF (lang = "C") OR (lang = "3") THEN
			EnterProc("BITS", "bits", bitsfn);
			EnterProc("USHORT", "ushort", ushortfn)
		END;
		EnterAttr("ABSTRACT", "abstract", absAttr);
		EnterAttr("LIMITED", "limited", limAttr);
		EnterAttr("EMPTY", "empty", empAttr);
		EnterAttr("EXTENSIBLE", "extensible", extAttr);
		impCtxt.ref[Undef] := undftyp; impCtxt.ref[Byte] := bytetyp;
		impCtxt.ref[Bool] := booltyp;  impCtxt.ref[Char8] := char8typ;
		impCtxt.ref[SInt] := sinttyp;  impCtxt.ref[Int] := inttyp;
		impCtxt.ref[LInt] := linttyp;  impCtxt.ref[Real] := realtyp;
		impCtxt.ref[LReal] := lrltyp;  impCtxt.ref[Set] := settyp;
		impCtxt.ref[String8] := string8typ; impCtxt.ref[NilTyp] := niltyp;
		impCtxt.ref[NoTyp] := notyp; impCtxt.ref[Pointer] := sysptrtyp;
		impCtxt.ref[AnyPtr] := anyptrtyp; impCtxt.ref[AnyRec] := anytyp;
		impCtxt.ref[Char16] := char16typ; impCtxt.ref[String16] := string16typ;
		impCtxt.ref[UByte] := ubytetyp
	END InitScope;

	PROCEDURE Init*(IN name: ARRAY OF SHORTCHAR; lang: SHORTCHAR; opt: SET);
		CONST nsf = 4; fpc = 8; esf = 9;
	BEGIN
		InitScope(lang);
		topScope := universe; OpenScope(0, NIL); SYSimported := FALSE;
		SelfName := name$; topScope^.name := NewName(name);
		GlbMod[0] := topScope; nofGmod := 1;
		newsf := nsf IN opt; findpc := fpc IN opt; extsf := newsf OR (esf IN opt); sfpresent := TRUE;
		infinity^.conval^.intval := OPM.ConstNotAlloc;
		depth := 0
	END Init;

BEGIN
	NEW(null, 1); null^ := ""
END OfrontOPT.

Objects:

    mode  | adr    conval  link     scope    leaf
   ------------------------------------------------
    Undef |                                         Not used
    Var   | vadr           next              regopt Glob or loc var or proc value parameter
    VarPar| vadr           next              regopt Procedure var parameter (vis = 0 | inPar | outPar)
    Con   |        val                              Constant
    Fld   | off            next                     Record field
    Typ   |                                         Named type
    LProc | entry  sizes   firstpar scope    leaf   Local procedure, entry adr set in back-end
    XProc | entry  sizes   firstpar scope    leaf   External procedure, entry adr set in back-end
    SProc | fno    sizes                            Standard procedure
    CProc |        code    firstpar scope           Code procedure
    IProc | entry  sizes            scope    leaf   Interrupt procedure, entry adr set in back-end
    Mod   |                         scope           Module
    Head  | txtpos         owner    firstvar        Scope anchor
    TProc | index  sizes   firstpar scope    leaf   Bound procedure, index = 10000H*mthno+entry, entry adr set in back-end

		Structures:

    form     comp  | n      BaseTyp   link     mno  txtpos   sysflag
	--------------------------------------------------------------------------
    Undef    Basic |
    Byte     Basic |
    Bool     Basic |
    Char8    Basic |
    SInt     Basic |
    Int      Basic |
    LInt     Basic |
    Real     Basic |
    LReal    Basic |
    Set      Basic |
    String8  Basic |
    NilTyp   Basic |
    NoTyp    Basic |
    Pointer  Basic |        PBaseTyp           mno  txtpos   sysflag
    ProcTyp  Basic |        ResTyp    params   mno  txtpos   sysflag
    Comp     Array | nofel  ElemTyp            mno  txtpos   sysflag
    Comp     DynArr| dim    ElemTyp            mno  txtpos   sysflag
    Comp     Record| nofmth RBaseTyp  fields   mno  txtpos   sysflag
    Char16   Basic |
    String16 Basic |

Nodes:

design   = Nvar|Nvarpar|Nfield|Nderef|Nindex|Nguard|Neguard|Ntype|Nproc.
expr     = design|Nconst|Nupto|Nmop|Ndop|Ncall.
nextexpr = NIL|expr.
ifstat   = NIL|Nif.
casestat = Ncaselse.
sglcase  = NIL|Ncasedo.
stat     = NIL|Ninittd|Nenter|Nassign|Ncall|Nifelse|Ncase|Nwhile|Nrepeat|
           Nloop|Nexit|Nreturn|Nwith|Ntrap.


              class     subcl     obj      left      right     link
              ---------------------------------------------------------

design        Nvar                var                          nextexpr
              Nvarpar             varpar                       nextexpr
              Nfield              field    design              nextexpr
              Nderef                       design              nextexpr
              Nindex                       design    expr      nextexpr
              Nguard                       design              nextexpr (typ = guard type)
              Neguard                      design              nextexpr (typ = guard type)
              Ntype               type                         nextexpr
              Nproc     normal    proc                         nextexpr
                        super     proc                         nextexpr


expr          design
              Nconst              const                                 (val = node^.conval)
              Nupto                        expr      expr      nextexpr
              Nmop      not                expr                nextexpr
                        minus              expr                nextexpr
                        is        tsttype  expr                nextexpr
                        conv               expr                nextexpr
                        abs                expr                nextexpr
                        cap                expr                nextexpr
                        odd                expr                nextexpr
                        adr                expr                nextexpr SYSTEM.ADR
                        val                expr                nextexpr SYSTEM.VAL
              Ndop      times              expr      expr      nextexpr
                        slash              expr      expr      nextexpr
                        div                expr      expr      nextexpr
                        div0               expr      expr      nextexpr
                        mod                expr      expr      nextexpr
                        rem0               expr      expr      nextexpr
                        and                expr      expr      nextexpr
                        plus               expr      expr      nextexpr
                        minus              expr      expr      nextexpr
                        or                 expr      expr      nextexpr
                        eql                expr      expr      nextexpr
                        neq                expr      expr      nextexpr
                        lss                expr      expr      nextexpr
                        leq                expr      expr      nextexpr
                        grt                expr      expr      nextexpr
                        geq                expr      expr      nextexpr
                        in                 expr      expr      nextexpr
                        ash                expr      expr      nextexpr
                        msk                expr      Nconst    nextexpr
                        len                design    Nconst    nextexpr
                        bit                expr      expr      nextexpr SYSTEM.BIT
                        lsh                expr      expr      nextexpr SYSTEM.LSH
                        rot                expr      expr      nextexpr SYSTEM.ROT
              Ncall               fpar     design    nextexpr  nextexpr

nextexpr      NIL
              expr

ifstat        NIL
              Nif                          expr      stat      ifstat

casestat      Ncaselse                     sglcase   stat            (minmax = node^.conval)

sglcase       NIL
              Ncasedo                      Nconst    stat      sglcase

stat          NIL
              Ninittd                                          stat     (of node^.typ)
              Nenter              proc     stat      stat      stat     (proc=NIL for mod)
              Nassign   assign             design    expr      stat
                        newfn              design              stat
                        incfn              design    expr      stat
                        decfn              design    expr      stat
                        inclfn             design    expr      stat
                        exclfn             design    expr      stat
                        copyfn             design    expr      stat
                        getfn              design    expr      stat     SYSTEM.GET
                        putfn              expr      expr      stat     SYSTEM.PUT
                        sysnewfn           design    expr      stat     SYSTEM.NEW
                        movefn             expr      expr      stat     SYSTEM.MOVE
                                                                        (right^.link = 3rd par)
              Ncall               fpar     design    nextexpr  stat
              Nifelse                      ifstat    stat      stat
              Ncase                        expr      casestat  stat
              Nwhile                       expr      stat      stat
              Nrepeat                      stat      expr      stat
              Nloop                        stat                stat
              Nexit                                            stat
              Nreturn             proc     nextexpr            stat     (proc = NIL for mod)
              Nwith                        ifstat    stat      stat
              Ntrap                                  expr      stat
