MODULE OfrontOPM;	(* RC 6.3.89 / 28.6.89, J.Templ 10.7.89 / 8.5.95*)
(* constants needed for C code generation *)
(*
2002-04-11  jt: Number changed for BlackBox 1.4: SHORT(-e)
2002-08-15  jt: Scan: path separator changed from blank to semicolon
2002-08-21  jt: CloseFiles: inC tested for NIL
*)

	IMPORT SYSTEM, Math, Strings, Files, Models, Out, Stores, TextControllers, TextModels,
		TextMappers, TextViews, Views, Dialog, Controllers, DevMarkers, Windows;

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
		widechar* = 18;	(* UCS-2 encoded character format enabled *)
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
		InfRealPat = 07F800000H;	(* real infinity pattern *)
		maxErrors = 64;

	VAR
		LEHost: BOOLEAN;	(* little or big endian host *)
		MinReal32-, MaxReal32-, InfReal-, MinReal64-, MaxReal64-: REAL;
		SetSize-, IndexSize-, MaxSet-: SHORTINT; MaxIndex-: LONGINT;

		noerr-, warning156-: BOOLEAN;
		curpos-, errpos*: INTEGER;	(* character and error position in source file *)
		errors: INTEGER;
		breakpc-: INTEGER;	(* set by OPV.Init *)
		currFile*: INTEGER;	(* current output file *)
		level*: INTEGER;	(* procedure nesting level *)
		pc-, entno-: INTEGER;  (* entry number *)
		modName-: ARRAY 32 OF SHORTCHAR;
		objname*: ARRAY 64 OF SHORTCHAR;

		opt*, glbopt: SET;
		GlobalLang, Lang-: SHORTCHAR; (* "1", "2": S8/I16/L32 | "C", "3": S16/I32/L64 *)
		GlobalAdrSize, AdrSize-, GlobalAlignment, Alignment-: SHORTINT;

		crc32tab: ARRAY 256 OF INTEGER;
		lasterrpos: INTEGER;
		inR: TextModels.Reader;
		CP: ARRAY 128 + 1 OF CHAR;
		inC: TextControllers.Controller;
		script: Stores.Operation; (* for undoing insert error marks *)
		inS: TextMappers.Scanner;
		inV: Views.View;
		inLoc: Files.Locator;
		inName: Files.Name;
		end: INTEGER;
		translateList: BOOLEAN;
		oldSF: Stores.Reader; newSF: Stores.Writer;
		R: ARRAY 3 OF Files.Writer;
		errNo: ARRAY maxErrors OF SHORTINT;
		errPos: ARRAY maxErrors OF INTEGER;
		workDir, subsys: Files.Name;
		SFsubdir, CFsubdir: ARRAY 5 + 2 OF CHAR;
		oldSFile, newSFile, HFile, BFile, HIFile: Files.File;
		stop: BOOLEAN;

		options*: RECORD
			dirs*: ARRAY 256 OF CHAR;
			mainprog*: BOOLEAN;
			cp866*: BOOLEAN;
			par: POINTER TO ARRAY OF CHAR
		END;

		locate*: RECORD
			num*: INTEGER
		END;

	(* ------------------------- System runtime ------------------------- *)

	TYPE
		LongCard = LONGINT;

PROCEDURE [code] udiv (x, y: LongCard): LongCard
	068H, 3EH,1FH,0,0, (*     PUSH   1F3EH ; Set FPU mode *)
	0D9H, 02CH, 024H,  (*     FLDCW  [ESP]                *)
	059H,              (*     POP    ECX                  *)
	068H, 0,0,80H,5FH, (*     PUSH   $5F800000            *)
	0DFH, 06DH, 008H,  (*     FILD   QWORD PTR [EBP+8]    *)
	085H, 0D2H,        (*     TEST   EDX, EDX             *)
	079H, 003H,        (*     JNS    SHORT L0             *)
	0D8H, 004H, 024H,  (*     FADD   DWORD PTR [ESP]      *)
	0DFH, 06DH, 010H,  (* L0: FILD   QWORD PTR [EBP+16]   *)
	083H, 7DH, 14H, 0, (*     CMP    DWORD PTR [EBP+20],0 *)
	079H, 003H,        (*     JNS    SHORT L1             *)
	0D8H, 004H, 024H,  (*     FADD   DWORD PTR [ESP]      *)
	0DEH, 0F9H,        (* L1: FDIVP                       *)
	068H, 0,0,0,05FH,  (*     PUSH   $5F000000            *)
	0D8H, 014H, 024H,  (*     FCOM   DWORD PTR [ESP]      *)
	059H,              (*     POP    ECX                  *)
	0DFH, 0E0H,        (*     FNSTSW AX                   *)
	09EH,              (*     SAHF                        *)
	072H, 003H,        (*     JB     SHORT L2             *)
	0D8H, 024H, 024H,  (*     FSUB   DWORD PTR [ESP]      *)
	059H,              (* L2: POP    ECX                  *)
	0DFH, 03CH, 024H,  (*     FISTP  QWORD PTR [ESP]      *)
	09BH,              (*     FWAIT                       *)
	058H,              (*     POP    EAX                  *)
	05AH,              (*     POP    EDX                  *)
	068H, 3EH,03H,0,0, (*     PUSH   $33E ; Rest FPU mode *)
	0D9H, 02CH, 024H,  (*     FLDCW  [ESP]                *)
	059H;              (*     POP    ECX                  *)

	PROCEDURE UDiv*(x, y: LongCard): LongCard;
	BEGIN
		ASSERT(y # 0); RETURN udiv(x, y)
	END UDiv;

	PROCEDURE Div0*(x, y: LONGINT): LONGINT;
	BEGIN
		ASSERT(y # 0);
		IF (x < 0) = (y < 0) THEN RETURN x DIV y
		ELSIF x < 0 THEN RETURN -x DIV (-y)
		ELSE RETURN -(-x) DIV y
		END
	END Div0;

	PROCEDURE Rem0*(x, y: LONGINT): LONGINT;
	BEGIN
		ASSERT(y # 0);
		IF (x < 0) = (y < 0) THEN RETURN x MOD y
		ELSIF x < 0 THEN RETURN x MOD (-y)
		ELSE RETURN -(-x) MOD y
		END
	END Rem0;

	PROCEDURE [code] lsl(x: LONGINT; n: INTEGER): LONGINT
		059H,             (*     POP  ECX          *)
		0F6H, 0C1H, 020H, (*     TEST CL, 32       *)
		074H, 003H,       (*     JZ   SHORT L1     *)
		033H, 0D2H,       (*     XOR  EDX, EDX     *)
		092H,             (*     XCHG EDX, EAX     *)
		00FH, 0A5H, 0C2H, (* L1: SHLD EDX, EAX, CL *)
		0D3H, 0E0H;       (*     SHL  EAX, CL      *)

	PROCEDURE Lsl*(x: LONGINT; n: INTEGER): LONGINT;
	BEGIN
		ASSERT((0 <= n) & (n <= 63));
		RETURN lsl(x, n)
	END Lsl;

	PROCEDURE [code] lsr(x: LONGINT; n: INTEGER): LONGINT
		059H,             (*     POP  ECX          *)
		0F6H, 0C1H, 020H, (*     TEST CL, 32       *)
		074H, 003H,       (*     JZ   SHORT L1     *)
		033H, 0C0H,       (*     XOR  EAX, EAX     *)
		092H,             (*     XCHG EAX, EDX     *)
		00FH, 0ADH, 0D6H, (* L1: SHRD EAX, EDX, CL *)
		0D3H, 0EFH;       (*     SHR  EDX, CL      *)

	PROCEDURE Lsr*(x: LONGINT; n: INTEGER): LONGINT;
	BEGIN
		ASSERT((0 <= n) & (n <= 63));
		RETURN lsr(x, n)
	END Lsr;

	PROCEDURE [code] ror(x: LONGINT; n: INTEGER): LONGINT
		059H,             (*     POP  ECX          *)
		0F6H, 0C1H, 020H, (*     TEST CL, 32       *)
		074H, 001H,       (*     JZ   SHORT L1     *)
		092H,             (*     XCHG EAX, EDX     *)
		08BH, 0F0H,       (* L1: MOV  ESI, EAX     *)
		00FH, 0ADH, 0D0H, (*     SHRD EAX, EDX, CL *)
		00FH, 0ADH, 0F2H; (*     SHRD EDX, ESI, CL *)

	PROCEDURE Ror*(x: LONGINT; n: INTEGER): LONGINT;
	BEGIN
		ASSERT((0 <= n) & (n <= 63));
		RETURN ror(x, n)
	END Ror;

	PROCEDURE IntPower* (x: REAL; n: INTEGER): REAL;
	BEGIN
		RETURN Math.IntPower(x, n)
	END IntPower;

	(* -------------------------  lookup with search path ------------------------- *)

	PROCEDURE Scan(VAR dirs, dir: ARRAY OF CHAR; VAR pos: INTEGER);
		VAR i: INTEGER; ch: CHAR;
	BEGIN ch := dirs[pos]; i := 0;
		WHILE ch = ';' DO INC(pos); ch := dirs[pos] END ;
		WHILE (ch # 0X) & (ch # ';') DO dir[i] := ch; INC(i); INC(pos); ch := dirs[pos] END ;
		dir[i] := 0X;
		Strings.Find(dir, "%WorkDir%", 0, i); IF i # -1 THEN Strings.Replace (dir, i, 9, workDir) END;
		Strings.Find(dir, "%Subsys%", 0, i); IF i # -1 THEN Strings.Replace (dir, i, 8, subsys) END;
	END Scan;

	PROCEDURE FilesNew(IN subdir: ARRAY OF CHAR): Files.File;
		VAR dirs, dir: ARRAY 256 OF CHAR; pos: INTEGER;
	BEGIN
		Dialog.MapString(options.dirs, dirs);
		pos := 0; Scan(dirs, dir, pos);
		RETURN Files.dir.New(Files.dir.This(dir + subdir), Files.ask)
	END FilesNew;

	PROCEDURE FilesOld(IN name: Files.Name; IN subdir: ARRAY OF CHAR): Files.File;
		VAR dirs, dir: ARRAY 256 OF CHAR; pos: INTEGER; F: Files.File;
	BEGIN
		Dialog.MapString(options.dirs, dirs);
		pos := 0; Scan(dirs, dir, pos);
		REPEAT
			F := Files.dir.Old(Files.dir.This(dir + subdir), name, Files.shared); Scan(dirs, dir, pos)
		UNTIL (F # NIL) OR (dir = "");
		RETURN F
	END FilesOld;

	PROCEDURE FilesDelete(IN name: Files.Name; IN subdir: ARRAY OF CHAR);
		VAR dirs, dir: ARRAY 256 OF CHAR; pos: INTEGER;
	BEGIN
		Dialog.MapString(options.dirs, dirs);
		pos := 0; Scan(dirs, dir, pos);
		Files.dir.Delete(Files.dir.This(dir + subdir), name)
	END FilesDelete;

	PROCEDURE ViewsOld(IN name: ARRAY OF CHAR): Views.View;
		VAR dirs, dir: ARRAY 256 OF CHAR; pos: INTEGER;
	BEGIN
		Dialog.MapString(options.dirs, dirs);
		pos := 0; Scan(dirs, dir, pos);
		inName := name$;
		REPEAT
			inLoc := Files.dir.This(dir);
			inV := Views.OldView(inLoc, inName);
			Scan(dirs, dir, pos)
		UNTIL (inV # NIL) OR (dir = "");
		RETURN inV
	END ViewsOld;

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

	PROCEDURE LogW*(ch: CHAR);
	BEGIN
		Out.Char(ch)
	END LogW;

	PROCEDURE LogWStr*(IN s: ARRAY OF SHORTCHAR);
	BEGIN
		Out.String(LONG(s));
	END LogWStr;

	PROCEDURE LogWNum*(i, len: INTEGER);
	BEGIN
		Out.Int(i, len)
	END LogWNum;

	PROCEDURE LogWLn*;
	BEGIN
		Out.Ln
	END LogWLn;


	(* ------------------------- initialization ------------------------- *)

	PROCEDURE GetAlignment(VAR a: SHORTINT);
	BEGIN a := 8	(* Windows x86/Linux ARM = 48, Linux x86 = 44, x64 = 88 *)
	END GetAlignment;

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
	BEGIN i := 0;
		WHILE s[i] # 0X DO
			CASE s[i] OF
			| "e": opt := opt / {extsf}
			| "s": opt := opt / {newsf}
			| "m": opt := opt / {mainprog}
			| "x": opt := opt / {inxchk}
			| "v": opt := opt / {vcpp, ansi};
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
			| "1", "3"(*undocumented*), "C", "7": Lang := SHORT(s[i])
			| "G": opt := opt / {gpcp}
			| "O": opt := opt / {oakwood}
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
		END
	END ScanOptions;

	PROCEDURE GetOptionsAt (pos: INTEGER; VAR opt: SET);
		VAR respos: INTEGER; ch: CHAR; module, done: BOOLEAN; optstr: ARRAY 256 OF CHAR;
	BEGIN
		respos := 0; module := TRUE; done := FALSE;
		WHILE ~done & (pos < LEN(options.par^)) DO
			ch := options.par^[pos];
			IF ch = 0X THEN done := TRUE
			ELSIF ch <= " " THEN module := TRUE
			ELSIF module & (ch = OptionChar) THEN module := FALSE
			ELSIF ~module THEN optstr[respos] := ch; INC(respos)
			ELSE done := TRUE
			END;
			INC(pos)
		END;
		optstr[respos] := 0X; ScanOptions(optstr, opt)
	END GetOptionsAt;

	PROCEDURE GetModuleOptions (VAR opt: SET);
		VAR pos, endpos: INTEGER; source: TextModels.Model; w: Windows.Window;
	BEGIN
		source := TextViews.FocusText();
		IF source # NIL THEN
			w := Windows.dir.Focus(Controllers.targetPath);
			IF (w # NIL) & (w.loc # NIL) & (w.name # "") THEN
				endpos := LEN(w.name$) - 4;
				Strings.Find(w.name, ".ob1", 1, pos);
				IF pos = endpos THEN Lang := "1"
				ELSE Strings.Find(w.name, ".ob2", 1, pos);
					IF pos = endpos THEN Lang := "2"
					ELSE Strings.Find(w.name, ".ob3", 1, pos);
						IF pos = endpos THEN Lang := "3"
						ELSE Strings.Find(w.name, ".ob7", 1, pos);
							IF pos = endpos THEN Lang := "7" END
						END
					END
				END;
				IF options.par # NIL THEN
					pos := 0;
					REPEAT
						Strings.Find(options.par^, w.name, pos, pos);
						IF pos # -1 THEN
							endpos := pos + LEN(w.name$);
							IF (pos > 0) & (options.par^[pos - 1] > " ") THEN INC(pos)
							ELSIF (endpos < LEN(options.par^$)) & (options.par^[endpos] > " ") THEN INC(pos)
							ELSE (* found *)
								GetOptionsAt(endpos, opt); pos := -1
							END
						END
					UNTIL pos = -1
				END
			END
		END
	END GetModuleOptions;

	PROCEDURE OpenPar*(list: BOOLEAN);	(* prepare for a sequence of translations *)
		VAR ch: SHORTCHAR; pos, beg: INTEGER; c: TextControllers.Controller;
			F: Files.File; R: Files.Reader; optstr: ARRAY 256 OF CHAR;
	BEGIN
		(* set options to initial defaults *)
		Lang := "C";	(* default input language: Component Pascal *)
		AdrSize := -1;	(* undefined *)
		glbopt := defopt;
		translateList := list; stop := FALSE;
		c := TextControllers.Focus();  (* get the focus controller, if any *)
		IF c # NIL THEN
			IF translateList THEN
				IF c.HasSelection() THEN c.GetSelection(beg, end);
					inS.ConnectTo(c.text); inS.SetPos(beg); inS.Scan;
					IF (inS.type = TextMappers.char) & (inS.char = "/") THEN
						pos := inS.start; inS.Scan;
						IF (inS.start = pos + 1) & (inS.type = TextMappers.string) THEN
							ScanOptions(inS.string, glbopt); inS.Scan
						END
					END
				ELSE stop := TRUE
				END
			ELSE
				F := FilesOld("Ofront+.par", CFsubdir);
				IF F # NIL THEN
					R := F.NewReader(NIL); R.SetPos(0); NEW(options.par, F.Length() + 1);
					pos := 0;
					WHILE (~R.eof) & (R.Pos() < F.Length()) DO
						R.ReadByte(SYSTEM.VAL(BYTE, ch)); options.par^[pos] := ch; INC(pos)
					END;
					options.par^[pos] := 0X; GetOptionsAt(0, glbopt)
				ELSE
					options.par := NIL
				END
			END
		ELSE stop := TRUE
		END;
		(* Record global option settings for this command line *)
		GlobalAdrSize := AdrSize; GlobalAlignment := Alignment; GlobalLang := Lang
	END OpenPar;

	PROCEDURE InitOptions*;	(* get the options for one translation *)
		VAR pos: INTEGER;
	BEGIN
		Alignment := GlobalAlignment; AdrSize := GlobalAdrSize; Lang := GlobalLang;
		opt := glbopt;
		IF translateList & (inS.start < end) & (inS.type = TextMappers.char) & (inS.char = "/") THEN
			pos := inS.start; glbopt := defopt; inS.Scan;
			IF (inS.start = pos + 1) & (inS.type = TextMappers.string) THEN
				ScanOptions(inS.string, opt); inS.Scan
			END
		ELSIF ~translateList THEN GetModuleOptions(opt)
		END;
		GetProperties; IF options.mainprog THEN INCL(opt, mainprog) END
	END InitOptions;

	PROCEDURE InitCP (cp: INTEGER);
	BEGIN
		IF cp = 866 THEN
			CP := (* https://ru.wikipedia.org/wiki/CP866 *)
				(* АБВГДЕЖЗ *)
				 410X +  411X +  412X +  413X +  414X +  415X +  416X +  417X +
				(* ИЙКЛМНОП *)
				 418X +  419X +  41AX +  41BX +  41CX +  41DX +  41EX +  41FX +
				(* РСТУФХЦЧ *)
				 420X +  421X +  422X +  423X +  424X +  425X +  426X +  427X +
				(* ШЩЪЫЬЭЮЯ *)
				 428X +  429X +  42AX +  42BX +  42CX +  42DX +  42EX +  42FX +
				(* абвгдежз *)
				 430X +  431X +  432X +  433X +  434X +  435X +  436X +  437X +
				(* ийклмноп *)
				 438X +  439X +  43AX +  43BX +  43CX +  43DX +  43EX +  43FX +
				(* ░▒▓│┤╡╢╖ *)
				2591X + 2592X + 2593X + 2502X + 2524X + 2561X + 2562X + 2556X +
				(* ╕╣║╗╝╜╛┐ *)
				2555X + 2563X + 2551X + 2557X + 255DX + 255CX + 255BX + 2510X +
				(* └┴┬├─┼╞╟ *)
				2514X + 2534X + 252CX + 251CX + 2500X + 253CX + 255EX + 255FX +
				(* ╚╔╩╦╠═╬╧ *)
				255AX + 2554X + 2569X + 2566X + 2560X + 2550X + 256CX + 2567X +
				(* ╨╤╥╙╘╒╓╫ *)
				2568X + 2564X + 2565X + 2559X + 2558X + 2552X + 2553X + 256BX +
				(* ╪┘┌█▄▌▐▀ *)
				256AX + 2518X + 250CX + 2588X + 2584X + 258CX + 2590X + 2580X +
				(* рстуфхцч *)
				 440X +  441X +  442X +  443X +  444X +  445X +  446X +  447X +
				(* шщъыьэюя *)
				 448X +  449X +  44AX +  44BX +  44CX +  44DX +  44EX +  44FX +
				(* ЁёЄєЇїЎў *)
				 401X + 451X + 404X + 454X + 407X + 457X + 40EX + 45EX +
				(* °∙·√№¤■  *)
				 0B0X + 2219X + 0B7X + 221AX + 2116X + 0A4X + 25A0X + 0A0X
		ELSE (* 1251 *)
			CP := (* http://ru.wikipedia.org/wiki/Windows-1251 *)
				(* ЂЃ‚ѓ„…†‡ *)
				 402X +  403X + 201AX +  453X + 201EX + 2026X + 2020X + 2021X +
				(* €‰Љ‹ЊЌЋЏ *)
				20ACX + 2030X +  409X + 2039X +  40AX +  40CX +  40BX +  40FX +
				(* ђ‘’“”•–— *)
				 452X + 2018X + 2019X + 201CX + 201DX + 2022X + 2013X + 2014X +
				(* ?™љ›њќћџ *)
				  "?" + 2122X +  459X + 203AX +  45AX +  45CX +  45BX +  45FX +
				(*  ЎўЈ¤Ґ¦§ *)
				 0A0X +  40EX +  45EX +  408X +  0A4X +  490X +  0A6X +  0A7X +
				(* Ё©Є«¬-­®Ї *)
				 401X +  0A9X +  404X +  0ABX +  0ACX +  0ADX +  0AEX +  407X +
				(* °±Ііґµ¶· *)
				 0B0X +  0B1X +  406X +  456X +  491X +  0B5X +  0B6X +  0B7X +
				(* ё№є»јЅѕї *)
				 451X + 2116X +  454X +  0BBX +  458X +  405X +  455X +  457X +
				(* АБВГДЕЖЗ *)
				 410X +  411X +  412X +  413X +  414X +  415X +  416X +  417X +
				(* ИЙКЛМНОП *)
				 418X +  419X +  41AX +  41BX +  41CX +  41DX +  41EX +  41FX +
				(* РСТУФХЦЧ *)
				 420X +  421X +  422X +  423X +  424X +  425X +  426X +  427X +
				(* ШЩЪЫЬЭЮЯ *)
				 428X +  429X +  42AX +  42BX +  42CX +  42DX +  42EX +  42FX +
				(* абвгдежз *)
				 430X +  431X +  432X +  433X +  434X +  435X +  436X +  437X +
				(* ийклмноп *)
				 438X +  439X +  43AX +  43BX +  43CX +  43DX +  43EX +  43FX +
				(* рстуфхцч *)
				 440X +  441X +  442X +  443X +  444X +  445X +  446X +  447X +
				(* шщъыьэюя *)
				 448X +  449X +  44AX +  44BX +  44CX +  44DX +  44EX +  44FX
		END
	END InitCP;

	PROCEDURE Init*(VAR done: BOOLEAN);	(* get the source for one translation *)
		VAR v: Views.View; m: Models.Model; source: TextModels.Model;
	BEGIN
		done := FALSE; curpos := 0;
		IF stop THEN RETURN END;
		IF translateList THEN
			IF (inS.start < end) & (inS.type = TextMappers.string) THEN
				LogWStr(SHORT(inS.string));
				v := ViewsOld(inS.string);
				IF v # NIL THEN
					m := v.ThisModel();
					IF (m # NIL) & (m IS TextModels.Model) THEN
						LogWStr("  translating");
						source := m(TextModels.Model);
						inR := source.NewReader(inR); inR.SetPos(0);
						InitCP(1251); done := TRUE; inS.Scan
					ELSE LogWStr(" not a text document"); LogWLn
					END
				ELSE LogWStr(" not found"); LogWLn
				END
			END
		ELSE
			inC := TextControllers.Focus();  (* get the focus controller, if any *)
			IF inC # NIL THEN  (* a text view must be focus *)
				LogWStr("translating");
				inR := inC.text.NewReader(inR); inR.SetPos(0); DevMarkers.Unmark(inR.Base());
				IF options.cp866 THEN InitCP(866) ELSE InitCP(1251) END;
				Models.BeginScript(inC.text, "#ofront:insertMarkers", script);
				done := TRUE; stop := TRUE
			END
		END;
		level := 0; noerr := TRUE; errpos := curpos; lasterrpos := curpos - 10; errors := 0
	END Init;

	PROCEDURE AppInt(x, pos: INTEGER; VAR s: ARRAY OF CHAR);
	BEGIN
		IF x > 100 THEN s[pos] := CHR(ORD("0") + x DIV 100); INC(pos) END ;
		IF x > 10 THEN s[pos] := CHR(ORD("0") + x MOD 100 DIV 10); INC(pos) END ;
		s[pos] := CHR(ORD("0") + x MOD 10); INC(pos); s[pos] := 0X
	END AppInt;


	PROCEDURE LocatePos*;
		VAR C: TextControllers.Controller; pos: INTEGER;
	BEGIN
		C := TextControllers.Focus();
		IF C = NIL THEN RETURN END ;
		IF locate.num < 0 THEN pos := 0; Dialog.Beep
		ELSIF locate.num > C.text.Length() THEN pos := C.text.Length(); Dialog.Beep
		ELSE pos := locate.num
		END ;
		C.view.ShowRange(pos, pos, FALSE);
		C.SetCaret(pos)
	END LocatePos;

	PROCEDURE LocateLine*;
		VAR C: TextControllers.Controller; R: TextModels.Reader; ch: CHAR; line, pos: INTEGER;
	BEGIN
		C := TextControllers.Focus();
		IF C = NIL THEN RETURN END ;
		R := C.text.NewReader(NIL);
		R.SetPos(0); line := 0;
		REPEAT
			R.ReadChar(ch);
			IF ch = TextModels.line THEN INC(line) END
		UNTIL R.eot OR (line = locate.num);
		IF R.eot THEN pos := C.text.Length(); Dialog.Beep
		ELSIF locate.num < 0 THEN pos := 0; Dialog.Beep
		ELSE pos := R.Pos() - 1
		END ;
		C.view.ShowRange(pos, pos, FALSE);
		C.SetCaret(pos)
	END LocateLine;


	(* ------------------------- read source text -------------------------*)



	PROCEDURE MakeFileName(IN name: ARRAY OF SHORTCHAR; VAR FName: Files.Name; IN ext: ARRAY OF CHAR);
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

	PROCEDURE InsertMarks*;
		CONST OpenFirstMark = TRUE;
		VAR i, j, x, n: INTEGER; y: SHORTINT; E: DevMarkers.View; s: ARRAY 256 OF CHAR;
	BEGIN
		n := errors;
		IF n > maxErrors THEN n := maxErrors END;
		(* sort *)
		i := 1;
		WHILE i < n DO
			x := errPos[i]; y := errNo[i]; j := i-1;
			WHILE (j >= 0) & (errPos[j] > x) DO errPos[j+1] := errPos[j]; errNo[j+1] := errNo[j]; DEC(j) END;
			errPos[j+1] := x; errNo[j+1] := y; INC(i)
		END;
		(* insert *)
		WHILE n > 0 DO DEC(n);
			E := DevMarkers.dir.New(DevMarkers.message);
			Strings.IntToString(errNo[n], s); Dialog.MapString("#ofront:"+s, s);
			E.InitErr(errNo[n]); E.InitMsg(s);
			IF OpenFirstMark & (n = 0) THEN E.SetMode(2) END;
			DevMarkers.Insert(inR.Base(), errPos[n], E)
		END
	END InsertMarks;

	PROCEDURE Mark*(n: SHORTINT; pos: INTEGER);
		VAR s: ARRAY 256 OF CHAR;
	BEGIN
		IF n >= 0 THEN
			IF pos < 0 THEN pos := 0 END;
			IF (pos < lasterrpos) OR (lasterrpos + 9 < pos) THEN lasterrpos := pos;
				IF n < 249 THEN
					IF inC = NIL THEN RETURN END;
					IF errors < maxErrors THEN
						errNo[errors] := n; errPos[errors] := pos;
						INC(errors)
					END;
					IF noerr THEN
						IF translateList THEN Views.Open(inV, inLoc, inName, NIL);
							inV(TextViews.View).ShowRange(pos, pos, FALSE)
						ELSE inC.view.ShowRange(pos, pos, FALSE)
						END
					END
				ELSE
					s := "#ofront:"; AppInt(n, 8, s);
					Dialog.MapParamString(s, LONG(objname), "", "", s);
					LogWLn; LogWStr("  "); LogWStr(SHORT(s))
				END
			END ;
			noerr := FALSE
		ELSE
			IF pos >= 0 THEN LogWLn; LogWStr("  pos "); LogWNum(pos, 1) END;
			LogWStr("  warning  "); s := "#ofront:"; AppInt(-n, 8, s);
			Dialog.MapString(s, s); LogWStr('"'); LogWStr(SHORT(s)); LogWStr('"');
			IF pos < 0 THEN LogWLn END
		END
	END Mark;

	PROCEDURE WideCharToCP (ch: CHAR): SHORTCHAR;
		VAR n: INTEGER;
	BEGIN
		IF ch <= 7FX THEN RETURN SHORT(ch) END;
		FOR n := 7FH TO 0 BY -1 DO
			IF ch = CP[n] THEN RETURN SHORT(CHR(n + 80H)) END
		END;
		Mark(3, curpos + 1);
		RETURN " " (* Replace an unknown character to " " *)
	END WideCharToCP;

	PROCEDURE Get* (OUT ch: CHAR);	(* read next character from source text, 0X if eof *)
	BEGIN
		inR.Read();
		IF inR.eot THEN ch := 0X
		ELSE
			IF widechar IN opt THEN ch := inR.char ELSE ch := WideCharToCP(inR.char) END;
			INC(curpos)
		END
	END Get;

	PROCEDURE err*(n: SHORTINT);
	BEGIN Mark(n, errpos)
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

	PROCEDURE SymRCh*(VAR ch: SHORTCHAR);
	BEGIN
		oldSF.ReadSChar(ch)
	END SymRCh;

	PROCEDURE SymRInt* (): LONGINT;
		VAR s, byte: BYTE; ch: SHORTCHAR; n: LONGINT;
	BEGIN s := 0; n := 0; oldSF.ReadByte(byte); ch := SHORT(CHR(byte));
		WHILE ORD(ch) >= 128 DO INC(n, ASH(LONG(ORD(ch) - 128), s) ); INC(s, 7);
			oldSF.ReadByte(byte); ch := SHORT(CHR(byte))
		END;
		INC(n, ASH(LONG(ORD(ch) MOD 64 - ORD(ch) DIV 64 * 64), s) );
		RETURN n
	END SymRInt;

	PROCEDURE SymRSet*(VAR s: SET);
	BEGIN
		s := SYSTEM.VAL(SET, SHORT(SymRInt()))
	END SymRSet;

	PROCEDURE SymRReal*(VAR r: SHORTREAL);
	BEGIN
		oldSF.ReadSReal(r)
	END SymRReal;

	PROCEDURE SymRLReal*(VAR lr: REAL);
	BEGIN
		oldSF.ReadReal(lr)
	END SymRLReal;

	PROCEDURE CloseOldSym*;
	BEGIN oldSFile.Close; oldSFile := NIL; oldSF.ConnectTo(NIL)
	END CloseOldSym;

	PROCEDURE OldSym*(IN modName: ARRAY OF SHORTCHAR; VAR done: BOOLEAN);
		VAR ch: SHORTCHAR; fileName: Files.Name;
	BEGIN MakeFileName(modName, fileName, SFext);
		oldSFile := FilesOld(fileName, SFsubdir); done := oldSFile # NIL;
		IF done THEN
			oldSF.ConnectTo(oldSFile); oldSF.ReadSChar(ch);
			IF ch # SFtag THEN err(-306);  (*possibly a symbol file from another Oberon implementation, e.g. HP-Oberon*)
				CloseOldSym; done := FALSE
			END
		END
	END OldSym;

	PROCEDURE eofSF*(): BOOLEAN;
	BEGIN
		RETURN oldSF.rider.eof
	END eofSF;

	(* ------------------------- Write Symbol File ------------------------- *)

	PROCEDURE SymWCh*(ch: SHORTCHAR);
	BEGIN
		newSF.WriteSChar(ch)
	END SymWCh;

	PROCEDURE SymWInt* (x: LONGINT);
	BEGIN
		WHILE (x < - 64) OR (x > 63) DO newSF.WriteByte(SHORT(SHORT(SHORT(x) MOD 128 + 128))); x := x DIV 128 END;
		newSF.WriteByte(SHORT(SHORT(SHORT(x) MOD 128)))
	END SymWInt;

	PROCEDURE SymWSet*(s: SET);
	BEGIN
		SymWInt(SYSTEM.VAL(INTEGER, s))
	END SymWSet;

	PROCEDURE SymWReal*(r: SHORTREAL);
	BEGIN
		newSF.WriteSReal(r)
	END SymWReal;

	PROCEDURE SymWLReal*(lr: REAL);
	BEGIN
		newSF.WriteReal(lr)
	END SymWLReal;

	PROCEDURE RegisterNewSym*;
		VAR fileName: Files.Name; res: INTEGER;
	BEGIN
		IF (modName # "SYSTEM") OR (mainprog IN opt) THEN
			MakeFileName(modName, fileName, SFext);
			newSFile.Register(fileName, "*", Files.dontAsk, res)
		END
	END RegisterNewSym;

	PROCEDURE DeleteNewSym*;
	END DeleteNewSym;

	PROCEDURE NewSym*(VAR modName: ARRAY OF SHORTCHAR);
		VAR fileName: Files.Name;
	BEGIN MakeFileName(modName, fileName, SFext);
		newSFile := FilesNew(SFsubdir);
		IF newSFile # NIL THEN
			newSF.ConnectTo(newSFile); newSF.WriteSChar(SFtag)
		ELSE err(153)
		END
	END NewSym;

	(* ------------------------- Write Header & Body Files ------------------------- *)

	PROCEDURE Write* (ch: SHORTCHAR);
	BEGIN
		R[currFile].WriteByte(SYSTEM.VAL(BYTE, ch))
	END Write;

	PROCEDURE WriteString* (IN s: ARRAY OF SHORTCHAR);
		VAR i: INTEGER; ch: SHORTCHAR;
	BEGIN i := 0; ch := s[0];
	WHILE ch # 0X DO Write(ch); INC(i); ch := s[i] END
	END WriteString;

	PROCEDURE WriteHex* (i: INTEGER);
		VAR s: ARRAY 3 OF SHORTCHAR;
			digit : INTEGER;
	BEGIN
		digit := SHORT(i) DIV 16;
		IF digit < 10 THEN s[0] := SHORT(CHR (ORD ("0") + digit)); ELSE s[0] := SHORT(CHR (ORD ("a") - 10 + digit)); END;
		digit := SHORT(i) MOD 16;
		IF digit < 10 THEN s[1] := SHORT(CHR (ORD ("0") + digit)); ELSE s[1] := SHORT(CHR (ORD ("a") - 10 + digit)); END;
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
			Write("("); WriteInt(i+1); WriteString("-1)")
		ELSE i1 := ABS(i);
			s[0] := CHR(i1 MOD 10 + ORD("0")); i1 := i1 DIV 10; k := 1;
			WHILE i1 > 0 DO s[k] := CHR(i1 MOD 10 + ORD("0")); i1 := i1 DIV 10; INC(k) END;
			IF i < 0 THEN s[k] := "-"; INC(k) END;
			WHILE k > 0 DO DEC(k); Write(SHORT(s[k])) END;
			IF (ansi IN opt) & ((i < MIN(INTEGER)) OR (i > MAX(INTEGER))) THEN WriteString("LL") END
		END
	END WriteInt;

	PROCEDURE WriteReal* (r: REAL; suffx: CHAR);
		VAR s: ARRAY 32 OF CHAR; ch: CHAR; i: INTEGER;
	BEGIN
		IF ABS(r) > 1.0E+308 THEN Write("("); WriteReal(r / 2, suffx); WriteString(" * 2)"); RETURN END;
		IF (r < MAX(LONGINT)) & (r > MIN(LONGINT)) & (r = ENTIER(r)) THEN
			IF suffx = "f" THEN WriteString("(SHORTREAL)") ELSE WriteString("(REAL)") END;
			WriteInt(ENTIER(r))
		ELSE
			IF suffx = "f" THEN Strings.RealToString(SHORT(r), s) ELSE Strings.RealToString(r, s) END;
(* s[i] := suffx; s[i+1] := 0X;
suffix does not work in K&R *)
			i := 0; ch := s[0];
			WHILE (ch # "E") & (ch # 0X) DO INC(i); ch := s[i] END;
			IF ch = "E" THEN s[i] := "e" END;
			WriteString(SHORT(s))
		END
	END WriteReal;

	PROCEDURE WriteLn*;
	BEGIN
		R[currFile].WriteByte(0DH); R[currFile].WriteByte(0AH)
	END WriteLn;

	PROCEDURE WriteTab*;
	BEGIN
		WriteString("  ")
	END WriteTab;

	PROCEDURE WriteModPos*;
	BEGIN
		IF ansi IN opt THEN WriteString("(CHAR*)") END;
		Write('"'); WriteString(modName); WriteString('", '); WriteInt(-errpos)
	END WriteModPos;

	PROCEDURE Append(VAR R: Files.Writer; F: Files.File);
		VAR R1: Files.Reader; buffer: ARRAY 4096 OF BYTE; oldPos: INTEGER;
	BEGIN
		IF F # NIL THEN
			R1 := F.NewReader(NIL); oldPos := R1.Pos(); R1.ReadBytes(buffer, 0, LEN(buffer));
			WHILE R1.Pos() # oldPos DO
				R.WriteBytes(buffer, 0, R1.Pos() - oldPos); oldPos := R1.Pos();
				R1.ReadBytes(buffer, 0, LEN(buffer))
			END
		END
	END Append;

	PROCEDURE OpenFiles*(VAR moduleName: ARRAY OF SHORTCHAR);
		VAR FName: Files.Name;
	BEGIN
		modName := moduleName$;
		HFile := FilesNew(CFsubdir);
		IF HFile # NIL THEN R[HeaderFile] := HFile.NewWriter(NIL) ELSE err(153) END;
		IF ~(foreign IN opt) THEN
			MakeFileName(moduleName, FName, BFext);
			BFile := FilesNew(CFsubdir);
			IF BFile # NIL THEN R[BodyFile] := BFile.NewWriter(NIL) ELSE err(153) END
		END;
		MakeFileName(moduleName, FName, HFext);
		HIFile := FilesNew(CFsubdir);
		IF HIFile # NIL THEN R[HeaderInclude] := HIFile.NewWriter(NIL) ELSE err(153) END;
		IF include0 IN opt THEN
			MakeFileName(moduleName, FName, ".h0"); Append(R[HeaderInclude], FilesOld(FName, "/Mod"));
			MakeFileName(moduleName, FName, ".c0"); Append(R[BodyFile], FilesOld(FName, "/Mod"))
		END
	END OpenFiles;

	PROCEDURE FileDiff(name: Files.Name; new: Files.File): BOOLEAN;
		VAR old: Files.File; RO, RN: Files.Reader;
			bo, bn: BYTE; i, pos: INTEGER;
			dirs, dir: ARRAY 256 OF CHAR;
	BEGIN
		Dialog.MapString(options.dirs, dirs); pos := 0; Scan(dirs, dir, pos);
		old := Files.dir.Old(Files.dir.This(dir), name, Files.shared);
		IF (old = NIL) OR (old.Length() # new.Length()) THEN RETURN TRUE END ;
		RO := old.NewReader(NIL); RN := new.NewReader(NIL);
		FOR i := 0 TO old.Length() - 1 DO
			RO.ReadByte(bo); RN.ReadByte(bn);
			IF bo # bn THEN RETURN TRUE END
		END ;
		RETURN FALSE
	END FileDiff;

	PROCEDURE CloseFiles*;
		VAR FName: Files.Name; res: INTEGER; body: BOOLEAN;
	BEGIN
		body := ~(foreign IN opt);
		IF noerr THEN LogWStr("    ");
			IF body THEN LogWNum(R[BodyFile].Pos(), 0) ELSE LogWNum(R[HeaderFile].Pos(), 0) END;
			Dialog.ShowStatus("#ofront:ok")
		ELSE Dialog.ShowStatus("#ofront:error")
		END;
		IF noerr THEN
			IF modName = "SYSTEM" THEN
				IF ~(mainprog IN opt) THEN
					MakeFileName(modName, FName, BFext);
					IF body THEN BFile.Register(FName, "*", Files.dontAsk, res)
					ELSE FilesDelete(FName, CFsubdir)
					END
				END
			ELSIF ~(mainprog IN opt) THEN
				Append(R[HeaderInclude], HFile);
				MakeFileName(modName, FName, HFext);
				IF FileDiff(FName, HIFile) THEN HIFile.Register(FName, "*", Files.dontAsk, res) END;
				MakeFileName(modName, FName, BFext);
				IF body THEN BFile.Register(FName, "*", Files.dontAsk, res)
				ELSE FilesDelete(FName, CFsubdir)
				END
			ELSE
				MakeFileName(modName, FName, HFext); FilesDelete(FName, CFsubdir);
				MakeFileName(modName, FName, SFext); FilesDelete(FName, SFsubdir);
				MakeFileName(modName, FName, BFext);
				IF body THEN BFile.Register(FName, "*", Files.dontAsk, res)
				ELSE FilesDelete(FName, CFsubdir)
				END
			END
		END;
		HFile := NIL; BFile := NIL; HIFile := NIL; newSFile := NIL; oldSFile := NIL;
		R[0] := NIL; R[1] := NIL; R[2] := NIL; newSF.ConnectTo(NIL); oldSF.ConnectTo(NIL);
		IF inC # NIL THEN Models.EndScript(inC.text, script); inC := NIL END
	END CloseFiles;

	PROCEDURE GetDefaultDirectories;
	BEGIN
		Dialog.MapString( "#ofront:directories", options.dirs)
	END GetDefaultDirectories;

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

	PROCEDURE SetWorkDir* (
		IN workdir, subsysdir: ARRAY OF CHAR; useObj, useSym: BOOLEAN; IN addTo: ARRAY 3 OF CHAR);
	VAR
		i: INTEGER;
	BEGIN
		workDir := workdir$;
		i := LEN(workDir$) - 1; IF (i >= 0) & ((workDir[i] = "\") OR (workDir[i] = "/")) THEN workDir[i] := 0X END;
		subsys := subsysdir$;
		i := LEN(subsys$) - 1; IF (i >= 0) & ((subsys[i] = "\") OR (subsys[i] = "/")) THEN subsys[i] := 0X END;
		SFsubdir := ""; CFsubdir := "";
		IF useObj THEN CFsubdir := "/Obj" + addTo END;
		IF useSym THEN SFsubdir := "/Sym" + addTo ELSIF useObj THEN SFsubdir := "/Obj" + addTo END;
	END SetWorkDir;

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
		InfReal := SYSTEM.VAL(SHORTREAL, InfRealPat);
		MinReal32 := SYSTEM.VAL(SHORTREAL, MinReal32Pat);
		MaxReal32 := SYSTEM.VAL(SHORTREAL, MaxReal32Pat);
		MinReal64 := Compound(MinReal64PatL, MinReal64PatH);
		MaxReal64 := Compound(MaxReal64PatL, MaxReal64PatH)
	END InitHost;

BEGIN
	InitCrcTab; InitHost; options.cp866 := FALSE;
	subsys := ""; workDir := ""; SFsubdir := ""; CFsubdir := "";
	locate.num := 0; GetDefaultDirectories
END OfrontOPM.


OfrontCmd OfrontOPP OfrontOPB OfrontOPC OfrontOPV OfrontOPT OfrontOPS OfrontOPM
OfrontBrowser
