MODULE OMake;

IMPORT 
	Kernel,
	Console,
	Files,
	Args, 
	Platform,
	Out := arOut,

	Path := arPath,
	Char := arChar,
	Text := arText,
	Pattern := arPattern,
	C := arConfiguration,
	SL := arStringList,
	SA := arStringAssoc,
	Strings := arStrings;

CONST
	eof = 0;
	identifier = 1;
	string = 2;

	comma = 100;	
	semicolon = 101;
	colon = 102;
	becomes = 103;
	leftParenthesis = 104;
	rightParenthesis = 105;
	openComment = 106;
	closeComment = 107;
	times = 108;

	module = 200;
	import = 201;

	link = 300;

TYPE
	StringList = SL.List;
	StringArray = SL.Array;
	String = SL.String;

	Keyword = POINTER TO RECORD
		name : String;
		symbol : INTEGER;
		next : Keyword;
	END;

	Import = POINTER TO RECORD
		name : String;
		module : Module;
		next : Import;
	END;

	Module = POINTER TO RECORD
		name : String;
		file : String;
		inLibrary : BOOLEAN;
		built : BOOLEAN;
		flags : String;			(* flags specified in module *)
		extFlags : String;		(* flags infered from extension *)
		links : StringList;
		imports : Import;
		next : Module;
	END;

VAR
	path : C.String;
	exclude : C.String;
	require : C.String;
	debug : C.Boolean;
	script : C.Boolean;
	compiler : C.String;
	linker : C.String;
	libraries : C.Assoc;
	linkFlags : C.Assoc;
	extensions : C.Assoc;
	builtin : C.Assoc;

	sPath : StringArray;
	sExclude : StringArray;
	sRequire : StringArray;
	modules : Module;
	keywords, makeKeywords : Keyword;

PROCEDURE AddSymbol(name : ARRAY OF CHAR; symbol : INTEGER);
VAR keyword : Keyword;
BEGIN
	NEW(keyword);
	keyword.name := SL.Copy(name);
	keyword.symbol := symbol;
	keyword.next := keywords;
	keywords := keyword;
END AddSymbol;

PROCEDURE LocateModule(name : ARRAY OF CHAR; VAR result : String; VAR library : String);
VAR
	i, j : LONGINT;
	fileName, path, ext : String;
	buffer : Text.Buffer;
	item : SA.Assoc;
BEGIN
	buffer.Init(64);

	FOR i := 0 TO LEN(sPath^)-1 DO
		path := sPath[i];
		library := SA.GetKey(libraries.value, path^);
		item := extensions.value;
		WHILE item # NIL DO
			ext := item.key;

			buffer.Clear;
			buffer.String(path^);
			buffer.String(Platform.PathDelimiter);
			buffer.String(name);
			buffer.String(".");
			buffer.String(ext^);
			fileName := buffer.GetString();

			IF Platform.FileExists(fileName^) THEN
				result := fileName;
				RETURN;
			END;
			item := item.next
		END
	END;
	result := NIL;
END LocateModule;

PROCEDURE FindModule(name : ARRAY OF CHAR) : Module;
VAR m : Module;
BEGIN
	m := modules;
	WHILE m # NIL DO
		IF Strings.Equal(m.name^, name) THEN
			RETURN m;
		END;
		m := m.next;
	END;
	RETURN NIL;
END FindModule;

PROCEDURE ShowModule(m : Module );
VAR 
	i : Import;
	l : StringList;
BEGIN
	Console.String("MODULE "); Console.String(m.name^);
	IF m.flags # NIL THEN
		Console.String(" FLAGS '"); Console.String(m.flags^); Console.String("'");
	END;
	IF m.imports # NIL THEN
		Console.String(" IMPORTS ");
		i := m.imports;
		WHILE i # NIL DO
			Console.String(i.name^);
			i := i.next;
			IF i # NIL THEN Console.Char(",") END;
		END;
	END;
	IF m.links # NIL THEN
		l := m.links;
		Console.String(" LINKS ");
		WHILE l # NIL DO
			Console.String(l.name^);
			l := l.next;
			IF l # NIL THEN Console.Char(","); END;
		END;
	END;
	Console.String(" IN FILE "); Console.String(m.file^);
	Console.Ln;
END ShowModule;

PROCEDURE ShowAllModules;
VAR m : Module;
BEGIN
	m := modules;
	WHILE m # NIL DO
		ShowModule(m);
		m := m.next;
	END;
END ShowAllModules;

PROCEDURE CheckFileType(path : String; VAR flags : String);
VAR extension : ARRAY 32 OF CHAR;
BEGIN
	Path.ExtName(path^, extension);
	Strings.Extract(extension, 1, LEN(extension), extension);
	flags := SA.Get(extensions.value, extension);
END CheckFileType;

PROCEDURE ParseFile(fileName : String) : Module;
VAR
	line, column : INTEGER;
	rd : Files.Rider;
	file : Files.File;

	sym : INTEGER; ch : CHAR;

	ident : Text.Buffer;

	importName : String;
	library : String;
	m : Module;
	imp : Import;

	PROCEDURE ErrorString(reason, str : ARRAY OF CHAR);
	BEGIN
		IF line > 0 THEN
			Console.String("At line="); Console.Int(line, 0);
			Console.String(", column="); Console.Int(column, 0);
			Console.String(" in file "); Console.String(fileName^);
			Console.Ln;
		END;
	
		Console.String("ERROR: ");
		Console.String(reason);
		IF str[0] # 0X THEN
			Console.String(" : ");
			Console.String(str);
		END;
		Console.Ln;
		Kernel.Exit(1);
	END ErrorString;

	PROCEDURE Error(reason : ARRAY OF CHAR);
	BEGIN
		ErrorString(reason, "");
	END Error;

	PROCEDURE Next;
	BEGIN
		IF ch = 0AX THEN
			column := 0;
			INC(line);
		END;
		Files.ReadChar(rd, ch);
		INC(column);
	END Next;

	PROCEDURE Ident;
	BEGIN
		ident.Clear;
		REPEAT
			ident.Char(ch);
			Next;
		UNTIL ~Char.IsLetter(ch) & ~Char.IsDigit(ch);
		ident.Terminate;
		sym := identifier;
	END Ident;

	PROCEDURE CopyIdent() : String;
	BEGIN
		RETURN SL.Copy(ident.value^);
	END CopyIdent;

	PROCEDURE CheckKeyword;
	VAR keyword : Keyword;
	BEGIN
		keyword := keywords;
		WHILE keyword # NIL DO
			IF Strings.Equal(keyword.name^, ident.value^) THEN
				sym := keyword.symbol; RETURN
			END;
			keyword := keyword.next;
		END;
	END CheckKeyword;

	PROCEDURE ParseString(term : CHAR);
	BEGIN
		Next;
		ident.Clear;
		WHILE ch # term DO
			IF (ch = 0X) OR (ch = 0AX) THEN
				Error("Unterminated String");
			END;
			ident.Char(ch);
			Next;
		END;
		ident.Terminate;
		Next;
		sym := string;
	END ParseString;

	PROCEDURE SkipSpace;
	BEGIN
		WHILE Char.IsSpace(ch) DO Next; END;
	END SkipSpace;

	PROCEDURE Symbol0;
	BEGIN
		SkipSpace;
		CASE ch OF
		| 0X: sym := eof;
		| "a".."z", "A".."Z", "_":
			Ident;
			CheckKeyword;
		| '"': ParseString('"')
		| "'": ParseString("'")
		| ';': sym := semicolon; Next;
		| ',': sym := comma; Next;
		| ':': 
			sym := colon; Next;
			IF ch = '=' THEN
				Next;
				sym := becomes;
			END;
		| '(': sym := leftParenthesis; Next;
			IF ch = '*' THEN
				sym := openComment; Next;
			END;
		| ')': sym := rightParenthesis; Next;
		| '*': sym := times; Next;
			IF ch = ')' THEN
				sym := closeComment; Next;
			END;
		ELSE
			Error("Unexpected symbol");
		END;
		IF debug.value THEN
			Console.Int(sym, 0); Console.Ln;
		END;
	END Symbol0;

	PROCEDURE SkipComment;
	VAR 
		lastCh : CHAR;
		level : INTEGER;
	BEGIN
		level := 1;
		lastCh := 0X;
		LOOP
			IF (ch = ')') & (lastCh = '*') THEN
				DEC(level);
				IF level = 0 THEN
					Next; RETURN;
				END;
			ELSIF (ch = '*') & (lastCh = '(') THEN
				INC(level);
				Next;
			END;
			lastCh := ch;
			Next;
		END;
	END SkipComment;

	PROCEDURE Symbol;
	BEGIN
		Symbol0;
		WHILE sym = openComment DO
			SkipComment;
			Symbol0;
		END;
	END Symbol;

	PROCEDURE Expect(symbol : INTEGER);
	BEGIN
		IF sym # symbol THEN
			Console.String("Expected="); Console.Int(symbol, 0); Console.String(" Found="); Console.Int(sym, 0); Console.Ln;
			Error("Unexpected symbol");
		END;
	END Expect;

	PROCEDURE ParseImport(m : Module);
	VAR
		i : Import;
		name : String;
	BEGIN
		Expect(identifier);
		name := CopyIdent();
		Symbol;
		IF sym = becomes THEN
			Symbol;
			Expect(identifier);
			name := CopyIdent();
			Symbol;
		END;
		NEW(i);
		i.name := name;
		i.next := m.imports;
		m.imports := i;
	END ParseImport;

	PROCEDURE ParseLink(m : Module);
	BEGIN
		Expect(string);
		SL.Add(m.links, CopyIdent());
		Symbol0;
	END ParseLink;

	PROCEDURE ParseMake(m : Module);
	BEGIN
		IF sym = string THEN
			m.flags := CopyIdent();
			Symbol0;
		END;

		IF sym = link THEN
			Symbol0;
			ParseLink(m);
			WHILE sym = comma DO
				Symbol0;
				ParseLink(m);
			END;
		END;
	END ParseMake;

	PROCEDURE ParseModule(m : Module);
	VAR original : Keyword;
	BEGIN
		Expect(module);
		Symbol; Expect(identifier);
		m.name := CopyIdent();
		Symbol; Expect(semicolon);

		(* comments after the module header may be OMAKE commands *)
		Symbol0;
		WHILE sym = openComment DO
			SkipSpace;
			IF ch = "O" THEN
				Ident;
				IF Strings.Equal(ident.value^, "OMAKE") THEN
					(* push new keyword set *)
					original := keywords;
					keywords := makeKeywords;

					Symbol0;
					ParseMake(m);
					Expect(closeComment);

					(* restore original keywords set *)
					keywords := original;
				ELSE
					SkipComment;
				END;
				Symbol0;
			ELSE
				SkipComment;
				Symbol0;
			END;
		END;

		(* parse imports *)
		IF sym = import THEN
			Symbol;
			ParseImport(m);
			WHILE sym = comma DO
				Symbol;
				ParseImport(m);
			END;
			Expect(semicolon);
		END;
		(* ignore the rest of the module *)
	END ParseModule;	

BEGIN
	ident.Init(64);

	IF debug.value THEN
		Console.String("Parse file "); Console.String(fileName^); Console.Ln;
	END;
	m := NIL;
	line := 0; column := 0;
	file := Files.Old(fileName^);
	IF file = NIL THEN
		ErrorString("Cannot open file", fileName^);
	END;
	Files.Set(rd, file, 0);
	ch := 0AX;
	Symbol;

	NEW(m);
	m.name := NIL;
	m.file := fileName;
	m.flags := NIL;
	CheckFileType(m.file, m.extFlags);
	m.links := NIL;
	m.imports := NIL;
	m.inLibrary := FALSE;
	m.built := FALSE;

	ParseModule(m);
	Files.Close(file);
	imp := m.imports;
	WHILE imp # NIL DO
		IF ~SL.ArrayContains(sExclude, imp.name) THEN
			imp.module := FindModule(imp.name^);
			IF imp.module = NIL THEN
				LocateModule(imp.name^, importName, library);
				IF importName = NIL THEN
					ErrorString("Cannot locate module source", imp.name^);
				END;
				imp.module := ParseFile(importName);
				IF library # NIL THEN
					SL.Add(imp.module.links, library);
					imp.module.inLibrary := TRUE;
				END;
			END;
		END;
		imp := imp.next;
	END;
	m.next := modules;
	modules := m;
	RETURN m;
END ParseFile;

(* execute command, or if -script specified just print *)

PROCEDURE Run(command : String) : LONGINT;
VAR result : LONGINT; i : LONGINT; ch : CHAR;
BEGIN
	IF script.value THEN
		Console.String(command^); Console.Ln;
		RETURN 0;
	ELSE
		Console.String("--> "); Console.String(command^); Console.Ln;
		RETURN Platform.System(command^);
	END;
END Run;

PROCEDURE CompileModule(module : Module; VAR libs, objs : StringList; main : BOOLEAN) : BOOLEAN;
VAR
	buffer : Text.Buffer;
	result : LONGINT;
	import : Import;
BEGIN
	IF module.built THEN RETURN TRUE END;

	(* include link libraries for this module *)
	SL.Merge(module.links, libs);

	(* don't compile modules that are already in libraries *)
	IF module.inLibrary THEN
		RETURN TRUE;
	END;

	(* include this module in object files *)
	SL.Add(objs, module.name);

	(* first compile all imported modules *)
	import := module.imports;
	WHILE import # NIL DO
		IF import.module # NIL THEN
			IF ~CompileModule(import.module, libs, objs, FALSE) THEN
				RETURN FALSE;
			END;
		END;
		import := import.next;
	END;

	(* now make a compile command for this module *)
	buffer.Init(128);

	buffer.String(compiler.value^);
	buffer.Char(" ");

	IF module.flags # NIL THEN
		buffer.String(module.flags^);
		buffer.Char(" ");
	END;
	IF module.extFlags # NIL THEN
		buffer.String(module.extFlags^);
		buffer.Char(" ");
	END;
	IF main THEN
		buffer.String("-m ");
	END;
	buffer.String(module.file^);

	(* compile the module *)
	IF ~script.value THEN
		Console.String("Building Module "); Console.String(module.name^); Console.Ln;
	END;
	result := Run(buffer.GetString());

	module.built := TRUE;		(* build attempted *)
	RETURN result = 0;
END CompileModule;

PROCEDURE LinkModule(module : Module; libs, objs : StringList) : BOOLEAN;
VAR
	buffer : Text.Buffer;
	i, result : LONGINT;
	l, flag : StringList;
	name : String;
	omitLibs : StringList;
BEGIN
	omitLibs := NIL;
	buffer.Init(128);
	buffer.String(linker.value^);
	buffer.String(" ");

	(* add any libraries specified by "require" directive *)
	IF sRequire # NIL THEN
		FOR i := 0 TO LEN(sRequire^)-1 DO
			SL.Include(libs, sRequire[i]);
		END
	END;

	(* output linkflags for all required libraries *)
	l := libs;
	WHILE l # NIL DO
		flag := SA.GetAll(linkFlags.value, l.name^);
		WHILE flag # NIL DO
			name := flag.name;
			IF Strings.Equal(name^, "NOLIB") THEN
				SL.Include(omitLibs, l.name);
			ELSE
				buffer.String(name^); buffer.Char(" ");
			END;
			flag := flag.next;
		END;
		l := l.next;
	END;

	(* include all object files *)
	l := objs;
	WHILE l # NIL DO
		buffer.String(l.name^); buffer.String(".c ");
		l := l.next;
	END;

	(* output name *)
	buffer.String("-o ");
	buffer.String(module.name^);
	buffer.Char(" ");

	(* include all necessary libraries *)
	l := libs;
	WHILE l # NIL DO
		IF ~SL.Contains(omitLibs, l.name) THEN
			buffer.String("-l");
			buffer.String(l.name^);
			buffer.Char(" ");
		END;
		l := l.next;
	END;
	result := Run(buffer.GetString());
	RETURN result = 0;
END LinkModule;

PROCEDURE BuildModule(module : Module);
VAR
	libs, objs : StringList;
BEGIN
	libs := NIL;
	objs := NIL;
	IF script.value THEN
		Console.String("# OMake generated script for ");
		Console.String(module.name^);
		Console.Ln;
	END;
	IF CompileModule(module, libs, objs, TRUE) THEN
		IF LinkModule(module, libs, objs) THEN
		END;
	END;
END BuildModule;

(* Process either a file name, or module name *)

PROCEDURE ProcessFile(name : String) : Module;
VAR 
	file, library : String;
	module : Module;
BEGIN
	IF Strings.LastIndexOf(name^, ".", -1) >= 0 THEN
		RETURN ParseFile(name);
	ELSE
		LocateModule(name^, file, library);
		IF file = NIL THEN
			Console.String("Cannot locate module source "); 
			Console.String(name^); Console.Ln;
			Kernel.Exit(1);
		END;
		RETURN ParseFile(file)
	END;
END ProcessFile;

PROCEDURE Command;
VAR
	pos : LONGINT;
	m : Module;
BEGIN
	debug := C.NewBoolean("debug", "Debug output", FALSE);
	path := C.NewString("source", "Search path for Oberon source modules", "");
	extensions := C.NewAssoc("extension", "Define compiler flags for accepted extensions", TRUE);
	exclude := C.NewString("exclude", "Don't parse these module sources", "Platform,SYSTEM");
	require := C.NewString("require", "Libraries which must always be linked", "Ofront");
	libraries := C.NewAssoc("lib", "Define library source location", FALSE);
	linkFlags := C.NewAssoc("linkflag", "Define linker flags for library", TRUE);
	compiler := C.NewString("compile", "Compile command", "Ofront+");
	linker := C.NewString("link", "Link command", "gcc");
	script := C.NewBoolean("script", "Generate script instead of building", FALSE);

	IF C.ReadOptions("omake.ini") THEN
	END;
	pos := 1;
	C.ScanOptions(pos);
	WHILE pos < LEN(C.argv^) DO
		sPath := Pattern.Split(path.value^, ",");
		sExclude := Pattern.Split(exclude.value^, ",");
		sRequire := Pattern.Split(require.value^, ",");
		m := ProcessFile(C.argv[pos]);
		BuildModule(m);
		INC(pos);
		C.ScanOptions(pos);
	END;
	C.CheckFinished(pos);
END Command;

BEGIN
	modules := NIL;
	libraries := NIL;

	(* keywords inside OMAKE directive *)
	keywords := NIL;
	AddSymbol("LINK", link);
	makeKeywords := keywords;

	(* standard Oberon keywords *)
	keywords := NIL;
	AddSymbol("MODULE", module);
	AddSymbol("IMPORT", import);

	Command;
END OMake.
