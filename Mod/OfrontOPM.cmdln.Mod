MODULE OfrontOPM;	(* RC 6.3.89 / 28.6.89, J.Templ 10.7.89 / 22.7.96  *)
(* constants needed for C code generation

	31.1.2007 jt synchronized with BlackBox version, in particular PromoteIntConstToLInt added
*)

	IMPORT SYSTEM, Texts, Files, Platform, Args, Console, OfrontErrors;

	CONST
		OptionChar* = "-";

		(* compiler options; don't change the encoding *)
		inxchk* = 0;	(* index check on *)
		vcpp* = 1;	(* VC++ support on; former ovflchk; neither used nor documented *)
		ranchk* = 2;	(* range check on *)
		typchk* = 3;	(* type check on *)
		newsf* = 4;	(* generation of new symbol file allowed *)
		ptrinit* = 5;	(* pointer initialization *)
		ansi* = 6;	(* ANSI or K&R style prototypes *)
		assert* = 7;	(* assert evaluation *)
		include0* = 8;	(* include M.h0 in header file and M.c0 in body file if such files exist *)
		extsf* = 9;	(* extension of old symbol file allowed *)
		mainprog* = 10;	(* translate module body into C main function *)
		defopt* = {inxchk, typchk, ptrinit, assert, ansi};	(* default options *)

		nilval* = 0;
(*
		MinRealPat = 0FF7FFFFFH;	(* most  negative, 32-bit pattern, -3.40282346E38 *)
		MinLRealPatL = 0FFFFFFFFH;	(* most  negative, lower 32-bit pattern *)
		MinLRealPatH = 0FFEFFFFFH;	(* most  negative, higher 32-bit pattern *)
		MaxRealPat = 7F7FFFFFH; (*3.40282346E38*)
		MaxLRealPatL = -1;
		MaxLRealPatH = 7FEFFFFFH;
*)

		MaxRExp* = 38;	MaxLExp* = 308;

		MinHaltNr* = 0;
		MaxHaltNr* = 255;
		MaxSysFlag* = 1;

		MaxCC* = -1;	(* SYSTEM.CC, GETREG, PUTREG; not implementable in Ofront *)
		MinRegNr* = 0;
		MaxRegNr* = -1;

		LANotAlloc* = -1;	(* XProc link adr initialization *)
		ConstNotAlloc* = -1;	(* for allocation of string and real constants *)
		TDAdrUndef* = -1;	(* no type desc allocated *)

		MaxCases* = 256;
		MaxCaseRange* = 512;

		MaxStruct* = 1282;

		(* maximal number of pointer fields in a record: *)
		MaxPtr* = MAX(INTEGER);

		(* maximal number of global pointers per module: *)
		MaxGPtr* = MAX(INTEGER);

		(* maximal number of hidden fields in an exported record: *)
		MaxHdFld* = 512;

		HdPtrName* = "@ptr";
		HdProcName* = "@proc";
		HdTProcName* = "@tproc";

		ExpHdPtrFld* = TRUE;
		ExpHdProcFld* = FALSE;
		ExpHdTProc* = FALSE;

		NEWusingAdr* = FALSE;

		Eot* = 0X;

		SFext = ".sym";	(* symbol file extension *)
		BFext = ".c";	(* body file extension *)
		HFext = ".h";	(* header file extension *)
		SFtag = 0F7X;	(* symbol file tag *)

		HeaderFile* = 0;
		BodyFile* = 1;
		HeaderInclude* = 2;

	TYPE
		FileName = ARRAY 32 OF CHAR;

	VAR
		SIntSize-, IntSize-, LIntSize-, SetSize-, RealSize-, LRealSize-,
		PointerSize-, ProcSize-, IndexSize-, MaxSet-, MaxHDig-: SHORTINT;
		MinSInt-, MinInt-, MaxSInt-, MaxInt-: INTEGER; MaxIndex-, MinLInt-, MaxLInt-: LONGINT;
		MinReal-, MaxReal-, MinLReal-, MaxLReal-: LONGREAL;

		noerr-, warning156-: BOOLEAN;
		curpos-, errpos*: INTEGER;	(* character and error position in source file *)
		breakpc-: INTEGER;	(* set by OPV.Init *)
		currFile*: INTEGER;	(* current output file *)
		level*: INTEGER;	(* procedure nesting level *)
		pc-, entno-: INTEGER;  (* entry number *)
		modName-: ARRAY 32 OF CHAR;
		objname*: ARRAY 64 OF CHAR;

		opt-, glbopt-: SET;
		GlobalLanguage, Language-: CHAR; (* "1", "2": S8/I16/L32 | "C", "3": S16/I32/L64 *)
		GlobalAddressSize, AddressSize-, GlobalAlignment, Alignment-: SHORTINT;

		lasterrpos: INTEGER;
		inR: Texts.Reader;
		Log: Texts.Text;
		W: Texts.Writer;
		oldSF, newSF: Files.Rider;
		R: ARRAY 3 OF Files.Rider;
		oldSFile, newSFile, HFile, BFile, HIFile: Files.File;

		S: INTEGER;
		stop: BOOLEAN;

		OBERON:  ARRAY 1024 OF CHAR;

	(* ------------------------- Log Output ------------------------- *)

	PROCEDURE LogW*(ch: CHAR);
	BEGIN Console.Char(ch)
	END LogW;

	PROCEDURE LogWStr*(IN s: ARRAY OF CHAR);
	BEGIN Console.String(s)
	END LogWStr;

	PROCEDURE LogWNum*(i, len: INTEGER);
	BEGIN Console.Int(i, len)
	END LogWNum;

	PROCEDURE LogWLn*;
	BEGIN Console.Ln
	END LogWLn;


	(* ------------------------- parameter handling -------------------------*)

	PROCEDURE ScanOptions(VAR s: ARRAY OF CHAR; VAR opt: SET);
		VAR i: INTEGER;
	BEGIN i := 1; (* skip - *)
		WHILE s[i] # 0X DO
			CASE s[i] OF
			| "e": opt := opt / {extsf}
			| "s": opt := opt / {newsf}
			| "m": opt := opt / {mainprog}
			| "x": opt := opt / {inxchk}
			| "v": opt := opt / {vcpp}
			| "r": opt := opt / {ranchk}
			| "t": opt := opt / {typchk}
			| "a": opt := opt / {assert}
			| "k": opt := opt / {ansi}
			| "p": opt := opt / {ptrinit}
			| "i": opt := opt / {include0}
			| "1", "3", "C", "7"(*undocumented*): Language := s[i]

			(* target machine address size and alignment
					"21": 16 bit addresses, SIZE(SET) = 1 byte (e.g. Zilog Z80 CPU).
					"44": 32 bit addresses, 32 bit alignment (e.g. Unix/Linux 32 bit on x86).
					"48": 32 bit addresses, 64 bit alignment (e.g. Windows 32 bit on x86, Linux 32 bit on ARM).
					"88": 64 bit addresses, 64 bit alignment (e.g. 64 bit platforms).
			*)
			| "2":
				IF s[i+1] = "1" THEN AddressSize := 2; Alignment := 4; INC(i)
				ELSE Language := "2"
				END
			| "4":
				IF s[i+1] = "4" THEN AddressSize := 4; Alignment := 4; INC(i)
				ELSIF s[i+1] = "8" THEN AddressSize := 4; Alignment := 8; INC(i)
				ELSE
					LogWStr("  warning: option "); LogW(OptionChar); LogW("4");
					IF s[i+1] # 0X THEN LogW(s[i+1]); INC(i) END;
					LogWStr(" ignored"); LogWLn
				END
			| "8":
				IF s[i+1] = "8" THEN AddressSize := 8; Alignment := 8; INC(i)
				ELSE
					LogWStr("  warning: option "); LogW(OptionChar); LogW("8");
					IF s[i+1] # 0X THEN LogW(s[i+1]); INC(i) END;
					LogWStr(" ignored"); LogWLn
				END

			ELSE LogWStr("  warning: option "); LogW(OptionChar); LogW(s[i]); LogWStr(" ignored"); LogWLn
			END ;
			INC(i)
		END
	END ScanOptions;

	PROCEDURE ^GetProperties;

	PROCEDURE OpenPar*(IN title, cmd: ARRAY OF CHAR);	(* prepare for a sequence of translations *)
		VAR s: ARRAY 256 OF CHAR;
	BEGIN
		IF Args.argc = 1 THEN stop := TRUE;
			Console.Ln;
			Console.String(title); Console.Ln;
			Console.String("Copyright (c) Software Templ OG, 1995-2018 & VEDAsoft Oberon Club, 2013-2018"); Console.Ln; Console.Ln;
			Console.String("SYNOPSIS"); Console.Ln;
			Console.String('  command = "'); Console.String(cmd); Console.String('" options {file options}.'); Console.Ln;
			Console.String('  options = ["-" {option} ].'); Console.Ln;
			Console.String('  option  = "m" | "s" | "e" | "i" | "k" | "r" | "x" | "a" | "p" | "t"'); Console.Ln;
			Console.String('            "1" Oberon | "2" Oberon-2 | "C" Component Pascal | "3" Oberon-3'); Console.Ln;
			Console.String('    "21": 16 bit addresses, SIZE(SET) = 1 byte (e.g. Zilog Z80 CPU)'); Console.Ln;
			Console.String('    "44": 32 bit addresses, 32 bit alignment (UNIX/Linux 32 bit on x86)'); Console.Ln;
			Console.String('    "48": 32 bit addresses, 64 bit alignment (Windows 32 bit, Linux 32 bit/ARM)'); Console.Ln;
			Console.String('    "88": 64 bit addresses, 64 bit alignment (64 bit platforms)'); Console.Ln
		ELSE
			(* set options to initial defaults *)
			Language := "C"; (* default input language: Component Pascal *)
			AddressSize := -1; (* undefined *)
			glbopt := defopt; S := 1; s := "";
			Args.Get(1, s); stop := FALSE;
			WHILE s[0] = OptionChar DO ScanOptions(s, glbopt); INC(S); s := ""; Args.Get(S, s) END;
			(* Record global option settings for this command line *)
			GlobalAddressSize := AddressSize; GlobalAlignment := Alignment;	GlobalLanguage := Language
		END
	END OpenPar;

	PROCEDURE InitOptions*;	(* get the options for one translation *)
		VAR s: ARRAY 256 OF CHAR;
	BEGIN
		Alignment := GlobalAlignment; AddressSize := GlobalAddressSize; Language := GlobalLanguage;
		opt := glbopt;
		s := ""; Args.Get(S, s);
		WHILE s[0] = OptionChar DO ScanOptions(s, opt); INC(S); s := ""; Args.Get(S, s) END ;
		curpos := 256; errpos := curpos; lasterrpos := curpos - 10;
		GetProperties
	END InitOptions;

	(* get the source for one translation; msg is "translating" or "compiling" *)
	PROCEDURE Init*(IN msg: ARRAY OF CHAR; VAR done: BOOLEAN);
		VAR T: Texts.Text; beg, end, time: LONGINT;
			s: ARRAY 256 OF CHAR;
	BEGIN
		done := FALSE; curpos := 0;
		IF stop OR (S >= Args.argc) THEN noerr := TRUE; RETURN END ;
		s := ""; Args.Get(S, s);
		NEW(T); Texts.Open(T, s);
		LogWStr(s);
		IF Files.Old(s) = NIL THEN LogWStr(" not found"); LogWLn; noerr := FALSE
		ELSE
			Texts.OpenReader(inR, T, 0);
			LogWStr("  "); LogWStr(msg);
			done := TRUE; noerr := TRUE
		END ;
		INC(S);
		level := 0; errpos := curpos; lasterrpos := curpos -10
	END Init;

	(* ------------------------- read source text -------------------------*)

	PROCEDURE Get*(VAR ch: CHAR);	(* read next character from source text, 0X if eof *)
	BEGIN
		Texts.Read(inR, ch);
		IF ch = 0DX THEN curpos := (curpos DIV 256 + 1) * 256
		ELSIF curpos MOD 256 # 255 THEN INC(curpos)
			(* at 255 means:  >= 255 *)
		END;
		IF (ch < 09X) & ~inR.eot THEN ch := " " END
	END Get;

	PROCEDURE MakeFileName(IN name: ARRAY OF CHAR; VAR FName: ARRAY OF CHAR; IN ext: ARRAY OF CHAR);
		VAR i, j: INTEGER; ch: CHAR;
	BEGIN i := 0;
		LOOP ch := name[i];
			IF ch = 0X THEN EXIT END ;
			FName[i] := ch; INC(i)
		END ;
		j := 0;
		REPEAT ch := ext[j]; FName[i] := ch; INC(i); INC(j)
		UNTIL ch = 0X
	END MakeFileName;

	PROCEDURE LogErrMsg(n: SHORTINT);
		(*VAR S: Texts.Scanner; T: Texts.Text; ch: CHAR; i: INTEGER;
			buf: ARRAY 1024 OF CHAR;*)
	BEGIN
		IF n >= 0 THEN LogWStr("  err ")
		ELSE LogWStr("  warning "); n := -n
		END ;
		LogWNum(n, 1);
		LogWStr("  ");
		(*NEW(T); Texts.Open(T, "OfrontErrors.Text"); Texts.OpenScanner(S, T, 0);
		REPEAT S.line := 0;
			REPEAT Texts.Scan(S) UNTIL S.eot OR (S.line # 0)
		UNTIL S.eot OR (S.class = Texts.Int) & (S.i = n);
		IF ~S.eot THEN Texts.Read(S, ch); i := 0;
			WHILE ~S.eot & (ch # 0DX) DO buf[i] := ch; INC(i); Texts.Read(S, ch) END ;
			buf[i] := 0X; LogWStr(buf);
		END*)
		OfrontErrors.LogErrMsg(n)
	END LogErrMsg;

	PROCEDURE Mark*(n: SHORTINT; pos: INTEGER);
	BEGIN
		IF n >= 0 THEN
			noerr := FALSE;
			IF (pos < lasterrpos) OR (lasterrpos + 9 < pos) THEN lasterrpos := pos; LogWLn; LogWStr("  ");
				IF n < 249 THEN LogWStr("  line "); LogWNum(pos DIV 256, 1);
					LogWStr("  pos "); LogWNum(pos MOD 256 + 1, 1); LogErrMsg(n)
				ELSIF n = 255 THEN LogWStr("  line "); LogWNum(pos DIV 256, 1);
					LogWStr("  pos "); LogWNum(pos MOD 256 + 1, 1); LogWStr("  pc "); LogWNum(breakpc, 1)
				ELSIF n = 254 THEN LogWStr("pc not found")
				ELSE LogWStr(objname);
					IF n = 253 THEN LogWStr(" is new, compile with option e")
					ELSIF n = 252 THEN LogWStr(" is redefined, compile with option s")
					ELSIF n = 251 THEN LogWStr(" is redefined (private part only), compile with option s")
					ELSIF n = 250 THEN LogWStr(" is no longer visible, compile with option s")
					ELSIF n = 249 THEN LogWStr(" is not consistently imported, recompile imports")
					END
				END
			END
		ELSE
			IF pos >= 0 THEN LogWLn;
				LogWStr("  line "); LogWNum(pos DIV 256, 1); LogWStr("  pos "); LogWNum(pos MOD 256 + 1, 1)
			END ;
			LogErrMsg(n);
			IF pos < 0 THEN LogWLn END
		END
	END Mark;

	PROCEDURE err*(n: SHORTINT);
	BEGIN
		IF errpos MOD 256 = 255 THEN (* line underflow from OPS.Get *)
			Mark(n, errpos + 1)
		ELSE
			Mark(n, errpos)
		END
	END err;

	PROCEDURE FPrint*(VAR fp: INTEGER; val: INTEGER);
	BEGIN
		fp := SYSTEM.ROT(SYSTEM.VAL(INTEGER, SYSTEM.VAL(SET, fp) / SYSTEM.VAL(SET, val)), 1)
	END FPrint;

	PROCEDURE FPrintSet*(VAR fp: INTEGER; set: SET);
	BEGIN FPrint(fp, SYSTEM.VAL(INTEGER, set))
	END FPrintSet;

	PROCEDURE FPrintReal*(VAR fp: INTEGER; real: REAL);
	BEGIN FPrint(fp, SYSTEM.VAL(INTEGER, real))
	END FPrintReal;

	PROCEDURE FPrintLReal*(VAR fp: INTEGER; lr: LONGREAL);
		VAR l, h: INTEGER;
	BEGIN
		SYSTEM.GET(SYSTEM.ADR(lr), l); SYSTEM.GET(SYSTEM.ADR(lr)+4, h);
		FPrint(fp, l); FPrint(fp, h)
	END FPrintLReal;

	(* ------------------------- initialization ------------------------- *)

	PROCEDURE -GetAlignment(VAR a: SHORTINT) "struct {char c; long long l;} _s; *a = (char*)&_s.l - (char*)&_s";

	PROCEDURE GetProperties;
		VAR base: INTEGER;
	BEGIN
		warning156 := AddressSize = -1;
		IF warning156 THEN
 			AddressSize := SIZE(SYSTEM.PTR); (* this compilers address size *)
			GetAlignment(Alignment)          (* this compilers alignment *)
		END;
		(* default characteristics *)
		IF (Language = "1") OR (Language = "2") THEN (* Oberon and Oberon-2 *)
			SIntSize := 1; IntSize := 2; LIntSize := 4;
			MinSInt := -80H; MinInt := -8000H; MinLInt := -2147483647-1;	(*-2147483648*)
			MaxSInt := 7FH; MaxInt := 7FFFH; MaxLInt := 7FFFFFFFH;	(*2147483647*)
		ELSE (* Component Pascal and Oberon-07 *)
			SIntSize := 2; IntSize := 4; LIntSize := 8;
			MinSInt := -8000H; MaxSInt := 7FFFH;
			MinInt := -2147483647-1 (*-2147483648*); MaxInt := 7FFFFFFFH (*2147483647*);
			base := 63;
			MinLInt := SYSTEM.LSH(SYSTEM.VAL(LONGINT, 1), base);	(*-9223372036854775808*)
			MaxLInt := -(MinLInt+1)	(*9223372036854775807*)
		END;
		IF AddressSize = 2 THEN (* Z80 and other 8 bit CPUs *)
			SetSize := 1; MaxSet := SetSize * 8 - 1;
			IndexSize := 2; MaxIndex := 7FFFH (*32767*)
		ELSE
			SetSize := 4; MaxSet := 31;
			IndexSize := 4; (* size of the type of array index, see also SYSTEM_ARRLEN *)
			MaxIndex := 7FFFFFFFH (*2147483647*)
		END;
		RealSize := 4; MaxReal := 3.402823466D38; MinReal := -MaxReal; LRealSize := 8;
		MaxLReal := 1.7976931348623158D307 * 9.999999 (*should be 1.7976931348623158D308 *);
		MinLReal := -MaxLReal;
		ProcSize := AddressSize; PointerSize := AddressSize;
		MaxHDig := 2*LIntSize
	END GetProperties;

	(* ------------------------- Read Symbol File ------------------------- *)

	PROCEDURE SymRCh*(VAR ch: CHAR);
	BEGIN Files.ReadChar(oldSF, ch)
	END SymRCh;

	PROCEDURE SymRInt*(): LONGINT;
		VAR k: LONGINT;
	BEGIN Files.ReadNum(oldSF, k); RETURN k
	END SymRInt;

	PROCEDURE SymRSet*(VAR s: SET);
		VAR k: LONGINT;
	BEGIN Files.ReadNum(oldSF, k); s := SYSTEM.VAL(SET, SHORT(k))
	END SymRSet;

	PROCEDURE SymRReal*(VAR r: REAL);
	BEGIN Files.ReadReal(oldSF, r)
	END SymRReal;

	PROCEDURE SymRLReal*(VAR lr: LONGREAL);
	BEGIN Files.ReadLReal(oldSF, lr)
	END SymRLReal;

	PROCEDURE CloseOldSym*;
	END CloseOldSym;

	PROCEDURE OldSym*(IN modName: ARRAY OF CHAR; VAR done: BOOLEAN);
		VAR ch: CHAR; fileName: FileName;
	BEGIN MakeFileName(modName, fileName, SFext);
		oldSFile := Files.Old(fileName); done := oldSFile # NIL;
		IF done THEN
			Files.Set(oldSF, oldSFile, 0); Files.ReadChar(oldSF, ch);
			IF ch # SFtag THEN err(-306);  (*possibly a symbol file from another Oberon implementation, e.g. HP-Oberon*)
				CloseOldSym; done := FALSE
			END
		END
	END OldSym;

	PROCEDURE eofSF*(): BOOLEAN;
	BEGIN RETURN oldSF.eof
	END eofSF;

	(* ------------------------- Write Symbol File ------------------------- *)

	PROCEDURE SymWCh*(ch: CHAR);
	BEGIN Files.WriteChar(newSF, ch)
	END SymWCh;

	PROCEDURE SymWInt*(i: LONGINT);
	BEGIN Files.WriteNum(newSF, i)
	END SymWInt;

	PROCEDURE SymWSet*(s: SET);
	BEGIN Files.WriteNum(newSF, SYSTEM.VAL(INTEGER, s))
	END SymWSet;

	PROCEDURE SymWReal*(r: REAL);
	BEGIN Files.WriteReal(newSF, r)
	END SymWReal;

	PROCEDURE SymWLReal*(lr: LONGREAL);
	BEGIN Files.WriteLReal(newSF, lr)
	END SymWLReal;

	PROCEDURE RegisterNewSym*;
	BEGIN
		IF (modName # "SYSTEM") OR (mainprog IN opt) THEN Files.Register(newSFile) END
	END RegisterNewSym;

	PROCEDURE DeleteNewSym*;
	END DeleteNewSym;

	PROCEDURE NewSym*(VAR modName: ARRAY OF CHAR);
		VAR fileName: FileName;
	BEGIN MakeFileName(modName, fileName, SFext);
		newSFile := Files.New(fileName);
		IF newSFile # NIL THEN Files.Set(newSF, newSFile, 0); Files.WriteChar(newSF, SFtag)
		ELSE err(153)
		END
	END NewSym;

	(* ------------------------- Write Header & Body Files ------------------------- *)

	PROCEDURE Write*(ch: CHAR);
	BEGIN Files.WriteChar(R[currFile], ch)
	END Write;

	PROCEDURE WriteString*(IN s: ARRAY OF CHAR);
		VAR i: INTEGER;
	BEGIN i := 0;
		WHILE s[i] # 0X DO INC(i) END;
		Files.WriteBytes(R[currFile], s, i)
	END WriteString;

	PROCEDURE WriteHex* (i: INTEGER);
		VAR s: ARRAY 3 OF CHAR;
			digit : INTEGER;
	BEGIN
		digit := SHORT(i) DIV 16;
		IF digit < 10 THEN s[0] := CHR (ORD ("0") + digit); ELSE s[0] := CHR (ORD ("a") - 10 + digit ); END;
		digit := SHORT(i) MOD 16;
		IF digit < 10 THEN s[1] := CHR (ORD ("0") + digit); ELSE s[1] := CHR (ORD ("a") - 10 + digit ); END;
		s[2] := 0X;
		WriteString(s)
	END WriteHex;

	PROCEDURE WriteInt* (i: LONGINT);
		VAR s: ARRAY 20 OF CHAR; i1: LONGINT; k: INTEGER;
	BEGIN
		IF (i = MIN(INTEGER)) OR (i = MIN(LONGINT)) THEN	(* requires special bootstrap for 64 bit *)
			(* ABS(MIN(INTEGER)) is one more than MAX(INTEGER), causing problems representing the value
				as a minus sign followed by absoute value. Therefore represent as -MaxInt - 1.
				For INTEGER this avoids a compiler warning 'this decimal constant is unsigned only in ISO C90'.
				For LONGINT it is the only way to represent MinLInt. *)
			Write("("); WriteInt(i+1); WriteString("-1)")	(* requires special bootstrap for 64 bit *)
		ELSE i1 := ABS(i);
			s[0] := CHR(i1 MOD 10 + ORD("0")); i1 := i1 DIV 10; k := 1;
			WHILE i1 > 0 DO s[k] := CHR(i1 MOD 10 + ORD("0")); i1 := i1 DIV 10; INC(k) END ;
			IF i < 0 THEN s[k] := "-"; INC(k) END ;
			WHILE k > 0 DO  DEC(k); Write(s[k]) END;
			IF (ansi IN opt) & ((i < MIN(INTEGER)) OR (i > MAX(INTEGER))) THEN WriteString("LL") END
		END ;
	END WriteInt;

	PROCEDURE WriteReal* (r: LONGREAL; suffx: CHAR);
		VAR W: Texts.Writer; T: Texts.Text; R: Texts.Reader; s: ARRAY 32 OF CHAR; ch: CHAR; i: INTEGER;
	BEGIN
(*should be improved *)
		IF (r < MaxLInt) & (r > MinLInt) & (r = ENTIER(r)) THEN
			IF suffx = "f" THEN WriteString("(REAL)") ELSE WriteString("(LONGREAL)") END ;
			WriteInt(ENTIER(r))
		ELSE
			Texts.OpenWriter(W);
			IF suffx = "f" THEN Texts.WriteLongReal(W, r, 16) ELSE Texts.WriteLongReal(W, r, 23) END ;
			NEW(T); Texts.Open(T, ""); Texts.Append(T, W.buf);
			Texts.OpenReader(R, T, 0); i := 0; Texts.Read(R, ch);
			WHILE ch # 0X DO s[i] := ch; INC(i); Texts.Read(R, ch) END ;
	(* s[i] := suffx; s[i+1] := 0X;
	suffix does not work in K&R *)
			s[i] := 0X;
			i := 0; ch := s[0];
			WHILE (ch # "D") & (ch # 0X) DO INC(i); ch := s[i] END ;
			IF ch = "D" THEN s[i] := "e" END ;
			WriteString(s)
		END
	END WriteReal;

	PROCEDURE WriteLn* ();
	BEGIN WriteString(Platform.newLine)
	END WriteLn;

	PROCEDURE WriteModPos* ;
	BEGIN
		WriteString(', "'); WriteString(modName); WriteString('", '); WriteInt(errpos)
	END WriteModPos;

	PROCEDURE Append(VAR R: Files.Rider; F: Files.File);
		VAR R1: Files.Rider; buffer: ARRAY 4096 OF CHAR;
	BEGIN
		IF F # NIL THEN
			Files.Set(R1, F, 0); Files.ReadBytes(R1, buffer, LEN(buffer));
			WHILE LEN(buffer) - R1.res > 0 DO
				Files.WriteBytes(R, buffer, LEN(buffer) - R1.res);
				Files.ReadBytes(R1, buffer, LEN(buffer))
			END
		END
	END Append;

	PROCEDURE OpenFiles*(VAR moduleName: ARRAY OF CHAR);
		VAR FName: ARRAY 32 OF CHAR;
	BEGIN
		COPY(moduleName, modName);
		HFile := Files.New("");
		IF HFile # NIL THEN Files.Set(R[HeaderFile], HFile, 0) ELSE err(153) END ;
		MakeFileName(moduleName, FName, BFext);
		BFile := Files.New(FName);
		IF BFile # NIL THEN Files.Set(R[BodyFile], BFile, 0) ELSE err(153) END ;
		MakeFileName(moduleName, FName, HFext);
		HIFile := Files.New(FName);
		IF HIFile # NIL THEN Files.Set(R[HeaderInclude], HIFile, 0) ELSE err(153) END ;
		IF include0 IN opt THEN
			MakeFileName(moduleName, FName, ".h0"); Append(R[HeaderInclude], Files.Old(FName));
			MakeFileName(moduleName, FName, ".c0"); Append(R[BodyFile], Files.Old(FName))
		END
	END OpenFiles;

	PROCEDURE CloseFiles*;
		VAR FName: ARRAY 32 OF CHAR; res: INTEGER;
	BEGIN
		IF noerr THEN LogWStr("    "); LogWNum(SHORT( Files.Pos(R[BodyFile]) ), 0) END ;
		IF noerr THEN
			IF modName = "SYSTEM" THEN
				IF ~(mainprog IN opt) THEN Files.Register(BFile) END
			ELSIF ~(mainprog IN opt) THEN
				Append(R[HeaderInclude], HFile);
				Files.Register(HIFile); Files.Register(BFile)
			ELSE
				MakeFileName(modName, FName, HFext); Files.Delete(FName, res);
				MakeFileName(modName, FName, SFext); Files.Delete(FName, res);
				Files.Register(BFile)
			END
		END ;
		HFile := NIL; BFile := NIL; HIFile := NIL; newSFile := NIL; oldSFile := NIL;
		Files.Set(R[0], NIL, 0); Files.Set(R[1], NIL, 0); Files.Set(R[2], NIL, 0); Files.Set(newSF, NIL, 0); Files.Set(oldSF, NIL, 0)
	END CloseFiles;

	PROCEDURE PromoteIntConstToLInt*();
	BEGIN
		(* ANSI C does not need explicit promotion.
			K&R C implicitly promotes integer constants to type int in parameter lists.
			if the formal parameter, however, is of type long, appending "L" is required in ordere to promote
			the parameter explicitly to type long (if LONGINT corresponds to long, which we do not really know).
			It works for all known K&R versions of ofront and K&R is dying out anyway.
			A cleaner solution would be to cast with type (LONGINT), but this requires a bit more changes.
		*)
		IF ~(ansi IN opt) THEN Write("L") END
	END PromoteIntConstToLInt;

BEGIN Texts.OpenWriter(W);
  OBERON := "."; Platform.GetEnv("OBERON", OBERON);
  Files.SetSearchPath(OBERON);
END OfrontOPM.
