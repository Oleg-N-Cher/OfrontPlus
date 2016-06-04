MODULE OfrontOPM;	(* RC 6.3.89 / 28.6.89, J.Templ 10.7.89 / 22.7.96  *)
(* constants needed for C code generation 

	31.1.2007 jt synchronized with BlackBox version, in particular PromoteIntConstToLInt added
*)

	IMPORT SYSTEM, Texts := CmdlnTexts, Files, Args, Console;

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
		lineno* = 11;	(* emit line numbers rather than text positions in error messages *)
		defopt* = {inxchk, typchk, ptrinit, ansi, assert};	(* default options *)

		nilval* = 0;
(*
		MinRealPat = 0FF7FFFFFH;	(* most  negative, 32-bit pattern, -3.40282346E38 *)
		MinLRealPatL = 0FFFFFFFFH;	(* most  negative, lower 32-bit pattern *)
		MinLRealPatH = 0FFEFFFFFH;	(* most  negative, higher 32-bit pattern *)
		MaxRealPat = 7F7FFFFFH; (*3.40282346E38*)
		MaxLRealPatL = -1;
		MaxLRealPatH = 7FEFFFFFH;
*)

		MaxRExp* = 38;	MaxLExp* = 308;	MaxHDig* = 8;

		MinHaltNr* = 0;
		MaxHaltNr* = 255;
		MaxSysFlag* = 1;

		MaxCC* = -1;	(* SYSTEM.CC, GETREG, PUTREG; not implementable in Ofront *)
		MinRegNr* = 0;
		MaxRegNr* = -1;

		LANotAlloc* = -1;	(* XProc link adr initialization *)
		ConstNotAlloc* = -1;	(* for allocation of string and real constants *)
		TDAdrUndef* = -1;	(* no type desc allocated *)

		MaxCases* = 128;
		MaxCaseRange* = 512;

		MaxStruct* = 255;

		(* maximal number of pointer fields in a record: *)
		MaxPtr* = MAX(LONGINT);

		(* maximal number of global pointers per module: *)
		MaxGPtr* = MAX(LONGINT);

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
		ByteSize*, CharSize*, BoolSize*, SIntSize*, IntSize*,
		LIntSize*, SetSize*, RealSize*, LRealSize*, PointerSize*, ProcSize*, RecSize*,
		CharAlign*, BoolAlign*, SIntAlign*, IntAlign*,
		LIntAlign*, SetAlign*, RealAlign*, LRealAlign*, PointerAlign*, ProcAlign*, RecAlign*,
		ByteOrder*, BitOrder*, MaxSet*: INTEGER;
		MinSInt*, MinInt*, MinLInt*, MaxSInt*, MaxInt*, MaxLInt*, MaxIndex*: LONGINT;
		MinReal*, MaxReal*, MinLReal*, MaxLReal*: LONGREAL;

		noerr*: BOOLEAN;
		curpos*, errpos*: LONGINT;	(* character and error position in source file *)
		breakpc*: LONGINT;	(* set by OPV.Init *)
		currFile*: INTEGER;	(* current output file *)
		level*: INTEGER;	(* procedure nesting level *)
		pc*, entno*: INTEGER;  (* entry number *)
		modName*: ARRAY 32 OF CHAR;
		objname*: ARRAY 64 OF CHAR;

		opt*, glbopt*: SET;

		lasterrpos: LONGINT;
		inR: Texts.Reader;
		Log: Texts.Text;
		W: Texts.Writer;
		oldSF, newSF: Files.Rider;
		R: ARRAY 3 OF Files.Rider;
		oldSFile, newSFile, HFile, BFile, HIFile: Files.File;

		S: INTEGER;
		stop, useLineNo: BOOLEAN;


	(* ------------------------- Log Output ------------------------- *)

	PROCEDURE LogW*(ch: CHAR);
	BEGIN Console.Char(ch)
	END LogW;

	PROCEDURE LogWStr*(s: ARRAY OF CHAR);
	BEGIN Console.String(s)
	END LogWStr;
	
	PROCEDURE LogWNum*(i, len: LONGINT);
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
			| "v": opt := opt / {vcpp};
			| "r": opt := opt / {ranchk}
			| "t": opt := opt / {typchk}
			| "a": opt := opt / {assert}
			| "k": opt := opt / {ansi}
			| "p": opt := opt / {ptrinit}
			| "i": opt := opt / {include0}
			| "l": opt := opt / {lineno}
			ELSE LogWStr("  warning: option "); LogW(OptionChar); LogW(s[i]); LogWStr(" ignored"); LogWLn
			END ;
			INC(i)
		END
	END ScanOptions;

	PROCEDURE ^GetProperties;

	PROCEDURE OpenPar*;	(* prepare for a sequence of translations *)
		VAR s: ARRAY 256 OF CHAR;
	BEGIN
		IF Args.argc = 1 THEN stop := TRUE;
			Console.Ln;
			Console.String("OfrontPlus - Oberon-2 to C Translator v1.0 #1"); Console.Ln;
			Console.String("Copyright (c) Software Templ OEG, 1995-2007 & VEDAsoft Oberon Club, 2013-2016"); Console.Ln; Console.Ln;
			Console.String("SYNOPSIS"); Console.Ln;
				Console.String('  command = "ofront+" options {file options}.'); Console.Ln;
			Console.String('  options = ["-" {option} ].'); Console.Ln;
			Console.String('  option  = "m" | "s" | "e" | "i" | "l" | "k" | "r" | "x" | "a" | "p" | "t".'); Console.Ln;
			Console.Ln;
		ELSE
			GetProperties;
			glbopt := defopt; S := 1; s := "";
			Args.Get(1, s); stop := FALSE;
			WHILE s[0] = OptionChar DO ScanOptions(s, glbopt); INC(S); s := ""; Args.Get(S, s) END
		END
	END OpenPar;

	PROCEDURE InitOptions*;	(* get the options for one translation *)
		VAR s: ARRAY 256 OF CHAR;
	BEGIN
		opt := glbopt; s := ""; Args.Get(S, s);
		WHILE s[0] = OptionChar DO ScanOptions(s, opt); INC(S); s := ""; Args.Get(S, s) END ;
		IF lineno IN opt THEN useLineNo := TRUE; curpos := 256; errpos := curpos; lasterrpos := curpos - 10
		ELSE useLineNo := FALSE;
		END
	END InitOptions;

	PROCEDURE Init*(VAR done: BOOLEAN);	(* get the source for one translation *)
		VAR T: Texts.Text; beg, end, time: LONGINT;
			s: ARRAY 256 OF CHAR;
	BEGIN
		done := FALSE; curpos := 0;
		IF stop OR (S >= Args.argc) THEN RETURN END ;
		s := ""; Args.Get(S, s);
		NEW(T); Texts.Open(T, s);
		LogWStr(s);
		IF T.len = 0 THEN LogWStr(" not found"); LogWLn
		ELSE
			Texts.OpenReader(inR, T, 0);
			LogWStr("  translating");
			done := TRUE
		END ;
		INC(S);
		level := 0; noerr := TRUE; errpos := curpos; lasterrpos := curpos -10
	END Init;

	(* ------------------------- read source text -------------------------*)
	
	PROCEDURE Get*(VAR ch: CHAR);	(* read next character from source text, 0X if eof *)
	BEGIN
		Texts.Read(inR, ch);
		IF useLineNo THEN 
			IF ch = 0DX THEN curpos := (curpos DIV 256 + 1) * 256
			ELSIF curpos MOD 256 # 255 THEN INC(curpos)
				(* at 255 means:  >= 255 *)
			END
		ELSIF ch = 0DX THEN
			curpos := Texts.Pos(inR)	(* supports CR LF mapping *)
		ELSE
			INC(curpos)
		END ;
		IF (ch < 09X) & ~inR.eot THEN ch := " " END
	END Get;
	
	PROCEDURE MakeFileName(VAR name, FName: ARRAY OF CHAR; ext: ARRAY OF CHAR);
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

	PROCEDURE LogErrMsg(n: INTEGER);
		VAR S: Texts.Scanner; T: Texts.Text; ch: CHAR; i: INTEGER;
			buf: ARRAY 1024 OF CHAR;
	BEGIN
		IF n >= 0 THEN LogWStr("  err ")
		ELSE LogWStr("  warning "); n := -n
		END ;
		LogWNum(n, 1);
		LogWStr("  ");
		NEW(T); Texts.Open(T, "OfrontErrors.Text"); Texts.OpenScanner(S, T, 0);
		REPEAT S.line := 0;
			REPEAT Texts.Scan(S) UNTIL S.eot OR (S.line # 0)
		UNTIL S.eot OR (S.class = Texts.Int) & (S.i = n);
		IF ~S.eot THEN Texts.Read(S, ch); i := 0;
			WHILE ~S.eot & (ch # 0DX) DO buf[i] := ch; INC(i); Texts.Read(S, ch) END ;
			buf[i] := 0X; LogWStr(buf);
		END
	END LogErrMsg;

	PROCEDURE Mark*(n: INTEGER; pos: LONGINT);
	BEGIN
		IF useLineNo THEN
			IF n >= 0 THEN
				noerr := FALSE;
				IF (pos < lasterrpos) OR (lasterrpos + 9 < pos) THEN lasterrpos := pos; LogWLn; LogWStr("  ");
					IF n < 249 THEN LogWStr("  line "); LogWNum(pos DIV 256, 1);
						LogWStr("  pos "); LogWNum(pos MOD 256, 1); LogErrMsg(n)
					ELSIF n = 255 THEN LogWStr("  line "); LogWNum(pos DIV 256, 1);
						LogWStr("  pos "); LogWNum(pos MOD 256, 1); LogWStr("  pc "); LogWNum(breakpc, 1)
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
					LogWStr("  line "); LogWNum(pos DIV 256, 1); LogWStr("  pos "); LogWNum(pos MOD 256, 1)
				END ;
				LogErrMsg(n);
				IF pos < 0 THEN LogWLn END
			END
		ELSE
			IF n >= 0 THEN
				noerr := FALSE;
				IF (pos < lasterrpos) OR (lasterrpos + 9 < pos) THEN lasterrpos := pos; LogWLn; LogWStr("  ");
					IF n < 249 THEN LogWStr("  pos"); LogWNum(pos, 6); LogErrMsg(n)
					ELSIF n = 255 THEN LogWStr("pos"); LogWNum(pos, 6); LogWStr("  pc "); LogWNum(breakpc, 1)
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
				IF pos >= 0 THEN LogWLn; LogWStr("  pos"); LogWNum(pos, 6) END ;
				LogErrMsg(n);
				IF pos < 0 THEN LogWLn END
			END
		END
	END Mark;

	PROCEDURE err*(n: INTEGER);
	BEGIN 
		IF useLineNo & (errpos MOD 256 = 255) THEN (* line underflow from OPS.Get *)
			Mark(n, errpos + 1)
		ELSE
			Mark(n, errpos)
		END
	END err;

	PROCEDURE FPrint*(VAR fp: LONGINT; val: LONGINT);
	BEGIN
		fp := SYSTEM.ROT(SYSTEM.VAL(LONGINT, SYSTEM.VAL(SET, fp) / SYSTEM.VAL(SET, val)), 1)
	END FPrint;

	PROCEDURE FPrintSet*(VAR fp: LONGINT; set: SET);
	BEGIN FPrint(fp, SYSTEM.VAL(LONGINT, set))
	END FPrintSet;

	PROCEDURE FPrintReal*(VAR fp: LONGINT; real: REAL);
	BEGIN FPrint(fp, SYSTEM.VAL(LONGINT, real))
	END FPrintReal;

	PROCEDURE FPrintLReal*(VAR fp: LONGINT; lr: LONGREAL);
		VAR l, h: LONGINT;
	BEGIN
		SYSTEM.GET(SYSTEM.ADR(lr), l); SYSTEM.GET(SYSTEM.ADR(lr)+4, h);
		FPrint(fp, l); FPrint(fp, h)
	END FPrintLReal;

	(* ------------------------- initialization ------------------------- *)

	PROCEDURE GetProperty(VAR S: Texts.Scanner; name: ARRAY OF CHAR; VAR size, align: INTEGER);
	BEGIN
		IF (S.class = Texts.Name) & (S.s = name) THEN Texts.Scan(S);
			IF S.class = Texts.Int THEN size := SHORT(S.i); Texts.Scan(S) ELSE Mark(-157, -1) END ;
			IF S.class = Texts.Int THEN align := SHORT(S.i); Texts.Scan(S) ELSE Mark(-157, -1) END
		ELSE Mark(-157, -1)
		END
	END GetProperty;

	PROCEDURE GetProperties();
		VAR T: Texts.Text; S: Texts.Scanner;
	BEGIN
		(* default characteristics *)
		ByteSize := 1; CharSize := 1; BoolSize := 1; SIntSize := 1; IntSize := 2; LIntSize := 4;
		SetSize := 4; RealSize := 4; LRealSize := 8; ProcSize := 4; PointerSize := 4; RecSize := 1;
		CharAlign := 1; BoolAlign := 1; SIntAlign := 1; IntAlign := 2; LIntAlign := 4;
		SetAlign := 4; RealAlign := 4; LRealAlign := 8; ProcAlign := 4; PointerAlign := 4; RecAlign := 1;
		MinSInt := -80H; MinInt := -8000H; MinLInt :=  80000000H;	(*-2147483648*)
		MaxSInt := 7FH; MaxInt := 7FFFH; MaxLInt := 7FFFFFFFH;	(*2147483647*)
		MaxSet := 31;
		(* read Ofront.par *)
		NEW(T); Texts.Open(T, "Ofront.par");
		IF T.len # 0 THEN
			Texts.OpenScanner(S, T, 0); Texts.Scan(S);
			GetProperty(S, "CHAR", CharSize, CharAlign);
			GetProperty(S, "BOOLEAN", BoolSize, BoolAlign);
			GetProperty(S, "SHORTINT", SIntSize, SIntAlign);
			GetProperty(S, "INTEGER", IntSize, IntAlign);
			GetProperty(S, "LONGINT", LIntSize, LIntAlign);
			GetProperty(S, "SET", SetSize, SetAlign);
			GetProperty(S, "REAL", RealSize, RealAlign);
			GetProperty(S, "LONGREAL", LRealSize, LRealAlign);
			GetProperty(S, "PTR", PointerSize, PointerAlign);
			GetProperty(S, "PROC", ProcSize, ProcAlign);
			GetProperty(S, "RECORD", RecSize, RecAlign);
			(* Size = 0: natural size aligned to next power of 2 up to RecAlign; e.g. i960
				Size = 1; size and alignment follows from field types but at least RecAlign; e.g, SPARC, MIPS, PowerPC
			*)
			GetProperty(S, "ENDIAN", ByteOrder, BitOrder);	(*currently not used*)
		ELSE Mark(-156, -1)
		END ;
		IF RealSize = 4 THEN MaxReal := 3.40282346D38
		ELSIF RealSize = 8 THEN MaxReal := 1.7976931348623157D307 * 9.999999
			(*should be 1.7976931348623157D308 *)
		END ;
		IF LRealSize = 4 THEN MaxLReal := 3.40282346D38
		ELSIF LRealSize = 8 THEN MaxLReal := 1.7976931348623157D307 * 9.999999
			(*should be 1.7976931348623157D308 *)
		END ;
		MinReal := -MaxReal;
		MinLReal := -MaxLReal;
		IF IntSize = 4 THEN MinInt := MinLInt; MaxInt := MaxLInt END ;
		IF SIntSize = 2 THEN MinSInt := -8000H; MaxSInt := 7FFFH END ;
		MaxSet := SetSize * 8 - 1;
		MaxIndex := MaxLInt
	END GetProperties;

	(* ------------------------- Read Symbol File ------------------------- *)

	PROCEDURE SymRCh*(VAR ch: CHAR);
	BEGIN Files.Read(oldSF, ch)
	END SymRCh;
	
	PROCEDURE SymRInt*(): LONGINT;
		VAR k: LONGINT;
	BEGIN Files.ReadNum(oldSF, k); RETURN k
	END SymRInt;
		
	PROCEDURE SymRSet*(VAR s: SET);
	BEGIN Files.ReadNum(oldSF, SYSTEM.VAL(LONGINT, s))
	END SymRSet;
	
	PROCEDURE SymRReal*(VAR r: REAL);
	BEGIN Files.ReadReal(oldSF, r)
	END SymRReal;
	
	PROCEDURE SymRLReal*(VAR lr: LONGREAL);
	BEGIN Files.ReadLReal(oldSF, lr)
	END SymRLReal;
	
	PROCEDURE CloseOldSym*;
	END CloseOldSym;

	PROCEDURE OldSym*(VAR modName: ARRAY OF CHAR; VAR done: BOOLEAN);
		VAR ch: CHAR; fileName: FileName;
	BEGIN MakeFileName(modName, fileName, SFext);
		oldSFile := Files.Old(fileName); done := oldSFile # NIL;
		IF done THEN
			Files.Set(oldSF, oldSFile, 0); Files.Read(oldSF, ch);
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
	BEGIN Files.Write(newSF, ch)
	END SymWCh;

	PROCEDURE SymWInt*(i: LONGINT);
	BEGIN Files.WriteNum(newSF, i)
	END SymWInt;

	PROCEDURE SymWSet*(s: SET);
	BEGIN Files.WriteNum(newSF, SYSTEM.VAL(LONGINT, s))
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
		IF newSFile # NIL THEN Files.Set(newSF, newSFile, 0); Files.Write(newSF, SFtag)
		ELSE err(153)
		END
	END NewSym;

	(* ------------------------- Write Header & Body Files ------------------------- *)

	PROCEDURE Write*(ch: CHAR);
	BEGIN Files.Write(R[currFile], ch)
	END Write;

	PROCEDURE WriteString*(s: ARRAY [1] OF CHAR);
		VAR i: INTEGER;
	BEGIN i := 0;
		WHILE s[i] # 0X DO INC(i) END ;
		Files.WriteBytes(R[currFile], s, i)
	END WriteString;

	PROCEDURE WriteStringVar*(VAR s: ARRAY OF CHAR);
		VAR i: INTEGER;
	BEGIN i := 0;
		WHILE s[i] # 0X DO INC(i) END ;
		Files.WriteBytes(R[currFile], s, i)
	END WriteStringVar;

	PROCEDURE WriteHex* (i: LONGINT);
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
		VAR s: ARRAY 20 OF CHAR; i1, k: LONGINT;
	BEGIN
		IF i = MinLInt THEN Write("("); WriteInt(i+1); WriteString("-1)")	(* requires special bootstrap for 64 bit *)
		ELSE i1 := ABS(i);
			s[0] := CHR(i1 MOD 10 + ORD("0")); i1 := i1 DIV 10; k := 1;
			WHILE i1 > 0 DO s[k] := CHR(i1 MOD 10 + ORD("0")); i1 := i1 DIV 10; INC(k) END ;
			IF i < 0 THEN s[k] := "-"; INC(k) END ;
			WHILE k > 0 DO  DEC(k); Write(s[k]) END
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
	BEGIN Files.Write(R[currFile], 0AX)
	END WriteLn;

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
		IF noerr THEN LogWStr("    "); LogWNum(Files.Pos(R[BodyFile]), 0) END ;
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

BEGIN Texts.OpenWriter(W)
END OfrontOPM.
