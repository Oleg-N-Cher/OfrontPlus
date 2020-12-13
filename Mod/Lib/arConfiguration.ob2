
(* Configuration - Setting program options via command line or configuration file

This module handles:
	Setting and validating values via the command line
	Reading parameter values from a configuration file
	Printing usage help text, including options and default values

Each program parameter is declared with:
	name, 
	description, 
	type information (eg. maximum, minimum value)
	default value

Create a parameter using one of the following:
	NewBoolean(name, desc, default)
	NewInteger(name, desc, default, min, max)
	NewString(name, desc, default)
	NewAssoc(name, desc, multi)

<name> includes the name and a description of any parameters. For example:
	"tabs <N>"
	"debug"
The returned object has a <value> field of the corresponding type.

To scan parameters from the command line:
	C.ScanOptions(pos);
which returns the position of the first parameter that is not an option

To scan parameters from a file:
	success := C.ReadOptions("program.ini")
which returns TRUE if the file was processed, and FALSE if it cannot be opened.

Example:
	IMPORT C := Configuration;
	VAR
		tabs : C.Integer;
		debug : C.Boolean;
	...
	tabs := C.NewInteger("tabs <N>", "Set <N> spaces per tab", 2, 1, C.None);
	debug := C.NewBoolean("debug", "Enable debugging output", FALSE);
	...
	IF debug.value THEN
		Console.String("Tabs are set to "); Out.Int(tabs.value, 0); Out.Ln;
	...
	success := C.ReadOptions("program.ini")
	pos := 1;
	C.ScanOptions(pos);
	... handle any positional parameters
	C.CheckFinished(pos);
*)

(* SG 2020/08/27 *)

MODULE arConfiguration;

IMPORT 
	Kernel,
	Console,
	Args, 
	Platform,
	Files, 

	Size := arSize,
	SA := arStringAssoc,
	SL := arStringList,
	P := arPattern,
	Strings := arStrings;

CONST
	None* = MIN(INTEGER)+1;		(* maximum or minimum is not defined *)

TYPE
	Handler* = PROCEDURE(argv : SL.Array; VAR pos : LONGINT);

	Value* = POINTER TO ValueDesc;
	ValueDesc* = RECORD
		name- : SL.String;
		synopsis- : SL.String;
		desc- : SL.String;
		next- : Value;
	END;

	Boolean* = POINTER TO RECORD (ValueDesc)
		value* : BOOLEAN;
	END;

	Integer* = POINTER TO RECORD (ValueDesc)
		value*: INTEGER;
		min-, max- : INTEGER;
	END;

	String* = POINTER TO RECORD (ValueDesc)
		value*: SL.String;
	END;

	Procedure* = POINTER TO RECORD (ValueDesc)
		value: Handler;
	END;

	Assoc* = POINTER TO RECORD (ValueDesc)
		value* : SA.Assoc;
		multi- : BOOLEAN;
	END;

VAR
	(* copy of arg vector as an Oberon structure *)
	argc : LONGINT;
	argv- : SL.Array;

	(* list of all configuration values *)
	values- : Value;

	banner : SL.String;

	settings : Assoc;

(* Platform.ArgPtr is not exported  :-(

PROCEDURE CopyArgPtr(p : Platform.ArgPtr) : SL.String;
VAR len, i : INTEGER; result : SL.String;
BEGIN
	len := 0;
	WHILE p[len] # 0X DO
		INC(len);
	END:
	NEW(result, len+1);
	FOR i := 0 TO len DO
		result[i] := p[i]
	END;
	RETURN result;
END CopyArgPtr;
*)

(* Copy C argument vector into an Oberon array *)

PROCEDURE CopyArgVec(p : Platform.ArgVec) : SL.Array;
VAR
	len, strLen : INTEGER;
	i, j : INTEGER; 
	result : SL.Array;
BEGIN
	len := 0;
	WHILE p[len] # NIL DO
		INC(len);
	END;
	NEW(result, len);
	FOR i := 0 TO len-1 DO
		(* calculate length of C arg *)
		strLen := 0;
		WHILE p[i][strLen] # 0X DO
			INC(strLen)
		END;
		(* copy arg into Oberon string *)
		NEW(result[i], strLen+1);
		FOR j := 0 TO strLen DO
			result[i][j] := p[i][j]
		END;
	END;
	RETURN result;
END CopyArgVec;

(* ------ Value ------ *)

PROCEDURE (self : Value) Init*(name, desc: ARRAY OF CHAR);
VAR parts : SL.Array;
BEGIN
	self.synopsis := SL.Copy(name);
	parts := P.Split(name, " ");
	self.name := parts[0];
	self.desc := SL.Copy(desc);
	self.next := values;
	values := self;
END Init;

(* output the current value as text *)

PROCEDURE (self : Value) Format*;
BEGIN
	(* abstract *)
END Format;

(* Scan as many command line arguments as required from position <i> in the
argument array <argv>. On any errors, show usage and halt. After consuming the
argument, increment <pos> to indicate the position of the next unused argument.
*)

PROCEDURE (self : Value) Accept*(argv : SL.Array; VAR pos : LONGINT);
BEGIN
	(* abstract *)
END Accept;

(* ------ Scanning and printing ------ *)

(* Output <count> spaces *)

PROCEDURE Spaces(count : LONGINT);
BEGIN
	WHILE count > 0 DO
		Console.Char(" "); DEC(count);
	END;
END Spaces;

PROCEDURE OutString(string : ARRAY OF CHAR);
VAR i : LONGINT; ch : CHAR;
BEGIN
	i := 0;
	LOOP
		ch := string[i];
		IF ch = 0X THEN	RETURN END;
		INC(i);
		IF ch = '\' THEN
			IF string[i] = 'n' THEN
				Console.Ln; INC(i);
			ELSE
				Console.Char(ch);
			END;
		ELSE
			Console.Char(ch);
		END;
	END;
END OutString;

(* Show program usage *)

PROCEDURE Usage*;
VAR 
	value : Value;
	nameWidth, descWidth, length, sLength: LONGINT;
BEGIN
	IF banner # NIL THEN
		OutString(banner^);
	END;
	(* compute maximum name and description width *)
	nameWidth := 0; descWidth := 0; length := 0;
	value := values;
	WHILE value # NIL DO
		nameWidth := Size.Max(nameWidth, Strings.Length(value.synopsis^));
		descWidth := Size.Max(descWidth, Strings.Length(value.desc^));
		INC(length);
		value := value.next;
	END;

	(* limit space in case of very long names *)
	nameWidth := Size.Min(nameWidth, 15);

	IF length > 0 THEN
		Console.String("Options:"); Console.Ln;
		value := values;
		WHILE value # NIL DO
			Console.String("  ");
			Console.Char("-"); Console.String(value.synopsis^);
			sLength := Strings.Length(value.synopsis^);
			(* take a separate line if name is too long *)
			IF sLength > nameWidth THEN
				Console.Ln; Spaces(3+nameWidth);
			ELSE
				Spaces(nameWidth-sLength);
			END;
			Console.String("  ");
			Console.String(value.desc^); Spaces(1+descWidth-Strings.Length(value.desc^));
			Console.Char("("); value.Format; Console.Char(")");
			Console.Ln;
			value := value.next;
		END;
	END;
END Usage;

(* Show usage and exit with error *)

PROCEDURE Error;
BEGIN
	Usage();
	Kernel.Exit(1);
END Error;

(* Show usage and exit cleanly *)

PROCEDURE Help(argv : SL.Array; VAR pos : LONGINT);
BEGIN
	Usage();
	Kernel.Exit(0);
END Help;

(* get an argument, and halt if none remain. Apply substitutions for any
"set" values *)

PROCEDURE GetArg*(argv : SL.Array; VAR pos : LONGINT; VAR arg : SL.String);
BEGIN
	IF pos < LEN(argv^) THEN
		arg := P.Substitute(argv[pos]^, settings.value); INC(pos);
	ELSE
		Console.String("Missing argument at position "); Console.Int(pos, 0); 
		Console.Ln;
		Error;
	END;
END GetArg;

(* parse integer from string, setting <result> and returning TRUE when valid *)

PROCEDURE ParseInteger*(VAR arg : ARRAY OF CHAR; VAR result : INTEGER) : BOOLEAN;
VAR
	value, sign, pos : INTEGER;
	ch : CHAR;
BEGIN
	(* parse integer *)
	value := 0; sign := 1;
	pos := 0; ch := arg[0];
	IF ch = '-' THEN
		sign := -1;
		INC(pos); ch := arg[pos];
	END;
	WHILE (ch >= '0') & (ch <= '9') DO
		value := value * 10 + ORD(ch) - ORD('0');
		INC(pos); ch := arg[pos];
	END;
	IF ch # 0X THEN RETURN FALSE END;
	result := value * sign;
	RETURN TRUE;
END ParseInteger;

(* parse boolean from string, setting <result> and returning TRUE when valid *)

PROCEDURE ParseBoolean*(VAR arg : ARRAY OF CHAR; VAR result : BOOLEAN) : BOOLEAN;
BEGIN
	IF Strings.Equal(arg, "TRUE") THEN
		result := TRUE;
	ELSIF Strings.Equal(arg, "FALSE") THEN
		result := FALSE;
	ELSE
		RETURN FALSE;
	END;
	RETURN TRUE;
END ParseBoolean;

(* find an option with the given <name> *)

PROCEDURE FindOption(name : ARRAY OF CHAR) : Value;
VAR value : Value;
BEGIN
	value := values;
	WHILE value # NIL DO
		IF Strings.Equal(value.name^, name) THEN
			RETURN value;
		END;
		value := value.next;
	END;
	RETURN NIL;
END FindOption;

(* If an option is at the current position, process it, update pos and RETURN
TRUE *)

PROCEDURE CheckOption*(VAR pos : LONGINT) : BOOLEAN;
VAR
	arg : SL.String;
	value : Value;
	option : SL.String; length : Strings.LengthType;
BEGIN
	arg := argv[pos];
	IF arg[0] = '-' THEN
		INC(pos);
		length := Strings.Length(arg^); NEW(option, length);
		Strings.Extract(arg^, 1, length-1, option^);
		value := FindOption(option^);
		IF value = NIL THEN
			Console.String("ERROR: Unknown option: "); Console.String(option^); 
			Console.Ln;
			Error;
		END;
		value.Accept(argv, pos);
		RETURN TRUE;
	END;
	RETURN FALSE;
END CheckOption;

(* Process a sequence of options on the command line. Return the argument
position of the first argument that is not an option. Options start with '-',
and may include an additional argument specifying a value *)

PROCEDURE ScanOptions*(VAR pos : LONGINT);
BEGIN
	WHILE (pos < argc) & CheckOption(pos) DO
	END;
END ScanOptions;

PROCEDURE SetBanner*(text : ARRAY OF CHAR);
BEGIN
	banner := SL.Copy(text);
END SetBanner;

PROCEDURE ReadLineFromFile(VAR line : ARRAY OF CHAR; VAR rd : Files.Rider) : BOOLEAN;
VAR
	i : INTEGER;
	ch : CHAR;
BEGIN
	i := 0;
	Files.ReadChar(rd, ch);
	IF ch = 0X THEN RETURN FALSE END;
	WHILE (ch # 0AX) & (ch # 0X) DO
		IF (ch # 0DX) & (i < LEN(line)-1) THEN
			line[i] := ch; INC(i);
		END;
		Files.ReadChar(rd, ch);
	END;
	line[i] := 0X;
	RETURN TRUE;
END ReadLineFromFile;

(* Read options from file *)

PROCEDURE ReadOptions*(fileName : ARRAY OF CHAR) : BOOLEAN;
VAR 
	line : ARRAY 1024 OF CHAR;
	nLines : INTEGER;
	match : SL.Array;
	rd : Files.Rider;
	file : Files.File;
	name : SL.String;
	value : Value;
	pos : LONGINT;

	PROCEDURE ErrorLine(reason, context : ARRAY OF CHAR);
	BEGIN
		Console.String("ERROR: "); Console.String(reason);
		Console.String(" '"); Console.String(context);
		Console.String("' on line "); Console.Int(nLines, 0);
		Console.String(" of file "); Console.String(fileName);
		Console.Ln;
		Error;
	END ErrorLine;

BEGIN
	file := Files.Old(fileName);
	IF file = NIL THEN
		RETURN FALSE;
	END;
	Files.Set(rd, file, 0);
	nLines := 0;
	WHILE ReadLineFromFile(line, rd) DO
		INC(nLines);
		IF (line[0] = 0X) OR (line[0] = '#') THEN
			(* ignore empty lines and comments *)
		ELSIF P.Match("*: *", line, match) THEN
			name := match[0];
			value := FindOption(name^);
			IF value = NIL THEN
				ErrorLine("Unknown option", name^);
			END;
			match := P.ParseArgs(match[1]^);
			pos := 0;
			value.Accept(match, pos);
		ELSE
			ErrorLine("Malformed line", line);
		END;
	END;
	Files.Close(file);
	RETURN TRUE;
END ReadOptions;

PROCEDURE Read(argv : SL.Array; VAR pos : LONGINT);
VAR 
	arg : SL.String;
BEGIN
	GetArg(argv, pos, arg);
	IF ~ReadOptions(arg^) THEN
		Console.String("Cannot read file: "); Console.String(arg^); Console.Ln;
		Error;
	END;
END Read;

(* Make sure there are no extraneous arguments after <pos> *)

PROCEDURE CheckFinished*(pos : LONGINT);
BEGIN
	IF pos # argc THEN
		Console.String("Unexpected arguments"); Console.Ln;
		Error;
	END;
END CheckFinished;

(* ------ Boolean ------ *)

PROCEDURE (self : Boolean) Format*;
BEGIN
	IF self.value THEN
		Console.String("TRUE");
	ELSE 
		Console.String("FALSE");
	END
END Format;

PROCEDURE (self : Boolean) Accept*(argv : SL.Array; VAR pos : LONGINT);
VAR arg : SL.String;
BEGIN
	IF pos<LEN(argv^) THEN
		arg := argv[pos];
		(* scan Boolean value if valid *)
		IF ParseBoolean(arg^, self.value) THEN
			INC(pos); RETURN;
		END;
	END;
	self.value := ~self.value;	(* toggle Boolean value *)
END Accept;

(* Create Boolean with default value *)

PROCEDURE NewBoolean*(name, desc : ARRAY OF CHAR; default : BOOLEAN) : Boolean;
VAR result : Boolean;
BEGIN
	NEW(result); 
	result.Init(name, desc);
	result.value := default;
	RETURN result;
END NewBoolean;

(* ------ Integer ------ *)

PROCEDURE (self : Integer) Format*;
BEGIN
	Console.Int(self.value, 0);
END Format;

PROCEDURE (self : Integer) Accept*(argv : SL.Array; VAR pos : LONGINT);
VAR 	
	arg : SL.String;
	value : INTEGER;
BEGIN
	GetArg(argv, pos, arg);

	IF ~ParseInteger(arg^, value) THEN
		Console.String("ERROR: Illegal Integer value: "); Console.String(arg^); Console.Ln;
		Error;
	END;

	(* validate range *)
	IF ((self.min # None) & (value < self.min)) OR ((self.max # None) & (value > self.max)) THEN
		Console.String("ERROR: Value "); Console.Int(value, 0); 
		IF self.min = None THEN
			Console.String(" is greater than maximum of "); Console.Int(self.max, 0);
		ELSIF self.max = None THEN
			Console.String(" is less than minimum of "); Console.Int(self.min, 0);
		ELSE
			Console.String(" out of range ");
			Console.Int(self.min, 0); Console.String(" ... "); Console.Int(self.max, 0);
		END;
		Console.Ln;
		Error;
	END;

	self.value := value;
END Accept;

(* Create Integer with default value, minimum and maximum *)

PROCEDURE NewInteger*(name, desc : ARRAY OF CHAR; default, min, max : INTEGER) : Integer;
VAR result : Integer;
BEGIN
	NEW(result);
	result.Init(name, desc);
	result.value := default;
	result.min := min;
	result.max := max;
	RETURN result;
END NewInteger;

(* ------ String ------ *)

PROCEDURE StringWidth(value : ARRAY OF CHAR; width : INTEGER);
VAR i : INTEGER; length : Strings.LengthType;
BEGIN
	length := Strings.Length(value);
	IF length > width-3 THEN
		FOR i := 0 TO width-4 DO
			Console.Char(value[i])
		END;
		Console.String("...");
	ELSE
		Console.String(value);
	END;
END StringWidth;

PROCEDURE (self : String) Format*;
BEGIN
	Console.Char('"');
	Console.String(self.value^);
	(* StringWidth(self.value^, 16); *)
	Console.Char('"');
END Format;

PROCEDURE (self : String) Accept*(argv : SL.Array; VAR pos : LONGINT);
BEGIN
	GetArg(argv, pos, self.value);
END Accept;

PROCEDURE NewString*(name, desc : ARRAY OF CHAR; default : ARRAY OF CHAR) : String;
VAR result : String;
BEGIN
	NEW(result);
	result.Init(name, desc);
	result.value := SL.Copy(default);
	RETURN result;
END NewString;

(* ------ Procedure ------ *)

PROCEDURE (self : Procedure) Accept*(argv : SL.Array; VAR pos : LONGINT);
BEGIN
	self.value(argv, pos);
END Accept;

PROCEDURE NewProcedure*(name, desc : ARRAY OF CHAR; value : Handler);
VAR result : Procedure;
BEGIN
	NEW(result);
	result.Init(name, desc);
	result.value := value;
END NewProcedure;

(* ------ Assoc ------ *)

PROCEDURE (self : Assoc) Accept*(argv : SL.Array; VAR pos : LONGINT);
VAR key, value : SL.String;
BEGIN
	GetArg(argv, pos, key);
	GetArg(argv, pos, value);
	IF self.multi THEN
		SA.Set0(self.value, key^, value^);
	ELSE
		SA.Set(self.value, key^, value^);
	END;
END Accept;

PROCEDURE NewAssoc*(name, desc : ARRAY OF CHAR; multi : BOOLEAN) : Assoc;
VAR result : Assoc;
BEGIN
	NEW(result);
	result.Init(name, desc);
	result.value := NIL;
	result.multi := multi;
	RETURN result;
END NewAssoc;

BEGIN
	argv := CopyArgVec(Args.argv);
	argc := LEN(argv^);
	banner := NIL;
	values := NIL;
	NewProcedure("help", "Display this message", Help);
	NewProcedure("read", "Read parameters from file", Read);
	settings := NewAssoc("set", "Define a macro parameter", FALSE);
END arConfiguration.
