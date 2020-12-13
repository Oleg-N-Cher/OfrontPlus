(* Ofront+ Error Filter - Show error messages from the compiler in a
human-readable format, including the source code position and context.

This program parses messages from standard input (via Console) which will
normally be piped from the compiler output. For example:

  ofront+ -2 -88 -m Module.Mod | OEF

Parameters are:

contextLines: (default: 1)
	The number of lines of source code to display before and after the error
	line Setting this to zero displays just the error line and message

tabSize: (default: 2)
	Number of spaces per tab. Tabs are expanded by padding to the column that
	is the next multiple of the tab size. OEF will adjust the column pointer to
	account for tabs.

width: (default: 80)
	By default the error message is shown after the position pointer, but when
	the error message is long, this can lead to bad formatting that wraps
	around to the next line. If it looks like the message will extend past
	<width>, OEF will place the error message at the start of the next line.

withLineNumbers: (default: TRUE)
	When enabled, line numbers are displayed in front of each source line.
	When disabled, line numbers are shown after the error message

alwaysWrap: (default: FALSE)
	When true, error messages are always shown on a separate line. Normal
	behaviour is to only do this for very wide lines. See discussion of width
	above.

showSeparators: (default: TRUE)
	When true, a separator is shown between distcontiguous source code lines.
	This normally happens between context blocks.
*)

(* SG 2020/08/21 *)

MODULE OEF;

IMPORT 
	Kernel,
	Files,
	Console,
	Platform,
	Pattern := arPattern,
	Strings := arStrings,
	C := arConfiguration;

TYPE
	String = Pattern.String;

	Message = POINTER TO RECORD
		line : INTEGER;
		column : INTEGER;
		code : INTEGER;
		text : String;
		next : Message;
	END;

CONST
	filePattern = "* translating *";
	messagePattern = " *:* err * *";
	TAB = 9X;

VAR
	(* number of lines of source code context to show before and after error line *)
	contextLines : C.Integer;

	(* number of spaces equivalent to one tab *)
	tabSize : C.Integer;

	(* show a separator at discontinuities between context blocks *)
	showSeparators : C.Boolean;

	(* TRUE to always show error message on a separate line. Otherwise, it will be appended to the column pointer if there is room *)
	alwaysWrap : C.Boolean;

	(* Nominal display width, to control wrapping *)
	width : C.Integer;

	(* display line numbers for source lines *)
	withLineNumbers : C.Boolean;

	(* current set of paths to search files *)
	OBERON: ARRAY 1024 OF CHAR;

PROCEDURE Min(x, y : INTEGER) : INTEGER;
BEGIN
	IF x < y THEN RETURN x ELSE RETURN y END;
END Min;

PROCEDURE Max(x, y : INTEGER) : INTEGER;
BEGIN
	IF x > y THEN RETURN x ELSE RETURN y END;
END Max;

(* convert string to integer *)

PROCEDURE StringToInt(VAR s : ARRAY OF CHAR) : INTEGER;
VAR value, i : INTEGER; ch : CHAR;
BEGIN
	value := 0;
	i := 0; ch := s[i];
	WHILE (ch >= '0') & (ch <= '9') DO
		value := value * 10 + ORD(ch) - ORD('0');
		INC(i); ch := s[i];
	END;
	RETURN value;
END StringToInt;

(* conditionally output string *)

PROCEDURE CondOutString(condition : BOOLEAN; string : ARRAY OF CHAR);
BEGIN
	IF condition THEN
		Console.String(string);
	END;
END CondOutString;

(* Read line from file, and return FALSE at end of input *)

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

(* Read line of input from stdin, and return FALSE at end of input *)
(* FIXME: Avoid duplication by reading STDIN via Files.Rider if possible? *)

PROCEDURE ReadLineFromStdin(VAR line : ARRAY OF CHAR) : BOOLEAN;
VAR
	i : INTEGER;
	ch : CHAR;
BEGIN
	i := 0;
	Console.Read(ch);
	IF ch = 0X THEN RETURN FALSE END;
	WHILE (ch # 0AX) & (ch # 0X) DO
		IF (ch # 0DX) & (i < LEN(line)-1) THEN
			line[i] := ch; INC(i);
		END;
		Console.Read(ch);
	END;
	line[i] := 0X;
	RETURN TRUE;
END ReadLineFromStdin;

(* Insert message in list, ordered by line and column *)

PROCEDURE InsertMessage(list, item : Message);
BEGIN
	WHILE (list.next # NIL) & ((list.next.line < item.line) OR ((list.next.line = item.line) & (list.next.column < item.column))) DO
		list := list.next;
	END;
	item.next := list.next;
	list.next := item;
END InsertMessage;

(* Emit a line of source from <buffer>, expanding any tabs. Output the
horizontal character position <position> for each column number in original
source line. *)

PROCEDURE EmitLine(VAR buffer : ARRAY OF CHAR; VAR position : ARRAY OF INTEGER);
VAR i, pos : INTEGER; ch : CHAR;
BEGIN
	i := 0; pos := 0; ch := buffer[i];
	WHILE ch # 0X DO
		position[i] := pos;
		IF ch = TAB THEN
			Console.Char(' '); INC(pos);
			WHILE pos MOD tabSize.value # 0 DO
				Console.Char(' '); INC(pos);
			END;
		ELSE
			Console.Char(ch); INC(pos);
		END;
		INC(i); ch := buffer[i];
	END;
	Console.Ln;
END EmitLine;

(* Emit an arrow pointing to <position>. *)

PROCEDURE EmitArrow(position : INTEGER; body, tip : CHAR);
VAR i : INTEGER; ch : CHAR;
BEGIN
	DEC(position);
	WHILE position > 0 DO
		DEC(position); Console.Char(body);
	END;
	Console.Char(tip);
END EmitArrow;

(* Emit messages with context lines from original source file. 
<contextLines> defines the number of additional source lines displayed before and
after the message line. *)

PROCEDURE ProcessFile(fileName : ARRAY OF CHAR; message : Message);
CONST
	maxLineLength = 128;
VAR
	line : INTEGER;
	trailingContext : INTEGER;
	buffer : ARRAY maxLineLength OF CHAR;
	position : ARRAY maxLineLength OF INTEGER;
	rd : Files.Rider;
	file : Files.File;
	needSeparator : BOOLEAN;
	columnPosition : INTEGER;
BEGIN
	trailingContext := 0;
	needSeparator := FALSE;
	file := Files.Old(fileName);
	IF file = NIL THEN
		Console.String("Cannot open file: "); Console.String(fileName); Console.Ln;
		RETURN
	END;
	Files.Set(rd, file, 0);
	line := 0;
	WHILE ReadLineFromFile(buffer, rd) DO
		INC(line);
		(* look-ahead for leading context lines *)
		IF (line <= trailingContext) OR ((message # NIL) & (ABS(line-message.line)<=contextLines.value)) THEN
			IF (line > trailingContext) & needSeparator THEN
				(* output a separator after trailing context *)
				needSeparator := FALSE;
				CondOutString(withLineNumbers.value, "-----");
				Console.Ln;
			END;
			(* emit the source line *)
			IF withLineNumbers.value THEN
				(* formatting assumes < 10000 source lines *)
				Console.Int(line, 4); Console.String("| ");
			END;
			EmitLine(buffer, position);
			(* now emit message lines. May be multiple messages per line *)
			WHILE (message # NIL) & (message.line = line) DO
				trailingContext := line + contextLines.value;
				needSeparator := showSeparators.value;
				CondOutString(withLineNumbers.value, "    +-");
				columnPosition := position[Max(0, Min(LEN(position)-1, message.column-1))];
				EmitArrow(columnPosition, '-', '^');
				(* if message will wrap awkwardly after arrow, start on a new line *)
				IF ~alwaysWrap.value & (columnPosition + Strings.Length(message.text^) + 7 < width.value) THEN
					Console.String(" ");
				ELSE
					Console.Ln;
					CondOutString(withLineNumbers.value, "    | ");
				END;
				Console.String(message.text^);
				IF ~withLineNumbers.value THEN
					Console.String(" @"); Console.Int(message.line, 0);
					Console.String(":"); Console.Int(message.column, 0);
				END;
				Console.Ln;
				message := message.next;
			END;
		END;
	END;
	Files.Close(file);
END ProcessFile;

(* Process input, normally piped from the compiler output *)

PROCEDURE ProcessInput;
VAR	
	line : ARRAY 128 OF CHAR;
	fileName : Pattern.String;
	match : Pattern.StringArray;
	message, list : Message;
BEGIN
	(* Note: message list has dummy header node to facilitate insertion *)
	NEW(list); list.next := NIL;
	fileName := NIL;

	WHILE ReadLineFromStdin(line) DO
		IF Pattern.Match(filePattern, line, match) THEN
			(* when file changes, emit any previous messages first *)
			IF (fileName # NIL) & (list.next # NIL) THEN
				ProcessFile(fileName^, list.next);
				NEW(list); list.next := NIL;
			END;
			(* now set the new file *)
			fileName := match[0];
			Console.String(line); Console.Ln;
		ELSIF Pattern.Match(messagePattern, line, match) THEN
			NEW(message);
			message.line := StringToInt(match[0]^);
			message.column := StringToInt(match[1]^);
			message.code := StringToInt(match[2]^);
			message.text := match[3];
			message.text[0] := CAP(message.text[0]);
			InsertMessage(list, message);
		ELSE
			Console.String(line); Console.Ln;
		END;
	END;
	IF fileName # NIL THEN
		ProcessFile(fileName^, list.next);
	END;
END ProcessInput;

PROCEDURE Options;
VAR pos : LONGINT;
BEGIN
	withLineNumbers := 
		C.NewBoolean("numbers", "Show line numbers before source lines", TRUE);
	alwaysWrap := 	
		C.NewBoolean("wrap", "Always show error message on separate line", FALSE);
	showSeparators := 
		C.NewBoolean("separate", "Separate discontiguous source lines", TRUE);
	contextLines := 
		C.NewInteger("context <N>", "Show <N> context lines before and after message", 1, 0, 10);
	tabSize := 
		C.NewInteger("tabs <N>", "Set <N> spaces per tab character", 2, 1, C.None);
	width := 	
		C.NewInteger("width <N>", "Wrap messages extending past column <N>", 80, 40, C.None);
	IF C.ReadOptions("oef.ini") THEN
		(* Console.String("Read parameters from file"); Console.Ln; *)
	END;
	pos := 1;
	C.ScanOptions(pos);
	C.CheckFinished(pos);
END Options;

BEGIN
	OBERON := "."; Platform.GetEnv("OBERON", OBERON);
	Files.SetSearchPath(OBERON);
	C.SetBanner("oef - Ofront+ Error Filter\nThis tool shows source code context for Ofront+ compiler messages\nUsage:\n  ofront+ ... Module.Mod | oef [options]\n");
	Options;
	ProcessInput;
END OEF.
