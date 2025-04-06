MODULE BrowserCmd;	(* RC 29.10.93 *)	(* object model 4.12.93, command line version jt 4.4.95 *)

	IMPORT
		OPM := OfrontOPM, OPS := OfrontOPS, OPT := OfrontOPT, OPV := OfrontOPV,
		Texts, Console, CmdArgs;

	CONST
		OptionChar = "-";
		(* object modes *)
		Var = 1; VarPar = 2; Con = 3; Fld = 4; Typ = 5; LProc = 6; XProc = 7;
		SProc = 8; CProc = 9; IProc = 10; Mod = 11; Head = 12; TProc = 13;

		(* structure forms *)
		Undef = 0; Byte = 1; Bool = 2; Char8 = 3; SInt = 4; Int = 5; LInt = 6;
		Real = 7; LReal = 8; Set = 9; String8 = 10; NilTyp = 11; NoTyp = 12;
		Pointer = 13; UByte = 14; ProcTyp = 15; Comp = 16;
		Char16 = 17; String16 = 18;

		(* composite structure forms *)
		Basic = 1; Array = 2; DynArr = 3; Record = 4;

		(* attribute flags (attr.adr, struct.attribute, proc.conval.setval) *)
		newAttr = 16; absAttr = 17; limAttr = 18; empAttr = 19; extAttr = 20;

		(* module visibility of objects *)
		internal = 0; external = 1; externalR = 2; inPar = 3; outPar = 4;

		(* sysflags *)
		nilBit = 1; notag = 1; noalign = 3; align2 = 4; align4 = 5; align8 = 6; union = 7;
		ccall = 0; stdcall = 1; fastcall = 2; inline = 3;

		(* symbol file items *)
		Smname = 16; Send = 18; Stype = 19; Salias = 20; Svar = 21; Srvar = 22;
		Svalpar = 23; Svarpar = 24; Sfld = 25; Srfld = 26; Shdptr = 27; Shdpro = 28; Stpro = 29; Shdtpro = 30;
		Sxpro = 31; Sipro = 32; Scpro = 33; Sstruct = 34; Ssys = 35; Sptr = 36; Sarr = 37; Sdarr = 38; Srec = 39; Spro = 40;

	VAR
		W: Texts.Writer;
		lang, option: SHORTCHAR;
		hex: ARRAY 17 OF SHORTCHAR;
		global: RECORD
			level: INTEGER;	(* current indentation level *)
			gap: BOOLEAN	(* request for lazy Ln, issued with next Indent *)
		END;

	PROCEDURE Ws(IN s: ARRAY OF SHORTCHAR); BEGIN Texts.WriteString(W, s) END Ws;
	PROCEDURE Wch(ch: SHORTCHAR); BEGIN Texts.Write(W, ch) END Wch;
	PROCEDURE Wi(i: LONGINT); BEGIN Texts.WriteLongInt(W, i, 0) END Wi;
	PROCEDURE Wln; BEGIN Texts.WriteLn(W) END Wln;
	PROCEDURE Whex(i: INTEGER); BEGIN Texts.WriteHex(W, i) END Whex;

	PROCEDURE StringConst (IN s: ARRAY OF SHORTCHAR);
		VAR ch: SHORTCHAR; i: INTEGER; quoted, first: BOOLEAN;
	BEGIN
		ch := s[0]; i := 1; quoted := FALSE; first := TRUE;
		WHILE ch # 0X DO
			IF ch = 22X THEN quoted := TRUE
			ELSIF (ch < " ") OR (ch = 27X) OR (ch >= 7FX) THEN first := FALSE
			END;
			ch := s[i]; INC(i)
		END;
		IF quoted & first THEN Wch(27X); Ws(s); Wch(27X)
		ELSE
			ch := s[0]; i := 1; quoted := FALSE; first := TRUE;
			WHILE ch # 0X DO
				IF (ch < " ") OR (ch = 22X) OR (ch >= 7FX) THEN
					IF quoted THEN Wch(22X) END;
					IF ~first THEN Ws(" + ") END;
					IF ch > 9FX THEN Wch("0") END;
					Wch(hex[ORD(ch) DIV 10H]); Wch(hex[ORD(ch) MOD 10H]); Wch("X");
					quoted := FALSE
				ELSE
					IF ~quoted THEN
						IF ~first THEN Ws(" + ") END;
						Wch(22X)
					END;
					Wch(ch);
					quoted := TRUE
				END;
				ch := s[i]; INC(i);
				first := FALSE
			END;
			IF quoted THEN Wch(22X) END
		END
	END StringConst;

	PROCEDURE Indent (i: INTEGER);
	BEGIN
		IF global.gap THEN global.gap := FALSE; Wln END;
		WHILE i > 0 DO Wch(" "); Wch(" "); DEC(i) END
	END Indent;

	PROCEDURE ^Wtype(typ: OPT.Struct);
	PROCEDURE ^Wstruct(typ: OPT.Struct);

	PROCEDURE Wsign(result: OPT.Struct; par: OPT.Object);
		VAR paren, res, first: BOOLEAN;
	BEGIN first := TRUE;
		res := (result # NIL) (* hidden mthd *) & (result # OPT.notyp);
		paren := res OR (par # NIL);
		IF paren THEN Ws(" (") END ;
		WHILE par # NIL DO
			IF ~first THEN Ws("; ") ELSE first := FALSE END;
			IF option = "x" THEN Wi(par^.adr); Wch(" ") END;
			IF par^.mode = VarPar THEN
				IF lang = "C" THEN
					IF par^.vis = inPar THEN Ws("IN ") ELSIF par^.vis = outPar THEN Ws("OUT ") ELSE Ws("VAR ") END
				ELSIF par^.vis # inPar THEN Ws("VAR ")
				END;
				IF ODD(par^.sysflag DIV nilBit) THEN Ws("[nil] ") END
			END;
			Ws(par^.name^);
			IF (lang <= "2") & (par^.vis = inPar) THEN Wch("-") END;
			WHILE (par^.link # NIL) & (par^.link^.typ = par^.typ) & (par^.link^.mode = par^.mode)
					& (par^.link^.vis = par^.vis) & (par^.link^.sysflag = par^.sysflag) DO
				par := par^.link; Ws(", "); Ws(par^.name^)
			END;
			Ws(": "); Wtype(par^.typ);
			par := par^.link
		END;
		IF paren THEN Wch(")") END ;
		IF res THEN Ws(": "); Wtype(result) END
	END Wsign;

	PROCEDURE Objects(obj: OPT.Object; mode: SET);
		VAR i, m: INTEGER; s: SET; ext: OPT.ConstExt;
	BEGIN
		IF obj # NIL THEN
			Objects(obj^.left, mode);
			IF obj^.mode IN mode THEN
				CASE obj^.mode OF
				| Con:
						Indent(2); Ws(obj^.name^); Ws(" = ");
						CASE obj^.typ^.form OF
						| Bool:
								IF obj^.conval^.intval = 1 THEN Ws("TRUE") ELSE Ws("FALSE") END
						| Char8, Char16:
								IF obj^.conval^.intval = 22H THEN Wch("'"); Wch('"'); Wch("'")
								ELSIF (obj^.conval^.intval >= 32) & (obj^.conval^.intval <= 126) THEN
									Wch(22X); Wch(SHORT(CHR(obj^.conval^.intval))); Wch(22X)
								ELSE
									Whex(SHORT(obj^.conval^.intval)); Wch("X")
								END
						| Byte, SInt, Int, LInt:
								Wi(obj^.conval^.intval)
						| Set:
								Wch("{"); i := 0; s := obj^.conval^.setval;
								WHILE i <= MAX(SET) DO
									IF i IN s THEN Wi(i); EXCL(s, i);
										IF s # {} THEN Ws(", ") END
									END ;
									INC(i)
								END ;
								Wch("}")
						| Real:
								Texts.WriteLongReal(W, obj^.conval^.realval, 8)
						| LReal:
								Texts.WriteLongReal(W, obj^.conval^.realval, 17)
						| String8, String16:
								StringConst(obj^.conval^.ext^)
						| NilTyp:
								Ws("NIL")
						END ;
						Wch(";"); Wln
				| Typ:
						IF obj^.name # OPT.null THEN Indent(2);
							IF obj^.typ^.strobj = obj THEN	(* canonical name *)
								Wtype(obj^.typ); Ws(" = "); Wstruct(obj^.typ)
							ELSE	(* alias *)
								Ws(obj^.name^); Ws(" = "); Wtype(obj^.typ)
							END ;
							Wch(";"); Wln
						END
				| Var:
						Indent(2); Ws(obj^.name^);
						IF (lang # "7") & (obj^.vis = externalR) THEN Ws("-: ") ELSE Ws(": ") END;
						Wtype(obj^.typ); Wch(";"); Wln
				| XProc, CProc, IProc:
						IF (obj^.mode # CProc) OR (obj^.sysflag >= 0) THEN
							Indent(1); Ws("PROCEDURE");
							IF obj^.mode = IProc THEN Wch("+")
							ELSIF (option = "x") & (obj^.mode = CProc) THEN Wch("-")
							END;
							Wch(" ");
							CASE obj^.sysflag OF
								| ccall:
								| stdcall: Ws("[stdcall] ")
								| fastcall: Ws("[fastcall] ")
								| inline: Ws("[inline] ")
							END;
							Ws(obj^.name^);
							Wsign(obj^.typ, obj^.link);
							IF obj^.mode = CProc THEN ext := obj^.conval^.ext;
								IF (option = "x") & (ext # NIL) THEN m := LEN(ext^); i := 0; Ws(' "');
									WHILE i < m DO Wch(ext^[i]); INC(i) END;
									Wch('"')
								END
							END;
							Wch(";"); Wln
						END
				END
			END;
			Objects(obj^.right, mode)
		END
	END Objects;

	PROCEDURE Wmthd(obj: OPT.Object; VAR cont: BOOLEAN);
	BEGIN
		IF obj # NIL THEN
			Wmthd(obj^.left, cont);
			IF (obj^.mode = TProc) & ((obj^.name^ # OPM.HdTProcName) OR (option = "x")) THEN
				IF cont THEN Wch(";") END;
				Wln; Indent(global.level); INC(global.level); Wch("(");
				IF obj^.name^ # OPM.HdTProcName THEN
					IF obj^.link^.mode = VarPar THEN
						IF lang = "C" THEN
							IF obj^.link^.vis = inPar THEN Ws("IN ") ELSIF obj^.link^.vis = outPar THEN Ws("OUT ") ELSE Ws("VAR ") END
						ELSIF obj^.link^.vis # inPar THEN Ws("VAR ")
						END;
						IF ODD(obj^.link^.sysflag DIV nilBit) THEN Ws("[nil] ") END
					END;
					Ws(obj^.link^.name^); Ws(": "); Wtype(obj^.link^.typ)
				END;
				Ws(") "); Ws(obj^.name^);
				Wsign(obj^.typ, obj^.link^.link);
				IF lang = "C" THEN
					IF newAttr IN BITS(obj^.link^.typ^.attribute) THEN Ws(", NEW") END;
					IF absAttr IN BITS(obj^.link^.typ^.attribute) THEN Ws(", ABSTRACT")
					ELSIF empAttr IN BITS(obj^.link^.typ^.attribute) THEN Ws(", EMPTY")
					ELSIF extAttr IN BITS(obj^.link^.typ^.attribute) THEN Ws(", EXTENSIBLE")
					END
				END;
				cont := TRUE;
				IF option = "x" THEN Indent(1);
					Ws("(* methno: "); Wi(obj^.adr DIV 10000H);  Ws(" *)")
				END;
				DEC(global.level)
			END;
			Wmthd(obj^.right, cont)
		END
	END Wmthd;

	PROCEDURE Wflds (typ: OPT.Struct; VAR cont: BOOLEAN);
		VAR fld: OPT.Object;
	BEGIN
		fld := typ^.link;
		WHILE (fld # NIL) & (fld^.mode = Fld) DO
			IF (option = "x") OR (fld^.name[0] # "@") THEN
				IF cont THEN Wch(";") END;
				Wln;
				Indent(global.level);
				IF option = "x" THEN Wi(fld^.adr); Wch(" ") END;
				Ws(fld^.name^);
				WHILE (fld.link # NIL) & (fld.link.typ = fld.typ) & (fld.link.name # NIL) DO
					IF fld.vis = externalR THEN Wch("-") END;
					fld := fld.link; Ws(", "); Ws(fld.name^)
				END;
				IF fld.vis = externalR THEN Wch("-") END;
				Ws(": "); Wtype(fld^.typ);
				cont := TRUE
			END;
			fld := fld.link
		END
	END Wflds;

	PROCEDURE Wstruct(typ: OPT.Struct);
		VAR cont: BOOLEAN;

		PROCEDURE SysFlag;
		BEGIN
			IF (option = "x") & (typ^.sysflag # 0) THEN
				Ws(" ["); Wi(typ^.sysflag); Wch("]")
			ELSE
				CASE typ^.sysflag MOD 100H OF
					| 0:
					| notag: Ws(" [notag]")
					| noalign: Ws(" [noalign]")
					| align2: Ws(" [align2]")
					| align4: Ws(" [align4]")
					| align8: Ws(" [align8]")
					| union: Ws(" [union]")
					ELSE Ws(" ["); Wi(typ^.sysflag MOD 100H); Wch("]")
				END
			END
		END SysFlag;

	BEGIN
		INC(global.level);
		CASE typ^.form OF
		| Undef:
				Ws("Undef")
		| Pointer:
				Ws("POINTER"); SysFlag; Ws(" TO ");
				DEC(global.level); Wtype(typ^.BaseTyp); INC(global.level)
		| ProcTyp:
				Ws("PROCEDURE");
				IF typ^.sysflag = 1 THEN Ws(" [stdcall]")
				ELSIF typ^.sysflag = 2 THEN Ws(" [fastcall]")
				END;
				Wsign(typ^.BaseTyp, typ^.link)
		| Comp:
				CASE typ^.comp OF
				| Array:
						Ws("ARRAY"); SysFlag; Wch(" "); Wi(typ^.n); Ws(" OF "); Wtype(typ^.BaseTyp)
				| DynArr:
						Ws("ARRAY"); SysFlag; Ws(" OF "); Wtype(typ^.BaseTyp)
				| Record:
						IF lang = "C" THEN
							IF typ^.attribute = absAttr THEN Ws("ABSTRACT ")
							ELSIF typ^.attribute = limAttr THEN Ws("LIMITED ")
							ELSIF typ^.attribute = extAttr THEN Ws("EXTENSIBLE ")
							END
						END;
						Ws("RECORD"); SysFlag;
						cont := FALSE;
						IF typ^.BaseTyp # NIL THEN Ws(" ("); Wtype(typ^.BaseTyp); Wch(")") END;
						Wflds(typ, cont); Wmthd(typ^.link, cont);
						IF cont THEN Wln; Indent(global.level - 1) ELSE Wch(" ") END;
						Ws("END");
						IF option = "x" THEN Indent(1);
							Ws("(* size: "); Wi(typ^.size); Ws(" align: "); Wi(typ^.align);
							Ws(" nofm: "); Wi(typ^.n); Ws(" *)")
						END
				END
		ELSE
			IF typ^.BaseTyp # OPT.undftyp THEN Wtype(typ^.BaseTyp) END	(* alias structures *)
		END;
		DEC(global.level)
	END Wstruct;

	PROCEDURE Wtype(typ: OPT.Struct);
		VAR obj: OPT.Object;
	BEGIN
		obj := typ^.strobj;
		IF obj^.name # OPT.null THEN
			IF typ^.mno # 0 THEN Ws(OPT.GlbMod[typ^.mno].name^); Wch(".")
			ELSIF typ = OPT.sysptrtyp THEN Ws("SYSTEM.")
			ELSIF obj^.vis = internal THEN Wch("#")
			END;
			IF lang = "C" THEN
				IF obj = OPT.char8typ^.strobj THEN Ws("SHORTCHAR")
				ELSIF obj = OPT.char16typ^.strobj THEN Ws("CHAR")
				ELSIF obj = OPT.bytetyp^.strobj THEN Ws("BYTE")
				ELSIF obj = OPT.sinttyp^.strobj THEN Ws("SHORTINT")
				ELSIF obj = OPT.inttyp^.strobj THEN Ws("INTEGER")
				ELSIF obj = OPT.linttyp^.strobj THEN Ws("LONGINT")
				ELSIF obj = OPT.realtyp^.strobj THEN Ws("SHORTREAL")
				ELSIF obj = OPT.lrltyp^.strobj THEN Ws("REAL")
				ELSIF obj = OPT.ubytetyp^.strobj THEN Ws("UBYTE")
				ELSE Ws(obj^.name^)
				END
			ELSIF lang = "3" THEN
				IF obj = OPT.char8typ^.strobj THEN Ws("CHAR")
				ELSIF obj = OPT.char16typ^.strobj THEN Ws("LONGCHAR")
				ELSIF obj = OPT.bytetyp^.strobj THEN Ws("INT8")
				ELSIF obj = OPT.sinttyp^.strobj THEN Ws("INT16")
				ELSIF obj = OPT.inttyp^.strobj THEN Ws("INT32")
				ELSIF obj = OPT.linttyp^.strobj THEN Ws("INT64")
				ELSIF obj = OPT.realtyp^.strobj THEN Ws("REAL32")
				ELSIF obj = OPT.lrltyp^.strobj THEN Ws("REAL64")
				ELSIF obj = OPT.ubytetyp^.strobj THEN Ws("SYSTEM.BYTE")
				ELSE Ws(obj^.name^)
				END
			ELSIF lang <= "2" THEN
				IF obj = OPT.char8typ^.strobj THEN Ws("CHAR")
				ELSIF obj = OPT.bytetyp^.strobj THEN Ws("SHORTINT")
				ELSIF obj = OPT.sinttyp^.strobj THEN Ws("INTEGER")
				ELSIF obj = OPT.inttyp^.strobj THEN Ws("LONGINT")
				ELSIF obj = OPT.linttyp^.strobj THEN Ws("HUGEINT")
				ELSIF obj = OPT.realtyp^.strobj THEN Ws("REAL")
				ELSIF obj = OPT.lrltyp^.strobj THEN Ws("LONGREAL")
				ELSIF obj = OPT.ubytetyp^.strobj THEN Ws("SYSTEM.BYTE")
				ELSIF obj = OPT.char16typ^.strobj THEN Ws("SYSTEM.CHAR16")
				ELSE Ws(obj^.name^)
				END
			ELSE (* "7" *)
				IF obj = OPT.char8typ^.strobj THEN Ws("CHAR")
				ELSIF obj = OPT.ubytetyp^.strobj THEN Ws("BYTE")
				ELSIF obj = OPT.bytetyp^.strobj THEN Ws("SYSTEM.INT8")
				ELSIF obj = OPT.sinttyp^.strobj THEN Ws("SYSTEM.INT16")
				ELSIF obj = OPT.inttyp^.strobj THEN Ws("INTEGER")
				ELSIF obj = OPT.linttyp^.strobj THEN Ws("SYSTEM.INT64")
				ELSIF obj = OPT.realtyp^.strobj THEN Ws("REAL")
				ELSIF obj = OPT.lrltyp^.strobj THEN Ws("SYSTEM.REAL64")
				ELSIF obj = OPT.char16typ^.strobj THEN Ws("SYSTEM.CHAR16")
				ELSE Ws(obj^.name^)
				END
			END
		ELSE
			IF (option = "x") & (typ^.ref > OPM.MaxStruct) THEN Wch("#"); Wi(typ^.ref - OPM.MaxStruct); Wch(" ") END;
			Wstruct(typ)
		END
	END Wtype;

	PROCEDURE WModule (IN name: OPS.Name; T: Texts.Text);
		VAR i, beg, end: INTEGER; first, done: BOOLEAN;

		PROCEDURE Header (IN s: ARRAY OF SHORTCHAR);
		BEGIN
			beg := W.buf.len; Indent(1); Ws(s); Wln; end := W.buf.len; global.level := 2
		END Header;

		PROCEDURE CheckHeader;
		 	VAR len: INTEGER;
		BEGIN
			global.gap := TRUE;
			len := T.len;
			IF end = W.buf.len THEN
				Texts.Append(T, W.buf); Texts.Delete(T, len+beg, len+end)
			END
		END CheckHeader;

	BEGIN
		OPT.Import("@notself", name, done);
		IF done THEN
			Ws("DEFINITION "); Ws(name); Ws("; (* ");
			CASE lang OF
			| "1": Ws("Oberon")
			| "2": Ws("Oberon-2")
			| "7": Ws("Oberon-07")
			| "C": Ws("Component Pascal")
			| "3": Ws("Oberon-3")
			END;
			Ws(" *)"); Wln; global.gap := TRUE;
			Header("IMPORT"); i := 1; first := TRUE;
			WHILE i < OPT.nofGmod DO
				IF first THEN first := FALSE; Indent(2) ELSE Ws(", ") END;
				Ws(OPT.GlbMod[i].name^);
				INC(i)
			END;
			IF ~first THEN Wch(";"); Wln END;
			CheckHeader;
			Header("CONST"); Objects(OPT.GlbMod[0].right, {Con}); CheckHeader;
			Header("TYPE"); Objects(OPT.GlbMod[0].right, {Typ}); CheckHeader;
			Header("VAR"); Objects(OPT.GlbMod[0].right, {Var}); CheckHeader;
			Objects(OPT.GlbMod[0].right, {XProc, IProc, CProc});
			Wln;
			Ws("END "); Ws(name); Wch("."); Wln; Texts.Append(T, W.buf)
		ELSE
			Texts.WriteString(W, name); Texts.WriteString(W, " -- symbol file not found");
			Texts.WriteLn(W); Texts.Append(T, W.buf)
		END
	END WModule;

	PROCEDURE Ident(VAR name, first: ARRAY OF SHORTCHAR);
		VAR i, j: SHORTINT; ch: SHORTCHAR;
	BEGIN i := 0;
		WHILE name[i] # 0X DO INC(i) END ;
		WHILE (i >= 0) & (name[i] # "/") DO DEC(i) END ;
		INC(i); j := 0; ch := name[i];
		WHILE (ch # ".") & (ch # 0X) DO first[j] := ch; INC(i); INC(j); ch := name[i] END ;
		first[j] := 0X
	END Ident;

	PROCEDURE ShowDef*;
		VAR T, dummyT: Texts.Text; S, vname, name: OPS.Name; R: Texts.Reader; ch: SHORTCHAR;
			s: ARRAY 1024 OF SHORTCHAR; i: SHORTINT;
	BEGIN
		option := 0X; CmdArgs.Get(1, S);
		IF CmdArgs.Count > 1 THEN
			IF S[0] = OptionChar THEN option := S[1]; CmdArgs.Get(2, S)
			ELSE CmdArgs.Get(2, vname); option := vname[1]
			END
		END;
		CASE option OF "1".."3", "7": lang := option; ELSE lang := "C" END;
		IF CmdArgs.Count >= 1 THEN
			Ident(S, name);
			NEW(T); Texts.Open(T, "");
			OPT.Init(name, lang, {}); OPT.SelfName := "AvoidErr154"; WModule(name, T); OPT.Close;
			Texts.OpenReader(R, T, 0); Texts.Read(R, ch); i := 0;
			WHILE ~R.eot DO
				IF ch = 0DX THEN s[i] := 0X; i := 0; Console.String(s); Console.Ln
				ELSE s[i] := ch; INC(i)
				END ;
				Texts.Read(R, ch)
			END ;
			s[i] := 0X; Console.String(s)
		END
	END ShowDef;

BEGIN
	hex := "0123456789ABCDEF";
	OPT.typSize := OPV.TypSize; Texts.OpenWriter(W);
	IF CmdArgs.Count # 0 THEN ShowDef
	ELSE
		Console.String("Ofront+ (TM) Show Definition (Browser) Tool v1.0"); Console.Ln;
		Console.String("Copyright (c) Software Templ OG, 1995-2020 & VEDAsoft Oberon Club, 2013-2025"); Console.Ln; Console.Ln;
		Console.String('  command = "showdef" [-option] module'); Console.Ln;
		Console.String('  option  = "1" | "2" | "7" | "C" | "3" | "x"'); Console.Ln; Console.Ln;
		Console.String('       "1": Oberon (1990)'); Console.Ln;
		Console.String('       "2": Oberon-2'); Console.Ln;
		Console.String('       "7": Oberon-07/16'); Console.Ln;
		Console.String('       "C": Component Pascal (by default)'); Console.Ln;
		Console.String('       "3": Oberon-3 (experimental dialect)'); Console.Ln;
		Console.String('       "x": eXtra system information'); Console.Ln
	END
END BrowserCmd.
