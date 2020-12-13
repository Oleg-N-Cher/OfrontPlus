(* JSON parser *)

MODULE arJSON;

IMPORT
	Files,
	String := arStrings,

	Out := arOut,
	Text := arText,
	V := arValue;

CONST
	optComments* = 0;
	optRelaxKeys* = 1;
	optJS* = {optComments, optRelaxKeys};
	optDebug* = 2;

(*
1 - Unexpected character in value
2 - String quote expected
3 - Invalid control character in string
4 - Unknown escape character in string
5 - Invalid number format : failed to parse
6 - Undefined identifier
7 - Expected character not found
*)

PROCEDURE ParseJSON* (text-: ARRAY OF CHAR; VAR err : LONGINT; features : SET) : V.Object;
CONST
	CR=0DX; LF=0AX; TAB=09X; SPACE=20X; FF=0CX; BS=08X;
VAR
	pos, length : LONGINT;
	ch : CHAR;

	(* get next character into ch, setting 0X when complete *)
	PROCEDURE next0;
	BEGIN
		IF pos < length THEN
			ch := text[pos]; INC(pos);
		ELSE
			ch := 0X;
		END;
		IF optDebug IN features THEN
			Out.Char(ch);
		END;
	END next0;

	PROCEDURE skipSpace0;
	BEGIN
		WHILE (ch=CR) OR (ch=LF) OR (ch=TAB) OR (ch=SPACE) DO
			next0
		END
	END skipSpace0;

	PROCEDURE skipSpace;
	VAR chLast, chSafe : CHAR; posSafe : LONGINT;
	BEGIN
		skipSpace0;
		LOOP
			IF (ch#'/') OR ~(optComments IN features) THEN EXIT END;
			chSafe := ch; posSafe := pos;
			next0;	(* lookahead *)
			IF ch='/' THEN
				(* javascript comment until end of line *)
				next0;
				WHILE (ch # LF) & (ch # 0X) DO
					next0;
				END;
				skipSpace0;
			ELSIF ch='*' THEN
				(* javascript comment until closing '*/' *)
				next0;
				chLast := 0X;
				LOOP
					IF (ch=0X) OR ((ch='/') & (chLast='*')) THEN EXIT END;
					chLast := ch;
					next0;
				END;
				next0;
				skipSpace0;
			ELSE
				ch := chSafe; pos := posSafe; (* hack! restore lookahed *)
				EXIT;
			END
		END;
	END skipSpace;

	(* get next character, bypassing any space *)
	PROCEDURE next;
	BEGIN
		next0;
		skipSpace;
	END next;

	PROCEDURE error(code : LONGINT);
	BEGIN
		Out.String("Error: "); Out.Int(code);
		Out.String(" at position "); Out.Int(pos); 
		Out.Ln;
		err := code;
	END error;

	PROCEDURE consume(expect : CHAR);
	BEGIN
		IF ch = expect THEN next 
		ELSE
			Out.String("Error: Expected '"); Out.Char(expect);
			Out.String("' but found '"); Out.Char(ch);
			Out.String("' at position "); Out.Int(pos);
			Out.Ln;
			err := 7;
		END;
	END consume;

	PROCEDURE ^parseValue() : V.Object;

	PROCEDURE parseString() : V.String;
	VAR buffer : Text.Buffer;
	BEGIN
		buffer.Init(16);
		IF ch # '"' THEN 
			error(2); 
		ELSE 
			next0;
			LOOP
				IF ch = '"' THEN EXIT 
				ELSIF ch < SPACE THEN error(3); EXIT
				ELSIF ch = '\' THEN
					next0;
					CASE ch OF
					| '"', '\', '/':	(* literal value *)
					| 'b': ch := BS
					| 'f': ch := FF
					| 'n': ch := LF
					| 'r': ch := CR
					| 't': ch := TAB
					ELSE
						error(4);
					END;
				END;
				buffer.Char(ch); next0;
			END;
			next;
		END;
		RETURN V.strb(buffer);
	END parseString;

	PROCEDURE parseNumber() : V.Object;
	VAR
		negate, isReal : BOOLEAN;
		buffer : Text.Buffer;
		result : V.Object;

		PROCEDURE digits();
		BEGIN
			WHILE (ch >= '0') & (ch <= '9') DO
				buffer.Char(ch); next0;
			END;
		END digits;
	BEGIN
		buffer.Init(32);
		negate := ch = '-';
		IF negate THEN buffer.Char(ch); next0 END;
		digits();
		isReal := FALSE;
		IF ch = '.' THEN
			isReal := TRUE;
			buffer.Char(ch); next0;
			digits();
		END;
		IF (ch = 'E') OR (ch = 'e') THEN
			isReal := TRUE;
			buffer.Char('E'); next0;
			IF (ch = '-') OR (ch = '+') THEN
				buffer.Char(ch); next0;
			END;
			digits();
		END;
		skipSpace;
		buffer.Terminate();
		IF isReal THEN
			result := V.ParseReal(buffer.value^);
		ELSE
			result := V.ParseInt(buffer.value^);
		END;
		IF result = NIL THEN 
			error(5); RETURN V.strb(buffer); 
		END;
		RETURN result
	END parseNumber;

	PROCEDURE isAlpha(ch : CHAR) : BOOLEAN;
	BEGIN
		RETURN ((ch >= 'a') & (ch <= 'z')) OR ((ch >='A') & (ch <= 'Z'))
	END isAlpha;

	PROCEDURE getIdent() : V.String;
	VAR buffer : Text.Buffer;
	BEGIN
		buffer.Init(32);
		WHILE isAlpha(ch) DO
			buffer.Char(ch); next0;
		END;
		skipSpace;
		RETURN V.strb(buffer);
	END getIdent;

	PROCEDURE parseIdent() : V.Object;
	VAR i : V.String;
	BEGIN
		i := getIdent();
		IF i.value^ = "true" THEN
			RETURN V.bool(TRUE);
		ELSIF i.value^ = "false" THEN
			RETURN V.bool(FALSE);
		END;
		error(6); RETURN i;
	END parseIdent;

	PROCEDURE parseObject() : V.Record;
	VAR r : V.Record;
		PROCEDURE keyValue();
		VAR key : V.String; value : V.Object;
		BEGIN
			IF (optRelaxKeys IN features) & isAlpha(ch) THEN
				key := getIdent()
			ELSE
				key := parseString();
			END;
			consume(':');
			value := parseValue();
			r.Set(key.value^, value);
		END keyValue;
	BEGIN
		next;
		r := V.record();
		IF ch # '}' THEN
			keyValue;
			WHILE ch = ',' DO
				next;
				keyValue;
			END
		END;
		consume('}');
		RETURN r;
	END parseObject;

	PROCEDURE parseList() : V.List;
	VAR l : V.List;
	BEGIN
		next;
		l := V.list();
		IF ch # ']' THEN
			l.Append(parseValue());
			WHILE ch = ',' DO
				next;
				l.Append(parseValue());
			END;
		END;
		consume(']');
		RETURN l;
	END parseList;

	PROCEDURE parseValue() : V.Object;
	BEGIN
		IF isAlpha(ch) THEN RETURN parseIdent() END;
		CASE ch OF
		| '{' : RETURN parseObject();
		| '[' : RETURN parseList();
		| '"' : RETURN parseString();
		| '0'..'9', '-': RETURN parseNumber();
		ELSE
			error(1); RETURN NIL;
		END
	END parseValue;

BEGIN
	length := String.Length(text);
	pos := 0;
	next;
	err := 0;
	RETURN parseValue();
END ParseJSON;

PROCEDURE ReadFile*(name-: ARRAY OF CHAR) : V.String;
VAR 
	ch : CHAR; 
	f : Files.File;
	r : Files.Rider;
	s : Text.Buffer;
BEGIN
	f := Files.Old(name);
	IF f = NIL THEN RETURN NIL END;
	Files.Set(r, f, 0);
	s.Init(128);
	LOOP
		Files.ReadChar(r, ch);
		IF ch = 0X THEN 
			Files.Close(f);
			RETURN V.strb(s) 
		END;
		s.Char(ch);
	END
END ReadFile;

PROCEDURE ParseString*(text : ARRAY OF CHAR) : V.Object;
VAR result : V.Object; res : LONGINT;
BEGIN
	result := ParseJSON(text, res, optJS);
	IF res # 0 THEN RETURN NIL END;
	RETURN result;
END ParseString;

PROCEDURE ReadJSON*(name-: ARRAY OF CHAR) : V.Object;
VAR text : V.String; 
BEGIN
	text := ReadFile(name);
	IF text = NIL THEN RETURN NIL END;
	RETURN ParseString(text.value^);
END ReadJSON;

END arJSON.
