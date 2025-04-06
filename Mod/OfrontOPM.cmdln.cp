MODULE OfrontOPM;	(* RC 6.3.89 / 28.6.89, J.Templ 10.7.89 / 22.7.96  *)
(* constants needed for C code generation

	31.1.2007 jt synchronized with BlackBox version, in particular PromoteIntConstToLInt added
*)

	IMPORT SYSTEM, Texts, Files, Strings, Platform, CmdArgs, Console, OfrontErrors;

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
		gpcp* = 16;	(* Gardens Point Component Pascal extensions enabled *)
		oakwood* = 17;	(* Oakwood Guidelines extensions enabled *)
		widechar* = 18;	(* UTF-8 to UCS-2 wide character transcoding enabled *)
		close* = 19;	(* the module has CLOSE section *)
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

		MaxStruct* = 1754;

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

		DummyFile* = -1;
		HeaderFile* = 0;
		BodyFile* = 1;
		HeaderInclude* = 2;

		MinReal32Pat = 0FF7FFFFFH;	(* most positive, 32-bit pattern *)
		MinReal64PatL = 0FFFFFFFFH;	(* most  negative, lower 32-bit pattern *)
		MinReal64PatH = 0FFEFFFFFH;	(* most  negative, higher 32-bit pattern *)
		MaxReal32Pat = 07F7FFFFFH;	(* most positive, 32-bit pattern *)
		MaxReal64PatL = 0FFFFFFFFH;	(* most positive, lower 32-bit pattern *)
		MaxReal64PatH = 07FEFFFFFH;	(* most positive, higher 32-bit pattern *)
		InfRealPat = 07F800000H;	(* real infinity pattern *)

	TYPE
		FileName = ARRAY 36 OF SHORTCHAR; (* LEN(modName) + LEN(".sym") *)

	VAR
		LEHost: BOOLEAN;	(* little or big endian host *)
		MinReal32-, MaxReal32-, InfReal-, MinReal64-, MaxReal64-: REAL;
		SetSize-, IndexSize-, MaxSet-: SHORTINT; MaxIndex-: LONGINT;

		noerr-, warning156-: BOOLEAN;
		curpos-, errpos*: INTEGER;	(* character and error position in source file *)
		breakpc-: INTEGER;	(* set by OPV.Init *)
		currFile*: INTEGER;	(* current output file *)
		level*: INTEGER;	(* procedure nesting level *)
		pc-, entno-: INTEGER;  (* entry number *)
		modName-: ARRAY 32 OF SHORTCHAR;
		objname*: ARRAY 64 OF SHORTCHAR;
		checksum*: INTEGER;	(* symbol file checksum *)

		opt*, glbopt: SET;
		GlobalLang, Lang-: SHORTCHAR; (* "1", "2": S8/I16/L32 | "C", "3": S16/I32/L64 *)
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

		OBERON: ARRAY 1024 OF SHORTCHAR;

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

	PROCEDURE ChkSum (VAR fp: INTEGER; val: INTEGER);	(* symbolfile checksum *)
	BEGIN
		(* same as FPrint, 8 bit only *)
		fp := ORD(BITS(fp * 256) / BITS(crc32tab[ORD(BITS(fp DIV 1000000H) / BITS(val)) MOD 256]))
	END ChkSum;

	(* VAR is necessary here to avoid problems with GCC optimizer *)
	PROCEDURE LoWord (VAR r: REAL): INTEGER;
		VAR x: INTEGER; adr: SYSTEM.ADRINT;
	BEGIN
		adr := SYSTEM.ADR(r);
		IF ~LEHost THEN INC(adr, 4) END;
		SYSTEM.GET(adr, x);
		RETURN x
	END LoWord;

	(* VAR is necessary here to avoid problems with GCC optimizer *)
	PROCEDURE HiWord (VAR r: REAL): INTEGER;
		VAR x: INTEGER; adr: SYSTEM.ADRINT;
	BEGIN
		adr := SYSTEM.ADR(r);
		IF LEHost THEN INC(adr, 4) END;
		SYSTEM.GET(adr, x);
		RETURN x
	END HiWord;

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

	(* utf8 strings *)

	PROCEDURE PutUtf8* (VAR str: ARRAY OF SHORTCHAR; val: INTEGER; VAR idx: INTEGER);
	BEGIN
		ASSERT((val >= 0) & (val < 65536));
		IF val < 128 THEN
			str[idx] := SHORT(CHR(val)); INC(idx)
		ELSIF val < 2048 THEN
			str[idx] := SHORT(CHR(val DIV 64 + 192)); INC(idx);
			str[idx] := SHORT(CHR(val MOD 64 + 128)); INC(idx)
		ELSE
			str[idx] := SHORT(CHR(val DIV 4096 + 224)); INC(idx);
			str[idx] := SHORT(CHR(val DIV 64 MOD 64 + 128)); INC(idx);
			str[idx] := SHORT(CHR(val MOD 64 + 128)); INC(idx)
		END
	END PutUtf8;

	PROCEDURE GetUtf8* (IN str: ARRAY OF SHORTCHAR; VAR val, idx: INTEGER);
		VAR ch: SHORTCHAR;
	BEGIN
		ch := str[idx]; INC(idx);
		IF ch < 80X THEN
			val := ORD(ch)
		ELSIF ch < 0E0X THEN
			val := ORD(ch) - 192;
			ch := str[idx]; INC(idx); val := val * 64 + ORD(ch) - 128
		ELSE
			val := ORD(ch) - 224;
			ch := str[idx]; INC(idx); val := val * 64 + ORD(ch) - 128;
			ch := str[idx]; INC(idx); val := val * 64 + ORD(ch) - 128
		END
	END GetUtf8;

	(* ------------------------- Log Output ------------------------- *)

	PROCEDURE LogW* (ch: SHORTCHAR);
	BEGIN Console.Char(ch)
	END LogW;

	PROCEDURE LogWStr* (IN s: ARRAY OF SHORTCHAR);
	BEGIN Console.String(s)
	END LogWStr;

	PROCEDURE LogWNum* (i, len: INTEGER);
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
		IF AdrSize = 2 THEN	(* Z80 and other 8-bit / 16-bit CPUs *)
			SetSize := Alignment; MaxSet := SHORT(SetSize*8 - 1);
			IndexSize := 2; MaxIndex := MAX(SHORTINT)
		ELSE
			SetSize := 4; MaxSet := 31;
			IndexSize := 4;	(* size of the type of array index, see also SYSTEM_ARRLEN *)
			MaxIndex := MAX(INTEGER)
		END
	END GetProperties;

	(* ------------------------- parameter handling -------------------------*)

	PROCEDURE ScanOptions(VAR s: ARRAY OF SHORTCHAR; VAR opt: SET);
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
			| "w": opt := opt / {widechar}
			| "1", "3"(*undocumented*), "C", "7": Lang := s[i]
			| "G": opt := opt / {gpcp}
			| "O": opt := opt / {oakwood}
			(* target machine address size and alignment
					"21": 16-bit addresses, SIZE(SET) = 1 byte (e.g. Zilog Z80 CPU).
					"44": 32-bit addresses, 32-bit alignment (e.g. 32-bit Unix/Linux on x86).
					"48": 32-bit addresses, 64-bit alignment (e.g. 32-bit Windows on x86, 32-bit Linux on ARM).
					"88": 64-bit addresses, 64-bit alignment (e.g. 64-bit platforms).
			*)
			| "2":
				IF s[i+1] = "1" THEN AdrSize := 2; Alignment := 1; INC(i)
				ELSIF s[i+1] = "2" THEN AdrSize := 2; Alignment := 2; INC(i)
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

	PROCEDURE OpenPar*(IN title, cmd: ARRAY OF SHORTCHAR);	(* prepare for a sequence of translations *)
		VAR s: ARRAY 256 OF SHORTCHAR;
	BEGIN
		IF CmdArgs.Count = 0 THEN stop := TRUE;
			Console.Ln;
			Console.String(title); Console.Ln;
			Console.String("Copyright (c) Software Templ OG, 1995-2020 & VEDAsoft Oberon Club, 2013-2025"); Console.Ln; Console.Ln;
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
			Console.String('       "f": allow "proper FOR" (concept by Oleg Komlev)             (off)'); Console.Ln;
			Console.String('       "d": translate a module into dynamic library (.dll/.so)      (off)'); Console.Ln;
			Console.String('       "w": UTF-8 to UCS-2 wide character transcoding enabled       (off)'); Console.Ln;
			Console.String('  "1": Oberon "2": Oberon-2 "C": Component Pascal "7": Oberon-07 "3": Oberon-3'); Console.Ln;
			Console.String('  "21": 16-bit addresses, SIZE(SET) = 1 byte (e.g. Zilog Z80 CPU)'); Console.Ln;
			Console.String('  "44": 32-bit addresses, 32-bit alignment (32-bit UNIX/Linux on x86)'); Console.Ln;
			Console.String('  "48": 32-bit addresses, 64-bit alignment (32-bit Windows, 32-bit Linux/ARMv7)'); Console.Ln;
			Console.String('  "88": 64-bit addresses, 64-bit alignment (64-bit platforms)'); Console.Ln
		ELSE
			(* set options to initial defaults *)
			Lang := "C";	(* default input language: Component Pascal *)
			AdrSize := -1;	(* undefined *)
			glbopt := defopt; S := 1;
			CmdArgs.Get(1, s); stop := FALSE;
			WHILE s[0] = OptionChar DO ScanOptions(s, glbopt); INC(S); CmdArgs.Get(S, s) END;
			(* Record global option settings for this command line *)
			GlobalAdrSize := AdrSize; GlobalAlignment := Alignment;	GlobalLang := Lang
		END
	END OpenPar;

	PROCEDURE InitOptions*;	(* get the options for one translation *)
		VAR s: ARRAY 256 OF SHORTCHAR;
	BEGIN
		Alignment := GlobalAlignment; AdrSize := GlobalAdrSize;
		opt := glbopt;
		CmdArgs.Get(S, s);
		WHILE s[0] = OptionChar DO ScanOptions(s, opt); INC(S); CmdArgs.Get(S, s) END;
		curpos := 256; errpos := curpos; lasterrpos := curpos - 10;
		GetProperties
	END InitOptions;

	(* get the source for one translation; msg is "translating" or "compiling" *)
	PROCEDURE Init* (IN msg: ARRAY OF SHORTCHAR; VAR done: BOOLEAN);
		VAR T: Texts.Text; endpos: INTEGER; s: ARRAY 256 OF SHORTCHAR;
	BEGIN
		done := FALSE; curpos := 0;
		IF stop OR (S > CmdArgs.Count) THEN noerr := TRUE; RETURN END;
		CmdArgs.Get(S, s);
		endpos := LEN(s$) - 4;
		IF    Strings.Pos(".ob1", s, 1) = endpos THEN Lang := "1"
		ELSIF Strings.Pos(".ob2", s, 1) = endpos THEN Lang := "2"
		ELSIF Strings.Pos(".ob3", s, 1) = endpos THEN Lang := "3"
		ELSIF Strings.Pos(".ob7", s, 1) = endpos THEN Lang := "7"
		ELSE Lang := GlobalLang
		END;
		NEW(T); Texts.Open(T, s);
		LogWStr(s);
		IF Files.Old(s) = NIL THEN LogWStr(" not found"); LogWLn; noerr := FALSE
		ELSE
			Texts.OpenReader(inR, T, 0);
			LogWStr("  "); LogWStr(msg);
			done := TRUE; noerr := TRUE
		END;
		INC(S);
		level := 0; errpos := curpos; lasterrpos := curpos - 10
	END Init;

	(* ------------------------- read source text -------------------------*)
	PROCEDURE ^ err*(n: SHORTINT);

	PROCEDURE GetCh (OUT longch: CHAR; chk: BOOLEAN);	(* read next character from source text, 0X if eof *)
		VAR ch: SHORTCHAR;
	BEGIN
		IF ~(widechar IN opt) THEN Texts.Read(inR, ch); longch := ch
		ELSIF ~Texts.ReadLong(inR, longch) THEN longch := "?";
			IF chk THEN err(-3) END
		END;
		IF longch = 0DX THEN curpos := (curpos DIV 256 + 1) * 256
		ELSIF curpos MOD 256 # 255 THEN INC(curpos)
			(* at 255 means:  >= 255 *)
		END;
		IF (longch < 09X) & ~inR.eot THEN longch := " " END
	END GetCh;

	PROCEDURE Get* (OUT longch: CHAR);	(* read next character from source text, 0X if eof *)
	BEGIN
		GetCh(longch, TRUE)
	END Get;

	PROCEDURE GetNoChk* (OUT longch: CHAR);	(* read next character from source text, 0X if eof *)
	BEGIN
		GetCh(longch, FALSE)
	END GetNoChk;

	PROCEDURE MakeFileName (IN name: ARRAY OF SHORTCHAR; OUT FName: ARRAY OF SHORTCHAR; IN ext: ARRAY OF SHORTCHAR);
		VAR i, j: INTEGER; ch: SHORTCHAR;
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
	BEGIN
		IF n >= 0 THEN LogWStr("  err ")
		ELSE LogWStr("  warning "); n := SHORT(-n)
		END;
		LogWNum(n, 1);
		LogWStr("  ");
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

	PROCEDURE FPrint* (VAR fp: INTEGER; val: INTEGER);
		VAR c: INTEGER;
	BEGIN
		(* CRC32, high bit first, pre & post inverted *)
		c := ORD(BITS(fp * 256) / BITS(crc32tab[ORD(BITS(fp DIV 1000000H) / BITS(val DIV 1000000H)) MOD 256]));
		c := ORD(BITS(c * 256) / BITS(crc32tab[ORD(BITS(c DIV 1000000H) / BITS(val DIV 10000H)) MOD 256]));
		c := ORD(BITS(c * 256) / BITS(crc32tab[ORD(BITS(c DIV 1000000H) / BITS(val DIV 100H)) MOD 256]));
		fp := ORD(BITS(c * 256) / BITS(crc32tab[ORD(BITS(c DIV 1000000H) / BITS(val)) MOD 256]))
	END FPrint;

	PROCEDURE FPrintSet* (VAR fp: INTEGER; set: SET);
	BEGIN FPrint(fp, SYSTEM.VAL(INTEGER, set))
	END FPrintSet;

	PROCEDURE FPrintReal* (VAR fp: INTEGER; real: SHORTREAL);
	BEGIN FPrint(fp, SYSTEM.VAL(INTEGER, real))
	END FPrintReal;

	PROCEDURE FPrintLReal* (VAR fp: INTEGER; lr: REAL);
	BEGIN FPrint(fp, LoWord(lr)); FPrint(fp, HiWord(lr))
	END FPrintLReal;

	(* ------------------------- Read Symbol File ------------------------- *)

	PROCEDURE ReadInt32 (VAR i: INTEGER);
		VAR b: BYTE; x: INTEGER;
	BEGIN
		Files.ReadByte(oldSF, b); x := b MOD 256;
		ChkSum(checksum, b);
		Files.ReadByte(oldSF, b); x := x + 100H * (b MOD 256);
		ChkSum(checksum, b);
		Files.ReadByte(oldSF, b); x := x + 10000H * (b MOD 256);
		ChkSum(checksum, b);
		Files.ReadByte(oldSF, b); i := x + 1000000H * b;
		ChkSum(checksum, b)
	END ReadInt32;

	PROCEDURE SymRCh* (VAR ch: SHORTCHAR);
	BEGIN Files.ReadChar(oldSF, ch); ChkSum(checksum, SYSTEM.VAL(BYTE, ch))
	END SymRCh;

	PROCEDURE SymRInt* (): LONGINT;
		VAR b: BYTE; s: INTEGER; n: LONGINT;
	BEGIN s := 0; n := 0; Files.ReadByte(oldSF, b);
		IF ~oldSF.eof THEN ChkSum(checksum, b) END;
		WHILE b < 0 DO INC(n, ASH(LONG(b + 128), s)); INC(s, 7);
			Files.ReadByte(oldSF, b); ChkSum(checksum, b)
		END;
		INC(n, ASH(LONG((b + 64) MOD 128 - 64), s));
		RETURN n
	END SymRInt;

	PROCEDURE SymRSet* (VAR s: SET);
	BEGIN s := SYSTEM.VAL(SET, SHORT(SymRInt()))
	END SymRSet;

	PROCEDURE SymRReal* (VAR r: SHORTREAL);
	BEGIN ReadInt32(SYSTEM.VAL(INTEGER, r))
	END SymRReal;

	PROCEDURE SymRLReal* (VAR lr: REAL);
		VAR h, l: INTEGER;
	BEGIN
		ReadInt32(l); ReadInt32(h); lr := Compound(l, h)
	END SymRLReal;

	PROCEDURE CloseOldSym*;
	END CloseOldSym;

	PROCEDURE OldSym*(IN modName: ARRAY OF SHORTCHAR; VAR done: BOOLEAN);
		VAR ch: SHORTCHAR; fileName: FileName;
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

	PROCEDURE WriteInt32 (i: INTEGER);
	BEGIN
		ChkSum(checksum, i);
		Files.WriteByte(newSF, SHORT(SHORT(i MOD 256))); i := i DIV 256;
		ChkSum(checksum, i);
		Files.WriteByte(newSF, SHORT(SHORT(i MOD 256))); i := i DIV 256;
		ChkSum(checksum, i);
		Files.WriteByte(newSF, SHORT(SHORT(i MOD 256))); i := i DIV 256;
		ChkSum(checksum, i);
		Files.WriteByte(newSF, SHORT(SHORT(i MOD 256)))
	END WriteInt32;

	PROCEDURE SymWCh* (ch: SHORTCHAR);
	BEGIN ChkSum(checksum, ORD(ch)); Files.WriteChar(newSF, ch)
	END SymWCh;

	PROCEDURE SymWInt* (x: LONGINT);
	BEGIN
		WHILE (x < - 64) OR (x > 63) DO
			ChkSum(checksum, SHORT(x) MOD 128 - 128);
			Files.WriteByte(newSF, SHORT(SHORT(SHORT(x) MOD 128 - 128)));
			x := x DIV 128
		END;
		ChkSum(checksum, SHORT(x) MOD 128);
		Files.WriteByte(newSF, SHORT(SHORT(SHORT(x) MOD 128)))
	END SymWInt;

	PROCEDURE SymWSet* (s: SET);
	BEGIN SymWInt(SYSTEM.VAL(INTEGER, s))
	END SymWSet;

	PROCEDURE SymWReal* (r: SHORTREAL);
	BEGIN WriteInt32(SYSTEM.VAL(INTEGER, r))
	END SymWReal;

	PROCEDURE SymWLReal* (lr: REAL);
	BEGIN WriteInt32(LoWord(lr)); WriteInt32(HiWord(lr))
	END SymWLReal;

	PROCEDURE RegisterNewSym*;
	BEGIN
		IF (modName # "SYSTEM") OR (mainprog IN opt) THEN Files.Register(newSFile) END
	END RegisterNewSym;

	PROCEDURE DeleteNewSym*;
	END DeleteNewSym;

	PROCEDURE NewSym*(VAR modName: ARRAY OF SHORTCHAR);
		VAR fileName: FileName;
	BEGIN MakeFileName(modName, fileName, SFext);
		newSFile := Files.New(fileName);
		IF newSFile # NIL THEN Files.Set(newSF, newSFile, 0); Files.WriteChar(newSF, SFtag)
		ELSE err(153)
		END
	END NewSym;

	(* ------------------------- Write Header & Body Files ------------------------- *)

	PROCEDURE Write* (ch: SHORTCHAR);
	BEGIN
		IF currFile # DummyFile THEN Files.WriteChar(R[currFile], ch) END
	END Write;

	PROCEDURE WriteString* (IN s: ARRAY OF SHORTCHAR);
		VAR i: INTEGER;
	BEGIN
		IF currFile # DummyFile THEN
			i := LEN(s$);
			Files.WriteBytes(R[currFile], SYSTEM.THISARR(SYSTEM.ADR(s), i), i)
		END
	END WriteString;

	PROCEDURE WriteHex* (i: INTEGER);
		VAR s: ARRAY 3 OF SHORTCHAR; digit: INTEGER;
	BEGIN
		digit := SHORT(i) DIV 16;
		IF digit < 10 THEN s[0] := SHORT(CHR(ORD("0") + digit))
		ELSE s[0] := SHORT(CHR(ORD("a") - 10 + digit))
		END;
		digit := SHORT(i) MOD 16;
		IF digit < 10 THEN s[1] := SHORT(CHR(ORD("0") + digit))
		ELSE s[1] := SHORT(CHR(ORD("a") - 10 + digit))
		END;
		s[2] := 0X;
		WriteString(s)
	END WriteHex;

	PROCEDURE WriteInt* (i: LONGINT);
		VAR s: ARRAY 20 OF SHORTCHAR; i1: LONGINT; k: INTEGER;
	BEGIN
		IF (i = MIN(INTEGER)) OR (i = MIN(LONGINT)) THEN	(* requires special bootstrap for 64 bit *)
			(* ABS(MIN(INTEGER)) is one more than MAX(INTEGER), causing problems representing the value
				as a minus sign followed by absoute value. Therefore represent as -MaxInt - 1.
				For INTEGER this avoids a compiler warning 'this decimal constant is unsigned only in ISO C90'.
				For LONGINT it is the only way to represent MinLInt. *)
			Write("("); WriteInt(i+1); WriteString("-1)")	(* requires special bootstrap for 64 bit *)
		ELSE i1 := ABS(i);
			s[0] := SHORT(CHR(i1 MOD 10 + ORD("0"))); i1 := i1 DIV 10; k := 1;
			WHILE i1 > 0 DO s[k] := SHORT(CHR(i1 MOD 10 + ORD("0"))); i1 := i1 DIV 10; INC(k) END;
			IF i < 0 THEN s[k] := "-"; INC(k) END;
			WHILE k > 0 DO  DEC(k); Write(s[k]) END;
			IF (ansi IN opt) & ((i < MIN(INTEGER)) OR (i > MAX(INTEGER))) THEN WriteString("LL") END
		END
	END WriteInt;

	PROCEDURE WriteReal* (r: REAL; suffx: SHORTCHAR);
		VAR W: Texts.Writer; T: Texts.Text; R: Texts.Reader; s: ARRAY 32 OF SHORTCHAR; ch: SHORTCHAR; i: INTEGER;
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

	PROCEDURE WriteLn*;
	BEGIN WriteString(Platform.NewLine)
	END WriteLn;

	PROCEDURE WriteTab*;
	BEGIN
		WriteString("  ")
	END WriteTab;

	PROCEDURE WriteModPos*;
	BEGIN
		IF ansi IN opt THEN WriteString("(CHAR*)") END;
		Write('"'); WriteString(modName); WriteString('", '); WriteInt(errpos)
	END WriteModPos;

	PROCEDURE Append (VAR R: Files.Rider; F: Files.File);
		VAR R1: Files.Rider; buffer: ARRAY 4096 OF SHORTCHAR;
	BEGIN
		IF F # NIL THEN
			Files.Set(R1, F, 0);
			Files.ReadBytes(R1, SYSTEM.THISARR(SYSTEM.ADR(buffer), LEN(buffer)), LEN(buffer));
			WHILE LEN(buffer) - R1.res > 0 DO
				Files.WriteBytes(R, SYSTEM.THISARR(SYSTEM.ADR(buffer), LEN(buffer)), LEN(buffer) - R1.res);
				Files.ReadBytes(R1, SYSTEM.THISARR(SYSTEM.ADR(buffer), LEN(buffer)), LEN(buffer))
			END
		END
	END Append;

	PROCEDURE OpenFiles*(VAR moduleName: ARRAY OF SHORTCHAR);
		VAR FName: FileName;
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
			IF ~(foreign IN opt) THEN
				MakeFileName(moduleName, FName, ".c0"); Append(R[BodyFile], Files.Old(FName))
			END
		END
	END OpenFiles;

	PROCEDURE CloseFiles*;
		VAR FName: FileName; res: INTEGER; body: BOOLEAN;
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

	PROCEDURE InitHost;
		VAR test: SHORTINT; lo: SHORTCHAR;
	BEGIN
		test := 1; SYSTEM.GET(SYSTEM.ADR(test), lo); LEHost := lo = 1X;
		InfReal := SYSTEM.VAL(SHORTREAL, InfRealPat);
		MinReal32 := SYSTEM.VAL(SHORTREAL, MinReal32Pat);
		MaxReal32 := SYSTEM.VAL(SHORTREAL, MaxReal32Pat);
		MinReal64 := Compound(MinReal64PatL, MinReal64PatH);
		MaxReal64 := Compound(MaxReal64PatL, MaxReal64PatH)
	END InitHost;

BEGIN InitCrcTab; InitHost; Texts.OpenWriter(W);
  CmdArgs.GetEnv("OBERON", OBERON);
  IF OBERON = "" THEN OBERON := "." END;
  Files.SetSearchPath(OBERON)
END OfrontOPM.
