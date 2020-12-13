(*
Text.Writer knows how to convert various primitive values to text.
Text.Stream is a simpler interface that just converts characters and strings
(but see below).

Basic Writer operations are:
  Char, String, Integer, Real, Boolean, Set, Ln
  StringLiteral is for strings that are part of data structures (eg. JSON)

Some options are:
  intFormat: format to use for integers (default is "%d")
  realFormat: format to use for reals (default is "%g")
  quoteLiterals: TRUE to quote string literals
  setAsList : output sets with [] delimiters (for JSON) rather than {}

Text.Buffer is a mutable text string that can be built incrementally

Text.Buffers are often instantiated statically as local variables, and are
used for constructing strings. Logically: A Buffer IS a Stream, and a Writer
USES a Stream, but this would mean instantiating a separate object on the
heap (ie.  Text.Buffer) whenever a string needs to be constructed. Hence this
slightly unsatisfactory construction in which: a Writer IS a Stream and a
Buffer IS a Writer. As a consolation, we offer StreamWriter,  an alternative
Writer that USES a separate Stream.

*)

MODULE arText;

IMPORT 
	SYSTEM, (* MOVE, ADR *)
	Platform,
	CFormat := arCFormat; 

TYPE
	String* = POINTER TO ARRAY OF CHAR;

	StreamPtr* = POINTER TO Stream;
	Stream* = RECORD
	END;

	Writer* = RECORD (Stream)
		intFormat* : ARRAY 8 OF CHAR;
		hexFormat* : ARRAY 8 OF CHAR;
		realFormat* : ARRAY 8 OF CHAR;
		quoteLiterals* : BOOLEAN;
		setAsList* : BOOLEAN;
	END;

	StreamWriter* = RECORD (Writer)
		stream- : StreamPtr;
	END;

	Buffer* = RECORD (Writer)
		length- : LONGINT;
		capacity- : LONGINT;
		value- : String;
	END;

(* ------ Stream ------ *)

PROCEDURE (VAR self : Stream) Char*(value : CHAR);
BEGIN
	(* abstract *)
END Char;

PROCEDURE (VAR self : Stream) String*(value-: ARRAY OF CHAR);
BEGIN
	(* abstract *)
END String;

PROCEDURE (VAR self : Stream) Flush*;
BEGIN
	(* abstract *)
END Flush;

(* ------ Writer ------ *)

PROCEDURE (VAR self : Writer) InitWriter*;
BEGIN
	self.intFormat := "%d";
	self.hexFormat := "%x";
	self.realFormat := "%g";
	self.quoteLiterals := FALSE;
	self.setAsList := FALSE;
END InitWriter;

PROCEDURE (VAR self : Writer) StringLiteral*(value : ARRAY OF CHAR);
BEGIN
	IF self.quoteLiterals THEN 
		self.Char('"');
		self.String(value);
		self.Char('"');
	ELSE
		self.String(value);
	END;
END StringLiteral;

PROCEDURE (VAR self : Writer) Integer*(value : LONGINT);
VAR buffer : ARRAY 32 OF CHAR;
BEGIN 
	CFormat.IntToString(buffer, self.intFormat, value);
	self.String(buffer);
END Integer;

PROCEDURE (VAR self : Writer) Hex*(value : LONGINT);
VAR buffer : ARRAY 32 OF CHAR;
BEGIN 
	CFormat.IntToString(buffer, self.hexFormat, value);
	self.String(buffer);
END Hex;

PROCEDURE (VAR self : Writer) Real*(value : LONGREAL);
VAR buffer : ARRAY 32 OF CHAR;
BEGIN
	CFormat.RealToString(buffer, self.realFormat, value);
	self.String(buffer);
END Real;

PROCEDURE (VAR self : Writer) Boolean*(value : BOOLEAN);
BEGIN
	IF value THEN self.String("true") ELSE self.String("false") END;
END Boolean;

PROCEDURE (VAR self : Writer) Set*(value : SET);
VAR i, count : INTEGER;
BEGIN
	count := 0;
	IF self.setAsList THEN self.Char("[") ELSE self.Char("{") END;
	FOR i := 0 TO MAX(SET) DO
		IF i IN value THEN
			IF count > 0 THEN self.Char(",") END;
			self.Integer(i);
			INC(count);
		END;
	END;
	IF self.setAsList THEN self.Char("]") ELSE self.Char("}") END;
END Set;

PROCEDURE (VAR self : Writer) Ln*;
BEGIN
	self.String(Platform.NewLine);
END Ln;

(* ----- Buffer ----- *)

PROCEDURE (VAR self : Buffer) Init* (capacity : LONGINT);
BEGIN
	self.InitWriter();
	self.length := 0;
	IF capacity < 8 THEN capacity := 8; END;
	self.capacity := capacity;
	NEW(self.value, capacity);
END Init;

PROCEDURE (VAR self : Buffer) EnsureCapacity(capacity : LONGINT);
VAR 
	newCapacity : LONGINT;
	newValue : String;
BEGIN
	IF self.length < capacity THEN
		newCapacity := self.capacity;
		WHILE newCapacity < capacity DO
			newCapacity := newCapacity * 2;
		END;
		self.capacity := newCapacity;
		NEW(newValue, newCapacity);
		SYSTEM.MOVE(SYSTEM.ADR(self.value[0]), SYSTEM.ADR(newValue[0]), self.length);
		(* COPY(self.value^, newValue^); *)
		self.value := newValue;
	END;
END EnsureCapacity;

PROCEDURE (VAR self : Buffer) Char*(value : CHAR);
BEGIN
	self.EnsureCapacity(self.length+1);
	self.value[self.length] := value;
	INC(self.length);
END Char;

PROCEDURE (VAR self : Buffer) String*(value-: ARRAY OF CHAR);
VAR 
	i, length : LONGINT;
BEGIN
	length := 0;
	WHILE (length<LEN(value)) & (value[length] # 0X) DO
		INC(length)
	END;
	self.EnsureCapacity(self.length + length);
	FOR i := 0 TO length-1 DO
		self.value[self.length+i] := value[i];
	END;
	INC(self.length, length);
END String;

PROCEDURE (VAR self : Buffer) Terminate*();
BEGIN
	(* add null terminator *)
	self.EnsureCapacity(self.length+1);
	self.value[self.length] := 0X;
END Terminate;

PROCEDURE (VAR self : Buffer) Clear*();
BEGIN
	self.length := 0;
END Clear;

PROCEDURE (VAR self : Buffer) GetString*() : String;
BEGIN
	self.Terminate();
	RETURN self.value;
END GetString;

(* ----- StreamWriter ----- *)

PROCEDURE (VAR self : StreamWriter) Init*(stream : StreamPtr);
BEGIN
	self.InitWriter;
	self.stream := stream;
END Init;

PROCEDURE (VAR self : StreamWriter) Char*(value : CHAR);
BEGIN
	self.stream.Char(value);
END Char;

PROCEDURE (VAR self : StreamWriter) String*(value-: ARRAY OF CHAR);
BEGIN
	self.stream.String(value);
END String;

PROCEDURE (VAR self : StreamWriter) Flush*;
BEGIN
	self.stream.Flush;
END Flush;

END arText.
