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
		dynlib* = 11;	(* translate module to be a dynamic library *)
		for* = 12;	(* "proper FOR" by Oleg Komlev *)
		oldc* = 13;	(* old C compilers restrictions support *)
		foreign* = 14;	(* interface on external library *)
		noinit* = 15;	(* don't generate BEGIN section's body *)
		defopt* = {inxchk, typchk, ptrinit, assert, ansi};	(* default options *)

		nilval* = 0;

		MinHaltNr* = 0;
		MaxHaltNr* = 255;
		MaxSysFlag* = 1;

		LANotAlloc* = -1;	(* XProc link adr initialization *)
		ConstNotAlloc* = -1;	(* for allocation of string and real constants *)
		TDAdrUndef* = -1;	(* no type desc allocated *)

		MaxCases* = 256;
		MaxCaseRange* = 512;

		MaxStruct* = 1294;

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
		HFext = ".oh";	(* header file extension *)
		SFtag = 0F7X;	(* symbol file tag *)

		HeaderFile* = 0;
		BodyFile* = 1;
		HeaderInclude* = 2;

		MinReal32Pat = 0FF7FFFFFH;	(* most positive, 32-bit pattern *)
		MinReal64PatL = 0FFFFFFFFH;	(* most  negative, lower 32-bit pattern *)
		MinReal64PatH = 0FFEFFFFFH;	(* most  negative, higher 32-bit pattern *)
		MaxReal32Pat = 07F7FFFFFH;	(* most positive, 32-bit pattern *)
		MaxReal64PatL = 0FFFFFFFFH;	(* most positive, lower 32-bit pattern *)
		MaxReal64PatH = 07FEFFFFFH;	(* most positive, higher 32-bit pattern *)

	TYPE
		CHAR = SHORTCHAR;
		FileName = ARRAY 32 OF CHAR;

	VAR
		LEHost: BOOLEAN;	(* little or big endian host *)
		MinReal32-, MaxReal32-, MinReal64-, MaxReal64-: REAL;
		SetSize-, IndexSize-, MaxSet-: SHORTINT; MaxIndex-: LONGINT;

		noerr-, warning156-: BOOLEAN;
		curpos-, errpos*: INTEGER;	(* character and error position in source file *)
		breakpc-: INTEGER;	(* set by OPV.Init *)
		currFile*: INTEGER;	(* current output file *)
		level*: INTEGER;	(* procedure nesting level *)
		pc-, entno-: INTEGER;  (* entry number *)
		modName-: ARRAY 32 OF CHAR;
		objname*: ARRAY 64 OF CHAR;

		opt*, glbopt: SET;
		GlobalLang, Lang-: CHAR; (* "1", "2": S8/I16/L32 | "C", "3": S16/I32/L64 *)
		GlobalAdrSize, AdrSize-, GlobalAlignment, Alignment-: SHORTINT;

		crc32tab: ARRAY 256 OF INTEGER;
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

	(* ----------------------- System runtime ----------------------- *)

	TYPE
		LongCard = LONGINT;

	PROCEDURE -udiv(x, y: LongCard): LongCard "((__U_LONGINT)(x) / (__U_LONGINT)(y))";

	PROCEDURE UDiv*(x, y: LongCard): LongCard;
	BEGIN
		ASSERT(y # 0); RETURN udiv(x, y)
	END UDiv;

	PROCEDURE Div0*(x, y: LONGINT): LONGINT;
	BEGIN
		ASSERT(y # 0); RETURN x DIV0 y
	END Div0;

	PROCEDURE Rem0*(x, y: LONGINT): LONGINT;
	BEGIN
		ASSERT(y # 0); RETURN x REM0 y
	END Rem0;

	PROCEDURE Lsl*(x: LONGINT; n: INTEGER): LONGINT;
	BEGIN
		ASSERT((0 <= n) & (n <= 63));
		RETURN SYSTEM.LSH(x, n)
	END Lsl;

	PROCEDURE Lsr*(x: LONGINT; n: INTEGER): LONGINT;
	BEGIN
		ASSERT((0 <= n) & (n <= 63));
		RETURN SYSTEM.LSH(x, -n)
	END Lsr;

	PROCEDURE Ror*(x: LONGINT; n: INTEGER): LONGINT;
	BEGIN
		ASSERT((0 <= n) & (n <= 63));
		RETURN SYSTEM.ROT(x, -n)
	END Ror;

	PROCEDURE IntPower* (x: REAL; n: INTEGER): REAL;
		VAR y: REAL;
	BEGIN
		IF n = MIN(INTEGER) THEN RETURN IntPower(x, n + 1) / x END;
		y := 1.;
		IF n < 0 THEN x := 1. / x; n := -n END;
		WHILE n > 0 DO
			IF ODD(n) THEN y := y * x; DEC(n)
			ELSE x := x * x; n := n DIV 2
			END
		END;
		RETURN y
	END IntPower;

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


	(* ------------------------- initialization ------------------------- *)

	PROCEDURE- GetAlignment(VAR a: SHORTINT) (* Windows x86/Linux ARM = 48, Linux x86 = 44, x64 = 88 *)
		"struct {char c; long long l;} _s; *a = (char*)&_s.l - (char*)&_s";

	PROCEDURE GetProperties;
	BEGIN
		warning156 := AdrSize = -1;
		IF warning156 THEN	(* this compiler's address size and alignment *)
			AdrSize := SIZE(SYSTEM.PTR); GetAlignment(Alignment)
		END;
		(* default characteristics *)
		IF AdrSize = 2 THEN	(* Z80 and other 8 bit CPUs *)
			SetSize := 1; MaxSet := 7; IndexSize := 2; MaxIndex := MAX(SHORTINT)
		ELSE
			SetSize := 4; MaxSet := 31;
			IndexSize := 4;	(* size of the type of array index, see also SYSTEM_ARRLEN *)
			MaxIndex := MAX(INTEGER)
		END
	END GetProperties;

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
			| "d": opt := opt / {dynlib}
			| "f": opt := opt / {for}
			| "o": opt := opt / {oldc}
			| "1", "3"(*undocumented*), "C", "7": Lang := s[i]

			(* target machine address size and alignment
					"21": 16 bit addresses, SIZE(SET) = 1 byte (e.g. Zilog Z80 CPU).
					"44": 32 bit addresses, 32 bit alignment (e.g. Unix/Linux 32 bit on x86).
					"48": 32 bit addresses, 64 bit alignment (e.g. Windows 32 bit on x86, Linux 32 bit on ARM).
					"88": 64 bit addresses, 64 bit alignment (e.g. 64 bit platforms).
			*)
			| "2":
				IF s[i+1] = "1" THEN AdrSize := 2; Alignment := 1; INC(i)
				ELSE Lang := "2"
				END
			| "4":
				IF s[i+1] = "4" THEN AdrSize := 4; Alignment := 4; INC(i)
				ELSIF s[i+1] = "8" THEN AdrSize := 4; Alignment := 8; INC(i)
				ELSE
					LogWStr("  warning: option "); LogW(OptionChar); LogW("4");
					IF s[i+1] # 0X THEN LogW(s[i+1]); INC(i) END;
					LogWStr(" ignored"); LogWLn
				END
			| "8":
				IF s[i+1] = "8" THEN AdrSize := 8; Alignment := 8; INC(i)
				ELSE
					LogWStr("  warning: option "); LogW(OptionChar); LogW("8");
					IF s[i+1] # 0X THEN LogW(s[i+1]); INC(i) END;
					LogWStr(" ignored"); LogWLn
				END

			ELSE LogWStr("  warning: option "); LogW(OptionChar); LogW(s[i]); LogWStr(" ignored"); LogWLn
			END;
			INC(i)
		END;
		IF {dynlib, mainprog} * opt = {dynlib, mainprog} THEN EXCL(opt, mainprog);
			LogWStr("  warning: option "); LogW(OptionChar); LogWStr("m ignored"); LogWLn
		END
	END ScanOptions;

	PROCEDURE OpenPar*(IN title, cmd: ARRAY OF CHAR);	(* prepare for a sequence of translations *)
		VAR s: ARRAY 256 OF CHAR;
	BEGIN
		IF Args.argc = 1 THEN stop := TRUE;
			Console.Ln;
			Console.String(title); Console.Ln;
			Console.String("Copyright (c) Software Templ OG, 1995-2020 & VEDAsoft Oberon Club, 2013-2020"); Console.Ln; Console.Ln;
			Console.String('  command = "'); Console.String(cmd); Console.String('" options {file options}.'); Console.Ln;
			Console.String('  options = ["-" {option} ].'); Console.Ln;
			Console.String('  option  = "m" | "s" | "e" | "i" | "r" | "x" | "a" | "p" | "t" | "f" | "d"'); Console.Ln;
			Console.String('            [ "1" | "2" | "C" | "7" | "3" ]    [ "21" |"44" |"48" | "88" ].'); Console.Ln;
			Console.String('       "m": generate a main module                                  (off)'); Console.Ln;
			Console.String('       "s": allow changing the symbol file                          (off)'); Console.Ln;
			Console.String('       "e": allow extending the module interface                    (off)'); Console.Ln;
			Console.String('       "i": include header and body prefix files (Module.h0/.c0)    (off)'); Console.Ln;
			Console.String('       "r": check value ranges                                      (off)'); Console.Ln;
			Console.String('       "x": check array indices                                      (on)'); Console.Ln;
			Console.String('       "a": check assertions                                         (on)'); Console.Ln;
			Console.String('       "p": pointer initialization                                   (on)'); Console.Ln;
			Console.String('       "t": check type guards                                        (on)'); Console.Ln;
			Console.String('       "f": allow "correct FOR" (concept by Oleg Komlev)            (off)'); Console.Ln;
			Console.String('       "d": translate a module into dynamic library (.dll/.so)      (off)'); Console.Ln;
			Console.String('  "1": Oberon "2": Oberon-2 "C": Component Pascal "7": Oberon-07 "3": Oberon-3'); Console.Ln;
			Console.String('  "21": 16 bit addresses, SIZE(SET) = 1 byte (e.g. Zilog Z80 CPU)'); Console.Ln;
			Console.String('  "44": 32 bit addresses, 32 bit alignment (UNIX/Linux 32 bit on x86)'); Console.Ln;
			Console.String('  "48": 32 bit addresses, 64 bit alignment (Windows 32 bit, Linux 32 bit/ARM)'); Console.Ln;
			Console.String('  "88": 64 bit addresses, 64 bit alignment (64 bit platforms)'); Console.Ln
		ELSE
			(* set options to initial defaults *)
			Lang := "C";	(* default input language: Component Pascal *)
			AdrSize := -1;	(* undefined *)
			glbopt := defopt; S := 1; s := "";
			Args.Get(1, s); stop := FALSE;
			WHILE s[0] = OptionChar DO ScanOptions(s, glbopt); INC(S); s := ""; Args.Get(S, s) END;
			(* Record global option settings for this command line *)
			GlobalAdrSize := AdrSize; GlobalAlignment := Alignment;	GlobalLang := Lang
		END
	END OpenPar;

	PROCEDURE InitOptions*;	(* get the options for one translation *)
		VAR s: ARRAY 256 OF CHAR;
	BEGIN
		Alignment := GlobalAlignment; AdrSize := GlobalAdrSize; Lang := GlobalLang;
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
		ELSE LogWStr("  warning "); n := SHORT(-n)
		END;
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
				IF n < 249 THEN LogWStr("  "); LogWNum(pos DIV 256, 1);
					LogW(":"); LogWNum(pos MOD 256 + 1, 1); LogErrMsg(n)
				ELSIF n = 255 THEN LogWStr("  "); LogWNum(pos DIV 256, 1);
					LogW(":"); LogWNum(pos MOD 256 + 1, 1); LogWStr("  pc "); LogWNum(breakpc, 1)
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
				LogWStr("  "); LogWNum(pos DIV 256, 1); LogW(":"); LogWNum(pos MOD 256 + 1, 1)
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


	(* fingerprinting *)

	PROCEDURE InitCrcTab;
		(* CRC32, high bit first, pre & post inverted *)
		CONST poly = {0, 1, 2, 4, 5, 7, 8, 10, 11, 12, 16, 22, 23, 26};	(* CRC32 polynom *)
		VAR x, c, i: INTEGER;
	BEGIN
		x := 0;
		WHILE x < 256 DO
			c := x * 1000000H; i := 0;
			WHILE i < 8 DO
				IF c < 0 THEN c := ORD(BITS(c * 2) / poly)
				ELSE c := c * 2
				END;
				INC(i)
			END;
			crc32tab[ORD(BITS(x) / BITS(255))] := ORD(BITS(c) / BITS(255));
			INC(x)
		END
	END InitCrcTab;

	PROCEDURE FPrint*(VAR fp: INTEGER; val: INTEGER);
		VAR c: INTEGER;
	BEGIN
		(* CRC32, high bit first, pre & post inverted *)
		c := ORD(BITS(fp * 256) / BITS(crc32tab[ORD(BITS(fp DIV 1000000H) / BITS(val DIV 1000000H)) MOD 256]));
		c := ORD(BITS(c * 256) / BITS(crc32tab[ORD(BITS(c DIV 1000000H) / BITS(val DIV 10000H)) MOD 256]));
		c := ORD(BITS(c * 256) / BITS(crc32tab[ORD(BITS(c DIV 1000000H) / BITS(val DIV 100H)) MOD 256]));
		fp := ORD(BITS(c * 256) / BITS(crc32tab[ORD(BITS(c DIV 1000000H) / BITS(val)) MOD 256]))
	END FPrint;

	PROCEDURE FPrintSet*(VAR fp: INTEGER; set: SET);
	BEGIN FPrint(fp, SYSTEM.VAL(INTEGER, set))
	END FPrintSet;

	PROCEDURE FPrintReal*(VAR fp: INTEGER; real: SHORTREAL);
	BEGIN FPrint(fp, SYSTEM.VAL(INTEGER, real))
	END FPrintReal;

	PROCEDURE FPrintLReal*(VAR fp: INTEGER; lr: REAL);
		VAR li: LONGINT;
	BEGIN li := SYSTEM.VAL(LONGINT, lr);
		FPrint(fp, SHORT(li)); FPrint(fp, SHORT(ASH(li, -32)))
	END FPrintLReal;

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

	PROCEDURE SymRReal*(VAR r: SHORTREAL);
	BEGIN Files.ReadReal(oldSF, r)
	END SymRReal;

	PROCEDURE SymRLReal*(VAR lr: REAL);
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

	PROCEDURE SymWReal*(r: SHORTREAL);
	BEGIN Files.WriteReal(newSF, r)
	END SymWReal;

	PROCEDURE SymWLReal*(lr: REAL);
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
			WHILE i1 > 0 DO s[k] := CHR(i1 MOD 10 + ORD("0")); i1 := i1 DIV 10; INC(k) END;
			IF i < 0 THEN s[k] := "-"; INC(k) END;
			WHILE k > 0 DO  DEC(k); Write(s[k]) END;
			IF (ansi IN opt) & ((i < MIN(INTEGER)) OR (i > MAX(INTEGER))) THEN WriteString("LL") END
		END
	END WriteInt;

	PROCEDURE WriteReal* (r: REAL; suffx: CHAR);
		VAR W: Texts.Writer; T: Texts.Text; R: Texts.Reader; s: ARRAY 32 OF CHAR; ch: CHAR; i: INTEGER;
	BEGIN
		IF ABS(r) > 1.0E+308 THEN Write("("); WriteReal(r / 2, suffx); WriteString(" * 2)"); RETURN END;
		IF (r < MAX(LONGINT)) & (r > MIN(LONGINT)) & (r = ENTIER(r)) THEN
			IF suffx = "f" THEN WriteString("(SHORTREAL)") ELSE WriteString("(REAL)") END;
			WriteInt(ENTIER(r))
		ELSE
			Texts.OpenWriter(W);
			IF suffx = "f" THEN Texts.WriteLongReal(W, r, 8) ELSE Texts.WriteLongReal(W, r, 17) END;
			NEW(T); Texts.Open(T, ""); Texts.Append(T, W.buf);
			Texts.OpenReader(R, T, 0); i := 0; Texts.Read(R, ch);
			WHILE ch # 0X DO s[i] := ch; INC(i); Texts.Read(R, ch) END;
	(* s[i] := suffx; s[i+1] := 0X;
	suffix does not work in K&R *)
			s[i] := 0X;
			i := 0; ch := s[0];
			WHILE (ch # "E") & (ch # 0X) DO INC(i); ch := s[i] END;
			IF ch = "E" THEN s[i] := "e" END;
			WriteString(s)
		END
	END WriteReal;

	PROCEDURE WriteLn* ();
	BEGIN WriteString(Platform.NewLine)
	END WriteLn;

	PROCEDURE WriteModPos*;
	BEGIN
		IF ansi IN opt THEN WriteString("(CHAR*)") END;
		Write('"'); WriteString(modName); WriteString('", '); WriteInt(errpos)
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
		modName := moduleName$;
		HFile := Files.New("");
		IF HFile # NIL THEN Files.Set(R[HeaderFile], HFile, 0) ELSE err(153) END;
		IF ~(foreign IN opt) THEN
			MakeFileName(moduleName, FName, BFext);
			BFile := Files.New(FName);
			IF BFile # NIL THEN Files.Set(R[BodyFile], BFile, 0) ELSE err(153) END
		END;
		MakeFileName(moduleName, FName, HFext);
		HIFile := Files.New(FName);
		IF HIFile # NIL THEN Files.Set(R[HeaderInclude], HIFile, 0) ELSE err(153) END;
		IF include0 IN opt THEN
			MakeFileName(moduleName, FName, ".h0"); Append(R[HeaderInclude], Files.Old(FName));
			MakeFileName(moduleName, FName, ".c0"); Append(R[BodyFile], Files.Old(FName))
		END
	END OpenFiles;

	PROCEDURE CloseFiles*;
		VAR FName: ARRAY 32 OF CHAR; res: INTEGER; body: BOOLEAN;
	BEGIN
		body := ~(foreign IN opt);
		IF noerr THEN LogWStr("    ");
			IF body THEN LogWNum(SHORT( Files.Pos(R[BodyFile]) ), 0) ELSE LogWNum(SHORT( Files.Pos(R[HeaderFile]) ), 0) END
		END;
		IF noerr THEN LogWStr("    ") END;
		IF noerr THEN
			IF modName = "SYSTEM" THEN
				IF ~(mainprog IN opt) THEN
					IF body THEN Files.Register(BFile);
					ELSE MakeFileName(modName, FName, BFext); Files.Delete(FName, res)
					END
				END
			ELSIF ~(mainprog IN opt) THEN
				Append(R[HeaderInclude], HFile);
				Files.Register(HIFile);
				IF body THEN Files.Register(BFile)
				ELSE MakeFileName(modName, FName, BFext); Files.Delete(FName, res)
				END
			ELSE
				MakeFileName(modName, FName, HFext); Files.Delete(FName, res);
				MakeFileName(modName, FName, SFext); Files.Delete(FName, res);
				IF body THEN Files.Register(BFile)
				ELSE MakeFileName(modName, FName, BFext); Files.Delete(FName, res)
				END
			END
		END;
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

	PROCEDURE Compound (lo, hi: INTEGER): REAL;
		VAR r: REAL;
	BEGIN
		IF LEHost THEN
			SYSTEM.PUT(SYSTEM.ADR(r), lo); SYSTEM.PUT(SYSTEM.ADR(r) + 4, hi)
		ELSE
			SYSTEM.PUT(SYSTEM.ADR(r) + 4, lo); SYSTEM.PUT(SYSTEM.ADR(r), hi)
		END;
		RETURN r
	END Compound;

	PROCEDURE InitHost;
		VAR test: SHORTINT; lo: SHORTCHAR;
	BEGIN
		test := 1; SYSTEM.GET(SYSTEM.ADR(test), lo); LEHost := lo = 1X;
		MinReal32 := SYSTEM.VAL(SHORTREAL, MinReal32Pat);
		MaxReal32 := SYSTEM.VAL(SHORTREAL, MaxReal32Pat);
		MinReal64 := Compound(MinReal64PatL, MinReal64PatH);
		MaxReal64 := Compound(MaxReal64PatL, MaxReal64PatH)
	END InitHost;

BEGIN InitCrcTab; InitHost; Texts.OpenWriter(W);
  OBERON := "."; Platform.GetEnv("OBERON", OBERON);
  Files.SetSearchPath(OBERON);
END OfrontOPM.
